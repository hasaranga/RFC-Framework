
/*
 * Copyright (c) 2020 Nwutobo Samuel Ugochukwu <sammycageagle@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

// Modified to work with windows GDI/GDI-Plus/Direct-2D. (by R.Hasaranga)

#pragma once

#include <cstring>
#include <cmath>
#include <limits>
#include <string>
#include <algorithm>
#include <memory>
#include <vector>
#include <array>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <list>
#include <map>
#include <set>

#ifndef PARSERUTILS_H
#define PARSERUTILS_H

namespace lunasvg {

#define IS_ALPHA(c) ((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z')
#define IS_NUM(c) ((c) >= '0' && (c) <= '9')
#define IS_WS(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')

    namespace Utils {

        inline const char* rtrim(const char* start, const char* end)
        {
            while (end > start && IS_WS(end[-1]))
                --end;

            return end;
        }

        inline const char* ltrim(const char* start, const char* end)
        {
            while (start < end && IS_WS(*start))
                ++start;

            return start;
        }

        inline bool skipDesc(const char*& ptr, const char* end, const char ch)
        {
            if (ptr >= end || *ptr != ch)
                return false;

            ++ptr;
            return true;
        }

        inline bool skipDesc(const char*& ptr, const char* end, const char* data)
        {
            int read = 0;
            while (data[read])
            {
                if (ptr >= end || *ptr != data[read])
                {
                    ptr -= read;
                    return false;
                }

                ++read;
                ++ptr;
            }

            return true;
        }

        inline bool skipUntil(const char*& ptr, const char* end, const char ch)
        {
            while (ptr < end && *ptr != ch)
                ++ptr;

            return ptr < end;
        }

        inline bool skipUntil(const char*& ptr, const char* end, const char* data)
        {
            while (ptr < end)
            {
                auto start = ptr;
                if (skipDesc(start, end, data))
                    break;
                ++ptr;
            }

            return ptr < end;
        }

        inline bool readUntil(const char*& ptr, const char* end, const char ch, std::string& value)
        {
            auto start = ptr;
            if (!skipUntil(ptr, end, ch))
                return false;

            value.assign(start, ptr);
            return true;
        }

        inline bool readUntil(const char*& ptr, const char* end, const char* data, std::string& value)
        {
            auto start = ptr;
            if (!skipUntil(ptr, end, data))
                return false;

            value.assign(start, ptr);
            return true;
        }

        inline bool skipWs(const char*& ptr, const char* end)
        {
            while (ptr < end && IS_WS(*ptr))
                ++ptr;

            return ptr < end;
        }

        inline bool skipWsDelimiter(const char*& ptr, const char* end, const char delimiter)
        {
            if (ptr < end && !IS_WS(*ptr) && *ptr != delimiter)
                return false;

            if (skipWs(ptr, end))
            {
                if (ptr < end && *ptr == delimiter)
                {
                    ++ptr;
                    skipWs(ptr, end);
                }
            }

            return ptr < end;
        }

        inline bool skipWsComma(const char*& ptr, const char* end)
        {
            return skipWsDelimiter(ptr, end, ',');
        }

        inline bool isIntegralDigit(char ch, int base)
        {
            if (IS_NUM(ch))
                return ch - '0' < base;

            if (IS_ALPHA(ch))
                return (ch >= 'a' && ch < 'a' + (std::min)(base, 36) - 10) || (ch >= 'A' && ch < 'A' + (std::min)(base, 36) - 10);

            return false;
        }

        template<typename T>
        inline bool parseInteger(const char*& ptr, const char* end, T& integer, int base = 10)
        {
            bool isNegative = 0;
            T value = 0;

            static const T intMax = (std::numeric_limits<T>::max)();
            static const bool isSigned = std::numeric_limits<T>::is_signed;
            using signed_t = typename std::make_signed<T>::type;
            const T maxMultiplier = intMax / static_cast<T>(base);

            if (ptr < end && *ptr == '+')
                ++ptr;
            else if (ptr < end && isSigned && *ptr == '-')
            {
                ++ptr;
                isNegative = true;
            }

            if (ptr >= end || !isIntegralDigit(*ptr, base))
                return false;

            do {
                const char ch = *ptr++;
                int digitValue;
                if (IS_NUM(ch))
                    digitValue = ch - '0';
                else if (ch >= 'a')
                    digitValue = ch - 'a' + 10;
                else
                    digitValue = ch - 'A' + 10;

                if (value > maxMultiplier || (value == maxMultiplier && static_cast<T>(digitValue) > (intMax % static_cast<T>(base)) + isNegative))
                    return false;
                value = static_cast<T>(base) * value + static_cast<T>(digitValue);
            } while (ptr < end && isIntegralDigit(*ptr, base));

            if (isNegative)
                integer = -static_cast<signed_t>(value);
            else
                integer = value;

            return true;
        }

        template<typename T>
        inline bool parseNumber(const char*& ptr, const char* end, T& number)
        {
            T integer, fraction;
            int sign, expsign, exponent;

            static const T numberMax = (std::numeric_limits<T>::max)();
            fraction = 0;
            integer = 0;
            exponent = 0;
            sign = 1;
            expsign = 1;

            if (ptr < end && *ptr == '+')
                ++ptr;
            else if (ptr < end && *ptr == '-')
            {
                ++ptr;
                sign = -1;
            }

            if (ptr >= end || !(IS_NUM(*ptr) || *ptr == '.'))
                return false;

            if (*ptr != '.')
            {
                do {
                    integer = static_cast<T>(10) * integer + (*ptr - '0');
                    ++ptr;
                } while (ptr < end && IS_NUM(*ptr));
            }

            if (ptr < end && *ptr == '.')
            {
                ++ptr;
                if (ptr >= end || !IS_NUM(*ptr))
                    return false;

                T divisor = 1;
                do {
                    fraction = static_cast<T>(10) * fraction + (*ptr - '0');
                    divisor *= static_cast<T>(10);
                    ++ptr;
                } while (ptr < end && IS_NUM(*ptr));
                fraction /= divisor;
            }

            if (ptr < end && (*ptr == 'e' || *ptr == 'E')
                && (ptr[1] != 'x' && ptr[1] != 'm'))
            {
                ++ptr;
                if (ptr < end && *ptr == '+')
                    ++ptr;
                else if (ptr < end && *ptr == '-')
                {
                    ++ptr;
                    expsign = -1;
                }

                if (ptr >= end || !IS_NUM(*ptr))
                    return false;

                do {
                    exponent = 10 * exponent + (*ptr - '0');
                    ++ptr;
                } while (ptr < end && IS_NUM(*ptr));
            }

            number = sign * (integer + fraction);
            if (exponent)
                number *= static_cast<T>(pow(10.0, expsign * exponent));

            return number >= -numberMax && number <= numberMax;
        }

    } // namespace Utils

} // namespace lunasvg

#endif // PARSERUTILS_H

#ifndef SW_FT_TYPES_H
#define SW_FT_TYPES_H

/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    SW_FT_Fixed                                                           */
/*                                                                       */
/* <Description>                                                         */
/*    This type is used to store 16.16 fixed-point values, like scaling  */
/*    values or matrix coefficients.                                     */
/*                                                                       */
typedef signed long  SW_FT_Fixed;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    SW_FT_Int                                                             */
/*                                                                       */
/* <Description>                                                         */
/*    A typedef for the int type.                                        */
/*                                                                       */
typedef signed int  SW_FT_Int;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    SW_FT_UInt                                                            */
/*                                                                       */
/* <Description>                                                         */
/*    A typedef for the unsigned int type.                               */
/*                                                                       */
typedef unsigned int  SW_FT_UInt;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    SW_FT_Long                                                            */
/*                                                                       */
/* <Description>                                                         */
/*    A typedef for signed long.                                         */
/*                                                                       */
typedef signed long  SW_FT_Long;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    SW_FT_ULong                                                           */
/*                                                                       */
/* <Description>                                                         */
/*    A typedef for unsigned long.                                       */
/*                                                                       */
typedef unsigned long SW_FT_ULong;

/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    SW_FT_Short                                                           */
/*                                                                       */
/* <Description>                                                         */
/*    A typedef for signed short.                                        */
/*                                                                       */
typedef signed short  SW_FT_Short;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    SW_FT_Byte                                                            */
/*                                                                       */
/* <Description>                                                         */
/*    A simple typedef for the _unsigned_ char type.                     */
/*                                                                       */
typedef unsigned char  SW_FT_Byte;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    SW_FT_Bool                                                            */
/*                                                                       */
/* <Description>                                                         */
/*    A typedef of unsigned char, used for simple booleans.  As usual,   */
/*    values 1 and~0 represent true and false, respectively.             */
/*                                                                       */
typedef unsigned char  SW_FT_Bool;



/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    SW_FT_Error                                                           */
/*                                                                       */
/* <Description>                                                         */
/*    The FreeType error code type.  A value of~0 is always interpreted  */
/*    as a successful operation.                                         */
/*                                                                       */
typedef int  SW_FT_Error;


/*************************************************************************/
/*                                                                       */
/* <Type>                                                                */
/*    SW_FT_Pos                                                             */
/*                                                                       */
/* <Description>                                                         */
/*    The type SW_FT_Pos is used to store vectorial coordinates.  Depending */
/*    on the context, these can represent distances in integer font      */
/*    units, or 16.16, or 26.6 fixed-point pixel coordinates.            */
/*                                                                       */
typedef signed long  SW_FT_Pos;


/*************************************************************************/
/*                                                                       */
/* <Struct>                                                              */
/*    SW_FT_Vector                                                          */
/*                                                                       */
/* <Description>                                                         */
/*    A simple structure used to store a 2D vector; coordinates are of   */
/*    the SW_FT_Pos type.                                                   */
/*                                                                       */
/* <Fields>                                                              */
/*    x :: The horizontal coordinate.                                    */
/*    y :: The vertical coordinate.                                      */
/*                                                                       */
typedef struct  SW_FT_Vector_
{
    SW_FT_Pos  x;
    SW_FT_Pos  y;

} SW_FT_Vector;


typedef long long int           SW_FT_Int64;
typedef unsigned long long int  SW_FT_UInt64;

typedef signed int              SW_FT_Int32;
typedef unsigned int            SW_FT_UInt32;


#define SW_FT_BOOL( x )  ( (SW_FT_Bool)( x ) )

#define SW_FT_SIZEOF_LONG 4

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE  0
#endif


#endif // SW_FT_TYPES_H

/*
 * Copyright (c) 2020 Nwutobo Samuel Ugochukwu <sammycageagle@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#ifndef LUNASVG_H
#define LUNASVG_H

#if defined(_MSC_VER) && defined(LUNASVG_SHARED)
#ifdef LUNASVG_EXPORT
#define LUNASVG_API __declspec(dllexport)
#else
#define LUNASVG_API __declspec(dllimport)
#endif
#else
#define LUNASVG_API
#endif

namespace lunasvg {

    class Box
    {
    public:
        Box() = default;
        Box(double x, double y, double w, double h)
            : x(x), y(y), w(w), h(h)
        {}

    public:
        double x{ 0 };
        double y{ 0 };
        double w{ 0 };
        double h{ 0 };
    };

    class Matrix
    {
    public:
        Matrix() = default;
        Matrix(double a, double b, double c, double d, double e, double f)
            : a(a), b(b), c(c), d(d), e(e), f(f)
        {}

    public:
        double a{ 1 };
        double b{ 0 };
        double c{ 0 };
        double d{ 1 };
        double e{ 0 };
        double f{ 0 };
    };

    class LUNASVG_API Bitmap
    {
    public:
        /**
         * @note Default bitmap format is RGBA (non-premultiplied).
         */
        Bitmap();
        Bitmap(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride);
        Bitmap(std::uint32_t width, std::uint32_t height);

        void reset(std::uint8_t* data, std::uint32_t width, std::uint32_t height, std::uint32_t stride);
        void reset(std::uint32_t width, std::uint32_t height);

        std::uint8_t* data() const;
        std::uint32_t width() const;
        std::uint32_t height() const;
        std::uint32_t stride() const;
        bool valid() const;

    private:
        struct Impl;
        std::shared_ptr<Impl> m_impl;
    };

    class LayoutSymbol;

    class LUNASVG_API Document
    {
    public:
        /**
         * @brief Creates a document from a file
         * @param filename - file to load
         * @return pointer to document on success, otherwise nullptr
         */
        static std::unique_ptr<Document> loadFromFile(const std::string& filename);

        /**
         * @brief Creates a document from a string
         * @param string - string to load
         * @return pointer to document on success, otherwise nullptr
         */
        static std::unique_ptr<Document> loadFromData(const std::string& string);

        /**
         * @brief Creates a document from a string data and size
         * @param data - string data to load
         * @param size - size of the data to load, in bytes
         * @return pointer to document on success, otherwise nullptr
         */
        static std::unique_ptr<Document> loadFromData(const char* data, std::size_t size);

        /**
         * @brief Creates a document from a null terminated string data
         * @param data - null terminated string data to load
         * @return pointer to document on success, otherwise nullptr
         */
        static std::unique_ptr<Document> loadFromData(const char* data);

        /**
         * @brief Pre-Rotates the document matrix clockwise around the current origin
         * @param angle - rotation angle, in degrees
         * @return this
         */
        Document* rotate(double angle);

        /**
         * @brief Pre-Rotates the document matrix clockwise around the given point
         * @param angle - rotation angle, in degrees
         * @param cx - horizontal translation
         * @param cy - vertical translation
         * @return this
         */
        Document* rotate(double angle, double cx, double cy);

        /**
         * @brief Pre-Scales the document matrix by sx horizontally and sy vertically
         * @param sx - horizontal scale factor
         * @param sy - vertical scale factor
         * @return this
         */
        Document* scale(double sx, double sy);

        /**
         * @brief Pre-Shears the document matrix by shx horizontally and shy vertically
         * @param shx - horizontal skew factor, in degree
         * @param shy - vertical skew factor, in degree
         * @return this
         */
        Document* shear(double shx, double shy);

        /**
         * @brief Pre-Translates the document matrix by tx horizontally and ty vertically
         * @param tx - horizontal translation
         * @param ty - vertical translation
         * @return this
         */
        Document* translate(double tx, double ty);

        /**
         * @brief Pre-Multiplies the document matrix by Matrix(a, b, c, d, e, f)
         * @param a - horizontal scale factor
         * @param b - horizontal skew factor
         * @param c - vertical skew factor
         * @param d - vertical scale factor
         * @param e - horizontal translation
         * @param f - vertical translation
         * @return this
         */
        Document* transform(double a, double b, double c, double d, double e, double f);

        /**
         * @brief Resets the document matrix to identity
         * @return this
         */
        Document* identity();

        /**
         * @brief Returns the current transformation matrix of the document
         * @return the current transformation matrix
         */
        Matrix matrix() const;

        /**
         * @brief Returns the smallest rectangle in which the document fits
         * @return the smallest rectangle in which the document fits
         */
        Box box() const;

        /**
         * @brief Returns width of the document
         * @return the width of the document in pixels
         */
        double width() const;

        /**
         * @brief Returns the height of the document
         * @return the height of the document in pixels
         */
        double height() const;

        /**
         * @brief Renders the document to a bitmap
         * @param matrix - the current transformation matrix
         * @param bitmap - target image on which the content will be drawn
         * @param backgroundColor - background color in 0xRRGGBBAA format
         */
        void render(Bitmap bitmap, const Matrix& matrix = Matrix{}, std::uint32_t backgroundColor = 0x00000000) const;

        /**
         * @brief Renders the document to a bitmap
         * @param width - maximum width, in pixels
         * @param height - maximum height, in pixels
         * @param backgroundColor - background color in 0xRRGGBBAA format
         * @return the raster representation of the document
         */
        Bitmap renderToBitmap(std::uint32_t width = 0, std::uint32_t height = 0, std::uint32_t backgroundColor = 0x00000000) const;

        ~Document();
    private:
        Document();

        std::unique_ptr<LayoutSymbol> root;
    };

} //namespace lunasvg

#endif // LUNASVG_H

#ifndef PROPERTY_H
#define PROPERTY_H

namespace lunasvg {

    enum class Display
    {
        Inline,
        None
    };

    enum class Visibility
    {
        Visible,
        Hidden
    };

    enum class Overflow
    {
        Visible,
        Hidden
    };

    enum class LineCap
    {
        Butt,
        Round,
        Square
    };

    enum class LineJoin
    {
        Miter,
        Round,
        Bevel
    };

    enum class WindRule
    {
        NonZero,
        EvenOdd
    };

    enum class Units
    {
        UserSpaceOnUse,
        ObjectBoundingBox
    };

    enum class SpreadMethod
    {
        Pad,
        Reflect,
        Repeat
    };

    enum class MarkerUnits
    {
        StrokeWidth,
        UserSpaceOnUse
    };

    class Color
    {
    public:
        Color() = default;
        Color(double r, double g, double b, double a = 1);

        bool isNone() const { return  a == 0.0; }

        static const Color Black;
        static const Color White;
        static const Color Red;
        static const Color Green;
        static const Color Blue;
        static const Color Yellow;
        static const Color Transparent;

    public:
        double r{ 0 };
        double g{ 0 };
        double b{ 0 };
        double a{ 1 };
    };

    class Paint
    {
    public:
        Paint() = default;
        Paint(const Color& color);
        Paint(const std::string& ref, const Color& color);

        const Color& color() const { return m_color; }
        const std::string& ref() const { return m_ref; }
        bool isNone() const { return m_ref.empty() && m_color.isNone(); }

    public:
        Color m_color{ Color::Transparent };
        std::string m_ref;
    };

    class Point
    {
    public:
        Point() = default;
        Point(double x, double y);

    public:
        double x{ 0 };
        double y{ 0 };
    };

    using PointList = std::vector<Point>;

    class Rect
    {
    public:
        Rect() = default;
        Rect(double x, double y, double w, double h);

        Rect operator&(const Rect& rect) const;
        Rect operator|(const Rect& rect) const;

        Rect& intersect(const Rect& rect);
        Rect& unite(const Rect& rect);

        bool empty() const { return w <= 0.0 || h <= 0.0; }
        bool valid() const { return w >= 0.0 && h >= 0.0; }

        static const Rect Empty;
        static const Rect Invalid;

    public:
        double x{ 0 };
        double y{ 0 };
        double w{ 0 };
        double h{ 0 };
    };

    class Transform
    {
    public:
        Transform() = default;
        Transform(double m00, double m10, double m01, double m11, double m02, double m12);

        Transform inverted() const;
        Transform operator*(const Transform& transform) const;
        Transform& operator*=(const Transform& transform);

        Transform& premultiply(const Transform& transform);
        Transform& postmultiply(const Transform& transform);
        Transform& rotate(double angle);
        Transform& rotate(double angle, double cx, double cy);
        Transform& scale(double sx, double sy);
        Transform& shear(double shx, double shy);
        Transform& translate(double tx, double ty);
        Transform& transform(double m00, double m10, double m01, double m11, double m02, double m12);
        Transform& identity();
        Transform& invert();

        void map(double x, double y, double* _x, double* _y) const;
        Point map(double x, double y) const;
        Point map(const Point& point) const;
        Rect map(const Rect& rect) const;

        static Transform rotated(double angle);
        static Transform rotated(double angle, double cx, double cy);
        static Transform scaled(double sx, double sy);
        static Transform sheared(double shx, double shy);
        static Transform translated(double tx, double ty);

    public:
        double m00{ 1 };
        double m10{ 0 };
        double m01{ 0 };
        double m11{ 1 };
        double m02{ 0 };
        double m12{ 0 };
    };

    enum class PathCommand
    {
        MoveTo,
        LineTo,
        CubicTo,
        Close
    };

    class Path
    {
    public:
        Path() = default;

        void moveTo(double x, double y);
        void lineTo(double x, double y);
        void cubicTo(double x1, double y1, double x2, double y2, double x3, double y3);
        void close();
        void reset();
        bool empty() const;

        void quadTo(double cx, double cy, double x1, double y1, double x2, double y2);
        void arcTo(double cx, double cy, double rx, double ry, double xAxisRotation, bool largeArcFlag, bool sweepFlag, double x, double y);

        void ellipse(double cx, double cy, double rx, double ry);
        void rect(double x, double y, double w, double h, double rx, double ry);

        Rect box() const;

        const std::vector<PathCommand>& commands() const { return m_commands; }
        const std::vector<Point>& points() const { return m_points; }

    private:
        std::vector<PathCommand> m_commands;
        std::vector<Point> m_points;
    };

    class PathIterator
    {
    public:
        PathIterator(const Path& path);

        PathCommand currentSegment(std::array<Point, 3>& points) const;
        bool isDone() const;
        void next();

    private:
        mutable Point m_startPoint;
        const std::vector<PathCommand>& m_commands;
        const Point* m_points{ nullptr };
        unsigned int m_index{ 0 };
    };

    enum class LengthUnits
    {
        Unknown,
        Number,
        Px,
        Pt,
        Pc,
        In,
        Cm,
        Mm,
        Ex,
        Em,
        Percent
    };

    enum LengthMode
    {
        Width,
        Height,
        Both
    };

    class Element;

    class Length
    {
    public:
        Length() = default;
        Length(double value);
        Length(double value, LengthUnits units);

        double value(double max) const;
        double value(const Element* element, LengthMode mode) const;

        bool isValid() const { return  m_units != LengthUnits::Unknown; }
        bool isZero() const { return m_value == 0.0; }
        bool isRelative() const { return m_units == LengthUnits::Percent || m_units == LengthUnits::Em || m_units == LengthUnits::Ex; }

        static const Length Unknown;
        static const Length Zero;
        static const Length One;
        static const Length ThreePercent;
        static const Length HundredPercent;
        static const Length FiftyPercent;
        static const Length OneTwentyPercent;
        static const Length MinusTenPercent;

    private:
        double m_value{ 0 };
        LengthUnits m_units{ LengthUnits::Px };
    };

    using LengthList = std::vector<Length>;

    class LengthContext
    {
    public:
        LengthContext(const Element* element);
        LengthContext(const Element* element, Units units);

        double valueForLength(const Length& length, LengthMode mode) const;

    private:
        const Element* m_element{ nullptr };
        Units m_units{ Units::UserSpaceOnUse };
    };

    enum class Align
    {
        None,
        xMinYMin,
        xMidYMin,
        xMaxYMin,
        xMinYMid,
        xMidYMid,
        xMaxYMid,
        xMinYMax,
        xMidYMax,
        xMaxYMax
    };

    enum class MeetOrSlice
    {
        Meet,
        Slice
    };

    class PreserveAspectRatio
    {
    public:
        PreserveAspectRatio() = default;
        PreserveAspectRatio(Align align, MeetOrSlice scale);

        Transform getMatrix(double width, double height, const Rect& viewBox) const;
        Rect getClip(double width, double height, const Rect& viewBox) const;

        Align align() const { return m_align; }
        MeetOrSlice scale() const { return m_scale; }

    private:
        Align m_align{ Align::xMidYMid };
        MeetOrSlice m_scale{ MeetOrSlice::Meet };
    };

    enum class MarkerOrient
    {
        Auto,
        Angle
    };

    class Angle
    {
    public:
        Angle() = default;
        Angle(MarkerOrient type);
        Angle(double value, MarkerOrient type);

        double value() const { return m_value; }
        MarkerOrient type() const { return m_type; }

    private:
        double m_value{ 0 };
        MarkerOrient m_type{ MarkerOrient::Angle };
    };

} // namespace lunasvg

#endif // PROPERTY_H

#ifndef PLUTOVG_H
#define PLUTOVG_H


typedef struct plutovg_surface plutovg_surface_t;

plutovg_surface_t* plutovg_surface_create(int width, int height);
plutovg_surface_t* plutovg_surface_create_for_data(unsigned char* data, int width, int height, int stride);
plutovg_surface_t* plutovg_surface_reference(plutovg_surface_t* surface);
void plutovg_surface_destroy(plutovg_surface_t* surface);
int plutovg_surface_get_reference_count(const plutovg_surface_t* surface);
unsigned char* plutovg_surface_get_data(const plutovg_surface_t* surface);
int plutovg_surface_get_width(const plutovg_surface_t* surface);
int plutovg_surface_get_height(const plutovg_surface_t* surface);
int plutovg_surface_get_stride(const plutovg_surface_t* surface);

typedef struct {
    double x;
    double y;
} plutovg_point_t;

typedef struct {
    double x;
    double y;
    double w;
    double h;
} plutovg_rect_t;

void plutovg_rect_init(plutovg_rect_t* rect, double x, double y, double w, double h);
void plutovg_rect_init_zero(plutovg_rect_t* rect);

typedef struct {
    double m00; double m10;
    double m01; double m11;
    double m02; double m12;
} plutovg_matrix_t;

void plutovg_matrix_init(plutovg_matrix_t* matrix, double m00, double m10, double m01, double m11, double m02, double m12);
void plutovg_matrix_init_identity(plutovg_matrix_t* matrix);
void plutovg_matrix_init_translate(plutovg_matrix_t* matrix, double x, double y);
void plutovg_matrix_init_scale(plutovg_matrix_t* matrix, double x, double y);
void plutovg_matrix_init_shear(plutovg_matrix_t* matrix, double x, double y);
void plutovg_matrix_init_rotate(plutovg_matrix_t* matrix, double radians, double x, double y);
void plutovg_matrix_translate(plutovg_matrix_t* matrix, double x, double y);
void plutovg_matrix_scale(plutovg_matrix_t* matrix, double x, double y);
void plutovg_matrix_shear(plutovg_matrix_t* matrix, double x, double y);
void plutovg_matrix_rotate(plutovg_matrix_t* matrix, double radians, double x, double y);
void plutovg_matrix_multiply(plutovg_matrix_t* matrix, const plutovg_matrix_t* a, const plutovg_matrix_t* b);
int plutovg_matrix_invert(plutovg_matrix_t* matrix);
void plutovg_matrix_map(const plutovg_matrix_t* matrix, double x, double y, double* _x, double* _y);
void plutovg_matrix_map_point(const plutovg_matrix_t* matrix, const plutovg_point_t* src, plutovg_point_t* dst);
void plutovg_matrix_map_rect(const plutovg_matrix_t* matrix, const plutovg_rect_t* src, plutovg_rect_t* dst);

typedef struct plutovg_path plutovg_path_t;

typedef enum {
    plutovg_path_element_move_to,
    plutovg_path_element_line_to,
    plutovg_path_element_cubic_to,
    plutovg_path_element_close
} plutovg_path_element_t;

plutovg_path_t* plutovg_path_create(void);
plutovg_path_t* plutovg_path_reference(plutovg_path_t* path);
void plutovg_path_destroy(plutovg_path_t* path);
int plutovg_path_get_reference_count(const plutovg_path_t* path);
void plutovg_path_move_to(plutovg_path_t* path, double x, double y);
void plutovg_path_line_to(plutovg_path_t* path, double x, double y);
void plutovg_path_quad_to(plutovg_path_t* path, double x1, double y1, double x2, double y2);
void plutovg_path_cubic_to(plutovg_path_t* path, double x1, double y1, double x2, double y2, double x3, double y3);
void plutovg_path_close(plutovg_path_t* path);
void plutovg_path_rel_move_to(plutovg_path_t* path, double x, double y);
void plutovg_path_rel_line_to(plutovg_path_t* path, double x, double y);
void plutovg_path_rel_quad_to(plutovg_path_t* path, double x1, double y1, double x2, double y2);
void plutovg_path_rel_cubic_to(plutovg_path_t* path, double x1, double y1, double x2, double y2, double x3, double y3);
void plutovg_path_add_rect(plutovg_path_t* path, double x, double y, double w, double h);
void plutovg_path_add_round_rect(plutovg_path_t* path, double x, double y, double w, double h, double rx, double ry);
void plutovg_path_add_ellipse(plutovg_path_t* path, double cx, double cy, double rx, double ry);
void plutovg_path_add_circle(plutovg_path_t* path, double cx, double cy, double r);
void plutovg_path_add_path(plutovg_path_t* path, const plutovg_path_t* source, const plutovg_matrix_t* matrix);
void plutovg_path_transform(plutovg_path_t* path, const plutovg_matrix_t* matrix);
void plutovg_path_get_current_point(const plutovg_path_t* path, double* x, double* y);
int plutovg_path_get_element_count(const plutovg_path_t* path);
plutovg_path_element_t* plutovg_path_get_elements(const plutovg_path_t* path);
int plutovg_path_get_point_count(const plutovg_path_t* path);
plutovg_point_t* plutovg_path_get_points(const plutovg_path_t* path);
void plutovg_path_clear(plutovg_path_t* path);
int plutovg_path_empty(const plutovg_path_t* path);
plutovg_path_t* plutovg_path_clone(const plutovg_path_t* path);
plutovg_path_t* plutovg_path_clone_flat(const plutovg_path_t* path);

typedef struct {
    double r;
    double g;
    double b;
    double a;
} plutovg_color_t;

void plutovg_color_init_rgb(plutovg_color_t* color, double r, double g, double b);
void plutovg_color_init_rgba(plutovg_color_t* color, double r, double g, double b, double a);

typedef enum {
    plutovg_spread_method_pad,
    plutovg_spread_method_reflect,
    plutovg_spread_method_repeat
} plutovg_spread_method_t;

typedef struct plutovg_gradient plutovg_gradient_t;

typedef enum {
    plutovg_gradient_type_linear,
    plutovg_gradient_type_radial
} plutovg_gradient_type_t;

typedef struct {
    double offset;
    plutovg_color_t color;
} plutovg_gradient_stop_t;

plutovg_gradient_t* plutovg_gradient_create_linear(double x1, double y1, double x2, double y2);
plutovg_gradient_t* plutovg_gradient_create_radial(double cx, double cy, double cr, double fx, double fy, double fr);
plutovg_gradient_t* plutovg_gradient_reference(plutovg_gradient_t* gradient);
void plutovg_gradient_destroy(plutovg_gradient_t* gradient);
int plutovg_gradient_get_reference_count(const plutovg_gradient_t* gradient);
void plutovg_gradient_set_type(plutovg_gradient_t* gradient, plutovg_gradient_type_t type);
plutovg_gradient_type_t plutovg_gradient_get_type(const plutovg_gradient_t* gradient);
void plutovg_gradient_set_spread(plutovg_gradient_t* gradient, plutovg_spread_method_t spread);
plutovg_spread_method_t plutovg_gradient_get_spread(const plutovg_gradient_t* gradient);
void plutovg_gradient_set_matrix(plutovg_gradient_t* gradient, const plutovg_matrix_t* matrix);
void plutovg_gradient_get_matrix(const plutovg_gradient_t* gradient, plutovg_matrix_t* matrix);
void plutovg_gradient_add_stop_rgb(plutovg_gradient_t* gradient, double offset, double r, double g, double b);
void plutovg_gradient_add_stop_rgba(plutovg_gradient_t* gradient, double offset, double r, double g, double b, double a);
void plutovg_gradient_add_stop(plutovg_gradient_t* gradient, const plutovg_gradient_stop_t* stop);
void plutovg_gradient_clear_stops(plutovg_gradient_t* gradient);
int plutovg_gradient_get_stop_count(const plutovg_gradient_t* gradient);
plutovg_gradient_stop_t* plutovg_gradient_get_stops(const plutovg_gradient_t* gradient);
void plutovg_gradient_get_values_linear(const plutovg_gradient_t* gradient, double* x1, double* y1, double* x2, double* y2);
void plutovg_gradient_get_values_radial(const plutovg_gradient_t* gradient, double* cx, double* cy, double* cr, double* fx, double* fy, double* fr);
void plutovg_gradient_set_values_linear(plutovg_gradient_t* gradient, double x1, double y1, double x2, double y2);
void plutovg_gradient_set_values_radial(plutovg_gradient_t* gradient, double cx, double cy, double cr, double fx, double fy, double fr);
void plutovg_gradient_set_opacity(plutovg_gradient_t* paint, double opacity);
double plutovg_gradient_get_opacity(const plutovg_gradient_t* paint);

typedef struct plutovg_texture plutovg_texture_t;

typedef enum {
    plutovg_texture_type_plain,
    plutovg_texture_type_tiled
} plutovg_texture_type_t;

plutovg_texture_t* plutovg_texture_create(plutovg_surface_t* surface);
plutovg_texture_t* plutovg_texture_reference(plutovg_texture_t* texture);
void plutovg_texture_destroy(plutovg_texture_t* texture);
int plutovg_texture_get_reference_count(const plutovg_texture_t* texture);
void plutovg_texture_set_type(plutovg_texture_t* texture, plutovg_texture_type_t type);
plutovg_texture_type_t plutovg_texture_get_type(const plutovg_texture_t* texture);
void plutovg_texture_set_matrix(plutovg_texture_t* texture, const plutovg_matrix_t* matrix);
void plutovg_texture_get_matrix(const plutovg_texture_t* texture, plutovg_matrix_t* matrix);
void plutovg_texture_set_surface(plutovg_texture_t* texture, plutovg_surface_t* surface);
plutovg_surface_t* plutovg_texture_get_surface(const plutovg_texture_t* texture);
void plutovg_texture_set_opacity(plutovg_texture_t* texture, double opacity);
double plutovg_texture_get_opacity(const plutovg_texture_t* texture);

typedef struct plutovg_paint plutovg_paint_t;

typedef enum {
    plutovg_paint_type_color,
    plutovg_paint_type_gradient,
    plutovg_paint_type_texture
} plutovg_paint_type_t;

plutovg_paint_t* plutovg_paint_create_rgb(double r, double g, double b);
plutovg_paint_t* plutovg_paint_create_rgba(double r, double g, double b, double a);
plutovg_paint_t* plutovg_paint_create_linear(double x1, double y1, double x2, double y2);
plutovg_paint_t* plutovg_paint_create_radial(double cx, double cy, double cr, double fx, double fy, double fr);
plutovg_paint_t* plutovg_paint_create_for_surface(plutovg_surface_t* surface);
plutovg_paint_t* plutovg_paint_create_color(const plutovg_color_t* color);
plutovg_paint_t* plutovg_paint_create_gradient(plutovg_gradient_t* gradient);
plutovg_paint_t* plutovg_paint_create_texture(plutovg_texture_t* texture);
plutovg_paint_t* plutovg_paint_reference(plutovg_paint_t* paint);
void plutovg_paint_destroy(plutovg_paint_t* paint);
int plutovg_paint_get_reference_count(const plutovg_paint_t* paint);
plutovg_paint_type_t plutovg_paint_get_type(const plutovg_paint_t* paint);
plutovg_color_t* plutovg_paint_get_color(const plutovg_paint_t* paint);
plutovg_gradient_t* plutovg_paint_get_gradient(const plutovg_paint_t* paint);
plutovg_texture_t* plutovg_paint_get_texture(const plutovg_paint_t* paint);

typedef enum {
    plutovg_line_cap_butt,
    plutovg_line_cap_round,
    plutovg_line_cap_square
} plutovg_line_cap_t;

typedef enum {
    plutovg_line_join_miter,
    plutovg_line_join_round,
    plutovg_line_join_bevel
} plutovg_line_join_t;

typedef enum {
    plutovg_fill_rule_non_zero,
    plutovg_fill_rule_even_odd
} plutovg_fill_rule_t;

typedef enum {
    plutovg_operator_src,
    plutovg_operator_src_over,
    plutovg_operator_dst_in,
    plutovg_operator_dst_out
} plutovg_operator_t;

typedef struct plutovg plutovg_t;

plutovg_t* plutovg_create(plutovg_surface_t* surface);
plutovg_t* plutovg_reference(plutovg_t* pluto);
void plutovg_destroy(plutovg_t* pluto);
int plutovg_get_reference_count(const plutovg_t* pluto);
void plutovg_save(plutovg_t* pluto);
void plutovg_restore(plutovg_t* pluto);
void plutovg_set_source_rgb(plutovg_t* pluto, double r, double g, double b);
void plutovg_set_source_rgba(plutovg_t* pluto, double r, double g, double b, double a);
void plutovg_set_source_surface(plutovg_t* pluto, plutovg_surface_t* surface, double x, double y);
void plutovg_set_source_color(plutovg_t* pluto, const plutovg_color_t* color);
void plutovg_set_source_gradient(plutovg_t* pluto, plutovg_gradient_t* gradient);
void plutovg_set_source_texture(plutovg_t* pluto, plutovg_texture_t* texture);
void plutovg_set_source(plutovg_t* pluto, plutovg_paint_t* source);
plutovg_paint_t* plutovg_get_source(const plutovg_t* pluto);

void plutovg_set_operator(plutovg_t* pluto, plutovg_operator_t op);
void plutovg_set_opacity(plutovg_t* pluto, double opacity);
void plutovg_set_fill_rule(plutovg_t* pluto, plutovg_fill_rule_t fill_rule);
plutovg_operator_t plutovg_get_operator(const plutovg_t* pluto);
double plutovg_get_opacity(const plutovg_t* pluto);
plutovg_fill_rule_t plutovg_get_fill_rule(const plutovg_t* pluto);

void plutovg_set_line_width(plutovg_t* pluto, double width);
void plutovg_set_line_cap(plutovg_t* pluto, plutovg_line_cap_t cap);
void plutovg_set_line_join(plutovg_t* pluto, plutovg_line_join_t join);
void plutovg_set_miter_limit(plutovg_t* pluto, double limit);
void plutovg_set_dash(plutovg_t* pluto, double offset, const double* data, int size);
double plutovg_get_line_width(const plutovg_t* pluto);
plutovg_line_cap_t plutovg_get_line_cap(const plutovg_t* pluto);
plutovg_line_join_t plutovg_get_line_join(const plutovg_t* pluto);
double plutovg_get_miter_limit(const plutovg_t* pluto);

void plutovg_translate(plutovg_t* pluto, double x, double y);
void plutovg_scale(plutovg_t* pluto, double x, double y);
void plutovg_rotate(plutovg_t* pluto, double radians, double x, double y);
void plutovg_transform(plutovg_t* pluto, const plutovg_matrix_t* matrix);
void plutovg_set_matrix(plutovg_t* pluto, const plutovg_matrix_t* matrix);
void plutovg_identity_matrix(plutovg_t* pluto);
void plutovg_get_matrix(const plutovg_t* pluto, plutovg_matrix_t* matrix);

void plutovg_move_to(plutovg_t* pluto, double x, double y);
void plutovg_line_to(plutovg_t* pluto, double x, double y);
void plutovg_quad_to(plutovg_t* pluto, double x1, double y1, double x2, double y2);
void plutovg_cubic_to(plutovg_t* pluto, double x1, double y1, double x2, double y2, double x3, double y3);
void plutovg_rel_move_to(plutovg_t* pluto, double x, double y);
void plutovg_rel_line_to(plutovg_t* pluto, double x, double y);
void plutovg_rel_quad_to(plutovg_t* pluto, double x1, double y1, double x2, double y2);
void plutovg_rel_cubic_to(plutovg_t* pluto, double x1, double y1, double x2, double y2, double x3, double y3);
void plutovg_rect(plutovg_t* pluto, double x, double y, double w, double h);
void plutovg_round_rect(plutovg_t* pluto, double x, double y, double w, double h, double rx, double ry);
void plutovg_ellipse(plutovg_t* pluto, double cx, double cy, double rx, double ry);
void plutovg_circle(plutovg_t* pluto, double cx, double cy, double r);
void plutovg_add_path(plutovg_t* pluto, const plutovg_path_t* path);
void plutovg_new_path(plutovg_t* pluto);
void plutovg_close_path(plutovg_t* pluto);
plutovg_path_t* plutovg_get_path(const plutovg_t* pluto);

void plutovg_fill(plutovg_t* pluto);
void plutovg_stroke(plutovg_t* pluto);
void plutovg_clip(plutovg_t* pluto);
void plutovg_paint(plutovg_t* pluto);

void plutovg_fill_preserve(plutovg_t* pluto);
void plutovg_stroke_preserve(plutovg_t* pluto);
void plutovg_clip_preserve(plutovg_t* pluto);
void plutovg_reset_clip(plutovg_t* pluto);


#endif // PLUTOVG_H

#ifndef SW_FT_IMG_H
#define SW_FT_IMG_H
/***************************************************************************/
/*                                                                         */
/*  ftimage.h                                                              */
/*                                                                         */
/*    FreeType glyph image formats and default raster interface            */
/*    (specification).                                                     */
/*                                                                         */
/*  Copyright 1996-2010, 2013 by                                           */
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
  /* Note: A `raster' is simply a scan-line converter, used to render      */
  /*       SW_FT_Outlines into SW_FT_Bitmaps.                                    */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* <Struct>                                                              */
  /*    FT_BBox                                                            */
  /*                                                                       */
  /* <Description>                                                         */
  /*    A structure used to hold an outline's bounding box, i.e., the      */
  /*    coordinates of its extrema in the horizontal and vertical          */
  /*    directions.                                                        */
  /*                                                                       */
  /* <Fields>                                                              */
  /*    xMin :: The horizontal minimum (left-most).                        */
  /*                                                                       */
  /*    yMin :: The vertical minimum (bottom-most).                        */
  /*                                                                       */
  /*    xMax :: The horizontal maximum (right-most).                       */
  /*                                                                       */
  /*    yMax :: The vertical maximum (top-most).                           */
  /*                                                                       */
  /* <Note>                                                                */
  /*    The bounding box is specified with the coordinates of the lower    */
  /*    left and the upper right corner.  In PostScript, those values are  */
  /*    often called (llx,lly) and (urx,ury), respectively.                */
  /*                                                                       */
  /*    If `yMin' is negative, this value gives the glyph's descender.     */
  /*    Otherwise, the glyph doesn't descend below the baseline.           */
  /*    Similarly, if `ymax' is positive, this value gives the glyph's     */
  /*    ascender.                                                          */
  /*                                                                       */
  /*    `xMin' gives the horizontal distance from the glyph's origin to    */
  /*    the left edge of the glyph's bounding box.  If `xMin' is negative, */
  /*    the glyph extends to the left of the origin.                       */
  /*                                                                       */
typedef struct  SW_FT_BBox_
{
    SW_FT_Pos  xMin, yMin;
    SW_FT_Pos  xMax, yMax;

} SW_FT_BBox;

/*************************************************************************/
/*                                                                       */
/* <Struct>                                                              */
/*    SW_FT_Outline                                                      */
/*                                                                       */
/* <Description>                                                         */
/*    This structure is used to describe an outline to the scan-line     */
/*    converter.                                                         */
/*                                                                       */
/* <Fields>                                                              */
/*    n_contours :: The number of contours in the outline.               */
/*                                                                       */
/*    n_points   :: The number of points in the outline.                 */
/*                                                                       */
/*    points     :: A pointer to an array of `n_points' @SW_FT_Vector    */
/*                  elements, giving the outline's point coordinates.    */
/*                                                                       */
/*    tags       :: A pointer to an array of `n_points' chars, giving    */
/*                  each outline point's type.                           */
/*                                                                       */
/*                  If bit~0 is unset, the point is `off' the curve,     */
/*                  i.e., a Bézier control point, while it is `on' if    */
/*                  set.                                                 */
/*                                                                       */
/*                  Bit~1 is meaningful for `off' points only.  If set,  */
/*                  it indicates a third-order Bézier arc control point; */
/*                  and a second-order control point if unset.           */
/*                                                                       */
/*                  If bit~2 is set, bits 5-7 contain the drop-out mode  */
/*                  (as defined in the OpenType specification; the value */
/*                  is the same as the argument to the SCANMODE          */
/*                  instruction).                                        */
/*                                                                       */
/*                  Bits 3 and~4 are reserved for internal purposes.     */
/*                                                                       */
/*    contours   :: An array of `n_contours' shorts, giving the end      */
/*                  point of each contour within the outline.  For       */
/*                  example, the first contour is defined by the points  */
/*                  `0' to `contours[0]', the second one is defined by   */
/*                  the points `contours[0]+1' to `contours[1]', etc.    */
/*                                                                       */
/*    flags      :: A set of bit flags used to characterize the outline  */
/*                  and give hints to the scan-converter and hinter on   */
/*                  how to convert/grid-fit it.  See @SW_FT_OUTLINE_FLAGS.*/
/*                                                                       */
typedef struct  SW_FT_Outline_
{
    short       n_contours;      /* number of contours in glyph        */
    short       n_points;        /* number of points in the glyph      */

    SW_FT_Vector* points;          /* the outline's points               */
    char* tags;            /* the points flags                   */
    short* contours;        /* the contour end points             */
    char* contours_flag;   /* the contour open flags             */

    int         flags;           /* outline masks                      */

} SW_FT_Outline;


/*************************************************************************/
/*                                                                       */
/* <Enum>                                                                */
/*    SW_FT_OUTLINE_FLAGS                                                   */
/*                                                                       */
/* <Description>                                                         */
/*    A list of bit-field constants use for the flags in an outline's    */
/*    `flags' field.                                                     */
/*                                                                       */
/* <Values>                                                              */
/*    SW_FT_OUTLINE_NONE ::                                                 */
/*      Value~0 is reserved.                                             */
/*                                                                       */
/*    SW_FT_OUTLINE_OWNER ::                                                */
/*      If set, this flag indicates that the outline's field arrays      */
/*      (i.e., `points', `flags', and `contours') are `owned' by the     */
/*      outline object, and should thus be freed when it is destroyed.   */
/*                                                                       */
/*    SW_FT_OUTLINE_EVEN_ODD_FILL ::                                        */
/*      By default, outlines are filled using the non-zero winding rule. */
/*      If set to 1, the outline will be filled using the even-odd fill  */
/*      rule (only works with the smooth rasterizer).                    */
/*                                                                       */
/*    SW_FT_OUTLINE_REVERSE_FILL ::                                         */
/*      By default, outside contours of an outline are oriented in       */
/*      clock-wise direction, as defined in the TrueType specification.  */
/*      This flag is set if the outline uses the opposite direction      */
/*      (typically for Type~1 fonts).  This flag is ignored by the scan  */
/*      converter.                                                       */
/*                                                                       */
/*                                                                       */
/*                                                                       */
/*    There exists a second mechanism to pass the drop-out mode to the   */
/*    B/W rasterizer; see the `tags' field in @SW_FT_Outline.               */
/*                                                                       */
/*    Please refer to the description of the `SCANTYPE' instruction in   */
/*    the OpenType specification (in file `ttinst1.doc') how simple      */
/*    drop-outs, smart drop-outs, and stubs are defined.                 */
/*                                                                       */
#define SW_FT_OUTLINE_NONE             0x0
#define SW_FT_OUTLINE_OWNER            0x1
#define SW_FT_OUTLINE_EVEN_ODD_FILL    0x2
#define SW_FT_OUTLINE_REVERSE_FILL     0x4

  /* */

#define SW_FT_CURVE_TAG( flag )  ( flag & 3 )

#define SW_FT_CURVE_TAG_ON            1
#define SW_FT_CURVE_TAG_CONIC         0
#define SW_FT_CURVE_TAG_CUBIC         2


#define SW_FT_Curve_Tag_On       SW_FT_CURVE_TAG_ON
#define SW_FT_Curve_Tag_Conic    SW_FT_CURVE_TAG_CONIC
#define SW_FT_Curve_Tag_Cubic    SW_FT_CURVE_TAG_CUBIC

  /*************************************************************************/
  /*                                                                       */
  /* A raster is a scan converter, in charge of rendering an outline into  */
  /* a a bitmap.  This section contains the public API for rasters.        */
  /*                                                                       */
  /* Note that in FreeType 2, all rasters are now encapsulated within      */
  /* specific modules called `renderers'.  See `ftrender.h' for more       */
  /* details on renderers.                                                 */
  /*                                                                       */
  /*************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* <Type>                                                                */
  /*    SW_FT_Raster                                                          */
  /*                                                                       */
  /* <Description>                                                         */
  /*    A handle (pointer) to a raster object.  Each object can be used    */
  /*    independently to convert an outline into a bitmap or pixmap.       */
  /*                                                                       */
typedef struct SW_FT_RasterRec_* SW_FT_Raster;


/*************************************************************************/
/*                                                                       */
/* <Struct>                                                              */
/*    SW_FT_Span                                                            */
/*                                                                       */
/* <Description>                                                         */
/*    A structure used to model a single span of gray (or black) pixels  */
/*    when rendering a monochrome or anti-aliased bitmap.                */
/*                                                                       */
/* <Fields>                                                              */
/*    x        :: The span's horizontal start position.                  */
/*                                                                       */
/*    len      :: The span's length in pixels.                           */
/*                                                                       */
/*    coverage :: The span color/coverage, ranging from 0 (background)   */
/*                to 255 (foreground).  Only used for anti-aliased       */
/*                rendering.                                             */
/*                                                                       */
/* <Note>                                                                */
/*    This structure is used by the span drawing callback type named     */
/*    @SW_FT_SpanFunc that takes the y~coordinate of the span as a          */
/*    parameter.                                                         */
/*                                                                       */
/*    The coverage value is always between 0 and 255.  If you want less  */
/*    gray values, the callback function has to reduce them.             */
/*                                                                       */
typedef struct  SW_FT_Span_
{
    short           x;
    short           y;
    unsigned short  len;
    unsigned char   coverage;

} SW_FT_Span;


/*************************************************************************/
/*                                                                       */
/* <FuncType>                                                            */
/*    SW_FT_SpanFunc                                                        */
/*                                                                       */
/* <Description>                                                         */
/*    A function used as a call-back by the anti-aliased renderer in     */
/*    order to let client applications draw themselves the gray pixel    */
/*    spans on each scan line.                                           */
/*                                                                       */
/* <Input>                                                               */
/*    y     :: The scanline's y~coordinate.                              */
/*                                                                       */
/*    count :: The number of spans to draw on this scanline.             */
/*                                                                       */
/*    spans :: A table of `count' spans to draw on the scanline.         */
/*                                                                       */
/*    user  :: User-supplied data that is passed to the callback.        */
/*                                                                       */
/* <Note>                                                                */
/*    This callback allows client applications to directly render the    */
/*    gray spans of the anti-aliased bitmap to any kind of surfaces.     */
/*                                                                       */
/*    This can be used to write anti-aliased outlines directly to a      */
/*    given background bitmap, and even perform translucency.            */
/*                                                                       */
/*    Note that the `count' field cannot be greater than a fixed value   */
/*    defined by the `SW_FT_MAX_GRAY_SPANS' configuration macro in          */
/*    `ftoption.h'.  By default, this value is set to~32, which means    */
/*    that if there are more than 32~spans on a given scanline, the      */
/*    callback is called several times with the same `y' parameter in    */
/*    order to draw all callbacks.                                       */
/*                                                                       */
/*    Otherwise, the callback is only called once per scan-line, and     */
/*    only for those scanlines that do have `gray' pixels on them.       */
/*                                                                       */
typedef void
(*SW_FT_SpanFunc)(int             count,
    const SW_FT_Span* spans,
    void* user);

typedef void
(*SW_FT_BboxFunc)(int x, int y, int w, int h,
    void* user);

#define SW_FT_Raster_Span_Func  SW_FT_SpanFunc



/*************************************************************************/
/*                                                                       */
/* <Enum>                                                                */
/*    SW_FT_RASTER_FLAG_XXX                                                 */
/*                                                                       */
/* <Description>                                                         */
/*    A list of bit flag constants as used in the `flags' field of a     */
/*    @SW_FT_Raster_Params structure.                                       */
/*                                                                       */
/* <Values>                                                              */
/*    SW_FT_RASTER_FLAG_DEFAULT :: This value is 0.                         */
/*                                                                       */
/*    SW_FT_RASTER_FLAG_AA      :: This flag is set to indicate that an     */
/*                              anti-aliased glyph image should be       */
/*                              generated.  Otherwise, it will be        */
/*                              monochrome (1-bit).                      */
/*                                                                       */
/*    SW_FT_RASTER_FLAG_DIRECT  :: This flag is set to indicate direct      */
/*                              rendering.  In this mode, client         */
/*                              applications must provide their own span */
/*                              callback.  This lets them directly       */
/*                              draw or compose over an existing bitmap. */
/*                              If this bit is not set, the target       */
/*                              pixmap's buffer _must_ be zeroed before  */
/*                              rendering.                               */
/*                                                                       */
/*                              Note that for now, direct rendering is   */
/*                              only possible with anti-aliased glyphs.  */
/*                                                                       */
/*    SW_FT_RASTER_FLAG_CLIP    :: This flag is only used in direct         */
/*                              rendering mode.  If set, the output will */
/*                              be clipped to a box specified in the     */
/*                              `clip_box' field of the                  */
/*                              @SW_FT_Raster_Params structure.             */
/*                                                                       */
/*                              Note that by default, the glyph bitmap   */
/*                              is clipped to the target pixmap, except  */
/*                              in direct rendering mode where all spans */
/*                              are generated if no clipping box is set. */
/*                                                                       */
#define SW_FT_RASTER_FLAG_DEFAULT  0x0
#define SW_FT_RASTER_FLAG_AA       0x1
#define SW_FT_RASTER_FLAG_DIRECT   0x2
#define SW_FT_RASTER_FLAG_CLIP     0x4


  /*************************************************************************/
  /*                                                                       */
  /* <Struct>                                                              */
  /*    SW_FT_Raster_Params                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    A structure to hold the arguments used by a raster's render        */
  /*    function.                                                          */
  /*                                                                       */
  /* <Fields>                                                              */
  /*    target      :: The target bitmap.                                  */
  /*                                                                       */
  /*    source      :: A pointer to the source glyph image (e.g., an       */
  /*                   @SW_FT_Outline).                                       */
  /*                                                                       */
  /*    flags       :: The rendering flags.                                */
  /*                                                                       */
  /*    gray_spans  :: The gray span drawing callback.                     */
  /*                                                                       */
  /*    black_spans :: The black span drawing callback.  UNIMPLEMENTED!    */
  /*                                                                       */
  /*    bit_test    :: The bit test callback.  UNIMPLEMENTED!              */
  /*                                                                       */
  /*    bit_set     :: The bit set callback.  UNIMPLEMENTED!               */
  /*                                                                       */
  /*    user        :: User-supplied data that is passed to each drawing   */
  /*                   callback.                                           */
  /*                                                                       */
  /*    clip_box    :: An optional clipping box.  It is only used in       */
  /*                   direct rendering mode.  Note that coordinates here  */
  /*                   should be expressed in _integer_ pixels (and not in */
  /*                   26.6 fixed-point units).                            */
  /*                                                                       */
  /* <Note>                                                                */
  /*    An anti-aliased glyph bitmap is drawn if the @SW_FT_RASTER_FLAG_AA    */
  /*    bit flag is set in the `flags' field, otherwise a monochrome       */
  /*    bitmap is generated.                                               */
  /*                                                                       */
  /*    If the @SW_FT_RASTER_FLAG_DIRECT bit flag is set in `flags', the      */
  /*    raster will call the `gray_spans' callback to draw gray pixel      */
  /*    spans, in the case of an aa glyph bitmap, it will call             */
  /*    `black_spans', and `bit_test' and `bit_set' in the case of a       */
  /*    monochrome bitmap.  This allows direct composition over a          */
  /*    pre-existing bitmap through user-provided callbacks to perform the */
  /*    span drawing/composition.                                          */
  /*                                                                       */
  /*    Note that the `bit_test' and `bit_set' callbacks are required when */
  /*    rendering a monochrome bitmap, as they are crucial to implement    */
  /*    correct drop-out control as defined in the TrueType specification. */
  /*                                                                       */
typedef struct  SW_FT_Raster_Params_
{
    const void* source;
    int                     flags;
    SW_FT_SpanFunc          gray_spans;
    SW_FT_BboxFunc          bbox_cb;
    void* user;
    SW_FT_BBox              clip_box;

} SW_FT_Raster_Params;


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    SW_FT_Outline_Check                                                   */
/*                                                                       */
/* <Description>                                                         */
/*    Check the contents of an outline descriptor.                       */
/*                                                                       */
/* <Input>                                                               */
/*    outline :: A handle to a source outline.                           */
/*                                                                       */
/* <Return>                                                              */
/*    FreeType error code.  0~means success.                             */
/*                                                                       */
SW_FT_Error
SW_FT_Outline_Check(SW_FT_Outline* outline);


/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    SW_FT_Outline_Get_CBox                                                */
/*                                                                       */
/* <Description>                                                         */
/*    Return an outline's `control box'.  The control box encloses all   */
/*    the outline's points, including Bézier control points.  Though it  */
/*    coincides with the exact bounding box for most glyphs, it can be   */
/*    slightly larger in some situations (like when rotating an outline  */
/*    that contains Bézier outside arcs).                                */
/*                                                                       */
/*    Computing the control box is very fast, while getting the bounding */
/*    box can take much more time as it needs to walk over all segments  */
/*    and arcs in the outline.  To get the latter, you can use the       */
/*    `ftbbox' component, which is dedicated to this single task.        */
/*                                                                       */
/* <Input>                                                               */
/*    outline :: A pointer to the source outline descriptor.             */
/*                                                                       */
/* <Output>                                                              */
/*    acbox   :: The outline's control box.                              */
/*                                                                       */
/* <Note>                                                                */
/*    See @SW_FT_Glyph_Get_CBox for a discussion of tricky fonts.           */
/*                                                                       */
void
SW_FT_Outline_Get_CBox(const SW_FT_Outline* outline,
    SW_FT_BBox* acbox);


/*************************************************************************/
/*                                                                       */
/* <FuncType>                                                            */
/*    SW_FT_Raster_NewFunc                                                  */
/*                                                                       */
/* <Description>                                                         */
/*    A function used to create a new raster object.                     */
/*                                                                       */
/* <Input>                                                               */
/*    memory :: A handle to the memory allocator.                        */
/*                                                                       */
/* <Output>                                                              */
/*    raster :: A handle to the new raster object.                       */
/*                                                                       */
/* <Return>                                                              */
/*    Error code.  0~means success.                                      */
/*                                                                       */
/* <Note>                                                                */
/*    The `memory' parameter is a typeless pointer in order to avoid     */
/*    un-wanted dependencies on the rest of the FreeType code.  In       */
/*    practice, it is an @SW_FT_Memory object, i.e., a handle to the        */
/*    standard FreeType memory allocator.  However, this field can be    */
/*    completely ignored by a given raster implementation.               */
/*                                                                       */
typedef int
(*SW_FT_Raster_NewFunc)(SW_FT_Raster* raster);

#define SW_FT_Raster_New_Func  SW_FT_Raster_NewFunc


/*************************************************************************/
/*                                                                       */
/* <FuncType>                                                            */
/*    SW_FT_Raster_DoneFunc                                                 */
/*                                                                       */
/* <Description>                                                         */
/*    A function used to destroy a given raster object.                  */
/*                                                                       */
/* <Input>                                                               */
/*    raster :: A handle to the raster object.                           */
/*                                                                       */
typedef void
(*SW_FT_Raster_DoneFunc)(SW_FT_Raster  raster);

#define SW_FT_Raster_Done_Func  SW_FT_Raster_DoneFunc


/*************************************************************************/
/*                                                                       */
/* <FuncType>                                                            */
/*    SW_FT_Raster_ResetFunc                                                */
/*                                                                       */
/* <Description>                                                         */
/*    FreeType provides an area of memory called the `render pool',      */
/*    available to all registered rasters.  This pool can be freely used */
/*    during a given scan-conversion but is shared by all rasters.  Its  */
/*    content is thus transient.                                         */
/*                                                                       */
/*    This function is called each time the render pool changes, or just */
/*    after a new raster object is created.                              */
/*                                                                       */
/* <Input>                                                               */
/*    raster    :: A handle to the new raster object.                    */
/*                                                                       */
/*    pool_base :: The address in memory of the render pool.             */
/*                                                                       */
/*    pool_size :: The size in bytes of the render pool.                 */
/*                                                                       */
/* <Note>                                                                */
/*    Rasters can ignore the render pool and rely on dynamic memory      */
/*    allocation if they want to (a handle to the memory allocator is    */
/*    passed to the raster constructor).  However, this is not           */
/*    recommended for efficiency purposes.                               */
/*                                                                       */
typedef void
(*SW_FT_Raster_ResetFunc)(SW_FT_Raster       raster,
    unsigned char* pool_base,
    unsigned long   pool_size);

#define SW_FT_Raster_Reset_Func  SW_FT_Raster_ResetFunc


/*************************************************************************/
/*                                                                       */
/* <FuncType>                                                            */
/*    SW_FT_Raster_RenderFunc                                               */
/*                                                                       */
/* <Description>                                                         */
/*    Invoke a given raster to scan-convert a given glyph image into a   */
/*    target bitmap.                                                     */
/*                                                                       */
/* <Input>                                                               */
/*    raster :: A handle to the raster object.                           */
/*                                                                       */
/*    params :: A pointer to an @SW_FT_Raster_Params structure used to      */
/*              store the rendering parameters.                          */
/*                                                                       */
/* <Return>                                                              */
/*    Error code.  0~means success.                                      */
/*                                                                       */
/* <Note>                                                                */
/*    The exact format of the source image depends on the raster's glyph */
/*    format defined in its @SW_FT_Raster_Funcs structure.  It can be an    */
/*    @SW_FT_Outline or anything else in order to support a large array of  */
/*    glyph formats.                                                     */
/*                                                                       */
/*    Note also that the render function can fail and return a           */
/*    `SW_FT_Err_Unimplemented_Feature' error code if the raster used does  */
/*    not support direct composition.                                    */
/*                                                                       */
/*    XXX: For now, the standard raster doesn't support direct           */
/*         composition but this should change for the final release (see */
/*         the files `demos/src/ftgrays.c' and `demos/src/ftgrays2.c'    */
/*         for examples of distinct implementations that support direct  */
/*         composition).                                                 */
/*                                                                       */
typedef int
(*SW_FT_Raster_RenderFunc)(SW_FT_Raster                raster,
    const SW_FT_Raster_Params* params);

#define SW_FT_Raster_Render_Func  SW_FT_Raster_RenderFunc


/*************************************************************************/
/*                                                                       */
/* <Struct>                                                              */
/*    SW_FT_Raster_Funcs                                                    */
/*                                                                       */
/* <Description>                                                         */
/*   A structure used to describe a given raster class to the library.   */
/*                                                                       */
/* <Fields>                                                              */
/*    glyph_format  :: The supported glyph format for this raster.       */
/*                                                                       */
/*    raster_new    :: The raster constructor.                           */
/*                                                                       */
/*    raster_reset  :: Used to reset the render pool within the raster.  */
/*                                                                       */
/*    raster_render :: A function to render a glyph into a given bitmap. */
/*                                                                       */
/*    raster_done   :: The raster destructor.                            */
/*                                                                       */
typedef struct  SW_FT_Raster_Funcs_
{
    SW_FT_Raster_NewFunc      raster_new;
    SW_FT_Raster_ResetFunc    raster_reset;
    SW_FT_Raster_RenderFunc   raster_render;
    SW_FT_Raster_DoneFunc     raster_done;

} SW_FT_Raster_Funcs;


extern const SW_FT_Raster_Funcs   sw_ft_grays_raster;

#endif // SW_FT_IMG_H

#ifndef SW_FT_MATH_H
#define SW_FT_MATH_H

/***************************************************************************/
/*                                                                         */
/*  fttrigon.h                                                             */
/*                                                                         */
/*    FreeType trigonometric functions (specification).                    */
/*                                                                         */
/*  Copyright 2001, 2003, 2005, 2007, 2013 by                              */
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
/* The min and max functions missing in C.  As usual, be careful not to  */
/* write things like SW_FT_MIN( a++, b++ ) to avoid side effects.           */
/*                                                                       */
#define SW_FT_MIN( a, b )  ( (a) < (b) ? (a) : (b) )
#define SW_FT_MAX( a, b )  ( (a) > (b) ? (a) : (b) )

#define SW_FT_ABS( a )     ( (a) < 0 ? -(a) : (a) )

/*
 *  Approximate sqrt(x*x+y*y) using the `alpha max plus beta min'
 *  algorithm.  We use alpha = 1, beta = 3/8, giving us results with a
 *  largest error less than 7% compared to the exact value.
 */
#define SW_FT_HYPOT( x, y )                 \
        ( x = SW_FT_ABS( x ),             \
          y = SW_FT_ABS( y ),             \
          x > y ? x + ( 3 * y >> 3 )   \
                : y + ( 3 * x >> 3 ) )

 /*************************************************************************/
 /*                                                                       */
 /* <Function>                                                            */
 /*    SW_FT_MulFix                                                          */
 /*                                                                       */
 /* <Description>                                                         */
 /*    A very simple function used to perform the computation             */
 /*    `(a*b)/0x10000' with maximum accuracy.  Most of the time this is   */
 /*    used to multiply a given value by a 16.16 fixed-point factor.      */
 /*                                                                       */
 /* <Input>                                                               */
 /*    a :: The first multiplier.                                         */
 /*    b :: The second multiplier.  Use a 16.16 factor here whenever      */
 /*         possible (see note below).                                    */
 /*                                                                       */
 /* <Return>                                                              */
 /*    The result of `(a*b)/0x10000'.                                     */
 /*                                                                       */
 /* <Note>                                                                */
 /*    This function has been optimized for the case where the absolute   */
 /*    value of `a' is less than 2048, and `b' is a 16.16 scaling factor. */
 /*    As this happens mainly when scaling from notional units to         */
 /*    fractional pixels in FreeType, it resulted in noticeable speed     */
 /*    improvements between versions 2.x and 1.x.                         */
 /*                                                                       */
 /*    As a conclusion, always try to place a 16.16 factor as the         */
 /*    _second_ argument of this function; this can make a great          */
 /*    difference.                                                        */
 /*                                                                       */
SW_FT_Long
SW_FT_MulFix(SW_FT_Long  a,
    SW_FT_Long  b);

/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    SW_FT_MulDiv                                                          */
/*                                                                       */
/* <Description>                                                         */
/*    A very simple function used to perform the computation `(a*b)/c'   */
/*    with maximum accuracy (it uses a 64-bit intermediate integer       */
/*    whenever necessary).                                               */
/*                                                                       */
/*    This function isn't necessarily as fast as some processor specific */
/*    operations, but is at least completely portable.                   */
/*                                                                       */
/* <Input>                                                               */
/*    a :: The first multiplier.                                         */
/*    b :: The second multiplier.                                        */
/*    c :: The divisor.                                                  */
/*                                                                       */
/* <Return>                                                              */
/*    The result of `(a*b)/c'.  This function never traps when trying to */
/*    divide by zero; it simply returns `MaxInt' or `MinInt' depending   */
/*    on the signs of `a' and `b'.                                       */
/*                                                                       */
SW_FT_Long
SW_FT_MulDiv(SW_FT_Long  a,
    SW_FT_Long  b,
    SW_FT_Long  c);

/*************************************************************************/
/*                                                                       */
/* <Function>                                                            */
/*    SW_FT_DivFix                                                          */
/*                                                                       */
/* <Description>                                                         */
/*    A very simple function used to perform the computation             */
/*    `(a*0x10000)/b' with maximum accuracy.  Most of the time, this is  */
/*    used to divide a given value by a 16.16 fixed-point factor.        */
/*                                                                       */
/* <Input>                                                               */
/*    a :: The numerator.                                                */
/*    b :: The denominator.  Use a 16.16 factor here.                    */
/*                                                                       */
/* <Return>                                                              */
/*    The result of `(a*0x10000)/b'.                                     */
/*                                                                       */
SW_FT_Long
SW_FT_DivFix(SW_FT_Long  a,
    SW_FT_Long  b);



/*************************************************************************/
/*                                                                       */
/* <Section>                                                             */
/*   computations                                                        */
/*                                                                       */
/*************************************************************************/


/*************************************************************************
 *
 * @type:
 *   SW_FT_Angle
 *
 * @description:
 *   This type is used to model angle values in FreeType.  Note that the
 *   angle is a 16.16 fixed-point value expressed in degrees.
 *
 */
typedef SW_FT_Fixed  SW_FT_Angle;


/*************************************************************************
 *
 * @macro:
 *   SW_FT_ANGLE_PI
 *
 * @description:
 *   The angle pi expressed in @SW_FT_Angle units.
 *
 */
#define SW_FT_ANGLE_PI  ( 180L << 16 )


 /*************************************************************************
  *
  * @macro:
  *   SW_FT_ANGLE_2PI
  *
  * @description:
  *   The angle 2*pi expressed in @SW_FT_Angle units.
  *
  */
#define SW_FT_ANGLE_2PI  ( SW_FT_ANGLE_PI * 2 )


  /*************************************************************************
   *
   * @macro:
   *   SW_FT_ANGLE_PI2
   *
   * @description:
   *   The angle pi/2 expressed in @SW_FT_Angle units.
   *
   */
#define SW_FT_ANGLE_PI2  ( SW_FT_ANGLE_PI / 2 )


   /*************************************************************************
    *
    * @macro:
    *   SW_FT_ANGLE_PI4
    *
    * @description:
    *   The angle pi/4 expressed in @SW_FT_Angle units.
    *
    */
#define SW_FT_ANGLE_PI4  ( SW_FT_ANGLE_PI / 4 )


    /*************************************************************************
     *
     * @function:
     *   SW_FT_Sin
     *
     * @description:
     *   Return the sinus of a given angle in fixed-point format.
     *
     * @input:
     *   angle ::
     *     The input angle.
     *
     * @return:
     *   The sinus value.
     *
     * @note:
     *   If you need both the sinus and cosinus for a given angle, use the
     *   function @SW_FT_Vector_Unit.
     *
     */
SW_FT_Fixed
SW_FT_Sin(SW_FT_Angle  angle);


/*************************************************************************
 *
 * @function:
 *   SW_FT_Cos
 *
 * @description:
 *   Return the cosinus of a given angle in fixed-point format.
 *
 * @input:
 *   angle ::
 *     The input angle.
 *
 * @return:
 *   The cosinus value.
 *
 * @note:
 *   If you need both the sinus and cosinus for a given angle, use the
 *   function @SW_FT_Vector_Unit.
 *
 */
SW_FT_Fixed
SW_FT_Cos(SW_FT_Angle  angle);


/*************************************************************************
 *
 * @function:
 *   SW_FT_Tan
 *
 * @description:
 *   Return the tangent of a given angle in fixed-point format.
 *
 * @input:
 *   angle ::
 *     The input angle.
 *
 * @return:
 *   The tangent value.
 *
 */
SW_FT_Fixed
SW_FT_Tan(SW_FT_Angle  angle);


/*************************************************************************
 *
 * @function:
 *   SW_FT_Atan2
 *
 * @description:
 *   Return the arc-tangent corresponding to a given vector (x,y) in
 *   the 2d plane.
 *
 * @input:
 *   x ::
 *     The horizontal vector coordinate.
 *
 *   y ::
 *     The vertical vector coordinate.
 *
 * @return:
 *   The arc-tangent value (i.e. angle).
 *
 */
SW_FT_Angle
SW_FT_Atan2(SW_FT_Fixed  x,
    SW_FT_Fixed  y);


/*************************************************************************
 *
 * @function:
 *   SW_FT_Angle_Diff
 *
 * @description:
 *   Return the difference between two angles.  The result is always
 *   constrained to the ]-PI..PI] interval.
 *
 * @input:
 *   angle1 ::
 *     First angle.
 *
 *   angle2 ::
 *     Second angle.
 *
 * @return:
 *   Constrained value of `value2-value1'.
 *
 */
SW_FT_Angle
SW_FT_Angle_Diff(SW_FT_Angle  angle1,
    SW_FT_Angle  angle2);


/*************************************************************************
 *
 * @function:
 *   SW_FT_Vector_Unit
 *
 * @description:
 *   Return the unit vector corresponding to a given angle.  After the
 *   call, the value of `vec.x' will be `sin(angle)', and the value of
 *   `vec.y' will be `cos(angle)'.
 *
 *   This function is useful to retrieve both the sinus and cosinus of a
 *   given angle quickly.
 *
 * @output:
 *   vec ::
 *     The address of target vector.
 *
 * @input:
 *   angle ::
 *     The input angle.
 *
 */
void
SW_FT_Vector_Unit(SW_FT_Vector* vec,
    SW_FT_Angle    angle);


/*************************************************************************
 *
 * @function:
 *   SW_FT_Vector_Rotate
 *
 * @description:
 *   Rotate a vector by a given angle.
 *
 * @inout:
 *   vec ::
 *     The address of target vector.
 *
 * @input:
 *   angle ::
 *     The input angle.
 *
 */
void
SW_FT_Vector_Rotate(SW_FT_Vector* vec,
    SW_FT_Angle    angle);


/*************************************************************************
 *
 * @function:
 *   SW_FT_Vector_Length
 *
 * @description:
 *   Return the length of a given vector.
 *
 * @input:
 *   vec ::
 *     The address of target vector.
 *
 * @return:
 *   The vector length, expressed in the same units that the original
 *   vector coordinates.
 *
 */
SW_FT_Fixed
SW_FT_Vector_Length(SW_FT_Vector* vec);


/*************************************************************************
 *
 * @function:
 *   SW_FT_Vector_Polarize
 *
 * @description:
 *   Compute both the length and angle of a given vector.
 *
 * @input:
 *   vec ::
 *     The address of source vector.
 *
 * @output:
 *   length ::
 *     The vector length.
 *
 *   angle ::
 *     The vector angle.
 *
 */
void
SW_FT_Vector_Polarize(SW_FT_Vector* vec,
    SW_FT_Fixed* length,
    SW_FT_Angle* angle);


/*************************************************************************
 *
 * @function:
 *   SW_FT_Vector_From_Polar
 *
 * @description:
 *   Compute vector coordinates from a length and angle.
 *
 * @output:
 *   vec ::
 *     The address of source vector.
 *
 * @input:
 *   length ::
 *     The vector length.
 *
 *   angle ::
 *     The vector angle.
 *
 */
void
SW_FT_Vector_From_Polar(SW_FT_Vector* vec,
    SW_FT_Fixed    length,
    SW_FT_Angle    angle);


#endif // SW_FT_MATH_H

#ifndef PLUTOVG_PRIVATE_H
#define PLUTOVG_PRIVATE_H

struct plutovg_surface {
    int ref;
    unsigned char* data;
    int owndata;
    int width;
    int height;
    int stride;
};

struct plutovg_path {
    int ref;
    int contours;
    plutovg_point_t start;
    struct {
        plutovg_path_element_t* data;
        int size;
        int capacity;
    } elements;
    struct {
        plutovg_point_t* data;
        int size;
        int capacity;
    } points;
};

struct plutovg_gradient {
    int ref;
    plutovg_gradient_type_t type;
    plutovg_spread_method_t spread;
    plutovg_matrix_t matrix;
    double values[6];
    double opacity;
    struct {
        plutovg_gradient_stop_t* data;
        int size;
        int capacity;
    } stops;
};

struct plutovg_texture {
    int ref;
    plutovg_texture_type_t type;
    plutovg_surface_t* surface;
    plutovg_matrix_t matrix;
    double opacity;
};

struct plutovg_paint {
    int ref;
    plutovg_paint_type_t type;
    union {
        plutovg_color_t* color;
        plutovg_gradient_t* gradient;
        plutovg_texture_t* texture;
    };
};

typedef struct {
    short x;
    short y;
    unsigned short len;
    unsigned char coverage;
} plutovg_span_t;

typedef struct {
    struct {
        plutovg_span_t* data;
        int size;
        int capacity;
    } spans;

    int x;
    int y;
    int w;
    int h;
} plutovg_rle_t;

typedef struct {
    double offset;
    double* data;
    int size;
} plutovg_dash_t;

typedef struct {
    double width;
    double miterlimit;
    plutovg_line_cap_t cap;
    plutovg_line_join_t join;
    plutovg_dash_t* dash;
} plutovg_stroke_data_t;

typedef struct plutovg_state {
    plutovg_rle_t* clippath;
    plutovg_paint_t* source;
    plutovg_matrix_t matrix;
    plutovg_fill_rule_t winding;
    plutovg_stroke_data_t stroke;
    plutovg_operator_t op;
    double opacity;
    struct plutovg_state* next;
} plutovg_state_t;

struct plutovg {
    int ref;
    plutovg_surface_t* surface;
    plutovg_state_t* state;
    plutovg_path_t* path;
    plutovg_rle_t* rle;
    plutovg_rle_t* clippath;
    plutovg_rect_t clip;
};

plutovg_rle_t* plutovg_rle_create(void);
void plutovg_rle_destroy(plutovg_rle_t* rle);
void plutovg_rle_rasterize(plutovg_rle_t* rle, const plutovg_path_t* path, const plutovg_matrix_t* matrix, const plutovg_rect_t* clip, const plutovg_stroke_data_t* stroke, plutovg_fill_rule_t winding);
plutovg_rle_t* plutovg_rle_intersection(const plutovg_rle_t* a, const plutovg_rle_t* b);
void plutovg_rle_clip_path(plutovg_rle_t* rle, const plutovg_rle_t* clip);
plutovg_rle_t* plutovg_rle_clone(const plutovg_rle_t* rle);
void plutovg_rle_clear(plutovg_rle_t* rle);

plutovg_dash_t* plutovg_dash_create(double offset, const double* data, int size);
plutovg_dash_t* plutovg_dash_clone(const plutovg_dash_t* dash);
void plutovg_dash_destroy(plutovg_dash_t* dash);
plutovg_path_t* plutovg_dash_path(const plutovg_dash_t* dash, const plutovg_path_t* path);

plutovg_state_t* plutovg_state_create(void);
plutovg_state_t* plutovg_state_clone(const plutovg_state_t* state);
void plutovg_state_destroy(plutovg_state_t* state);

void plutovg_blend(plutovg_t* pluto, const plutovg_rle_t* rle);
void plutovg_blend_color(plutovg_t* pluto, const plutovg_rle_t* rle, const plutovg_color_t* color);
void plutovg_blend_gradient(plutovg_t* pluto, const plutovg_rle_t* rle, const plutovg_gradient_t* gradient);
void plutovg_blend_texture(plutovg_t* pluto, const plutovg_rle_t* rle, const plutovg_texture_t* texture);

#define plutovg_alpha(c) ((c) >> 24)
#define plutovg_red(c) (((c) >> 16) & 0xff)
#define plutovg_green(c) (((c) >> 8) & 0xff)
#define plutovg_blue(c) (((c) >> 0) & 0xff)

#define plutovg_array_init(array) \
    do { \
        array.data = NULL; \
        array.size = 0; \
        array.capacity = 0; \
    } while(0)

#define plutovg_array_ensure(array, count, dataType) \
    do { \
    if(array.size + count > array.capacity) { \
        int capacity = array.size + count; \
        int newcapacity = array.capacity == 0 ? 8 : array.capacity; \
        while(newcapacity < capacity) { newcapacity *= 2; } \
        array.data = (dataType)realloc(array.data, (size_t)newcapacity * sizeof(array.data[0])); \
        array.capacity = newcapacity; \
    } \
    } while(0)

#endif // PLUTOVG_PRIVATE_H

#ifndef ELEMENT_H
#define ELEMENT_H

namespace lunasvg {

    enum class ElementId
    {
        Unknown = 0,
        Star,
        Circle,
        ClipPath,
        Defs,
        Ellipse,
        G,
        Line,
        LinearGradient,
        Marker,
        Mask,
        Path,
        Pattern,
        Polygon,
        Polyline,
        RadialGradient,
        Rect,
        SolidColor,
        Stop,
        Style,
        Svg,
        Symbol,
        Use
    };

    enum class PropertyId
    {
        Unknown = 0,
        Class,
        Clip_Path,
        Clip_Rule,
        ClipPathUnits,
        Color,
        Cx,
        Cy,
        D,
        Display,
        Fill,
        Fill_Opacity,
        Fill_Rule,
        Fx,
        Fy,
        GradientTransform,
        GradientUnits,
        Height,
        Href,
        Id,
        Marker_End,
        Marker_Mid,
        Marker_Start,
        MarkerHeight,
        MarkerUnits,
        MarkerWidth,
        Mask,
        MaskContentUnits,
        MaskUnits,
        Offset,
        Opacity,
        Orient,
        Overflow,
        PatternContentUnits,
        PatternTransform,
        PatternUnits,
        Points,
        PreserveAspectRatio,
        R,
        RefX,
        RefY,
        Rx,
        Ry,
        Solid_Color,
        Solid_Opacity,
        SpreadMethod,
        Stop_Color,
        Stop_Opacity,
        Stroke,
        Stroke_Dasharray,
        Stroke_Dashoffset,
        Stroke_Linecap,
        Stroke_Linejoin,
        Stroke_Miterlimit,
        Stroke_Opacity,
        Stroke_Width,
        Style,
        Transform,
        ViewBox,
        Visibility,
        Width,
        X,
        X1,
        X2,
        Y,
        Y1,
        Y2
    };

    struct Property
    {
        PropertyId id;
        std::string value;
        int specificity;
    };

    class PropertyList
    {
    public:
        PropertyList() = default;

        void set(PropertyId id, const std::string& value, int specificity);
        Property* get(PropertyId id) const;
        void add(const Property& property);
        void add(const PropertyList& properties);

    private:
        std::vector<Property> m_properties;
    };

    class LayoutContext;
    class LayoutContainer;
    class Element;

    class Node
    {
    public:
        Node() = default;
        virtual ~Node() = default;

        virtual bool isText() const { return false; }
        virtual bool isPaint() const { return false; }
        virtual bool isGeometry() const { return false; }
        virtual void layout(LayoutContext*, LayoutContainer*) const;
        virtual std::unique_ptr<Node> clone() const = 0;

    public:
        Element* parent = nullptr;
    };

    class TextNode : public Node
    {
    public:
        TextNode() = default;

        bool isText() const { return true; }
        std::unique_ptr<Node> clone() const;

    public:
        std::string text;
    };

    using NodeList = std::list<std::unique_ptr<Node>>;

    class Element : public Node
    {
    public:
        Element(ElementId id);

        void set(PropertyId id, const std::string& value, int specificity);
        const std::string& get(PropertyId id) const;
        const std::string& find(PropertyId id) const;
        bool has(PropertyId id) const;

        Element* previousSibling() const;
        Element* nextSibling() const;
        Node* addChild(std::unique_ptr<Node> child);
        void layoutChildren(LayoutContext* context, LayoutContainer* current) const;
        Rect currentViewport() const;

        template<typename T>
        void transverse(T callback)
        {
            if (callback(this))
                return;

            for (auto& child : children)
            {
                if (child->isText())
                {
                    if (callback(child.get()))
                        return;
                    continue;
                }

                auto element = static_cast<Element*>(child.get());
                element->transverse(callback);
            }
        }

        template<typename T>
        std::unique_ptr<T> cloneElement() const
        {
            auto element = std::make_unique<T>();
            element->properties = properties;
            for (auto& child : children)
                element->addChild(child->clone());
            return element;
        }

    public:
        ElementId id;
        NodeList children;
        PropertyList properties;
    };

} // namespace lunasvg

#endif // ELEMENT_H

#ifndef PARSER_H
#define PARSER_H

namespace lunasvg {

    class SVGElement;
    class StyledElement;

    enum LengthNegativeValuesMode
    {
        AllowNegativeLengths,
        ForbidNegativeLengths
    };

    enum class TransformType
    {
        Matrix,
        Rotate,
        Scale,
        SkewX,
        SkewY,
        Translate
    };

    class Parser
    {
    public:
        static Length parseLength(const std::string& string, LengthNegativeValuesMode mode, const Length& defaultValue);
        static LengthList parseLengthList(const std::string& string, LengthNegativeValuesMode mode);
        static double parseNumber(const std::string& string, double defaultValue);
        static double parseNumberPercentage(const std::string& string, double defaultValue);
        static PointList parsePointList(const std::string& string);
        static Transform parseTransform(const std::string& string);
        static Path parsePath(const std::string& string);
        static std::string parseUrl(const std::string& string);
        static std::string parseHref(const std::string& string);
        static Rect parseViewBox(const std::string& string);
        static PreserveAspectRatio parsePreserveAspectRatio(const std::string& string);
        static Angle parseAngle(const std::string& string);
        static MarkerUnits parseMarkerUnits(const std::string& string);
        static SpreadMethod parseSpreadMethod(const std::string& string);
        static Units parseUnits(const std::string& string, Units defaultValue);
        static Color parseColor(const std::string& string, const StyledElement* element, const Color& defaultValue);
        static Paint parsePaint(const std::string& string, const StyledElement* element, const Color& defaultValue);
        static WindRule parseWindRule(const std::string& string);
        static LineCap parseLineCap(const std::string& string);
        static LineJoin parseLineJoin(const std::string& string);
        static Display parseDisplay(const std::string& string);
        static Visibility parseVisibility(const std::string& string);
        static Overflow parseOverflow(const std::string& string, Overflow defaultValue);

    private:
        static bool parseLength(const char*& ptr, const char* end, double& value, LengthUnits& units, LengthNegativeValuesMode mode);
        static bool parseNumberList(const char*& ptr, const char* end, double* values, int count);
        static bool parseArcFlag(const char*& ptr, const char* end, bool& flag);
        static bool parseColorComponent(const char*& ptr, const char* end, double& value);
        static bool parseTransform(const char*& ptr, const char* end, TransformType& type, double* values, int& count);
    };

    struct Selector;

    struct AttributeSelector
    {
        enum class MatchType
        {
            None,
            Equal,
            Includes,
            DashMatch,
            StartsWith,
            EndsWith,
            Contains
        };

        PropertyId id{ PropertyId::Unknown };
        std::string value;
        MatchType matchType{ MatchType::None };
    };

    using SelectorList = std::vector<Selector>;

    struct PseudoClass
    {
        enum class Type
        {
            Unknown,
            Empty,
            Root,
            Not,
            FirstChild,
            LastChild,
            OnlyChild,
            FirstOfType,
            LastOfType,
            OnlyOfType
        };

        Type type{ Type::Unknown };
        SelectorList notSelectors;
    };

    struct SimpleSelector
    {
        enum class Combinator
        {
            Descendant,
            Child,
            DirectAdjacent,
            InDirectAdjacent
        };

        ElementId id{ ElementId::Star };
        std::vector<AttributeSelector> attributeSelectors;
        std::vector<PseudoClass> pseudoClasses;
        Combinator combinator{ Combinator::Descendant };
    };

    struct Selector
    {
        std::vector<SimpleSelector> simpleSelectors;
        int specificity{ 0 };
    };

    struct Rule
    {
        SelectorList selectors;
        PropertyList declarations;
    };

    class RuleMatchContext
    {
    public:
        RuleMatchContext(const std::vector<Rule>& rules);

        std::vector<const PropertyList*> match(const Element* element) const;

    private:
        bool selectorMatch(const Selector* selector, const Element* element) const;
        bool simpleSelectorMatch(const SimpleSelector& selector, const Element* element) const;
        bool attributeSelectorMatch(const AttributeSelector& selector, const Element* element) const;
        bool pseudoClassMatch(const PseudoClass& pseudo, const Element* element) const;

    private:
        std::multimap<int, std::pair<const Selector*, const PropertyList*>, std::less<int>> m_selectors;
    };

    class CSSParser
    {
    public:
        CSSParser() = default;

        bool parseMore(const std::string& value);

        const std::vector<Rule>& rules() const { return m_rules; }

    private:
        bool parseAtRule(const char*& ptr, const char* end) const;
        bool parseRule(const char*& ptr, const char* end, Rule& rule) const;
        bool parseSelectors(const char*& ptr, const char* end, SelectorList& selectors) const;
        bool parseDeclarations(const char*& ptr, const char* end, PropertyList& declarations) const;
        bool parseSelector(const char*& ptr, const char* end, Selector& selector) const;
        bool parseSimpleSelector(const char*& ptr, const char* end, SimpleSelector& simpleSelector) const;

    private:
        std::vector<Rule> m_rules;
    };

    class LayoutSymbol;

    class ParseDocument
    {
    public:
        ParseDocument();
        ~ParseDocument();

        bool parse(const char* data, std::size_t size);

        SVGElement* rootElement() const { return m_rootElement.get(); }
        Element* getElementById(const std::string& id) const;
        std::unique_ptr<LayoutSymbol> layout() const;

    private:
        std::unique_ptr<SVGElement> m_rootElement;
        std::map<std::string, Element*> m_idCache;
    };

} // namespace lunasvg

#endif // PARSER_H

#ifndef CANVAS_H
#define CANVAS_H

namespace lunasvg {

    using GradientStop = std::pair<double, Color>;
    using GradientStops = std::vector<GradientStop>;

    using DashArray = std::vector<double>;

    struct DashData
    {
        DashArray array;
        double offset{ 0.0 };
    };

    enum class TextureType
    {
        Plain,
        Tiled
    };

    enum class BlendMode
    {
        Src,
        Src_Over,
        Dst_In,
        Dst_Out
    };

    class CanvasImpl;

    class Canvas
    {
    public:
        static std::shared_ptr<Canvas> create(unsigned char* data, unsigned int width, unsigned int height, unsigned int stride);
        static std::shared_ptr<Canvas> create(double x, double y, double width, double height);
        static std::shared_ptr<Canvas> create(const Rect& box);

        void setColor(const Color& color);
        void setLinearGradient(double x1, double y1, double x2, double y2, const GradientStops& stops, SpreadMethod spread, const Transform& transform);
        void setRadialGradient(double cx, double cy, double r, double fx, double fy, const GradientStops& stops, SpreadMethod spread, const Transform& transform);
        void setTexture(const Canvas* source, TextureType type, const Transform& transform);

        void fill(const Path& path, const Transform& transform, WindRule winding, BlendMode mode, double opacity);
        void stroke(const Path& path, const Transform& transform, double width, LineCap cap, LineJoin join, double miterlimit, const DashData& dash, BlendMode mode, double opacity);
        void blend(const Canvas* source, BlendMode mode, double opacity);
        void mask(const Rect& clip, const Transform& transform);

        void clear(unsigned int value);
        void rgba(); // with unpremultiplied alpha
        void luminance();

        unsigned int width() const;
        unsigned int height() const;
        unsigned int stride() const;
        unsigned char* data() const;
        Rect box() const;

        ~Canvas();
    private:
        Canvas(unsigned char* data, int width, int height, int stride);
        Canvas(int x, int y, int width, int height);

        plutovg_surface_t* surface;
        plutovg_t* pluto;
        plutovg_matrix_t translation;
        plutovg_rect_t rect;
    };

} // namespace lunasvg

#endif // CANVAS_H

#ifndef SW_FT_STROKER_H
#define SW_FT_STROKER_H
/***************************************************************************/
/*                                                                         */
/*  ftstroke.h                                                             */
/*                                                                         */
/*    FreeType path stroker (specification).                               */
/*                                                                         */
/*  Copyright 2002-2006, 2008, 2009, 2011-2012 by                          */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

 /**************************************************************
  *
  * @type:
  *   SW_FT_Stroker
  *
  * @description:
  *   Opaque handler to a path stroker object.
  */
typedef struct SW_FT_StrokerRec_* SW_FT_Stroker;


/**************************************************************
 *
 * @enum:
 *   SW_FT_Stroker_LineJoin
 *
 * @description:
 *   These values determine how two joining lines are rendered
 *   in a stroker.
 *
 * @values:
 *   SW_FT_STROKER_LINEJOIN_ROUND ::
 *     Used to render rounded line joins.  Circular arcs are used
 *     to join two lines smoothly.
 *
 *   SW_FT_STROKER_LINEJOIN_BEVEL ::
 *     Used to render beveled line joins.  The outer corner of
 *     the joined lines is filled by enclosing the triangular
 *     region of the corner with a straight line between the
 *     outer corners of each stroke.
 *
 *   SW_FT_STROKER_LINEJOIN_MITER_FIXED ::
 *     Used to render mitered line joins, with fixed bevels if the
 *     miter limit is exceeded.  The outer edges of the strokes
 *     for the two segments are extended until they meet at an
 *     angle.  If the segments meet at too sharp an angle (such
 *     that the miter would extend from the intersection of the
 *     segments a distance greater than the product of the miter
 *     limit value and the border radius), then a bevel join (see
 *     above) is used instead.  This prevents long spikes being
 *     created.  SW_FT_STROKER_LINEJOIN_MITER_FIXED generates a miter
 *     line join as used in PostScript and PDF.
 *
 *   SW_FT_STROKER_LINEJOIN_MITER_VARIABLE ::
 *   SW_FT_STROKER_LINEJOIN_MITER ::
 *     Used to render mitered line joins, with variable bevels if
 *     the miter limit is exceeded.  The intersection of the
 *     strokes is clipped at a line perpendicular to the bisector
 *     of the angle between the strokes, at the distance from the
 *     intersection of the segments equal to the product of the
 *     miter limit value and the border radius.  This prevents
 *     long spikes being created.
 *     SW_FT_STROKER_LINEJOIN_MITER_VARIABLE generates a mitered line
 *     join as used in XPS.  SW_FT_STROKER_LINEJOIN_MITER is an alias
 *     for SW_FT_STROKER_LINEJOIN_MITER_VARIABLE, retained for
 *     backwards compatibility.
 */
typedef enum  SW_FT_Stroker_LineJoin_
{
    SW_FT_STROKER_LINEJOIN_ROUND = 0,
    SW_FT_STROKER_LINEJOIN_BEVEL = 1,
    SW_FT_STROKER_LINEJOIN_MITER_VARIABLE = 2,
    SW_FT_STROKER_LINEJOIN_MITER = SW_FT_STROKER_LINEJOIN_MITER_VARIABLE,
    SW_FT_STROKER_LINEJOIN_MITER_FIXED = 3

} SW_FT_Stroker_LineJoin;


/**************************************************************
 *
 * @enum:
 *   SW_FT_Stroker_LineCap
 *
 * @description:
 *   These values determine how the end of opened sub-paths are
 *   rendered in a stroke.
 *
 * @values:
 *   SW_FT_STROKER_LINECAP_BUTT ::
 *     The end of lines is rendered as a full stop on the last
 *     point itself.
 *
 *   SW_FT_STROKER_LINECAP_ROUND ::
 *     The end of lines is rendered as a half-circle around the
 *     last point.
 *
 *   SW_FT_STROKER_LINECAP_SQUARE ::
 *     The end of lines is rendered as a square around the
 *     last point.
 */
typedef enum  SW_FT_Stroker_LineCap_
{
    SW_FT_STROKER_LINECAP_BUTT = 0,
    SW_FT_STROKER_LINECAP_ROUND,
    SW_FT_STROKER_LINECAP_SQUARE

} SW_FT_Stroker_LineCap;


/**************************************************************
 *
 * @enum:
 *   SW_FT_StrokerBorder
 *
 * @description:
 *   These values are used to select a given stroke border
 *   in @SW_FT_Stroker_GetBorderCounts and @SW_FT_Stroker_ExportBorder.
 *
 * @values:
 *   SW_FT_STROKER_BORDER_LEFT ::
 *     Select the left border, relative to the drawing direction.
 *
 *   SW_FT_STROKER_BORDER_RIGHT ::
 *     Select the right border, relative to the drawing direction.
 *
 * @note:
 *   Applications are generally interested in the `inside' and `outside'
 *   borders.  However, there is no direct mapping between these and the
 *   `left' and `right' ones, since this really depends on the glyph's
 *   drawing orientation, which varies between font formats.
 *
 *   You can however use @SW_FT_Outline_GetInsideBorder and
 *   @SW_FT_Outline_GetOutsideBorder to get these.
 */
typedef enum  SW_FT_StrokerBorder_
{
    SW_FT_STROKER_BORDER_LEFT = 0,
    SW_FT_STROKER_BORDER_RIGHT

} SW_FT_StrokerBorder;


/**************************************************************
 *
 * @function:
 *   SW_FT_Stroker_New
 *
 * @description:
 *   Create a new stroker object.
 *
 * @input:
 *   library ::
 *     FreeType library handle.
 *
 * @output:
 *   astroker ::
 *     A new stroker object handle.  NULL in case of error.
 *
 * @return:
 *    FreeType error code.  0~means success.
 */
SW_FT_Error
SW_FT_Stroker_New(SW_FT_Stroker* astroker);


/**************************************************************
 *
 * @function:
 *   SW_FT_Stroker_Set
 *
 * @description:
 *   Reset a stroker object's attributes.
 *
 * @input:
 *   stroker ::
 *     The target stroker handle.
 *
 *   radius ::
 *     The border radius.
 *
 *   line_cap ::
 *     The line cap style.
 *
 *   line_join ::
 *     The line join style.
 *
 *   miter_limit ::
 *     The miter limit for the SW_FT_STROKER_LINEJOIN_MITER_FIXED and
 *     SW_FT_STROKER_LINEJOIN_MITER_VARIABLE line join styles,
 *     expressed as 16.16 fixed-point value.
 *
 * @note:
 *   The radius is expressed in the same units as the outline
 *   coordinates.
 */
void
SW_FT_Stroker_Set(SW_FT_Stroker           stroker,
    SW_FT_Fixed             radius,
    SW_FT_Stroker_LineCap   line_cap,
    SW_FT_Stroker_LineJoin  line_join,
    SW_FT_Fixed             miter_limit);

/**************************************************************
 *
 * @function:
 *   SW_FT_Stroker_ParseOutline
 *
 * @description:
 *   A convenience function used to parse a whole outline with
 *   the stroker.  The resulting outline(s) can be retrieved
 *   later by functions like @SW_FT_Stroker_GetCounts and @SW_FT_Stroker_Export.
 *
 * @input:
 *   stroker ::
 *     The target stroker handle.
 *
 *   outline ::
 *     The source outline.
 *
 *
 * @return:
 *   FreeType error code.  0~means success.
 *
 * @note:
 *   If `opened' is~0 (the default), the outline is treated as a closed
 *   path, and the stroker generates two distinct `border' outlines.
 *
 *
 *   This function calls @SW_FT_Stroker_Rewind automatically.
 */
SW_FT_Error
SW_FT_Stroker_ParseOutline(SW_FT_Stroker   stroker,
    const SW_FT_Outline* outline);


/**************************************************************
 *
 * @function:
 *   SW_FT_Stroker_GetCounts
 *
 * @description:
 *   Call this function once you have finished parsing your paths
 *   with the stroker.  It returns the number of points and
 *   contours necessary to export all points/borders from the stroked
 *   outline/path.
 *
 * @input:
 *   stroker ::
 *     The target stroker handle.
 *
 * @output:
 *   anum_points ::
 *     The number of points.
 *
 *   anum_contours ::
 *     The number of contours.
 *
 * @return:
 *   FreeType error code.  0~means success.
 */
SW_FT_Error
SW_FT_Stroker_GetCounts(SW_FT_Stroker  stroker,
    SW_FT_UInt* anum_points,
    SW_FT_UInt* anum_contours);


/**************************************************************
 *
 * @function:
 *   SW_FT_Stroker_Export
 *
 * @description:
 *   Call this function after @SW_FT_Stroker_GetBorderCounts to
 *   export all borders to your own @SW_FT_Outline structure.
 *
 *   Note that this function appends the border points and
 *   contours to your outline, but does not try to resize its
 *   arrays.
 *
 * @input:
 *   stroker ::
 *     The target stroker handle.
 *
 *   outline ::
 *     The target outline handle.
 */
void
SW_FT_Stroker_Export(SW_FT_Stroker   stroker,
    SW_FT_Outline* outline);


/**************************************************************
 *
 * @function:
 *   SW_FT_Stroker_Done
 *
 * @description:
 *   Destroy a stroker object.
 *
 * @input:
 *   stroker ::
 *     A stroker handle.  Can be NULL.
 */
void
SW_FT_Stroker_Done(SW_FT_Stroker  stroker);


#endif // SW_FT_STROKER_H

#ifndef STYLEELEMENT_H
#define STYLEELEMENT_H

namespace lunasvg {

    class StyleElement : public Element
    {
    public:
        StyleElement();

        std::unique_ptr<Node> clone() const;
    };

} // namespace lunasvg

#endif // STYLEELEMENT_H

#ifndef STYLEDELEMENT_H
#define STYLEDELEMENT_H

namespace lunasvg {

    class StyledElement : public Element
    {
    public:
        StyledElement(ElementId id);

        Paint fill() const;
        Paint stroke() const;

        Color color() const;
        Color stop_color() const;
        Color solid_color() const;

        double opacity() const;
        double fill_opacity() const;
        double stroke_opacity() const;
        double stop_opacity() const;
        double solid_opacity() const;
        double stroke_miterlimit() const;

        Length stroke_width() const;
        Length stroke_dashoffset() const;
        LengthList stroke_dasharray() const;

        WindRule fill_rule() const;
        WindRule clip_rule() const;

        LineCap stroke_linecap() const;
        LineJoin stroke_linejoin() const;

        Display display() const;
        Visibility visibility() const;
        Overflow overflow() const;

        std::string clip_path() const;
        std::string mask() const;
        std::string marker_start() const;
        std::string marker_mid() const;
        std::string marker_end() const;

        bool isDisplayNone() const;
        bool isOverflowHidden() const;
    };

} // namespace lunasvg

#endif // STYLEDELEMENT_H

#ifndef STOPELEMENT_H
#define STOPELEMENT_H

namespace lunasvg {

    class StopElement : public StyledElement
    {
    public:
        StopElement();

        double offset() const;
        Color stopColorWithOpacity() const;

        std::unique_ptr<Node> clone() const;
    };

} // namespace lunasvg

#endif // STOPELEMENT_H

#ifndef MASKELEMENT_H
#define MASKELEMENT_H

namespace lunasvg {

    class LayoutMask;

    class MaskElement : public StyledElement
    {
    public:
        MaskElement();

        Length x() const;
        Length y() const;
        Length width() const;
        Length height() const;
        Units maskUnits() const;
        Units maskContentUnits() const;
        std::unique_ptr<LayoutMask> getMasker(LayoutContext* context) const;

        std::unique_ptr<Node> clone() const;
    };

} // namespace lunasvg

#endif // MASKELEMENT_H

#ifndef GRAPHICSELEMENT_H
#define GRAPHICSELEMENT_H

namespace lunasvg {

    class GraphicsElement : public StyledElement
    {
    public:
        GraphicsElement(ElementId id);

        Transform transform() const;
    };

} // namespace lunasvg

#endif // GRAPHICSELEMENT_H

#ifndef GEOMETRYELEMENT_H
#define GEOMETRYELEMENT_H

namespace lunasvg {

    class LayoutShape;

    class GeometryElement : public GraphicsElement
    {
    public:
        GeometryElement(ElementId id);

        bool isGeometry() const { return true; }
        virtual void layout(LayoutContext* context, LayoutContainer* current) const;
        virtual Path path() const = 0;
    };

    class PathElement : public GeometryElement
    {
    public:
        PathElement();

        Path d() const;
        Path path() const;

        std::unique_ptr<Node> clone() const;
    };

    class PolyElement : public GeometryElement
    {
    public:
        PolyElement(ElementId id);

        PointList points() const;
    };

    class PolygonElement : public PolyElement
    {
    public:
        PolygonElement();

        Path path() const;

        std::unique_ptr<Node> clone() const;
    };

    class PolylineElement : public PolyElement
    {
    public:
        PolylineElement();

        Path path() const;

        std::unique_ptr<Node> clone() const;
    };

    class CircleElement : public GeometryElement
    {
    public:
        CircleElement();

        Length cx() const;
        Length cy() const;
        Length r() const;

        Path path() const;

        std::unique_ptr<Node> clone() const;
    };

    class EllipseElement : public GeometryElement
    {
    public:
        EllipseElement();

        Length cx() const;
        Length cy() const;
        Length rx() const;
        Length ry() const;

        Path path() const;

        std::unique_ptr<Node> clone() const;
    };

    class LineElement : public GeometryElement
    {
    public:
        LineElement();

        Length x1() const;
        Length y1() const;
        Length x2() const;
        Length y2() const;

        Path path() const;

        std::unique_ptr<Node> clone() const;
    };

    class RectElement : public GeometryElement
    {
    public:
        RectElement();

        Length x() const;
        Length y() const;
        Length rx() const;
        Length ry() const;
        Length width() const;
        Length height() const;

        Path path() const;

        std::unique_ptr<Node> clone() const;
    };

} // namespace lunasvg

#endif // GEOMETRYELEMENT_H

#ifndef GELEMENT_H
#define GELEMENT_H

namespace lunasvg {

    class GElement : public GraphicsElement
    {
    public:
        GElement();

        void layout(LayoutContext* context, LayoutContainer* current) const;
        std::unique_ptr<Node> clone() const;
    };

} // namespace lunasvg

#endif // GELEMENT_H

#ifndef DEFSELEMENT_H
#define DEFSELEMENT_H

namespace lunasvg {

    class DefsElement : public GraphicsElement
    {
    public:
        DefsElement();

        std::unique_ptr<Node> clone() const;
    };

} // namespace lunasvg

#endif // DEFSELEMENT_H

#ifndef CLIPPATHELEMENT_H
#define CLIPPATHELEMENT_H

namespace lunasvg {

    class LayoutClipPath;

    class ClipPathElement : public GraphicsElement
    {
    public:
        ClipPathElement();

        Units clipPathUnits() const;
        std::unique_ptr<LayoutClipPath> getClipper(LayoutContext* context) const;

        std::unique_ptr<Node> clone() const;
    };

} // namespace lunasvg

#endif // CLIPPATHELEMENT_H

#ifndef SYMBOLELEMENT_H
#define SYMBOLELEMENT_H

namespace lunasvg {

    class SymbolElement : public StyledElement
    {
    public:
        SymbolElement();

        Length x() const;
        Length y() const;
        Length width() const;
        Length height() const;
        Rect viewBox() const;
        PreserveAspectRatio preserveAspectRatio() const;

        std::unique_ptr<Node> clone() const;
    };

} // namespace lunasvg

#endif // SYMBOLELEMENT_H

#ifndef PAINTELEMENT_H
#define PAINTELEMENT_H

namespace lunasvg {

    class LayoutObject;

    class PaintElement : public StyledElement
    {
    public:
        PaintElement(ElementId id);

        bool isPaint() const { return true; }
        virtual std::unique_ptr<LayoutObject> getPainter(LayoutContext* context) const = 0;
    };

    class GradientElement : public PaintElement
    {
    public:
        GradientElement(ElementId id);

        Transform gradientTransform() const;
        SpreadMethod spreadMethod() const;
        Units gradientUnits() const;
        std::string href() const;
        GradientStops buildGradientStops() const;
    };

    class LinearGradientElement : public GradientElement
    {
    public:
        LinearGradientElement();

        Length x1() const;
        Length y1() const;
        Length x2() const;
        Length y2() const;

        std::unique_ptr<LayoutObject> getPainter(LayoutContext* context) const;
        std::unique_ptr<Node> clone() const;
    };

    class RadialGradientElement : public GradientElement
    {
    public:
        RadialGradientElement();

        Length cx() const;
        Length cy() const;
        Length r() const;
        Length fx() const;
        Length fy() const;

        std::unique_ptr<LayoutObject> getPainter(LayoutContext* context) const;
        std::unique_ptr<Node> clone() const;
    };

    class PatternElement : public PaintElement
    {
    public:
        PatternElement();

        Length x() const;
        Length y() const;
        Length width() const;
        Length height() const;
        Transform patternTransform() const;
        Units patternUnits() const;
        Units patternContentUnits() const;

        Rect viewBox() const;
        PreserveAspectRatio preserveAspectRatio() const;
        std::string href() const;

        std::unique_ptr<LayoutObject> getPainter(LayoutContext* context) const;
        std::unique_ptr<Node> clone() const;
    };

    class SolidColorElement : public PaintElement
    {
    public:
        SolidColorElement();

        std::unique_ptr<LayoutObject> getPainter(LayoutContext*) const;
        std::unique_ptr<Node> clone() const;
    };

    class GradientAttributes
    {
    public:
        GradientAttributes() = default;

        const Transform& gradientTransform() const { return m_gradientTransform; }
        SpreadMethod spreadMethod() const { return m_spreadMethod; }
        Units gradientUnits() const { return m_gradientUnits; }
        const GradientStops& gradientStops() const { return m_gradientStops; }

        bool hasGradientTransform() const { return m_hasGradientTransform; }
        bool hasSpreadMethod() const { return m_hasSpreadMethod; }
        bool hasGradientUnits() const { return m_hasGradientUnits; }
        bool hasGradientStops() const { return m_hasGradientStops; }

        void setGradientTransform(const Transform& gradientTransform)
        {
            m_gradientTransform = gradientTransform;
            m_hasGradientTransform = true;
        }

        void setSpreadMethod(SpreadMethod spreadMethod)
        {
            m_spreadMethod = spreadMethod;
            m_hasSpreadMethod = true;
        }

        void setGradientUnits(Units gradientUnits)
        {
            m_gradientUnits = gradientUnits;
            m_hasGradientUnits = true;
        }

        void setGradientStops(const GradientStops& gradientStops)
        {
            m_gradientStops = gradientStops;
            m_hasGradientStops = gradientStops.size();
        }

    private:
        Transform m_gradientTransform;
        SpreadMethod m_spreadMethod{ SpreadMethod::Pad };
        Units m_gradientUnits{ Units::ObjectBoundingBox };
        GradientStops m_gradientStops;

        bool m_hasGradientTransform{ false };
        bool m_hasSpreadMethod{ false };
        bool m_hasGradientUnits{ false };
        bool m_hasGradientStops{ false };
    };

    class LinearGradientAttributes : public GradientAttributes
    {
    public:
        LinearGradientAttributes() = default;

        const Length& x1() const { return m_x1; }
        const Length& y1() const { return m_y1; }
        const Length& x2() const { return m_x2; }
        const Length& y2() const { return m_y2; }

        bool hasX1() const { return m_hasX1; }
        bool hasY1() const { return m_hasY1; }
        bool hasX2() const { return m_hasX2; }
        bool hasY2() const { return m_hasY2; }

        void setX1(const Length& x1)
        {
            m_x1 = x1;
            m_hasX1 = true;
        }

        void setY1(const Length& y1)
        {
            m_y1 = y1;
            m_hasY1 = true;
        }

        void setX2(const Length& x2)
        {
            m_x2 = x2;
            m_hasX2 = true;
        }

        void setY2(const Length& y2)
        {
            m_y2 = y2;
            m_hasY2 = true;
        }

    private:
        Length m_x1;
        Length m_y1;
        Length m_x2{ 100, LengthUnits::Percent };
        Length m_y2;

        bool m_hasX1{ false };
        bool m_hasY1{ false };
        bool m_hasX2{ false };
        bool m_hasY2{ false };
    };

    class RadialGradientAttributes : public GradientAttributes
    {
    public:
        RadialGradientAttributes() = default;

        const Length& cx() const { return m_cx; }
        const Length& cy() const { return m_cy; }
        const Length& r() const { return m_r; }
        const Length& fx() const { return m_fx; }
        const Length& fy() const { return m_fy; }

        bool hasCx() const { return m_hasCx; }
        bool hasCy() const { return m_hasCy; }
        bool hasR() const { return m_hasR; }
        bool hasFx() const { return m_hasFx; }
        bool hasFy() const { return m_hasFy; }

        void setCx(const Length& cx)
        {
            m_cx = cx;
            m_hasCx = true;
        }

        void setCy(const Length& cy)
        {
            m_cy = cy;
            m_hasCy = true;
        }

        void setR(const Length& r)
        {
            m_r = r;
            m_hasR = true;
        }

        void setFx(const Length& fx)
        {
            m_fx = fx;
            m_hasFx = true;
        }

        void setFy(const Length& fy)
        {
            m_fy = fy;
            m_hasFy = true;
        }


    private:
        Length m_cx{ 50, LengthUnits::Percent };
        Length m_cy{ 50, LengthUnits::Percent };
        Length m_r{ 50, LengthUnits::Percent };
        Length m_fx;
        Length m_fy;

        bool m_hasCx{ false };
        bool m_hasCy{ false };
        bool m_hasR{ false };
        bool m_hasFx{ false };
        bool m_hasFy{ false };
    };

    class PatternAttributes
    {
    public:
        PatternAttributes() = default;

        const Length& x() const { return m_x; }
        const Length& y() const { return m_y; }
        const Length& width() const { return m_width; }
        const Length& height() const { return m_height; }
        const Transform& patternTransform() const { return m_patternTransform; }
        Units patternUnits() const { return m_patternUnits; }
        Units patternContentUnits() const { return m_patternContentUnits; }
        const Rect& viewBox() const { return m_viewBox; }
        const PreserveAspectRatio& preserveAspectRatio() const { return m_preserveAspectRatio; }
        const PatternElement* patternContentElement() const { return m_patternContentElement; }

        bool hasX() const { return m_hasX; }
        bool hasY() const { return m_hasY; }
        bool hasWidth() const { return m_hasWidth; }
        bool hasHeight() const { return m_hasHeight; }
        bool hasPatternTransform() const { return m_hasPatternTransform; }
        bool hasPatternUnits() const { return m_hasPatternUnits; }
        bool hasPatternContentUnits() const { return m_hasPatternContentUnits; }
        bool hasViewBox() const { return m_hasViewBox; }
        bool hasPreserveAspectRatio() const { return m_hasPreserveAspectRatio; }
        bool hasPatternContentElement() const { return m_hasPatternContentElement; }

        void setX(const Length& x)
        {
            m_x = x;
            m_hasX = true;
        }

        void setY(const Length& y)
        {
            m_y = y;
            m_hasY = true;
        }

        void setWidth(const Length& width)
        {
            m_width = width;
            m_hasWidth = true;
        }

        void setHeight(const Length& height)
        {
            m_height = height;
            m_hasHeight = true;
        }

        void setPatternTransform(const Transform& patternTransform)
        {
            m_patternTransform = patternTransform;
            m_hasPatternTransform = true;
        }

        void setPatternUnits(Units patternUnits)
        {
            m_patternUnits = patternUnits;
            m_hasPatternUnits = true;
        }

        void setPatternContentUnits(Units patternContentUnits)
        {
            m_patternContentUnits = patternContentUnits;
            m_hasPatternContentUnits = true;
        }

        void setViewBox(const Rect& viewBox)
        {
            m_viewBox = viewBox;
            m_hasViewBox = true;
        }

        void setPreserveAspectRatio(const PreserveAspectRatio& preserveAspectRatio)
        {
            m_preserveAspectRatio = preserveAspectRatio;
            m_hasPreserveAspectRatio = true;
        }

        void setPatternContentElement(const PatternElement* patternContentElement)
        {
            m_patternContentElement = patternContentElement;
            m_hasPatternContentElement = true;
        }

    private:
        Length m_x;
        Length m_y;
        Length m_width;
        Length m_height;
        Transform m_patternTransform;
        Units m_patternUnits{ Units::ObjectBoundingBox };
        Units m_patternContentUnits{ Units::UserSpaceOnUse };
        Rect m_viewBox{ Rect::Invalid };
        PreserveAspectRatio m_preserveAspectRatio;
        const PatternElement* m_patternContentElement{ nullptr };

        bool m_hasX{ false };
        bool m_hasY{ false };
        bool m_hasWidth{ false };
        bool m_hasHeight{ false };
        bool m_hasPatternTransform{ false };
        bool m_hasPatternUnits{ false };
        bool m_hasPatternContentUnits{ false };
        bool m_hasViewBox{ false };
        bool m_hasPreserveAspectRatio{ false };
        bool m_hasPatternContentElement{ false };
    };

} // namespace lunasvg

#endif // PAINTELEMENT_H

#ifndef LAYOUTCONTEXT_H
#define LAYOUTCONTEXT_H

namespace lunasvg {

    enum class LayoutId
    {
        Symbol,
        Group,
        Shape,
        Mask,
        ClipPath,
        Marker,
        LinearGradient,
        RadialGradient,
        Pattern,
        SolidColor
    };

    class RenderState;

    class LayoutObject
    {
    public:
        LayoutObject(LayoutId id);
        virtual ~LayoutObject();
        virtual void render(RenderState&) const;
        virtual void apply(RenderState&) const;
        virtual Rect map(const Rect&) const;

        virtual const Rect& fillBoundingBox() const { return Rect::Invalid; }
        virtual const Rect& strokeBoundingBox() const { return Rect::Invalid; }

        bool isPaint() const { return id == LayoutId::LinearGradient || id == LayoutId::RadialGradient || id == LayoutId::Pattern || id == LayoutId::SolidColor; }
        bool isHidden() const { return isPaint() || id == LayoutId::ClipPath || id == LayoutId::Mask || id == LayoutId::Marker; }

    public:
        LayoutId id;
    };

    using LayoutList = std::list<std::unique_ptr<LayoutObject>>;

    class LayoutContainer : public LayoutObject
    {
    public:
        LayoutContainer(LayoutId id);

        const Rect& fillBoundingBox() const;
        const Rect& strokeBoundingBox() const;

        LayoutObject* addChild(std::unique_ptr<LayoutObject> child);
        LayoutObject* addChildIfNotEmpty(std::unique_ptr<LayoutContainer> child);
        void renderChildren(RenderState& state) const;

    public:
        LayoutList children;

    protected:
        mutable Rect m_fillBoundingBox{ Rect::Invalid };
        mutable Rect m_strokeBoundingBox{ Rect::Invalid };
    };

    class LayoutClipPath : public LayoutContainer
    {
    public:
        LayoutClipPath();

        void apply(RenderState& state) const;

    public:
        Units units;
        Transform transform;
        const LayoutClipPath* clipper;
    };

    class LayoutMask : public LayoutContainer
    {
    public:
        LayoutMask();

        void apply(RenderState& state) const;

    public:
        double x;
        double y;
        double width;
        double height;
        Units units;
        Units contentUnits;
        double opacity;
        const LayoutMask* masker;
        const LayoutClipPath* clipper;
    };

    class LayoutSymbol : public LayoutContainer
    {
    public:
        LayoutSymbol();

        void render(RenderState& state) const;
        Rect map(const Rect& rect) const;

    public:
        double width;
        double height;
        Transform transform;
        Rect clip;
        double opacity;
        const LayoutMask* masker;
        const LayoutClipPath* clipper;
    };

    class LayoutGroup : public LayoutContainer
    {
    public:
        LayoutGroup();

        void render(RenderState& state) const;
        Rect map(const Rect& rect) const;

    public:
        Transform transform;
        double opacity;
        const LayoutMask* masker;
        const LayoutClipPath* clipper;
    };

    class LayoutMarker : public LayoutContainer
    {
    public:
        LayoutMarker();

        Transform markerTransform(const Point& origin, double angle, double strokeWidth) const;
        Rect markerBoundingBox(const Point& origin, double angle, double strokeWidth) const;
        void renderMarker(RenderState& state, const Point& origin, double angle, double strokeWidth) const;

    public:
        double refX;
        double refY;
        Transform transform;
        Angle orient;
        MarkerUnits units;
        Rect clip;
        double opacity;
        const LayoutMask* masker;
        const LayoutClipPath* clipper;
    };

    class LayoutPattern : public LayoutContainer
    {
    public:
        LayoutPattern();

        void apply(RenderState& state) const;

    public:
        double x;
        double y;
        double width;
        double height;
        Transform transform;
        Units units;
        Units contentUnits;
        Rect viewBox;
        PreserveAspectRatio preserveAspectRatio;
    };

    class LayoutGradient : public LayoutObject
    {
    public:
        LayoutGradient(LayoutId id);

    public:
        Transform transform;
        SpreadMethod spreadMethod;
        Units units;
        GradientStops stops;
    };

    class LayoutLinearGradient : public LayoutGradient
    {
    public:
        LayoutLinearGradient();

        void apply(RenderState& state) const;

    public:
        double x1;
        double y1;
        double x2;
        double y2;
    };

    class LayoutRadialGradient : public LayoutGradient
    {
    public:
        LayoutRadialGradient();

        void apply(RenderState& state) const;

    public:
        double cx;
        double cy;
        double r;
        double fx;
        double fy;
    };

    class LayoutSolidColor : public LayoutObject
    {
    public:
        LayoutSolidColor();

        void apply(RenderState& state) const;

    public:
        Color color;
    };

    class FillData
    {
    public:
        FillData() = default;

        void fill(RenderState& state, const Path& path) const;

    public:
        const LayoutObject* painter{ nullptr };
        Color color{ Color::Transparent };
        double opacity{ 0 };
        WindRule fillRule{ WindRule::NonZero };
    };

    class StrokeData
    {
    public:
        StrokeData() = default;

        void stroke(RenderState& state, const Path& path) const;
        void inflate(Rect& box) const;

    public:
        const LayoutObject* painter{ nullptr };
        Color color{ Color::Transparent };
        double opacity{ 0 };
        double width{ 1 };
        double miterlimit{ 4 };
        LineCap cap{ LineCap::Butt };
        LineJoin join{ LineJoin::Miter };
        DashData dash;
    };

    class MarkerPosition
    {
    public:
        MarkerPosition(const LayoutMarker* marker, const Point& origin, double angle);

    public:
        const LayoutMarker* marker;
        Point origin;
        double angle;
    };

    using MarkerPositionList = std::vector<MarkerPosition>;

    class MarkerData
    {
    public:
        MarkerData() = default;

        void add(const LayoutMarker* marker, const Point& origin, double angle);
        void render(RenderState& state) const;
        void inflate(Rect& box) const;

    public:
        MarkerPositionList positions;
        double strokeWidth{ 1 };
    };

    class LayoutShape : public LayoutObject
    {
    public:
        LayoutShape();

        void render(RenderState& state) const;
        Rect map(const Rect& rect) const;
        const Rect& fillBoundingBox() const;
        const Rect& strokeBoundingBox() const;

    public:
        Path path;
        Transform transform;
        FillData fillData;
        StrokeData strokeData;
        MarkerData markerData;
        Visibility visibility;
        WindRule clipRule;
        const LayoutMask* masker;
        const LayoutClipPath* clipper;

    private:
        mutable Rect m_fillBoundingBox{ Rect::Invalid };
        mutable Rect m_strokeBoundingBox{ Rect::Invalid };
    };

    enum class RenderMode
    {
        Display,
        Clipping
    };

    struct BlendInfo
    {
        const LayoutClipPath* clipper;
        const LayoutMask* masker;
        double opacity;
        Rect clip;
    };

    class RenderState
    {
    public:
        RenderState(const LayoutObject* object, RenderMode mode);

        void beginGroup(RenderState& state, const BlendInfo& info);
        void endGroup(RenderState& state, const BlendInfo& info);

        const LayoutObject* object() const { return m_object; }
        RenderMode mode() const { return m_mode; }
        const Rect& objectBoundingBox() const { return m_object->fillBoundingBox(); }

    public:
        std::shared_ptr<Canvas> canvas;
        Transform transform;

    private:
        const LayoutObject* m_object;
        RenderMode m_mode;
    };

    class ParseDocument;
    class StyledElement;
    class GeometryElement;

    class LayoutContext
    {
    public:
        LayoutContext(const ParseDocument* document, LayoutSymbol* root);

        Element* getElementById(const std::string& id) const;
        LayoutObject* getResourcesById(const std::string& id) const;
        LayoutObject* addToResourcesCache(const std::string& id, std::unique_ptr<LayoutObject> resources);
        LayoutMask* getMasker(const std::string& id);
        LayoutClipPath* getClipper(const std::string& id);
        LayoutMarker* getMarker(const std::string& id);
        LayoutObject* getPainter(const std::string& id);

        FillData fillData(const StyledElement* element);
        DashData dashData(const StyledElement* element);
        StrokeData strokeData(const StyledElement* element);
        MarkerData markerData(const GeometryElement* element, const Path& path);

        void addReference(const Element* element);
        void removeReference(const Element* element);
        bool hasReference(const Element* element) const;

    private:
        const ParseDocument* m_document;
        LayoutSymbol* m_root;
        std::map<std::string, LayoutObject*> m_resourcesCache;
        std::set<const Element*> m_references;
    };

    class LayoutBreaker
    {
    public:
        LayoutBreaker(LayoutContext* context, const Element* element);
        ~LayoutBreaker();

    private:
        LayoutContext* m_context;
        const Element* m_element;
    };

} // namespace lunasvg

#endif // LAYOUTCONTEXT_H

#ifndef SVGELEMENT_H
#define SVGELEMENT_H

namespace lunasvg {

    class ParseDocument;
    class LayoutSymbol;

    class SVGElement : public GraphicsElement
    {
    public:
        SVGElement();

        Length x() const;
        Length y() const;
        Length width() const;
        Length height() const;

        Rect viewBox() const;
        PreserveAspectRatio preserveAspectRatio() const;
        std::unique_ptr<LayoutSymbol> layoutDocument(const ParseDocument* document) const;

        void layout(LayoutContext* context, LayoutContainer* current) const;
        std::unique_ptr<Node> clone() const;
    };

} // namespace lunasvg

#endif // SVGELEMENT_H

#ifndef USEELEMENT_H
#define USEELEMENT_H

namespace lunasvg {

    class UseElement : public GraphicsElement
    {
    public:
        UseElement();

        Length x() const;
        Length y() const;
        Length width() const;
        Length height() const;
        std::string href() const;
        void transferWidthAndHeight(Element* element) const;

        void layout(LayoutContext* context, LayoutContainer* current) const;
        std::unique_ptr<Node> clone() const;
    };

} // namespace lunasvg

#endif // USEELEMENT_H

#ifndef MARKERELEMENT_H
#define MARKERELEMENT_H

namespace lunasvg {

    class LayoutMarker;

    class MarkerElement : public StyledElement
    {
    public:
        MarkerElement();

        Length refX() const;
        Length refY() const;
        Length markerWidth() const;
        Length markerHeight() const;
        Angle orient() const;
        MarkerUnits markerUnits() const;

        Rect viewBox() const;
        PreserveAspectRatio preserveAspectRatio() const;
        std::unique_ptr<LayoutMarker> getMarker(LayoutContext* context) const;

        std::unique_ptr<Node> clone() const;
    };

} // namespace lunasvg

#endif // MARKERELEMENT_H




