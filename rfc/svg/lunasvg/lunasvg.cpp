
#include "lunasvg.h"
#include <fstream>
#include <limits.h>
#include <math.h>
#include <setjmp.h>
#include <string.h>
#include <assert.h>

namespace lunasvg {

    static plutovg_matrix_t to_plutovg_matrix(const Transform& transform);
    static plutovg_fill_rule_t to_plutovg_fill_rule(WindRule winding);
    static plutovg_operator_t to_plutovg_operator(BlendMode mode);
    static plutovg_line_cap_t to_plutovg_line_cap(LineCap cap);
    static plutovg_line_join_t to_plutovg_line_join(LineJoin join);
    static plutovg_spread_method_t to_plutovg_spread_methood(SpreadMethod spread);
    static void to_plutovg_stops(plutovg_gradient_t* gradient, const GradientStops& stops);
    static void to_plutovg_path(plutovg_t* pluto, const Path& path);

    std::shared_ptr<Canvas> Canvas::create(unsigned char* data, unsigned int width, unsigned int height, unsigned int stride)
    {
        return std::shared_ptr<Canvas>(new Canvas(data, static_cast<int>(width), static_cast<int>(height), static_cast<int>(stride)));
    }

    std::shared_ptr<Canvas> Canvas::create(double x, double y, double width, double height)
    {
        if (width <= 0.0 || height <= 0.0)
            return std::shared_ptr<Canvas>(new Canvas(0, 0, 1, 1));

        auto l = static_cast<int>(floor(x));
        auto t = static_cast<int>(floor(y));
        auto r = static_cast<int>(ceil(x + width));
        auto b = static_cast<int>(ceil(y + height));
        return std::shared_ptr<Canvas>(new Canvas(l, t, r - l, b - t));
    }

    std::shared_ptr<Canvas> Canvas::create(const Rect& box)
    {
        return create(box.x, box.y, box.w, box.h);
    }

    Canvas::Canvas(unsigned char* data, int width, int height, int stride)
    {
        surface = plutovg_surface_create_for_data(data, width, height, stride);
        pluto = plutovg_create(surface);
        plutovg_matrix_init_identity(&translation);
        plutovg_rect_init(&rect, 0, 0, width, height);
    }

    Canvas::Canvas(int x, int y, int width, int height)
    {
        surface = plutovg_surface_create(width, height);
        pluto = plutovg_create(surface);
        plutovg_matrix_init_translate(&translation, -x, -y);
        plutovg_rect_init(&rect, x, y, width, height);
    }

    Canvas::~Canvas()
    {
        plutovg_surface_destroy(surface);
        plutovg_destroy(pluto);
    }

    void Canvas::setColor(const Color& color)
    {
        plutovg_set_source_rgba(pluto, color.r, color.g, color.b, color.a);
    }

    void Canvas::setLinearGradient(double x1, double y1, double x2, double y2, const GradientStops& stops, SpreadMethod spread, const Transform& transform)
    {
        auto gradient = plutovg_gradient_create_linear(x1, y1, x2, y2);
        auto matrix = to_plutovg_matrix(transform);
        to_plutovg_stops(gradient, stops);
        plutovg_gradient_set_spread(gradient, to_plutovg_spread_methood(spread));
        plutovg_gradient_set_matrix(gradient, &matrix);
        plutovg_set_source_gradient(pluto, gradient);
        plutovg_gradient_destroy(gradient);
    }

    void Canvas::setRadialGradient(double cx, double cy, double r, double fx, double fy, const GradientStops& stops, SpreadMethod spread, const Transform& transform)
    {
        auto gradient = plutovg_gradient_create_radial(cx, cy, r, fx, fy, 0);
        auto matrix = to_plutovg_matrix(transform);
        to_plutovg_stops(gradient, stops);
        plutovg_gradient_set_spread(gradient, to_plutovg_spread_methood(spread));
        plutovg_gradient_set_matrix(gradient, &matrix);
        plutovg_set_source_gradient(pluto, gradient);
        plutovg_gradient_destroy(gradient);
    }

    void Canvas::setTexture(const Canvas* source, TextureType type, const Transform& transform)
    {
        auto texture = plutovg_texture_create(source->surface);
        auto matrix = to_plutovg_matrix(transform);
        if (type == TextureType::Plain)
            plutovg_texture_set_type(texture, plutovg_texture_type_plain);
        else
            plutovg_texture_set_type(texture, plutovg_texture_type_tiled);

        plutovg_texture_set_matrix(texture, &matrix);
        plutovg_set_source_texture(pluto, texture);
        plutovg_texture_destroy(texture);
    }

    void Canvas::fill(const Path& path, const Transform& transform, WindRule winding, BlendMode mode, double opacity)
    {
        auto matrix = to_plutovg_matrix(transform);
        plutovg_matrix_multiply(&matrix, &matrix, &translation);
        to_plutovg_path(pluto, path);
        plutovg_set_matrix(pluto, &matrix);
        plutovg_set_fill_rule(pluto, to_plutovg_fill_rule(winding));
        plutovg_set_opacity(pluto, opacity);
        plutovg_set_operator(pluto, to_plutovg_operator(mode));
        plutovg_fill(pluto);
    }

    void Canvas::stroke(const Path& path, const Transform& transform, double width, LineCap cap, LineJoin join, double miterlimit, const DashData& dash, BlendMode mode, double opacity)
    {
        auto matrix = to_plutovg_matrix(transform);
        plutovg_matrix_multiply(&matrix, &matrix, &translation);
        to_plutovg_path(pluto, path);
        plutovg_set_matrix(pluto, &matrix);
        plutovg_set_line_width(pluto, width);
        plutovg_set_line_cap(pluto, to_plutovg_line_cap(cap));
        plutovg_set_line_join(pluto, to_plutovg_line_join(join));
        plutovg_set_miter_limit(pluto, miterlimit);
        plutovg_set_dash(pluto, dash.offset, dash.array.data(), static_cast<int>(dash.array.size()));
        plutovg_set_operator(pluto, to_plutovg_operator(mode));
        plutovg_set_opacity(pluto, opacity);
        plutovg_stroke(pluto);
    }

    void Canvas::blend(const Canvas* source, BlendMode mode, double opacity)
    {
        plutovg_set_source_surface(pluto, source->surface, source->rect.x, source->rect.y);
        plutovg_set_operator(pluto, to_plutovg_operator(mode));
        plutovg_set_opacity(pluto, opacity);
        plutovg_set_matrix(pluto, &translation);
        plutovg_paint(pluto);
    }

    void Canvas::mask(const Rect& clip, const Transform& transform)
    {
        auto matrix = to_plutovg_matrix(transform);
        auto path = plutovg_path_create();
        plutovg_path_add_rect(path, clip.x, clip.y, clip.w, clip.h);
        plutovg_path_transform(path, &matrix);
        plutovg_rect(pluto, rect.x, rect.y, rect.w, rect.h);
        plutovg_add_path(pluto, path);
        plutovg_path_destroy(path);

        plutovg_set_source_rgba(pluto, 0, 0, 0, 0);
        plutovg_set_fill_rule(pluto, plutovg_fill_rule_even_odd);
        plutovg_set_operator(pluto, plutovg_operator_src);
        plutovg_set_opacity(pluto, 0.0);
        plutovg_set_matrix(pluto, &translation);
        plutovg_fill(pluto);
    }

    void Canvas::clear(unsigned int value)
    {
        auto r = (value >> 24) & 0xFF;
        auto g = (value >> 16) & 0xFF;
        auto b = (value >> 8) & 0xFF;
        auto a = (value >> 0) & 0xFF;

        plutovg_set_source_rgba(pluto, r / 255.0, g / 255.0, b / 255.0, a / 255.0);
        plutovg_set_opacity(pluto, 1.0);
        plutovg_set_operator(pluto, plutovg_operator_src);
        plutovg_paint(pluto);
    }

    void Canvas::rgba()
    {
        auto width = plutovg_surface_get_width(surface);
        auto height = plutovg_surface_get_height(surface);
        auto stride = plutovg_surface_get_stride(surface);
        auto data = plutovg_surface_get_data(surface);
        for (int y = 0;y < height;y++)
        {
            auto pixels = reinterpret_cast<uint32_t*>(data + stride * y);
            for (int x = 0;x < width;x++)
            {
                auto pixel = pixels[x];
                auto a = (pixel >> 24) & 0xFF;
                if (a == 0)
                    continue;

                auto r = (pixel >> 16) & 0xFF;
                auto g = (pixel >> 8) & 0xFF;
                auto b = (pixel >> 0) & 0xFF;
                if (a != 255)
                {
                    r = (r * 255) / a;
                    g = (g * 255) / a;
                    b = (b * 255) / a;
                }

                pixels[x] = (a << 24) | (b << 16) | (g << 8) | r;
            }
        }
    }

    void Canvas::luminance()
    {
        auto width = plutovg_surface_get_width(surface);
        auto height = plutovg_surface_get_height(surface);
        auto stride = plutovg_surface_get_stride(surface);
        auto data = plutovg_surface_get_data(surface);
        for (int y = 0;y < height;y++)
        {
            auto pixels = reinterpret_cast<uint32_t*>(data + stride * y);
            for (int x = 0;x < width;x++)
            {
                auto pixel = pixels[x];
                auto r = (pixel >> 16) & 0xFF;
                auto g = (pixel >> 8) & 0xFF;
                auto b = (pixel >> 0) & 0xFF;
                auto l = (2 * r + 3 * g + b) / 6;

                pixels[x] = l << 24;
            }
        }
    }

    unsigned int Canvas::width() const
    {
        return plutovg_surface_get_width(surface);
    }

    unsigned int Canvas::height() const
    {
        return plutovg_surface_get_height(surface);
    }

    unsigned int Canvas::stride() const
    {
        return plutovg_surface_get_stride(surface);
    }

    unsigned char* Canvas::data() const
    {
        return plutovg_surface_get_data(surface);
    }

    Rect Canvas::box() const
    {
        return Rect(rect.x, rect.y, rect.w, rect.h);
    }

    plutovg_matrix_t to_plutovg_matrix(const Transform& transform)
    {
        plutovg_matrix_t matrix;
        plutovg_matrix_init(&matrix, transform.m00, transform.m10, transform.m01, transform.m11, transform.m02, transform.m12);
        return matrix;
    }

    plutovg_fill_rule_t to_plutovg_fill_rule(WindRule winding)
    {
        return winding == WindRule::EvenOdd ? plutovg_fill_rule_even_odd : plutovg_fill_rule_non_zero;
    }

    plutovg_operator_t to_plutovg_operator(BlendMode mode)
    {
        return mode == BlendMode::Src ? plutovg_operator_src : mode == BlendMode::Src_Over ? plutovg_operator_src_over : mode == BlendMode::Dst_In ? plutovg_operator_dst_in : plutovg_operator_dst_out;
    }

    plutovg_line_cap_t to_plutovg_line_cap(LineCap cap)
    {
        return cap == LineCap::Butt ? plutovg_line_cap_butt : cap == LineCap::Round ? plutovg_line_cap_round : plutovg_line_cap_square;
    }

    plutovg_line_join_t to_plutovg_line_join(LineJoin join)
    {
        return join == LineJoin::Miter ? plutovg_line_join_miter : join == LineJoin::Round ? plutovg_line_join_round : plutovg_line_join_bevel;
    }

    static plutovg_spread_method_t to_plutovg_spread_methood(SpreadMethod spread)
    {
        return spread == SpreadMethod::Pad ? plutovg_spread_method_pad : spread == SpreadMethod::Reflect ? plutovg_spread_method_reflect : plutovg_spread_method_repeat;
    }

    static void to_plutovg_stops(plutovg_gradient_t* gradient, const GradientStops& stops)
    {
        for (const auto& stop : stops)
        {
            auto offset = std::get<0>(stop);
            auto& color = std::get<1>(stop);
            plutovg_gradient_add_stop_rgba(gradient, offset, color.r, color.g, color.b, color.a);
        }
    }

    void to_plutovg_path(plutovg_t* pluto, const Path& path)
    {
        PathIterator it(path);
        std::array<Point, 3> p;
        while (!it.isDone())
        {
            switch (it.currentSegment(p)) {
            case PathCommand::MoveTo:
                plutovg_move_to(pluto, p[0].x, p[0].y);
                break;
            case PathCommand::LineTo:
                plutovg_line_to(pluto, p[0].x, p[0].y);
                break;
            case PathCommand::CubicTo:
                plutovg_cubic_to(pluto, p[0].x, p[0].y, p[1].x, p[1].y, p[2].x, p[2].y);
                break;
            case PathCommand::Close:
                plutovg_close_path(pluto);
                break;
            }

            it.next();
        }
    }

} // namespace lunasvg


namespace lunasvg {

    ClipPathElement::ClipPathElement()
        : GraphicsElement(ElementId::ClipPath)
    {
    }

    Units ClipPathElement::clipPathUnits() const
    {
        auto& value = get(PropertyId::ClipPathUnits);
        return Parser::parseUnits(value, Units::UserSpaceOnUse);
    }

    std::unique_ptr<LayoutClipPath> ClipPathElement::getClipper(LayoutContext* context) const
    {
        if (context->hasReference(this))
            return nullptr;

        LayoutBreaker layoutBreaker(context, this);
        auto clipper = std::make_unique<LayoutClipPath>();
        clipper->units = clipPathUnits();
        clipper->transform = transform();
        clipper->clipper = context->getClipper(clip_path());
        layoutChildren(context, clipper.get());
        return clipper;
    }

    std::unique_ptr<Node> ClipPathElement::clone() const
    {
        return cloneElement<ClipPathElement>();
    }

} // namespace lunasvg

namespace lunasvg {

    DefsElement::DefsElement()
        : GraphicsElement(ElementId::Defs)
    {
    }

    std::unique_ptr<Node> DefsElement::clone() const
    {
        return cloneElement<DefsElement>();
    }

} // namespace lunasvg


namespace lunasvg {

    void PropertyList::set(PropertyId id, const std::string& value, int specificity)
    {
        auto property = get(id);
        if (property == nullptr)
        {
            Property property{ id, value, specificity };
            m_properties.push_back(std::move(property));
            return;
        }

        if (property->specificity > specificity)
            return;

        property->specificity = specificity;
        property->value = value;
    }

    Property* PropertyList::get(PropertyId id) const
    {
        auto data = m_properties.data();
        auto end = data + m_properties.size();
        while (data < end)
        {
            if (data->id == id)
                return const_cast<Property*>(data);
            ++data;
        }

        return nullptr;
    }

    void PropertyList::add(const Property& property)
    {
        set(property.id, property.value, property.specificity);
    }

    void PropertyList::add(const PropertyList& properties)
    {
        auto it = properties.m_properties.begin();
        auto end = properties.m_properties.end();
        for (;it != end;++it)
            add(*it);
    }

    void Node::layout(LayoutContext*, LayoutContainer*) const
    {
    }

    std::unique_ptr<Node> TextNode::clone() const
    {
        auto node = std::make_unique<TextNode>();
        node->text = text;
        return std::move(node);
    }

    Element::Element(ElementId id)
        : id(id)
    {
    }

    void Element::set(PropertyId id, const std::string& value, int specificity)
    {
        properties.set(id, value, specificity);
    }

    static const std::string EmptyString;

    const std::string& Element::get(PropertyId id) const
    {
        auto property = properties.get(id);
        if (property == nullptr)
            return EmptyString;

        return property->value;
    }

    static const std::string InheritString{ "inherit" };

    const std::string& Element::find(PropertyId id) const
    {
        auto element = this;
        do {
            auto& value = element->get(id);
            if (!value.empty() && value != InheritString)
                return value;
            element = element->parent;
        } while (element);

        return EmptyString;
    }

    bool Element::has(PropertyId id) const
    {
        return properties.get(id);
    }

    Element* Element::previousSibling() const
    {
        if (parent == nullptr)
            return nullptr;

        Element* element = nullptr;
        auto it = parent->children.begin();
        auto end = parent->children.end();
        for (;it != end;++it)
        {
            auto node = it->get();
            if (node->isText())
                continue;

            if (node == this)
                return element;
            element = static_cast<Element*>(node);
        }

        return nullptr;
    }

    Element* Element::nextSibling() const
    {
        if (parent == nullptr)
            return nullptr;

        Element* element = nullptr;
        auto it = parent->children.rbegin();
        auto end = parent->children.rend();
        for (;it != end;++it)
        {
            auto node = it->get();
            if (node->isText())
                continue;

            if (node == this)
                return element;
            element = static_cast<Element*>(node);
        }

        return nullptr;
    }

    Node* Element::addChild(std::unique_ptr<Node> child)
    {
        child->parent = this;
        children.push_back(std::move(child));
        return &*children.back();
    }

    void Element::layoutChildren(LayoutContext* context, LayoutContainer* current) const
    {
        for (auto& child : children)
            child->layout(context, current);
    }

    Rect Element::currentViewport() const
    {
        if (parent == nullptr)
        {
            auto element = static_cast<const SVGElement*>(this);
            if (element->has(PropertyId::ViewBox))
                return element->viewBox();
            return Rect{ 0, 0, 512, 512 };
        }

        if (parent->id == ElementId::Svg)
        {
            auto element = static_cast<SVGElement*>(parent);
            if (element->has(PropertyId::ViewBox))
                return element->viewBox();

            LengthContext lengthContext(element);
            auto _x = lengthContext.valueForLength(element->x(), LengthMode::Width);
            auto _y = lengthContext.valueForLength(element->y(), LengthMode::Height);
            auto _w = lengthContext.valueForLength(element->width(), LengthMode::Width);
            auto _h = lengthContext.valueForLength(element->height(), LengthMode::Height);
            return Rect{ _x, _y, _w, _h };
        }

        return parent->currentViewport();
    }

} // namespace lunasvg

namespace lunasvg {

    GElement::GElement()
        : GraphicsElement(ElementId::G)
    {
    }

    void GElement::layout(LayoutContext* context, LayoutContainer* current) const
    {
        if (isDisplayNone())
            return;

        auto group = std::make_unique<LayoutGroup>();
        group->transform = transform();
        group->opacity = opacity();
        group->masker = context->getMasker(mask());
        group->clipper = context->getClipper(clip_path());
        layoutChildren(context, group.get());
        current->addChildIfNotEmpty(std::move(group));
    }

    std::unique_ptr<Node> GElement::clone() const
    {
        return cloneElement<GElement>();
    }

} // namespace lunasvg


namespace lunasvg {

    GeometryElement::GeometryElement(ElementId id)
        : GraphicsElement(id)
    {
    }

    void GeometryElement::layout(LayoutContext* context, LayoutContainer* current) const
    {
        if (isDisplayNone())
            return;

        auto path = this->path();
        if (path.empty())
            return;

        auto shape = std::make_unique<LayoutShape>();
        shape->path = std::move(path);
        shape->transform = transform();
        shape->fillData = context->fillData(this);
        shape->strokeData = context->strokeData(this);
        shape->markerData = context->markerData(this, shape->path);
        shape->visibility = visibility();
        shape->clipRule = clip_rule();
        shape->masker = context->getMasker(mask());
        shape->clipper = context->getClipper(clip_path());
        current->addChild(std::move(shape));
    }

    PathElement::PathElement()
        : GeometryElement(ElementId::Path)
    {
    }

    Path PathElement::d() const
    {
        auto& value = get(PropertyId::D);
        return Parser::parsePath(value);
    }

    Path PathElement::path() const
    {
        return d();
    }

    std::unique_ptr<Node> PathElement::clone() const
    {
        return cloneElement<PathElement>();
    }

    PolyElement::PolyElement(ElementId id)
        : GeometryElement(id)
    {
    }

    PointList PolyElement::points() const
    {
        auto& value = get(PropertyId::Points);
        return Parser::parsePointList(value);
    }

    PolygonElement::PolygonElement()
        : PolyElement(ElementId::Polygon)
    {
    }

    Path PolygonElement::path() const
    {
        auto points = this->points();
        if (points.empty())
            return Path{};

        Path path;
        path.moveTo(points[0].x, points[0].y);
        for (std::size_t i = 1;i < points.size();i++)
            path.lineTo(points[i].x, points[i].y);

        path.close();
        return path;
    }

    std::unique_ptr<Node> PolygonElement::clone() const
    {
        return cloneElement<PolygonElement>();
    }

    PolylineElement::PolylineElement()
        : PolyElement(ElementId::Polyline)
    {
    }

    Path PolylineElement::path() const
    {
        auto points = this->points();
        if (points.empty())
            return Path{};

        Path path;
        path.moveTo(points[0].x, points[0].y);
        for (std::size_t i = 1;i < points.size();i++)
            path.lineTo(points[i].x, points[i].y);

        return path;
    }

    std::unique_ptr<Node> PolylineElement::clone() const
    {
        return cloneElement<PolylineElement>();
    }

    CircleElement::CircleElement()
        : GeometryElement(ElementId::Circle)
    {
    }

    Length CircleElement::cx() const
    {
        auto& value = get(PropertyId::Cx);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length CircleElement::cy() const
    {
        auto& value = get(PropertyId::Cy);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length CircleElement::r() const
    {
        auto& value = get(PropertyId::R);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::Zero);
    }

    Path CircleElement::path() const
    {
        auto r = this->r();
        if (r.isZero())
            return Path{};

        LengthContext lengthContext(this);
        auto _cx = lengthContext.valueForLength(cx(), LengthMode::Width);
        auto _cy = lengthContext.valueForLength(cy(), LengthMode::Height);
        auto _r = lengthContext.valueForLength(r, LengthMode::Both);

        Path path;
        path.ellipse(_cx, _cy, _r, _r);
        return path;
    }

    std::unique_ptr<Node> CircleElement::clone() const
    {
        return cloneElement<CircleElement>();
    }

    EllipseElement::EllipseElement()
        : GeometryElement(ElementId::Ellipse)
    {
    }

    Length EllipseElement::cx() const
    {
        auto& value = get(PropertyId::Cx);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length EllipseElement::cy() const
    {
        auto& value = get(PropertyId::Cy);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length EllipseElement::rx() const
    {
        auto& value = get(PropertyId::Rx);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::Zero);
    }

    Length EllipseElement::ry() const
    {
        auto& value = get(PropertyId::Ry);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::Zero);
    }

    Path EllipseElement::path() const
    {
        auto rx = this->rx();
        auto ry = this->ry();
        if (rx.isZero() || ry.isZero())
            return Path{};

        LengthContext lengthContext(this);
        auto _cx = lengthContext.valueForLength(cx(), LengthMode::Width);
        auto _cy = lengthContext.valueForLength(cy(), LengthMode::Height);
        auto _rx = lengthContext.valueForLength(rx, LengthMode::Width);
        auto _ry = lengthContext.valueForLength(ry, LengthMode::Height);

        Path path;
        path.ellipse(_cx, _cy, _rx, _ry);
        return path;
    }

    std::unique_ptr<Node> EllipseElement::clone() const
    {
        return cloneElement<EllipseElement>();
    }

    LineElement::LineElement()
        : GeometryElement(ElementId::Line)
    {
    }

    Length LineElement::x1() const
    {
        auto& value = get(PropertyId::X1);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length LineElement::y1() const
    {
        auto& value = get(PropertyId::Y1);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length LineElement::x2() const
    {
        auto& value = get(PropertyId::X2);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length LineElement::y2() const
    {
        auto& value = get(PropertyId::Y2);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Path LineElement::path() const
    {
        LengthContext lengthContext(this);
        auto _x1 = lengthContext.valueForLength(x1(), LengthMode::Width);
        auto _y1 = lengthContext.valueForLength(y1(), LengthMode::Height);
        auto _x2 = lengthContext.valueForLength(x2(), LengthMode::Width);
        auto _y2 = lengthContext.valueForLength(y2(), LengthMode::Height);

        Path path;
        path.moveTo(_x1, _y1);
        path.lineTo(_x2, _y2);
        return path;
    }

    std::unique_ptr<Node> LineElement::clone() const
    {
        return cloneElement<LineElement>();
    }

    RectElement::RectElement()
        : GeometryElement(ElementId::Rect)
    {
    }

    Length RectElement::x() const
    {
        auto& value = get(PropertyId::X);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length RectElement::y() const
    {
        auto& value = get(PropertyId::Y);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length RectElement::rx() const
    {
        auto& value = get(PropertyId::Rx);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::Unknown);
    }

    Length RectElement::ry() const
    {
        auto& value = get(PropertyId::Ry);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::Unknown);
    }

    Length RectElement::width() const
    {
        auto& value = get(PropertyId::Width);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::Zero);
    }

    Length RectElement::height() const
    {
        auto& value = get(PropertyId::Height);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::Zero);
    }

    Path RectElement::path() const
    {
        auto w = this->width();
        auto h = this->height();
        if (w.isZero() || h.isZero())
            return Path{};

        LengthContext lengthContext(this);
        auto _x = lengthContext.valueForLength(x(), LengthMode::Width);
        auto _y = lengthContext.valueForLength(y(), LengthMode::Height);
        auto _w = lengthContext.valueForLength(w, LengthMode::Width);
        auto _h = lengthContext.valueForLength(h, LengthMode::Height);

        auto rx = this->rx();
        auto ry = this->ry();

        auto _rx = lengthContext.valueForLength(rx, LengthMode::Width);
        auto _ry = lengthContext.valueForLength(ry, LengthMode::Height);

        if (!rx.isValid()) _rx = _ry;
        if (!ry.isValid()) _ry = _rx;

        Path path;
        path.rect(_x, _y, _w, _h, _rx, _ry);
        return path;
    }

    std::unique_ptr<Node> RectElement::clone() const
    {
        return cloneElement<RectElement>();
    }

} // namespace lunasvg

namespace lunasvg {

    GraphicsElement::GraphicsElement(ElementId id)
        : StyledElement(id)
    {
    }

    Transform GraphicsElement::transform() const
    {
        auto& value = get(PropertyId::Transform);
        return Parser::parseTransform(value);
    }

} // namespace lunasvg


namespace lunasvg {

    LayoutObject::LayoutObject(LayoutId id)
        : id(id)
    {
    }

    LayoutObject::~LayoutObject()
    {
    }

    void LayoutObject::render(RenderState&) const
    {
    }

    void LayoutObject::apply(RenderState&) const
    {
    }

    Rect LayoutObject::map(const Rect&) const
    {
        return Rect::Invalid;
    }

    LayoutContainer::LayoutContainer(LayoutId id)
        : LayoutObject(id)
    {
    }

    const Rect& LayoutContainer::fillBoundingBox() const
    {
        if (m_fillBoundingBox.valid())
            return m_fillBoundingBox;

        for (const auto& child : children)
        {
            if (child->isHidden())
                continue;
            m_fillBoundingBox.unite(child->map(child->fillBoundingBox()));
        }

        return m_fillBoundingBox;
    }

    const Rect& LayoutContainer::strokeBoundingBox() const
    {
        if (m_strokeBoundingBox.valid())
            return m_strokeBoundingBox;

        for (const auto& child : children)
        {
            if (child->isHidden())
                continue;
            m_strokeBoundingBox.unite(child->map(child->strokeBoundingBox()));
        }

        return m_strokeBoundingBox;
    }

    LayoutObject* LayoutContainer::addChild(std::unique_ptr<LayoutObject> child)
    {
        children.push_back(std::move(child));
        return &*children.back();
    }

    LayoutObject* LayoutContainer::addChildIfNotEmpty(std::unique_ptr<LayoutContainer> child)
    {
        if (child->children.empty())
            return nullptr;

        return addChild(std::move(child));
    }

    void LayoutContainer::renderChildren(RenderState& state) const
    {
        for (const auto& child : children)
            child->render(state);
    }

    LayoutClipPath::LayoutClipPath()
        : LayoutContainer(LayoutId::ClipPath)
    {
    }

    void LayoutClipPath::apply(RenderState& state) const
    {
        RenderState newState(this, RenderMode::Clipping);
        newState.canvas = Canvas::create(state.canvas->box());
        newState.transform = transform * state.transform;
        if (units == Units::ObjectBoundingBox)
        {
            const auto& box = state.objectBoundingBox();
            newState.transform.translate(box.x, box.y);
            newState.transform.scale(box.w, box.h);
        }

        renderChildren(newState);
        if (clipper) clipper->apply(newState);
        state.canvas->blend(newState.canvas.get(), BlendMode::Dst_In, 1.0);
    }

    LayoutMask::LayoutMask()
        : LayoutContainer(LayoutId::Mask)
    {
    }

    void LayoutMask::apply(RenderState& state) const
    {
        Rect rect{ x, y, width, height };
        if (units == Units::ObjectBoundingBox)
        {
            const auto& box = state.objectBoundingBox();
            rect.x = rect.x * box.w + box.x;
            rect.y = rect.y * box.h + box.y;
            rect.w = rect.w * box.w;
            rect.h = rect.h * box.h;
        }

        RenderState newState(this, state.mode());
        newState.canvas = Canvas::create(state.canvas->box());
        newState.transform = state.transform;
        if (contentUnits == Units::ObjectBoundingBox)
        {
            const auto& box = state.objectBoundingBox();
            newState.transform.translate(box.x, box.y);
            newState.transform.scale(box.w, box.h);
        }

        renderChildren(newState);
        if (clipper) clipper->apply(newState);
        if (masker) masker->apply(newState);
        newState.canvas->mask(rect, state.transform);
        newState.canvas->luminance();
        state.canvas->blend(newState.canvas.get(), BlendMode::Dst_In, opacity);
    }

    LayoutSymbol::LayoutSymbol()
        : LayoutContainer(LayoutId::Symbol)
    {
    }

    void LayoutSymbol::render(RenderState& state) const
    {
        BlendInfo info{ clipper, masker, opacity, clip };
        RenderState newState(this, state.mode());
        newState.transform = transform * state.transform;
        newState.beginGroup(state, info);
        renderChildren(newState);
        newState.endGroup(state, info);
    }

    Rect LayoutSymbol::map(const Rect& rect) const
    {
        return transform.map(rect);
    }

    LayoutGroup::LayoutGroup()
        : LayoutContainer(LayoutId::Group)
    {
    }

    void LayoutGroup::render(RenderState& state) const
    {
        BlendInfo info{ clipper, masker, opacity, Rect::Invalid };
        RenderState newState(this, state.mode());
        newState.transform = transform * state.transform;
        newState.beginGroup(state, info);
        renderChildren(newState);
        newState.endGroup(state, info);
    }

    Rect LayoutGroup::map(const Rect& rect) const
    {
        return transform.map(rect);
    }

    LayoutMarker::LayoutMarker()
        : LayoutContainer(LayoutId::Marker)
    {
    }

    Transform LayoutMarker::markerTransform(const Point& origin, double angle, double strokeWidth) const
    {
        auto transform = Transform::translated(origin.x, origin.y);
        if (orient.type() == MarkerOrient::Auto)
            transform.rotate(angle);
        else
            transform.rotate(orient.value());

        if (units == MarkerUnits::StrokeWidth)
            transform.scale(strokeWidth, strokeWidth);

        transform.translate(-refX, -refY);
        return transform;
    }

    Rect LayoutMarker::markerBoundingBox(const Point& origin, double angle, double strokeWidth) const
    {
        auto box = transform.map(strokeBoundingBox());
        auto transform = markerTransform(origin, angle, strokeWidth);
        return transform.map(box);
    }

    void LayoutMarker::renderMarker(RenderState& state, const Point& origin, double angle, double strokeWidth) const
    {
        BlendInfo info{ clipper, masker, opacity, clip };
        RenderState newState(this, state.mode());
        newState.transform = transform * markerTransform(origin, angle, strokeWidth) * state.transform;
        newState.beginGroup(state, info);
        renderChildren(newState);
        newState.endGroup(state, info);
    }

    LayoutPattern::LayoutPattern()
        : LayoutContainer(LayoutId::Pattern)
    {
    }

    void LayoutPattern::apply(RenderState& state) const
    {
        Rect rect{ x, y, width, height };
        if (units == Units::ObjectBoundingBox)
        {
            const auto& box = state.objectBoundingBox();
            rect.x = rect.x * box.w + box.x;
            rect.y = rect.y * box.h + box.y;
            rect.w = rect.w * box.w;
            rect.h = rect.h * box.h;
        }

        auto ctm = state.transform * transform;
        auto scalex = std::sqrt(ctm.m00 * ctm.m00 + ctm.m01 * ctm.m01);
        auto scaley = std::sqrt(ctm.m10 * ctm.m10 + ctm.m11 * ctm.m11);

        auto width = rect.w * scalex;
        auto height = rect.h * scaley;

        RenderState newState(this, RenderMode::Display);
        newState.canvas = Canvas::create(0, 0, width, height);
        newState.transform = Transform::scaled(scalex, scaley);

        if (viewBox.valid())
        {
            auto viewTransform = preserveAspectRatio.getMatrix(rect.w, rect.h, viewBox);
            newState.transform.premultiply(viewTransform);
        }
        else if (contentUnits == Units::ObjectBoundingBox)
        {
            const auto& box = state.objectBoundingBox();
            newState.transform.scale(box.w, box.h);
        }

        auto transform = this->transform;
        transform.translate(rect.x, rect.y);
        transform.scale(1.0 / scalex, 1.0 / scaley);

        renderChildren(newState);
        state.canvas->setTexture(newState.canvas.get(), TextureType::Tiled, transform);
    }

    LayoutGradient::LayoutGradient(LayoutId id)
        : LayoutObject(id)
    {
    }

    LayoutLinearGradient::LayoutLinearGradient()
        : LayoutGradient(LayoutId::LinearGradient)
    {
    }

    void LayoutLinearGradient::apply(RenderState& state) const
    {
        auto transform = this->transform;
        if (units == Units::ObjectBoundingBox)
        {
            const auto& box = state.objectBoundingBox();
            transform *= Transform(box.w, 0, 0, box.h, box.x, box.y);
        }

        state.canvas->setLinearGradient(x1, y1, x2, y2, stops, spreadMethod, transform);
    }

    LayoutRadialGradient::LayoutRadialGradient()
        : LayoutGradient(LayoutId::RadialGradient)
    {
    }

    void LayoutRadialGradient::apply(RenderState& state) const
    {
        auto transform = this->transform;
        if (units == Units::ObjectBoundingBox)
        {
            const auto& box = state.objectBoundingBox();
            transform *= Transform(box.w, 0, 0, box.h, box.x, box.y);
        }

        state.canvas->setRadialGradient(cx, cy, r, fx, fy, stops, spreadMethod, transform);
    }

    LayoutSolidColor::LayoutSolidColor()
        : LayoutObject(LayoutId::SolidColor)
    {
    }

    void LayoutSolidColor::apply(RenderState& state) const
    {
        state.canvas->setColor(color);
    }

    void FillData::fill(RenderState& state, const Path& path) const
    {
        if (opacity == 0.0 || (painter == nullptr && color.isNone()))
            return;

        if (painter == nullptr)
            state.canvas->setColor(color);
        else
            painter->apply(state);

        state.canvas->fill(path, state.transform, fillRule, BlendMode::Src_Over, opacity);
    }

    void StrokeData::stroke(RenderState& state, const Path& path) const
    {
        if (opacity == 0.0 || (painter == nullptr && color.isNone()))
            return;

        if (painter == nullptr)
            state.canvas->setColor(color);
        else
            painter->apply(state);

        state.canvas->stroke(path, state.transform, width, cap, join, miterlimit, dash, BlendMode::Src_Over, opacity);
    }

    static const double sqrt2 = 1.41421356237309504880;

    void StrokeData::inflate(Rect& box) const
    {
        if (opacity == 0.0 || (painter == nullptr && color.isNone()))
            return;

        double caplimit = width / 2.0;
        if (cap == LineCap::Square)
            caplimit *= sqrt2;

        double joinlimit = width / 2.0;
        if (join == LineJoin::Miter)
            joinlimit *= miterlimit;

        double delta = (std::max)(caplimit, joinlimit);
        box.x -= delta;
        box.y -= delta;
        box.w += delta * 2.0;
        box.h += delta * 2.0;
    }

    MarkerPosition::MarkerPosition(const LayoutMarker* marker, const Point& origin, double angle)
        : marker(marker), origin(origin), angle(angle)
    {
    }

    void MarkerData::add(const LayoutMarker* marker, const Point& origin, double angle)
    {
        positions.emplace_back(marker, origin, angle);
    }

    void MarkerData::render(RenderState& state) const
    {
        for (const auto& position : positions)
            position.marker->renderMarker(state, position.origin, position.angle, strokeWidth);
    }

    void MarkerData::inflate(Rect& box) const
    {
        for (const auto& position : positions)
            box.unite(position.marker->markerBoundingBox(position.origin, position.angle, strokeWidth));
    }

    LayoutShape::LayoutShape()
        : LayoutObject(LayoutId::Shape)
    {
    }

    void LayoutShape::render(RenderState& state) const
    {
        if (visibility == Visibility::Hidden)
            return;

        BlendInfo info{ clipper, masker, 1.0, Rect::Invalid };
        RenderState newState(this, state.mode());
        newState.transform = transform * state.transform;
        newState.beginGroup(state, info);

        if (newState.mode() == RenderMode::Display)
        {
            fillData.fill(newState, path);
            strokeData.stroke(newState, path);
            markerData.render(newState);
        }
        else
        {
            newState.canvas->setColor(Color::Black);
            newState.canvas->fill(path, newState.transform, clipRule, BlendMode::Src, 1.0);
        }

        newState.endGroup(state, info);
    }

    Rect LayoutShape::map(const Rect& rect) const
    {
        return transform.map(rect);
    }

    const Rect& LayoutShape::fillBoundingBox() const
    {
        if (m_fillBoundingBox.valid())
            return m_fillBoundingBox;

        m_fillBoundingBox = path.box();
        return m_fillBoundingBox;
    }

    const Rect& LayoutShape::strokeBoundingBox() const
    {
        if (m_strokeBoundingBox.valid())
            return m_strokeBoundingBox;

        m_strokeBoundingBox = fillBoundingBox();
        strokeData.inflate(m_strokeBoundingBox);
        markerData.inflate(m_strokeBoundingBox);
        return m_strokeBoundingBox;
    }

    RenderState::RenderState(const LayoutObject* object, RenderMode mode)
        : m_object(object), m_mode(mode)
    {
    }

    void RenderState::beginGroup(RenderState& state, const BlendInfo& info)
    {
        if (!info.clipper && !info.clip.valid() && (m_mode == RenderMode::Display && !(info.masker || info.opacity < 1.0)))
        {
            canvas = state.canvas;
            return;
        }

        auto box = transform.map(m_object->strokeBoundingBox());
        box.intersect(transform.map(info.clip));
        box.intersect(state.canvas->box());
        canvas = Canvas::create(box);
    }

    void RenderState::endGroup(RenderState& state, const BlendInfo& info)
    {
        if (state.canvas == canvas)
            return;

        if (info.clipper)
            info.clipper->apply(*this);

        if (info.masker && m_mode == RenderMode::Display)
            info.masker->apply(*this);

        if (info.clip.valid())
            canvas->mask(info.clip, transform);

        state.canvas->blend(canvas.get(), BlendMode::Src_Over, m_mode == RenderMode::Display ? info.opacity : 1.0);
    }

    LayoutContext::LayoutContext(const ParseDocument* document, LayoutSymbol* root)
        : m_document(document), m_root(root)
    {
    }

    Element* LayoutContext::getElementById(const std::string& id) const
    {
        return m_document->getElementById(id);
    }

    LayoutObject* LayoutContext::getResourcesById(const std::string& id) const
    {
        auto it = m_resourcesCache.find(id);
        if (it == m_resourcesCache.end())
            return nullptr;

        return it->second;
    }

    LayoutObject* LayoutContext::addToResourcesCache(const std::string& id, std::unique_ptr<LayoutObject> resources)
    {
        if (resources == nullptr)
            return nullptr;

        m_resourcesCache.emplace(id, resources.get());
        return m_root->addChild(std::move(resources));
    }

    LayoutMask* LayoutContext::getMasker(const std::string& id)
    {
        if (id.empty())
            return nullptr;

        auto ref = getResourcesById(id);
        if (ref && ref->id == LayoutId::Mask)
            return static_cast<LayoutMask*>(ref);

        auto element = getElementById(id);
        if (element == nullptr || element->id != ElementId::Mask)
            return nullptr;

        auto masker = static_cast<MaskElement*>(element)->getMasker(this);
        return static_cast<LayoutMask*>(addToResourcesCache(id, std::move(masker)));
    }

    LayoutClipPath* LayoutContext::getClipper(const std::string& id)
    {
        if (id.empty())
            return nullptr;

        auto ref = getResourcesById(id);
        if (ref && ref->id == LayoutId::ClipPath)
            return static_cast<LayoutClipPath*>(ref);

        auto element = getElementById(id);
        if (element == nullptr || element->id != ElementId::ClipPath)
            return nullptr;

        auto clipper = static_cast<ClipPathElement*>(element)->getClipper(this);
        return static_cast<LayoutClipPath*>(addToResourcesCache(id, std::move(clipper)));
    }

    LayoutMarker* LayoutContext::getMarker(const std::string& id)
    {
        if (id.empty())
            return nullptr;

        auto ref = getResourcesById(id);
        if (ref && ref->id == LayoutId::Marker)
            return static_cast<LayoutMarker*>(ref);

        auto element = getElementById(id);
        if (element == nullptr || element->id != ElementId::Marker)
            return nullptr;

        auto marker = static_cast<MarkerElement*>(element)->getMarker(this);
        return static_cast<LayoutMarker*>(addToResourcesCache(id, std::move(marker)));
    }

    LayoutObject* LayoutContext::getPainter(const std::string& id)
    {
        if (id.empty())
            return nullptr;

        auto ref = getResourcesById(id);
        if (ref && ref->isPaint())
            return ref;

        auto element = getElementById(id);
        if (element == nullptr || !element->isPaint())
            return nullptr;

        auto painter = static_cast<PaintElement*>(element)->getPainter(this);
        return addToResourcesCache(id, std::move(painter));
    }

    FillData LayoutContext::fillData(const StyledElement* element)
    {
        auto fill = element->fill();
        if (fill.isNone())
            return FillData{};

        FillData fillData;
        fillData.painter = getPainter(fill.ref());
        fillData.color = fill.color();
        fillData.opacity = element->opacity() * element->fill_opacity();
        fillData.fillRule = element->fill_rule();
        return fillData;
    }

    DashData LayoutContext::dashData(const StyledElement* element)
    {
        auto dasharray = element->stroke_dasharray();
        if (dasharray.empty())
            return DashData{};

        LengthContext lengthContex(element);
        DashArray dashes;
        for (auto& dash : dasharray)
        {
            auto value = lengthContex.valueForLength(dash, LengthMode::Both);
            dashes.push_back(value);
        }

        auto num_dash = dashes.size();
        if (num_dash % 2)
            num_dash *= 2;

        DashData dashData;
        dashData.array.resize(num_dash);
        double sum = 0.0;
        for (std::size_t i = 0;i < num_dash;i++)
        {
            dashData.array[i] = dashes[i % dashes.size()];
            sum += dashData.array[i];
        }

        if (sum == 0.0)
            return DashData{};

        auto offset = lengthContex.valueForLength(element->stroke_dashoffset(), LengthMode::Both);
        dashData.offset = std::fmod(offset, sum);
        if (dashData.offset < 0.0)
            dashData.offset += sum;

        return dashData;
    }

    StrokeData LayoutContext::strokeData(const StyledElement* element)
    {
        auto stroke = element->stroke();
        if (stroke.isNone())
            return StrokeData{};

        LengthContext lengthContex(element);
        StrokeData strokeData;
        strokeData.painter = getPainter(stroke.ref());
        strokeData.color = stroke.color();
        strokeData.opacity = element->opacity() * element->stroke_opacity();
        strokeData.width = lengthContex.valueForLength(element->stroke_width(), LengthMode::Both);
        strokeData.miterlimit = element->stroke_miterlimit();
        strokeData.cap = element->stroke_linecap();
        strokeData.join = element->stroke_linejoin();
        strokeData.dash = dashData(element);
        return strokeData;
    }

    static const double pi = 3.14159265358979323846;

    MarkerData LayoutContext::markerData(const GeometryElement* element, const Path& path)
    {
        auto markerStart = getMarker(element->marker_start());
        auto markerMid = getMarker(element->marker_mid());
        auto markerEnd = getMarker(element->marker_end());

        if (markerStart == nullptr && markerMid == nullptr && markerEnd == nullptr)
            return MarkerData{};

        LengthContext lengthContex(element);
        MarkerData markerData;
        markerData.strokeWidth = lengthContex.valueForLength(element->stroke_width(), LengthMode::Both);

        PathIterator it(path);
        Point origin;
        Point startPoint;
        Point inslopePoints[2];
        Point outslopePoints[2];

        int index = 0;
        std::array<Point, 3> points;
        while (!it.isDone())
        {
            switch (it.currentSegment(points)) {
            case PathCommand::MoveTo:
                startPoint = points[0];
                inslopePoints[0] = origin;
                inslopePoints[1] = points[0];
                origin = points[0];
                break;
            case PathCommand::LineTo:
                inslopePoints[0] = origin;
                inslopePoints[1] = points[0];
                origin = points[0];
                break;
            case PathCommand::CubicTo:
                inslopePoints[0] = points[1];
                inslopePoints[1] = points[2];
                origin = points[2];
                break;
            case PathCommand::Close:
                inslopePoints[0] = origin;
                inslopePoints[1] = points[0];
                origin = startPoint;
                startPoint = Point{};
                break;
            }

            index += 1;
            it.next();

            if (!it.isDone() && (markerStart || markerMid))
            {
                it.currentSegment(points);
                outslopePoints[0] = origin;
                outslopePoints[1] = points[0];

                if (index == 1 && markerStart)
                {
                    Point slope{ outslopePoints[1].x - outslopePoints[0].x, outslopePoints[1].y - outslopePoints[0].y };
                    auto angle = 180.0 * std::atan2(slope.y, slope.x) / pi;

                    markerData.add(markerStart, origin, angle);
                }

                if (index > 1 && markerMid)
                {
                    Point inslope{ inslopePoints[1].x - inslopePoints[0].x, inslopePoints[1].y - inslopePoints[0].y };
                    Point outslope{ outslopePoints[1].x - outslopePoints[0].x, outslopePoints[1].y - outslopePoints[0].y };
                    auto inangle = 180.0 * std::atan2(inslope.y, inslope.x) / pi;
                    auto outangle = 180.0 * std::atan2(outslope.y, outslope.x) / pi;
                    auto angle = (inangle + outangle) * 0.5;

                    markerData.add(markerMid, origin, angle);
                }
            }

            if (it.isDone() && markerEnd)
            {
                Point slope{ inslopePoints[1].x - inslopePoints[0].x, inslopePoints[1].y - inslopePoints[0].y };
                auto angle = 180.0 * std::atan2(slope.y, slope.x) / pi;

                markerData.add(markerEnd, origin, angle);
            }
        }

        return markerData;
    }

    void LayoutContext::addReference(const Element* element)
    {
        m_references.insert(element);
    }

    void LayoutContext::removeReference(const Element* element)
    {
        m_references.erase(element);
    }

    bool LayoutContext::hasReference(const Element* element) const
    {
        return m_references.count(element);
    }

    LayoutBreaker::LayoutBreaker(LayoutContext* context, const Element* element)
        : m_context(context), m_element(element)
    {
        context->addReference(element);
    }

    LayoutBreaker::~LayoutBreaker()
    {
        m_context->removeReference(m_element);
    }

} // namespace lunasvg


namespace lunasvg {

    struct Bitmap::Impl
    {
        Impl(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride);
        Impl(std::uint32_t width, std::uint32_t height);

        std::unique_ptr<std::uint8_t[]> ownData;
        std::uint8_t* data;
        std::uint32_t width;
        std::uint32_t height;
        std::uint32_t stride;
    };

    Bitmap::Impl::Impl(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride)
        : data(data), width(width), height(height), stride(stride)
    {
    }

    Bitmap::Impl::Impl(std::uint32_t width, std::uint32_t height)
        : ownData(new std::uint8_t[width * height * 4]), data(nullptr), width(width), height(height), stride(width * 4)
    {
    }

    Bitmap::Bitmap()
    {
    }

    Bitmap::Bitmap(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride)
        : m_impl(new Impl(data, width, height, stride))
    {
    }

    Bitmap::Bitmap(std::uint32_t width, std::uint32_t height)
        : m_impl(new Impl(width, height))
    {
    }

    void Bitmap::reset(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride)
    {
        m_impl.reset(new Impl(data, width, height, stride));
    }

    void Bitmap::reset(std::uint32_t width, std::uint32_t height)
    {
        m_impl.reset(new Impl(width, height));
    }

    std::uint8_t* Bitmap::data() const
    {
        return m_impl ? m_impl->data ? m_impl->data : m_impl->ownData.get() : nullptr;
    }

    std::uint32_t Bitmap::width() const
    {
        return m_impl ? m_impl->width : 0;
    }

    std::uint32_t Bitmap::height() const
    {
        return m_impl ? m_impl->height : 0;
    }

    std::uint32_t Bitmap::stride() const
    {
        return m_impl ? m_impl->stride : 0;
    }

    bool Bitmap::valid() const
    {
        return !!m_impl;
    }

    std::unique_ptr<Document> Document::loadFromFile(const std::string& filename)
    {
        std::ifstream fs;
        fs.open(filename);
        if (!fs.is_open())
            return nullptr;

        std::string content;
        std::getline(fs, content, '\0');
        fs.close();

        return loadFromData(content);
    }

    std::unique_ptr<Document> Document::loadFromData(const std::string& string)
    {
        return loadFromData(string.data(), string.size());
    }

    std::unique_ptr<Document> Document::loadFromData(const char* data, std::size_t size)
    {
        ParseDocument parser;
        if (!parser.parse(data, size))
            return nullptr;

        auto root = parser.layout();
        if (!root || root->children.empty())
            return nullptr;

        std::unique_ptr<Document> document(new Document);
        document->root = std::move(root);
        return document;
    }

    std::unique_ptr<Document> Document::loadFromData(const char* data)
    {
        return loadFromData(data, std::strlen(data));
    }

    Document* Document::rotate(double angle)
    {
        root->transform.rotate(angle);
        return this;
    }

    Document* Document::rotate(double angle, double cx, double cy)
    {
        root->transform.rotate(angle, cx, cy);
        return this;
    }

    Document* Document::scale(double sx, double sy)
    {
        root->transform.scale(sx, sy);
        return this;
    }

    Document* Document::shear(double shx, double shy)
    {
        root->transform.shear(shx, shy);
        return this;
    }

    Document* Document::translate(double tx, double ty)
    {
        root->transform.translate(tx, ty);
        return this;
    }

    Document* Document::transform(double a, double b, double c, double d, double e, double f)
    {
        root->transform.transform(a, b, c, d, e, f);
        return this;
    }

    Document* Document::identity()
    {
        root->transform.identity();
        return this;
    }

    Matrix Document::matrix() const
    {
        Matrix matrix;
        matrix.a = root->transform.m00;
        matrix.b = root->transform.m10;
        matrix.c = root->transform.m01;
        matrix.d = root->transform.m11;
        matrix.e = root->transform.m02;
        matrix.f = root->transform.m12;
        return matrix;
    }

    Box Document::box() const
    {
        auto box = root->map(root->strokeBoundingBox());
        return Box{ box.x, box.y, box.w, box.h };
    }

    double Document::width() const
    {
        return root->width;
    }

    double Document::height() const
    {
        return root->height;
    }

    void Document::render(Bitmap bitmap, const Matrix& matrix, std::uint32_t backgroundColor) const
    {
        RenderState state(nullptr, RenderMode::Display);
        state.canvas = Canvas::create(bitmap.data(), bitmap.width(), bitmap.height(), bitmap.stride());
        state.transform = Transform(matrix.a, matrix.b, matrix.c, matrix.d, matrix.e, matrix.f);
        state.canvas->clear(backgroundColor);
        root->render(state);

        // default format is premultiplied gdi 0xaarrggbb or gdiplus PixelFormat32bppPARGB or direct2d {DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_PREMULTIPLIED}

        //state.canvas->rgba();
    }

    Bitmap Document::renderToBitmap(std::uint32_t width, std::uint32_t height, std::uint32_t backgroundColor) const
    {
        if (root->width == 0.0 || root->height == 0.0)
            return Bitmap{};

        if (width == 0 && height == 0)
        {
            width = static_cast<std::uint32_t>(std::ceil(root->width));
            height = static_cast<std::uint32_t>(std::ceil(root->height));
        }
        else if (width != 0 && height == 0)
        {
            height = static_cast<std::uint32_t>(std::ceil(width * root->height / root->width));
        }
        else if (height != 0 && width == 0)
        {
            width = static_cast<std::uint32_t>(std::ceil(height * root->width / root->height));
        }

        Bitmap bitmap{ width, height };
        Matrix matrix{ width / root->width, 0, 0, height / root->height, 0, 0 };
        render(bitmap, matrix, backgroundColor);
        return bitmap;
    }

    Document::Document()
    {
    }

    Document::~Document()
    {
    }

} // namespace lunasvg


namespace lunasvg {

    MarkerElement::MarkerElement()
        : StyledElement(ElementId::Marker)
    {
    }

    Length MarkerElement::refX() const
    {
        auto& value = get(PropertyId::RefX);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length MarkerElement::refY() const
    {
        auto& value = get(PropertyId::RefY);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length MarkerElement::markerWidth() const
    {
        auto& value = get(PropertyId::MarkerWidth);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::ThreePercent);
    }

    Length MarkerElement::markerHeight() const
    {
        auto& value = get(PropertyId::MarkerHeight);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::ThreePercent);
    }

    Angle MarkerElement::orient() const
    {
        auto& value = get(PropertyId::Orient);
        return Parser::parseAngle(value);
    }

    MarkerUnits MarkerElement::markerUnits() const
    {
        auto& value = get(PropertyId::MarkerUnits);
        return Parser::parseMarkerUnits(value);
    }

    Rect MarkerElement::viewBox() const
    {
        auto& value = get(PropertyId::ViewBox);
        return Parser::parseViewBox(value);
    }

    PreserveAspectRatio MarkerElement::preserveAspectRatio() const
    {
        auto& value = get(PropertyId::PreserveAspectRatio);
        return Parser::parsePreserveAspectRatio(value);
    }

    std::unique_ptr<LayoutMarker> MarkerElement::getMarker(LayoutContext* context) const
    {
        auto markerWidth = this->markerWidth();
        auto markerHeight = this->markerHeight();
        if (markerWidth.isZero() || markerHeight.isZero() || context->hasReference(this))
            return nullptr;

        LengthContext lengthContext(this);
        auto _refX = lengthContext.valueForLength(refX(), LengthMode::Width);
        auto _refY = lengthContext.valueForLength(refY(), LengthMode::Height);
        auto _markerWidth = lengthContext.valueForLength(markerWidth, LengthMode::Width);
        auto _markerHeight = lengthContext.valueForLength(markerHeight, LengthMode::Height);

        auto viewBox = this->viewBox();
        auto preserveAspectRatio = this->preserveAspectRatio();
        auto viewTransform = preserveAspectRatio.getMatrix(_markerWidth, _markerHeight, viewBox);
        viewTransform.map(_refX, _refY, &_refX, &_refY);

        LayoutBreaker layoutBreaker(context, this);
        auto marker = std::make_unique<LayoutMarker>();
        marker->refX = _refX;
        marker->refY = _refY;
        marker->transform = viewTransform;
        marker->orient = orient();
        marker->units = markerUnits();
        marker->clip = isOverflowHidden() ? preserveAspectRatio.getClip(_markerWidth, _markerHeight, viewBox) : Rect::Invalid;
        marker->opacity = opacity();
        marker->masker = context->getMasker(mask());
        marker->clipper = context->getClipper(clip_path());
        layoutChildren(context, marker.get());
        return marker;
    }

    std::unique_ptr<Node> MarkerElement::clone() const
    {
        return cloneElement<MarkerElement>();
    }

} // namespace lunasvg


namespace lunasvg {

    MaskElement::MaskElement()
        : StyledElement(ElementId::Mask)
    {
    }

    Length MaskElement::x() const
    {
        auto& value = get(PropertyId::X);
        return Parser::parseLength(value, AllowNegativeLengths, Length::MinusTenPercent);
    }

    Length MaskElement::y() const
    {
        auto& value = get(PropertyId::Y);
        return Parser::parseLength(value, AllowNegativeLengths, Length::MinusTenPercent);
    }

    Length MaskElement::width() const
    {
        auto& value = get(PropertyId::Width);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::OneTwentyPercent);
    }

    Length MaskElement::height() const
    {
        auto& value = get(PropertyId::Height);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::OneTwentyPercent);
    }

    Units MaskElement::maskUnits() const
    {
        auto& value = get(PropertyId::MaskUnits);
        return Parser::parseUnits(value, Units::ObjectBoundingBox);
    }

    Units MaskElement::maskContentUnits() const
    {
        auto& value = get(PropertyId::MaskContentUnits);
        return Parser::parseUnits(value, Units::UserSpaceOnUse);
    }

    std::unique_ptr<LayoutMask> MaskElement::getMasker(LayoutContext* context) const
    {
        auto w = this->width();
        auto h = this->height();
        if (w.isZero() || h.isZero() || context->hasReference(this))
            return nullptr;

        LayoutBreaker layoutBreaker(context, this);
        auto masker = std::make_unique<LayoutMask>();
        masker->units = maskUnits();
        masker->contentUnits = maskContentUnits();
        masker->opacity = opacity();
        masker->clipper = context->getClipper(clip_path());
        masker->masker = context->getMasker(mask());

        LengthContext lengthContext(this, maskUnits());
        masker->x = lengthContext.valueForLength(x(), LengthMode::Width);
        masker->y = lengthContext.valueForLength(y(), LengthMode::Height);
        masker->width = lengthContext.valueForLength(w, LengthMode::Width);
        masker->height = lengthContext.valueForLength(h, LengthMode::Height);
        layoutChildren(context, masker.get());
        return masker;
    }

    std::unique_ptr<Node> MaskElement::clone() const
    {
        return cloneElement<MaskElement>();
    }

} // namespace lunasvg



namespace lunasvg {

    PaintElement::PaintElement(ElementId id)
        : StyledElement(id)
    {
    }

    GradientElement::GradientElement(ElementId id)
        : PaintElement(id)
    {
    }

    Transform GradientElement::gradientTransform() const
    {
        auto& value = get(PropertyId::GradientTransform);
        return Parser::parseTransform(value);
    }

    SpreadMethod GradientElement::spreadMethod() const
    {
        auto& value = get(PropertyId::SpreadMethod);
        return Parser::parseSpreadMethod(value);
    }

    Units GradientElement::gradientUnits() const
    {
        auto& value = get(PropertyId::GradientUnits);
        return Parser::parseUnits(value, Units::ObjectBoundingBox);
    }

    std::string GradientElement::href() const
    {
        auto& value = get(PropertyId::Href);
        return Parser::parseHref(value);
    }

    GradientStops GradientElement::buildGradientStops() const
    {
        GradientStops gradientStops;
        double prevOffset = 0.0;
        for (auto& child : children)
        {
            if (child->isText())
                continue;
            auto element = static_cast<Element*>(child.get());
            if (element->id != ElementId::Stop)
                continue;
            auto stop = static_cast<StopElement*>(element);
            auto offset = (std::max)(prevOffset, stop->offset());
            prevOffset = offset;
            gradientStops.emplace_back(offset, stop->stopColorWithOpacity());
        }

        return gradientStops;
    }

    LinearGradientElement::LinearGradientElement()
        : GradientElement(ElementId::LinearGradient)
    {
    }

    Length LinearGradientElement::x1() const
    {
        auto& value = get(PropertyId::X1);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length LinearGradientElement::y1() const
    {
        auto& value = get(PropertyId::Y1);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length LinearGradientElement::x2() const
    {
        auto& value = get(PropertyId::X2);
        return Parser::parseLength(value, AllowNegativeLengths, Length::HundredPercent);
    }

    Length LinearGradientElement::y2() const
    {
        auto& value = get(PropertyId::Y2);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    std::unique_ptr<LayoutObject> LinearGradientElement::getPainter(LayoutContext* context) const
    {
        LinearGradientAttributes attributes;
        std::set<const GradientElement*> processedGradients;
        const GradientElement* current = this;

        while (true)
        {
            if (!attributes.hasGradientTransform() && current->has(PropertyId::GradientTransform))
                attributes.setGradientTransform(current->gradientTransform());
            if (!attributes.hasSpreadMethod() && current->has(PropertyId::SpreadMethod))
                attributes.setSpreadMethod(current->spreadMethod());
            if (!attributes.hasGradientUnits() && current->has(PropertyId::GradientUnits))
                attributes.setGradientUnits(current->gradientUnits());
            if (!attributes.hasGradientStops())
                attributes.setGradientStops(current->buildGradientStops());

            if (current->id == ElementId::LinearGradient)
            {
                auto element = static_cast<const LinearGradientElement*>(current);
                if (!attributes.hasX1() && element->has(PropertyId::X1))
                    attributes.setX1(element->x1());
                if (!attributes.hasY1() && element->has(PropertyId::Y1))
                    attributes.setY1(element->y1());
                if (!attributes.hasX2() && element->has(PropertyId::X2))
                    attributes.setX2(element->x2());
                if (!attributes.hasY2() && element->has(PropertyId::Y2))
                    attributes.setY2(element->y2());
            }

            auto ref = context->getElementById(current->href());
            if (!ref || !(ref->id == ElementId::LinearGradient || ref->id == ElementId::RadialGradient))
                break;

            processedGradients.insert(current);
            current = static_cast<const GradientElement*>(ref);
            if (processedGradients.find(current) != processedGradients.end())
                break;
        }

        auto& stops = attributes.gradientStops();
        if (stops.empty())
            return nullptr;

        LengthContext lengthContext(this, attributes.gradientUnits());
        auto x1 = lengthContext.valueForLength(attributes.x1(), LengthMode::Width);
        auto y1 = lengthContext.valueForLength(attributes.y1(), LengthMode::Height);
        auto x2 = lengthContext.valueForLength(attributes.x2(), LengthMode::Width);
        auto y2 = lengthContext.valueForLength(attributes.y2(), LengthMode::Height);
        if ((x1 == x2 && y1 == y2) || stops.size() == 1)
        {
            auto solid = std::make_unique<LayoutSolidColor>();
            solid->color = std::get<1>(stops.back());
            return std::move(solid);
        }

        auto gradient = std::make_unique<LayoutLinearGradient>();
        gradient->transform = attributes.gradientTransform();
        gradient->spreadMethod = attributes.spreadMethod();
        gradient->units = attributes.gradientUnits();
        gradient->stops = attributes.gradientStops();
        gradient->x1 = x1;
        gradient->y1 = y1;
        gradient->x2 = x2;
        gradient->y2 = y2;
        return std::move(gradient);
    }

    std::unique_ptr<Node> LinearGradientElement::clone() const
    {
        return cloneElement<LinearGradientElement>();
    }

    RadialGradientElement::RadialGradientElement()
        : GradientElement(ElementId::RadialGradient)
    {
    }

    Length RadialGradientElement::cx() const
    {
        auto& value = get(PropertyId::Cx);
        return Parser::parseLength(value, AllowNegativeLengths, Length::FiftyPercent);
    }

    Length RadialGradientElement::cy() const
    {
        auto& value = get(PropertyId::Cy);
        return Parser::parseLength(value, AllowNegativeLengths, Length::FiftyPercent);
    }

    Length RadialGradientElement::r() const
    {
        auto& value = get(PropertyId::R);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::FiftyPercent);
    }

    Length RadialGradientElement::fx() const
    {
        auto& value = get(PropertyId::Fx);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length RadialGradientElement::fy() const
    {
        auto& value = get(PropertyId::Fy);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    std::unique_ptr<LayoutObject> RadialGradientElement::getPainter(LayoutContext* context) const
    {
        RadialGradientAttributes attributes;
        std::set<const GradientElement*> processedGradients;
        const GradientElement* current = this;

        while (true)
        {
            if (!attributes.hasGradientTransform() && current->has(PropertyId::GradientTransform))
                attributes.setGradientTransform(current->gradientTransform());
            if (!attributes.hasSpreadMethod() && current->has(PropertyId::SpreadMethod))
                attributes.setSpreadMethod(current->spreadMethod());
            if (!attributes.hasGradientUnits() && current->has(PropertyId::GradientUnits))
                attributes.setGradientUnits(current->gradientUnits());
            if (!attributes.hasGradientStops())
                attributes.setGradientStops(current->buildGradientStops());

            if (current->id == ElementId::RadialGradient)
            {
                auto element = static_cast<const RadialGradientElement*>(current);
                if (!attributes.hasCx() && element->has(PropertyId::Cx))
                    attributes.setCx(element->cx());
                if (!attributes.hasCy() && element->has(PropertyId::Cy))
                    attributes.setCy(element->cy());
                if (!attributes.hasR() && element->has(PropertyId::R))
                    attributes.setR(element->r());
                if (!attributes.hasFx() && element->has(PropertyId::Fx))
                    attributes.setFx(element->fx());
                if (!attributes.hasFy() && element->has(PropertyId::Fy))
                    attributes.setFy(element->fy());
            }

            auto ref = context->getElementById(current->href());
            if (!ref || !(ref->id == ElementId::LinearGradient || ref->id == ElementId::RadialGradient))
                break;

            processedGradients.insert(current);
            current = static_cast<const GradientElement*>(ref);
            if (processedGradients.find(current) != processedGradients.end())
                break;
        }

        if (!attributes.hasFx())
            attributes.setFx(attributes.cx());
        if (!attributes.hasFy())
            attributes.setFy(attributes.cy());

        auto& stops = attributes.gradientStops();
        if (stops.empty())
            return nullptr;

        auto& r = attributes.r();
        if (r.isZero() || stops.size() == 1)
        {
            auto solid = std::make_unique<LayoutSolidColor>();
            solid->color = std::get<1>(stops.back());
            return std::move(solid);
        }

        auto gradient = std::make_unique<LayoutRadialGradient>();
        gradient->transform = attributes.gradientTransform();
        gradient->spreadMethod = attributes.spreadMethod();
        gradient->units = attributes.gradientUnits();
        gradient->stops = attributes.gradientStops();

        LengthContext lengthContext(this, attributes.gradientUnits());
        gradient->cx = lengthContext.valueForLength(attributes.cx(), LengthMode::Width);
        gradient->cy = lengthContext.valueForLength(attributes.cy(), LengthMode::Height);
        gradient->r = lengthContext.valueForLength(attributes.r(), LengthMode::Both);
        gradient->fx = lengthContext.valueForLength(attributes.fx(), LengthMode::Width);
        gradient->fy = lengthContext.valueForLength(attributes.fy(), LengthMode::Height);
        return std::move(gradient);
    }

    std::unique_ptr<Node> RadialGradientElement::clone() const
    {
        return cloneElement<RadialGradientElement>();
    }

    PatternElement::PatternElement()
        : PaintElement(ElementId::Pattern)
    {
    }

    Length PatternElement::x() const
    {
        auto& value = get(PropertyId::X);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length PatternElement::y() const
    {
        auto& value = get(PropertyId::Y);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length PatternElement::width() const
    {
        auto& value = get(PropertyId::Width);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::Zero);
    }

    Length PatternElement::height() const
    {
        auto& value = get(PropertyId::Height);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::Zero);
    }

    Transform PatternElement::patternTransform() const
    {
        auto& value = get(PropertyId::PatternTransform);
        return Parser::parseTransform(value);
    }

    Units PatternElement::patternUnits() const
    {
        auto& value = get(PropertyId::PatternUnits);
        return Parser::parseUnits(value, Units::ObjectBoundingBox);
    }

    Units PatternElement::patternContentUnits() const
    {
        auto& value = get(PropertyId::PatternContentUnits);
        return Parser::parseUnits(value, Units::UserSpaceOnUse);
    }

    Rect PatternElement::viewBox() const
    {
        auto& value = get(PropertyId::ViewBox);
        return Parser::parseViewBox(value);
    }

    PreserveAspectRatio PatternElement::preserveAspectRatio() const
    {
        auto& value = get(PropertyId::PreserveAspectRatio);
        return Parser::parsePreserveAspectRatio(value);
    }

    std::string PatternElement::href() const
    {
        auto& value = get(PropertyId::Href);
        return Parser::parseHref(value);
    }

    std::unique_ptr<LayoutObject> PatternElement::getPainter(LayoutContext* context) const
    {
        if (context->hasReference(this))
            return nullptr;

        PatternAttributes attributes;
        std::set<const PatternElement*> processedPatterns;
        const PatternElement* current = this;

        while (true)
        {
            if (!attributes.hasX() && current->has(PropertyId::X))
                attributes.setX(current->x());
            if (!attributes.hasY() && current->has(PropertyId::Y))
                attributes.setY(current->y());
            if (!attributes.hasWidth() && current->has(PropertyId::Width))
                attributes.setWidth(current->width());
            if (!attributes.hasHeight() && current->has(PropertyId::Height))
                attributes.setHeight(current->height());
            if (!attributes.hasPatternTransform() && current->has(PropertyId::PatternTransform))
                attributes.setPatternTransform(current->patternTransform());
            if (!attributes.hasPatternUnits() && current->has(PropertyId::PatternUnits))
                attributes.setPatternUnits(current->patternUnits());
            if (!attributes.hasPatternContentUnits() && current->has(PropertyId::PatternContentUnits))
                attributes.setPatternContentUnits(current->patternContentUnits());
            if (!attributes.hasViewBox() && current->has(PropertyId::ViewBox))
                attributes.setViewBox(current->viewBox());
            if (!attributes.hasPreserveAspectRatio() && current->has(PropertyId::PreserveAspectRatio))
                attributes.setPreserveAspectRatio(current->preserveAspectRatio());
            if (!attributes.hasPatternContentElement() && current->children.size())
                attributes.setPatternContentElement(current);

            auto ref = context->getElementById(current->href());
            if (!ref || ref->id != ElementId::Pattern)
                break;

            processedPatterns.insert(current);
            current = static_cast<const PatternElement*>(ref);
            if (processedPatterns.find(current) != processedPatterns.end())
                break;
        }

        auto& width = attributes.width();
        auto& height = attributes.height();
        auto element = attributes.patternContentElement();
        if (element == nullptr || width.isZero() || height.isZero())
            return nullptr;

        LayoutBreaker layoutBreaker(context, this);
        auto pattern = std::make_unique<LayoutPattern>();
        pattern->transform = attributes.patternTransform();
        pattern->units = attributes.patternUnits();
        pattern->contentUnits = attributes.patternContentUnits();
        pattern->viewBox = attributes.viewBox();
        pattern->preserveAspectRatio = attributes.preserveAspectRatio();

        LengthContext lengthContext(this, attributes.patternUnits());
        pattern->x = lengthContext.valueForLength(attributes.x(), LengthMode::Width);
        pattern->y = lengthContext.valueForLength(attributes.y(), LengthMode::Height);
        pattern->width = lengthContext.valueForLength(attributes.width(), LengthMode::Width);
        pattern->height = lengthContext.valueForLength(attributes.height(), LengthMode::Height);
        element->layoutChildren(context, pattern.get());
        return std::move(pattern);
    }

    std::unique_ptr<Node> PatternElement::clone() const
    {
        return cloneElement<PatternElement>();
    }

    SolidColorElement::SolidColorElement()
        : PaintElement(ElementId::SolidColor)
    {
    }

    std::unique_ptr<LayoutObject> SolidColorElement::getPainter(LayoutContext*) const
    {
        auto solid = std::make_unique<LayoutSolidColor>();
        solid->color = solid_color();
        solid->color.a = solid_opacity();
        return std::move(solid);
    }

    std::unique_ptr<Node> SolidColorElement::clone() const
    {
        return cloneElement<SolidColorElement>();
    }

} // namespace lunasvg


namespace lunasvg {

    Length Parser::parseLength(const std::string& string, LengthNegativeValuesMode mode, const Length& defaultValue)
    {
        if (string.empty())
            return defaultValue;

        auto ptr = string.data();
        auto end = ptr + string.size();

        double value;
        LengthUnits units;
        if (!parseLength(ptr, end, value, units, mode))
            return defaultValue;

        return Length{ value, units };
    }

    LengthList Parser::parseLengthList(const std::string& string, LengthNegativeValuesMode mode)
    {
        if (string.empty())
            return LengthList{};

        auto ptr = string.data();
        auto end = ptr + string.size();

        double value;
        LengthUnits units;

        LengthList values;
        while (ptr < end)
        {
            if (!parseLength(ptr, end, value, units, mode))
                break;
            values.emplace_back(value, units);
            Utils::skipWsComma(ptr, end);
        }

        return values;
    }

    double Parser::parseNumber(const std::string& string, double defaultValue)
    {
        if (string.empty())
            return defaultValue;

        auto ptr = string.data();
        auto end = ptr + string.size();

        double value;
        if (!Utils::parseNumber(ptr, end, value))
            return defaultValue;

        return value;
    }

    double Parser::parseNumberPercentage(const std::string& string, double defaultValue)
    {
        if (string.empty())
            return defaultValue;

        auto ptr = string.data();
        auto end = ptr + string.size();

        double value;
        if (!Utils::parseNumber(ptr, end, value))
            return defaultValue;

        if (Utils::skipDesc(ptr, end, '%'))
            value /= 100.0;
        return value < 0.0 ? 0.0 : value > 1.0 ? 1.0 : value;
    }

    PointList Parser::parsePointList(const std::string& string)
    {
        if (string.empty())
            return PointList{};

        auto ptr = string.data();
        auto end = ptr + string.size();

        double x;
        double y;

        PointList values;
        while (ptr < end)
        {
            if (!Utils::parseNumber(ptr, end, x)
                || !Utils::skipWsComma(ptr, end)
                || !Utils::parseNumber(ptr, end, y))
                break;
            values.emplace_back(x, y);
            Utils::skipWsComma(ptr, end);
        }

        return values;
    }

    Transform Parser::parseTransform(const std::string& string)
    {
        if (string.empty())
            return Transform{};

        auto ptr = string.data();
        auto end = ptr + string.size();

        TransformType type;
        double values[6];
        int count;

        Transform transform;
        while (ptr < end)
        {
            if (!parseTransform(ptr, end, type, values, count))
                break;
            Utils::skipWsComma(ptr, end);
            switch (type) {
            case TransformType::Matrix:
                transform.transform(values[0], values[1], values[2], values[3], values[4], values[5]);
                break;
            case TransformType::Rotate:
                if (count == 1)
                    transform.rotate(values[0], 0, 0);
                else
                    transform.rotate(values[0], values[1], values[2]);
                break;
            case TransformType::Scale:
                if (count == 1)
                    transform.scale(values[0], values[0]);
                else
                    transform.scale(values[0], values[1]);
                break;
            case TransformType::SkewX:
                transform.shear(values[0], 0);
                break;
            case TransformType::SkewY:
                transform.shear(0, values[0]);
                break;
            case TransformType::Translate:
                if (count == 1)
                    transform.translate(values[0], 0);
                else
                    transform.translate(values[0], values[1]);
                break;
            }
        }

        return transform;
    }

    Path Parser::parsePath(const std::string& string)
    {
        auto ptr = string.data();
        auto end = ptr + string.size();
        if (ptr >= end || !(*ptr == 'M' || *ptr == 'm'))
            return Path{};

        auto command = *ptr++;
        double c[6];
        bool f[2];

        Point startPoint;
        Point currentPoint;
        Point controlPoint;

        Path path;
        while (true)
        {
            Utils::skipWs(ptr, end);
            switch (command) {
            case 'M':
            case 'm':
                if (!parseNumberList(ptr, end, c, 2))
                    return path;

                if (command == 'm')
                {
                    c[0] += currentPoint.x;
                    c[1] += currentPoint.y;
                }

                path.moveTo(c[0], c[1]);
                startPoint.x = currentPoint.x = controlPoint.x = c[0];
                startPoint.y = currentPoint.y = controlPoint.y = c[1];
                command = command == 'm' ? 'l' : 'L';
                break;
            case 'L':
            case 'l':
                if (!parseNumberList(ptr, end, c, 2))
                    return path;

                if (command == 'l')
                {
                    c[0] += currentPoint.x;
                    c[1] += currentPoint.y;
                }

                path.lineTo(c[0], c[1]);
                currentPoint.x = controlPoint.x = c[0];
                currentPoint.y = controlPoint.y = c[1];
                break;
            case 'Q':
            case 'q':
                if (!parseNumberList(ptr, end, c, 4))
                    return path;

                if (command == 'q')
                {
                    c[0] += currentPoint.x;
                    c[1] += currentPoint.y;
                    c[2] += currentPoint.x;
                    c[3] += currentPoint.y;
                }

                path.quadTo(currentPoint.x, currentPoint.y, c[0], c[1], c[2], c[3]);
                controlPoint.x = c[0];
                controlPoint.y = c[1];
                currentPoint.x = c[2];
                currentPoint.y = c[3];
                break;
            case 'C':
            case 'c':
                if (!parseNumberList(ptr, end, c, 6))
                    return path;

                if (command == 'c')
                {
                    c[0] += currentPoint.x;
                    c[1] += currentPoint.y;
                    c[2] += currentPoint.x;
                    c[3] += currentPoint.y;
                    c[4] += currentPoint.x;
                    c[5] += currentPoint.y;
                }

                path.cubicTo(c[0], c[1], c[2], c[3], c[4], c[5]);
                controlPoint.x = c[2];
                controlPoint.y = c[3];
                currentPoint.x = c[4];
                currentPoint.y = c[5];
                break;
            case 'T':
            case 't':
                c[0] = 2 * currentPoint.x - controlPoint.x;
                c[1] = 2 * currentPoint.y - controlPoint.y;
                if (!parseNumberList(ptr, end, c + 2, 2))
                    return path;

                if (command == 't')
                {
                    c[2] += currentPoint.x;
                    c[3] += currentPoint.y;
                }

                path.quadTo(currentPoint.x, currentPoint.y, c[0], c[1], c[2], c[3]);
                controlPoint.x = c[0];
                controlPoint.y = c[1];
                currentPoint.x = c[2];
                currentPoint.y = c[3];
                break;
            case 'S':
            case 's':
                c[0] = 2 * currentPoint.x - controlPoint.x;
                c[1] = 2 * currentPoint.y - controlPoint.y;
                if (!parseNumberList(ptr, end, c + 2, 4))
                    return path;

                if (command == 's')
                {
                    c[2] += currentPoint.x;
                    c[3] += currentPoint.y;
                    c[4] += currentPoint.x;
                    c[5] += currentPoint.y;
                }

                path.cubicTo(c[0], c[1], c[2], c[3], c[4], c[5]);
                controlPoint.x = c[2];
                controlPoint.y = c[3];
                currentPoint.x = c[4];
                currentPoint.y = c[5];
                break;
            case 'H':
            case 'h':
                if (!parseNumberList(ptr, end, c, 1))
                    return path;

                if (command == 'h')
                    c[0] += currentPoint.x;

                path.lineTo(c[0], currentPoint.y);
                currentPoint.x = controlPoint.x = c[0];
                break;
            case 'V':
            case 'v':
                if (!parseNumberList(ptr, end, c + 1, 1))
                    return path;

                if (command == 'v')
                    c[1] += currentPoint.y;

                path.lineTo(currentPoint.x, c[1]);
                currentPoint.y = controlPoint.y = c[1];
                break;
            case 'A':
            case 'a':
                if (!parseNumberList(ptr, end, c, 3)
                    || !parseArcFlag(ptr, end, f[0])
                    || !parseArcFlag(ptr, end, f[1])
                    || !parseNumberList(ptr, end, c + 3, 2))
                    return path;

                if (command == 'a')
                {
                    c[3] += currentPoint.x;
                    c[4] += currentPoint.y;
                }

                path.arcTo(currentPoint.x, currentPoint.y, c[0], c[1], c[2], f[0], f[1], c[3], c[4]);
                currentPoint.x = controlPoint.x = c[3];
                currentPoint.y = controlPoint.y = c[4];
                break;
            case 'Z':
            case 'z':
                path.close();
                currentPoint.x = controlPoint.x = startPoint.x;
                currentPoint.y = controlPoint.y = startPoint.y;
                break;
            default:
                return path;
            }

            Utils::skipWsComma(ptr, end);
            if (ptr >= end)
                break;

            if (IS_ALPHA(*ptr))
                command = *ptr++;
        }

        return path;
    }

    std::string Parser::parseUrl(const std::string& string)
    {
        if (string.empty())
            return std::string{};

        auto ptr = string.data();
        auto end = ptr + string.size();

        if (!Utils::skipDesc(ptr, end, "url(#"))
            return std::string{};

        std::string value;
        if (!Utils::readUntil(ptr, end, ')', value))
            return std::string{};

        return value;
    }

    std::string Parser::parseHref(const std::string& string)
    {
        if (string.size() > 1 && string.front() == '#')
            return string.substr(1);

        return std::string{};
    }

    Rect Parser::parseViewBox(const std::string& string)
    {
        if (string.empty())
            return Rect::Invalid;

        auto ptr = string.data();
        auto end = ptr + string.size();

        double x;
        double y;
        double w;
        double h;
        if (!Utils::parseNumber(ptr, end, x)
            || !Utils::skipWsComma(ptr, end)
            || !Utils::parseNumber(ptr, end, y)
            || !Utils::skipWsComma(ptr, end)
            || !Utils::parseNumber(ptr, end, w)
            || !Utils::skipWsComma(ptr, end)
            || !Utils::parseNumber(ptr, end, h))
            return Rect::Invalid;

        if (w < 0.0 || h < 0.0)
            return Rect::Invalid;

        return Rect{ x, y, w, h };
    }

    PreserveAspectRatio Parser::parsePreserveAspectRatio(const std::string& string)
    {
        if (string.empty())
            return PreserveAspectRatio{};

        auto ptr = string.data();
        auto end = ptr + string.size();

        Align align{ Align::xMidYMid };
        MeetOrSlice scale{ MeetOrSlice::Meet };
        if (Utils::skipDesc(ptr, end, "none"))
            align = Align::None;
        else if (Utils::skipDesc(ptr, end, "xMinYMin"))
            align = Align::xMinYMin;
        else if (Utils::skipDesc(ptr, end, "xMidYMin"))
            align = Align::xMidYMin;
        else if (Utils::skipDesc(ptr, end, "xMaxYMin"))
            align = Align::xMaxYMin;
        else if (Utils::skipDesc(ptr, end, "xMinYMid"))
            align = Align::xMinYMid;
        else if (Utils::skipDesc(ptr, end, "xMidYMid"))
            align = Align::xMidYMid;
        else if (Utils::skipDesc(ptr, end, "xMaxYMid"))
            align = Align::xMaxYMid;
        else if (Utils::skipDesc(ptr, end, "xMinYMax"))
            align = Align::xMinYMax;
        else if (Utils::skipDesc(ptr, end, "xMidYMax"))
            align = Align::xMidYMax;
        else if (Utils::skipDesc(ptr, end, "xMaxYMax"))
            align = Align::xMaxYMax;
        else
            return PreserveAspectRatio{};

        Utils::skipWs(ptr, end);
        if (Utils::skipDesc(ptr, end, "slice"))
            scale = MeetOrSlice::Slice;
        else
            scale = MeetOrSlice::Meet;

        return PreserveAspectRatio{ align, scale };
    }

    //static const double pi = 3.14159265358979323846;

    Angle Parser::parseAngle(const std::string& string)
    {
        if (string.empty())
            return Angle{};

        auto ptr = string.data();
        auto end = ptr + string.size();

        if (Utils::skipDesc(ptr, end, "auto"))
            return MarkerOrient::Auto;

        double value;
        if (!Utils::parseNumber(ptr, end, value))
            return Angle{};

        if (Utils::skipDesc(ptr, end, "rad"))
            value *= 180.0 / pi;
        else if (Utils::skipDesc(ptr, end, "grad"))
            value *= 360.0 / 400.0;

        return Angle{ value, MarkerOrient::Angle };
    }

    MarkerUnits Parser::parseMarkerUnits(const std::string& string)
    {
        if (string.empty())
            return MarkerUnits::StrokeWidth;

        if (string.compare("userSpaceOnUse") == 0)
            return MarkerUnits::UserSpaceOnUse;
        return MarkerUnits::StrokeWidth;
    }

    SpreadMethod Parser::parseSpreadMethod(const std::string& string)
    {
        if (string.empty())
            return SpreadMethod::Pad;

        if (string.compare("repeat") == 0)
            return SpreadMethod::Repeat;
        if (string.compare("reflect") == 0)
            return SpreadMethod::Reflect;
        return SpreadMethod::Pad;
    }

    Units Parser::parseUnits(const std::string& string, Units defaultValue)
    {
        if (string.empty())
            return defaultValue;

        if (string.compare("userSpaceOnUse") == 0)
            return Units::UserSpaceOnUse;
        if (string.compare("objectBoundingBox") == 0)
            return Units::ObjectBoundingBox;
        return defaultValue;
    }

    static const std::map<std::string, unsigned int> colormap = {
        {"aliceblue", 0xF0F8FF},
        {"antiquewhite", 0xFAEBD7},
        {"aqua", 0x00FFFF},
        {"aquamarine", 0x7FFFD4},
        {"azure", 0xF0FFFF},
        {"beige", 0xF5F5DC},
        {"bisque", 0xFFE4C4},
        {"black", 0x000000},
        {"blanchedalmond", 0xFFEBCD},
        {"blue", 0x0000FF},
        {"blueviolet", 0x8A2BE2},
        {"brown", 0xA52A2A},
        {"burlywood", 0xDEB887},
        {"cadetblue", 0x5F9EA0},
        {"chartreuse", 0x7FFF00},
        {"chocolate", 0xD2691E},
        {"coral", 0xFF7F50},
        {"cornflowerblue", 0x6495ED},
        {"cornsilk", 0xFFF8DC},
        {"crimson", 0xDC143C},
        {"cyan", 0x00FFFF},
        {"darkblue", 0x00008B},
        {"darkcyan", 0x008B8B},
        {"darkgoldenrod", 0xB8860B},
        {"darkgray", 0xA9A9A9},
        {"darkgreen", 0x006400},
        {"darkgrey", 0xA9A9A9},
        {"darkkhaki", 0xBDB76B},
        {"darkmagenta", 0x8B008B},
        {"darkolivegreen", 0x556B2F},
        {"darkorange", 0xFF8C00},
        {"darkorchid", 0x9932CC},
        {"darkred", 0x8B0000},
        {"darksalmon", 0xE9967A},
        {"darkseagreen", 0x8FBC8F},
        {"darkslateblue", 0x483D8B},
        {"darkslategray", 0x2F4F4F},
        {"darkslategrey", 0x2F4F4F},
        {"darkturquoise", 0x00CED1},
        {"darkviolet", 0x9400D3},
        {"deeppink", 0xFF1493},
        {"deepskyblue", 0x00BFFF},
        {"dimgray", 0x696969},
        {"dimgrey", 0x696969},
        {"dodgerblue", 0x1E90FF},
        {"firebrick", 0xB22222},
        {"floralwhite", 0xFFFAF0},
        {"forestgreen", 0x228B22},
        {"fuchsia", 0xFF00FF},
        {"gainsboro", 0xDCDCDC},
        {"ghostwhite", 0xF8F8FF},
        {"gold", 0xFFD700},
        {"goldenrod", 0xDAA520},
        {"gray", 0x808080},
        {"green", 0x008000},
        {"greenyellow", 0xADFF2F},
        {"grey", 0x808080},
        {"honeydew", 0xF0FFF0},
        {"hotpink", 0xFF69B4},
        {"indianred", 0xCD5C5C},
        {"indigo", 0x4B0082},
        {"ivory", 0xFFFFF0},
        {"khaki", 0xF0E68C},
        {"lavender", 0xE6E6FA},
        {"lavenderblush", 0xFFF0F5},
        {"lawngreen", 0x7CFC00},
        {"lemonchiffon", 0xFFFACD},
        {"lightblue", 0xADD8E6},
        {"lightcoral", 0xF08080},
        {"lightcyan", 0xE0FFFF},
        {"lightgoldenrodyellow", 0xFAFAD2},
        {"lightgray", 0xD3D3D3},
        {"lightgreen", 0x90EE90},
        {"lightgrey", 0xD3D3D3},
        {"lightpink", 0xFFB6C1},
        {"lightsalmon", 0xFFA07A},
        {"lightseagreen", 0x20B2AA},
        {"lightskyblue", 0x87CEFA},
        {"lightslategray", 0x778899},
        {"lightslategrey", 0x778899},
        {"lightsteelblue", 0xB0C4DE},
        {"lightyellow", 0xFFFFE0},
        {"lime", 0x00FF00},
        {"limegreen", 0x32CD32},
        {"linen", 0xFAF0E6},
        {"magenta", 0xFF00FF},
        {"maroon", 0x800000},
        {"mediumaquamarine", 0x66CDAA},
        {"mediumblue", 0x0000CD},
        {"mediumorchid", 0xBA55D3},
        {"mediumpurple", 0x9370DB},
        {"mediumseagreen", 0x3CB371},
        {"mediumslateblue", 0x7B68EE},
        {"mediumspringgreen", 0x00FA9A},
        {"mediumturquoise", 0x48D1CC},
        {"mediumvioletred", 0xC71585},
        {"midnightblue", 0x191970},
        {"mintcream", 0xF5FFFA},
        {"mistyrose", 0xFFE4E1},
        {"moccasin", 0xFFE4B5},
        {"navajowhite", 0xFFDEAD},
        {"navy", 0x000080},
        {"oldlace", 0xFDF5E6},
        {"olive", 0x808000},
        {"olivedrab", 0x6B8E23},
        {"orange", 0xFFA500},
        {"orangered", 0xFF4500},
        {"orchid", 0xDA70D6},
        {"palegoldenrod", 0xEEE8AA},
        {"palegreen", 0x98FB98},
        {"paleturquoise", 0xAFEEEE},
        {"palevioletred", 0xDB7093},
        {"papayawhip", 0xFFEFD5},
        {"peachpuff", 0xFFDAB9},
        {"peru", 0xCD853F},
        {"pink", 0xFFC0CB},
        {"plum", 0xDDA0DD},
        {"powderblue", 0xB0E0E6},
        {"purple", 0x800080},
        {"rebeccapurple", 0x663399},
        {"red", 0xFF0000},
        {"rosybrown", 0xBC8F8F},
        {"royalblue", 0x4169E1},
        {"saddlebrown", 0x8B4513},
        {"salmon", 0xFA8072},
        {"sandybrown", 0xF4A460},
        {"seagreen", 0x2E8B57},
        {"seashell", 0xFFF5EE},
        {"sienna", 0xA0522D},
        {"silver", 0xC0C0C0},
        {"skyblue", 0x87CEEB},
        {"slateblue", 0x6A5ACD},
        {"slategray", 0x708090},
        {"slategrey", 0x708090},
        {"snow", 0xFFFAFA},
        {"springgreen", 0x00FF7F},
        {"steelblue", 0x4682B4},
        {"tan", 0xD2B48C},
        {"teal", 0x008080},
        {"thistle", 0xD8BFD8},
        {"tomato", 0xFF6347},
        {"turquoise", 0x40E0D0},
        {"violet", 0xEE82EE},
        {"wheat", 0xF5DEB3},
        {"white", 0xFFFFFF},
        {"whitesmoke", 0xF5F5F5},
        {"yellow", 0xFFFF00},
        {"yellowgreen", 0x9ACD32}
    };

    Color Parser::parseColor(const std::string& string, const StyledElement* element, const Color& defaultValue)
    {
        if (string.empty())
            return defaultValue;

        auto ptr = string.data();
        auto end = ptr + string.size();

        if (Utils::skipDesc(ptr, end, '#'))
        {
            auto start = ptr;
            unsigned int value;
            if (!Utils::parseInteger(ptr, end, value, 16))
                return defaultValue;

            auto n = ptr - start;
            if (n != 3 && n != 6)
                return defaultValue;

            if (n == 3)
            {
                value = ((value & 0xf00) << 8) | ((value & 0x0f0) << 4) | (value & 0x00f);
                value |= value << 4;
            }

            auto r = (value & 0xff0000) >> 16;
            auto g = (value & 0x00ff00) >> 8;
            auto b = (value & 0x0000ff) >> 0;

            return Color{ r / 255.0, g / 255.0, b / 255.0 };
        }

        if (Utils::skipDesc(ptr, end, "rgb("))
        {
            double r, g, b;
            if (!Utils::skipWs(ptr, end)
                || !parseColorComponent(ptr, end, r)
                || !Utils::skipWsComma(ptr, end)
                || !parseColorComponent(ptr, end, g)
                || !Utils::skipWsComma(ptr, end)
                || !parseColorComponent(ptr, end, b)
                || !Utils::skipWs(ptr, end)
                || !Utils::skipDesc(ptr, end, ')'))
                return defaultValue;

            return Color{ r / 255.0, g / 255.0, b / 255.0 };
        }

        if (Utils::skipDesc(ptr, end, "none"))
            return Color::Transparent;

        if (Utils::skipDesc(ptr, end, "currentColor"))
            return element->color();

        auto it = colormap.find(string);
        if (it == colormap.end())
            return defaultValue;

        auto value = it->second;
        auto r = (value & 0xff0000) >> 16;
        auto g = (value & 0x00ff00) >> 8;
        auto b = (value & 0x0000ff) >> 0;

        return Color{ r / 255.0, g / 255.0, b / 255.0 };
    }

    Paint Parser::parsePaint(const std::string& string, const StyledElement* element, const Color& defaultValue)
    {
        if (string.empty())
            return defaultValue;

        auto ptr = string.data();
        auto end = ptr + string.size();

        if (!Utils::skipDesc(ptr, end, "url(#"))
            return parseColor(string, element, defaultValue);

        std::string ref;
        if (!Utils::readUntil(ptr, end, ')', ref))
            return defaultValue;

        ++ptr;
        Utils::skipWs(ptr, end);

        std::string fallback{ ptr, end };
        if (fallback.empty())
            return Paint{ ref, Color::Transparent };
        return Paint{ ref, parseColor(fallback, element, defaultValue) };
    }

    WindRule Parser::parseWindRule(const std::string& string)
    {
        if (string.empty())
            return WindRule::NonZero;

        if (string.compare("evenodd") == 0)
            return WindRule::EvenOdd;
        return WindRule::NonZero;
    }

    LineCap Parser::parseLineCap(const std::string& string)
    {
        if (string.empty())
            return LineCap::Butt;

        if (string.compare("round") == 0)
            return LineCap::Round;
        if (string.compare("square") == 0)
            return LineCap::Square;
        return LineCap::Butt;
    }

    LineJoin Parser::parseLineJoin(const std::string& string)
    {
        if (string.empty())
            return LineJoin::Miter;

        if (string.compare("bevel") == 0)
            return LineJoin::Bevel;
        if (string.compare("round") == 0)
            return LineJoin::Round;
        return LineJoin::Miter;
    }

    Display Parser::parseDisplay(const std::string& string)
    {
        if (string.empty())
            return Display::Inline;

        if (string.compare("none") == 0)
            return Display::None;
        return Display::Inline;
    }

    Visibility Parser::parseVisibility(const std::string& string)
    {
        if (string.empty())
            return Visibility::Visible;

        if (string.compare("visible") == 0)
            return Visibility::Visible;
        return Visibility::Hidden;
    }

    Overflow Parser::parseOverflow(const std::string& string, Overflow defaultValue)
    {
        if (string.empty())
            return defaultValue;

        if (string.compare("visible") == 0)
            return Overflow::Visible;
        if (string.compare("hidden") == 0)
            return Overflow::Hidden;
        return defaultValue;
    }

    bool Parser::parseLength(const char*& ptr, const char* end, double& value, LengthUnits& units, LengthNegativeValuesMode mode)
    {
        if (!Utils::parseNumber(ptr, end, value))
            return false;

        if (mode == ForbidNegativeLengths && value < 0.0)
            return false;

        char c[2] = { 0, 0 };
        if (ptr + 0 < end) c[0] = ptr[0];
        if (ptr + 1 < end) c[1] = ptr[1];

        switch (c[0]) {
        case '%':
            units = LengthUnits::Percent;
            ptr += 1;
            break;
        case 'p':
            if (c[1] == 'x')
                units = LengthUnits::Px;
            else if (c[1] == 'c')
                units = LengthUnits::Pc;
            else if (ptr[1] == 't')
                units = LengthUnits::Pt;
            else
                return false;
            ptr += 2;
            break;
        case 'i':
            if (c[1] == 'n')
                units = LengthUnits::In;
            else
                return false;
            ptr += 2;
            break;
        case 'c':
            if (c[1] == 'm')
                units = LengthUnits::Cm;
            else
                return false;
            ptr += 2;
            break;
        case 'm':
            if (c[1] == 'm')
                units = LengthUnits::Mm;
            else
                return false;
            ptr += 2;
            break;
        case 'e':
            if (c[1] == 'm')
                units = LengthUnits::Em;
            else if (c[1] == 'x')
                units = LengthUnits::Ex;
            else
                return false;
            ptr += 2;
            break;
        default:
            units = LengthUnits::Number;
            break;
        }

        return true;
    }

    bool Parser::parseNumberList(const char*& ptr, const char* end, double* values, int count)
    {
        for (int i = 0;i < count;i++)
        {
            if (!Utils::parseNumber(ptr, end, values[i]))
                return false;
            Utils::skipWsComma(ptr, end);
        }

        return true;
    }

    bool Parser::parseArcFlag(const char*& ptr, const char* end, bool& flag)
    {
        if (ptr < end && *ptr == '0')
            flag = false;
        else if (ptr < end && *ptr == '1')
            flag = true;
        else
            return false;

        ++ptr;
        Utils::skipWsComma(ptr, end);
        return true;
    }

    bool Parser::parseColorComponent(const char*& ptr, const char* end, double& value)
    {
        if (!Utils::parseNumber(ptr, end, value))
            return false;

        if (Utils::skipDesc(ptr, end, '%'))
            value *= 2.55;

        value = (value < 0.0) ? 0.0 : (value > 255.0) ? 255.0 : std::round(value);
        return true;
    }

    bool Parser::parseTransform(const char*& ptr, const char* end, TransformType& type, double* values, int& count)
    {
        int required = 0;
        int optional = 0;
        if (Utils::skipDesc(ptr, end, "matrix"))
        {
            type = TransformType::Matrix;
            required = 6;
            optional = 0;
        }
        else if (Utils::skipDesc(ptr, end, "rotate"))
        {
            type = TransformType::Rotate;
            required = 1;
            optional = 2;
        }
        else if (Utils::skipDesc(ptr, end, "scale"))
        {
            type = TransformType::Scale;
            required = 1;
            optional = 1;
        }
        else if (Utils::skipDesc(ptr, end, "skewX"))
        {
            type = TransformType::SkewX;
            required = 1;
            optional = 0;
        }
        else if (Utils::skipDesc(ptr, end, "skewY"))
        {
            type = TransformType::SkewY;
            required = 1;
            optional = 0;
        }
        else if (Utils::skipDesc(ptr, end, "translate"))
        {
            type = TransformType::Translate;
            required = 1;
            optional = 1;
        }
        else
        {
            return false;
        }

        Utils::skipWs(ptr, end);
        if (ptr >= end || *ptr != '(')
            return false;
        ++ptr;

        int maxCount = required + optional;
        count = 0;
        Utils::skipWs(ptr, end);
        while (count < maxCount)
        {
            if (!Utils::parseNumber(ptr, end, values[count]))
                break;
            ++count;
            Utils::skipWsComma(ptr, end);
        }

        if (ptr >= end || *ptr != ')' || !(count == required || count == maxCount))
            return false;
        ++ptr;

        return true;
    }

    static const std::map<std::string, ElementId> elementmap = {
        {"circle", ElementId::Circle},
        {"clipPath", ElementId::ClipPath},
        {"defs", ElementId::Defs},
        {"ellipse", ElementId::Ellipse},
        {"g", ElementId::G},
        {"line", ElementId::Line},
        {"linearGradient", ElementId::LinearGradient},
        {"marker", ElementId::Marker},
        {"mask", ElementId::Mask},
        {"path", ElementId::Path},
        {"pattern", ElementId::Pattern},
        {"polygon", ElementId::Polygon},
        {"polyline", ElementId::Polyline},
        {"radialGradient", ElementId::RadialGradient},
        {"rect", ElementId::Rect},
        {"stop", ElementId::Stop},
        {"style", ElementId::Style},
        {"solidColor", ElementId::SolidColor},
        {"svg", ElementId::Svg},
        {"symbol", ElementId::Symbol},
        {"use", ElementId::Use}
    };

    static const std::map<std::string, PropertyId> propertymap = {
        {"class", PropertyId::Class},
        {"clipPathUnits", PropertyId::ClipPathUnits},
        {"cx", PropertyId::Cx},
        {"cy", PropertyId::Cy},
        {"d", PropertyId::D},
        {"fx", PropertyId::Fx},
        {"fy", PropertyId::Fy},
        {"gradientTransform", PropertyId::GradientTransform},
        {"gradientUnits", PropertyId::GradientUnits},
        {"height", PropertyId::Height},
        {"id", PropertyId::Id},
        {"markerHeight", PropertyId::MarkerHeight},
        {"markerUnits", PropertyId::MarkerUnits},
        {"markerWidth", PropertyId::MarkerWidth},
        {"maskContentUnits", PropertyId::MaskContentUnits},
        {"maskUnits", PropertyId::MaskUnits},
        {"offset", PropertyId::Offset},
        {"orient", PropertyId::Orient},
        {"patternContentUnits", PropertyId::PatternContentUnits},
        {"patternTransform", PropertyId::PatternTransform},
        {"patternUnits", PropertyId::PatternUnits},
        {"points", PropertyId::Points},
        {"preserveAspectRatio", PropertyId::PreserveAspectRatio},
        {"r", PropertyId::R},
        {"refX", PropertyId::RefX},
        {"refY", PropertyId::RefY},
        {"rx", PropertyId::Rx},
        {"ry", PropertyId::Ry},
        {"spreadMethod", PropertyId::SpreadMethod},
        {"style", PropertyId::Style},
        {"transform", PropertyId::Transform},
        {"viewBox", PropertyId::ViewBox},
        {"width", PropertyId::Width},
        {"x", PropertyId::X},
        {"x1", PropertyId::X1},
        {"x2", PropertyId::X2},
        {"xlink:href", PropertyId::Href},
        {"y", PropertyId::Y},
        {"y1", PropertyId::Y1},
        {"y2", PropertyId::Y2}
    };

    static const std::map<std::string, PropertyId> csspropertymap = {
        {"clip-path", PropertyId::Clip_Path},
        {"clip-rule", PropertyId::Clip_Rule},
        {"color", PropertyId::Color},
        {"display", PropertyId::Display},
        {"fill", PropertyId::Fill},
        {"fill-opacity", PropertyId::Fill_Opacity},
        {"fill-rule", PropertyId::Fill_Rule},
        {"marker-end", PropertyId::Marker_End},
        {"marker-mid", PropertyId::Marker_Mid},
        {"marker-start", PropertyId::Marker_Start},
        {"mask", PropertyId::Mask},
        {"opacity", PropertyId::Opacity},
        {"overflow", PropertyId::Overflow},
        {"solid-color", PropertyId::Solid_Color},
        {"solid-opacity", PropertyId::Solid_Opacity},
        {"stop-color", PropertyId::Stop_Color},
        {"stop-opacity", PropertyId::Stop_Opacity},
        {"stroke", PropertyId::Stroke},
        {"stroke-dasharray", PropertyId::Stroke_Dasharray},
        {"stroke-dashoffset", PropertyId::Stroke_Dashoffset},
        {"stroke-linecap", PropertyId::Stroke_Linecap},
        {"stroke-linejoin", PropertyId::Stroke_Linejoin},
        {"stroke-miterlimit", PropertyId::Stroke_Miterlimit},
        {"stroke-opacity", PropertyId::Stroke_Opacity},
        {"stroke-width", PropertyId::Stroke_Width},
        {"visibility", PropertyId::Visibility}
    };

    static inline ElementId elementId(const std::string& name)
    {
        auto it = elementmap.find(name);
        if (it == elementmap.end())
            return ElementId::Unknown;

        return it->second;
    }

    static inline PropertyId cssPropertyId(const std::string& name)
    {
        auto it = csspropertymap.find(name);
        if (it == csspropertymap.end())
            return PropertyId::Unknown;

        return it->second;
    }

    static inline PropertyId propertyId(const std::string& name)
    {
        auto it = propertymap.find(name);
        if (it == propertymap.end())
            return cssPropertyId(name);

        return it->second;
    }

#define IS_STARTNAMECHAR(c) (IS_ALPHA(c) ||  (c) == '_' || (c) == ':')
#define IS_NAMECHAR(c) (IS_STARTNAMECHAR(c) || IS_NUM(c) || (c) == '-' || (c) == '.')
    static inline bool readIdentifier(const char*& ptr, const char* end, std::string& value)
    {
        if (ptr >= end || !IS_STARTNAMECHAR(*ptr))
            return false;

        auto start = ptr;
        ++ptr;
        while (ptr < end && IS_NAMECHAR(*ptr))
            ++ptr;

        value.assign(start, ptr);
        return true;
    }

#define IS_CSS_STARTNAMECHAR(c) (IS_ALPHA(c) || (c) == '_')
#define IS_CSS_NAMECHAR(c) (IS_CSS_STARTNAMECHAR(c) || IS_NUM(c) || (c) == '-')
    static inline bool readCSSIdentifier(const char*& ptr, const char* end, std::string& value)
    {
        if (ptr >= end || !IS_CSS_STARTNAMECHAR(*ptr))
            return false;

        auto start = ptr;
        ++ptr;
        while (ptr < end && IS_CSS_NAMECHAR(*ptr))
            ++ptr;

        value.assign(start, ptr);
        return true;
    }

    bool CSSParser::parseMore(const std::string& value)
    {
        auto ptr = value.data();
        auto end = ptr + value.size();

        while (ptr < end)
        {
            Utils::skipWs(ptr, end);
            if (Utils::skipDesc(ptr, end, '@'))
            {
                if (!parseAtRule(ptr, end))
                    return false;
                continue;
            }

            Rule rule;
            if (!parseRule(ptr, end, rule))
                return false;
            m_rules.push_back(rule);
        }

        return true;
    }

    bool CSSParser::parseAtRule(const char*& ptr, const char* end) const
    {
        int depth = 0;
        while (ptr < end)
        {
            auto ch = *ptr;
            ++ptr;
            if (ch == ';' && depth == 0)
                break;
            if (ch == '{') ++depth;
            else if (ch == '}' && depth > 0)
            {
                if (depth == 1)
                    break;
                --depth;
            }
        }

        return true;
    }

    bool CSSParser::parseRule(const char*& ptr, const char* end, Rule& rule) const
    {
        if (!parseSelectors(ptr, end, rule.selectors))
            return false;

        if (!parseDeclarations(ptr, end, rule.declarations))
            return false;

        return true;
    }

    bool CSSParser::parseSelectors(const char*& ptr, const char* end, SelectorList& selectors) const
    {
        Selector selector;
        if (!parseSelector(ptr, end, selector))
            return false;
        selectors.push_back(selector);

        while (Utils::skipDesc(ptr, end, ','))
        {
            Utils::skipWs(ptr, end);
            Selector selector;
            if (!parseSelector(ptr, end, selector))
                return false;
            selectors.push_back(selector);
        }

        return true;
    }

    bool CSSParser::parseDeclarations(const char*& ptr, const char* end, PropertyList& declarations) const
    {
        if (!Utils::skipDesc(ptr, end, '{'))
            return false;

        std::string name;
        std::string value;
        Utils::skipWs(ptr, end);
        do {
            if (!readCSSIdentifier(ptr, end, name))
                return false;
            Utils::skipWs(ptr, end);
            if (!Utils::skipDesc(ptr, end, ':'))
                return false;
            Utils::skipWs(ptr, end);
            auto start = ptr;
            while (ptr < end && !(*ptr == '!' || *ptr == ';' || *ptr == '}'))
                ++ptr;
            value.assign(start, Utils::rtrim(start, ptr));
            int specificity = 0x10;
            if (Utils::skipDesc(ptr, end, '!'))
            {
                if (!Utils::skipDesc(ptr, end, "important"))
                    return false;
                specificity = 0x1000;
            }

            auto id = cssPropertyId(name);
            if (id != PropertyId::Unknown)
                declarations.set(id, value, specificity);
            Utils::skipWsDelimiter(ptr, end, ';');
        } while (ptr < end && *ptr != '}');

        return Utils::skipDesc(ptr, end, '}');
    }

#define IS_SELECTOR_STARTNAMECHAR(c) (IS_CSS_STARTNAMECHAR(c) || (c) == '*' || (c) == '#' || (c) == '.' || (c) == '[' || (c) == ':')
    bool CSSParser::parseSelector(const char*& ptr, const char* end, Selector& selector) const
    {
        do {
            SimpleSelector simpleSelector;
            if (!parseSimpleSelector(ptr, end, simpleSelector))
                return false;

            selector.specificity += (simpleSelector.id == ElementId::Star) ? 0x0 : 0x1;
            for (auto& attributeSelector : simpleSelector.attributeSelectors)
                selector.specificity += (attributeSelector.id == PropertyId::Id) ? 0x10000 : 0x100;

            selector.simpleSelectors.push_back(simpleSelector);
            Utils::skipWs(ptr, end);
        } while (ptr < end && IS_SELECTOR_STARTNAMECHAR(*ptr));

        return true;
    }

    bool CSSParser::parseSimpleSelector(const char*& ptr, const char* end, SimpleSelector& simpleSelector) const
    {
        std::string name;
        if (Utils::skipDesc(ptr, end, '*'))
            simpleSelector.id = ElementId::Star;
        else if (readCSSIdentifier(ptr, end, name))
            simpleSelector.id = elementId(name);

        while (ptr < end)
        {
            if (Utils::skipDesc(ptr, end, '#'))
            {
                AttributeSelector a;
                a.id = PropertyId::Id;
                a.matchType = AttributeSelector::MatchType::Equal;
                if (!readCSSIdentifier(ptr, end, a.value))
                    return false;
                simpleSelector.attributeSelectors.push_back(a);
                continue;
            }

            if (Utils::skipDesc(ptr, end, '.'))
            {
                AttributeSelector a;
                a.id = PropertyId::Class;
                a.matchType = AttributeSelector::MatchType::Includes;
                if (!readCSSIdentifier(ptr, end, a.value))
                    return false;
                simpleSelector.attributeSelectors.push_back(a);
                continue;
            }

            if (Utils::skipDesc(ptr, end, '['))
            {
                Utils::skipWs(ptr, end);
                if (!readCSSIdentifier(ptr, end, name))
                    return false;
                AttributeSelector a;
                a.id = propertyId(name);
                if (Utils::skipDesc(ptr, end, '='))
                    a.matchType = AttributeSelector::MatchType::Equal;
                else if (Utils::skipDesc(ptr, end, "~="))
                    a.matchType = AttributeSelector::MatchType::Includes;
                else if (Utils::skipDesc(ptr, end, "|="))
                    a.matchType = AttributeSelector::MatchType::DashMatch;
                else if (Utils::skipDesc(ptr, end, "^="))
                    a.matchType = AttributeSelector::MatchType::StartsWith;
                else if (Utils::skipDesc(ptr, end, "$="))
                    a.matchType = AttributeSelector::MatchType::EndsWith;
                else if (Utils::skipDesc(ptr, end, "*="))
                    a.matchType = AttributeSelector::MatchType::Contains;
                if (a.matchType != AttributeSelector::MatchType::None)
                {
                    Utils::skipWs(ptr, end);
                    if (!readCSSIdentifier(ptr, end, a.value))
                    {
                        if (ptr >= end || !(*ptr == '\"' || *ptr == '\''))
                            return false;

                        auto quote = *ptr;
                        ++ptr;
                        if (!Utils::readUntil(ptr, end, quote, a.value))
                            return false;
                        ++ptr;
                    }
                }

                Utils::skipWs(ptr, end);
                if (!Utils::skipDesc(ptr, end, ']'))
                    return false;
                simpleSelector.attributeSelectors.push_back(a);
                continue;
            }

            if (Utils::skipDesc(ptr, end, ':'))
            {
                if (!readCSSIdentifier(ptr, end, name))
                    return false;
                PseudoClass pseudo;
                if (name.compare("empty") == 0)
                    pseudo.type = PseudoClass::Type::Empty;
                else if (name.compare("root") == 0)
                    pseudo.type = PseudoClass::Type::Root;
                else if (name.compare("not") == 0)
                    pseudo.type = PseudoClass::Type::Not;
                else if (name.compare("first-child") == 0)
                    pseudo.type = PseudoClass::Type::FirstChild;
                else if (name.compare("last-child") == 0)
                    pseudo.type = PseudoClass::Type::LastChild;
                else if (name.compare("only-child") == 0)
                    pseudo.type = PseudoClass::Type::OnlyChild;
                else if (name.compare("first-of-type") == 0)
                    pseudo.type = PseudoClass::Type::FirstOfType;
                else if (name.compare("last-of-type") == 0)
                    pseudo.type = PseudoClass::Type::LastOfType;
                else if (name.compare("only-of-type") == 0)
                    pseudo.type = PseudoClass::Type::OnlyOfType;
                if (pseudo.type == PseudoClass::Type::Not)
                {
                    if (!Utils::skipDesc(ptr, end, '('))
                        return false;

                    Utils::skipWs(ptr, end);
                    if (!parseSelectors(ptr, end, pseudo.notSelectors))
                        return false;

                    Utils::skipWs(ptr, end);
                    if (!Utils::skipDesc(ptr, end, ')'))
                        return false;
                }

                simpleSelector.pseudoClasses.push_back(pseudo);
                continue;
            }

            break;
        }

        Utils::skipWs(ptr, end);
        if (Utils::skipDesc(ptr, end, '>'))
            simpleSelector.combinator = SimpleSelector::Combinator::Child;
        else if (Utils::skipDesc(ptr, end, '+'))
            simpleSelector.combinator = SimpleSelector::Combinator::DirectAdjacent;
        else if (Utils::skipDesc(ptr, end, '~'))
            simpleSelector.combinator = SimpleSelector::Combinator::InDirectAdjacent;

        return true;
    }

    RuleMatchContext::RuleMatchContext(const std::vector<Rule>& rules)
    {
        for (auto& rule : rules)
            for (auto& selector : rule.selectors)
                m_selectors.emplace(selector.specificity, std::make_pair(&selector, &rule.declarations));
    }

    std::vector<const PropertyList*> RuleMatchContext::match(const Element* element) const
    {
        std::vector<const PropertyList*> declarations;
        auto it = m_selectors.begin();
        auto end = m_selectors.end();
        for (;it != end;++it)
        {
            auto& value = it->second;
            if (!selectorMatch(std::get<0>(value), element))
                continue;
            declarations.push_back(std::get<1>(value));
        }

        return declarations;
    }

    bool RuleMatchContext::selectorMatch(const Selector* selector, const Element* element) const
    {
        if (selector->simpleSelectors.empty())
            return false;

        if (selector->simpleSelectors.size() == 1)
            return simpleSelectorMatch(selector->simpleSelectors.front(), element);

        auto it = selector->simpleSelectors.rbegin();
        auto end = selector->simpleSelectors.rend();
        if (!simpleSelectorMatch(*it, element))
            return false;
        ++it;

        while (it != end)
        {
            switch (it->combinator) {
            case SimpleSelector::Combinator::Child:
            case SimpleSelector::Combinator::Descendant:
                element = element->parent;
                break;
            case SimpleSelector::Combinator::DirectAdjacent:
            case SimpleSelector::Combinator::InDirectAdjacent:
                element = element->previousSibling();
                break;
            }

            if (element == nullptr)
                return false;

            auto match = simpleSelectorMatch(*it, element);
            if (!match && (it->combinator != SimpleSelector::Combinator::Descendant && it->combinator != SimpleSelector::Combinator::InDirectAdjacent))
                return false;

            if (match || (it->combinator != SimpleSelector::Combinator::Descendant && it->combinator != SimpleSelector::Combinator::InDirectAdjacent))
                ++it;
        }

        return true;
    }

    bool RuleMatchContext::simpleSelectorMatch(const SimpleSelector& selector, const Element* element) const
    {
        if (selector.id != ElementId::Star && selector.id != element->id)
            return false;

        for (auto& attributeSelector : selector.attributeSelectors)
            if (!attributeSelectorMatch(attributeSelector, element))
                return false;

        for (auto& pseudoClass : selector.pseudoClasses)
            if (!pseudoClassMatch(pseudoClass, element))
                return false;

        return true;
    }

    bool RuleMatchContext::attributeSelectorMatch(const AttributeSelector& selector, const Element* element) const
    {
        auto& value = element->get(selector.id);
        if (value.empty())
            return false;

        if (selector.matchType == AttributeSelector::MatchType::None)
            return true;

        if (selector.matchType == AttributeSelector::MatchType::Equal)
            return selector.value == value;

        if (selector.matchType == AttributeSelector::MatchType::Includes)
        {
            auto ptr = value.data();
            auto end = ptr + value.size();
            while (ptr < end)
            {
                auto start = ptr;
                while (ptr < end && !IS_WS(*ptr))
                    ++ptr;

                if (selector.value == std::string(start, ptr))
                    return true;
                Utils::skipWs(ptr, end);
            }

            return false;
        }

        auto starts_with = [](const std::string& string, const std::string& prefix) {
            if (prefix.empty() || prefix.size() > string.size())
                return false;

            return string.compare(0, prefix.size(), prefix) == 0;
        };

        auto ends_with = [](const std::string& string, const std::string& suffix) {
            if (suffix.empty() || suffix.size() > string.size())
                return false;

            return string.compare(string.size() - suffix.size(), suffix.size(), suffix) == 0;
        };

        if (selector.matchType == AttributeSelector::MatchType::DashMatch)
        {
            if (selector.value == value)
                return true;

            return starts_with(value, selector.value + '-');
        }

        if (selector.matchType == AttributeSelector::MatchType::StartsWith)
            return starts_with(value, selector.value);

        if (selector.matchType == AttributeSelector::MatchType::EndsWith)
            return ends_with(value, selector.value);

        if (selector.matchType == AttributeSelector::MatchType::Contains)
            return value.find(selector.value) != std::string::npos;

        return false;
    }

    bool RuleMatchContext::pseudoClassMatch(const PseudoClass& pseudo, const Element* element) const
    {
        if (pseudo.type == PseudoClass::Type::Empty)
            return element->children.empty();

        if (pseudo.type == PseudoClass::Type::Root)
            return element->parent == nullptr;

        if (pseudo.type == PseudoClass::Type::Not)
        {
            for (auto& selector : pseudo.notSelectors)
                if (selectorMatch(&selector, element))
                    return false;
            return true;
        }

        if (pseudo.type == PseudoClass::Type::FirstChild)
            return !element->previousSibling();

        if (pseudo.type == PseudoClass::Type::LastChild)
            return !element->nextSibling();

        if (pseudo.type == PseudoClass::Type::OnlyChild)
            return !(element->previousSibling() || element->nextSibling());

        if (pseudo.type == PseudoClass::Type::FirstOfType)
        {
            auto sibling = element->previousSibling();
            while (sibling)
            {
                if (sibling->id == element->id)
                    return false;
                sibling = element->previousSibling();
            }

            return true;
        }

        if (pseudo.type == PseudoClass::Type::LastOfType)
        {
            auto sibling = element->nextSibling();
            while (sibling)
            {
                if (sibling->id == element->id)
                    return false;
                sibling = element->nextSibling();
            }

            return true;
        }

        return false;
    }

    static inline std::unique_ptr<Element> createElement(ElementId id)
    {
        switch (id) {
        case ElementId::Svg:
            return std::make_unique<SVGElement>();
        case ElementId::Path:
            return std::make_unique<PathElement>();
        case ElementId::G:
            return std::make_unique<GElement>();
        case ElementId::Rect:
            return std::make_unique<RectElement>();
        case ElementId::Circle:
            return std::make_unique<CircleElement>();
        case ElementId::Ellipse:
            return std::make_unique<EllipseElement>();
        case ElementId::Line:
            return std::make_unique<LineElement>();
        case ElementId::Defs:
            return std::make_unique<DefsElement>();
        case ElementId::Polygon:
            return std::make_unique<PolygonElement>();
        case ElementId::Polyline:
            return std::make_unique<PolylineElement>();
        case ElementId::Stop:
            return std::make_unique<StopElement>();
        case ElementId::LinearGradient:
            return std::make_unique<LinearGradientElement>();
        case ElementId::RadialGradient:
            return std::make_unique<RadialGradientElement>();
        case ElementId::Symbol:
            return std::make_unique<SymbolElement>();
        case ElementId::Use:
            return std::make_unique<UseElement>();
        case ElementId::Pattern:
            return std::make_unique<PatternElement>();
        case ElementId::Mask:
            return std::make_unique<MaskElement>();
        case ElementId::ClipPath:
            return std::make_unique<ClipPathElement>();
        case ElementId::SolidColor:
            return std::make_unique<SolidColorElement>();
        case ElementId::Marker:
            return std::make_unique<MarkerElement>();
        case ElementId::Style:
            return std::make_unique<StyleElement>();
        default:
            break;
        }

        return nullptr;
    }

    static inline bool decodeText(const char* ptr, const char* end, std::string& value)
    {
        value.clear();
        while (ptr < end)
        {
            auto ch = *ptr;
            ++ptr;
            if (ch != '&')
            {
                value.push_back(ch);
                continue;
            }

            if (Utils::skipDesc(ptr, end, '#'))
            {
                int base = 10;
                if (Utils::skipDesc(ptr, end, 'x'))
                    base = 16;

                unsigned int cp;
                if (!Utils::parseInteger(ptr, end, cp, base))
                    return false;

                char c[5] = { 0, 0, 0, 0, 0 };
                if (cp < 0x80)
                {
                    c[1] = 0;
                    c[0] = cp;
                }
                else if (cp < 0x800)
                {
                    c[2] = 0;
                    c[1] = (cp & 0x3F) | 0x80;
                    cp >>= 6;
                    c[0] = cp | 0xC0;
                }
                else if (cp < 0x10000)
                {
                    c[3] = 0;
                    c[2] = (cp & 0x3F) | 0x80;
                    cp >>= 6;
                    c[1] = (cp & 0x3F) | 0x80;
                    cp >>= 6;
                    c[0] = cp | 0xE0;
                }
                else if (cp < 0x200000)
                {
                    c[4] = 0;
                    c[3] = (cp & 0x3F) | 0x80;
                    cp >>= 6;
                    c[2] = (cp & 0x3F) | 0x80;
                    cp >>= 6;
                    c[1] = (cp & 0x3F) | 0x80;
                    cp >>= 6;
                    c[0] = cp | 0xF0;
                }

                value.append(c);
            }
            else
            {
                if (Utils::skipDesc(ptr, end, "amp"))
                    value.push_back('&');
                else if (Utils::skipDesc(ptr, end, "lt"))
                    value.push_back('<');
                else if (Utils::skipDesc(ptr, end, "gt"))
                    value.push_back('>');
                else if (Utils::skipDesc(ptr, end, "quot"))
                    value.push_back('\"');
                else if (Utils::skipDesc(ptr, end, "apos"))
                    value.push_back('\'');
                else
                    return false;
            }

            if (!Utils::skipDesc(ptr, end, ';'))
                return false;
        }

        return true;
    }

    static inline void parseStyle(const std::string& string, Element* element)
    {
        auto ptr = string.data();
        auto end = ptr + string.size();

        std::string name;
        std::string value;
        Utils::skipWs(ptr, end);
        while (ptr < end && readCSSIdentifier(ptr, end, name))
        {
            Utils::skipWs(ptr, end);
            if (!Utils::skipDesc(ptr, end, ':'))
                return;
            Utils::skipWs(ptr, end);
            auto start = ptr;
            while (ptr < end && *ptr != ';')
                ++ptr;
            value.assign(start, Utils::rtrim(start, ptr));
            auto id = cssPropertyId(name);
            if (id != PropertyId::Unknown)
                element->set(id, value, 0x100);
            Utils::skipWsDelimiter(ptr, end, ';');
        }
    }

    ParseDocument::ParseDocument()
    {
    }

    ParseDocument::~ParseDocument()
    {
    }

    bool ParseDocument::parse(const char* data, std::size_t size)
    {
        auto ptr = data;
        auto end = ptr + size;

        CSSParser cssparser;
        Element* current = nullptr;
        std::string name;
        std::string value;
        int ignoring = 0;

        auto remove_comments = [](std::string& value) {
            auto start = value.find("/*");
            while (start != std::string::npos) {
                auto end = value.find("*/", start + 2);
                value.erase(start, end - start + 2);
                start = value.find("/*");
            }
        };

        auto handle_text = [&](const char* start, const char* end, bool in_cdata) {
            if (ignoring > 0 || current == nullptr || current->id != ElementId::Style)
                return;

            if (in_cdata)
                value.assign(start, end);
            else
                decodeText(start, end, value);

            remove_comments(value);
            cssparser.parseMore(value);
        };

        while (ptr < end)
        {
            auto start = ptr;
            if (!Utils::skipUntil(ptr, end, '<'))
                break;

            handle_text(start, ptr, false);
            ptr += 1;

            if (ptr < end && *ptr == '/')
            {
                if (current == nullptr && ignoring == 0)
                    return false;

                ++ptr;
                if (!readIdentifier(ptr, end, name))
                    return false;

                if (ptr >= end || *ptr != '>')
                    return false;

                if (ignoring > 0)
                    --ignoring;
                else
                    current = current->parent;

                ++ptr;
                continue;
            }

            if (ptr < end && *ptr == '?')
            {
                ++ptr;
                if (!readIdentifier(ptr, end, name))
                    return false;

                if (!Utils::skipUntil(ptr, end, "?>"))
                    return false;

                ptr += 2;
                continue;
            }

            if (ptr < end && *ptr == '!')
            {
                ++ptr;
                if (Utils::skipDesc(ptr, end, "--"))
                {
                    start = ptr;
                    if (!Utils::skipUntil(ptr, end, "-->"))
                        return false;

                    handle_text(start, ptr, false);
                    ptr += 3;
                    continue;
                }

                if (Utils::skipDesc(ptr, end, "[CDATA["))
                {
                    start = ptr;
                    if (!Utils::skipUntil(ptr, end, "]]>"))
                        return false;

                    handle_text(start, ptr, true);
                    ptr += 3;
                    continue;
                }

                if (Utils::skipDesc(ptr, end, "DOCTYPE"))
                {
                    while (ptr < end && *ptr != '>')
                    {
                        if (*ptr == '[')
                        {
                            ++ptr;
                            int depth = 1;
                            while (ptr < end && depth > 0)
                            {
                                if (*ptr == '[') ++depth;
                                else if (*ptr == ']') --depth;
                                ++ptr;
                            }
                        }
                        else
                        {
                            ++ptr;
                        }
                    }

                    if (ptr >= end || *ptr != '>')
                        return false;

                    ptr += 1;
                    continue;
                }

                return false;
            }

            if (!readIdentifier(ptr, end, name))
                return false;

            auto id = ignoring == 0 ? elementId(name) : ElementId::Unknown;
            if (id == ElementId::Unknown)
                ++ignoring;

            Element* element = nullptr;
            if (ignoring == 0)
            {
                if (m_rootElement && current == nullptr)
                    return false;

                if (m_rootElement == nullptr)
                {
                    if (id != ElementId::Svg)
                        return false;

                    m_rootElement = std::make_unique<SVGElement>();
                    element = m_rootElement.get();
                }
                else
                {
                    auto child = createElement(id);
                    element = child.get();
                    current->addChild(std::move(child));
                }
            }

            Utils::skipWs(ptr, end);
            while (ptr < end && readIdentifier(ptr, end, name))
            {
                Utils::skipWs(ptr, end);
                if (ptr >= end || *ptr != '=')
                    return false;
                ++ptr;

                Utils::skipWs(ptr, end);
                if (ptr >= end || !(*ptr == '\"' || *ptr == '\''))
                    return false;

                auto quote = *ptr;
                ++ptr;
                Utils::skipWs(ptr, end);
                start = ptr;
                while (ptr < end && *ptr != quote)
                    ++ptr;

                if (ptr >= end || *ptr != quote)
                    return false;

                auto id = element ? propertyId(name) : PropertyId::Unknown;
                if (id != PropertyId::Unknown)
                {
                    decodeText(start, Utils::rtrim(start, ptr), value);
                    if (id == PropertyId::Style)
                    {
                        remove_comments(value);
                        parseStyle(value, element);
                    }
                    else
                    {
                        if (id == PropertyId::Id)
                            m_idCache.emplace(value, element);
                        element->set(id, value, 0x1);
                    }
                }

                ++ptr;
                Utils::skipWs(ptr, end);
            }

            if (ptr < end && *ptr == '>')
            {
                if (element != nullptr)
                    current = element;

                ++ptr;
                continue;
            }

            if (ptr < end && *ptr == '/')
            {
                ++ptr;
                if (ptr >= end || *ptr != '>')
                    return false;

                if (ignoring > 0)
                    --ignoring;

                ++ptr;
                continue;
            }

            return false;
        }

        if (!m_rootElement || ptr < end || ignoring > 0)
            return false;

        const auto& rules = cssparser.rules();
        if (!rules.empty())
        {
            RuleMatchContext context(rules);
            m_rootElement->transverse([&context](Node* node) {
                if (node->isText())
                    return false;

                auto element = static_cast<Element*>(node);
                auto declarations = context.match(element);
                for (auto& declaration : declarations)
                    element->properties.add(*declaration);
                return false;
                });
        }

        return true;
    }

    Element* ParseDocument::getElementById(const std::string& id) const
    {
        auto it = m_idCache.find(id);
        if (it == m_idCache.end())
            return nullptr;

        return it->second;
    }

    std::unique_ptr<LayoutSymbol> ParseDocument::layout() const
    {
        return m_rootElement->layoutDocument(this);
    }

} // namespace lunasvg


namespace lunasvg {

    const Color Color::Black{ 0, 0, 0, 1 };
    const Color Color::White{ 1, 1, 1, 1 };
    const Color Color::Red{ 1, 0, 0, 1 };
    const Color Color::Green{ 0, 1, 0, 1 };
    const Color Color::Blue{ 0, 0, 1, 1 };
    const Color Color::Yellow{ 1, 1, 0, 1 };
    const Color Color::Transparent{ 0, 0, 0, 0 };

    Color::Color(double r, double g, double b, double a)
        : r(r), g(g), b(b), a(a)
    {
    }

    Paint::Paint(const Color& color)
        : m_color(color)
    {
    }

    Paint::Paint(const std::string& ref, const Color& color)
        : m_color(color), m_ref(ref)
    {
    }

    Point::Point(double x, double y)
        : x(x), y(y)
    {
    }

    const Rect Rect::Empty{ 0, 0, 0, 0 };
    const Rect Rect::Invalid{ 0, 0, -1, -1 };

    Rect::Rect(double x, double y, double w, double h)
        : x(x), y(y), w(w), h(h)
    {
    }

    Rect Rect::operator&(const Rect& rect) const
    {
        if (!rect.valid())
            return *this;

        if (!valid())
            return rect;

        auto l = (std::max)(x, rect.x);
        auto t = (std::max)(y, rect.y);
        auto r = (std::min)(x + w, rect.x + rect.w);
        auto b = (std::min)(y + h, rect.y + rect.h);

        return Rect{ l, t, r - l, b - t };
    }

    Rect Rect::operator|(const Rect& rect) const
    {
        if (!rect.valid())
            return *this;

        if (!valid())
            return rect;

        auto l = (std::min)(x, rect.x);
        auto t = (std::min)(y, rect.y);
        auto r = (std::max)(x + w, rect.x + rect.w);
        auto b = (std::max)(y + h, rect.y + rect.h);

        return Rect{ l, t, r - l, b - t };
    }

    Rect& Rect::intersect(const Rect& rect)
    {
        *this = *this & rect;
        return *this;
    }

    Rect& Rect::unite(const Rect& rect)
    {
        *this = *this | rect;
        return *this;
    }

    Transform::Transform(double m00, double m10, double m01, double m11, double m02, double m12)
        : m00(m00), m10(m10), m01(m01), m11(m11), m02(m02), m12(m12)
    {
    }

    Transform Transform::inverted() const
    {
        double det = (this->m00 * this->m11 - this->m10 * this->m01);
        if (det == 0.0)
            return Transform{};

        double inv_det = 1.0 / det;
        double m00 = this->m00 * inv_det;
        double m10 = this->m10 * inv_det;
        double m01 = this->m01 * inv_det;
        double m11 = this->m11 * inv_det;
        double m02 = (this->m01 * this->m12 - this->m11 * this->m02) * inv_det;
        double m12 = (this->m10 * this->m02 - this->m00 * this->m12) * inv_det;

        return Transform{ m11, -m10, -m01, m00, m02, m12 };
    }

    Transform Transform::operator*(const Transform& transform) const
    {
        double m00 = this->m00 * transform.m00 + this->m10 * transform.m01;
        double m10 = this->m00 * transform.m10 + this->m10 * transform.m11;
        double m01 = this->m01 * transform.m00 + this->m11 * transform.m01;
        double m11 = this->m01 * transform.m10 + this->m11 * transform.m11;
        double m02 = this->m02 * transform.m00 + this->m12 * transform.m01 + transform.m02;
        double m12 = this->m02 * transform.m10 + this->m12 * transform.m11 + transform.m12;

        return Transform{ m00, m10, m01, m11, m02, m12 };
    }

    Transform& Transform::operator*=(const Transform& transform)
    {
        *this = *this * transform;
        return *this;
    }

    Transform& Transform::premultiply(const Transform& transform)
    {
        *this = transform * *this;
        return *this;
    }

    Transform& Transform::postmultiply(const Transform& transform)
    {
        *this = *this * transform;
        return *this;
    }

    Transform& Transform::rotate(double angle)
    {
        *this = rotated(angle) * *this;
        return *this;
    }

    Transform& Transform::rotate(double angle, double cx, double cy)
    {
        *this = rotated(angle, cx, cy) * *this;
        return *this;
    }

    Transform& Transform::scale(double sx, double sy)
    {
        *this = scaled(sx, sy) * *this;
        return *this;
    }

    Transform& Transform::shear(double shx, double shy)
    {
        *this = sheared(shx, shy) * *this;
        return *this;
    }

    Transform& Transform::translate(double tx, double ty)
    {
        *this = translated(tx, ty) * *this;
        return *this;
    }

    Transform& Transform::transform(double m00, double m10, double m01, double m11, double m02, double m12)
    {
        *this = Transform{ m00, m10, m01, m11, m02, m12 } **this;
        return *this;
    }

    Transform& Transform::identity()
    {
        *this = Transform{ 1, 0, 0, 1, 0, 0 };
        return *this;
    }

    Transform& Transform::invert()
    {
        *this = inverted();
        return *this;
    }

    void Transform::map(double x, double y, double* _x, double* _y) const
    {
        *_x = x * m00 + y * m01 + m02;
        *_y = x * m10 + y * m11 + m12;
    }

    Point Transform::map(double x, double y) const
    {
        map(x, y, &x, &y);
        return Point{ x, y };
    }

    Point Transform::map(const Point& point) const
    {
        return map(point.x, point.y);
    }

    Rect Transform::map(const Rect& rect) const
    {
        if (!rect.valid())
            return Rect::Invalid;

        auto x1 = rect.x;
        auto y1 = rect.y;
        auto x2 = rect.x + rect.w;
        auto y2 = rect.y + rect.h;

        const Point p[] = {
            map(x1, y1), map(x2, y1),
            map(x2, y2), map(x1, y2)
        };

        auto l = p[0].x;
        auto t = p[0].y;
        auto r = p[0].x;
        auto b = p[0].y;

        for (int i = 1;i < 4;i++)
        {
            if (p[i].x < l) l = p[i].x;
            if (p[i].x > r) r = p[i].x;
            if (p[i].y < t) t = p[i].y;
            if (p[i].y > b) b = p[i].y;
        }

        return Rect{ l, t, r - l, b - t };
    }

    //static const double pi = 3.14159265358979323846;

    Transform Transform::rotated(double angle)
    {
        auto c = std::cos(angle * pi / 180.0);
        auto s = std::sin(angle * pi / 180.0);

        return Transform{ c, s, -s, c, 0, 0 };
    }

    Transform Transform::rotated(double angle, double cx, double cy)
    {
        auto c = std::cos(angle * pi / 180.0);
        auto s = std::sin(angle * pi / 180.0);

        auto x = cx * (1 - c) + cy * s;
        auto y = cy * (1 - c) - cx * s;

        return Transform{ c, s, -s, c, x, y };
    }

    Transform Transform::scaled(double sx, double sy)
    {
        return Transform{ sx, 0, 0, sy, 0, 0 };
    }

    Transform Transform::sheared(double shx, double shy)
    {
        auto x = std::tan(shx * pi / 180.0);
        auto y = std::tan(shy * pi / 180.0);

        return Transform{ 1, y, x, 1, 0, 0 };
    }

    Transform Transform::translated(double tx, double ty)
    {
        return Transform{ 1, 0, 0, 1, tx, ty };
    }

    void Path::moveTo(double x, double y)
    {
        m_commands.push_back(PathCommand::MoveTo);
        m_points.emplace_back(x, y);
    }

    void Path::lineTo(double x, double y)
    {
        m_commands.push_back(PathCommand::LineTo);
        m_points.emplace_back(x, y);
    }

    void Path::cubicTo(double x1, double y1, double x2, double y2, double x3, double y3)
    {
        m_commands.push_back(PathCommand::CubicTo);
        m_points.emplace_back(x1, y1);
        m_points.emplace_back(x2, y2);
        m_points.emplace_back(x3, y3);
    }

    void Path::close()
    {
        if (m_commands.empty())
            return;

        if (m_commands.back() == PathCommand::Close)
            return;

        m_commands.push_back(PathCommand::Close);
    }

    void Path::reset()
    {
        m_commands.clear();
        m_points.clear();
    }

    bool Path::empty() const
    {
        return m_commands.empty();
    }

    void Path::quadTo(double cx, double cy, double x1, double y1, double x2, double y2)
    {
        auto cx1 = 2.0 / 3.0 * x1 + 1.0 / 3.0 * cx;
        auto cy1 = 2.0 / 3.0 * y1 + 1.0 / 3.0 * cy;
        auto cx2 = 2.0 / 3.0 * x1 + 1.0 / 3.0 * x2;
        auto cy2 = 2.0 / 3.0 * y1 + 1.0 / 3.0 * y2;
        cubicTo(cx1, cy1, cx2, cy2, x2, y2);
    }

    void Path::arcTo(double cx, double cy, double rx, double ry, double xAxisRotation, bool largeArcFlag, bool sweepFlag, double x, double y)
    {
        rx = std::fabs(rx);
        ry = std::fabs(ry);

        auto sin_th = std::sin(xAxisRotation * pi / 180.0);
        auto cos_th = std::cos(xAxisRotation * pi / 180.0);

        auto dx = (cx - x) / 2.0;
        auto dy = (cy - y) / 2.0;
        auto dx1 = cos_th * dx + sin_th * dy;
        auto dy1 = -sin_th * dx + cos_th * dy;
        auto Pr1 = rx * rx;
        auto Pr2 = ry * ry;
        auto Px = dx1 * dx1;
        auto Py = dy1 * dy1;
        auto check = Px / Pr1 + Py / Pr2;
        if (check > 1)
        {
            rx = rx * std::sqrt(check);
            ry = ry * std::sqrt(check);
        }

        auto a00 = cos_th / rx;
        auto a01 = sin_th / rx;
        auto a10 = -sin_th / ry;
        auto a11 = cos_th / ry;
        auto x0 = a00 * cx + a01 * cy;
        auto y0 = a10 * cx + a11 * cy;
        auto x1 = a00 * x + a01 * y;
        auto y1 = a10 * x + a11 * y;
        auto d = (x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0);
        auto sfactor_sq = 1.0 / d - 0.25;
        if (sfactor_sq < 0) sfactor_sq = 0;
        auto sfactor = std::sqrt(sfactor_sq);
        if (sweepFlag == largeArcFlag) sfactor = -sfactor;
        auto xc = 0.5 * (x0 + x1) - sfactor * (y1 - y0);
        auto yc = 0.5 * (y0 + y1) + sfactor * (x1 - x0);

        auto th0 = std::atan2(y0 - yc, x0 - xc);
        auto th1 = std::atan2(y1 - yc, x1 - xc);

        double th_arc = th1 - th0;
        if (th_arc < 0.0 && sweepFlag)
            th_arc += 2.0 * pi;
        else if (th_arc > 0.0 && !sweepFlag)
            th_arc -= 2.0 * pi;

        auto n_segs = static_cast<int>(std::ceil(std::fabs(th_arc / (pi * 0.5 + 0.001))));
        for (int i = 0;i < n_segs;i++)
        {
            auto th2 = th0 + i * th_arc / n_segs;
            auto th3 = th0 + (i + 1) * th_arc / n_segs;

            auto a00 = cos_th * rx;
            auto a01 = -sin_th * ry;
            auto a10 = sin_th * rx;
            auto a11 = cos_th * ry;

            auto thHalf = 0.5 * (th3 - th2);
            auto t = (8.0 / 3.0) * std::sin(thHalf * 0.5) * std::sin(thHalf * 0.5) / std::sin(thHalf);
            auto x1 = xc + std::cos(th2) - t * std::sin(th2);
            auto y1 = yc + std::sin(th2) + t * std::cos(th2);
            auto x3 = xc + std::cos(th3);
            auto y3 = yc + std::sin(th3);
            auto x2 = x3 + t * std::sin(th3);
            auto y2 = y3 - t * std::cos(th3);

            auto cx1 = a00 * x1 + a01 * y1;
            auto cy1 = a10 * x1 + a11 * y1;
            auto cx2 = a00 * x2 + a01 * y2;
            auto cy2 = a10 * x2 + a11 * y2;
            auto cx3 = a00 * x3 + a01 * y3;
            auto cy3 = a10 * x3 + a11 * y3;
            cubicTo(cx1, cy1, cx2, cy2, cx3, cy3);
        }
    }

    static const double kappa = 0.55228474983079339840;

    void Path::ellipse(double cx, double cy, double rx, double ry)
    {
        auto left = cx - rx;
        auto top = cy - ry;
        auto right = cx + rx;
        auto bottom = cy + ry;

        auto cpx = rx * kappa;
        auto cpy = ry * kappa;

        moveTo(cx, top);
        cubicTo(cx + cpx, top, right, cy - cpy, right, cy);
        cubicTo(right, cy + cpy, cx + cpx, bottom, cx, bottom);
        cubicTo(cx - cpx, bottom, left, cy + cpy, left, cy);
        cubicTo(left, cy - cpy, cx - cpx, top, cx, top);
        close();
    }

    void Path::rect(double x, double y, double w, double h, double rx, double ry)
    {
        rx = (std::min)(rx, w * 0.5);
        ry = (std::min)(ry, h * 0.5);

        auto right = x + w;
        auto bottom = y + h;

        if (rx == 0.0 && ry == 0.0)
        {
            moveTo(x, y);
            lineTo(right, y);
            lineTo(right, bottom);
            lineTo(x, bottom);
            lineTo(x, y);
            close();
        }
        else
        {
            double cpx = rx * kappa;
            double cpy = ry * kappa;
            moveTo(x, y + ry);
            cubicTo(x, y + ry - cpy, x + rx - cpx, y, x + rx, y);
            lineTo(right - rx, y);
            cubicTo(right - rx + cpx, y, right, y + ry - cpy, right, y + ry);
            lineTo(right, bottom - ry);
            cubicTo(right, bottom - ry + cpy, right - rx + cpx, bottom, right - rx, bottom);
            lineTo(x + rx, bottom);
            cubicTo(x + rx - cpx, bottom, x, bottom - ry + cpy, x, bottom - ry);
            lineTo(x, y + ry);
            close();
        }
    }

    Rect Path::box() const
    {
        if (m_points.empty())
            return Rect{};

        auto l = m_points[0].x;
        auto t = m_points[0].y;
        auto r = m_points[0].x;
        auto b = m_points[0].y;

        for (std::size_t i = 1;i < m_points.size();i++)
        {
            if (m_points[i].x < l) l = m_points[i].x;
            if (m_points[i].x > r) r = m_points[i].x;
            if (m_points[i].y < t) t = m_points[i].y;
            if (m_points[i].y > b) b = m_points[i].y;
        }

        return Rect{ l, t, r - l, b - t };
    }

    PathIterator::PathIterator(const Path& path)
        : m_commands(path.commands()),
        m_points(path.points().data())
    {
    }

    PathCommand PathIterator::currentSegment(std::array<Point, 3>& points) const
    {
        auto command = m_commands[m_index];
        switch (command) {
        case PathCommand::MoveTo:
            points[0] = m_points[0];
            m_startPoint = points[0];
            break;
        case PathCommand::LineTo:
            points[0] = m_points[0];
            break;
        case PathCommand::CubicTo:
            points[0] = m_points[0];
            points[1] = m_points[1];
            points[2] = m_points[2];
            break;
        case PathCommand::Close:
            points[0] = m_startPoint;
            break;
        }

        return command;
    }

    bool PathIterator::isDone() const
    {
        return (m_index >= m_commands.size());
    }

    void PathIterator::next()
    {
        switch (m_commands[m_index]) {
        case PathCommand::MoveTo:
        case PathCommand::LineTo:
            m_points += 1;
            break;
        case PathCommand::CubicTo:
            m_points += 3;
            break;
        default:
            break;
        }

        m_index += 1;
    }

    const Length Length::Unknown{ 0, LengthUnits::Unknown };
    const Length Length::Zero{ 0, LengthUnits::Number };
    const Length Length::One{ 1, LengthUnits::Number };
    const Length Length::ThreePercent{ 3, LengthUnits::Percent };
    const Length Length::HundredPercent{ 100, LengthUnits::Percent };
    const Length Length::FiftyPercent{ 50, LengthUnits::Percent };
    const Length Length::OneTwentyPercent{ 120, LengthUnits::Percent };
    const Length Length::MinusTenPercent{ -10, LengthUnits::Percent };

    Length::Length(double value)
        : m_value(value)
    {
    }

    Length::Length(double value, LengthUnits units)
        : m_value(value), m_units(units)
    {
    }

    static const double dpi = 96.0;

    double Length::value(double max) const
    {
        switch (m_units) {
        case LengthUnits::Number:
        case LengthUnits::Px:
            return m_value;
        case LengthUnits::In:
            return m_value * dpi;
        case LengthUnits::Cm:
            return m_value * dpi / 2.54;
        case LengthUnits::Mm:
            return m_value * dpi / 25.4;
        case LengthUnits::Pt:
            return m_value * dpi / 72.0;
        case LengthUnits::Pc:
            return m_value * dpi / 6.0;
        case LengthUnits::Percent:
            return m_value * max / 100.0;
        default:
            break;
        }

        return 0.0;
    }

    //static const double sqrt2 = 1.41421356237309504880;

    double Length::value(const Element* element, LengthMode mode) const
    {
        if (m_units == LengthUnits::Percent)
        {
            auto viewport = element->currentViewport();
            auto w = viewport.w;
            auto h = viewport.h;
            auto max = (mode == LengthMode::Width) ? w : (mode == LengthMode::Height) ? h : std::sqrt(w * w + h * h) / sqrt2;
            return m_value * max / 100.0;
        }

        return value(1.0);
    }

    LengthContext::LengthContext(const Element* element)
        : m_element(element)
    {
    }

    LengthContext::LengthContext(const Element* element, Units units)
        : m_element(element), m_units(units)
    {
    }

    double LengthContext::valueForLength(const Length& length, LengthMode mode) const
    {
        if (m_units == Units::ObjectBoundingBox)
            return length.value(1.0);
        return length.value(m_element, mode);
    }

    PreserveAspectRatio::PreserveAspectRatio(Align align, MeetOrSlice scale)
        : m_align(align), m_scale(scale)
    {
    }

    Transform PreserveAspectRatio::getMatrix(double width, double height, const Rect& viewBox) const
    {
        if (viewBox.empty())
            return Transform{};

        auto xscale = width / viewBox.w;
        auto yscale = height / viewBox.h;
        if (m_align == Align::None)
        {
            auto xoffset = -viewBox.x * xscale;
            auto yoffset = -viewBox.y * yscale;
            return Transform{ xscale, 0, 0, yscale, xoffset, yoffset };
        }

        auto scale = (m_scale == MeetOrSlice::Meet) ? (std::min)(xscale, yscale) : (std::max)(xscale, yscale);
        auto viewWidth = viewBox.w * scale;
        auto viewHeight = viewBox.h * scale;

        auto xoffset = -viewBox.x * scale;
        auto yoffset = -viewBox.y * scale;

        switch (m_align) {
        case Align::xMidYMin:
        case Align::xMidYMid:
        case Align::xMidYMax:
            xoffset += (width - viewWidth) * 0.5;
            break;
        case Align::xMaxYMin:
        case Align::xMaxYMid:
        case Align::xMaxYMax:
            xoffset += (width - viewWidth);
            break;
        default:
            break;
        }

        switch (m_align) {
        case Align::xMinYMid:
        case Align::xMidYMid:
        case Align::xMaxYMid:
            yoffset += (height - viewHeight) * 0.5;
            break;
        case Align::xMinYMax:
        case Align::xMidYMax:
        case Align::xMaxYMax:
            yoffset += (height - viewHeight);
            break;
        default:
            break;
        }

        return Transform{ scale, 0, 0, scale, xoffset, yoffset };
    }

    Rect PreserveAspectRatio::getClip(double width, double height, const Rect& viewBox) const
    {
        if (viewBox.empty())
            return Rect{ 0, 0, width, height };

        return viewBox;
    }

    Angle::Angle(MarkerOrient type)
        : m_type(type)
    {
    }

    Angle::Angle(double value, MarkerOrient type)
        : m_value(value), m_type(type)
    {
    }

} // namespace lunasvg

namespace lunasvg {

    StopElement::StopElement()
        : StyledElement(ElementId::Stop)
    {
    }

    double StopElement::offset() const
    {
        auto& value = get(PropertyId::Offset);
        return Parser::parseNumberPercentage(value, 1.0);
    }

    Color StopElement::stopColorWithOpacity() const
    {
        auto color = stop_color();
        color.a = stop_opacity();
        return color;
    }

    std::unique_ptr<Node> StopElement::clone() const
    {
        return cloneElement<StopElement>();
    }

} // namespace lunasvg


namespace lunasvg {

    StyledElement::StyledElement(ElementId id)
        : Element(id)
    {
    }

    Paint StyledElement::fill() const
    {
        auto& value = find(PropertyId::Fill);
        return Parser::parsePaint(value, this, Color::Black);
    }

    Paint StyledElement::stroke() const
    {
        auto& value = find(PropertyId::Stroke);
        return Parser::parsePaint(value, this, Color::Transparent);
    }

    Color StyledElement::color() const
    {
        auto& value = find(PropertyId::Color);
        return Parser::parseColor(value, this, Color::Black);
    }

    Color StyledElement::stop_color() const
    {
        auto& value = find(PropertyId::Stop_Color);
        return Parser::parseColor(value, this, Color::Black);
    }

    Color StyledElement::solid_color() const
    {
        auto& value = find(PropertyId::Solid_Color);
        return Parser::parseColor(value, this, Color::Black);
    }

    double StyledElement::opacity() const
    {
        auto& value = get(PropertyId::Opacity);
        return Parser::parseNumberPercentage(value, 1.0);
    }

    double StyledElement::fill_opacity() const
    {
        auto& value = find(PropertyId::Fill_Opacity);
        return Parser::parseNumberPercentage(value, 1.0);
    }

    double StyledElement::stroke_opacity() const
    {
        auto& value = find(PropertyId::Stroke_Opacity);
        return Parser::parseNumberPercentage(value, 1.0);
    }

    double StyledElement::stop_opacity() const
    {
        auto& value = find(PropertyId::Stop_Opacity);
        return Parser::parseNumberPercentage(value, 1.0);
    }

    double StyledElement::solid_opacity() const
    {
        auto& value = find(PropertyId::Solid_Opacity);
        return Parser::parseNumberPercentage(value, 1.0);
    }

    double StyledElement::stroke_miterlimit() const
    {
        auto& value = find(PropertyId::Stroke_Miterlimit);
        return Parser::parseNumber(value, 4.0);
    }

    Length StyledElement::stroke_width() const
    {
        auto& value = find(PropertyId::Stroke_Width);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::One);
    }

    Length StyledElement::stroke_dashoffset() const
    {
        auto& value = find(PropertyId::Stroke_Dashoffset);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    LengthList StyledElement::stroke_dasharray() const
    {
        auto& value = find(PropertyId::Stroke_Dasharray);
        return Parser::parseLengthList(value, ForbidNegativeLengths);
    }

    WindRule StyledElement::fill_rule() const
    {
        auto& value = find(PropertyId::Fill_Rule);
        return Parser::parseWindRule(value);
    }

    WindRule StyledElement::clip_rule() const
    {
        auto& value = find(PropertyId::Clip_Rule);
        return Parser::parseWindRule(value);
    }

    LineCap StyledElement::stroke_linecap() const
    {
        auto& value = find(PropertyId::Stroke_Linecap);
        return Parser::parseLineCap(value);
    }

    LineJoin StyledElement::stroke_linejoin() const
    {
        auto& value = find(PropertyId::Stroke_Linejoin);
        return Parser::parseLineJoin(value);
    }

    Display StyledElement::display() const
    {
        auto& value = get(PropertyId::Display);
        return Parser::parseDisplay(value);
    }

    Visibility StyledElement::visibility() const
    {
        auto& value = find(PropertyId::Visibility);
        return Parser::parseVisibility(value);
    }

    Overflow StyledElement::overflow() const
    {
        auto& value = get(PropertyId::Overflow);
        return Parser::parseOverflow(value, parent == nullptr ? Overflow::Visible : Overflow::Hidden);
    }

    std::string StyledElement::clip_path() const
    {
        auto& value = get(PropertyId::Clip_Path);
        return Parser::parseUrl(value);
    }

    std::string StyledElement::mask() const
    {
        auto& value = get(PropertyId::Mask);
        return Parser::parseUrl(value);
    }

    std::string StyledElement::marker_start() const
    {
        auto& value = find(PropertyId::Marker_Start);
        return Parser::parseUrl(value);
    }

    std::string StyledElement::marker_mid() const
    {
        auto& value = find(PropertyId::Marker_Mid);
        return Parser::parseUrl(value);
    }

    std::string StyledElement::marker_end() const
    {
        auto& value = find(PropertyId::Marker_End);
        return Parser::parseUrl(value);
    }

    bool StyledElement::isDisplayNone() const
    {
        return display() == Display::None;
    }

    bool StyledElement::isOverflowHidden() const
    {
        return overflow() == Overflow::Hidden;
    }

} // namespace lunasvg


namespace lunasvg {

    StyleElement::StyleElement()
        : Element(ElementId::Style)
    {
    }

    std::unique_ptr<Node> StyleElement::clone() const
    {
        return cloneElement<StyleElement>();
    }

} // namespace lunasvg


namespace lunasvg {

    SVGElement::SVGElement()
        : GraphicsElement(ElementId::Svg)
    {
    }

    Length SVGElement::x() const
    {
        auto& value = get(PropertyId::X);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length SVGElement::y() const
    {
        auto& value = get(PropertyId::Y);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length SVGElement::width() const
    {
        auto& value = get(PropertyId::Width);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::HundredPercent);
    }

    Length SVGElement::height() const
    {
        auto& value = get(PropertyId::Height);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::HundredPercent);
    }

    Rect SVGElement::viewBox() const
    {
        auto& value = get(PropertyId::ViewBox);
        return Parser::parseViewBox(value);
    }

    PreserveAspectRatio SVGElement::preserveAspectRatio() const
    {
        auto& value = get(PropertyId::PreserveAspectRatio);
        return Parser::parsePreserveAspectRatio(value);
    }

    std::unique_ptr<LayoutSymbol> SVGElement::layoutDocument(const ParseDocument* document) const
    {
        if (isDisplayNone())
            return nullptr;

        auto w = this->width();
        auto h = this->height();
        if (w.isZero() || h.isZero())
            return nullptr;

        LengthContext lengthContext(this);
        auto _x = lengthContext.valueForLength(x(), LengthMode::Width);
        auto _y = lengthContext.valueForLength(y(), LengthMode::Height);
        auto _w = lengthContext.valueForLength(w, LengthMode::Width);
        auto _h = lengthContext.valueForLength(h, LengthMode::Height);

        auto viewBox = this->viewBox();
        auto preserveAspectRatio = this->preserveAspectRatio();
        auto viewTranslation = Transform::translated(_x, _y);
        auto viewTransform = preserveAspectRatio.getMatrix(_w, _h, viewBox);

        auto root = std::make_unique<LayoutSymbol>();
        root->width = _w;
        root->height = _h;
        root->transform = (viewTransform * viewTranslation) * transform();
        root->clip = isOverflowHidden() ? preserveAspectRatio.getClip(_w, _h, viewBox) : Rect::Invalid;
        root->opacity = opacity();

        LayoutContext context(document, root.get());
        root->masker = context.getMasker(mask());
        root->clipper = context.getClipper(clip_path());
        layoutChildren(&context, root.get());
        return root;
    }

    void SVGElement::layout(LayoutContext* context, LayoutContainer* current) const
    {
        if (isDisplayNone())
            return;

        auto w = this->width();
        auto h = this->height();
        if (w.isZero() || h.isZero())
            return;

        LengthContext lengthContext(this);
        auto _x = lengthContext.valueForLength(x(), LengthMode::Width);
        auto _y = lengthContext.valueForLength(y(), LengthMode::Height);
        auto _w = lengthContext.valueForLength(w, LengthMode::Width);
        auto _h = lengthContext.valueForLength(h, LengthMode::Height);

        auto viewBox = this->viewBox();
        auto preserveAspectRatio = this->preserveAspectRatio();
        auto viewTranslation = Transform::translated(_x, _y);
        auto viewTransform = preserveAspectRatio.getMatrix(_w, _h, viewBox);

        auto symbol = std::make_unique<LayoutSymbol>();
        symbol->width = _w;
        symbol->height = _h;
        symbol->transform = (viewTransform * viewTranslation) * transform();
        symbol->clip = isOverflowHidden() ? preserveAspectRatio.getClip(_w, _h, viewBox) : Rect::Invalid;
        symbol->opacity = opacity();
        symbol->masker = context->getMasker(mask());
        symbol->clipper = context->getClipper(clip_path());
        layoutChildren(context, symbol.get());
        current->addChildIfNotEmpty(std::move(symbol));
    }

    std::unique_ptr<Node> SVGElement::clone() const
    {
        return cloneElement<SVGElement>();
    }

} // namespace lunasvg


namespace lunasvg {

    SymbolElement::SymbolElement()
        : StyledElement(ElementId::Symbol)
    {
    }

    Length SymbolElement::x() const
    {
        auto& value = get(PropertyId::X);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length SymbolElement::y() const
    {
        auto& value = get(PropertyId::Y);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length SymbolElement::width() const
    {
        auto& value = get(PropertyId::Width);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::HundredPercent);
    }

    Length SymbolElement::height() const
    {
        auto& value = get(PropertyId::Height);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::HundredPercent);
    }

    Rect SymbolElement::viewBox() const
    {
        auto& value = get(PropertyId::ViewBox);
        return Parser::parseViewBox(value);
    }

    PreserveAspectRatio SymbolElement::preserveAspectRatio() const
    {
        auto& value = get(PropertyId::PreserveAspectRatio);
        return Parser::parsePreserveAspectRatio(value);
    }

    std::unique_ptr<Node> SymbolElement::clone() const
    {
        return cloneElement<SymbolElement>();
    }

} // namespace lunasvg


namespace lunasvg {

    UseElement::UseElement()
        : GraphicsElement(ElementId::Use)
    {
    }

    Length UseElement::x() const
    {
        auto& value = get(PropertyId::X);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length UseElement::y() const
    {
        auto& value = get(PropertyId::Y);
        return Parser::parseLength(value, AllowNegativeLengths, Length::Zero);
    }

    Length UseElement::width() const
    {
        auto& value = get(PropertyId::Width);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::HundredPercent);
    }

    Length UseElement::height() const
    {
        auto& value = get(PropertyId::Height);
        return Parser::parseLength(value, ForbidNegativeLengths, Length::HundredPercent);
    }

    std::string UseElement::href() const
    {
        auto& value = get(PropertyId::Href);
        return Parser::parseHref(value);
    }

    void UseElement::transferWidthAndHeight(Element* element) const
    {
        auto& width = get(PropertyId::Width);
        auto& height = get(PropertyId::Height);

        element->set(PropertyId::Width, width, 0x0);
        element->set(PropertyId::Height, height, 0x0);
    }

    void UseElement::layout(LayoutContext* context, LayoutContainer* current) const
    {
        if (isDisplayNone())
            return;

        auto ref = context->getElementById(href());
        if (ref == nullptr || context->hasReference(ref) || (current->id == LayoutId::ClipPath && !ref->isGeometry()))
            return;

        LayoutBreaker layoutBreaker(context, ref);
        auto group = std::make_unique<GElement>();
        group->parent = parent;
        group->properties = properties;

        LengthContext lengthContext(this);
        auto _x = lengthContext.valueForLength(x(), LengthMode::Width);
        auto _y = lengthContext.valueForLength(y(), LengthMode::Height);

        auto transform = get(PropertyId::Transform);
        transform += "translate(";
        transform += std::to_string(_x);
        transform += ' ';
        transform += std::to_string(_y);
        transform += ')';
        group->set(PropertyId::Transform, transform, 0x10);

        if (ref->id == ElementId::Svg || ref->id == ElementId::Symbol)
        {
            auto element = ref->cloneElement<SVGElement>();
            transferWidthAndHeight(element.get());
            group->addChild(std::move(element));
        }
        else
        {
            group->addChild(ref->clone());
        }

        group->layout(context, current);
    }

    std::unique_ptr<Node> UseElement::clone() const
    {
        return cloneElement<UseElement>();
    }

} // namespace lunasvg


plutovg_surface_t* plutovg_surface_create(int width, int height)
{
    plutovg_surface_t* surface = (plutovg_surface_t*)malloc(sizeof(plutovg_surface_t));
    surface->ref = 1;
    surface->owndata = 1;
    surface->data = (unsigned char*)calloc(1, (size_t)(width * height * 4));
    surface->width = width;
    surface->height = height;
    surface->stride = width * 4;
    return surface;
}

plutovg_surface_t* plutovg_surface_create_for_data(unsigned char* data, int width, int height, int stride)
{
    plutovg_surface_t* surface = (plutovg_surface_t*)malloc(sizeof(plutovg_surface_t));
    surface->ref = 1;
    surface->owndata = 0;
    surface->data = data;
    surface->width = width;
    surface->height = height;
    surface->stride = stride;
    return surface;
}

plutovg_surface_t* plutovg_surface_reference(plutovg_surface_t* surface)
{
    ++surface->ref;
    return surface;
}

void plutovg_surface_destroy(plutovg_surface_t* surface)
{
    if (surface == NULL)
        return;

    if (--surface->ref == 0)
    {
        if (surface->owndata)
            free(surface->data);
        free(surface);
    }
}

int plutovg_surface_get_reference_count(const plutovg_surface_t* surface)
{
    return surface->ref;
}

unsigned char* plutovg_surface_get_data(const plutovg_surface_t* surface)
{
    return surface->data;
}

int plutovg_surface_get_width(const plutovg_surface_t* surface)
{
    return surface->width;
}

int plutovg_surface_get_height(const plutovg_surface_t* surface)
{
    return surface->height;
}

int plutovg_surface_get_stride(const plutovg_surface_t* surface)
{
    return surface->stride;
}

plutovg_state_t* plutovg_state_create(void)
{
    plutovg_state_t* state = (plutovg_state_t*)malloc(sizeof(plutovg_state_t));
    state->clippath = NULL;
    state->source = plutovg_paint_create_rgb(0, 0, 0);
    plutovg_matrix_init_identity(&state->matrix);
    state->winding = plutovg_fill_rule_non_zero;
    state->stroke.width = 1.0;
    state->stroke.miterlimit = 4.0;
    state->stroke.cap = plutovg_line_cap_butt;
    state->stroke.join = plutovg_line_join_miter;
    state->stroke.dash = NULL;
    state->op = plutovg_operator_src_over;
    state->opacity = 1.0;
    state->next = NULL;
    return state;
}

plutovg_state_t* plutovg_state_clone(const plutovg_state_t* state)
{
    plutovg_state_t* newstate = (plutovg_state_t*)malloc(sizeof(plutovg_state_t));
    newstate->clippath = plutovg_rle_clone(state->clippath);
    newstate->source = plutovg_paint_reference(state->source); /** FIXME: **/
    newstate->matrix = state->matrix;
    newstate->winding = state->winding;
    newstate->stroke.width = state->stroke.width;
    newstate->stroke.miterlimit = state->stroke.miterlimit;
    newstate->stroke.cap = state->stroke.cap;
    newstate->stroke.join = state->stroke.join;
    newstate->stroke.dash = plutovg_dash_clone(state->stroke.dash);
    newstate->op = state->op;
    newstate->opacity = state->opacity;
    newstate->next = NULL;
    return newstate;
}

void plutovg_state_destroy(plutovg_state_t* state)
{
    plutovg_rle_destroy(state->clippath);
    plutovg_paint_destroy(state->source);
    plutovg_dash_destroy(state->stroke.dash);
    free(state);
}

plutovg_t* plutovg_create(plutovg_surface_t* surface)
{
    plutovg_t* pluto = (plutovg_t*)malloc(sizeof(plutovg_t));
    pluto->ref = 1;
    pluto->surface = plutovg_surface_reference(surface);
    pluto->state = plutovg_state_create();
    pluto->path = plutovg_path_create();
    pluto->rle = plutovg_rle_create();
    pluto->clippath = NULL;
    pluto->clip.x = 0.0;
    pluto->clip.y = 0.0;
    pluto->clip.w = surface->width;
    pluto->clip.h = surface->height;
    return pluto;
}

plutovg_t* plutovg_reference(plutovg_t* pluto)
{
    ++pluto->ref;
    return pluto;
}

void plutovg_destroy(plutovg_t* pluto)
{
    if (pluto == NULL)
        return;

    if (--pluto->ref == 0)
    {
        while (pluto->state)
        {
            plutovg_state_t* state = pluto->state;
            pluto->state = state->next;
            plutovg_state_destroy(state);
        }

        plutovg_surface_destroy(pluto->surface);
        plutovg_path_destroy(pluto->path);
        plutovg_rle_destroy(pluto->rle);
        plutovg_rle_destroy(pluto->clippath);
        free(pluto);
    }
}

int plutovg_get_reference_count(const plutovg_t* pluto)
{
    return pluto->ref;
}

void plutovg_save(plutovg_t* pluto)
{
    plutovg_state_t* newstate = plutovg_state_clone(pluto->state);
    newstate->next = pluto->state;
    pluto->state = newstate;
}

void plutovg_restore(plutovg_t* pluto)
{
    plutovg_state_t* oldstate = pluto->state;
    pluto->state = oldstate->next;
    plutovg_state_destroy(oldstate);
}

void plutovg_set_source_rgb(plutovg_t* pluto, double r, double g, double b)
{
    plutovg_set_source_rgba(pluto, r, g, b, 1.0);
}

void plutovg_set_source_rgba(plutovg_t* pluto, double r, double g, double b, double a)
{
    plutovg_paint_t* source = plutovg_paint_create_rgba(r, g, b, a);
    plutovg_set_source(pluto, source);
    plutovg_paint_destroy(source);
}

void plutovg_set_source_surface(plutovg_t* pluto, plutovg_surface_t* surface, double x, double y)
{
    plutovg_paint_t* source = plutovg_paint_create_for_surface(surface);
    plutovg_texture_t* texture = plutovg_paint_get_texture(source);
    plutovg_matrix_t matrix;
    plutovg_matrix_init_translate(&matrix, x, y);
    plutovg_texture_set_matrix(texture, &matrix);
    plutovg_set_source(pluto, source);
    plutovg_paint_destroy(source);
}

void plutovg_set_source_color(plutovg_t* pluto, const plutovg_color_t* color)
{
    plutovg_set_source_rgba(pluto, color->r, color->g, color->b, color->a);
}

void plutovg_set_source_gradient(plutovg_t* pluto, plutovg_gradient_t* gradient)
{
    plutovg_paint_t* source = plutovg_paint_create_gradient(gradient);
    plutovg_set_source(pluto, source);
    plutovg_paint_destroy(source);
}

void plutovg_set_source_texture(plutovg_t* pluto, plutovg_texture_t* texture)
{
    plutovg_paint_t* source = plutovg_paint_create_texture(texture);
    plutovg_set_source(pluto, source);
    plutovg_paint_destroy(source);
}

void plutovg_set_source(plutovg_t* pluto, plutovg_paint_t* source)
{
    source = plutovg_paint_reference(source);
    plutovg_paint_destroy(pluto->state->source);
    pluto->state->source = source;
}

plutovg_paint_t* plutovg_get_source(const plutovg_t* pluto)
{
    return pluto->state->source;
}

void plutovg_set_operator(plutovg_t* pluto, plutovg_operator_t op)
{
    pluto->state->op = op;
}

void plutovg_set_opacity(plutovg_t* pluto, double opacity)
{
    pluto->state->opacity = opacity;
}

void plutovg_set_fill_rule(plutovg_t* pluto, plutovg_fill_rule_t fill_rule)
{
    pluto->state->winding = fill_rule;
}

plutovg_operator_t plutovg_get_operator(const plutovg_t* pluto)
{
    return pluto->state->op;
}

double plutovg_get_opacity(const plutovg_t* pluto)
{
    return pluto->state->opacity;
}

plutovg_fill_rule_t plutovg_get_fill_rule(const plutovg_t* pluto)
{
    return pluto->state->winding;
}

void plutovg_set_line_width(plutovg_t* pluto, double width)
{
    pluto->state->stroke.width = width;
}

void plutovg_set_line_cap(plutovg_t* pluto, plutovg_line_cap_t cap)
{
    pluto->state->stroke.cap = cap;
}

void plutovg_set_line_join(plutovg_t* pluto, plutovg_line_join_t join)
{
    pluto->state->stroke.join = join;
}

void plutovg_set_miter_limit(plutovg_t* pluto, double limit)
{
    pluto->state->stroke.miterlimit = limit;
}

void plutovg_set_dash(plutovg_t* pluto, double offset, const double* data, int size)
{
    plutovg_dash_destroy(pluto->state->stroke.dash);
    pluto->state->stroke.dash = plutovg_dash_create(offset, data, size);
}

double plutovg_get_line_width(const plutovg_t* pluto)
{
    return pluto->state->stroke.width;
}

plutovg_line_cap_t plutovg_get_line_cap(const plutovg_t* pluto)
{
    return pluto->state->stroke.cap;
}

plutovg_line_join_t plutovg_get_line_join(const plutovg_t* pluto)
{
    return pluto->state->stroke.join;
}

double plutovg_get_miter_limit(const plutovg_t* pluto)
{
    return pluto->state->stroke.miterlimit;
}

void plutovg_translate(plutovg_t* pluto, double x, double y)
{
    plutovg_matrix_translate(&pluto->state->matrix, x, y);
}

void plutovg_scale(plutovg_t* pluto, double x, double y)
{
    plutovg_matrix_scale(&pluto->state->matrix, x, y);
}

void plutovg_rotate(plutovg_t* pluto, double radians, double x, double y)
{
    plutovg_matrix_rotate(&pluto->state->matrix, radians, x, y);
}

void plutovg_transform(plutovg_t* pluto, const plutovg_matrix_t* matrix)
{
    plutovg_matrix_multiply(&pluto->state->matrix, matrix, &pluto->state->matrix);
}

void plutovg_set_matrix(plutovg_t* pluto, const plutovg_matrix_t* matrix)
{
    pluto->state->matrix = *matrix;
}

void plutovg_identity_matrix(plutovg_t* pluto)
{
    plutovg_matrix_init_identity(&pluto->state->matrix);
}

void plutovg_get_matrix(const plutovg_t* pluto, plutovg_matrix_t* matrix)
{
    *matrix = pluto->state->matrix;
}

void plutovg_move_to(plutovg_t* pluto, double x, double y)
{
    plutovg_path_move_to(pluto->path, x, y);
}

void plutovg_line_to(plutovg_t* pluto, double x, double y)
{
    plutovg_path_line_to(pluto->path, x, y);
}

void plutovg_quad_to(plutovg_t* pluto, double x1, double y1, double x2, double y2)
{
    plutovg_path_quad_to(pluto->path, x1, y1, x2, y2);
}

void plutovg_cubic_to(plutovg_t* pluto, double x1, double y1, double x2, double y2, double x3, double y3)
{
    plutovg_path_cubic_to(pluto->path, x1, y1, x2, y2, x3, y3);
}

void plutovg_rel_move_to(plutovg_t* pluto, double x, double y)
{
    plutovg_path_rel_move_to(pluto->path, x, y);
}

void plutovg_rel_line_to(plutovg_t* pluto, double x, double y)
{
    plutovg_path_rel_line_to(pluto->path, x, y);
}

void plutovg_rel_quad_to(plutovg_t* pluto, double x1, double y1, double x2, double y2)
{
    plutovg_path_rel_quad_to(pluto->path, x1, y1, x2, y2);
}

void plutovg_rel_cubic_to(plutovg_t* pluto, double x1, double y1, double x2, double y2, double x3, double y3)
{
    plutovg_path_rel_cubic_to(pluto->path, x1, y1, x2, y2, x3, y3);
}

void plutovg_rect(plutovg_t* pluto, double x, double y, double w, double h)
{
    plutovg_path_add_rect(pluto->path, x, y, w, h);
}

void plutovg_round_rect(plutovg_t* pluto, double x, double y, double w, double h, double rx, double ry)
{
    plutovg_path_add_round_rect(pluto->path, x, y, w, h, rx, ry);
}

void plutovg_ellipse(plutovg_t* pluto, double cx, double cy, double rx, double ry)
{
    plutovg_path_add_ellipse(pluto->path, cx, cy, rx, ry);
}

void plutovg_circle(plutovg_t* pluto, double cx, double cy, double r)
{
    plutovg_ellipse(pluto, cx, cy, r, r);
}

void plutovg_add_path(plutovg_t* pluto, const plutovg_path_t* path)
{
    plutovg_path_add_path(pluto->path, path, NULL);
}

void plutovg_new_path(plutovg_t* pluto)
{
    plutovg_path_clear(pluto->path);
}

void plutovg_close_path(plutovg_t* pluto)
{
    plutovg_path_close(pluto->path);
}

plutovg_path_t* plutovg_get_path(const plutovg_t* pluto)
{
    return pluto->path;
}

void plutovg_fill(plutovg_t* pluto)
{
    plutovg_fill_preserve(pluto);
    plutovg_new_path(pluto);
}

void plutovg_stroke(plutovg_t* pluto)
{
    plutovg_stroke_preserve(pluto);
    plutovg_new_path(pluto);
}

void plutovg_clip(plutovg_t* pluto)
{
    plutovg_clip_preserve(pluto);
    plutovg_new_path(pluto);
}

void plutovg_paint(plutovg_t* pluto)
{
    plutovg_state_t* state = pluto->state;
    if (state->clippath == NULL && pluto->clippath == NULL)
    {
        plutovg_path_t* path = plutovg_path_create();
        plutovg_path_add_rect(path, pluto->clip.x, pluto->clip.y, pluto->clip.w, pluto->clip.h);
        plutovg_matrix_t matrix;
        plutovg_matrix_init_identity(&matrix);
        pluto->clippath = plutovg_rle_create();
        plutovg_rle_rasterize(pluto->clippath, path, &matrix, &pluto->clip, NULL, plutovg_fill_rule_non_zero);
        plutovg_path_destroy(path);
    }

    plutovg_rle_t* rle = state->clippath ? state->clippath : pluto->clippath;
    plutovg_blend(pluto, rle);
}

void plutovg_fill_preserve(plutovg_t* pluto)
{
    plutovg_state_t* state = pluto->state;
    plutovg_rle_clear(pluto->rle);
    plutovg_rle_rasterize(pluto->rle, pluto->path, &state->matrix, &pluto->clip, NULL, state->winding);
    plutovg_rle_clip_path(pluto->rle, state->clippath);
    plutovg_blend(pluto, pluto->rle);
}

void plutovg_stroke_preserve(plutovg_t* pluto)
{
    plutovg_state_t* state = pluto->state;
    plutovg_rle_clear(pluto->rle);
    plutovg_rle_rasterize(pluto->rle, pluto->path, &state->matrix, &pluto->clip, &state->stroke, plutovg_fill_rule_non_zero);
    plutovg_rle_clip_path(pluto->rle, state->clippath);
    plutovg_blend(pluto, pluto->rle);
}

void plutovg_clip_preserve(plutovg_t* pluto)
{
    plutovg_state_t* state = pluto->state;
    if (state->clippath)
    {
        plutovg_rle_clear(pluto->rle);
        plutovg_rle_rasterize(pluto->rle, pluto->path, &state->matrix, &pluto->clip, NULL, state->winding);
        plutovg_rle_clip_path(state->clippath, pluto->rle);
    }
    else
    {
        state->clippath = plutovg_rle_create();
        plutovg_rle_rasterize(state->clippath, pluto->path, &state->matrix, &pluto->clip, NULL, state->winding);
    }
}

void plutovg_reset_clip(plutovg_t* pluto)
{
    plutovg_rle_destroy(pluto->state->clippath);
    pluto->state->clippath = NULL;
}


#define COLOR_TABLE_SIZE 1024
typedef struct {
    plutovg_spread_method_t spread;
    plutovg_matrix_t matrix;
    uint32_t colortable[COLOR_TABLE_SIZE];
    union {
        struct {
            double x1, y1;
            double x2, y2;
        } linear;
        struct {
            double cx, cy, cr;
            double fx, fy, fr;
        } radial;
    };
} gradient_data_t;

typedef struct {
    plutovg_matrix_t matrix;
    uint8_t* data;
    int width;
    int height;
    int stride;
    int const_alpha;
} texture_data_t;

typedef struct {
    double dx;
    double dy;
    double l;
    double off;
} linear_gradient_values_t;

typedef struct {
    double dx;
    double dy;
    double dr;
    double sqrfr;
    double a;
    double inv2a;
    int extended;
} radial_gradient_values_t;

static inline uint32_t premultiply_color(const plutovg_color_t* color, double opacity)
{
    uint32_t alpha = (uint8_t)(color->a * opacity * 255);
    uint32_t pr = (uint8_t)(color->r * alpha);
    uint32_t pg = (uint8_t)(color->g * alpha);
    uint32_t pb = (uint8_t)(color->b * alpha);

    return (alpha << 24) | (pr << 16) | (pg << 8) | (pb);
}

static inline uint32_t combine_opacity(const plutovg_color_t* color, double opacity)
{
    uint32_t a = (uint8_t)(color->a * opacity * 255);
    uint32_t r = (uint8_t)(color->r * 255);
    uint32_t g = (uint8_t)(color->g * 255);
    uint32_t b = (uint8_t)(color->b * 255);

    return (a << 24) | (r << 16) | (g << 8) | (b);
}

static inline uint32_t premultiply_pixel(uint32_t color)
{
    uint32_t a = plutovg_alpha(color);
    uint32_t r = plutovg_red(color);
    uint32_t g = plutovg_green(color);
    uint32_t b = plutovg_blue(color);

    uint32_t pr = (r * a) / 255;
    uint32_t pg = (g * a) / 255;
    uint32_t pb = (b * a) / 255;
    return (a << 24) | (pr << 16) | (pg << 8) | (pb);
}

static inline uint32_t interpolate_pixel(uint32_t x, uint32_t a, uint32_t y, uint32_t b)
{
    uint32_t t = (x & 0xff00ff) * a + (y & 0xff00ff) * b;
    t = (t + ((t >> 8) & 0xff00ff) + 0x800080) >> 8;
    t &= 0xff00ff;
    x = ((x >> 8) & 0xff00ff) * a + ((y >> 8) & 0xff00ff) * b;
    x = (x + ((x >> 8) & 0xff00ff) + 0x800080);
    x &= 0xff00ff00;
    x |= t;
    return x;
}

static inline uint32_t BYTE_MUL(uint32_t x, uint32_t a)
{
    uint32_t t = (x & 0xff00ff) * a;
    t = (t + ((t >> 8) & 0xff00ff) + 0x800080) >> 8;
    t &= 0xff00ff;
    x = ((x >> 8) & 0xff00ff) * a;
    x = (x + ((x >> 8) & 0xff00ff) + 0x800080);
    x &= 0xff00ff00;
    x |= t;
    return x;
}

static inline void memfill32(uint32_t* dest, uint32_t value, int length)
{
    for (int i = 0;i < length;i++)
        dest[i] = value;
}

static inline int gradient_clamp(const gradient_data_t* gradient, int ipos)
{
    if (gradient->spread == plutovg_spread_method_repeat)
    {
        ipos = ipos % COLOR_TABLE_SIZE;
        ipos = ipos < 0 ? COLOR_TABLE_SIZE + ipos : ipos;
    }
    else if (gradient->spread == plutovg_spread_method_reflect)
    {
        const int limit = COLOR_TABLE_SIZE * 2;
        ipos = ipos % limit;
        ipos = ipos < 0 ? limit + ipos : ipos;
        ipos = ipos >= COLOR_TABLE_SIZE ? limit - 1 - ipos : ipos;
    }
    else
    {
        if (ipos < 0)
            ipos = 0;
        else if (ipos >= COLOR_TABLE_SIZE)
            ipos = COLOR_TABLE_SIZE - 1;
    }

    return ipos;
}

#define FIXPT_BITS 8
#define FIXPT_SIZE (1 << FIXPT_BITS)
static inline uint32_t gradient_pixel_fixed(const gradient_data_t* gradient, int fixed_pos)
{
    int ipos = (fixed_pos + (FIXPT_SIZE / 2)) >> FIXPT_BITS;
    return gradient->colortable[gradient_clamp(gradient, ipos)];
}

static inline uint32_t gradient_pixel(const gradient_data_t* gradient, double pos)
{
    int ipos = (int)(pos * (COLOR_TABLE_SIZE - 1) + 0.5);
    return gradient->colortable[gradient_clamp(gradient, ipos)];
}

static void fetch_linear_gradient(uint32_t* buffer, const linear_gradient_values_t* v, const gradient_data_t* gradient, int y, int x, int length)
{
    double t, inc;
    double rx = 0, ry = 0;

    if (v->l == 0.0)
    {
        t = inc = 0;
    }
    else
    {
        rx = gradient->matrix.m01 * (y + 0.5) + gradient->matrix.m00 * (x + 0.5) + gradient->matrix.m02;
        ry = gradient->matrix.m11 * (y + 0.5) + gradient->matrix.m10 * (x + 0.5) + gradient->matrix.m12;
        t = v->dx * rx + v->dy * ry + v->off;
        inc = v->dx * gradient->matrix.m00 + v->dy * gradient->matrix.m10;
        t *= (COLOR_TABLE_SIZE - 1);
        inc *= (COLOR_TABLE_SIZE - 1);
    }

    const uint32_t* end = buffer + length;
    if (inc > -1e-5 && inc < 1e-5)
    {
        memfill32(buffer, gradient_pixel_fixed(gradient, (int)(t * FIXPT_SIZE)), length);
    }
    else
    {
        if (t + inc * length < (double)(INT_MAX >> (FIXPT_BITS + 1)) && t + inc * length >(double)(INT_MIN >> (FIXPT_BITS + 1)))
        {
            int t_fixed = (int)(t * FIXPT_SIZE);
            int inc_fixed = (int)(inc * FIXPT_SIZE);
            while (buffer < end)
            {
                *buffer = gradient_pixel_fixed(gradient, t_fixed);
                t_fixed += inc_fixed;
                ++buffer;
            }
        }
        else
        {
            while (buffer < end)
            {
                *buffer = gradient_pixel(gradient, t / COLOR_TABLE_SIZE);
                t += inc;
                ++buffer;
            }
        }
    }
}

static void fetch_radial_gradient(uint32_t* buffer, const radial_gradient_values_t* v, const gradient_data_t* gradient, int y, int x, int length)
{
    if (v->a == 0.0)
    {
        memfill32(buffer, 0, length);
        return;
    }

    double rx = gradient->matrix.m01 * (y + 0.5) + gradient->matrix.m02 + gradient->matrix.m00 * (x + 0.5);
    double ry = gradient->matrix.m11 * (y + 0.5) + gradient->matrix.m12 + gradient->matrix.m10 * (x + 0.5);

    rx -= gradient->radial.fx;
    ry -= gradient->radial.fy;

    double inv_a = 1 / (2 * v->a);
    double delta_rx = gradient->matrix.m00;
    double delta_ry = gradient->matrix.m10;

    double b = 2 * (v->dr * gradient->radial.fr + rx * v->dx + ry * v->dy);
    double delta_b = 2 * (delta_rx * v->dx + delta_ry * v->dy);
    double b_delta_b = 2 * b * delta_b;
    double delta_b_delta_b = 2 * delta_b * delta_b;

    double bb = b * b;
    double delta_bb = delta_b * delta_b;

    b *= inv_a;
    delta_b *= inv_a;

    double rxrxryry = rx * rx + ry * ry;
    double delta_rxrxryry = delta_rx * delta_rx + delta_ry * delta_ry;
    double rx_plus_ry = 2 * (rx * delta_rx + ry * delta_ry);
    double delta_rx_plus_ry = 2 * delta_rxrxryry;

    inv_a *= inv_a;

    double det = (bb - 4 * v->a * (v->sqrfr - rxrxryry)) * inv_a;
    double delta_det = (b_delta_b + delta_bb + 4 * v->a * (rx_plus_ry + delta_rxrxryry)) * inv_a;
    double delta_delta_det = (delta_b_delta_b + 4 * v->a * delta_rx_plus_ry) * inv_a;

    const uint32_t* end = buffer + length;
    if (v->extended)
    {
        while (buffer < end)
        {
            uint32_t result = 0;
            if (det >= 0)
            {
                double w = sqrt(det) - b;
                if (gradient->radial.fr + v->dr * w >= 0)
                    result = gradient_pixel(gradient, w);
            }

            *buffer = result;
            det += delta_det;
            delta_det += delta_delta_det;
            b += delta_b;
            ++buffer;
        }
    }
    else
    {
        while (buffer < end)
        {
            *buffer++ = gradient_pixel(gradient, sqrt(det) - b);
            det += delta_det;
            delta_det += delta_delta_det;
            b += delta_b;
        }
    }
}

static void composition_solid_source(uint32_t* dest, int length, uint32_t color, uint32_t alpha)
{
    if (alpha == 255)
    {
        memfill32(dest, color, length);
    }
    else
    {
        uint32_t ialpha = 255 - alpha;
        color = BYTE_MUL(color, alpha);
        for (int i = 0;i < length;i++)
            dest[i] = color + BYTE_MUL(dest[i], ialpha);
    }
}

static void composition_solid_source_over(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
{
    if (const_alpha != 255) color = BYTE_MUL(color, const_alpha);
    uint32_t ialpha = 255 - plutovg_alpha(color);
    for (int i = 0;i < length;i++)
        dest[i] = color + BYTE_MUL(dest[i], ialpha);
}

static void composition_solid_destination_in(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
{
    uint32_t a = plutovg_alpha(color);
    if (const_alpha != 255) a = BYTE_MUL(a, const_alpha) + 255 - const_alpha;
    for (int i = 0;i < length;i++)
        dest[i] = BYTE_MUL(dest[i], a);
}

static void composition_solid_destination_out(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha)
{
    uint32_t a = plutovg_alpha(~color);
    if (const_alpha != 255) a = BYTE_MUL(a, const_alpha) + 255 - const_alpha;
    for (int i = 0; i < length;i++)
        dest[i] = BYTE_MUL(dest[i], a);
}

static void composition_source(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
{
    if (const_alpha == 255)
    {
        memcpy(dest, src, (size_t)(length) * sizeof(uint32_t));
    }
    else
    {
        uint32_t ialpha = 255 - const_alpha;
        for (int i = 0;i < length;i++)
            dest[i] = interpolate_pixel(src[i], const_alpha, dest[i], ialpha);
    }
}

static void composition_source_over(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
{
    uint32_t s, sia;
    if (const_alpha == 255)
    {
        for (int i = 0;i < length;i++)
        {
            s = src[i];
            if (s >= 0xff000000)
                dest[i] = s;
            else if (s != 0)
            {
                sia = plutovg_alpha(~s);
                dest[i] = s + BYTE_MUL(dest[i], sia);
            }
        }
    }
    else
    {
        for (int i = 0;i < length;i++)
        {
            s = BYTE_MUL(src[i], const_alpha);
            sia = plutovg_alpha(~s);
            dest[i] = s + BYTE_MUL(dest[i], sia);
        }
    }
}

static void composition_destination_in(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
{
    if (const_alpha == 255)
    {
        for (int i = 0; i < length;i++)
            dest[i] = BYTE_MUL(dest[i], plutovg_alpha(src[i]));
    }
    else
    {
        uint32_t cia = 255 - const_alpha;
        uint32_t a;
        for (int i = 0;i < length;i++)
        {
            a = BYTE_MUL(plutovg_alpha(src[i]), const_alpha) + cia;
            dest[i] = BYTE_MUL(dest[i], a);
        }
    }
}

static void composition_destination_out(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha)
{
    if (const_alpha == 255)
    {
        for (int i = 0;i < length;i++)
            dest[i] = BYTE_MUL(dest[i], plutovg_alpha(~src[i]));
    }
    else
    {
        uint32_t cia = 255 - const_alpha;
        uint32_t sia;
        for (int i = 0;i < length;i++)
        {
            sia = BYTE_MUL(plutovg_alpha(~src[i]), const_alpha) + cia;
            dest[i] = BYTE_MUL(dest[i], sia);
        }
    }
}

typedef void(*composition_solid_function_t)(uint32_t* dest, int length, uint32_t color, uint32_t const_alpha);
typedef void(*composition_function_t)(uint32_t* dest, int length, const uint32_t* src, uint32_t const_alpha);

static const composition_solid_function_t composition_solid_map[] = {
    composition_solid_source,
    composition_solid_source_over,
    composition_solid_destination_in,
    composition_solid_destination_out
};

static const composition_function_t composition_map[] = {
    composition_source,
    composition_source_over,
    composition_destination_in,
    composition_destination_out
};

static void blend_solid(plutovg_surface_t* surface, plutovg_operator_t op, const plutovg_rle_t* rle, uint32_t solid)
{
    composition_solid_function_t func = composition_solid_map[op];
    int count = rle->spans.size;
    const plutovg_span_t* spans = rle->spans.data;
    while (count--)
    {
        uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + spans->x;
        func(target, spans->len, solid, spans->coverage);
        ++spans;
    }
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define BUFFER_SIZE 1024
static void blend_linear_gradient(plutovg_surface_t* surface, plutovg_operator_t op, const plutovg_rle_t* rle, const gradient_data_t* gradient)
{
    composition_function_t func = composition_map[op];
    unsigned int buffer[BUFFER_SIZE];

    linear_gradient_values_t v;
    v.dx = gradient->linear.x2 - gradient->linear.x1;
    v.dy = gradient->linear.y2 - gradient->linear.y1;
    v.l = v.dx * v.dx + v.dy * v.dy;
    v.off = 0.0;
    if (v.l != 0.0)
    {
        v.dx /= v.l;
        v.dy /= v.l;
        v.off = -v.dx * gradient->linear.x1 - v.dy * gradient->linear.y1;
    }

    int count = rle->spans.size;
    const plutovg_span_t* spans = rle->spans.data;
    while (count--)
    {
        int length = spans->len;
        int x = spans->x;
        while (length)
        {
            int l = MIN(length, BUFFER_SIZE);
            fetch_linear_gradient(buffer, &v, gradient, spans->y, x, l);
            uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + x;
            func(target, l, buffer, spans->coverage);
            x += l;
            length -= l;
        }

        ++spans;
    }
}

static void blend_radial_gradient(plutovg_surface_t* surface, plutovg_operator_t op, const plutovg_rle_t* rle, const gradient_data_t* gradient)
{
    composition_function_t func = composition_map[op];
    unsigned int buffer[BUFFER_SIZE];

    radial_gradient_values_t v;
    v.dx = gradient->radial.cx - gradient->radial.fx;
    v.dy = gradient->radial.cy - gradient->radial.fy;
    v.dr = gradient->radial.cr - gradient->radial.fr;
    v.sqrfr = gradient->radial.fr * gradient->radial.fr;
    v.a = v.dr * v.dr - v.dx * v.dx - v.dy * v.dy;
    v.inv2a = 1.0 / (2.0 * v.a);
    v.extended = gradient->radial.fr != 0.0 || v.a <= 0.0;

    int count = rle->spans.size;
    const plutovg_span_t* spans = rle->spans.data;
    while (count--)
    {
        int length = spans->len;
        int x = spans->x;
        while (length)
        {
            int l = MIN(length, BUFFER_SIZE);
            fetch_radial_gradient(buffer, &v, gradient, spans->y, x, l);
            uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + x;
            func(target, l, buffer, spans->coverage);
            x += l;
            length -= l;
        }

        ++spans;
    }
}

#define CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : (hi) < (v) ? (hi) : (v))
#define FIXED_SCALE (1 << 16)
static void blend_transformed_argb(plutovg_surface_t* surface, plutovg_operator_t op, const plutovg_rle_t* rle, const texture_data_t* texture)
{
    composition_function_t func = composition_map[op];
    uint32_t buffer[BUFFER_SIZE];

    int image_width = texture->width;
    int image_height = texture->height;

    int fdx = (int)(texture->matrix.m00 * FIXED_SCALE);
    int fdy = (int)(texture->matrix.m10 * FIXED_SCALE);

    int count = rle->spans.size;
    const plutovg_span_t* spans = rle->spans.data;
    while (count--)
    {
        uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + spans->x;

        const double cx = spans->x + 0.5;
        const double cy = spans->y + 0.5;

        int x = (int)((texture->matrix.m01 * cy + texture->matrix.m00 * cx + texture->matrix.m02) * FIXED_SCALE);
        int y = (int)((texture->matrix.m11 * cy + texture->matrix.m10 * cx + texture->matrix.m12) * FIXED_SCALE);

        int length = spans->len;
        const int coverage = (spans->coverage * texture->const_alpha) >> 8;
        while (length)
        {
            int l = MIN(length, BUFFER_SIZE);
            const uint32_t* end = buffer + l;
            uint32_t* b = buffer;
            while (b < end)
            {
                int px = CLAMP(x >> 16, 0, image_width - 1);
                int py = CLAMP(y >> 16, 0, image_height - 1);
                *b = ((const uint32_t*)(texture->data + py * texture->stride))[px];

                x += fdx;
                y += fdy;
                ++b;
            }

            func(target, l, buffer, coverage);
            target += l;
            length -= l;
        }

        ++spans;
    }
}

static void blend_untransformed_argb(plutovg_surface_t* surface, plutovg_operator_t op, const plutovg_rle_t* rle, const texture_data_t* texture)
{
    composition_function_t func = composition_map[op];

    const int image_width = texture->width;
    const int image_height = texture->height;

    int xoff = (int)(texture->matrix.m02);
    int yoff = (int)(texture->matrix.m12);

    int count = rle->spans.size;
    const plutovg_span_t* spans = rle->spans.data;
    while (count--)
    {
        int x = spans->x;
        int length = spans->len;
        int sx = xoff + x;
        int sy = yoff + spans->y;
        if (sy >= 0 && sy < image_height && sx < image_width)
        {
            if (sx < 0)
            {
                x -= sx;
                length += sx;
                sx = 0;
            }
            if (sx + length > image_width) length = image_width - sx;
            if (length > 0)
            {
                const int coverage = (spans->coverage * texture->const_alpha) >> 8;
                const uint32_t* src = (const uint32_t*)(texture->data + sy * texture->stride) + sx;
                uint32_t* dest = (uint32_t*)(surface->data + spans->y * surface->stride) + x;
                func(dest, length, src, coverage);
            }
        }

        ++spans;
    }
}

static void blend_untransformed_tiled_argb(plutovg_surface_t* surface, plutovg_operator_t op, const plutovg_rle_t* rle, const texture_data_t* texture)
{
    composition_function_t func = composition_map[op];

    int image_width = texture->width;
    int image_height = texture->height;

    int xoff = (int)(texture->matrix.m02) % image_width;
    int yoff = (int)(texture->matrix.m12) % image_height;

    if (xoff < 0)
        xoff += image_width;
    if (yoff < 0)
        yoff += image_height;

    int count = rle->spans.size;
    const plutovg_span_t* spans = rle->spans.data;
    while (count--)
    {
        int x = spans->x;
        int length = spans->len;
        int sx = (xoff + spans->x) % image_width;
        int sy = (spans->y + yoff) % image_height;
        if (sx < 0)
            sx += image_width;
        if (sy < 0)
            sy += image_height;

        const int coverage = (spans->coverage * texture->const_alpha) >> 8;
        while (length)
        {
            int l = MIN(image_width - sx, length);
            if (BUFFER_SIZE < l)
                l = BUFFER_SIZE;
            const uint32_t* src = (const uint32_t*)(texture->data + sy * texture->stride) + sx;
            uint32_t* dest = (uint32_t*)(surface->data + spans->y * surface->stride) + x;
            func(dest, l, src, coverage);
            x += l;
            length -= l;
            sx = 0;
        }

        ++spans;
    }
}

static void blend_transformed_tiled_argb(plutovg_surface_t* surface, plutovg_operator_t op, const plutovg_rle_t* rle, const texture_data_t* texture)
{
    composition_function_t func = composition_map[op];
    uint32_t buffer[BUFFER_SIZE];

    int image_width = texture->width;
    int image_height = texture->height;
    const int scanline_offset = texture->stride / 4;

    int fdx = (int)(texture->matrix.m00 * FIXED_SCALE);
    int fdy = (int)(texture->matrix.m10 * FIXED_SCALE);

    int count = rle->spans.size;
    const plutovg_span_t* spans = rle->spans.data;
    while (count--)
    {
        uint32_t* target = (uint32_t*)(surface->data + spans->y * surface->stride) + spans->x;
        const uint32_t* image_bits = (const uint32_t*)texture->data;

        const double cx = spans->x + 0.5;
        const double cy = spans->y + 0.5;

        int x = (int)((texture->matrix.m01 * cy + texture->matrix.m00 * cx + texture->matrix.m02) * FIXED_SCALE);
        int y = (int)((texture->matrix.m11 * cy + texture->matrix.m10 * cx + texture->matrix.m12) * FIXED_SCALE);

        const int coverage = (spans->coverage * texture->const_alpha) >> 8;
        int length = spans->len;
        while (length)
        {
            int l = MIN(length, BUFFER_SIZE);
            const uint32_t* end = buffer + l;
            uint32_t* b = buffer;
            int px16 = x % (image_width << 16);
            int py16 = y % (image_height << 16);
            int px_delta = fdx % (image_width << 16);
            int py_delta = fdy % (image_height << 16);
            while (b < end)
            {
                if (px16 < 0) px16 += image_width << 16;
                if (py16 < 0) py16 += image_height << 16;
                int px = px16 >> 16;
                int py = py16 >> 16;
                int y_offset = py * scanline_offset;

                *b = image_bits[y_offset + px];
                x += fdx;
                y += fdy;
                px16 += px_delta;
                if (px16 >= image_width << 16)
                    px16 -= image_width << 16;
                py16 += py_delta;
                if (py16 >= image_height << 16)
                    py16 -= image_height << 16;
                ++b;
            }

            func(target, l, buffer, coverage);
            target += l;
            length -= l;
        }

        ++spans;
    }
}

void plutovg_blend(plutovg_t* pluto, const plutovg_rle_t* rle)
{
    plutovg_paint_t* source = pluto->state->source;
    if (source->type == plutovg_paint_type_color)
        plutovg_blend_color(pluto, rle, source->color);
    else if (source->type == plutovg_paint_type_gradient)
        plutovg_blend_gradient(pluto, rle, source->gradient);
    else
        plutovg_blend_texture(pluto, rle, source->texture);
}

void plutovg_blend_color(plutovg_t* pluto, const plutovg_rle_t* rle, const plutovg_color_t* color)
{
    plutovg_state_t* state = pluto->state;
    uint32_t solid = premultiply_color(color, state->opacity);

    uint32_t alpha = plutovg_alpha(solid);
    if (alpha == 255 && state->op == plutovg_operator_src_over)
        blend_solid(pluto->surface, plutovg_operator_src, rle, solid);
    else
        blend_solid(pluto->surface, state->op, rle, solid);
}

void plutovg_blend_gradient(plutovg_t* pluto, const plutovg_rle_t* rle, const plutovg_gradient_t* gradient)
{
    plutovg_state_t* state = pluto->state;
    gradient_data_t data;
    int i, pos = 0, nstop = gradient->stops.size;
    const plutovg_gradient_stop_t* curr, * next, * start, * last;
    uint32_t curr_color, next_color, last_color;
    uint32_t dist, idist;
    double delta, t, incr, fpos;
    double opacity = state->opacity * gradient->opacity;

    start = gradient->stops.data;
    curr = start;
    curr_color = combine_opacity(&curr->color, opacity);

    data.colortable[pos] = premultiply_pixel(curr_color);
    ++pos;
    incr = 1.0 / COLOR_TABLE_SIZE;
    fpos = 1.5 * incr;

    while (fpos <= curr->offset)
    {
        data.colortable[pos] = data.colortable[pos - 1];
        ++pos;
        fpos += incr;
    }

    for (i = 0;i < nstop - 1;i++)
    {
        curr = (start + i);
        next = (start + i + 1);
        delta = 1.0 / (next->offset - curr->offset);
        next_color = combine_opacity(&next->color, opacity);
        while (fpos < next->offset && pos < COLOR_TABLE_SIZE)
        {
            t = (fpos - curr->offset) * delta;
            dist = (uint32_t)(255 * t);
            idist = 255 - dist;
            data.colortable[pos] = premultiply_pixel(interpolate_pixel(curr_color, idist, next_color, dist));
            ++pos;
            fpos += incr;
        }

        curr_color = next_color;
    }

    last = start + nstop - 1;
    last_color = premultiply_color(&last->color, opacity);
    for (;pos < COLOR_TABLE_SIZE;++pos)
        data.colortable[pos] = last_color;

    data.spread = gradient->spread;
    data.matrix = gradient->matrix;
    plutovg_matrix_multiply(&data.matrix, &data.matrix, &state->matrix);
    plutovg_matrix_invert(&data.matrix);

    if (gradient->type == plutovg_gradient_type_linear)
    {
        data.linear.x1 = gradient->values[0];
        data.linear.y1 = gradient->values[1];
        data.linear.x2 = gradient->values[2];
        data.linear.y2 = gradient->values[3];
        blend_linear_gradient(pluto->surface, state->op, rle, &data);
    }
    else
    {
        data.radial.cx = gradient->values[0];
        data.radial.cy = gradient->values[1];
        data.radial.cr = gradient->values[2];
        data.radial.fx = gradient->values[3];
        data.radial.fy = gradient->values[4];
        data.radial.fr = gradient->values[5];
        blend_radial_gradient(pluto->surface, state->op, rle, &data);
    }
}

void plutovg_blend_texture(plutovg_t* pluto, const plutovg_rle_t* rle, const plutovg_texture_t* texture)
{
    plutovg_state_t* state = pluto->state;
    texture_data_t data;
    data.data = texture->surface->data;
    data.width = texture->surface->width;
    data.height = texture->surface->height;
    data.stride = texture->surface->stride;
    data.const_alpha = (int)(state->opacity * texture->opacity * 256.0);

    data.matrix = texture->matrix;
    plutovg_matrix_multiply(&data.matrix, &data.matrix, &state->matrix);
    plutovg_matrix_invert(&data.matrix);

    const plutovg_matrix_t* matrix = &data.matrix;
    int translating = (matrix->m00 == 1.0 && matrix->m10 == 0.0 && matrix->m01 == 0.0 && matrix->m11 == 1.0);
    if (translating)
    {
        if (texture->type == plutovg_texture_type_plain)
            blend_untransformed_argb(pluto->surface, state->op, rle, &data);
        else
            blend_untransformed_tiled_argb(pluto->surface, state->op, rle, &data);
    }
    else
    {
        if (texture->type == plutovg_texture_type_plain)
            blend_transformed_argb(pluto->surface, state->op, rle, &data);
        else
            blend_transformed_tiled_argb(pluto->surface, state->op, rle, &data);
    }
}


plutovg_dash_t* plutovg_dash_create(double offset, const double* data, int size)
{
    if (data == NULL || size == 0)
        return NULL;

    plutovg_dash_t* dash = (plutovg_dash_t*)malloc(sizeof(plutovg_dash_t));
    dash->offset = offset;
    dash->data = (double*)malloc((size_t)size * sizeof(double));
    dash->size = size;
    memcpy(dash->data, data, (size_t)size * sizeof(double));
    return dash;
}

plutovg_dash_t* plutovg_dash_clone(const plutovg_dash_t* dash)
{
    if (dash == NULL)
        return NULL;

    return plutovg_dash_create(dash->offset, dash->data, dash->size);
}

void plutovg_dash_destroy(plutovg_dash_t* dash)
{
    if (dash == NULL)
        return;

    free(dash->data);
    free(dash);
}

plutovg_path_t* plutovg_dash_path(const plutovg_dash_t* dash, const plutovg_path_t* path)
{
    if (dash->data == NULL || dash->size == 0)
        return plutovg_path_clone(path);

    int toggle = 1;
    int offset = 0;
    double phase = dash->offset;
    while (phase >= dash->data[offset])
    {
        toggle = !toggle;
        phase -= dash->data[offset];
        offset += 1;
        if (offset == dash->size) offset = 0;
    }

    plutovg_path_t* flat = plutovg_path_clone_flat(path);
    plutovg_path_t* result = plutovg_path_create();
    plutovg_array_ensure(result->elements, flat->elements.size, plutovg_path_element_t*);
    plutovg_array_ensure(result->points, flat->points.size, plutovg_point_t*);

    plutovg_path_element_t* elements = flat->elements.data;
    plutovg_path_element_t* end = elements + flat->elements.size;
    plutovg_point_t* points = flat->points.data;
    while (elements < end)
    {
        int itoggle = toggle;
        int ioffset = offset;
        double iphase = phase;

        double x0 = points->x;
        double y0 = points->y;

        if (itoggle)
            plutovg_path_move_to(result, x0, y0);

        ++elements;
        ++points;

        while (elements < end && *elements == plutovg_path_element_line_to)
        {
            double dx = points->x - x0;
            double dy = points->y - y0;
            double dist0 = sqrt(dx * dx + dy * dy);
            double dist1 = 0;

            while (dist0 - dist1 > dash->data[ioffset] - iphase)
            {
                dist1 += dash->data[ioffset] - iphase;
                double a = dist1 / dist0;
                double x = x0 + a * dx;
                double y = y0 + a * dy;

                if (itoggle)
                    plutovg_path_line_to(result, x, y);
                else
                    plutovg_path_move_to(result, x, y);

                itoggle = !itoggle;
                iphase = 0;
                ioffset += 1;
                if (ioffset == dash->size) ioffset = 0;
            }

            iphase += dist0 - dist1;

            x0 = points->x;
            y0 = points->y;

            if (itoggle)
                plutovg_path_line_to(result, x0, y0);

            ++elements;
            ++points;
        }
    }

    plutovg_path_destroy(flat);
    return result;
}


void plutovg_rect_init(plutovg_rect_t* rect, double x, double y, double w, double h)
{
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
}

void plutovg_rect_init_zero(plutovg_rect_t* rect)
{
    rect->x = 0.0;
    rect->y = 0.0;
    rect->w = 0.0;
    rect->h = 0.0;
}

void plutovg_matrix_init(plutovg_matrix_t* matrix, double m00, double m10, double m01, double m11, double m02, double m12)
{
    matrix->m00 = m00; matrix->m10 = m10;
    matrix->m01 = m01; matrix->m11 = m11;
    matrix->m02 = m02; matrix->m12 = m12;
}

void plutovg_matrix_init_identity(plutovg_matrix_t* matrix)
{
    matrix->m00 = 1.0; matrix->m10 = 0.0;
    matrix->m01 = 0.0; matrix->m11 = 1.0;
    matrix->m02 = 0.0; matrix->m12 = 0.0;
}

void plutovg_matrix_init_translate(plutovg_matrix_t* matrix, double x, double y)
{
    plutovg_matrix_init(matrix, 1.0, 0.0, 0.0, 1.0, x, y);
}

void plutovg_matrix_init_scale(plutovg_matrix_t* matrix, double x, double y)
{
    plutovg_matrix_init(matrix, x, 0.0, 0.0, y, 0.0, 0.0);
}

void plutovg_matrix_init_shear(plutovg_matrix_t* matrix, double x, double y)
{
    plutovg_matrix_init(matrix, 1.0, tan(y), tan(x), 1.0, 0.0, 0.0);
}

void plutovg_matrix_init_rotate(plutovg_matrix_t* matrix, double radians, double x, double y)
{
    double c = cos(radians);
    double s = sin(radians);

    double cx = x * (1 - c) + y * s;
    double cy = y * (1 - c) - x * s;

    plutovg_matrix_init(matrix, c, s, -s, c, cx, cy);
}

void plutovg_matrix_translate(plutovg_matrix_t* matrix, double x, double y)
{
    plutovg_matrix_t m;
    plutovg_matrix_init_translate(&m, x, y);
    plutovg_matrix_multiply(matrix, &m, matrix);
}

void plutovg_matrix_scale(plutovg_matrix_t* matrix, double x, double y)
{
    plutovg_matrix_t m;
    plutovg_matrix_init_scale(&m, x, y);
    plutovg_matrix_multiply(matrix, &m, matrix);
}

void plutovg_matrix_shear(plutovg_matrix_t* matrix, double x, double y)
{
    plutovg_matrix_t m;
    plutovg_matrix_init_shear(&m, x, y);
    plutovg_matrix_multiply(matrix, &m, matrix);
}

void plutovg_matrix_rotate(plutovg_matrix_t* matrix, double radians, double x, double y)
{
    plutovg_matrix_t m;
    plutovg_matrix_init_rotate(&m, radians, x, y);
    plutovg_matrix_multiply(matrix, &m, matrix);
}

void plutovg_matrix_multiply(plutovg_matrix_t* matrix, const plutovg_matrix_t* a, const plutovg_matrix_t* b)
{
    double m00 = a->m00 * b->m00 + a->m10 * b->m01;
    double m10 = a->m00 * b->m10 + a->m10 * b->m11;
    double m01 = a->m01 * b->m00 + a->m11 * b->m01;
    double m11 = a->m01 * b->m10 + a->m11 * b->m11;
    double m02 = a->m02 * b->m00 + a->m12 * b->m01 + b->m02;
    double m12 = a->m02 * b->m10 + a->m12 * b->m11 + b->m12;

    plutovg_matrix_init(matrix, m00, m10, m01, m11, m02, m12);
}

int plutovg_matrix_invert(plutovg_matrix_t* matrix)
{
    double det = (matrix->m00 * matrix->m11 - matrix->m10 * matrix->m01);
    if (det == 0.0)
        return 0;

    double inv_det = 1.0 / det;
    double m00 = matrix->m00 * inv_det;
    double m10 = matrix->m10 * inv_det;
    double m01 = matrix->m01 * inv_det;
    double m11 = matrix->m11 * inv_det;
    double m02 = (matrix->m01 * matrix->m12 - matrix->m11 * matrix->m02) * inv_det;
    double m12 = (matrix->m10 * matrix->m02 - matrix->m00 * matrix->m12) * inv_det;

    plutovg_matrix_init(matrix, m11, -m10, -m01, m00, m02, m12);
    return 1;
}

void plutovg_matrix_map(const plutovg_matrix_t* matrix, double x, double y, double* _x, double* _y)
{
    *_x = x * matrix->m00 + y * matrix->m01 + matrix->m02;
    *_y = x * matrix->m10 + y * matrix->m11 + matrix->m12;
}

void plutovg_matrix_map_point(const plutovg_matrix_t* matrix, const plutovg_point_t* src, plutovg_point_t* dst)
{
    plutovg_matrix_map(matrix, src->x, src->y, &dst->x, &dst->y);
}

void plutovg_matrix_map_rect(const plutovg_matrix_t* matrix, const plutovg_rect_t* src, plutovg_rect_t* dst)
{
    plutovg_point_t p[4];
    p[0].x = src->x;
    p[0].y = src->y;
    p[1].x = src->x + src->w;
    p[1].y = src->y;
    p[2].x = src->x + src->w;
    p[2].y = src->y + src->h;
    p[3].x = src->x;
    p[3].y = src->y + src->h;

    plutovg_matrix_map_point(matrix, &p[0], &p[0]);
    plutovg_matrix_map_point(matrix, &p[1], &p[1]);
    plutovg_matrix_map_point(matrix, &p[2], &p[2]);
    plutovg_matrix_map_point(matrix, &p[3], &p[3]);

    double l = p[0].x;
    double t = p[0].y;
    double r = p[0].x;
    double b = p[0].y;

    for (int i = 0;i < 4;i++)
    {
        if (p[i].x < l) l = p[i].x;
        if (p[i].x > r) r = p[i].x;
        if (p[i].y < t) t = p[i].y;
        if (p[i].y > b) b = p[i].y;
    }

    dst->x = l;
    dst->y = t;
    dst->w = r - l;
    dst->h = b - t;
}

plutovg_path_t* plutovg_path_create(void)
{
    plutovg_path_t* path = (plutovg_path_t*)malloc(sizeof(plutovg_path_t));
    path->ref = 1;
    path->contours = 0;
    path->start.x = 0.0;
    path->start.y = 0.0;
    plutovg_array_init(path->elements);
    plutovg_array_init(path->points);
    return path;
}

plutovg_path_t* plutovg_path_reference(plutovg_path_t* path)
{
    ++path->ref;
    return path;
}

void plutovg_path_destroy(plutovg_path_t* path)
{
    if (path == NULL)
        return;

    if (--path->ref == 0)
    {
        free(path->elements.data);
        free(path->points.data);
        free(path);
    }
}

int plutovg_path_get_reference_count(const plutovg_path_t* path)
{
    return path->ref;
}

void plutovg_path_move_to(plutovg_path_t* path, double x, double y)
{
    plutovg_array_ensure(path->elements, 1, plutovg_path_element_t*);
    plutovg_array_ensure(path->points, 1, plutovg_point_t*);

    path->elements.data[path->elements.size] = plutovg_path_element_move_to;
    plutovg_point_t* points = path->points.data + path->points.size;
    points[0].x = x;
    points[0].y = y;

    path->elements.size += 1;
    path->points.size += 1;
    path->contours += 1;

    path->start.x = x;
    path->start.y = y;
}

void plutovg_path_line_to(plutovg_path_t* path, double x, double y)
{
    plutovg_array_ensure(path->elements, 1, plutovg_path_element_t*);
    plutovg_array_ensure(path->points, 1, plutovg_point_t*);

    path->elements.data[path->elements.size] = plutovg_path_element_line_to;
    plutovg_point_t* points = path->points.data + path->points.size;
    points[0].x = x;
    points[0].y = y;

    path->elements.size += 1;
    path->points.size += 1;
}

void plutovg_path_quad_to(plutovg_path_t* path, double x1, double y1, double x2, double y2)
{
    double x, y;
    plutovg_path_get_current_point(path, &x, &y);

    double cx = 2.0 / 3.0 * x1 + 1.0 / 3.0 * x;
    double cy = 2.0 / 3.0 * y1 + 1.0 / 3.0 * y;
    double cx1 = 2.0 / 3.0 * x1 + 1.0 / 3.0 * x2;
    double cy1 = 2.0 / 3.0 * y1 + 1.0 / 3.0 * y2;
    plutovg_path_cubic_to(path, cx, cy, cx1, cy1, x2, y2);
}

void plutovg_path_cubic_to(plutovg_path_t* path, double x1, double y1, double x2, double y2, double x3, double y3)
{
    plutovg_array_ensure(path->elements, 1, plutovg_path_element_t*);
    plutovg_array_ensure(path->points, 3, plutovg_point_t*);

    path->elements.data[path->elements.size] = plutovg_path_element_cubic_to;
    plutovg_point_t* points = path->points.data + path->points.size;
    points[0].x = x1;
    points[0].y = y1;
    points[1].x = x2;
    points[1].y = y2;
    points[2].x = x3;
    points[2].y = y3;

    path->elements.size += 1;
    path->points.size += 3;
}

void plutovg_path_close(plutovg_path_t* path)
{
    if (path->elements.size == 0)
        return;

    if (path->elements.data[path->elements.size - 1] == plutovg_path_element_close)
        return;

    plutovg_array_ensure(path->elements, 1, plutovg_path_element_t*);
    plutovg_array_ensure(path->points, 1, plutovg_point_t*);

    path->elements.data[path->elements.size] = plutovg_path_element_close;
    plutovg_point_t* points = path->points.data + path->points.size;
    points[0].x = path->start.x;
    points[0].y = path->start.y;

    path->elements.size += 1;
    path->points.size += 1;
}

static inline void rel_to_abs(const plutovg_path_t* path, double* x, double* y)
{
    double _x, _y;
    plutovg_path_get_current_point(path, &_x, &_y);

    *x += _x;
    *y += _y;
}

void plutovg_path_rel_move_to(plutovg_path_t* path, double x, double y)
{
    rel_to_abs(path, &x, &y);
    plutovg_path_move_to(path, x, y);
}

void plutovg_path_rel_line_to(plutovg_path_t* path, double x, double y)
{
    rel_to_abs(path, &x, &y);
    plutovg_path_line_to(path, x, y);
}

void plutovg_path_rel_quad_to(plutovg_path_t* path, double x1, double y1, double x2, double y2)
{
    rel_to_abs(path, &x1, &y1);
    rel_to_abs(path, &x2, &y2);
    plutovg_path_quad_to(path, x1, y1, x2, y2);
}

void plutovg_path_rel_cubic_to(plutovg_path_t* path, double x1, double y1, double x2, double y2, double x3, double y3)
{
    rel_to_abs(path, &x1, &y1);
    rel_to_abs(path, &x2, &y2);
    rel_to_abs(path, &x3, &y3);
    plutovg_path_cubic_to(path, x1, y1, x2, y2, x3, y3);
}

void plutovg_path_add_rect(plutovg_path_t* path, double x, double y, double w, double h)
{
    plutovg_path_move_to(path, x, y);
    plutovg_path_line_to(path, x + w, y);
    plutovg_path_line_to(path, x + w, y + h);
    plutovg_path_line_to(path, x, y + h);
    plutovg_path_line_to(path, x, y);
    plutovg_path_close(path);
}

#define KAPPA 0.5522847498
void plutovg_path_add_round_rect(plutovg_path_t* path, double x, double y, double w, double h, double rx, double ry)
{
    double right = x + w;
    double bottom = y + h;

    double cpx = rx * KAPPA;
    double cpy = ry * KAPPA;

    plutovg_path_move_to(path, x, y + ry);
    plutovg_path_cubic_to(path, x, y + ry - cpy, x + rx - cpx, y, x + rx, y);
    plutovg_path_line_to(path, right - rx, y);
    plutovg_path_cubic_to(path, right - rx + cpx, y, right, y + ry - cpy, right, y + ry);
    plutovg_path_line_to(path, right, bottom - ry);
    plutovg_path_cubic_to(path, right, bottom - ry + cpy, right - rx + cpx, bottom, right - rx, bottom);
    plutovg_path_line_to(path, x + rx, bottom);
    plutovg_path_cubic_to(path, x + rx - cpx, bottom, x, bottom - ry + cpy, x, bottom - ry);
    plutovg_path_line_to(path, x, y + ry);
    plutovg_path_close(path);
}

void plutovg_path_add_ellipse(plutovg_path_t* path, double cx, double cy, double rx, double ry)
{
    double left = cx - rx;
    double top = cy - ry;
    double right = cx + rx;
    double bottom = cy + ry;

    double cpx = rx * KAPPA;
    double cpy = ry * KAPPA;

    plutovg_path_move_to(path, cx, top);
    plutovg_path_cubic_to(path, cx + cpx, top, right, cy - cpy, right, cy);
    plutovg_path_cubic_to(path, right, cy + cpy, cx + cpx, bottom, cx, bottom);
    plutovg_path_cubic_to(path, cx - cpx, bottom, left, cy + cpy, left, cy);
    plutovg_path_cubic_to(path, left, cy - cpy, cx - cpx, top, cx, top);
    plutovg_path_close(path);
}

void plutovg_path_add_circle(plutovg_path_t* path, double cx, double cy, double r)
{
    plutovg_path_add_ellipse(path, cx, cy, r, r);
}

void plutovg_path_add_path(plutovg_path_t* path, const plutovg_path_t* source, const plutovg_matrix_t* matrix)
{
    plutovg_array_ensure(path->elements, source->elements.size, plutovg_path_element_t*);
    plutovg_array_ensure(path->points, source->points.size, plutovg_point_t*);

    plutovg_point_t* points = path->points.data + path->points.size;
    const plutovg_point_t* data = source->points.data;
    const plutovg_point_t* end = data + source->points.size;
    while (data < end)
    {
        if (matrix)
            plutovg_matrix_map_point(matrix, data, points);
        else
            memcpy(points, data, sizeof(plutovg_point_t));

        points += 1;
        data += 1;
    }

    plutovg_path_element_t* elements = path->elements.data + path->elements.size;
    memcpy(elements, source->elements.data, (size_t)source->elements.size * sizeof(plutovg_path_element_t));

    path->elements.size += source->elements.size;
    path->points.size += source->points.size;
    path->contours += source->contours;
    path->start = source->start;
}

void plutovg_path_transform(plutovg_path_t* path, const plutovg_matrix_t* matrix)
{
    plutovg_point_t* points = path->points.data;
    plutovg_point_t* end = points + path->points.size;
    while (points < end)
    {
        plutovg_matrix_map_point(matrix, points, points);
        points += 1;
    }
}

void plutovg_path_get_current_point(const plutovg_path_t* path, double* x, double* y)
{
    *x = 0.0;
    *y = 0.0;

    if (path->points.size == 0)
        return;

    *x = path->points.data[path->points.size - 1].x;
    *y = path->points.data[path->points.size - 1].y;
}

int plutovg_path_get_element_count(const plutovg_path_t* path)
{
    return path->elements.size;
}

plutovg_path_element_t* plutovg_path_get_elements(const plutovg_path_t* path)
{
    return path->elements.data;
}

int plutovg_path_get_point_count(const plutovg_path_t* path)
{
    return path->points.size;
}

plutovg_point_t* plutovg_path_get_points(const plutovg_path_t* path)
{
    return path->points.data;
}

void plutovg_path_clear(plutovg_path_t* path)
{
    path->elements.size = 0;
    path->points.size = 0;
    path->contours = 0;
    path->start.x = 0.0;
    path->start.y = 0.0;
}

int plutovg_path_empty(const plutovg_path_t* path)
{
    return path->elements.size == 0;
}

plutovg_path_t* plutovg_path_clone(const plutovg_path_t* path)
{
    plutovg_path_t* result = plutovg_path_create();
    plutovg_array_ensure(result->elements, path->elements.size, plutovg_path_element_t*);
    plutovg_array_ensure(result->points, path->points.size, plutovg_point_t*);

    memcpy(result->elements.data, path->elements.data, (size_t)path->elements.size * sizeof(plutovg_path_element_t));
    memcpy(result->points.data, path->points.data, (size_t)path->points.size * sizeof(plutovg_point_t));

    result->elements.size = path->elements.size;
    result->points.size = path->points.size;
    result->contours = path->contours;
    result->start = path->start;
    return result;
}

typedef struct {
    double x1; double y1;
    double x2; double y2;
    double x3; double y3;
    double x4; double y4;
} bezier_t;

static inline void split(const bezier_t* b, bezier_t* first, bezier_t* second)
{
    double c = (b->x2 + b->x3) * 0.5;
    first->x2 = (b->x1 + b->x2) * 0.5;
    second->x3 = (b->x3 + b->x4) * 0.5;
    first->x1 = b->x1;
    second->x4 = b->x4;
    first->x3 = (first->x2 + c) * 0.5;
    second->x2 = (second->x3 + c) * 0.5;
    first->x4 = second->x1 = (first->x3 + second->x2) * 0.5;

    c = (b->y2 + b->y3) * 0.5;
    first->y2 = (b->y1 + b->y2) * 0.5;
    second->y3 = (b->y3 + b->y4) * 0.5;
    first->y1 = b->y1;
    second->y4 = b->y4;
    first->y3 = (first->y2 + c) * 0.5;
    second->y2 = (second->y3 + c) * 0.5;
    first->y4 = second->y1 = (first->y3 + second->y2) * 0.5;
}

static void flatten(plutovg_path_t* path, const plutovg_point_t* p0, const plutovg_point_t* p1, const plutovg_point_t* p2, const plutovg_point_t* p3)
{
    bezier_t beziers[32];
    beziers[0].x1 = p0->x;
    beziers[0].y1 = p0->y;
    beziers[0].x2 = p1->x;
    beziers[0].y2 = p1->y;
    beziers[0].x3 = p2->x;
    beziers[0].y3 = p2->y;
    beziers[0].x4 = p3->x;
    beziers[0].y4 = p3->y;

    const double threshold = 0.25;

    bezier_t* b = beziers;
    while (b >= beziers)
    {
        double y4y1 = b->y4 - b->y1;
        double x4x1 = b->x4 - b->x1;
        double l = fabs(x4x1) + fabs(y4y1);
        double d;
        if (l > 1.0)
        {
            d = fabs((x4x1) * (b->y1 - b->y2) - (y4y1) * (b->x1 - b->x2)) + fabs((x4x1) * (b->y1 - b->y3) - (y4y1) * (b->x1 - b->x3));
        }
        else
        {
            d = fabs(b->x1 - b->x2) + fabs(b->y1 - b->y2) + fabs(b->x1 - b->x3) + fabs(b->y1 - b->y3);
            l = 1.0;
        }

        if (d < threshold * l || b == beziers + 31)
        {
            plutovg_path_line_to(path, b->x4, b->y4);
            --b;
        }
        else
        {
            split(b, b + 1, b);
            ++b;
        }
    }
}

plutovg_path_t* plutovg_path_clone_flat(const plutovg_path_t* path)
{
    plutovg_path_t* result = plutovg_path_create();
    plutovg_array_ensure(result->elements, path->elements.size, plutovg_path_element_t*);
    plutovg_array_ensure(result->points, path->points.size, plutovg_point_t*);
    plutovg_point_t* points = path->points.data;

    for (int i = 0;i < path->elements.size;i++)
    {
        switch (path->elements.data[i])
        {
        case plutovg_path_element_move_to:
            plutovg_path_move_to(result, points[0].x, points[0].y);
            points += 1;
            break;
        case plutovg_path_element_line_to:
            plutovg_path_line_to(result, points[0].x, points[0].y);
            points += 1;
            break;
        case plutovg_path_element_close:
            plutovg_path_line_to(result, points[0].x, points[0].y);
            points += 1;
            break;
        case plutovg_path_element_cubic_to:
        {
            plutovg_point_t p0;
            plutovg_path_get_current_point(result, &p0.x, &p0.y);
            flatten(result, &p0, points, points + 1, points + 2);
            points += 3;
            break;
        }
        }
    }

    return result;
}


void plutovg_color_init_rgb(plutovg_color_t* color, double r, double g, double b)
{
    plutovg_color_init_rgba(color, r, g, b, 1.0);
}

void plutovg_color_init_rgba(plutovg_color_t* color, double r, double g, double b, double a)
{
    color->r = r;
    color->g = g;
    color->b = b;
    color->a = a;
}

plutovg_gradient_t* plutovg_gradient_create_linear(double x1, double y1, double x2, double y2)
{
    plutovg_gradient_t* gradient = (plutovg_gradient_t*)malloc(sizeof(plutovg_gradient_t));
    gradient->ref = 1;
    gradient->type = plutovg_gradient_type_linear;
    gradient->spread = plutovg_spread_method_pad;
    gradient->opacity = 1.0;
    plutovg_array_init(gradient->stops);
    plutovg_matrix_init_identity(&gradient->matrix);
    plutovg_gradient_set_values_linear(gradient, x1, y1, x2, y2);
    return gradient;
}

plutovg_gradient_t* plutovg_gradient_create_radial(double cx, double cy, double cr, double fx, double fy, double fr)
{
    plutovg_gradient_t* gradient = (plutovg_gradient_t*)malloc(sizeof(plutovg_gradient_t));
    gradient->ref = 1;
    gradient->type = plutovg_gradient_type_radial;
    gradient->spread = plutovg_spread_method_pad;
    gradient->opacity = 1.0;
    plutovg_array_init(gradient->stops);
    plutovg_matrix_init_identity(&gradient->matrix);
    plutovg_gradient_set_values_radial(gradient, cx, cy, cr, fx, fy, fr);
    return gradient;
}

plutovg_gradient_t* plutovg_gradient_reference(plutovg_gradient_t* gradient)
{
    ++gradient->ref;
    return gradient;
}

void plutovg_gradient_destroy(plutovg_gradient_t* gradient)
{
    if (gradient == NULL)
        return;

    if (--gradient->ref == 0)
    {
        free(gradient->stops.data);
        free(gradient);
    }
}

int plutovg_gradient_get_reference_count(const plutovg_gradient_t* gradient)
{
    return gradient->ref;
}

void plutovg_gradient_set_type(plutovg_gradient_t* gradient, plutovg_gradient_type_t type)
{
    gradient->type = type;
}

plutovg_gradient_type_t plutovg_gradient_get_type(const plutovg_gradient_t* gradient)
{
    return gradient->type;
}

void plutovg_gradient_set_spread(plutovg_gradient_t* gradient, plutovg_spread_method_t spread)
{
    gradient->spread = spread;
}

plutovg_spread_method_t plutovg_gradient_get_spread(const plutovg_gradient_t* gradient)
{
    return gradient->spread;
}

void plutovg_gradient_set_matrix(plutovg_gradient_t* gradient, const plutovg_matrix_t* matrix)
{
    gradient->matrix = *matrix;
}

void plutovg_gradient_get_matrix(const plutovg_gradient_t* gradient, plutovg_matrix_t* matrix)
{
    *matrix = gradient->matrix;
}

void plutovg_gradient_add_stop_rgb(plutovg_gradient_t* gradient, double offset, double r, double g, double b)
{
    plutovg_gradient_add_stop_rgba(gradient, offset, r, g, b, 1.0);
}

void plutovg_gradient_add_stop_rgba(plutovg_gradient_t* gradient, double offset, double r, double g, double b, double a)
{
    plutovg_array_ensure(gradient->stops, 1, plutovg_gradient_stop_t*);
    plutovg_gradient_stop_t* stops = gradient->stops.data;
    int nstops = gradient->stops.size;
    int i;
    for (i = 0;i < nstops;i++)
    {
        if (offset < stops[i].offset)
        {
            memmove(&stops[i + 1], &stops[i], (size_t)(nstops - i) * sizeof(plutovg_gradient_stop_t));
            break;
        }
    }

    stops[i].offset = offset;
    stops[i].color.r = r;
    stops[i].color.g = g;
    stops[i].color.b = b;
    stops[i].color.a = a;

    gradient->stops.size++;
}

void plutovg_gradient_add_stop(plutovg_gradient_t* gradient, const plutovg_gradient_stop_t* stop)
{
    plutovg_gradient_add_stop_rgba(gradient, stop->offset, stop->color.r, stop->color.g, stop->color.b, stop->color.a);
}

void plutovg_gradient_clear_stops(plutovg_gradient_t* gradient)
{
    gradient->stops.size = 0;
}

int plutovg_gradient_get_stop_count(const plutovg_gradient_t* gradient)
{
    return gradient->stops.size;
}

plutovg_gradient_stop_t* plutovg_gradient_get_stops(const plutovg_gradient_t* gradient)
{
    return gradient->stops.data;
}

void plutovg_gradient_get_values_linear(const plutovg_gradient_t* gradient, double* x1, double* y1, double* x2, double* y2)
{
    *x1 = gradient->values[0];
    *y1 = gradient->values[1];
    *x2 = gradient->values[2];
    *y2 = gradient->values[3];
}

void plutovg_gradient_get_values_radial(const plutovg_gradient_t* gradient, double* cx, double* cy, double* cr, double* fx, double* fy, double* fr)
{
    *cx = gradient->values[0];
    *cy = gradient->values[1];
    *cr = gradient->values[2];
    *fx = gradient->values[3];
    *fy = gradient->values[4];
    *fr = gradient->values[5];
}

void plutovg_gradient_set_values_linear(plutovg_gradient_t* gradient, double x1, double y1, double x2, double y2)
{
    gradient->values[0] = x1;
    gradient->values[1] = y1;
    gradient->values[2] = x2;
    gradient->values[3] = y2;
}

void plutovg_gradient_set_values_radial(plutovg_gradient_t* gradient, double cx, double cy, double cr, double fx, double fy, double fr)
{
    gradient->values[0] = cx;
    gradient->values[1] = cy;
    gradient->values[2] = cr;
    gradient->values[3] = fx;
    gradient->values[4] = fy;
    gradient->values[5] = fr;
}

void plutovg_gradient_set_opacity(plutovg_gradient_t* gradient, double opacity)
{
    gradient->opacity = opacity;
}

double plutovg_gradient_get_opacity(const plutovg_gradient_t* gradient)
{
    return gradient->opacity;
}

plutovg_texture_t* plutovg_texture_create(plutovg_surface_t* surface)
{
    plutovg_texture_t* texture = (plutovg_texture_t*)malloc(sizeof(plutovg_texture_t));
    texture->ref = 1;
    texture->type = plutovg_texture_type_plain;
    texture->surface = plutovg_surface_reference(surface);
    texture->opacity = 1.0;
    plutovg_matrix_init_identity(&texture->matrix);
    return texture;
}

plutovg_texture_t* plutovg_texture_reference(plutovg_texture_t* texture)
{
    ++texture->ref;
    return texture;
}

void plutovg_texture_destroy(plutovg_texture_t* texture)
{
    if (texture == NULL)
        return;

    if (--texture->ref == 0)
    {
        plutovg_surface_destroy(texture->surface);
        free(texture);
    }
}

int plutovg_texture_get_reference_count(const plutovg_texture_t* texture)
{
    return texture->ref;
}

void plutovg_texture_set_type(plutovg_texture_t* texture, plutovg_texture_type_t type)
{
    texture->type = type;
}

plutovg_texture_type_t plutovg_texture_get_type(const plutovg_texture_t* texture)
{
    return texture->type;
}

void plutovg_texture_set_matrix(plutovg_texture_t* texture, const plutovg_matrix_t* matrix)
{
    memcpy(&texture->matrix, matrix, sizeof(plutovg_matrix_t));
}

void plutovg_texture_get_matrix(const plutovg_texture_t* texture, plutovg_matrix_t* matrix)
{
    memcpy(matrix, &texture->matrix, sizeof(plutovg_matrix_t));
}

void plutovg_texture_set_surface(plutovg_texture_t* texture, plutovg_surface_t* surface)
{
    surface = plutovg_surface_reference(surface);
    plutovg_surface_destroy(texture->surface);
    texture->surface = surface;
}

plutovg_surface_t* plutovg_texture_get_surface(const plutovg_texture_t* texture)
{
    return texture->surface;
}

void plutovg_texture_set_opacity(plutovg_texture_t* texture, double opacity)
{
    texture->opacity = opacity;
}

double plutovg_texture_get_opacity(const plutovg_texture_t* texture)
{
    return texture->opacity;
}

plutovg_paint_t* plutovg_paint_create_rgb(double r, double g, double b)
{
    return plutovg_paint_create_rgba(r, g, b, 1.0);
}

plutovg_paint_t* plutovg_paint_create_rgba(double r, double g, double b, double a)
{
    plutovg_paint_t* paint = (plutovg_paint_t*)malloc(sizeof(plutovg_paint_t));
    paint->ref = 1;
    paint->type = plutovg_paint_type_color;
    paint->color = (plutovg_color_t*)malloc(sizeof(plutovg_color_t));
    plutovg_color_init_rgba(paint->color, r, g, b, a);
    return paint;
}

plutovg_paint_t* plutovg_paint_create_linear(double x1, double y1, double x2, double y2)
{
    plutovg_gradient_t* gradient = plutovg_gradient_create_linear(x1, y1, x2, y2);
    plutovg_paint_t* paint = plutovg_paint_create_gradient(gradient);
    plutovg_gradient_destroy(gradient);
    return paint;
}

plutovg_paint_t* plutovg_paint_create_radial(double cx, double cy, double cr, double fx, double fy, double fr)
{
    plutovg_gradient_t* gradient = plutovg_gradient_create_radial(cx, cy, cr, fx, fy, fr);
    plutovg_paint_t* paint = plutovg_paint_create_gradient(gradient);
    plutovg_gradient_destroy(gradient);
    return paint;
}

plutovg_paint_t* plutovg_paint_create_for_surface(plutovg_surface_t* surface)
{
    plutovg_texture_t* texture = plutovg_texture_create(surface);
    plutovg_paint_t* paint = plutovg_paint_create_texture(texture);
    plutovg_texture_destroy(texture);
    return paint;
}

plutovg_paint_t* plutovg_paint_create_color(const plutovg_color_t* color)
{
    return plutovg_paint_create_rgba(color->r, color->g, color->b, color->a);
}

plutovg_paint_t* plutovg_paint_create_gradient(plutovg_gradient_t* gradient)
{
    plutovg_paint_t* paint = (plutovg_paint_t*)malloc(sizeof(plutovg_paint_t));
    paint->ref = 1;
    paint->type = plutovg_paint_type_gradient;
    paint->gradient = plutovg_gradient_reference(gradient);
    return paint;
}

plutovg_paint_t* plutovg_paint_create_texture(plutovg_texture_t* texture)
{
    plutovg_paint_t* paint = (plutovg_paint_t*)malloc(sizeof(plutovg_paint_t));
    paint->ref = 1;
    paint->type = plutovg_paint_type_texture;
    paint->texture = plutovg_texture_reference(texture);
    return paint;
}

plutovg_paint_t* plutovg_paint_reference(plutovg_paint_t* paint)
{
    ++paint->ref;
    return paint;
}

void plutovg_paint_destroy(plutovg_paint_t* paint)
{
    if (paint == NULL)
        return;

    if (--paint->ref == 0)
    {
        if (paint->type == plutovg_paint_type_color)
            free(paint->color);
        if (paint->type == plutovg_paint_type_gradient)
            plutovg_gradient_destroy(paint->gradient);
        if (paint->type == plutovg_paint_type_texture)
            plutovg_texture_destroy(paint->texture);
        free(paint);
    }
}

int plutovg_paint_get_reference_count(const plutovg_paint_t* paint)
{
    return paint->ref;
}

plutovg_paint_type_t plutovg_paint_get_type(const plutovg_paint_t* paint)
{
    return paint->type;
}

plutovg_color_t* plutovg_paint_get_color(const plutovg_paint_t* paint)
{
    return paint->type == plutovg_paint_type_color ? paint->color : NULL;
}

plutovg_gradient_t* plutovg_paint_get_gradient(const plutovg_paint_t* paint)
{
    return paint->type == plutovg_paint_type_gradient ? paint->gradient : NULL;
}

plutovg_texture_t* plutovg_paint_get_texture(const plutovg_paint_t* paint)
{
    return paint->type == plutovg_paint_type_texture ? paint->texture : NULL;
}


static SW_FT_Outline* sw_ft_outline_create(int points, int contours)
{
    SW_FT_Outline* ft = (SW_FT_Outline*)malloc(sizeof(SW_FT_Outline));
    ft->points = (SW_FT_Vector*)malloc((size_t)(points + contours) * sizeof(SW_FT_Vector));
    ft->tags = (char*)malloc((size_t)(points + contours) * sizeof(char));
    ft->contours = (short*)malloc((size_t)contours * sizeof(short));
    ft->contours_flag = (char*)malloc((size_t)contours * sizeof(char));
    ft->n_points = ft->n_contours = 0;
    ft->flags = 0x0;
    return ft;
}

static void sw_ft_outline_destroy(SW_FT_Outline* ft)
{
    free(ft->points);
    free(ft->tags);
    free(ft->contours);
    free(ft->contours_flag);
    free(ft);
}

#define FT_COORD(x) (SW_FT_Pos)((x) * 64)
static void sw_ft_outline_move_to(SW_FT_Outline* ft, double x, double y)
{
    ft->points[ft->n_points].x = FT_COORD(x);
    ft->points[ft->n_points].y = FT_COORD(y);
    ft->tags[ft->n_points] = SW_FT_CURVE_TAG_ON;
    if (ft->n_points)
    {
        ft->contours[ft->n_contours] = ft->n_points - 1;
        ft->n_contours++;
    }

    ft->contours_flag[ft->n_contours] = 1;
    ft->n_points++;
}

static void sw_ft_outline_line_to(SW_FT_Outline* ft, double x, double y)
{
    ft->points[ft->n_points].x = FT_COORD(x);
    ft->points[ft->n_points].y = FT_COORD(y);
    ft->tags[ft->n_points] = SW_FT_CURVE_TAG_ON;
    ft->n_points++;
}

static void sw_ft_outline_cubic_to(SW_FT_Outline* ft, double x1, double y1, double x2, double y2, double x3, double y3)
{
    ft->points[ft->n_points].x = FT_COORD(x1);
    ft->points[ft->n_points].y = FT_COORD(y1);
    ft->tags[ft->n_points] = SW_FT_CURVE_TAG_CUBIC;
    ft->n_points++;

    ft->points[ft->n_points].x = FT_COORD(x2);
    ft->points[ft->n_points].y = FT_COORD(y2);
    ft->tags[ft->n_points] = SW_FT_CURVE_TAG_CUBIC;
    ft->n_points++;

    ft->points[ft->n_points].x = FT_COORD(x3);
    ft->points[ft->n_points].y = FT_COORD(y3);
    ft->tags[ft->n_points] = SW_FT_CURVE_TAG_ON;
    ft->n_points++;
}

static void sw_ft_outline_close(SW_FT_Outline* ft)
{
    ft->contours_flag[ft->n_contours] = 0;
    int index = ft->n_contours ? ft->contours[ft->n_contours - 1] + 1 : 0;
    if (index == ft->n_points)
        return;

    ft->points[ft->n_points].x = ft->points[index].x;
    ft->points[ft->n_points].y = ft->points[index].y;
    ft->tags[ft->n_points] = SW_FT_CURVE_TAG_ON;
    ft->n_points++;
}

static void sw_ft_outline_end(SW_FT_Outline* ft)
{
    if (ft->n_points)
    {
        ft->contours[ft->n_contours] = ft->n_points - 1;
        ft->n_contours++;
    }
}

static SW_FT_Outline* sw_ft_outline_convert(const plutovg_path_t* path, const plutovg_matrix_t* matrix)
{
    SW_FT_Outline* outline = sw_ft_outline_create(path->points.size, path->contours);
    plutovg_path_element_t* elements = path->elements.data;
    plutovg_point_t* points = path->points.data;
    plutovg_point_t p[3];
    for (int i = 0;i < path->elements.size;i++)
    {
        switch (elements[i])
        {
        case plutovg_path_element_move_to:
            plutovg_matrix_map_point(matrix, &points[0], &p[0]);
            sw_ft_outline_move_to(outline, p[0].x, p[0].y);
            points += 1;
            break;
        case plutovg_path_element_line_to:
            plutovg_matrix_map_point(matrix, &points[0], &p[0]);
            sw_ft_outline_line_to(outline, p[0].x, p[0].y);
            points += 1;
            break;
        case plutovg_path_element_cubic_to:
            plutovg_matrix_map_point(matrix, &points[0], &p[0]);
            plutovg_matrix_map_point(matrix, &points[1], &p[1]);
            plutovg_matrix_map_point(matrix, &points[2], &p[2]);
            sw_ft_outline_cubic_to(outline, p[0].x, p[0].y, p[1].x, p[1].y, p[2].x, p[2].y);
            points += 3;
            break;
        case plutovg_path_element_close:
            sw_ft_outline_close(outline);
            points += 1;
            break;
        }
    }

    sw_ft_outline_end(outline);
    return outline;
}

static SW_FT_Outline* sw_ft_outline_convert_dash(const plutovg_path_t* path, const plutovg_matrix_t* matrix, const plutovg_dash_t* dash)
{
    plutovg_path_t* dashed = plutovg_dash_path(dash, path);
    SW_FT_Outline* outline = sw_ft_outline_convert(dashed, matrix);
    plutovg_path_destroy(dashed);
    return outline;
}

static void generation_callback(int count, const SW_FT_Span* spans, void* user)
{
    plutovg_rle_t* rle = (plutovg_rle_t*)user;
    plutovg_array_ensure(rle->spans, count, plutovg_span_t*);
    plutovg_span_t* data = rle->spans.data + rle->spans.size;
    memcpy(data, spans, (size_t)count * sizeof(plutovg_span_t));
    rle->spans.size += count;
}

static void bbox_callback(int x, int y, int w, int h, void* user)
{
    plutovg_rle_t* rle = (plutovg_rle_t*)user;
    rle->x = x;
    rle->y = y;
    rle->w = w;
    rle->h = h;
}

plutovg_rle_t* plutovg_rle_create(void)
{
    plutovg_rle_t* rle = (plutovg_rle_t*)malloc(sizeof(plutovg_rle_t));
    plutovg_array_init(rle->spans);
    rle->x = 0;
    rle->y = 0;
    rle->w = 0;
    rle->h = 0;
    return rle;
}

void plutovg_rle_destroy(plutovg_rle_t* rle)
{
    if (rle == NULL)
        return;

    free(rle->spans.data);
    free(rle);
}

#define SQRT2 1.41421356237309504880
void plutovg_rle_rasterize(plutovg_rle_t* rle, const plutovg_path_t* path, const plutovg_matrix_t* matrix, const plutovg_rect_t* clip, const plutovg_stroke_data_t* stroke, plutovg_fill_rule_t winding)
{
    SW_FT_Raster_Params params;
    params.flags = SW_FT_RASTER_FLAG_DIRECT | SW_FT_RASTER_FLAG_AA;
    params.gray_spans = generation_callback;
    params.bbox_cb = bbox_callback;
    params.user = rle;

    if (clip)
    {
        params.flags |= SW_FT_RASTER_FLAG_CLIP;
        params.clip_box.xMin = (SW_FT_Pos)clip->x;
        params.clip_box.yMin = (SW_FT_Pos)clip->y;
        params.clip_box.xMax = (SW_FT_Pos)(clip->x + clip->w);
        params.clip_box.yMax = (SW_FT_Pos)(clip->y + clip->h);
    }

    if (stroke)
    {
        SW_FT_Outline* outline = stroke->dash ? sw_ft_outline_convert_dash(path, matrix, stroke->dash) : sw_ft_outline_convert(path, matrix);
        SW_FT_Stroker_LineCap ftCap;
        SW_FT_Stroker_LineJoin ftJoin;
        SW_FT_Fixed ftWidth;
        SW_FT_Fixed ftMiterLimit;

        plutovg_point_t p1 = { 0, 0 };
        plutovg_point_t p2 = { SQRT2, SQRT2 };
        plutovg_point_t p3;

        plutovg_matrix_map_point(matrix, &p1, &p1);
        plutovg_matrix_map_point(matrix, &p2, &p2);

        p3.x = p2.x - p1.x;
        p3.y = p2.y - p1.y;

        double scale = sqrt(p3.x * p3.x + p3.y * p3.y) / 2.0;

        ftWidth = (SW_FT_Fixed)(stroke->width * scale * 0.5 * (1 << 6));
        ftMiterLimit = (SW_FT_Fixed)(stroke->miterlimit * (1 << 16));

        switch (stroke->cap)
        {
        case plutovg_line_cap_square:
            ftCap = SW_FT_STROKER_LINECAP_SQUARE;
            break;
        case plutovg_line_cap_round:
            ftCap = SW_FT_STROKER_LINECAP_ROUND;
            break;
        default:
            ftCap = SW_FT_STROKER_LINECAP_BUTT;
            break;
        }

        switch (stroke->join)
        {
        case plutovg_line_join_bevel:
            ftJoin = SW_FT_STROKER_LINEJOIN_BEVEL;
            break;
        case plutovg_line_join_round:
            ftJoin = SW_FT_STROKER_LINEJOIN_ROUND;
            break;
        default:
            ftJoin = SW_FT_STROKER_LINEJOIN_MITER_FIXED;
            break;
        }

        SW_FT_Stroker stroker;
        SW_FT_Stroker_New(&stroker);
        SW_FT_Stroker_Set(stroker, ftWidth, ftCap, ftJoin, ftMiterLimit);
        SW_FT_Stroker_ParseOutline(stroker, outline);

        SW_FT_UInt points;
        SW_FT_UInt contours;
        SW_FT_Stroker_GetCounts(stroker, &points, &contours);

        SW_FT_Outline* strokeOutline = sw_ft_outline_create((int)points, (int)contours);
        SW_FT_Stroker_Export(stroker, strokeOutline);
        SW_FT_Stroker_Done(stroker);

        strokeOutline->flags = SW_FT_OUTLINE_NONE;
        params.source = strokeOutline;
        sw_ft_grays_raster.raster_render(NULL, &params);
        sw_ft_outline_destroy(outline);
        sw_ft_outline_destroy(strokeOutline);
    }
    else
    {
        SW_FT_Outline* outline = sw_ft_outline_convert(path, matrix);
        outline->flags = winding == plutovg_fill_rule_even_odd ? SW_FT_OUTLINE_EVEN_ODD_FILL : SW_FT_OUTLINE_NONE;
        params.source = outline;
        sw_ft_grays_raster.raster_render(NULL, &params);
        sw_ft_outline_destroy(outline);
    }
}

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define DIV255(x) (((x) + ((x) >> 8) + 0x80) >> 8)
plutovg_rle_t* plutovg_rle_intersection(const plutovg_rle_t* a, const plutovg_rle_t* b)
{
    int count = MAX(a->spans.size, b->spans.size);
    plutovg_rle_t* result = (plutovg_rle_t*)malloc(sizeof(plutovg_rle_t));
    plutovg_array_init(result->spans);
    plutovg_array_ensure(result->spans, count, plutovg_span_t*);

    plutovg_span_t* a_spans = a->spans.data;
    plutovg_span_t* a_end = a_spans + a->spans.size;

    plutovg_span_t* b_spans = b->spans.data;
    plutovg_span_t* b_end = b_spans + b->spans.size;

    while (count && a_spans < a_end && b_spans < b_end)
    {
        if (b_spans->y > a_spans->y)
        {
            ++a_spans;
            continue;
        }

        if (a_spans->y != b_spans->y)
        {
            ++b_spans;
            continue;
        }

        int ax1 = a_spans->x;
        int ax2 = ax1 + a_spans->len;
        int bx1 = b_spans->x;
        int bx2 = bx1 + b_spans->len;

        if (bx1 < ax1 && bx2 < ax1)
        {
            ++b_spans;
            continue;
        }
        else if (ax1 < bx1 && ax2 < bx1)
        {
            ++a_spans;
            continue;
        }

        int x = MAX(ax1, bx1);
        int len = MIN(ax2, bx2) - x;
        if (len)
        {
            plutovg_span_t* span = result->spans.data + result->spans.size;
            span->x = (short)x;
            span->len = (unsigned short)len;
            span->y = a_spans->y;
            span->coverage = DIV255(a_spans->coverage * b_spans->coverage);
            ++result->spans.size;
            --count;
        }

        if (ax2 < bx2)
        {
            ++a_spans;
        }
        else
        {
            ++b_spans;
        }
    }

    if (result->spans.size == 0)
    {
        result->x = 0;
        result->y = 0;
        result->w = 0;
        result->h = 0;
        return result;
    }

    plutovg_span_t* spans = result->spans.data;
    int x1 = INT_MAX;
    int y1 = spans[0].y;
    int x2 = 0;
    int y2 = spans[result->spans.size - 1].y;
    for (int i = 0;i < result->spans.size;i++)
    {
        if (spans[i].x < x1) x1 = spans[i].x;
        if (spans[i].x + spans[i].len > x2) x2 = spans[i].x + spans[i].len;
    }

    result->x = x1;
    result->y = y1;
    result->w = x2 - x1;
    result->h = y2 - y1 + 1;
    return result;
}

void plutovg_rle_clip_path(plutovg_rle_t* rle, const plutovg_rle_t* clip)
{
    if (rle == NULL || clip == NULL)
        return;

    plutovg_rle_t* result = plutovg_rle_intersection(rle, clip);
    plutovg_array_ensure(rle->spans, result->spans.size, plutovg_span_t*);
    memcpy(rle->spans.data, result->spans.data, (size_t)result->spans.size * sizeof(plutovg_span_t));
    rle->spans.size = result->spans.size;
    rle->x = result->x;
    rle->y = result->y;
    rle->w = result->w;
    rle->h = result->h;
    plutovg_rle_destroy(result);
}

plutovg_rle_t* plutovg_rle_clone(const plutovg_rle_t* rle)
{
    if (rle == NULL)
        return NULL;

    plutovg_rle_t* result = (plutovg_rle_t*)malloc(sizeof(plutovg_rle_t));
    plutovg_array_init(result->spans);
    plutovg_array_ensure(result->spans, rle->spans.size, plutovg_span_t*);

    memcpy(result->spans.data, rle->spans.data, (size_t)rle->spans.size * sizeof(plutovg_span_t));
    result->spans.size = rle->spans.size;
    result->x = rle->x;
    result->y = rle->y;
    result->w = rle->w;
    result->h = rle->h;
    return result;
}

void plutovg_rle_clear(plutovg_rle_t* rle)
{
    rle->spans.size = 0;
    rle->x = 0;
    rle->y = 0;
    rle->w = 0;
    rle->h = 0;
}

/***************************************************************************/
/*                                                                         */
/*  fttrigon.c                                                             */
/*                                                                         */
/*    FreeType trigonometric functions (body).                             */
/*                                                                         */
/*  Copyright 2001-2005, 2012-2013 by                                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

//form https://github.com/chromium/chromium/blob/59afd8336009c9d97c22854c52e0382b62b3aa5e/third_party/abseil-cpp/absl/base/internal/bits.h

#if defined(_MSC_VER)
#include <intrin.h>
static unsigned int __inline clz(unsigned int x) {
    unsigned long r = 0;
    if (x != 0)
    {
        _BitScanReverse(&r, x);
    }
    return  r;
}
#define SW_FT_MSB(x)  (clz(x))
#elif defined(__GNUC__)
#define SW_FT_MSB(x)  (31 - __builtin_clz(x))
#else
static unsigned int __inline clz(unsigned int x) {
    int c = 31;
    x &= ~x + 1;
    if (n & 0x0000FFFF) c -= 16;
    if (n & 0x00FF00FF) c -= 8;
    if (n & 0x0F0F0F0F) c -= 4;
    if (n & 0x33333333) c -= 2;
    if (n & 0x55555555) c -= 1;
    return c;
}
#define SW_FT_MSB(x)  (clz(x))
#endif





#define SW_FT_PAD_FLOOR(x, n) ((x) & ~((n)-1))
#define SW_FT_PAD_ROUND(x, n) SW_FT_PAD_FLOOR((x) + ((n) / 2), n)
#define SW_FT_PAD_CEIL(x, n) SW_FT_PAD_FLOOR((x) + ((n)-1), n)

#define SW_FT_BEGIN_STMNT do {
#define SW_FT_END_STMNT \
    }                   \
    while (0)
/* transfer sign leaving a positive number */
#define SW_FT_MOVE_SIGN(x, s) \
    SW_FT_BEGIN_STMNT         \
    if (x < 0) {              \
        x = -x;               \
        s = -s;               \
    }                         \
    SW_FT_END_STMNT

SW_FT_Long SW_FT_MulFix(SW_FT_Long a, SW_FT_Long b)
{
    SW_FT_Int  s = 1;
    SW_FT_Long c;

    SW_FT_MOVE_SIGN(a, s);
    SW_FT_MOVE_SIGN(b, s);

    c = (SW_FT_Long)(((SW_FT_Int64)a * b + 0x8000L) >> 16);

    return (s > 0) ? c : -c;
}

SW_FT_Long SW_FT_MulDiv(SW_FT_Long a, SW_FT_Long b, SW_FT_Long c)
{
    SW_FT_Int  s = 1;
    SW_FT_Long d;

    SW_FT_MOVE_SIGN(a, s);
    SW_FT_MOVE_SIGN(b, s);
    SW_FT_MOVE_SIGN(c, s);

    d = (SW_FT_Long)(c > 0 ? ((SW_FT_Int64)a * b + (c >> 1)) / c : 0x7FFFFFFFL);

    return (s > 0) ? d : -d;
}

SW_FT_Long SW_FT_DivFix(SW_FT_Long a, SW_FT_Long b)
{
    SW_FT_Int  s = 1;
    SW_FT_Long q;

    SW_FT_MOVE_SIGN(a, s);
    SW_FT_MOVE_SIGN(b, s);

    q = (SW_FT_Long)(b > 0 ? (((SW_FT_UInt64)a << 16) + (b >> 1)) / b
        : 0x7FFFFFFFL);

    return (s < 0 ? -q : q);
}

/*************************************************************************/
/*                                                                       */
/* This is a fixed-point CORDIC implementation of trigonometric          */
/* functions as well as transformations between Cartesian and polar      */
/* coordinates.  The angles are represented as 16.16 fixed-point values  */
/* in degrees, i.e., the angular resolution is 2^-16 degrees.  Note that */
/* only vectors longer than 2^16*180/pi (or at least 22 bits) on a       */
/* discrete Cartesian grid can have the same or better angular           */
/* resolution.  Therefore, to maintain this precision, some functions    */
/* require an interim upscaling of the vectors, whereas others operate   */
/* with 24-bit long vectors directly.                                    */
/*                                                                       */
/*************************************************************************/

/* the Cordic shrink factor 0.858785336480436 * 2^32 */
#define SW_FT_TRIG_SCALE 0xDBD95B16UL

/* the highest bit in overflow-safe vector components, */
/* MSB of 0.858785336480436 * sqrt(0.5) * 2^30         */
#define SW_FT_TRIG_SAFE_MSB 29

/* this table was generated for SW_FT_PI = 180L << 16, i.e. degrees */
#define SW_FT_TRIG_MAX_ITERS 23

static const SW_FT_Fixed ft_trig_arctan_table[] = {
    1740967L, 919879L, 466945L, 234379L, 117304L, 58666L, 29335L, 14668L,
    7334L,    3667L,   1833L,   917L,    458L,    229L,   115L,   57L,
    29L,      14L,     7L,      4L,      2L,      1L };

/* multiply a given value by the CORDIC shrink factor */
static SW_FT_Fixed ft_trig_downscale(SW_FT_Fixed val)
{
    SW_FT_Fixed s;
    SW_FT_Int64 v;

    s = val;
    val = SW_FT_ABS(val);

    v = (val * (SW_FT_Int64)SW_FT_TRIG_SCALE) + 0x100000000UL;
    val = (SW_FT_Fixed)(v >> 32);

    return (s >= 0) ? val : -val;
}

/* undefined and never called for zero vector */
static SW_FT_Int ft_trig_prenorm(SW_FT_Vector* vec)
{
    SW_FT_Pos x, y;
    SW_FT_Int shift;

    x = vec->x;
    y = vec->y;

    shift = SW_FT_MSB(SW_FT_ABS(x) | SW_FT_ABS(y));

    if (shift <= SW_FT_TRIG_SAFE_MSB) {
        shift = SW_FT_TRIG_SAFE_MSB - shift;
        vec->x = (SW_FT_Pos)((SW_FT_ULong)x << shift);
        vec->y = (SW_FT_Pos)((SW_FT_ULong)y << shift);
    }
    else {
        shift -= SW_FT_TRIG_SAFE_MSB;
        vec->x = x >> shift;
        vec->y = y >> shift;
        shift = -shift;
    }

    return shift;
}

static void ft_trig_pseudo_rotate(SW_FT_Vector* vec, SW_FT_Angle theta)
{
    SW_FT_Int          i;
    SW_FT_Fixed        x, y, xtemp, b;
    const SW_FT_Fixed* arctanptr;

    x = vec->x;
    y = vec->y;

    /* Rotate inside [-PI/4,PI/4] sector */
    while (theta < -SW_FT_ANGLE_PI4) {
        xtemp = y;
        y = -x;
        x = xtemp;
        theta += SW_FT_ANGLE_PI2;
    }

    while (theta > SW_FT_ANGLE_PI4) {
        xtemp = -y;
        y = x;
        x = xtemp;
        theta -= SW_FT_ANGLE_PI2;
    }

    arctanptr = ft_trig_arctan_table;

    /* Pseudorotations, with right shifts */
    for (i = 1, b = 1; i < SW_FT_TRIG_MAX_ITERS; b <<= 1, i++) {
        SW_FT_Fixed v1 = ((y + b) >> i);
        SW_FT_Fixed v2 = ((x + b) >> i);
        if (theta < 0) {
            xtemp = x + v1;
            y = y - v2;
            x = xtemp;
            theta += *arctanptr++;
        }
        else {
            xtemp = x - v1;
            y = y + v2;
            x = xtemp;
            theta -= *arctanptr++;
        }
    }

    vec->x = x;
    vec->y = y;
}

static void ft_trig_pseudo_polarize(SW_FT_Vector* vec)
{
    SW_FT_Angle        theta;
    SW_FT_Int          i;
    SW_FT_Fixed        x, y, xtemp, b;
    const SW_FT_Fixed* arctanptr;

    x = vec->x;
    y = vec->y;

    /* Get the vector into [-PI/4,PI/4] sector */
    if (y > x) {
        if (y > -x) {
            theta = SW_FT_ANGLE_PI2;
            xtemp = y;
            y = -x;
            x = xtemp;
        }
        else {
            theta = y > 0 ? SW_FT_ANGLE_PI : -SW_FT_ANGLE_PI;
            x = -x;
            y = -y;
        }
    }
    else {
        if (y < -x) {
            theta = -SW_FT_ANGLE_PI2;
            xtemp = -y;
            y = x;
            x = xtemp;
        }
        else {
            theta = 0;
        }
    }

    arctanptr = ft_trig_arctan_table;

    /* Pseudorotations, with right shifts */
    for (i = 1, b = 1; i < SW_FT_TRIG_MAX_ITERS; b <<= 1, i++) {
        SW_FT_Fixed v1 = ((y + b) >> i);
        SW_FT_Fixed v2 = ((x + b) >> i);
        if (y > 0) {
            xtemp = x + v1;
            y = y - v2;
            x = xtemp;
            theta += *arctanptr++;
        }
        else {
            xtemp = x - v1;
            y = y + v2;
            x = xtemp;
            theta -= *arctanptr++;
        }
    }

    /* round theta */
    if (theta >= 0)
        theta = SW_FT_PAD_ROUND(theta, 32);
    else
        theta = -SW_FT_PAD_ROUND(-theta, 32);

    vec->x = x;
    vec->y = theta;
}

/* documentation is in fttrigon.h */

SW_FT_Fixed SW_FT_Cos(SW_FT_Angle angle)
{
    SW_FT_Vector v;

    v.x = SW_FT_TRIG_SCALE >> 8;
    v.y = 0;
    ft_trig_pseudo_rotate(&v, angle);

    return (v.x + 0x80L) >> 8;
}

/* documentation is in fttrigon.h */

SW_FT_Fixed SW_FT_Sin(SW_FT_Angle angle)
{
    return SW_FT_Cos(SW_FT_ANGLE_PI2 - angle);
}

/* documentation is in fttrigon.h */

SW_FT_Fixed SW_FT_Tan(SW_FT_Angle angle)
{
    SW_FT_Vector v;

    v.x = SW_FT_TRIG_SCALE >> 8;
    v.y = 0;
    ft_trig_pseudo_rotate(&v, angle);

    return SW_FT_DivFix(v.y, v.x);
}

/* documentation is in fttrigon.h */

SW_FT_Angle SW_FT_Atan2(SW_FT_Fixed dx, SW_FT_Fixed dy)
{
    SW_FT_Vector v;

    if (dx == 0 && dy == 0) return 0;

    v.x = dx;
    v.y = dy;
    ft_trig_prenorm(&v);
    ft_trig_pseudo_polarize(&v);

    return v.y;
}

/* documentation is in fttrigon.h */

void SW_FT_Vector_Unit(SW_FT_Vector* vec, SW_FT_Angle angle)
{
    vec->x = SW_FT_TRIG_SCALE >> 8;
    vec->y = 0;
    ft_trig_pseudo_rotate(vec, angle);
    vec->x = (vec->x + 0x80L) >> 8;
    vec->y = (vec->y + 0x80L) >> 8;
}

/* these macros return 0 for positive numbers,
   and -1 for negative ones */
#define SW_FT_SIGN_LONG(x) ((x) >> (SW_FT_SIZEOF_LONG * 8 - 1))
#define SW_FT_SIGN_INT(x) ((x) >> (SW_FT_SIZEOF_INT * 8 - 1))
#define SW_FT_SIGN_INT32(x) ((x) >> 31)
#define SW_FT_SIGN_INT16(x) ((x) >> 15)

   /* documentation is in fttrigon.h */

void SW_FT_Vector_Rotate(SW_FT_Vector* vec, SW_FT_Angle angle)
{
    SW_FT_Int    shift;
    SW_FT_Vector v;

    v.x = vec->x;
    v.y = vec->y;

    if (angle && (v.x != 0 || v.y != 0)) {
        shift = ft_trig_prenorm(&v);
        ft_trig_pseudo_rotate(&v, angle);
        v.x = ft_trig_downscale(v.x);
        v.y = ft_trig_downscale(v.y);

        if (shift > 0) {
            SW_FT_Int32 half = (SW_FT_Int32)1L << (shift - 1);

            vec->x = (v.x + half + SW_FT_SIGN_LONG(v.x)) >> shift;
            vec->y = (v.y + half + SW_FT_SIGN_LONG(v.y)) >> shift;
        }
        else {
            shift = -shift;
            vec->x = (SW_FT_Pos)((SW_FT_ULong)v.x << shift);
            vec->y = (SW_FT_Pos)((SW_FT_ULong)v.y << shift);
        }
    }
}

/* documentation is in fttrigon.h */

SW_FT_Fixed SW_FT_Vector_Length(SW_FT_Vector* vec)
{
    SW_FT_Int    shift;
    SW_FT_Vector v;

    v = *vec;

    /* handle trivial cases */
    if (v.x == 0) {
        return SW_FT_ABS(v.y);
    }
    else if (v.y == 0) {
        return SW_FT_ABS(v.x);
    }

    /* general case */
    shift = ft_trig_prenorm(&v);
    ft_trig_pseudo_polarize(&v);

    v.x = ft_trig_downscale(v.x);

    if (shift > 0) return (v.x + (1 << (shift - 1))) >> shift;

    return (SW_FT_Fixed)((SW_FT_UInt32)v.x << -shift);
}

/* documentation is in fttrigon.h */

void SW_FT_Vector_Polarize(SW_FT_Vector* vec, SW_FT_Fixed* length,
    SW_FT_Angle* angle)
{
    SW_FT_Int    shift;
    SW_FT_Vector v;

    v = *vec;

    if (v.x == 0 && v.y == 0) return;

    shift = ft_trig_prenorm(&v);
    ft_trig_pseudo_polarize(&v);

    v.x = ft_trig_downscale(v.x);

    *length = (shift >= 0) ? (v.x >> shift)
        : (SW_FT_Fixed)((SW_FT_UInt32)v.x << -shift);
    *angle = v.y;
}

/* documentation is in fttrigon.h */

void SW_FT_Vector_From_Polar(SW_FT_Vector* vec, SW_FT_Fixed length,
    SW_FT_Angle angle)
{
    vec->x = length;
    vec->y = 0;

    SW_FT_Vector_Rotate(vec, angle);
}

/* documentation is in fttrigon.h */

SW_FT_Angle SW_FT_Angle_Diff(SW_FT_Angle  angle1, SW_FT_Angle  angle2)
{
    SW_FT_Angle  delta = angle2 - angle1;

    while (delta <= -SW_FT_ANGLE_PI)
        delta += SW_FT_ANGLE_2PI;

    while (delta > SW_FT_ANGLE_PI)
        delta -= SW_FT_ANGLE_2PI;

    return delta;
}

/* END */

/***************************************************************************/
/*                                                                         */
/*  ftgrays.c                                                              */
/*                                                                         */
/*    A new `perfect' anti-aliasing renderer (body).                       */
/*                                                                         */
/*  Copyright 2000-2003, 2005-2014 by                                      */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/*************************************************************************/
/*                                                                       */
/* This is a new anti-aliasing scan-converter for FreeType 2.  The       */
/* algorithm used here is _very_ different from the one in the standard  */
/* `ftraster' module.  Actually, `ftgrays' computes the _exact_          */
/* coverage of the outline on each pixel cell.                           */
/*                                                                       */
/* It is based on ideas that I initially found in Raph Levien's          */
/* excellent LibArt graphics library (see http://www.levien.com/libart   */
/* for more information, though the web pages do not tell anything       */
/* about the renderer; you'll have to dive into the source code to       */
/* understand how it works).                                             */
/*                                                                       */
/* Note, however, that this is a _very_ different implementation         */
/* compared to Raph's.  Coverage information is stored in a very         */
/* different way, and I don't use sorted vector paths.  Also, it doesn't */
/* use floating point values.                                            */
/*                                                                       */
/* This renderer has the following advantages:                           */
/*                                                                       */
/* - It doesn't need an intermediate bitmap.  Instead, one can supply a  */
/*   callback function that will be called by the renderer to draw gray  */
/*   spans on any target surface.  You can thus do direct composition on */
/*   any kind of bitmap, provided that you give the renderer the right   */
/*   callback.                                                           */
/*                                                                       */
/* - A perfect anti-aliaser, i.e., it computes the _exact_ coverage on   */
/*   each pixel cell.                                                    */
/*                                                                       */
/* - It performs a single pass on the outline (the `standard' FT2        */
/*   renderer makes two passes).                                         */
/*                                                                       */
/* - It can easily be modified to render to _any_ number of gray levels  */
/*   cheaply.                                                            */
/*                                                                       */
/* - For small (< 20) pixel sizes, it is faster than the standard        */
/*   renderer.                                                           */
/*                                                                       */
/*************************************************************************/

/* Auxiliary macros for token concatenation. */
#define SW_FT_ERR_XCAT(x, y) x##y
#define SW_FT_ERR_CAT(x, y) SW_FT_ERR_XCAT(x, y)

#define SW_FT_BEGIN_STMNT do {
#define SW_FT_END_STMNT \
    }                   \
    while (0)


#define SW_FT_UINT_MAX UINT_MAX
#define SW_FT_INT_MAX INT_MAX
#define SW_FT_ULONG_MAX ULONG_MAX
#define SW_FT_CHAR_BIT CHAR_BIT

#define ft_memset memset

#define ft_setjmp setjmp
#define ft_longjmp longjmp
#define ft_jmp_buf jmp_buf

typedef ptrdiff_t SW_FT_PtrDist;

#define ErrRaster_Invalid_Mode -2
#define ErrRaster_Invalid_Outline -1
#define ErrRaster_Invalid_Argument -3
#define ErrRaster_Memory_Overflow -4

#define SW_FT_BEGIN_HEADER
#define SW_FT_END_HEADER

/* This macro is used to indicate that a function parameter is unused. */
/* Its purpose is simply to reduce compiler warnings.  Note also that  */
/* simply defining it as `(void)x' doesn't avoid warnings with certain */
/* ANSI compilers (e.g. LCC).                                          */
#define SW_FT_UNUSED(x) (x) = (x)

#define SW_FT_THROW(e) SW_FT_ERR_CAT(ErrRaster_, e)

/* The size in bytes of the render pool used by the scan-line converter  */
/* to do all of its work.                                                */
#define SW_FT_RENDER_POOL_SIZE 16384L

typedef int (*SW_FT_Outline_MoveToFunc)(const SW_FT_Vector* to, void* user);

#define SW_FT_Outline_MoveTo_Func SW_FT_Outline_MoveToFunc

typedef int (*SW_FT_Outline_LineToFunc)(const SW_FT_Vector* to, void* user);

#define SW_FT_Outline_LineTo_Func SW_FT_Outline_LineToFunc

typedef int (*SW_FT_Outline_ConicToFunc)(const SW_FT_Vector* control,
    const SW_FT_Vector* to, void* user);

#define SW_FT_Outline_ConicTo_Func SW_FT_Outline_ConicToFunc

typedef int (*SW_FT_Outline_CubicToFunc)(const SW_FT_Vector* control1,
    const SW_FT_Vector* control2,
    const SW_FT_Vector* to, void* user);

#define SW_FT_Outline_CubicTo_Func SW_FT_Outline_CubicToFunc

typedef struct SW_FT_Outline_Funcs_ {
    SW_FT_Outline_MoveToFunc  move_to;
    SW_FT_Outline_LineToFunc  line_to;
    SW_FT_Outline_ConicToFunc conic_to;
    SW_FT_Outline_CubicToFunc cubic_to;

    int       shift;
    SW_FT_Pos delta;

} SW_FT_Outline_Funcs;

#define SW_FT_DEFINE_OUTLINE_FUNCS(class_, move_to_, line_to_, conic_to_,      \
                                   cubic_to_, shift_, delta_)                  \
    static const SW_FT_Outline_Funcs class_ = {move_to_,  line_to_, conic_to_, \
                                               cubic_to_, shift_,   delta_};

#define SW_FT_DEFINE_RASTER_FUNCS(class_, raster_new_, raster_reset_, \
                                  raster_render_, raster_done_)       \
    const SW_FT_Raster_Funcs class_ = {raster_new_, raster_reset_,    \
                                       raster_render_, raster_done_};

#ifndef SW_FT_MEM_SET
#define SW_FT_MEM_SET(d, s, c) ft_memset(d, s, c)
#endif

#ifndef SW_FT_MEM_ZERO
#define SW_FT_MEM_ZERO(dest, count) SW_FT_MEM_SET(dest, 0, count)
#endif

/* as usual, for the speed hungry :-) */

#undef RAS_ARG
#undef RAS_ARG_
#undef RAS_VAR
#undef RAS_VAR_

#ifndef SW_FT_STATIC_RASTER

#define RAS_ARG gray_PWorker worker
#define RAS_ARG_ gray_PWorker worker,

#define RAS_VAR worker
#define RAS_VAR_ worker,

#else /* SW_FT_STATIC_RASTER */

#define RAS_ARG  /* empty */
#define RAS_ARG_ /* empty */
#define RAS_VAR  /* empty */
#define RAS_VAR_ /* empty */

#endif /* SW_FT_STATIC_RASTER */

/* must be at least 6 bits! */
#define PIXEL_BITS 8

#undef FLOOR
#undef CEILING
#undef TRUNC
#undef SCALED

#define ONE_PIXEL (1L << PIXEL_BITS)
#define PIXEL_MASK (-1L << PIXEL_BITS)
#define TRUNC(x) ((TCoord)((x) >> PIXEL_BITS))
#define SUBPIXELS(x) ((TPos)(x) << PIXEL_BITS)
#define FLOOR(x) ((x) & -ONE_PIXEL)
#define CEILING(x) (((x) + ONE_PIXEL - 1) & -ONE_PIXEL)
#define ROUND(x) (((x) + ONE_PIXEL / 2) & -ONE_PIXEL)

#if PIXEL_BITS >= 6
#define UPSCALE(x) ((x) << (PIXEL_BITS - 6))
#define DOWNSCALE(x) ((x) >> (PIXEL_BITS - 6))
#else
#define UPSCALE(x) ((x) >> (6 - PIXEL_BITS))
#define DOWNSCALE(x) ((x) << (6 - PIXEL_BITS))
#endif

/* Compute `dividend / divisor' and return both its quotient and     */
/* remainder, cast to a specific type.  This macro also ensures that */
/* the remainder is always positive.                                 */
#define SW_FT_DIV_MOD(type, dividend, divisor, quotient, remainder) \
    SW_FT_BEGIN_STMNT(quotient) = (type)((dividend) / (divisor));   \
    (remainder) = (type)((dividend) % (divisor));                   \
    if ((remainder) < 0) {                                          \
        (quotient)--;                                               \
        (remainder) += (type)(divisor);                             \
    }                                                               \
    SW_FT_END_STMNT

#ifdef __arm__
/* Work around a bug specific to GCC which make the compiler fail to */
/* optimize a division and modulo operation on the same parameters   */
/* into a single call to `__aeabi_idivmod'.  See                     */
/*                                                                   */
/*  http://gcc.gnu.org/bugzilla/show_bug.cgi?id=43721                */
#undef SW_FT_DIV_MOD
#define SW_FT_DIV_MOD(type, dividend, divisor, quotient, remainder) \
    SW_FT_BEGIN_STMNT(quotient) = (type)((dividend) / (divisor));   \
    (remainder) = (type)((dividend) - (quotient) * (divisor));      \
    if ((remainder) < 0) {                                          \
        (quotient)--;                                               \
        (remainder) += (type)(divisor);                             \
    }                                                               \
    SW_FT_END_STMNT
#endif /* __arm__ */

/* These macros speed up repetitive divisions by replacing them */
/* with multiplications and right shifts.                       */
#define SW_FT_UDIVPREP(b) \
    long b##_r = (long)(SW_FT_ULONG_MAX >> PIXEL_BITS) / (b)
#define SW_FT_UDIV(a, b)                              \
    (((unsigned long)(a) * (unsigned long)(b##_r)) >> \
     (sizeof(long) * SW_FT_CHAR_BIT - PIXEL_BITS))

/*************************************************************************/
/*                                                                       */
/*   TYPE DEFINITIONS                                                    */
/*                                                                       */

/* don't change the following types to SW_FT_Int or SW_FT_Pos, since we might */
/* need to define them to "float" or "double" when experimenting with   */
/* new algorithms                                                       */

typedef long TCoord; /* integer scanline/pixel coordinate */
typedef long TPos;   /* sub-pixel coordinate              */

/* determine the type used to store cell areas.  This normally takes at */
/* least PIXEL_BITS*2 + 1 bits.  On 16-bit systems, we need to use      */
/* `long' instead of `int', otherwise bad things happen                 */

#if PIXEL_BITS <= 7

typedef int TArea;

#else /* PIXEL_BITS >= 8 */

/* approximately determine the size of integers using an ANSI-C header */
#if SW_FT_UINT_MAX == 0xFFFFU
typedef long        TArea;
#else
typedef int TArea;
#endif

#endif /* PIXEL_BITS >= 8 */

/* maximum number of gray spans in a call to the span callback */
#define SW_FT_MAX_GRAY_SPANS 256

typedef struct TCell_* PCell;

typedef struct TCell_ {
    TPos   x;     /* same with gray_TWorker.ex    */
    TCoord cover; /* same with gray_TWorker.cover */
    TArea  area;
    PCell  next;

} TCell;

#if defined(_MSC_VER) /* Visual C++ (and Intel C++) */
/* We disable the warning `structure was padded due to   */
/* __declspec(align())' in order to compile cleanly with */
/* the maximum level of warnings.                        */
#pragma warning(push)
#pragma warning(disable : 4324)
#endif /* _MSC_VER */

typedef struct gray_TWorker_ {
    TCoord ex, ey;
    TPos   min_ex, max_ex;
    TPos   min_ey, max_ey;
    TPos   count_ex, count_ey;

    TArea  area;
    TCoord cover;
    int    invalid;

    PCell         cells;
    SW_FT_PtrDist max_cells;
    SW_FT_PtrDist num_cells;

    TPos x, y;

    SW_FT_Vector bez_stack[32 * 3 + 1];
    int          lev_stack[32];

    SW_FT_Outline outline;
    SW_FT_BBox    clip_box;

    int           bound_left;
    int           bound_top;
    int           bound_right;
    int           bound_bottom;

    SW_FT_Span gray_spans[SW_FT_MAX_GRAY_SPANS];
    int        num_gray_spans;

    SW_FT_Raster_Span_Func render_span;
    void* render_span_data;

    int band_size;
    int band_shoot;

    ft_jmp_buf jump_buffer;

    void* buffer;
    long  buffer_size;

    PCell* ycells;
    TPos   ycount;

} gray_TWorker, * gray_PWorker;

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

#ifndef SW_FT_STATIC_RASTER
#define ras (*worker)
#else
static gray_TWorker ras;
#endif

typedef struct gray_TRaster_ {
    void* memory;

} gray_TRaster, * gray_PRaster;

/*************************************************************************/
/*                                                                       */
/* Initialize the cells table.                                           */
/*                                                                       */
static void gray_init_cells(RAS_ARG_ void* buffer, long byte_size)
{
    ras.buffer = buffer;
    ras.buffer_size = byte_size;

    ras.ycells = (PCell*)buffer;
    ras.cells = NULL;
    ras.max_cells = 0;
    ras.num_cells = 0;
    ras.area = 0;
    ras.cover = 0;
    ras.invalid = 1;

    ras.bound_left = INT_MAX;
    ras.bound_top = INT_MAX;
    ras.bound_right = INT_MIN;
    ras.bound_bottom = INT_MIN;
}

/*************************************************************************/
/*                                                                       */
/* Compute the outline bounding box.                                     */
/*                                                                       */
static void gray_compute_cbox(RAS_ARG)
{
    SW_FT_Outline* outline = &ras.outline;
    SW_FT_Vector* vec = outline->points;
    SW_FT_Vector* limit = vec + outline->n_points;

    if (outline->n_points <= 0) {
        ras.min_ex = ras.max_ex = 0;
        ras.min_ey = ras.max_ey = 0;
        return;
    }

    ras.min_ex = ras.max_ex = vec->x;
    ras.min_ey = ras.max_ey = vec->y;

    vec++;

    for (; vec < limit; vec++) {
        TPos x = vec->x;
        TPos y = vec->y;

        if (x < ras.min_ex) ras.min_ex = x;
        if (x > ras.max_ex) ras.max_ex = x;
        if (y < ras.min_ey) ras.min_ey = y;
        if (y > ras.max_ey) ras.max_ey = y;
    }

    /* truncate the bounding box to integer pixels */
    ras.min_ex = ras.min_ex >> 6;
    ras.min_ey = ras.min_ey >> 6;
    ras.max_ex = (ras.max_ex + 63) >> 6;
    ras.max_ey = (ras.max_ey + 63) >> 6;
}

/*************************************************************************/
/*                                                                       */
/* Record the current cell in the table.                                 */
/*                                                                       */
static PCell gray_find_cell(RAS_ARG)
{
    PCell* pcell, cell;
    TPos   x = ras.ex;

    if (x > ras.count_ex) x = ras.count_ex;

    pcell = &ras.ycells[ras.ey];
    for (;;) {
        cell = *pcell;
        if (cell == NULL || cell->x > x) break;

        if (cell->x == x) goto Exit;

        pcell = &cell->next;
    }

    if (ras.num_cells >= ras.max_cells) ft_longjmp(ras.jump_buffer, 1);

    cell = ras.cells + ras.num_cells++;
    cell->x = x;
    cell->area = 0;
    cell->cover = 0;

    cell->next = *pcell;
    *pcell = cell;

Exit:
    return cell;
}

static void gray_record_cell(RAS_ARG)
{
    if (ras.area | ras.cover) {
        PCell cell = gray_find_cell(RAS_VAR);

        cell->area += ras.area;
        cell->cover += ras.cover;
    }
}

/*************************************************************************/
/*                                                                       */
/* Set the current cell to a new position.                               */
/*                                                                       */
static void gray_set_cell(RAS_ARG_ TCoord ex, TCoord ey)
{
    /* Move the cell pointer to a new position.  We set the `invalid'      */
    /* flag to indicate that the cell isn't part of those we're interested */
    /* in during the render phase.  This means that:                       */
    /*                                                                     */
    /* . the new vertical position must be within min_ey..max_ey-1.        */
    /* . the new horizontal position must be strictly less than max_ex     */
    /*                                                                     */
    /* Note that if a cell is to the left of the clipping region, it is    */
    /* actually set to the (min_ex-1) horizontal position.                 */

    /* All cells that are on the left of the clipping region go to the */
    /* min_ex - 1 horizontal position.                                 */
    ey -= ras.min_ey;

    if (ex > ras.max_ex) ex = ras.max_ex;

    ex -= ras.min_ex;
    if (ex < 0) ex = -1;

    /* are we moving to a different cell ? */
    if (ex != ras.ex || ey != ras.ey) {
        /* record the current one if it is valid */
        if (!ras.invalid) gray_record_cell(RAS_VAR);

        ras.area = 0;
        ras.cover = 0;
        ras.ex = ex;
        ras.ey = ey;
    }

    ras.invalid =
        ((unsigned)ey >= (unsigned)ras.count_ey || ex >= ras.count_ex);
}

/*************************************************************************/
/*                                                                       */
/* Start a new contour at a given cell.                                  */
/*                                                                       */
static void gray_start_cell(RAS_ARG_ TCoord ex, TCoord ey)
{
    if (ex > ras.max_ex) ex = (TCoord)(ras.max_ex);

    if (ex < ras.min_ex) ex = (TCoord)(ras.min_ex - 1);

    ras.area = 0;
    ras.cover = 0;
    ras.ex = ex - ras.min_ex;
    ras.ey = ey - ras.min_ey;
    ras.invalid = 0;

    gray_set_cell(RAS_VAR_ ex, ey);
}

/*************************************************************************/
/*                                                                       */
/* Render a straight line across multiple cells in any direction.        */
/*                                                                       */
static void gray_render_line(RAS_ARG_ TPos to_x, TPos to_y)
{
    TPos   dx, dy, fx1, fy1, fx2, fy2;
    TCoord ex1, ex2, ey1, ey2;

    ex1 = TRUNC(ras.x);
    ex2 = TRUNC(to_x);
    ey1 = TRUNC(ras.y);
    ey2 = TRUNC(to_y);

    /* perform vertical clipping */
    if ((ey1 >= ras.max_ey && ey2 >= ras.max_ey) ||
        (ey1 < ras.min_ey && ey2 < ras.min_ey))
        goto End;

    dx = to_x - ras.x;
    dy = to_y - ras.y;

    fx1 = ras.x - SUBPIXELS(ex1);
    fy1 = ras.y - SUBPIXELS(ey1);

    if (ex1 == ex2 && ey1 == ey2) /* inside one cell */
        ;
    else if (dy == 0) /* ex1 != ex2 */ /* any horizontal line */
    {
        ex1 = ex2;
        gray_set_cell(RAS_VAR_ ex1, ey1);
    }
    else if (dx == 0) {
        if (dy > 0) /* vertical line up */
            do {
                fy2 = ONE_PIXEL;
                ras.cover += (fy2 - fy1);
                ras.area += (fy2 - fy1) * fx1 * 2;
                fy1 = 0;
                ey1++;
                gray_set_cell(RAS_VAR_ ex1, ey1);
            } while (ey1 != ey2);
        else /* vertical line down */
            do {
                fy2 = 0;
                ras.cover += (fy2 - fy1);
                ras.area += (fy2 - fy1) * fx1 * 2;
                fy1 = ONE_PIXEL;
                ey1--;
                gray_set_cell(RAS_VAR_ ex1, ey1);
            } while (ey1 != ey2);
    }
    else /* any other line */
    {
        TArea prod = dx * fy1 - dy * fx1;
        SW_FT_UDIVPREP(dx);
        SW_FT_UDIVPREP(dy);

        /* The fundamental value `prod' determines which side and the  */
        /* exact coordinate where the line exits current cell.  It is  */
        /* also easily updated when moving from one cell to the next.  */
        do {
            if (prod <= 0 && prod - dx * ONE_PIXEL > 0) /* left */
            {
                fx2 = 0;
                fy2 = (TPos)SW_FT_UDIV(-prod, -dx);
                prod -= dy * ONE_PIXEL;
                ras.cover += (fy2 - fy1);
                ras.area += (fy2 - fy1) * (fx1 + fx2);
                fx1 = ONE_PIXEL;
                fy1 = fy2;
                ex1--;
            }
            else if (prod - dx * ONE_PIXEL <= 0 &&
                prod - dx * ONE_PIXEL + dy * ONE_PIXEL > 0) /* up */
            {
                prod -= dx * ONE_PIXEL;
                fx2 = (TPos)SW_FT_UDIV(-prod, dy);
                fy2 = ONE_PIXEL;
                ras.cover += (fy2 - fy1);
                ras.area += (fy2 - fy1) * (fx1 + fx2);
                fx1 = fx2;
                fy1 = 0;
                ey1++;
            }
            else if (prod - dx * ONE_PIXEL + dy * ONE_PIXEL <= 0 &&
                prod + dy * ONE_PIXEL >= 0) /* right */
            {
                prod += dy * ONE_PIXEL;
                fx2 = ONE_PIXEL;
                fy2 = (TPos)SW_FT_UDIV(prod, dx);
                ras.cover += (fy2 - fy1);
                ras.area += (fy2 - fy1) * (fx1 + fx2);
                fx1 = 0;
                fy1 = fy2;
                ex1++;
            }
            else /* ( prod                  + dy * ONE_PIXEL <  0 &&
                     prod                                   >  0 )    down */
            {
                fx2 = (TPos)SW_FT_UDIV(prod, -dy);
                fy2 = 0;
                prod += dx * ONE_PIXEL;
                ras.cover += (fy2 - fy1);
                ras.area += (fy2 - fy1) * (fx1 + fx2);
                fx1 = fx2;
                fy1 = ONE_PIXEL;
                ey1--;
            }

            gray_set_cell(RAS_VAR_ ex1, ey1);
        } while (ex1 != ex2 || ey1 != ey2);
    }

    fx2 = to_x - SUBPIXELS(ex2);
    fy2 = to_y - SUBPIXELS(ey2);

    ras.cover += (fy2 - fy1);
    ras.area += (fy2 - fy1) * (fx1 + fx2);

End:
    ras.x = to_x;
    ras.y = to_y;
}

static void gray_split_conic(SW_FT_Vector* base)
{
    TPos  a, b;

    base[4].x = base[2].x;
    a = base[0].x + base[1].x;
    b = base[1].x + base[2].x;
    base[3].x = b >> 1;
    base[2].x = (a + b) >> 2;
    base[1].x = a >> 1;

    base[4].y = base[2].y;
    a = base[0].y + base[1].y;
    b = base[1].y + base[2].y;
    base[3].y = b >> 1;
    base[2].y = (a + b) >> 2;
    base[1].y = a >> 1;
}

static void gray_render_conic(RAS_ARG_ const SW_FT_Vector* control,
    const SW_FT_Vector* to)
{
    TPos          dx, dy;
    TPos          min, max, y;
    int           top, level;
    int* levels;
    SW_FT_Vector* arc;

    levels = ras.lev_stack;

    arc = ras.bez_stack;
    arc[0].x = UPSCALE(to->x);
    arc[0].y = UPSCALE(to->y);
    arc[1].x = UPSCALE(control->x);
    arc[1].y = UPSCALE(control->y);
    arc[2].x = ras.x;
    arc[2].y = ras.y;
    top = 0;

    dx = SW_FT_ABS(arc[2].x + arc[0].x - 2 * arc[1].x);
    dy = SW_FT_ABS(arc[2].y + arc[0].y - 2 * arc[1].y);
    if (dx < dy) dx = dy;

    if (dx < ONE_PIXEL / 4) goto Draw;

    /* short-cut the arc that crosses the current band */
    min = max = arc[0].y;

    y = arc[1].y;
    if (y < min) min = y;
    if (y > max) max = y;

    y = arc[2].y;
    if (y < min) min = y;
    if (y > max) max = y;

    if (TRUNC(min) >= ras.max_ey || TRUNC(max) < ras.min_ey) goto Draw;

    level = 0;
    do {
        dx >>= 2;
        level++;
    } while (dx > ONE_PIXEL / 4);

    levels[0] = level;

    do {
        level = levels[top];
        if (level > 0) {
            gray_split_conic(arc);
            arc += 2;
            top++;
            levels[top] = levels[top - 1] = level - 1;
            continue;
        }

    Draw:
        gray_render_line(RAS_VAR_ arc[0].x, arc[0].y);
        top--;
        arc -= 2;

    } while (top >= 0);
}

static void gray_split_cubic(SW_FT_Vector* base)
{
    TPos  a, b, c;


    base[6].x = base[3].x;
    a = base[0].x + base[1].x;
    b = base[1].x + base[2].x;
    c = base[2].x + base[3].x;
    base[5].x = c >> 1;
    c += b;
    base[4].x = c >> 2;
    base[1].x = a >> 1;
    a += b;
    base[2].x = a >> 2;
    base[3].x = (a + c) >> 3;

    base[6].y = base[3].y;
    a = base[0].y + base[1].y;
    b = base[1].y + base[2].y;
    c = base[2].y + base[3].y;
    base[5].y = c >> 1;
    c += b;
    base[4].y = c >> 2;
    base[1].y = a >> 1;
    a += b;
    base[2].y = a >> 2;
    base[3].y = (a + c) >> 3;
}


static void
gray_render_cubic(RAS_ARG_ const SW_FT_Vector* control1,
    const SW_FT_Vector* control2,
    const SW_FT_Vector* to)
{
    SW_FT_Vector* arc = ras.bez_stack;

    arc[0].x = UPSCALE(to->x);
    arc[0].y = UPSCALE(to->y);
    arc[1].x = UPSCALE(control2->x);
    arc[1].y = UPSCALE(control2->y);
    arc[2].x = UPSCALE(control1->x);
    arc[2].y = UPSCALE(control1->y);
    arc[3].x = ras.x;
    arc[3].y = ras.y;

    /* short-cut the arc that crosses the current band */
    if ((TRUNC(arc[0].y) >= ras.max_ey &&
        TRUNC(arc[1].y) >= ras.max_ey &&
        TRUNC(arc[2].y) >= ras.max_ey &&
        TRUNC(arc[3].y) >= ras.max_ey) ||
        (TRUNC(arc[0].y) < ras.min_ey &&
            TRUNC(arc[1].y) < ras.min_ey &&
            TRUNC(arc[2].y) < ras.min_ey &&
            TRUNC(arc[3].y) < ras.min_ey))
    {
        ras.x = arc[0].x;
        ras.y = arc[0].y;
        return;
    }

    for (;;)
    {
        /* with each split, control points quickly converge towards  */
        /* chord trisection points and the vanishing distances below */
        /* indicate when the segment is flat enough to draw          */
        if (SW_FT_ABS(2 * arc[0].x - 3 * arc[1].x + arc[3].x) > ONE_PIXEL / 2 ||
            SW_FT_ABS(2 * arc[0].y - 3 * arc[1].y + arc[3].y) > ONE_PIXEL / 2 ||
            SW_FT_ABS(arc[0].x - 3 * arc[2].x + 2 * arc[3].x) > ONE_PIXEL / 2 ||
            SW_FT_ABS(arc[0].y - 3 * arc[2].y + 2 * arc[3].y) > ONE_PIXEL / 2)
            goto Split;

        gray_render_line(RAS_VAR_ arc[0].x, arc[0].y);

        if (arc == ras.bez_stack)
            return;

        arc -= 3;
        continue;

    Split:
        gray_split_cubic(arc);
        arc += 3;
    }
}

static int gray_move_to(const SW_FT_Vector* to, gray_PWorker worker)
{
    TPos x, y;

    /* record current cell, if any */
    if (!ras.invalid) gray_record_cell(RAS_VAR);

    /* start to a new position */
    x = UPSCALE(to->x);
    y = UPSCALE(to->y);

    gray_start_cell(RAS_VAR_ TRUNC(x), TRUNC(y));

    worker->x = x;
    worker->y = y;
    return 0;
}

static int gray_line_to(const SW_FT_Vector* to, gray_PWorker worker)
{
    gray_render_line(RAS_VAR_ UPSCALE(to->x), UPSCALE(to->y));
    return 0;
}

static int gray_conic_to(const SW_FT_Vector* control, const SW_FT_Vector* to,
    gray_PWorker worker)
{
    gray_render_conic(RAS_VAR_ control, to);
    return 0;
}

static int gray_cubic_to(const SW_FT_Vector* control1,
    const SW_FT_Vector* control2, const SW_FT_Vector* to,
    gray_PWorker worker)
{
    gray_render_cubic(RAS_VAR_ control1, control2, to);
    return 0;
}

static void gray_hline(RAS_ARG_ TCoord x, TCoord y, TPos area, TCoord acount)
{
    int coverage;

    /* compute the coverage line's coverage, depending on the    */
    /* outline fill rule                                         */
    /*                                                           */
    /* the coverage percentage is area/(PIXEL_BITS*PIXEL_BITS*2) */
    /*                                                           */
    coverage = (int)(area >> (PIXEL_BITS * 2 + 1 - 8));
    /* use range 0..256 */
    if (coverage < 0) coverage = -coverage;

    if (ras.outline.flags & SW_FT_OUTLINE_EVEN_ODD_FILL) {
        coverage &= 511;

        if (coverage > 256)
            coverage = 512 - coverage;
        else if (coverage == 256)
            coverage = 255;
    }
    else {
        /* normal non-zero winding rule */
        if (coverage >= 256) coverage = 255;
    }

    y += (TCoord)ras.min_ey;
    x += (TCoord)ras.min_ex;

    /* SW_FT_Span.x is a 16-bit short, so limit our coordinates appropriately */
    if (x >= 32767) x = 32767;

    /* SW_FT_Span.y is an integer, so limit our coordinates appropriately */
    if (y >= SW_FT_INT_MAX) y = SW_FT_INT_MAX;

    if (coverage) {
        SW_FT_Span* span;
        int         count;

        // update bounding box.
        if (x < ras.bound_left) ras.bound_left = x;
        if (y < ras.bound_top) ras.bound_top = y;
        if (y > ras.bound_bottom) ras.bound_bottom = y;
        if (x + acount > ras.bound_right) ras.bound_right = x + acount;

        /* see whether we can add this span to the current list */
        count = ras.num_gray_spans;
        span = ras.gray_spans + count - 1;
        if (count > 0 && span->y == y && (int)span->x + span->len == (int)x &&
            span->coverage == coverage) {
            span->len = (unsigned short)(span->len + acount);
            return;
        }

        if (count >= SW_FT_MAX_GRAY_SPANS) {
            if (ras.render_span && count > 0)
                ras.render_span(count, ras.gray_spans, ras.render_span_data);

#ifdef DEBUG_GRAYS

            if (1) {
                int n;

                fprintf(stderr, "count = %3d ", count);
                span = ras.gray_spans;
                for (n = 0; n < count; n++, span++)
                    fprintf(stderr, "[%d , %d..%d] : %d ", span->y, span->x,
                        span->x + span->len - 1, span->coverage);
                fprintf(stderr, "\n");
            }

#endif /* DEBUG_GRAYS */

            ras.num_gray_spans = 0;

            span = ras.gray_spans;
        }
        else
            span++;

        /* add a gray span to the current list */
        span->x = (short)x;
        span->y = (short)y;
        span->len = (unsigned short)acount;
        span->coverage = (unsigned char)coverage;

        ras.num_gray_spans++;
    }
}

static void gray_sweep(RAS_ARG)
{
    int yindex;

    if (ras.num_cells == 0) return;

    ras.num_gray_spans = 0;

    for (yindex = 0; yindex < ras.ycount; yindex++) {
        PCell  cell = ras.ycells[yindex];
        TCoord cover = 0;
        TCoord x = 0;

        for (; cell != NULL; cell = cell->next) {
            TPos area;

            if (cell->x > x && cover != 0)
                gray_hline(RAS_VAR_ x, yindex, cover * (ONE_PIXEL * 2),
                    cell->x - x);

            cover += cell->cover;
            area = cover * (ONE_PIXEL * 2) - cell->area;

            if (area != 0 && cell->x >= 0)
                gray_hline(RAS_VAR_ cell->x, yindex, area, 1);

            x = cell->x + 1;
        }

        if (cover != 0)
            gray_hline(RAS_VAR_ x, yindex, cover * (ONE_PIXEL * 2),
                ras.count_ex - x);
    }

    if (ras.render_span && ras.num_gray_spans > 0)
        ras.render_span(ras.num_gray_spans, ras.gray_spans,
            ras.render_span_data);
}

/*************************************************************************/
/*                                                                       */
/*  The following function should only compile in stand-alone mode,      */
/*  i.e., when building this component without the rest of FreeType.     */
/*                                                                       */
/*************************************************************************/

/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    SW_FT_Outline_Decompose                                               */
/*                                                                       */
/* <Description>                                                         */
/*    Walk over an outline's structure to decompose it into individual   */
/*    segments and Bézier arcs.  This function is also able to emit      */
/*    `move to' and `close to' operations to indicate the start and end  */
/*    of new contours in the outline.                                    */
/*                                                                       */
/* <Input>                                                               */
/*    outline        :: A pointer to the source target.                  */
/*                                                                       */
/*    func_interface :: A table of `emitters', i.e., function pointers   */
/*                      called during decomposition to indicate path     */
/*                      operations.                                      */
/*                                                                       */
/* <InOut>                                                               */
/*    user           :: A typeless pointer which is passed to each       */
/*                      emitter during the decomposition.  It can be     */
/*                      used to store the state during the               */
/*                      decomposition.                                   */
/*                                                                       */
/* <Return>                                                              */
/*    Error code.  0 means success.                                      */
/*                                                                       */
static int SW_FT_Outline_Decompose(const SW_FT_Outline* outline,
    const SW_FT_Outline_Funcs* func_interface,
    void* user)
{
#undef SCALED
#define SCALED(x) (((x) << shift) - delta)

    SW_FT_Vector v_last;
    SW_FT_Vector v_control;
    SW_FT_Vector v_start;

    SW_FT_Vector* point;
    SW_FT_Vector* limit;
    char* tags;

    int error;

    int  n;     /* index of contour in outline     */
    int  first; /* index of first point in contour */
    char tag;   /* current point's state           */

    int  shift;
    TPos delta;

    if (!outline || !func_interface) return SW_FT_THROW(Invalid_Argument);

    shift = func_interface->shift;
    delta = func_interface->delta;
    first = 0;

    for (n = 0; n < outline->n_contours; n++) {
        int last; /* index of last point in contour */

        last = outline->contours[n];
        if (last < 0) goto Invalid_Outline;
        limit = outline->points + last;

        v_start = outline->points[first];
        v_start.x = SCALED(v_start.x);
        v_start.y = SCALED(v_start.y);

        v_last = outline->points[last];
        v_last.x = SCALED(v_last.x);
        v_last.y = SCALED(v_last.y);

        v_control = v_start;

        point = outline->points + first;
        tags = outline->tags + first;
        tag = SW_FT_CURVE_TAG(tags[0]);

        /* A contour cannot start with a cubic control point! */
        if (tag == SW_FT_CURVE_TAG_CUBIC) goto Invalid_Outline;

        /* check first point to determine origin */
        if (tag == SW_FT_CURVE_TAG_CONIC) {
            /* first point is conic control.  Yes, this happens. */
            if (SW_FT_CURVE_TAG(outline->tags[last]) == SW_FT_CURVE_TAG_ON) {
                /* start at last point if it is on the curve */
                v_start = v_last;
                limit--;
            }
            else {
                /* if both first and last points are conic,         */
                /* start at their middle and record its position    */
                /* for closure                                      */
                v_start.x = (v_start.x + v_last.x) / 2;
                v_start.y = (v_start.y + v_last.y) / 2;
            }
            point--;
            tags--;
        }

        error = func_interface->move_to(&v_start, user);
        if (error) goto Exit;

        while (point < limit) {
            point++;
            tags++;

            tag = SW_FT_CURVE_TAG(tags[0]);
            switch (tag) {
            case SW_FT_CURVE_TAG_ON: /* emit a single line_to */
            {
                SW_FT_Vector vec;

                vec.x = SCALED(point->x);
                vec.y = SCALED(point->y);

                error = func_interface->line_to(&vec, user);
                if (error) goto Exit;
                continue;
            }

            case SW_FT_CURVE_TAG_CONIC: /* consume conic arcs */
                v_control.x = SCALED(point->x);
                v_control.y = SCALED(point->y);

            Do_Conic:
                if (point < limit) {
                    SW_FT_Vector vec;
                    SW_FT_Vector v_middle;

                    point++;
                    tags++;
                    tag = SW_FT_CURVE_TAG(tags[0]);

                    vec.x = SCALED(point->x);
                    vec.y = SCALED(point->y);

                    if (tag == SW_FT_CURVE_TAG_ON) {
                        error =
                            func_interface->conic_to(&v_control, &vec, user);
                        if (error) goto Exit;
                        continue;
                    }

                    if (tag != SW_FT_CURVE_TAG_CONIC) goto Invalid_Outline;

                    v_middle.x = (v_control.x + vec.x) / 2;
                    v_middle.y = (v_control.y + vec.y) / 2;

                    error =
                        func_interface->conic_to(&v_control, &v_middle, user);
                    if (error) goto Exit;

                    v_control = vec;
                    goto Do_Conic;
                }

                error = func_interface->conic_to(&v_control, &v_start, user);
                goto Close;

            default: /* SW_FT_CURVE_TAG_CUBIC */
            {
                SW_FT_Vector vec1, vec2;

                if (point + 1 > limit ||
                    SW_FT_CURVE_TAG(tags[1]) != SW_FT_CURVE_TAG_CUBIC)
                    goto Invalid_Outline;

                point += 2;
                tags += 2;

                vec1.x = SCALED(point[-2].x);
                vec1.y = SCALED(point[-2].y);

                vec2.x = SCALED(point[-1].x);
                vec2.y = SCALED(point[-1].y);

                if (point <= limit) {
                    SW_FT_Vector vec;

                    vec.x = SCALED(point->x);
                    vec.y = SCALED(point->y);

                    error = func_interface->cubic_to(&vec1, &vec2, &vec, user);
                    if (error) goto Exit;
                    continue;
                }

                error = func_interface->cubic_to(&vec1, &vec2, &v_start, user);
                goto Close;
            }
            }
        }

        /* close the contour with a line segment */
        error = func_interface->line_to(&v_start, user);

    Close:
        if (error) goto Exit;

        first = last + 1;
    }

    return 0;

Exit:
    return error;

Invalid_Outline:
    return SW_FT_THROW(Invalid_Outline);
}

typedef struct gray_TBand_ {
    TPos min, max;

} gray_TBand;

SW_FT_DEFINE_OUTLINE_FUNCS(func_interface,
    (SW_FT_Outline_MoveTo_Func)gray_move_to,
    (SW_FT_Outline_LineTo_Func)gray_line_to,
    (SW_FT_Outline_ConicTo_Func)gray_conic_to,
    (SW_FT_Outline_CubicTo_Func)gray_cubic_to, 0, 0)

    static int gray_convert_glyph_inner(RAS_ARG)
{
    volatile int error = 0;

    if (ft_setjmp(ras.jump_buffer) == 0) {
        error = SW_FT_Outline_Decompose(&ras.outline, &func_interface, &ras);
        if (!ras.invalid) gray_record_cell(RAS_VAR);
    }
    else
        error = SW_FT_THROW(Memory_Overflow);

    return error;
}

static int gray_convert_glyph(RAS_ARG)
{
    gray_TBand bands[40];
    gray_TBand* volatile band;
    int volatile n, num_bands;
    TPos volatile min, max, max_y;
    SW_FT_BBox* clip;

    /* Set up state in the raster object */
    gray_compute_cbox(RAS_VAR);

    /* clip to target bitmap, exit if nothing to do */
    clip = &ras.clip_box;

    if (ras.max_ex <= clip->xMin || ras.min_ex >= clip->xMax ||
        ras.max_ey <= clip->yMin || ras.min_ey >= clip->yMax)
        return 0;

    if (ras.min_ex < clip->xMin) ras.min_ex = clip->xMin;
    if (ras.min_ey < clip->yMin) ras.min_ey = clip->yMin;

    if (ras.max_ex > clip->xMax) ras.max_ex = clip->xMax;
    if (ras.max_ey > clip->yMax) ras.max_ey = clip->yMax;

    ras.count_ex = ras.max_ex - ras.min_ex;
    ras.count_ey = ras.max_ey - ras.min_ey;

    /* set up vertical bands */
    num_bands = (int)((ras.max_ey - ras.min_ey) / ras.band_size);
    if (num_bands == 0) num_bands = 1;
    if (num_bands >= 39) num_bands = 39;

    ras.band_shoot = 0;

    min = ras.min_ey;
    max_y = ras.max_ey;

    for (n = 0; n < num_bands; n++, min = max) {
        max = min + ras.band_size;
        if (n == num_bands - 1 || max > max_y) max = max_y;

        bands[0].min = min;
        bands[0].max = max;
        band = bands;

        while (band >= bands) {
            TPos bottom, top, middle;
            int  error;

            {
                PCell cells_max;
                int   yindex;
                long  cell_start, cell_end, cell_mod;

                ras.ycells = (PCell*)ras.buffer;
                ras.ycount = band->max - band->min;

                cell_start = sizeof(PCell) * ras.ycount;
                cell_mod = cell_start % sizeof(TCell);
                if (cell_mod > 0) cell_start += sizeof(TCell) - cell_mod;

                cell_end = ras.buffer_size;
                cell_end -= cell_end % sizeof(TCell);

                cells_max = (PCell)((char*)ras.buffer + cell_end);
                ras.cells = (PCell)((char*)ras.buffer + cell_start);
                if (ras.cells >= cells_max) goto ReduceBands;

                ras.max_cells = cells_max - ras.cells;
                if (ras.max_cells < 2) goto ReduceBands;

                for (yindex = 0; yindex < ras.ycount; yindex++)
                    ras.ycells[yindex] = NULL;
            }

            ras.num_cells = 0;
            ras.invalid = 1;
            ras.min_ey = band->min;
            ras.max_ey = band->max;
            ras.count_ey = band->max - band->min;

            error = gray_convert_glyph_inner(RAS_VAR);

            if (!error) {
                gray_sweep(RAS_VAR);
                band--;
                continue;
            }
            else if (error != ErrRaster_Memory_Overflow)
                return 1;

        ReduceBands:
            /* render pool overflow; we will reduce the render band by half */
            bottom = band->min;
            top = band->max;
            middle = bottom + ((top - bottom) >> 1);

            /* This is too complex for a single scanline; there must */
            /* be some problems.                                     */
            if (middle == bottom) {
                return 1;
            }

            if (bottom - top >= ras.band_size) ras.band_shoot++;

            band[1].min = bottom;
            band[1].max = middle;
            band[0].min = middle;
            band[0].max = top;
            band++;
        }
    }

    if (ras.band_shoot > 8 && ras.band_size > 16)
        ras.band_size = ras.band_size / 2;

    return 0;
}

static int gray_raster_render(gray_PRaster               raster,
    const SW_FT_Raster_Params* params)
{
    SW_FT_UNUSED(raster);
    const SW_FT_Outline* outline = (const SW_FT_Outline*)params->source;

    gray_TWorker worker[1];

    TCell buffer[SW_FT_RENDER_POOL_SIZE / sizeof(TCell)];
    long  buffer_size = sizeof(buffer);
    int   band_size = (int)(buffer_size / (long)(sizeof(TCell) * 8));

    if (!outline) return SW_FT_THROW(Invalid_Outline);

    /* return immediately if the outline is empty */
    if (outline->n_points == 0 || outline->n_contours <= 0) return 0;

    if (!outline->contours || !outline->points)
        return SW_FT_THROW(Invalid_Outline);

    if (outline->n_points != outline->contours[outline->n_contours - 1] + 1)
        return SW_FT_THROW(Invalid_Outline);

    /* this version does not support monochrome rendering */
    if (!(params->flags & SW_FT_RASTER_FLAG_AA))
        return SW_FT_THROW(Invalid_Mode);

    if (params->flags & SW_FT_RASTER_FLAG_CLIP)
        ras.clip_box = params->clip_box;
    else {
        ras.clip_box.xMin = -32768L;
        ras.clip_box.yMin = -32768L;
        ras.clip_box.xMax = 32767L;
        ras.clip_box.yMax = 32767L;
    }

    gray_init_cells(RAS_VAR_ buffer, buffer_size);

    ras.outline = *outline;
    ras.num_cells = 0;
    ras.invalid = 1;
    ras.band_size = band_size;
    ras.num_gray_spans = 0;

    ras.render_span = (SW_FT_Raster_Span_Func)params->gray_spans;
    ras.render_span_data = params->user;

    gray_convert_glyph(RAS_VAR);
    params->bbox_cb(ras.bound_left, ras.bound_top,
        ras.bound_right - ras.bound_left,
        ras.bound_bottom - ras.bound_top + 1, params->user);
    return 1;
}

/**** RASTER OBJECT CREATION: In stand-alone mode, we simply use *****/
/****                         a static object.                   *****/

static int gray_raster_new(SW_FT_Raster* araster)
{
    static gray_TRaster the_raster;

    *araster = (SW_FT_Raster)&the_raster;
    SW_FT_MEM_ZERO(&the_raster, sizeof(the_raster));

    return 0;
}

static void gray_raster_done(SW_FT_Raster raster)
{
    /* nothing */
    SW_FT_UNUSED(raster);
}

static void gray_raster_reset(SW_FT_Raster raster, char* pool_base,
    long pool_size)
{
    SW_FT_UNUSED(raster);
    SW_FT_UNUSED(pool_base);
    SW_FT_UNUSED(pool_size);
}

SW_FT_DEFINE_RASTER_FUNCS(sw_ft_grays_raster,

    (SW_FT_Raster_New_Func)gray_raster_new,
    (SW_FT_Raster_Reset_Func)gray_raster_reset,
    (SW_FT_Raster_Render_Func)gray_raster_render,
    (SW_FT_Raster_Done_Func)gray_raster_done)

    /* END */


/***************************************************************************/
/*                                                                         */
/*  ftstroke.c                                                             */
/*                                                                         */
/*    FreeType path stroker (body).                                        */
/*                                                                         */
/*  Copyright 2002-2006, 2008-2011, 2013 by                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                      BEZIER COMPUTATIONS                      *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

#define SW_FT_SMALL_CONIC_THRESHOLD (SW_FT_ANGLE_PI / 6)
#define SW_FT_SMALL_CUBIC_THRESHOLD (SW_FT_ANGLE_PI / 8)

#define SW_FT_EPSILON 2

#define SW_FT_IS_SMALL(x) ((x) > -SW_FT_EPSILON && (x) < SW_FT_EPSILON)

    static SW_FT_Pos ft_pos_abs(SW_FT_Pos x)
{
    return x >= 0 ? x : -x;
}

static void ft_conic_split(SW_FT_Vector* base)
{
    SW_FT_Pos a, b;

    base[4].x = base[2].x;
    a = base[0].x + base[1].x;
    b = base[1].x + base[2].x;
    base[3].x = b >> 1;
    base[2].x = (a + b) >> 2;
    base[1].x = a >> 1;

    base[4].y = base[2].y;
    a = base[0].y + base[1].y;
    b = base[1].y + base[2].y;
    base[3].y = b >> 1;
    base[2].y = (a + b) >> 2;
    base[1].y = a >> 1;
}

static SW_FT_Bool ft_conic_is_small_enough(SW_FT_Vector* base,
    SW_FT_Angle* angle_in,
    SW_FT_Angle* angle_out)
{
    SW_FT_Vector d1, d2;
    SW_FT_Angle  theta;
    SW_FT_Int    close1, close2;

    d1.x = base[1].x - base[2].x;
    d1.y = base[1].y - base[2].y;
    d2.x = base[0].x - base[1].x;
    d2.y = base[0].y - base[1].y;

    close1 = SW_FT_IS_SMALL(d1.x) && SW_FT_IS_SMALL(d1.y);
    close2 = SW_FT_IS_SMALL(d2.x) && SW_FT_IS_SMALL(d2.y);

    if (close1) {
        if (close2) {
            /* basically a point;                      */
            /* do nothing to retain original direction */
        }
        else {
            *angle_in = *angle_out = SW_FT_Atan2(d2.x, d2.y);
        }
    }
    else /* !close1 */
    {
        if (close2) {
            *angle_in = *angle_out = SW_FT_Atan2(d1.x, d1.y);
        }
        else {
            *angle_in = SW_FT_Atan2(d1.x, d1.y);
            *angle_out = SW_FT_Atan2(d2.x, d2.y);
        }
    }

    theta = ft_pos_abs(SW_FT_Angle_Diff(*angle_in, *angle_out));

    return SW_FT_BOOL(theta < SW_FT_SMALL_CONIC_THRESHOLD);
}

static void ft_cubic_split(SW_FT_Vector* base)
{
    SW_FT_Pos a, b, c;

    base[6].x = base[3].x;
    a = base[0].x + base[1].x;
    b = base[1].x + base[2].x;
    c = base[2].x + base[3].x;
    base[5].x = c >> 1;
    c += b;
    base[4].x = c >> 2;
    base[1].x = a >> 1;
    a += b;
    base[2].x = a >> 2;
    base[3].x = (a + c) >> 3;

    base[6].y = base[3].y;
    a = base[0].y + base[1].y;
    b = base[1].y + base[2].y;
    c = base[2].y + base[3].y;
    base[5].y = c >> 1;
    c += b;
    base[4].y = c >> 2;
    base[1].y = a >> 1;
    a += b;
    base[2].y = a >> 2;
    base[3].y = (a + c) >> 3;
}

/* Return the average of `angle1' and `angle2'.            */
/* This gives correct result even if `angle1' and `angle2' */
/* have opposite signs.                                    */
static SW_FT_Angle ft_angle_mean(SW_FT_Angle angle1, SW_FT_Angle angle2)
{
    return angle1 + SW_FT_Angle_Diff(angle1, angle2) / 2;
}

static SW_FT_Bool ft_cubic_is_small_enough(SW_FT_Vector* base,
    SW_FT_Angle* angle_in,
    SW_FT_Angle* angle_mid,
    SW_FT_Angle* angle_out)
{
    SW_FT_Vector d1, d2, d3;
    SW_FT_Angle  theta1, theta2;
    SW_FT_Int    close1, close2, close3;

    d1.x = base[2].x - base[3].x;
    d1.y = base[2].y - base[3].y;
    d2.x = base[1].x - base[2].x;
    d2.y = base[1].y - base[2].y;
    d3.x = base[0].x - base[1].x;
    d3.y = base[0].y - base[1].y;

    close1 = SW_FT_IS_SMALL(d1.x) && SW_FT_IS_SMALL(d1.y);
    close2 = SW_FT_IS_SMALL(d2.x) && SW_FT_IS_SMALL(d2.y);
    close3 = SW_FT_IS_SMALL(d3.x) && SW_FT_IS_SMALL(d3.y);

    if (close1) {
        if (close2) {
            if (close3) {
                /* basically a point;                      */
                /* do nothing to retain original direction */
            }
            else /* !close3 */
            {
                *angle_in = *angle_mid = *angle_out = SW_FT_Atan2(d3.x, d3.y);
            }
        }
        else /* !close2 */
        {
            if (close3) {
                *angle_in = *angle_mid = *angle_out = SW_FT_Atan2(d2.x, d2.y);
            }
            else /* !close3 */
            {
                *angle_in = *angle_mid = SW_FT_Atan2(d2.x, d2.y);
                *angle_out = SW_FT_Atan2(d3.x, d3.y);
            }
        }
    }
    else /* !close1 */
    {
        if (close2) {
            if (close3) {
                *angle_in = *angle_mid = *angle_out = SW_FT_Atan2(d1.x, d1.y);
            }
            else /* !close3 */
            {
                *angle_in = SW_FT_Atan2(d1.x, d1.y);
                *angle_out = SW_FT_Atan2(d3.x, d3.y);
                *angle_mid = ft_angle_mean(*angle_in, *angle_out);
            }
        }
        else /* !close2 */
        {
            if (close3) {
                *angle_in = SW_FT_Atan2(d1.x, d1.y);
                *angle_mid = *angle_out = SW_FT_Atan2(d2.x, d2.y);
            }
            else /* !close3 */
            {
                *angle_in = SW_FT_Atan2(d1.x, d1.y);
                *angle_mid = SW_FT_Atan2(d2.x, d2.y);
                *angle_out = SW_FT_Atan2(d3.x, d3.y);
            }
        }
    }

    theta1 = ft_pos_abs(SW_FT_Angle_Diff(*angle_in, *angle_mid));
    theta2 = ft_pos_abs(SW_FT_Angle_Diff(*angle_mid, *angle_out));

    return SW_FT_BOOL(theta1 < SW_FT_SMALL_CUBIC_THRESHOLD&&
        theta2 < SW_FT_SMALL_CUBIC_THRESHOLD);
}

/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                       STROKE BORDERS                          *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

typedef enum SW_FT_StrokeTags_ {
    SW_FT_STROKE_TAG_ON = 1,    /* on-curve point  */
    SW_FT_STROKE_TAG_CUBIC = 2, /* cubic off-point */
    SW_FT_STROKE_TAG_BEGIN = 4, /* sub-path start  */
    SW_FT_STROKE_TAG_END = 8    /* sub-path end    */

} SW_FT_StrokeTags;

#define SW_FT_STROKE_TAG_BEGIN_END \
    (SW_FT_STROKE_TAG_BEGIN | SW_FT_STROKE_TAG_END)

typedef struct SW_FT_StrokeBorderRec_ {
    SW_FT_UInt    num_points;
    SW_FT_UInt    max_points;
    SW_FT_Vector* points;
    SW_FT_Byte* tags;
    SW_FT_Bool    movable; /* TRUE for ends of lineto borders */
    SW_FT_Int     start;   /* index of current sub-path start point */
    SW_FT_Bool    valid;

} SW_FT_StrokeBorderRec, * SW_FT_StrokeBorder;

SW_FT_Error SW_FT_Outline_Check(SW_FT_Outline* outline)
{
    if (outline) {
        SW_FT_Int n_points = outline->n_points;
        SW_FT_Int n_contours = outline->n_contours;
        SW_FT_Int end0, end;
        SW_FT_Int n;

        /* empty glyph? */
        if (n_points == 0 && n_contours == 0) return 0;

        /* check point and contour counts */
        if (n_points <= 0 || n_contours <= 0) goto Bad;

        end0 = end = -1;
        for (n = 0; n < n_contours; n++) {
            end = outline->contours[n];

            /* note that we don't accept empty contours */
            if (end <= end0 || end >= n_points) goto Bad;

            end0 = end;
        }

        if (end != n_points - 1) goto Bad;

        /* XXX: check the tags array */
        return 0;
    }

Bad:
    return -1;  // SW_FT_THROW( Invalid_Argument );
}

void SW_FT_Outline_Get_CBox(const SW_FT_Outline* outline, SW_FT_BBox* acbox)
{
    SW_FT_Pos xMin, yMin, xMax, yMax;

    if (outline && acbox) {
        if (outline->n_points == 0) {
            xMin = 0;
            yMin = 0;
            xMax = 0;
            yMax = 0;
        }
        else {
            SW_FT_Vector* vec = outline->points;
            SW_FT_Vector* limit = vec + outline->n_points;

            xMin = xMax = vec->x;
            yMin = yMax = vec->y;
            vec++;

            for (; vec < limit; vec++) {
                SW_FT_Pos x, y;

                x = vec->x;
                if (x < xMin) xMin = x;
                if (x > xMax) xMax = x;

                y = vec->y;
                if (y < yMin) yMin = y;
                if (y > yMax) yMax = y;
            }
        }
        acbox->xMin = xMin;
        acbox->xMax = xMax;
        acbox->yMin = yMin;
        acbox->yMax = yMax;
    }
}

static SW_FT_Error ft_stroke_border_grow(SW_FT_StrokeBorder border,
    SW_FT_UInt         new_points)
{
    SW_FT_UInt  old_max = border->max_points;
    SW_FT_UInt  new_max = border->num_points + new_points;
    SW_FT_Error error = 0;

    if (new_max > old_max) {
        SW_FT_UInt cur_max = old_max;

        while (cur_max < new_max) cur_max += (cur_max >> 1) + 16;

        border->points = (SW_FT_Vector*)realloc(border->points,
            cur_max * sizeof(SW_FT_Vector));
        border->tags =
            (SW_FT_Byte*)realloc(border->tags, cur_max * sizeof(SW_FT_Byte));

        if (!border->points || !border->tags) goto Exit;

        border->max_points = cur_max;
    }

Exit:
    return error;
}

static void ft_stroke_border_close(SW_FT_StrokeBorder border,
    SW_FT_Bool         reverse)
{
    SW_FT_UInt start = border->start;
    SW_FT_UInt count = border->num_points;

    assert(border->start >= 0);

    /* don't record empty paths! */
    if (count <= start + 1U)
        border->num_points = start;
    else {
        /* copy the last point to the start of this sub-path, since */
        /* it contains the `adjusted' starting coordinates          */
        border->num_points = --count;
        border->points[start] = border->points[count];

        if (reverse) {
            /* reverse the points */
            {
                SW_FT_Vector* vec1 = border->points + start + 1;
                SW_FT_Vector* vec2 = border->points + count - 1;

                for (; vec1 < vec2; vec1++, vec2--) {
                    SW_FT_Vector tmp;

                    tmp = *vec1;
                    *vec1 = *vec2;
                    *vec2 = tmp;
                }
            }

            /* then the tags */
            {
                SW_FT_Byte* tag1 = border->tags + start + 1;
                SW_FT_Byte* tag2 = border->tags + count - 1;

                for (; tag1 < tag2; tag1++, tag2--) {
                    SW_FT_Byte tmp;

                    tmp = *tag1;
                    *tag1 = *tag2;
                    *tag2 = tmp;
                }
            }
        }

        border->tags[start] |= SW_FT_STROKE_TAG_BEGIN;
        border->tags[count - 1] |= SW_FT_STROKE_TAG_END;
    }

    border->start = -1;
    border->movable = FALSE;
}

static SW_FT_Error ft_stroke_border_lineto(SW_FT_StrokeBorder border,
    SW_FT_Vector* to, SW_FT_Bool movable)
{
    SW_FT_Error error = 0;

    assert(border->start >= 0);

    if (border->movable) {
        /* move last point */
        border->points[border->num_points - 1] = *to;
    }
    else {
        /* don't add zero-length lineto */
        if (border->num_points > 0 &&
            SW_FT_IS_SMALL(border->points[border->num_points - 1].x - to->x) &&
            SW_FT_IS_SMALL(border->points[border->num_points - 1].y - to->y))
            return error;

        /* add one point */
        error = ft_stroke_border_grow(border, 1);
        if (!error) {
            SW_FT_Vector* vec = border->points + border->num_points;
            SW_FT_Byte* tag = border->tags + border->num_points;

            vec[0] = *to;
            tag[0] = SW_FT_STROKE_TAG_ON;

            border->num_points += 1;
        }
    }
    border->movable = movable;
    return error;
}

static SW_FT_Error ft_stroke_border_conicto(SW_FT_StrokeBorder border,
    SW_FT_Vector* control,
    SW_FT_Vector* to)
{
    SW_FT_Error error;

    assert(border->start >= 0);

    error = ft_stroke_border_grow(border, 2);
    if (!error) {
        SW_FT_Vector* vec = border->points + border->num_points;
        SW_FT_Byte* tag = border->tags + border->num_points;

        vec[0] = *control;
        vec[1] = *to;

        tag[0] = 0;
        tag[1] = SW_FT_STROKE_TAG_ON;

        border->num_points += 2;
    }

    border->movable = FALSE;

    return error;
}

static SW_FT_Error ft_stroke_border_cubicto(SW_FT_StrokeBorder border,
    SW_FT_Vector* control1,
    SW_FT_Vector* control2,
    SW_FT_Vector* to)
{
    SW_FT_Error error;

    assert(border->start >= 0);

    error = ft_stroke_border_grow(border, 3);
    if (!error) {
        SW_FT_Vector* vec = border->points + border->num_points;
        SW_FT_Byte* tag = border->tags + border->num_points;

        vec[0] = *control1;
        vec[1] = *control2;
        vec[2] = *to;

        tag[0] = SW_FT_STROKE_TAG_CUBIC;
        tag[1] = SW_FT_STROKE_TAG_CUBIC;
        tag[2] = SW_FT_STROKE_TAG_ON;

        border->num_points += 3;
    }

    border->movable = FALSE;

    return error;
}

#define SW_FT_ARC_CUBIC_ANGLE (SW_FT_ANGLE_PI / 2)


static SW_FT_Error
ft_stroke_border_arcto(SW_FT_StrokeBorder  border,
    SW_FT_Vector* center,
    SW_FT_Fixed         radius,
    SW_FT_Angle         angle_start,
    SW_FT_Angle         angle_diff)
{
    SW_FT_Fixed   coef;
    SW_FT_Vector  a0, a1, a2, a3;
    SW_FT_Int     i, arcs = 1;
    SW_FT_Error   error = 0;


    /* number of cubic arcs to draw */
    while (angle_diff > SW_FT_ARC_CUBIC_ANGLE * arcs ||
        -angle_diff > SW_FT_ARC_CUBIC_ANGLE * arcs)
        arcs++;

    /* control tangents */
    coef = SW_FT_Tan(angle_diff / (4 * arcs));
    coef += coef / 3;

    /* compute start and first control point */
    SW_FT_Vector_From_Polar(&a0, radius, angle_start);
    a1.x = SW_FT_MulFix(-a0.y, coef);
    a1.y = SW_FT_MulFix(a0.x, coef);

    a0.x += center->x;
    a0.y += center->y;
    a1.x += a0.x;
    a1.y += a0.y;

    for (i = 1; i <= arcs; i++)
    {
        /* compute end and second control point */
        SW_FT_Vector_From_Polar(&a3, radius,
            angle_start + i * angle_diff / arcs);
        a2.x = SW_FT_MulFix(a3.y, coef);
        a2.y = SW_FT_MulFix(-a3.x, coef);

        a3.x += center->x;
        a3.y += center->y;
        a2.x += a3.x;
        a2.y += a3.y;

        /* add cubic arc */
        error = ft_stroke_border_cubicto(border, &a1, &a2, &a3);
        if (error)
            break;

        /* a0 = a3; */
        a1.x = a3.x - a2.x + a3.x;
        a1.y = a3.y - a2.y + a3.y;
    }

    return error;
}

static SW_FT_Error ft_stroke_border_moveto(SW_FT_StrokeBorder border,
    SW_FT_Vector* to)
{
    /* close current open path if any ? */
    if (border->start >= 0) ft_stroke_border_close(border, FALSE);

    border->start = border->num_points;
    border->movable = FALSE;

    return ft_stroke_border_lineto(border, to, FALSE);
}

static void ft_stroke_border_init(SW_FT_StrokeBorder border)
{
    border->points = NULL;
    border->tags = NULL;

    border->num_points = 0;
    border->max_points = 0;
    border->start = -1;
    border->valid = FALSE;
}

static void ft_stroke_border_reset(SW_FT_StrokeBorder border)
{
    border->num_points = 0;
    border->start = -1;
    border->valid = FALSE;
}

static void ft_stroke_border_done(SW_FT_StrokeBorder border)
{
    free(border->points);
    free(border->tags);

    border->num_points = 0;
    border->max_points = 0;
    border->start = -1;
    border->valid = FALSE;
}

static SW_FT_Error ft_stroke_border_get_counts(SW_FT_StrokeBorder border,
    SW_FT_UInt* anum_points,
    SW_FT_UInt* anum_contours)
{
    SW_FT_Error error = 0;
    SW_FT_UInt  num_points = 0;
    SW_FT_UInt  num_contours = 0;

    SW_FT_UInt    count = border->num_points;
    SW_FT_Vector* point = border->points;
    SW_FT_Byte* tags = border->tags;
    SW_FT_Int     in_contour = 0;

    for (; count > 0; count--, num_points++, point++, tags++) {
        if (tags[0] & SW_FT_STROKE_TAG_BEGIN) {
            if (in_contour != 0) goto Fail;

            in_contour = 1;
        }
        else if (in_contour == 0)
            goto Fail;

        if (tags[0] & SW_FT_STROKE_TAG_END) {
            in_contour = 0;
            num_contours++;
        }
    }

    if (in_contour != 0) goto Fail;

    border->valid = TRUE;

Exit:
    *anum_points = num_points;
    *anum_contours = num_contours;
    return error;

Fail:
    num_points = 0;
    num_contours = 0;
    goto Exit;
}

static void ft_stroke_border_export(SW_FT_StrokeBorder border,
    SW_FT_Outline* outline)
{
    /* copy point locations */
    memcpy(outline->points + outline->n_points, border->points,
        border->num_points * sizeof(SW_FT_Vector));

    /* copy tags */
    {
        SW_FT_UInt  count = border->num_points;
        SW_FT_Byte* read = border->tags;
        SW_FT_Byte* write = (SW_FT_Byte*)outline->tags + outline->n_points;

        for (; count > 0; count--, read++, write++) {
            if (*read & SW_FT_STROKE_TAG_ON)
                *write = SW_FT_CURVE_TAG_ON;
            else if (*read & SW_FT_STROKE_TAG_CUBIC)
                *write = SW_FT_CURVE_TAG_CUBIC;
            else
                *write = SW_FT_CURVE_TAG_CONIC;
        }
    }

    /* copy contours */
    {
        SW_FT_UInt   count = border->num_points;
        SW_FT_Byte* tags = border->tags;
        SW_FT_Short* write = outline->contours + outline->n_contours;
        SW_FT_Short  idx = (SW_FT_Short)outline->n_points;

        for (; count > 0; count--, tags++, idx++) {
            if (*tags & SW_FT_STROKE_TAG_END) {
                *write++ = idx;
                outline->n_contours++;
            }
        }
    }

    outline->n_points = (short)(outline->n_points + border->num_points);

    assert(SW_FT_Outline_Check(outline) == 0);
}

/*************************************************************************/
/*************************************************************************/
/*****                                                               *****/
/*****                           STROKER                             *****/
/*****                                                               *****/
/*************************************************************************/
/*************************************************************************/

#define SW_FT_SIDE_TO_ROTATE(s) (SW_FT_ANGLE_PI2 - (s)*SW_FT_ANGLE_PI)

typedef struct SW_FT_StrokerRec_ {
    SW_FT_Angle  angle_in;            /* direction into curr join */
    SW_FT_Angle  angle_out;           /* direction out of join  */
    SW_FT_Vector center;              /* current position */
    SW_FT_Fixed  line_length;         /* length of last lineto */
    SW_FT_Bool   first_point;         /* is this the start? */
    SW_FT_Bool   subpath_open;        /* is the subpath open? */
    SW_FT_Angle  subpath_angle;       /* subpath start direction */
    SW_FT_Vector subpath_start;       /* subpath start position */
    SW_FT_Fixed  subpath_line_length; /* subpath start lineto len */
    SW_FT_Bool   handle_wide_strokes; /* use wide strokes logic? */

    SW_FT_Stroker_LineCap  line_cap;
    SW_FT_Stroker_LineJoin line_join;
    SW_FT_Stroker_LineJoin line_join_saved;
    SW_FT_Fixed            miter_limit;
    SW_FT_Fixed            radius;

    SW_FT_StrokeBorderRec borders[2];
} SW_FT_StrokerRec;

/* documentation is in ftstroke.h */

SW_FT_Error SW_FT_Stroker_New(SW_FT_Stroker* astroker)
{
    SW_FT_Error   error = 0; /* assigned in SW_FT_NEW */
    SW_FT_Stroker stroker = NULL;

    stroker = (SW_FT_StrokerRec*)calloc(1, sizeof(SW_FT_StrokerRec));
    if (stroker) {
        ft_stroke_border_init(&stroker->borders[0]);
        ft_stroke_border_init(&stroker->borders[1]);
    }

    *astroker = stroker;

    return error;
}

void SW_FT_Stroker_Rewind(SW_FT_Stroker stroker)
{
    if (stroker) {
        ft_stroke_border_reset(&stroker->borders[0]);
        ft_stroke_border_reset(&stroker->borders[1]);
    }
}

/* documentation is in ftstroke.h */

void SW_FT_Stroker_Set(SW_FT_Stroker stroker, SW_FT_Fixed radius,
    SW_FT_Stroker_LineCap  line_cap,
    SW_FT_Stroker_LineJoin line_join,
    SW_FT_Fixed            miter_limit)
{
    stroker->radius = radius;
    stroker->line_cap = line_cap;
    stroker->line_join = line_join;
    stroker->miter_limit = miter_limit;

    /* ensure miter limit has sensible value */
    if (stroker->miter_limit < 0x10000) stroker->miter_limit = 0x10000;

    /* save line join style:                                           */
    /* line join style can be temporarily changed when stroking curves */
    stroker->line_join_saved = line_join;

    SW_FT_Stroker_Rewind(stroker);
}

/* documentation is in ftstroke.h */

void SW_FT_Stroker_Done(SW_FT_Stroker stroker)
{
    if (stroker) {
        ft_stroke_border_done(&stroker->borders[0]);
        ft_stroke_border_done(&stroker->borders[1]);

        free(stroker);
    }
}

/* create a circular arc at a corner or cap */
static SW_FT_Error ft_stroker_arcto(SW_FT_Stroker stroker, SW_FT_Int side)
{
    SW_FT_Angle        total, rotate;
    SW_FT_Fixed        radius = stroker->radius;
    SW_FT_Error        error = 0;
    SW_FT_StrokeBorder border = stroker->borders + side;

    rotate = SW_FT_SIDE_TO_ROTATE(side);

    total = SW_FT_Angle_Diff(stroker->angle_in, stroker->angle_out);
    if (total == SW_FT_ANGLE_PI) total = -rotate * 2;

    error = ft_stroke_border_arcto(border, &stroker->center, radius,
        stroker->angle_in + rotate, total);
    border->movable = FALSE;
    return error;
}

/* add a cap at the end of an opened path */
static SW_FT_Error
ft_stroker_cap(SW_FT_Stroker stroker,
    SW_FT_Angle angle,
    SW_FT_Int side)
{
    SW_FT_Error error = 0;

    if (stroker->line_cap == SW_FT_STROKER_LINECAP_ROUND)
    {
        /* add a round cap */
        stroker->angle_in = angle;
        stroker->angle_out = angle + SW_FT_ANGLE_PI;

        error = ft_stroker_arcto(stroker, side);
    }
    else
    {
        /* add a square or butt cap */
        SW_FT_Vector        middle, delta;
        SW_FT_Fixed         radius = stroker->radius;
        SW_FT_StrokeBorder  border = stroker->borders + side;

        /* compute middle point and first angle point */
        SW_FT_Vector_From_Polar(&middle, radius, angle);
        delta.x = side ? middle.y : -middle.y;
        delta.y = side ? -middle.x : middle.x;

        if (stroker->line_cap == SW_FT_STROKER_LINECAP_SQUARE)
        {
            middle.x += stroker->center.x;
            middle.y += stroker->center.y;
        }
        else  /* SW_FT_STROKER_LINECAP_BUTT */
        {
            middle.x = stroker->center.x;
            middle.y = stroker->center.y;
        }

        delta.x += middle.x;
        delta.y += middle.y;

        error = ft_stroke_border_lineto(border, &delta, FALSE);
        if (error)
            goto Exit;

        /* compute second angle point */
        delta.x = middle.x - delta.x + middle.x;
        delta.y = middle.y - delta.y + middle.y;

        error = ft_stroke_border_lineto(border, &delta, FALSE);
    }

Exit:
    return error;
}

/* process an inside corner, i.e. compute intersection */
static SW_FT_Error ft_stroker_inside(SW_FT_Stroker stroker, SW_FT_Int side,
    SW_FT_Fixed line_length)
{
    SW_FT_StrokeBorder border = stroker->borders + side;
    SW_FT_Angle        phi, theta, rotate;
    SW_FT_Fixed        length;
    SW_FT_Vector       sigma, delta;
    SW_FT_Error        error = 0;
    SW_FT_Bool         intersect; /* use intersection of lines? */

    rotate = SW_FT_SIDE_TO_ROTATE(side);

    theta = SW_FT_Angle_Diff(stroker->angle_in, stroker->angle_out) / 2;

    /* Only intersect borders if between two lineto's and both */
    /* lines are long enough (line_length is zero for curves). */
    if (!border->movable || line_length == 0 ||
        theta > 0x59C000 || theta < -0x59C000)
        intersect = FALSE;
    else {
        /* compute minimum required length of lines */
        SW_FT_Fixed  min_length;


        SW_FT_Vector_Unit(&sigma, theta);
        min_length =
            ft_pos_abs(SW_FT_MulDiv(stroker->radius, sigma.y, sigma.x));

        intersect = SW_FT_BOOL(min_length &&
            stroker->line_length >= min_length &&
            line_length >= min_length);
    }

    if (!intersect) {
        SW_FT_Vector_From_Polar(&delta, stroker->radius,
            stroker->angle_out + rotate);
        delta.x += stroker->center.x;
        delta.y += stroker->center.y;

        border->movable = FALSE;
    }
    else {
        /* compute median angle */
        phi = stroker->angle_in + theta + rotate;

        length = SW_FT_DivFix(stroker->radius, sigma.x);

        SW_FT_Vector_From_Polar(&delta, length, phi);
        delta.x += stroker->center.x;
        delta.y += stroker->center.y;
    }

    error = ft_stroke_border_lineto(border, &delta, FALSE);

    return error;
}

/* process an outside corner, i.e. compute bevel/miter/round */
static SW_FT_Error
ft_stroker_outside(SW_FT_Stroker  stroker,
    SW_FT_Int      side,
    SW_FT_Fixed    line_length)
{
    SW_FT_StrokeBorder  border = stroker->borders + side;
    SW_FT_Error         error;
    SW_FT_Angle         rotate;


    if (stroker->line_join == SW_FT_STROKER_LINEJOIN_ROUND)
        error = ft_stroker_arcto(stroker, side);
    else
    {
        /* this is a mitered (pointed) or beveled (truncated) corner */
        SW_FT_Fixed   radius = stroker->radius;
        SW_FT_Vector  sigma;
        SW_FT_Angle   theta = 0, phi = 0;
        SW_FT_Bool    bevel, fixed_bevel;


        rotate = SW_FT_SIDE_TO_ROTATE(side);

        bevel =
            SW_FT_BOOL(stroker->line_join == SW_FT_STROKER_LINEJOIN_BEVEL);

        fixed_bevel =
            SW_FT_BOOL(stroker->line_join != SW_FT_STROKER_LINEJOIN_MITER_VARIABLE);

        /* check miter limit first */
        if (!bevel)
        {
            theta = SW_FT_Angle_Diff(stroker->angle_in, stroker->angle_out) / 2;

            if (theta == SW_FT_ANGLE_PI2)
                theta = -rotate;

            phi = stroker->angle_in + theta + rotate;

            SW_FT_Vector_From_Polar(&sigma, stroker->miter_limit, theta);

            /* is miter limit exceeded? */
            if (sigma.x < 0x10000L)
            {
                /* don't create variable bevels for very small deviations; */
                /* FT_Sin(x) = 0 for x <= 57                               */
                if (fixed_bevel || ft_pos_abs(theta) > 57)
                    bevel = TRUE;
            }
        }

        if (bevel)  /* this is a bevel (broken angle) */
        {
            if (fixed_bevel)
            {
                /* the outer corners are simply joined together */
                SW_FT_Vector  delta;


                /* add bevel */
                SW_FT_Vector_From_Polar(&delta,
                    radius,
                    stroker->angle_out + rotate);
                delta.x += stroker->center.x;
                delta.y += stroker->center.y;

                border->movable = FALSE;
                error = ft_stroke_border_lineto(border, &delta, FALSE);
            }
            else /* variable bevel or clipped miter */
            {
                /* the miter is truncated */
                SW_FT_Vector  middle, delta;
                SW_FT_Fixed   coef;


                /* compute middle point and first angle point */
                SW_FT_Vector_From_Polar(&middle,
                    SW_FT_MulFix(radius, stroker->miter_limit),
                    phi);

                coef = SW_FT_DivFix(0x10000L - sigma.x, sigma.y);
                delta.x = SW_FT_MulFix(middle.y, coef);
                delta.y = SW_FT_MulFix(-middle.x, coef);

                middle.x += stroker->center.x;
                middle.y += stroker->center.y;
                delta.x += middle.x;
                delta.y += middle.y;

                error = ft_stroke_border_lineto(border, &delta, FALSE);
                if (error)
                    goto Exit;

                /* compute second angle point */
                delta.x = middle.x - delta.x + middle.x;
                delta.y = middle.y - delta.y + middle.y;

                error = ft_stroke_border_lineto(border, &delta, FALSE);
                if (error)
                    goto Exit;

                /* finally, add an end point; only needed if not lineto */
                /* (line_length is zero for curves)                     */
                if (line_length == 0)
                {
                    SW_FT_Vector_From_Polar(&delta,
                        radius,
                        stroker->angle_out + rotate);

                    delta.x += stroker->center.x;
                    delta.y += stroker->center.y;

                    error = ft_stroke_border_lineto(border, &delta, FALSE);
                }
            }
        }
        else /* this is a miter (intersection) */
        {
            SW_FT_Fixed   length;
            SW_FT_Vector  delta;


            length = SW_FT_MulDiv(stroker->radius, stroker->miter_limit, sigma.x);

            SW_FT_Vector_From_Polar(&delta, length, phi);
            delta.x += stroker->center.x;
            delta.y += stroker->center.y;

            error = ft_stroke_border_lineto(border, &delta, FALSE);
            if (error)
                goto Exit;

            /* now add an end point; only needed if not lineto */
            /* (line_length is zero for curves)                */
            if (line_length == 0)
            {
                SW_FT_Vector_From_Polar(&delta,
                    stroker->radius,
                    stroker->angle_out + rotate);
                delta.x += stroker->center.x;
                delta.y += stroker->center.y;

                error = ft_stroke_border_lineto(border, &delta, FALSE);
            }
        }
    }

Exit:
    return error;
}

static SW_FT_Error ft_stroker_process_corner(SW_FT_Stroker stroker,
    SW_FT_Fixed   line_length)
{
    SW_FT_Error error = 0;
    SW_FT_Angle turn;
    SW_FT_Int   inside_side;

    turn = SW_FT_Angle_Diff(stroker->angle_in, stroker->angle_out);

    /* no specific corner processing is required if the turn is 0 */
    if (turn == 0) goto Exit;

    /* when we turn to the right, the inside side is 0 */
    inside_side = 0;

    /* otherwise, the inside side is 1 */
    if (turn < 0) inside_side = 1;

    /* process the inside side */
    error = ft_stroker_inside(stroker, inside_side, line_length);
    if (error) goto Exit;

    /* process the outside side */
    error = ft_stroker_outside(stroker, 1 - inside_side, line_length);

Exit:
    return error;
}

/* add two points to the left and right borders corresponding to the */
/* start of the subpath                                              */
static SW_FT_Error ft_stroker_subpath_start(SW_FT_Stroker stroker,
    SW_FT_Angle   start_angle,
    SW_FT_Fixed   line_length)
{
    SW_FT_Vector       delta;
    SW_FT_Vector       point;
    SW_FT_Error        error;
    SW_FT_StrokeBorder border;

    SW_FT_Vector_From_Polar(&delta, stroker->radius,
        start_angle + SW_FT_ANGLE_PI2);

    point.x = stroker->center.x + delta.x;
    point.y = stroker->center.y + delta.y;

    border = stroker->borders;
    error = ft_stroke_border_moveto(border, &point);
    if (error) goto Exit;

    point.x = stroker->center.x - delta.x;
    point.y = stroker->center.y - delta.y;

    border++;
    error = ft_stroke_border_moveto(border, &point);

    /* save angle, position, and line length for last join */
    /* (line_length is zero for curves)                    */
    stroker->subpath_angle = start_angle;
    stroker->first_point = FALSE;
    stroker->subpath_line_length = line_length;

Exit:
    return error;
}

/* documentation is in ftstroke.h */

SW_FT_Error SW_FT_Stroker_LineTo(SW_FT_Stroker stroker, SW_FT_Vector* to)
{
    SW_FT_Error        error = 0;
    SW_FT_StrokeBorder border;
    SW_FT_Vector       delta;
    SW_FT_Angle        angle;
    SW_FT_Int          side;
    SW_FT_Fixed        line_length;

    delta.x = to->x - stroker->center.x;
    delta.y = to->y - stroker->center.y;

    /* a zero-length lineto is a no-op; avoid creating a spurious corner */
    if (delta.x == 0 && delta.y == 0) goto Exit;

    /* compute length of line */
    line_length = SW_FT_Vector_Length(&delta);

    angle = SW_FT_Atan2(delta.x, delta.y);
    SW_FT_Vector_From_Polar(&delta, stroker->radius, angle + SW_FT_ANGLE_PI2);

    /* process corner if necessary */
    if (stroker->first_point) {
        /* This is the first segment of a subpath.  We need to     */
        /* add a point to each border at their respective starting */
        /* point locations.                                        */
        error = ft_stroker_subpath_start(stroker, angle, line_length);
        if (error) goto Exit;
    }
    else {
        /* process the current corner */
        stroker->angle_out = angle;
        error = ft_stroker_process_corner(stroker, line_length);
        if (error) goto Exit;
    }

    /* now add a line segment to both the `inside' and `outside' paths */
    for (border = stroker->borders, side = 1; side >= 0; side--, border++) {
        SW_FT_Vector point;

        point.x = to->x + delta.x;
        point.y = to->y + delta.y;

        /* the ends of lineto borders are movable */
        error = ft_stroke_border_lineto(border, &point, TRUE);
        if (error) goto Exit;

        delta.x = -delta.x;
        delta.y = -delta.y;
    }

    stroker->angle_in = angle;
    stroker->center = *to;
    stroker->line_length = line_length;

Exit:
    return error;
}

/* documentation is in ftstroke.h */

SW_FT_Error SW_FT_Stroker_ConicTo(SW_FT_Stroker stroker, SW_FT_Vector* control,
    SW_FT_Vector* to)
{
    SW_FT_Error   error = 0;
    SW_FT_Vector  bez_stack[34];
    SW_FT_Vector* arc;
    SW_FT_Vector* limit = bez_stack + 30;
    SW_FT_Bool    first_arc = TRUE;

    /* if all control points are coincident, this is a no-op; */
    /* avoid creating a spurious corner                       */
    if (SW_FT_IS_SMALL(stroker->center.x - control->x) &&
        SW_FT_IS_SMALL(stroker->center.y - control->y) &&
        SW_FT_IS_SMALL(control->x - to->x) &&
        SW_FT_IS_SMALL(control->y - to->y)) {
        stroker->center = *to;
        goto Exit;
    }

    arc = bez_stack;
    arc[0] = *to;
    arc[1] = *control;
    arc[2] = stroker->center;

    while (arc >= bez_stack) {
        SW_FT_Angle angle_in, angle_out;

        /* initialize with current direction */
        angle_in = angle_out = stroker->angle_in;

        if (arc < limit &&
            !ft_conic_is_small_enough(arc, &angle_in, &angle_out)) {
            if (stroker->first_point) stroker->angle_in = angle_in;

            ft_conic_split(arc);
            arc += 2;
            continue;
        }

        if (first_arc) {
            first_arc = FALSE;

            /* process corner if necessary */
            if (stroker->first_point)
                error = ft_stroker_subpath_start(stroker, angle_in, 0);
            else {
                stroker->angle_out = angle_in;
                error = ft_stroker_process_corner(stroker, 0);
            }
        }
        else if (ft_pos_abs(SW_FT_Angle_Diff(stroker->angle_in, angle_in)) >
            SW_FT_SMALL_CONIC_THRESHOLD / 4) {
            /* if the deviation from one arc to the next is too great, */
            /* add a round corner                                      */
            stroker->center = arc[2];
            stroker->angle_out = angle_in;
            stroker->line_join = SW_FT_STROKER_LINEJOIN_ROUND;

            error = ft_stroker_process_corner(stroker, 0);

            /* reinstate line join style */
            stroker->line_join = stroker->line_join_saved;
        }

        if (error) goto Exit;

        /* the arc's angle is small enough; we can add it directly to each */
        /* border                                                          */
        {
            SW_FT_Vector       ctrl, end;
            SW_FT_Angle        theta, phi, rotate, alpha0 = 0;
            SW_FT_Fixed        length;
            SW_FT_StrokeBorder border;
            SW_FT_Int          side;

            theta = SW_FT_Angle_Diff(angle_in, angle_out) / 2;
            phi = angle_in + theta;
            length = SW_FT_DivFix(stroker->radius, SW_FT_Cos(theta));

            /* compute direction of original arc */
            if (stroker->handle_wide_strokes)
                alpha0 = SW_FT_Atan2(arc[0].x - arc[2].x, arc[0].y - arc[2].y);

            for (border = stroker->borders, side = 0; side <= 1;
                side++, border++) {
                rotate = SW_FT_SIDE_TO_ROTATE(side);

                /* compute control point */
                SW_FT_Vector_From_Polar(&ctrl, length, phi + rotate);
                ctrl.x += arc[1].x;
                ctrl.y += arc[1].y;

                /* compute end point */
                SW_FT_Vector_From_Polar(&end, stroker->radius,
                    angle_out + rotate);
                end.x += arc[0].x;
                end.y += arc[0].y;

                if (stroker->handle_wide_strokes) {
                    SW_FT_Vector start;
                    SW_FT_Angle  alpha1;

                    /* determine whether the border radius is greater than the
                     */
                     /* radius of curvature of the original arc */
                    start = border->points[border->num_points - 1];

                    alpha1 = SW_FT_Atan2(end.x - start.x, end.y - start.y);

                    /* is the direction of the border arc opposite to */
                    /* that of the original arc? */
                    if (ft_pos_abs(SW_FT_Angle_Diff(alpha0, alpha1)) >
                        SW_FT_ANGLE_PI / 2) {
                        SW_FT_Angle  beta, gamma;
                        SW_FT_Vector bvec, delta;
                        SW_FT_Fixed  blen, sinA, sinB, alen;

                        /* use the sine rule to find the intersection point */
                        beta =
                            SW_FT_Atan2(arc[2].x - start.x, arc[2].y - start.y);
                        gamma = SW_FT_Atan2(arc[0].x - end.x, arc[0].y - end.y);

                        bvec.x = end.x - start.x;
                        bvec.y = end.y - start.y;

                        blen = SW_FT_Vector_Length(&bvec);

                        sinA = ft_pos_abs(SW_FT_Sin(alpha1 - gamma));
                        sinB = ft_pos_abs(SW_FT_Sin(beta - gamma));

                        alen = SW_FT_MulDiv(blen, sinA, sinB);

                        SW_FT_Vector_From_Polar(&delta, alen, beta);
                        delta.x += start.x;
                        delta.y += start.y;

                        /* circumnavigate the negative sector backwards */
                        border->movable = FALSE;
                        error = ft_stroke_border_lineto(border, &delta, FALSE);
                        if (error) goto Exit;
                        error = ft_stroke_border_lineto(border, &end, FALSE);
                        if (error) goto Exit;
                        error = ft_stroke_border_conicto(border, &ctrl, &start);
                        if (error) goto Exit;
                        /* and then move to the endpoint */
                        error = ft_stroke_border_lineto(border, &end, FALSE);
                        if (error) goto Exit;

                        continue;
                    }

                    /* else fall through */
                }

                /* simply add an arc */
                error = ft_stroke_border_conicto(border, &ctrl, &end);
                if (error) goto Exit;
            }
        }

        arc -= 2;

        stroker->angle_in = angle_out;
    }

    stroker->center = *to;

Exit:
    return error;
}

/* documentation is in ftstroke.h */

SW_FT_Error SW_FT_Stroker_CubicTo(SW_FT_Stroker stroker, SW_FT_Vector* control1,
    SW_FT_Vector* control2, SW_FT_Vector* to)
{
    SW_FT_Error   error = 0;
    SW_FT_Vector  bez_stack[37];
    SW_FT_Vector* arc;
    SW_FT_Vector* limit = bez_stack + 32;
    SW_FT_Bool    first_arc = TRUE;

    /* if all control points are coincident, this is a no-op; */
    /* avoid creating a spurious corner */
    if (SW_FT_IS_SMALL(stroker->center.x - control1->x) &&
        SW_FT_IS_SMALL(stroker->center.y - control1->y) &&
        SW_FT_IS_SMALL(control1->x - control2->x) &&
        SW_FT_IS_SMALL(control1->y - control2->y) &&
        SW_FT_IS_SMALL(control2->x - to->x) &&
        SW_FT_IS_SMALL(control2->y - to->y)) {
        stroker->center = *to;
        goto Exit;
    }

    arc = bez_stack;
    arc[0] = *to;
    arc[1] = *control2;
    arc[2] = *control1;
    arc[3] = stroker->center;

    while (arc >= bez_stack) {
        SW_FT_Angle angle_in, angle_mid, angle_out;

        /* initialize with current direction */
        angle_in = angle_out = angle_mid = stroker->angle_in;

        if (arc < limit &&
            !ft_cubic_is_small_enough(arc, &angle_in, &angle_mid, &angle_out)) {
            if (stroker->first_point) stroker->angle_in = angle_in;

            ft_cubic_split(arc);
            arc += 3;
            continue;
        }

        if (first_arc) {
            first_arc = FALSE;

            /* process corner if necessary */
            if (stroker->first_point)
                error = ft_stroker_subpath_start(stroker, angle_in, 0);
            else {
                stroker->angle_out = angle_in;
                error = ft_stroker_process_corner(stroker, 0);
            }
        }
        else if (ft_pos_abs(SW_FT_Angle_Diff(stroker->angle_in, angle_in)) >
            SW_FT_SMALL_CUBIC_THRESHOLD / 4) {
            /* if the deviation from one arc to the next is too great, */
            /* add a round corner                                      */
            stroker->center = arc[3];
            stroker->angle_out = angle_in;
            stroker->line_join = SW_FT_STROKER_LINEJOIN_ROUND;

            error = ft_stroker_process_corner(stroker, 0);

            /* reinstate line join style */
            stroker->line_join = stroker->line_join_saved;
        }

        if (error) goto Exit;

        /* the arc's angle is small enough; we can add it directly to each */
        /* border                                                          */
        {
            SW_FT_Vector       ctrl1, ctrl2, end;
            SW_FT_Angle        theta1, phi1, theta2, phi2, rotate, alpha0 = 0;
            SW_FT_Fixed        length1, length2;
            SW_FT_StrokeBorder border;
            SW_FT_Int          side;

            theta1 = SW_FT_Angle_Diff(angle_in, angle_mid) / 2;
            theta2 = SW_FT_Angle_Diff(angle_mid, angle_out) / 2;
            phi1 = ft_angle_mean(angle_in, angle_mid);
            phi2 = ft_angle_mean(angle_mid, angle_out);
            length1 = SW_FT_DivFix(stroker->radius, SW_FT_Cos(theta1));
            length2 = SW_FT_DivFix(stroker->radius, SW_FT_Cos(theta2));

            /* compute direction of original arc */
            if (stroker->handle_wide_strokes)
                alpha0 = SW_FT_Atan2(arc[0].x - arc[3].x, arc[0].y - arc[3].y);

            for (border = stroker->borders, side = 0; side <= 1;
                side++, border++) {
                rotate = SW_FT_SIDE_TO_ROTATE(side);

                /* compute control points */
                SW_FT_Vector_From_Polar(&ctrl1, length1, phi1 + rotate);
                ctrl1.x += arc[2].x;
                ctrl1.y += arc[2].y;

                SW_FT_Vector_From_Polar(&ctrl2, length2, phi2 + rotate);
                ctrl2.x += arc[1].x;
                ctrl2.y += arc[1].y;

                /* compute end point */
                SW_FT_Vector_From_Polar(&end, stroker->radius,
                    angle_out + rotate);
                end.x += arc[0].x;
                end.y += arc[0].y;

                if (stroker->handle_wide_strokes) {
                    SW_FT_Vector start;
                    SW_FT_Angle  alpha1;

                    /* determine whether the border radius is greater than the
                     */
                     /* radius of curvature of the original arc */
                    start = border->points[border->num_points - 1];

                    alpha1 = SW_FT_Atan2(end.x - start.x, end.y - start.y);

                    /* is the direction of the border arc opposite to */
                    /* that of the original arc? */
                    if (ft_pos_abs(SW_FT_Angle_Diff(alpha0, alpha1)) >
                        SW_FT_ANGLE_PI / 2) {
                        SW_FT_Angle  beta, gamma;
                        SW_FT_Vector bvec, delta;
                        SW_FT_Fixed  blen, sinA, sinB, alen;

                        /* use the sine rule to find the intersection point */
                        beta =
                            SW_FT_Atan2(arc[3].x - start.x, arc[3].y - start.y);
                        gamma = SW_FT_Atan2(arc[0].x - end.x, arc[0].y - end.y);

                        bvec.x = end.x - start.x;
                        bvec.y = end.y - start.y;

                        blen = SW_FT_Vector_Length(&bvec);

                        sinA = ft_pos_abs(SW_FT_Sin(alpha1 - gamma));
                        sinB = ft_pos_abs(SW_FT_Sin(beta - gamma));

                        alen = SW_FT_MulDiv(blen, sinA, sinB);

                        SW_FT_Vector_From_Polar(&delta, alen, beta);
                        delta.x += start.x;
                        delta.y += start.y;

                        /* circumnavigate the negative sector backwards */
                        border->movable = FALSE;
                        error = ft_stroke_border_lineto(border, &delta, FALSE);
                        if (error) goto Exit;
                        error = ft_stroke_border_lineto(border, &end, FALSE);
                        if (error) goto Exit;
                        error = ft_stroke_border_cubicto(border, &ctrl2, &ctrl1,
                            &start);
                        if (error) goto Exit;
                        /* and then move to the endpoint */
                        error = ft_stroke_border_lineto(border, &end, FALSE);
                        if (error) goto Exit;

                        continue;
                    }

                    /* else fall through */
                }

                /* simply add an arc */
                error = ft_stroke_border_cubicto(border, &ctrl1, &ctrl2, &end);
                if (error) goto Exit;
            }
        }

        arc -= 3;

        stroker->angle_in = angle_out;
    }

    stroker->center = *to;

Exit:
    return error;
}

/* documentation is in ftstroke.h */

SW_FT_Error SW_FT_Stroker_BeginSubPath(SW_FT_Stroker stroker, SW_FT_Vector* to,
    SW_FT_Bool open)
{
    /* We cannot process the first point, because there is not enough      */
    /* information regarding its corner/cap.  The latter will be processed */
    /* in the `SW_FT_Stroker_EndSubPath' routine.                             */
    /*                                                                     */
    stroker->first_point = TRUE;
    stroker->center = *to;
    stroker->subpath_open = open;

    /* Determine if we need to check whether the border radius is greater */
    /* than the radius of curvature of a curve, to handle this case       */
    /* specially.  This is only required if bevel joins or butt caps may  */
    /* be created, because round & miter joins and round & square caps    */
    /* cover the negative sector created with wide strokes.               */
    stroker->handle_wide_strokes =
        SW_FT_BOOL(stroker->line_join != SW_FT_STROKER_LINEJOIN_ROUND ||
            (stroker->subpath_open &&
                stroker->line_cap == SW_FT_STROKER_LINECAP_BUTT));

    /* record the subpath start point for each border */
    stroker->subpath_start = *to;

    stroker->angle_in = 0;

    return 0;
}

static SW_FT_Error ft_stroker_add_reverse_left(SW_FT_Stroker stroker,
    SW_FT_Bool    open)
{
    SW_FT_StrokeBorder right = stroker->borders + 0;
    SW_FT_StrokeBorder left = stroker->borders + 1;
    SW_FT_Int          new_points;
    SW_FT_Error        error = 0;

    assert(left->start >= 0);

    new_points = left->num_points - left->start;
    if (new_points > 0) {
        error = ft_stroke_border_grow(right, (SW_FT_UInt)new_points);
        if (error) goto Exit;

        {
            SW_FT_Vector* dst_point = right->points + right->num_points;
            SW_FT_Byte* dst_tag = right->tags + right->num_points;
            SW_FT_Vector* src_point = left->points + left->num_points - 1;
            SW_FT_Byte* src_tag = left->tags + left->num_points - 1;

            while (src_point >= left->points + left->start) {
                *dst_point = *src_point;
                *dst_tag = *src_tag;

                if (open)
                    dst_tag[0] &= ~SW_FT_STROKE_TAG_BEGIN_END;
                else {
                    SW_FT_Byte ttag =
                        (SW_FT_Byte)(dst_tag[0] & SW_FT_STROKE_TAG_BEGIN_END);

                    /* switch begin/end tags if necessary */
                    if (ttag == SW_FT_STROKE_TAG_BEGIN ||
                        ttag == SW_FT_STROKE_TAG_END)
                        dst_tag[0] ^= SW_FT_STROKE_TAG_BEGIN_END;
                }

                src_point--;
                src_tag--;
                dst_point++;
                dst_tag++;
            }
        }

        left->num_points = left->start;
        right->num_points += new_points;

        right->movable = FALSE;
        left->movable = FALSE;
    }

Exit:
    return error;
}

/* documentation is in ftstroke.h */

/* there's a lot of magic in this function! */
SW_FT_Error SW_FT_Stroker_EndSubPath(SW_FT_Stroker stroker)
{
    SW_FT_Error error = 0;

    if (stroker->subpath_open) {
        SW_FT_StrokeBorder right = stroker->borders;

        /* All right, this is an opened path, we need to add a cap between */
        /* right & left, add the reverse of left, then add a final cap     */
        /* between left & right.                                           */
        error = ft_stroker_cap(stroker, stroker->angle_in, 0);
        if (error) goto Exit;

        /* add reversed points from `left' to `right' */
        error = ft_stroker_add_reverse_left(stroker, TRUE);
        if (error) goto Exit;

        /* now add the final cap */
        stroker->center = stroker->subpath_start;
        error =
            ft_stroker_cap(stroker, stroker->subpath_angle + SW_FT_ANGLE_PI, 0);
        if (error) goto Exit;

        /* Now end the right subpath accordingly.  The left one is */
        /* rewind and doesn't need further processing.             */
        ft_stroke_border_close(right, FALSE);
    }
    else {
        SW_FT_Angle turn;
        SW_FT_Int   inside_side;

        /* close the path if needed */
        if (stroker->center.x != stroker->subpath_start.x ||
            stroker->center.y != stroker->subpath_start.y) {
            error = SW_FT_Stroker_LineTo(stroker, &stroker->subpath_start);
            if (error) goto Exit;
        }

        /* process the corner */
        stroker->angle_out = stroker->subpath_angle;
        turn = SW_FT_Angle_Diff(stroker->angle_in, stroker->angle_out);

        /* no specific corner processing is required if the turn is 0 */
        if (turn != 0) {
            /* when we turn to the right, the inside side is 0 */
            inside_side = 0;

            /* otherwise, the inside side is 1 */
            if (turn < 0) inside_side = 1;

            error = ft_stroker_inside(stroker, inside_side,
                stroker->subpath_line_length);
            if (error) goto Exit;

            /* process the outside side */
            error = ft_stroker_outside(stroker, 1 - inside_side,
                stroker->subpath_line_length);
            if (error) goto Exit;
        }

        /* then end our two subpaths */
        ft_stroke_border_close(stroker->borders + 0, FALSE);
        ft_stroke_border_close(stroker->borders + 1, TRUE);
    }

Exit:
    return error;
}

/* documentation is in ftstroke.h */

SW_FT_Error SW_FT_Stroker_GetBorderCounts(SW_FT_Stroker       stroker,
    SW_FT_StrokerBorder border,
    SW_FT_UInt* anum_points,
    SW_FT_UInt* anum_contours)
{
    SW_FT_UInt  num_points = 0, num_contours = 0;
    SW_FT_Error error;

    if (!stroker || border > 1) {
        error = -1;  // SW_FT_THROW( Invalid_Argument );
        goto Exit;
    }

    error = ft_stroke_border_get_counts(stroker->borders + border, &num_points,
        &num_contours);
Exit:
    if (anum_points) *anum_points = num_points;

    if (anum_contours) *anum_contours = num_contours;

    return error;
}

/* documentation is in ftstroke.h */

SW_FT_Error SW_FT_Stroker_GetCounts(SW_FT_Stroker stroker,
    SW_FT_UInt* anum_points,
    SW_FT_UInt* anum_contours)
{
    SW_FT_UInt  count1, count2, num_points = 0;
    SW_FT_UInt  count3, count4, num_contours = 0;
    SW_FT_Error error;

    error = ft_stroke_border_get_counts(stroker->borders + 0, &count1, &count2);
    if (error) goto Exit;

    error = ft_stroke_border_get_counts(stroker->borders + 1, &count3, &count4);
    if (error) goto Exit;

    num_points = count1 + count3;
    num_contours = count2 + count4;

Exit:
    *anum_points = num_points;
    *anum_contours = num_contours;
    return error;
}

/* documentation is in ftstroke.h */

void SW_FT_Stroker_ExportBorder(SW_FT_Stroker       stroker,
    SW_FT_StrokerBorder border,
    SW_FT_Outline* outline)
{
    if (border == SW_FT_STROKER_BORDER_LEFT ||
        border == SW_FT_STROKER_BORDER_RIGHT) {
        SW_FT_StrokeBorder sborder = &stroker->borders[border];

        if (sborder->valid) ft_stroke_border_export(sborder, outline);
    }
}

/* documentation is in ftstroke.h */

void SW_FT_Stroker_Export(SW_FT_Stroker stroker, SW_FT_Outline* outline)
{
    SW_FT_Stroker_ExportBorder(stroker, SW_FT_STROKER_BORDER_LEFT, outline);
    SW_FT_Stroker_ExportBorder(stroker, SW_FT_STROKER_BORDER_RIGHT, outline);
}

/* documentation is in ftstroke.h */

/*
 *  The following is very similar to SW_FT_Outline_Decompose, except
 *  that we do support opened paths, and do not scale the outline.
 */
SW_FT_Error SW_FT_Stroker_ParseOutline(SW_FT_Stroker        stroker,
    const SW_FT_Outline* outline)
{
    SW_FT_Vector v_last;
    SW_FT_Vector v_control;
    SW_FT_Vector v_start;

    SW_FT_Vector* point;
    SW_FT_Vector* limit;
    char* tags;

    SW_FT_Error error;

    SW_FT_Int  n;     /* index of contour in outline     */
    SW_FT_UInt first; /* index of first point in contour */
    SW_FT_Int  tag;   /* current point's state           */

    if (!outline || !stroker) return -1;  // SW_FT_THROW( Invalid_Argument );

    SW_FT_Stroker_Rewind(stroker);

    first = 0;

    for (n = 0; n < outline->n_contours; n++) {
        SW_FT_UInt last; /* index of last point in contour */

        last = outline->contours[n];
        limit = outline->points + last;

        /* skip empty points; we don't stroke these */
        if (last <= first) {
            first = last + 1;
            continue;
        }

        v_start = outline->points[first];
        v_last = outline->points[last];

        v_control = v_start;

        point = outline->points + first;
        tags = outline->tags + first;
        tag = SW_FT_CURVE_TAG(tags[0]);

        /* A contour cannot start with a cubic control point! */
        if (tag == SW_FT_CURVE_TAG_CUBIC) goto Invalid_Outline;

        /* check first point to determine origin */
        if (tag == SW_FT_CURVE_TAG_CONIC) {
            /* First point is conic control.  Yes, this happens. */
            if (SW_FT_CURVE_TAG(outline->tags[last]) == SW_FT_CURVE_TAG_ON) {
                /* start at last point if it is on the curve */
                v_start = v_last;
                limit--;
            }
            else {
                /* if both first and last points are conic, */
                /* start at their middle                    */
                v_start.x = (v_start.x + v_last.x) / 2;
                v_start.y = (v_start.y + v_last.y) / 2;
            }
            point--;
            tags--;
        }

        error = SW_FT_Stroker_BeginSubPath(stroker, &v_start, outline->contours_flag[n]);
        if (error) goto Exit;

        while (point < limit) {
            point++;
            tags++;

            tag = SW_FT_CURVE_TAG(tags[0]);
            switch (tag) {
            case SW_FT_CURVE_TAG_ON: /* emit a single line_to */
            {
                SW_FT_Vector vec;

                vec.x = point->x;
                vec.y = point->y;

                error = SW_FT_Stroker_LineTo(stroker, &vec);
                if (error) goto Exit;
                continue;
            }

            case SW_FT_CURVE_TAG_CONIC: /* consume conic arcs */
                v_control.x = point->x;
                v_control.y = point->y;

            Do_Conic:
                if (point < limit) {
                    SW_FT_Vector vec;
                    SW_FT_Vector v_middle;

                    point++;
                    tags++;
                    tag = SW_FT_CURVE_TAG(tags[0]);

                    vec = point[0];

                    if (tag == SW_FT_CURVE_TAG_ON) {
                        error =
                            SW_FT_Stroker_ConicTo(stroker, &v_control, &vec);
                        if (error) goto Exit;
                        continue;
                    }

                    if (tag != SW_FT_CURVE_TAG_CONIC) goto Invalid_Outline;

                    v_middle.x = (v_control.x + vec.x) / 2;
                    v_middle.y = (v_control.y + vec.y) / 2;

                    error =
                        SW_FT_Stroker_ConicTo(stroker, &v_control, &v_middle);
                    if (error) goto Exit;

                    v_control = vec;
                    goto Do_Conic;
                }

                error = SW_FT_Stroker_ConicTo(stroker, &v_control, &v_start);
                goto Close;

            default: /* SW_FT_CURVE_TAG_CUBIC */
            {
                SW_FT_Vector vec1, vec2;

                if (point + 1 > limit ||
                    SW_FT_CURVE_TAG(tags[1]) != SW_FT_CURVE_TAG_CUBIC)
                    goto Invalid_Outline;

                point += 2;
                tags += 2;

                vec1 = point[-2];
                vec2 = point[-1];

                if (point <= limit) {
                    SW_FT_Vector vec;

                    vec = point[0];

                    error = SW_FT_Stroker_CubicTo(stroker, &vec1, &vec2, &vec);
                    if (error) goto Exit;
                    continue;
                }

                error = SW_FT_Stroker_CubicTo(stroker, &vec1, &vec2, &v_start);
                goto Close;
            }
            }
        }

    Close:
        if (error) goto Exit;

        if (stroker->first_point) {
            stroker->subpath_open = TRUE;
            error = ft_stroker_subpath_start(stroker, 0, 0);
            if (error) goto Exit;
        }

        error = SW_FT_Stroker_EndSubPath(stroker);
        if (error) goto Exit;

        first = last + 1;
    }

    return 0;

Exit:
    return error;

Invalid_Outline:
    return -2;  // SW_FT_THROW( Invalid_Outline );
}

/* END */

