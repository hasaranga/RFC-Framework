
/*
 * Copyright (c) 2020-2025 Samuel Ugochukwu <sammycageagle@gmail.com>
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

#pragma once

#include <windows.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <float.h>
#include <cmath>

namespace plutosvg {

#define PLUTOVG_BUILD_STATIC
#define PLUTOVG_EXPORT
#define PLUTOVG_IMPORT
#define PLUTOVG_API

#define PLUTOVG_VERSION_MAJOR 1
#define PLUTOVG_VERSION_MINOR 1
#define PLUTOVG_VERSION_MICRO 0

#define PLUTOVG_VERSION_ENCODE(major, minor, micro) (((major) * 10000) + ((minor) * 100) + ((micro) * 1))
#define PLUTOVG_VERSION PLUTOVG_VERSION_ENCODE(PLUTOVG_VERSION_MAJOR, PLUTOVG_VERSION_MINOR, PLUTOVG_VERSION_MICRO)

#define PLUTOVG_VERSION_XSTRINGIZE(major, minor, micro) #major"."#minor"."#micro
#define PLUTOVG_VERSION_STRINGIZE(major, minor, micro) PLUTOVG_VERSION_XSTRINGIZE(major, minor, micro)
#define PLUTOVG_VERSION_STRING PLUTOVG_VERSION_STRINGIZE(PLUTOVG_VERSION_MAJOR, PLUTOVG_VERSION_MINOR, PLUTOVG_VERSION_MICRO)


	/**
	 * @brief Gets the version of the plutovg library.
	 * @return An integer representing the version of the plutovg library.
	 */
	PLUTOVG_API int plutovg_version(void);

	/**
	 * @brief Gets the version of the plutovg library as a string.
	 * @return A string representing the version of the plutovg library.
	 */
	PLUTOVG_API const char* plutovg_version_string(void);

	/**
	 * @brief A function pointer type for a cleanup callback.
	 * @param closure A pointer to the resource to be cleaned up.
	 */
	typedef void (*plutovg_destroy_func_t)(void* closure);

	/**
	 * @brief A function pointer type for a write callback.
	 * @param closure A pointer to user-defined data or context.
	 * @param data A pointer to the data to be written.
	 * @param size The size of the data in bytes.
	 */
	typedef void (*plutovg_write_func_t)(void* closure, void* data, int size);

#define PLUTOVG_PI      3.14159265358979323846f
#define PLUTOVG_TWO_PI  6.28318530717958647693f
#define PLUTOVG_HALF_PI 1.57079632679489661923f
#define PLUTOVG_SQRT2   1.41421356237309504880f
#define PLUTOVG_KAPPA   0.55228474983079339840f

#define PLUTOVG_DEG2RAD(x) ((x) * (PLUTOVG_PI / 180.0f))
#define PLUTOVG_RAD2DEG(x) ((x) * (180.0f / PLUTOVG_PI))


	/**
	 * @brief A structure representing a point in 2D space.
	 */
	typedef struct plutovg_point {
		float x; ///< The x-coordinate of the point.
		float y; ///< The y-coordinate of the point.
	} plutovg_point_t;

#define PLUTOVG_MAKE_POINT(x, y) plutovg_point_t{x, y}
#define PLUTOVG_EMPTY_POINT PLUTOVG_MAKE_POINT(0, 0)

	/**
	 * @brief A structure representing a rectangle in 2D space.
	 */
	typedef struct plutovg_rect {
		float x; ///< The x-coordinate of the top-left corner of the rectangle.
		float y; ///< The y-coordinate of the top-left corner of the rectangle.
		float w; ///< The width of the rectangle.
		float h; ///< The height of the rectangle.
	} plutovg_rect_t;

#define PLUTOVG_MAKE_RECT(x, y, w, h) plutovg_rect_t{x, y, w, h}
#define PLUTOVG_EMPTY_RECT PLUTOVG_MAKE_RECT(0, 0, 0, 0)

	/**
	 * @brief A structure representing a 2D transformation matrix.
	 */
	typedef struct plutovg_matrix {
		float a; ///< The horizontal scaling factor.
		float b; ///< The vertical shearing factor.
		float c; ///< The horizontal shearing factor.
		float d; ///< The vertical scaling factor.
		float e; ///< The horizontal translation offset.
		float f; ///< The vertical translation offset.
	} plutovg_matrix_t;

#define PLUTOVG_MAKE_MATRIX(a, b, c, d, e, f) plutovg_matrix_t{a, b, c, d, e, f}
#define PLUTOVG_MAKE_SCALE(x, y) PLUTOVG_MAKE_MATRIX(x, 0, 0, y, 0, 0)
#define PLUTOVG_MAKE_TRANSLATE(x, y) PLUTOVG_MAKE_MATRIX(1, 0, 0, 1, x, y)
#define PLUTOVG_IDENTITY_MATRIX PLUTOVG_MAKE_MATRIX(1, 0, 0, 1, 0, 0)


	/**
	 * @brief Initializes a 2D transformation matrix.
	 * @param matrix A pointer to the `plutovg_matrix_t` object to be initialized.
	 * @param a The horizontal scaling factor.
	 * @param b The vertical shearing factor.
	 * @param c The horizontal shearing factor.
	 * @param d The vertical scaling factor.
	 * @param e The horizontal translation offset.
	 * @param f The vertical translation offset.
	 */
	PLUTOVG_API void plutovg_matrix_init(plutovg_matrix_t* matrix, float a, float b, float c, float d, float e, float f);

	/**
	 * @brief Initializes a 2D transformation matrix to the identity matrix.
	 * @param matrix A pointer to the `plutovg_matrix_t` object to be initialized.
	 */
	PLUTOVG_API void plutovg_matrix_init_identity(plutovg_matrix_t* matrix);

	/**
	 * @brief Initializes a 2D transformation matrix for translation.
	 * @param matrix A pointer to the `plutovg_matrix_t` object to be initialized.
	 * @param tx The translation offset in the x-direction.
	 * @param ty The translation offset in the y-direction.
	 */
	PLUTOVG_API void plutovg_matrix_init_translate(plutovg_matrix_t* matrix, float tx, float ty);

	/**
	 * @brief Initializes a 2D transformation matrix for scaling.
	 * @param matrix A pointer to the `plutovg_matrix_t` object to be initialized.
	 * @param sx The scaling factor in the x-direction.
	 * @param sy The scaling factor in the y-direction.
	 */
	PLUTOVG_API void plutovg_matrix_init_scale(plutovg_matrix_t* matrix, float sx, float sy);

	/**
	 * @brief Initializes a 2D transformation matrix for rotation.
	 * @param matrix A pointer to the `plutovg_matrix_t` object to be initialized.
	 * @param angle The rotation angle in radians.
	 */
	PLUTOVG_API void plutovg_matrix_init_rotate(plutovg_matrix_t* matrix, float angle);

	/**
	 * @brief Initializes a 2D transformation matrix for shearing.
	 * @param matrix A pointer to the `plutovg_matrix_t` object to be initialized.
	 * @param shx The shearing factor in the x-direction.
	 * @param shy The shearing factor in the y-direction.
	 */
	PLUTOVG_API void plutovg_matrix_init_shear(plutovg_matrix_t* matrix, float shx, float shy);

	/**
	 * @brief Adds a translation with offsets `tx` and `ty` to the matrix.
	 * @param matrix A pointer to the `plutovg_matrix_t` object to be modified.
	 * @param tx The translation offset in the x-direction.
	 * @param ty The translation offset in the y-direction.
	 */
	PLUTOVG_API void plutovg_matrix_translate(plutovg_matrix_t* matrix, float tx, float ty);

	/**
	 * @brief Scales the matrix by factors `sx` and `sy`
	 * @param matrix A pointer to the `plutovg_matrix_t` object to be modified.
	 * @param sx The scaling factor in the x-direction.
	 * @param sy The scaling factor in the y-direction.
	 */
	PLUTOVG_API void plutovg_matrix_scale(plutovg_matrix_t* matrix, float sx, float sy);

	/**
	 * @brief Rotates the matrix by the specified angle (in radians).
	 * @param matrix A pointer to the `plutovg_matrix_t` object to be modified.
	 * @param angle The rotation angle in radians.
	 */
	PLUTOVG_API void plutovg_matrix_rotate(plutovg_matrix_t* matrix, float angle);

	/**
	 * @brief Shears the matrix by factors `shx` and `shy`.
	 * @param matrix A pointer to the `plutovg_matrix_t` object to be modified.
	 * @param shx The shearing factor in the x-direction.
	 * @param shy The shearing factor in the y-direction.
	 */
	PLUTOVG_API void plutovg_matrix_shear(plutovg_matrix_t* matrix, float shx, float shy);

	/**
	 * @brief Multiplies `left` and `right` matrices and stores the result in `matrix`.
	 * @note `matrix` can be identical to either `left` or `right`.
	 * @param matrix A pointer to the `plutovg_matrix_t` object to store the result.
	 * @param left A pointer to the first `plutovg_matrix_t` matrix.
	 * @param right A pointer to the second `plutovg_matrix_t` matrix.
	 */
	PLUTOVG_API void plutovg_matrix_multiply(plutovg_matrix_t* matrix, const plutovg_matrix_t* left, const plutovg_matrix_t* right);

	/**
	 * @brief Calculates the inverse of `matrix` and stores it in `inverse`.
	 *
	 * If `inverse` is `NULL`, the function only checks if the matrix is invertible.
	 *
	 * @note `matrix` and `inverse` can be identical.
	 * @param matrix A pointer to the `plutovg_matrix_t` object to invert.
	 * @param inverse A pointer to the `plutovg_matrix_t` object to store the result, or `NULL`.
	 * @return `true` if the matrix is invertible; `false` otherwise.
	 */
	PLUTOVG_API bool plutovg_matrix_invert(const plutovg_matrix_t* matrix, plutovg_matrix_t* inverse);

	/**
	 * @brief Transforms the point `(x, y)` using `matrix` and stores the result in `(xx, yy)`.
	 * @param matrix A pointer to a `plutovg_matrix_t` object.
	 * @param x The x-coordinate of the point to transform.
	 * @param y The y-coordinate of the point to transform.
	 * @param xx A pointer to store the transformed x-coordinate.
	 * @param yy A pointer to store the transformed y-coordinate.
	 */
	PLUTOVG_API void plutovg_matrix_map(const plutovg_matrix_t* matrix, float x, float y, float* xx, float* yy);

	/**
	 * @brief Transforms the `src` point using `matrix` and stores the result in `dst`.
	 * @note `src` and `dst` can be identical.
	 * @param matrix A pointer to a `plutovg_matrix_t` object.
	 * @param src A pointer to the `plutovg_point_t` object to transform.
	 * @param dst A pointer to the `plutovg_point_t` to store the transformed point.
	 */
	PLUTOVG_API void plutovg_matrix_map_point(const plutovg_matrix_t* matrix, const plutovg_point_t* src, plutovg_point_t* dst);

	/**
	 * @brief Transforms an array of `src` points using `matrix` and stores the results in `dst`.
	 * @note `src` and `dst` can be identical.
	 * @param matrix A pointer to a `plutovg_matrix_t` object.
	 * @param src A pointer to the array of `plutovg_point_t` objects to transform.
	 * @param dst A pointer to the array of `plutovg_point_t` to store the transformed points.
	 * @param count The number of points to transform.
	 */
	PLUTOVG_API void plutovg_matrix_map_points(const plutovg_matrix_t* matrix, const plutovg_point_t* src, plutovg_point_t* dst, int count);

	/**
	 * @brief Transforms the `src` rectangle using `matrix` and stores the result in `dst`.
	 * @note `src` and `dst` can be identical.
	 * @param matrix A pointer to a `plutovg_matrix_t` object.
	 * @param src A pointer to the `plutovg_rect_t` object to transform.
	 * @param dst A pointer to the `plutovg_rect_t` to store the transformed rectangle.
	 */
	PLUTOVG_API void plutovg_matrix_map_rect(const plutovg_matrix_t* matrix, const plutovg_rect_t* src, plutovg_rect_t* dst);

	/**
	 * @brief Parses an SVG transform string into a matrix.
	 *
	 * @param matrix A pointer to a `plutovg_matrix_t` object to store the result.
	 * @param data Input SVG transform string.
	 * @param length Length of the string, or `-1` if null-terminated.
	 *
	 * @return `true` on success, `false` on failure.
	 */
	PLUTOVG_API bool plutovg_matrix_parse(plutovg_matrix_t* matrix, const char* data, int length);


	/**
	 * @brief Represents a 2D path for drawing operations.
	 */
	typedef struct plutovg_path plutovg_path_t;

	/**
	 * @brief Enumeration defining path commands.
	 */
	typedef enum plutovg_path_command {
		PLUTOVG_PATH_COMMAND_MOVE_TO, ///< Moves the current point to a new position.
		PLUTOVG_PATH_COMMAND_LINE_TO, ///< Draws a straight line to a new point.
		PLUTOVG_PATH_COMMAND_CUBIC_TO, ///< Draws a cubic Bézier curve to a new point.
		PLUTOVG_PATH_COMMAND_CLOSE ///< Closes the current path by drawing a line to the starting point.
	} plutovg_path_command_t;

	/**
	 * @brief Union representing a path element.
	 *
	 * A path element can be a command with a length or a coordinate point.
	 * Each command type in the path element array is followed by a specific number of points:
	 * - `PLUTOVG_PATH_COMMAND_MOVE_TO`: 1 point
	 * - `PLUTOVG_PATH_COMMAND_LINE_TO`: 1 point
	 * - `PLUTOVG_PATH_COMMAND_CUBIC_TO`: 3 points
	 * - `PLUTOVG_PATH_COMMAND_CLOSE`: 1 point
	 *
	 * @example
	 * const plutovg_path_element_t* elements;
	 * int count = plutovg_path_get_elements(path, &elements);
	 * for(int i = 0; i < count; i += elements[i].header.length) {
	 *     plutovg_path_command_t command = elements[i].header.command;
	 *     switch(command) {
	 *     case PLUTOVG_PATH_COMMAND_MOVE_TO:
	 *         printf("MoveTo: %g %g\n", elements[i + 1].point.x, elements[i + 1].point.y);
	 *         break;
	 *     case PLUTOVG_PATH_COMMAND_LINE_TO:
	 *         printf("LineTo: %g %g\n", elements[i + 1].point.x, elements[i + 1].point.y);
	 *         break;
	 *     case PLUTOVG_PATH_COMMAND_CUBIC_TO:
	 *         printf("CubicTo: %g %g %g %g %g %g\n",
	 *                elements[i + 1].point.x, elements[i + 1].point.y,
	 *                elements[i + 2].point.x, elements[i + 2].point.y,
	 *                elements[i + 3].point.x, elements[i + 3].point.y);
	 *         break;
	 *     case PLUTOVG_PATH_COMMAND_CLOSE:
	 *         printf("Close: %g %g\n", elements[i + 1].point.x, elements[i + 1].point.y);
	 *         break;
	 *     }
	 * }
	 */
	typedef union plutovg_path_element {
		struct {
			plutovg_path_command_t command; ///< The path command.
			int length; ///< Number of elements including the header.
		} header; ///< Header for path commands.
		plutovg_point_t point; ///< A coordinate point in the path.
	} plutovg_path_element_t;

	/**
	 * @brief Iterator for traversing path elements in a path.
	 */
	typedef struct plutovg_path_iterator {
		const plutovg_path_element_t* elements; ///< Pointer to the array of path elements.
		int size; ///< Total number of elements in the array.
		int index; ///< Current position in the array.
	} plutovg_path_iterator_t;

	/**
	 * @brief Initializes a path iterator for a given path.
	 *
	 * @param it The path iterator to initialize.
	 * @param path The path to iterate over.
	 */
	PLUTOVG_API void plutovg_path_iterator_init(plutovg_path_iterator_t* it, const plutovg_path_t* path);

	/**
	 * @brief Checks if there are more elements to iterate over.
	 *
	 * @param it The path iterator.
	 * @return `true` if there are more elements; otherwise, `false`.
	 */
	PLUTOVG_API bool plutovg_path_iterator_has_next(const plutovg_path_iterator_t* it);

	/**
	 * @brief Retrieves the current command and its associated points, then advances the iterator.
	 *
	 * @param it The path iterator.
	 * @param points An array to store the points for the current command.
	 * @return The path command for the current element.
	 */
	PLUTOVG_API plutovg_path_command_t plutovg_path_iterator_next(plutovg_path_iterator_t* it, plutovg_point_t points[3]);

	/**
	 * @brief Creates a new path object.
	 *
	 * @return A pointer to the newly created path object.
	 */
	PLUTOVG_API plutovg_path_t* plutovg_path_create(void);

	/**
	 * @brief Increases the reference count of a path object.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @return A pointer to the same `plutovg_path_t` object.
	 */
	PLUTOVG_API plutovg_path_t* plutovg_path_reference(plutovg_path_t* path);

	/**
	 * @brief Decreases the reference count of a path object.
	 *
	 * This function decrements the reference count of the given path object. If
	 * the reference count reaches zero, the path object is destroyed and its
	 * resources are freed.
	 *
	 * @param path A pointer to the `plutovg_path_t` object.
	 */
	PLUTOVG_API void plutovg_path_destroy(plutovg_path_t* path);

	/**
	 * @brief Retrieves the reference count of a path object.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @return The current reference count of the path object.
	 */
	PLUTOVG_API int plutovg_path_get_reference_count(const plutovg_path_t* path);

	/**
	 * @brief Retrieves the elements of a path.
	 *
	 * Provides access to the array of path elements.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param elements A pointer to a pointer that will be set to the array of path elements.
	 * @return The number of elements in the path.
	 */
	PLUTOVG_API int plutovg_path_get_elements(const plutovg_path_t* path, const plutovg_path_element_t** elements);

	/**
	 * @brief Moves the current point to a new position.
	 *
	 * This function moves the current point to the specified coordinates without
	 * drawing a line. This is equivalent to the `M` command in SVG path syntax.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param x The x-coordinate of the new position.
	 * @param y The y-coordinate of the new position.
	 */
	PLUTOVG_API void plutovg_path_move_to(plutovg_path_t* path, float x, float y);

	/**
	 * @brief Adds a straight line segment to the path.
	 *
	 * This function adds a straight line segment from the current point to the
	 * specified coordinates. This is equivalent to the `L` command in SVG path syntax.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param x The x-coordinate of the end point of the line segment.
	 * @param y The y-coordinate of the end point of the line segment.
	 */
	PLUTOVG_API void plutovg_path_line_to(plutovg_path_t* path, float x, float y);

	/**
	 * @brief Adds a quadratic Bézier curve to the path.
	 *
	 * This function adds a quadratic Bézier curve segment from the current point
	 * to the specified end point, using the given control point. This is equivalent
	 * to the `Q` command in SVG path syntax.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param x1 The x-coordinate of the control point.
	 * @param y1 The y-coordinate of the control point.
	 * @param x2 The x-coordinate of the end point of the curve.
	 * @param y2 The y-coordinate of the end point of the curve.
	 */
	PLUTOVG_API void plutovg_path_quad_to(plutovg_path_t* path, float x1, float y1, float x2, float y2);

	/**
	 * @brief Adds a cubic Bézier curve to the path.
	 *
	 * This function adds a cubic Bézier curve segment from the current point
	 * to the specified end point, using the given two control points. This is
	 * equivalent to the `C` command in SVG path syntax.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param x1 The x-coordinate of the first control point.
	 * @param y1 The y-coordinate of the first control point.
	 * @param x2 The x-coordinate of the second control point.
	 * @param y2 The y-coordinate of the second control point.
	 * @param x3 The x-coordinate of the end point of the curve.
	 * @param y3 The y-coordinate of the end point of the curve.
	 */
	PLUTOVG_API void plutovg_path_cubic_to(plutovg_path_t* path, float x1, float y1, float x2, float y2, float x3, float y3);

	/**
	 * @brief Adds an elliptical arc to the path.
	 *
	 * This function adds an elliptical arc segment from the current point to the
	 * specified end point. The arc is defined by the radii, rotation angle, and
	 * flags for large arc and sweep. This is equivalent to the `A` command in SVG
	 * path syntax.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param rx The x-radius of the ellipse.
	 * @param ry The y-radius of the ellipse.
	 * @param angle The rotation angle of the ellipse in radians.
	 * @param large_arc_flag If true, draw the large arc; otherwise, draw the small arc.
	 * @param sweep_flag If true, draw the arc in the positive-angle direction; otherwise, in the negative-angle direction.
	 * @param x The x-coordinate of the end point of the arc.
	 * @param y The y-coordinate of the end point of the arc.
	 */
	PLUTOVG_API void plutovg_path_arc_to(plutovg_path_t* path, float rx, float ry, float angle, bool large_arc_flag, bool sweep_flag, float x, float y);

	/**
	 * @brief Closes the current sub-path.
	 *
	 * This function closes the current sub-path by drawing a straight line back to
	 * the start point of the sub-path. This is equivalent to the `Z` command in SVG
	 * path syntax.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 */
	PLUTOVG_API void plutovg_path_close(plutovg_path_t* path);

	/**
	 * @brief Retrieves the current point of the path.
	 *
	 * Gets the current point's coordinates in the path. This point is the last
	 * position used or the point where the path was last moved to.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param x The x-coordinate of the current point.
	 * @param y The y-coordinate of the current point.
	 */
	PLUTOVG_API void plutovg_path_get_current_point(const plutovg_path_t* path, float* x, float* y);

	/**
	 * @brief Reserves space for path elements.
	 *
	 * Reserves space for a specified number of elements in the path. This helps optimize
	 * memory allocation for future path operations.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param count The number of path elements to reserve space for.
	 */
	PLUTOVG_API void plutovg_path_reserve(plutovg_path_t* path, int count);

	/**
	 * @brief Resets the path.
	 *
	 * Clears all path data, effectively resetting the `plutovg_path_t` object to its initial state.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 */
	PLUTOVG_API void plutovg_path_reset(plutovg_path_t* path);

	/**
	 * @brief Adds a rectangle to the path.
	 *
	 * Adds a rectangle defined by the top-left corner (x, y) and dimensions (w, h) to the path.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param x The x-coordinate of the rectangle's top-left corner.
	 * @param y The y-coordinate of the rectangle's top-left corner.
	 * @param w The width of the rectangle.
	 * @param h The height of the rectangle.
	 */
	PLUTOVG_API void plutovg_path_add_rect(plutovg_path_t* path, float x, float y, float w, float h);

	/**
	 * @brief Adds a rounded rectangle to the path.
	 *
	 * Adds a rounded rectangle defined by the top-left corner (x, y), dimensions (w, h),
	 * and corner radii (rx, ry) to the path.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param x The x-coordinate of the rectangle's top-left corner.
	 * @param y The y-coordinate of the rectangle's top-left corner.
	 * @param w The width of the rectangle.
	 * @param h The height of the rectangle.
	 * @param rx The x-radius of the rectangle's corners.
	 * @param ry The y-radius of the rectangle's corners.
	 */
	PLUTOVG_API void plutovg_path_add_round_rect(plutovg_path_t* path, float x, float y, float w, float h, float rx, float ry);

	/**
	 * @brief Adds an ellipse to the path.
	 *
	 * Adds an ellipse defined by the center (cx, cy) and radii (rx, ry) to the path.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param cx The x-coordinate of the ellipse's center.
	 * @param cy The y-coordinate of the ellipse's center.
	 * @param rx The x-radius of the ellipse.
	 * @param ry The y-radius of the ellipse.
	 */
	PLUTOVG_API void plutovg_path_add_ellipse(plutovg_path_t* path, float cx, float cy, float rx, float ry);

	/**
	 * @brief Adds a circle to the path.
	 *
	 * Adds a circle defined by its center (cx, cy) and radius (r) to the path.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param cx The x-coordinate of the circle's center.
	 * @param cy The y-coordinate of the circle's center.
	 * @param r The radius of the circle.
	 */
	PLUTOVG_API void plutovg_path_add_circle(plutovg_path_t* path, float cx, float cy, float r);

	/**
	 * @brief Adds an arc to the path.
	 *
	 * Adds an arc defined by the center (cx, cy), radius (r), start angle (a0), end angle (a1),
	 * and direction (ccw) to the path.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param cx The x-coordinate of the arc's center.
	 * @param cy The y-coordinate of the arc's center.
	 * @param r The radius of the arc.
	 * @param a0 The start angle of the arc in radians.
	 * @param a1 The end angle of the arc in radians.
	 * @param ccw If true, the arc is drawn counter-clockwise; if false, clockwise.
	 */
	PLUTOVG_API void plutovg_path_add_arc(plutovg_path_t* path, float cx, float cy, float r, float a0, float a1, bool ccw);

	/**
	 * @brief Adds a sub-path to the path.
	 *
	 * Adds all elements from another path (`source`) to the current path, optionally
	 * applying a transformation matrix.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param source A pointer to the `plutovg_path_t` object to copy elements from.
	 * @param matrix A pointer to a `plutovg_matrix_t` object, or `NULL` to apply no transformation.
	 */
	PLUTOVG_API void plutovg_path_add_path(plutovg_path_t* path, const plutovg_path_t* source, const plutovg_matrix_t* matrix);

	/**
	 * @brief Applies a transformation matrix to the path.
	 *
	 * Transforms the entire path using the provided transformation matrix.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param matrix A pointer to a `plutovg_matrix_t` object.
	 */
	PLUTOVG_API void plutovg_path_transform(plutovg_path_t* path, const plutovg_matrix_t* matrix);

	/**
	 * @brief Callback function type for traversing a path.
	 *
	 * This function type defines a callback used to traverse path elements.
	 *
	 * @param closure A pointer to user-defined data passed to the callback.
	 * @param command The current path command.
	 * @param points An array of points associated with the command.
	 * @param npoints The number of points in the array.
	 */
	typedef void (*plutovg_path_traverse_func_t)(void* closure, plutovg_path_command_t command, const plutovg_point_t* points, int npoints);

	/**
	 * @brief Traverses the path and calls the callback for each element.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param traverse_func The callback function to be called for each element of the path.
	 * @param closure User-defined data passed to the callback.
	 */
	PLUTOVG_API void plutovg_path_traverse(const plutovg_path_t* path, plutovg_path_traverse_func_t traverse_func, void* closure);

	/**
	 * @brief Traverses the path with Bézier curves flattened to line segments.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param traverse_func The callback function to be called for each element of the path.
	 * @param closure User-defined data passed to the callback.
	 */
	PLUTOVG_API void plutovg_path_traverse_flatten(const plutovg_path_t* path, plutovg_path_traverse_func_t traverse_func, void* closure);

	/**
	 * @brief Traverses the path with a dashed pattern and calls the callback for each segment.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param offset The starting offset into the dash pattern.
	 * @param dashes An array of dash lengths.
	 * @param ndashes The number of elements in the `dashes` array.
	 * @param traverse_func The callback function to be called for each element of the path.
	 * @param closure User-defined data passed to the callback.
	 */
	PLUTOVG_API void plutovg_path_traverse_dashed(const plutovg_path_t* path, float offset, const float* dashes, int ndashes, plutovg_path_traverse_func_t traverse_func, void* closure);

	/**
	 * @brief Creates a copy of the path.
	 *
	 * @param path A pointer to the `plutovg_path_t` object to clone.
	 * @return A pointer to the newly created path clone.
	 */
	PLUTOVG_API plutovg_path_t* plutovg_path_clone(const plutovg_path_t* path);

	/**
	 * @brief Creates a copy of the path with Bézier curves flattened to line segments.
	 *
	 * @param path A pointer to the `plutovg_path_t` object to clone.
	 * @return A pointer to the newly created path clone with flattened curves.
	 */
	PLUTOVG_API plutovg_path_t* plutovg_path_clone_flatten(const plutovg_path_t* path);

	/**
	 * @brief Creates a copy of the path with a dashed pattern applied.
	 *
	 * @param path A pointer to the `plutovg_path_t` object to clone.
	 * @param offset The starting offset into the dash pattern.
	 * @param dashes An array of dash lengths.
	 * @param ndashes The number of elements in the `dashes` array.
	 * @return A pointer to the newly created path clone with dashed pattern.
	 */
	PLUTOVG_API plutovg_path_t* plutovg_path_clone_dashed(const plutovg_path_t* path, float offset, const float* dashes, int ndashes);

	/**
	 * @brief Computes the bounding box and total length of the path.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @param extents A pointer to a `plutovg_rect_t` object to store the bounding box.
	 * @param tight If `true`, computes a precise bounding box; otherwise, aligns to control points.
	 * @return The total length of the path.
	 */
	PLUTOVG_API float plutovg_path_extents(const plutovg_path_t* path, plutovg_rect_t* extents, bool tight);



	/**
	 * @brief Calculates the total length of the path.
	 *
	 * @param path A pointer to a `plutovg_path_t` object.
	 * @return The total length of the path.
	 */
	PLUTOVG_API float plutovg_path_length(const plutovg_path_t* path);

	/**
	 * @brief Parses SVG path data into a `plutovg_path_t` object.
	 *
	 * @param path A pointer to the `plutovg_path_t` object to populate.
	 * @param data The SVG path data string.
	 * @param length The length of `data`, or `-1` for null-terminated data.
	 * @return `true` if successful; `false` otherwise.
	 */
	PLUTOVG_API bool plutovg_path_parse(plutovg_path_t* path, const char* data, int length);

	/**
 * @brief Represents a color with red, green, blue, and alpha components.
 */
	typedef struct plutovg_color {
		float r; ///< Red component (0 to 1).
		float g; ///< Green component (0 to 1).
		float b; ///< Blue component (0 to 1).
		float a; ///< Alpha (opacity) component (0 to 1).
	} plutovg_color_t;

#define PLUTOVG_MAKE_COLOR(r, g, b, a) plutovg_color_t{r, g, b, a}

#define PLUTOVG_BLACK_COLOR   PLUTOVG_MAKE_COLOR(0, 0, 0, 1)
#define PLUTOVG_WHITE_COLOR   PLUTOVG_MAKE_COLOR(1, 1, 1, 1)
#define PLUTOVG_RED_COLOR     PLUTOVG_MAKE_COLOR(1, 0, 0, 1)
#define PLUTOVG_GREEN_COLOR   PLUTOVG_MAKE_COLOR(0, 1, 0, 1)
#define PLUTOVG_BLUE_COLOR    PLUTOVG_MAKE_COLOR(0, 0, 1, 1)
#define PLUTOVG_YELLOW_COLOR  PLUTOVG_MAKE_COLOR(1, 1, 0, 1)
#define PLUTOVG_CYAN_COLOR    PLUTOVG_MAKE_COLOR(0, 1, 1, 1)
#define PLUTOVG_MAGENTA_COLOR PLUTOVG_MAKE_COLOR(1, 0, 1, 1)

	/**
	 * @brief Initializes a color using RGB components in the 0-1 range.
	 *
	 * @param color A pointer to a `plutovg_color_t` object.
	 * @param r Red component (0 to 1).
	 * @param g Green component (0 to 1).
	 * @param b Blue component (0 to 1).
	 */
	PLUTOVG_API void plutovg_color_init_rgb(plutovg_color_t* color, float r, float g, float b);

	/**
	 * @brief Initializes a color using RGBA components in the 0-1 range.
	 *
	 * @param color A pointer to a `plutovg_color_t` object.
	 * @param r Red component (0 to 1).
	 * @param g Green component (0 to 1).
	 * @param b Blue component (0 to 1).
	 * @param a Alpha component (0 to 1).
	 */
	PLUTOVG_API void plutovg_color_init_rgba(plutovg_color_t* color, float r, float g, float b, float a);

	/**
	 * @brief Initializes a color using RGB components in the 0-255 range.
	 *
	 * @param color A pointer to a `plutovg_color_t` object.
	 * @param r Red component (0 to 255).
	 * @param g Green component (0 to 255).
	 * @param b Blue component (0 to 255).
	 */
	PLUTOVG_API void plutovg_color_init_rgb8(plutovg_color_t* color, int r, int g, int b);

	/**
	 * @brief Initializes a color using RGBA components in the 0-255 range.
	 *
	 * @param color A pointer to a `plutovg_color_t` object.
	 * @param r Red component (0 to 255).
	 * @param g Green component (0 to 255).
	 * @param b Blue component (0 to 255).
	 * @param a Alpha component (0 to 255).
	 */
	PLUTOVG_API void plutovg_color_init_rgba8(plutovg_color_t* color, int r, int g, int b, int a);

	/**
	 * @brief Initializes a color from a 32-bit unsigned RGBA value.
	 *
	 * @param color A pointer to a `plutovg_color_t` object.
	 * @param value 32-bit unsigned RGBA value.
	 */
	PLUTOVG_API void plutovg_color_init_rgba32(plutovg_color_t* color, unsigned int value);

	/**
	 * @brief Initializes a color from a 32-bit unsigned ARGB value.
	 *
	 * @param color A pointer to a `plutovg_color_t` object.
	 * @param value 32-bit unsigned ARGB value.
	 */
	PLUTOVG_API void plutovg_color_init_argb32(plutovg_color_t* color, unsigned int value);

	/**
	 * @brief Initializes a color with the specified HSL color values.
	 *
	 * @param color A pointer to a `plutovg_color_t` object.
	 * @param h Hue component in degrees (0 to 360).
	 * @param s Saturation component (0 to 1).
	 * @param l Lightness component (0 to 1).
	 */
	PLUTOVG_API void plutovg_color_init_hsl(plutovg_color_t* color, float h, float s, float l);

	/**
	 * @brief Initializes a color with the specified HSLA color values.
	 *
	 * @param color A pointer to a `plutovg_color_t` object.
	 * @param h Hue component in degrees (0 to 360).
	 * @param s Saturation component (0 to 1).
	 * @param l Lightness component (0 to 1).
	 * @param a Alpha component (0 to 1).
	 */
	PLUTOVG_API void plutovg_color_init_hsla(plutovg_color_t* color, float h, float s, float l, float a);

	/**
	 * @brief Converts a color to a 32-bit unsigned RGBA value.
	 *
	 * @param color A pointer to a `plutovg_color_t` object.
	 *
	 * @return 32-bit unsigned RGBA value.
	 */
	PLUTOVG_API unsigned int plutovg_color_to_rgba32(const plutovg_color_t* color);

	/**
	 * @brief Converts a color to a 32-bit unsigned ARGB value.
	 *
	 * @param color A pointer to a `plutovg_color_t` object.
	 *
	 * @return 32-bit unsigned ARGB value.
	 */
	PLUTOVG_API unsigned int plutovg_color_to_argb32(const plutovg_color_t* color);

	/**
	 * @brief Parses a color from a string using CSS color syntax.
	 *
	 * @param color A pointer to a `plutovg_color_t` object to store the parsed color.
	 * @param data A pointer to the input string containing the color data.
	 * @param length The length of the input string in bytes, or `-1` if the string is null-terminated.
	 *
	 * @return The number of characters consumed on success (including leading/trailing spaces), or 0 on failure.
	 */
	PLUTOVG_API int plutovg_color_parse(plutovg_color_t* color, const char* data, int length);

	/**
	 * @brief Represents an image surface for drawing operations.
	 *
	 * Stores pixel data in a 32-bit premultiplied ARGB format (0xAARRGGBB),
	 * where red, green, and blue channels are multiplied by the alpha channel
	 * and divided by 255.
	 */
	typedef struct plutovg_surface plutovg_surface_t;

	/**
	 * @brief Creates a new image surface with the specified dimensions.
	 *
	 * @param width The width of the surface in pixels.
	 * @param height The height of the surface in pixels.
	 * @return A pointer to the newly created `plutovg_surface_t` object.
	 */
	PLUTOVG_API plutovg_surface_t* plutovg_surface_create(int width, int height);

	/**
	 * @brief Creates an image surface using existing pixel data.
	 *
	 * @param data Pointer to the pixel data.
	 * @param width The width of the surface in pixels.
	 * @param height The height of the surface in pixels.
	 * @param stride The number of bytes per row in the pixel data.
	 * @return A pointer to the newly created `plutovg_surface_t` object.
	 */
	PLUTOVG_API plutovg_surface_t* plutovg_surface_create_for_data(unsigned char* data, int width, int height, int stride);


	/**
	 * @brief Increments the reference count for a surface.
	 *
	 * @param surface Pointer to the `plutovg_surface_t` object.
	 * @return Pointer to the `plutovg_surface_t` object.
	 */
	PLUTOVG_API plutovg_surface_t* plutovg_surface_reference(plutovg_surface_t* surface);

	/**
	 * @brief Decrements the reference count and destroys the surface if the count reaches zero.
	 *
	 * @param surface Pointer to the `plutovg_surface_t` object .
	 */
	PLUTOVG_API void plutovg_surface_destroy(plutovg_surface_t* surface);

	/**
	 * @brief Gets the current reference count of a surface.
	 *
	 * @param surface Pointer to the `plutovg_surface_t` object.
	 * @return The reference count of the surface.
	 */
	PLUTOVG_API int plutovg_surface_get_reference_count(const plutovg_surface_t* surface);

	/**
	 * @brief Gets the pixel data of the surface.
	 *
	 * @param surface Pointer to the `plutovg_surface_t` object.
	 * @return Pointer to the pixel data.
	 */
	PLUTOVG_API unsigned char* plutovg_surface_get_data(const plutovg_surface_t* surface);

	/**
	 * @brief Gets the width of the surface.
	 *
	 * @param surface Pointer to the `plutovg_surface_t` object.
	 * @return Width of the surface in pixels.
	 */
	PLUTOVG_API int plutovg_surface_get_width(const plutovg_surface_t* surface);

	/**
	 * @brief Gets the height of the surface.
	 *
	 * @param surface Pointer to the `plutovg_surface_t` object.
	 * @return Height of the surface in pixels.
	 */
	PLUTOVG_API int plutovg_surface_get_height(const plutovg_surface_t* surface);

	/**
	 * @brief Gets the stride of the surface.
	 *
	 * @param surface Pointer to the `plutovg_surface_t` object.
	 * @return Number of bytes per row.
	 */
	PLUTOVG_API int plutovg_surface_get_stride(const plutovg_surface_t* surface);

	/**
	 * @brief Clears the entire surface with the specified color.
	 *
	 * @param surface Pointer to the target surface.
	 * @param color Pointer to the color used for clearing.
	 */
	PLUTOVG_API void plutovg_surface_clear(plutovg_surface_t* surface, const plutovg_color_t* color);


	/**
	 * @brief Converts pixel data from premultiplied ARGB to RGBA format.
	 *
	 * Transforms pixel data from native-endian 32-bit ARGB premultiplied format
	 * to a non-premultiplied RGBA byte sequence.
	 *
	 * @param dst Pointer to the destination buffer (can overlap with `src`).
	 * @param src Pointer to the source buffer in ARGB premultiplied format.
	 * @param width Image width in pixels.
	 * @param height Image height in pixels.
	 * @param stride Number of bytes per image row in the buffers.
	 */
	PLUTOVG_API void plutovg_convert_argb_to_rgba(unsigned char* dst, const unsigned char* src, int width, int height, int stride);

	/**
	 * @brief Converts pixel data from RGBA to premultiplied ARGB format.
	 *
	 * Transforms pixel data from a non-premultiplied RGBA byte sequence
	 * to a native-endian 32-bit ARGB premultiplied format.
	 *
	 * @param dst Pointer to the destination buffer (can overlap with `src`).
	 * @param src Pointer to the source buffer in RGBA format.
	 * @param width Image width in pixels.
	 * @param height Image height in pixels.
	 * @param stride Number of bytes per image row in the buffers.
	 */
	PLUTOVG_API void plutovg_convert_rgba_to_argb(unsigned char* dst, const unsigned char* src, int width, int height, int stride);


	/**
	 * @brief Defines the type of texture, either plain or tiled.
	 */
	typedef enum {
		PLUTOVG_TEXTURE_TYPE_PLAIN, ///< Plain texture.
		PLUTOVG_TEXTURE_TYPE_TILED ///< Tiled texture.
	} plutovg_texture_type_t;

	/**
	 * @brief Defines the spread method for gradients.
	 */
	typedef enum {
		PLUTOVG_SPREAD_METHOD_PAD, ///< Pad the gradient's edges.
		PLUTOVG_SPREAD_METHOD_REFLECT, ///< Reflect the gradient beyond its bounds.
		PLUTOVG_SPREAD_METHOD_REPEAT ///< Repeat the gradient pattern.
	} plutovg_spread_method_t;

	/**
	 * @brief Represents a gradient stop.
	 */
	typedef struct {
		float offset; ///< The offset of the gradient stop, as a value between 0 and 1.
		plutovg_color_t color; ///< The color of the gradient stop.
	} plutovg_gradient_stop_t;

	/**
	 * @brief Represents a paint object used for drawing operations.
	 */
	typedef struct plutovg_paint plutovg_paint_t;

	/**
	 * @brief Creates a solid RGB paint.
	 *
	 * @param r The red component (0 to 1).
	 * @param g The green component (0 to 1).
	 * @param b The blue component (0 to 1).
	 * @return A pointer to the created `plutovg_paint_t` object.
	 */
	PLUTOVG_API plutovg_paint_t* plutovg_paint_create_rgb(float r, float g, float b);

	/**
	 * @brief Creates a solid RGBA paint.
	 *
	 * @param r The red component (0 to 1).
	 * @param g The green component (0 to 1).
	 * @param b The blue component (0 to 1).
	 * @param a The alpha component (0 to 1).
	 * @return A pointer to the created `plutovg_paint_t` object.
	 */
	PLUTOVG_API plutovg_paint_t* plutovg_paint_create_rgba(float r, float g, float b, float a);

	/**
	 * @brief Creates a solid color paint.
	 *
	 * @param color A pointer to the `plutovg_color_t` object.
	 * @return A pointer to the created `plutovg_paint_t` object.
	 */
	PLUTOVG_API plutovg_paint_t* plutovg_paint_create_color(const plutovg_color_t* color);

	/**
	 * @brief Creates a linear gradient paint.
	 *
	 * @param x1 The x coordinate of the gradient start.
	 * @param y1 The y coordinate of the gradient start.
	 * @param x2 The x coordinate of the gradient end.
	 * @param y2 The y coordinate of the gradient end.
	 * @param spread The gradient spread method.
	 * @param stops Array of gradient stops.
	 * @param nstops Number of gradient stops.
	 * @param matrix Optional transformation matrix.
	 * @return A pointer to the created `plutovg_paint_t` object.
	 */
	PLUTOVG_API plutovg_paint_t* plutovg_paint_create_linear_gradient(float x1, float y1, float x2, float y2,
		plutovg_spread_method_t spread, const plutovg_gradient_stop_t* stops, int nstops, const plutovg_matrix_t* matrix);

	/**
	 * @brief Creates a radial gradient paint.
	 *
	 * @param cx The x coordinate of the gradient center.
	 * @param cy The y coordinate of the gradient center.
	 * @param cr The radius of the gradient.
	 * @param fx The x coordinate of the focal point.
	 * @param fy The y coordinate of the focal point.
	 * @param fr The radius of the focal point.
	 * @param spread The gradient spread method.
	 * @param stops Array of gradient stops.
	 * @param nstops Number of gradient stops.
	 * @param matrix Optional transformation matrix.
	 * @return A pointer to the created `plutovg_paint_t` object.
	 */
	PLUTOVG_API plutovg_paint_t* plutovg_paint_create_radial_gradient(float cx, float cy, float cr, float fx, float fy, float fr,
		plutovg_spread_method_t spread, const plutovg_gradient_stop_t* stops, int nstops, const plutovg_matrix_t* matrix);

	/**
	 * @brief Creates a texture paint from a surface.
	 *
	 * @param surface The texture surface.
	 * @param type The texture type (plain or tiled).
	 * @param opacity The opacity of the texture (0 to 1).
	 * @param matrix Optional transformation matrix.
	 * @return A pointer to the created `plutovg_paint_t` object.
	 */
	PLUTOVG_API plutovg_paint_t* plutovg_paint_create_texture(plutovg_surface_t* surface, plutovg_texture_type_t type, float opacity, const plutovg_matrix_t* matrix);

	/**
	 * @brief Increments the reference count of a paint object.
	 *
	 * @param paint A pointer to the `plutovg_paint_t` object.
	 * @return A pointer to the referenced `plutovg_paint_t` object.
	 */
	PLUTOVG_API plutovg_paint_t* plutovg_paint_reference(plutovg_paint_t* paint);

	/**
	 * @brief Decrements the reference count and destroys the paint if the count reaches zero.
	 *
	 * @param paint A pointer to the `plutovg_paint_t` object.
	 */
	PLUTOVG_API void plutovg_paint_destroy(plutovg_paint_t* paint);

	/**
	 * @brief Retrieves the reference count of a paint object.
	 *
	 * @param paint A pointer to the `plutovg_paint_t` object.
	 * @return The reference count of the `plutovg_paint_t` object.
	 */
	PLUTOVG_API int plutovg_paint_get_reference_count(const plutovg_paint_t* paint);

	/**
	 * @brief Defines fill rule types for filling paths.
	 */
	typedef enum {
		PLUTOVG_FILL_RULE_NON_ZERO, ///< Non-zero winding fill rule.
		PLUTOVG_FILL_RULE_EVEN_ODD ///< Even-odd fill rule.
	} plutovg_fill_rule_t;

	/**
	 * @brief Defines compositing operations.
	 */
	typedef enum {
		PLUTOVG_OPERATOR_CLEAR,       ///< Clears the destination (resulting in a fully transparent image).
		PLUTOVG_OPERATOR_SRC,         ///< Source replaces destination.
		PLUTOVG_OPERATOR_DST,         ///< Destination is kept, source is ignored.
		PLUTOVG_OPERATOR_SRC_OVER,    ///< Source is composited over destination.
		PLUTOVG_OPERATOR_DST_OVER,    ///< Destination is composited over source.
		PLUTOVG_OPERATOR_SRC_IN,      ///< Source within destination (only the overlapping part of source is shown).
		PLUTOVG_OPERATOR_DST_IN,      ///< Destination within source.
		PLUTOVG_OPERATOR_SRC_OUT,     ///< Source outside destination (non-overlapping part of source is shown).
		PLUTOVG_OPERATOR_DST_OUT,     ///< Destination outside source.
		PLUTOVG_OPERATOR_SRC_ATOP,    ///< Source atop destination (source shown over destination but only in the destination's bounds).
		PLUTOVG_OPERATOR_DST_ATOP,    ///< Destination atop source (destination shown over source but only in the source's bounds).
		PLUTOVG_OPERATOR_XOR          ///< Source and destination are combined, but their overlapping regions are cleared.
	} plutovg_operator_t;

	/**
	 * @brief Defines the shape used at the ends of open subpaths.
	 */
	typedef enum {
		PLUTOVG_LINE_CAP_BUTT, ///< Flat edge at the end of the stroke.
		PLUTOVG_LINE_CAP_ROUND, ///< Rounded ends at the end of the stroke.
		PLUTOVG_LINE_CAP_SQUARE ///< Square ends at the end of the stroke.
	} plutovg_line_cap_t;

	/**
	 * @brief Defines the shape used at the corners of paths.
	 */
	typedef enum {
		PLUTOVG_LINE_JOIN_MITER, ///< Miter join with sharp corners.
		PLUTOVG_LINE_JOIN_ROUND, ///< Rounded join.
		PLUTOVG_LINE_JOIN_BEVEL ///< Beveled join with a flattened corner.
	} plutovg_line_join_t;

	/**
	 * @brief Represents a drawing context.
	 */
	typedef struct plutovg_canvas plutovg_canvas_t;

	/**
	 * @brief Creates a drawing context on a surface.
	 *
	 * @param surface A pointer to a `plutovg_surface_t` object.
	 * @return A pointer to the newly created `plutovg_canvas_t` object.
	 */
	PLUTOVG_API plutovg_canvas_t* plutovg_canvas_create(plutovg_surface_t* surface);

	/**
	 * @brief Increases the reference count of the canvas.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @return The same pointer to the `plutovg_canvas_t` object.
	 */
	PLUTOVG_API plutovg_canvas_t* plutovg_canvas_reference(plutovg_canvas_t* canvas);

	/**
	 * @brief Decreases the reference count and destroys the canvas when it reaches zero.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_destroy(plutovg_canvas_t* canvas);

	/**
	 * @brief Retrieves the reference count of the canvas.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @return The current reference count.
	 */
	PLUTOVG_API int plutovg_canvas_get_reference_count(const plutovg_canvas_t* canvas);

	/**
	 * @brief Gets the surface associated with the canvas.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @return A pointer to the `plutovg_surface_t` object.
	 */
	PLUTOVG_API plutovg_surface_t* plutovg_canvas_get_surface(const plutovg_canvas_t* canvas);

	/**
	 * @brief Saves the current state of the canvas.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_save(plutovg_canvas_t* canvas);

	/**
	 * @brief Restores the canvas to the most recently saved state.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_restore(plutovg_canvas_t* canvas);

	/**
	 * @brief Sets the current paint to a solid color.
	 *
	 * If not set, the default paint is opaque black color.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param r The red component (0 to 1).
	 * @param g The green component (0 to 1).
	 * @param b The blue component (0 to 1).
	 */
	PLUTOVG_API void plutovg_canvas_set_rgb(plutovg_canvas_t* canvas, float r, float g, float b);

	/**
	 * @brief Sets the current paint to a solid color.
	 *
	 * If not set, the default paint is opaque black color.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param r The red component (0 to 1).
	 * @param g The green component (0 to 1).
	 * @param b The blue component (0 to 1).
	 * @param a The alpha component (0 to 1).
	 */
	PLUTOVG_API void plutovg_canvas_set_rgba(plutovg_canvas_t* canvas, float r, float g, float b, float a);

	/**
	 * @brief Sets the current paint to a solid color.
	 *
	 * If not set, the default paint is opaque black color.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param color A pointer to a `plutovg_color_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_set_color(plutovg_canvas_t* canvas, const plutovg_color_t* color);

	/**
	 * @brief Sets the current paint to a linear gradient.
	 *
	 * If not set, the default paint is opaque black color.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param x1 The x coordinate of the start point.
	 * @param y1 The y coordinate of the start point.
	 * @param x2 The x coordinate of the end point.
	 * @param y2 The y coordinate of the end point.
	 * @param spread The gradient spread method.
	 * @param stops Array of gradient stops.
	 * @param nstops Number of gradient stops.
	 * @param matrix Optional transformation matrix.
	 */
	PLUTOVG_API void plutovg_canvas_set_linear_gradient(plutovg_canvas_t* canvas, float x1, float y1, float x2, float y2,
		plutovg_spread_method_t spread, const plutovg_gradient_stop_t* stops, int nstops, const plutovg_matrix_t* matrix);

	/**
	 * @brief Sets the current paint to a radial gradient.
	 *
	 * If not set, the default paint is opaque black color.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param cx The x coordinate of the center.
	 * @param cy The y coordinate of the center.
	 * @param cr The radius of the gradient.
	 * @param fx The x coordinate of the focal point.
	 * @param fy The y coordinate of the focal point.
	 * @param fr The radius of the focal point.
	 * @param spread The gradient spread method.
	 * @param stops Array of gradient stops.
	 * @param nstops Number of gradient stops.
	 * @param matrix Optional transformation matrix.
	 */
	PLUTOVG_API void plutovg_canvas_set_radial_gradient(plutovg_canvas_t* canvas, float cx, float cy, float cr, float fx, float fy, float fr,
		plutovg_spread_method_t spread, const plutovg_gradient_stop_t* stops, int nstops, const plutovg_matrix_t* matrix);

	/**
	 * @brief Sets the current paint to a texture.
	 *
	 * If not set, the default paint is opaque black color.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param surface The texture surface.
	 * @param type The texture type (plain or tiled).
	 * @param opacity The opacity of the texture (0 to 1).
	 * @param matrix Optional transformation matrix.
	 */
	PLUTOVG_API void plutovg_canvas_set_texture(plutovg_canvas_t* canvas, plutovg_surface_t* surface, plutovg_texture_type_t type, float opacity, const plutovg_matrix_t* matrix);

	/**
	 * @brief Sets the current paint.
	 *
	 * If not set, the default paint is opaque black color.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param paint The paint to be used for subsequent drawing operations.
	 */
	PLUTOVG_API void plutovg_canvas_set_paint(plutovg_canvas_t* canvas, plutovg_paint_t* paint);

	/**
	 * @brief Retrieves the current paint.
	 *
	 * If not set, the default paint is opaque black color.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param color A pointer to a `plutovg_color_t` object where the current color will be stored.
	 * @return The current `plutovg_paint_t` used for drawing operations. If no paint is set, `NULL` is returned.
	 */
	PLUTOVG_API plutovg_paint_t* plutovg_canvas_get_paint(const plutovg_canvas_t* canvas, plutovg_color_t* color);


	/**
	 * @brief Sets the fill rule.
	 *
	 * If not set, the default fill rule is `PLUTOVG_FILL_RULE_NON_ZERO`.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param winding The fill rule.
	 */
	PLUTOVG_API void plutovg_canvas_set_fill_rule(plutovg_canvas_t* canvas, plutovg_fill_rule_t winding);

	/**
	 * @brief Retrieves the current fill rule.
	 *
	 * If not set, the default fill rule is `PLUTOVG_FILL_RULE_NON_ZERO`.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @return The current fill rule.
	 */
	PLUTOVG_API plutovg_fill_rule_t plutovg_canvas_get_fill_rule(const plutovg_canvas_t* canvas);

	/**
	 * @brief Sets the compositing operator.
	 *
	 * If not set, the default compositing operator is `PLUTOVG_OPERATOR_SRC_OVER`.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param op The compositing operator.
	 */
	PLUTOVG_API void plutovg_canvas_set_operator(plutovg_canvas_t* canvas, plutovg_operator_t op);

	/**
	 * @brief Retrieves the current compositing operator.
	 *
	 * If not set, the default compositing operator is `PLUTOVG_OPERATOR_SRC_OVER`.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @return The current compositing operator.
	 */
	PLUTOVG_API plutovg_operator_t plutovg_canvas_get_operator(const plutovg_canvas_t* canvas);

	/**
	 * @brief Sets the global opacity.
	 *
	 * If not set, the default global opacity is 1.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param opacity The opacity value (0 to 1).
	 */
	PLUTOVG_API void plutovg_canvas_set_opacity(plutovg_canvas_t* canvas, float opacity);

	/**
	 * @brief Retrieves the current global opacity.
	 *
	 * If not set, the default global opacity is 1.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @return The current opacity value.
	 */
	PLUTOVG_API float plutovg_canvas_get_opacity(const plutovg_canvas_t* canvas);

	/**
	 * @brief Sets the line width.
	 *
	 * If not set, the default line width is 1.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param line_width The width of the stroke.
	 */
	PLUTOVG_API void plutovg_canvas_set_line_width(plutovg_canvas_t* canvas, float line_width);

	/**
	 * @brief Retrieves the current line width.
	 *
	 * If not set, the default line width is 1.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @return The current line width.
	 */
	PLUTOVG_API float plutovg_canvas_get_line_width(const plutovg_canvas_t* canvas);

	/**
	 * @brief Sets the line cap style.
	 *
	 * If not set, the default line cap is `PLUTOVG_LINE_CAP_BUTT`.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param line_cap The line cap style.
	 */
	PLUTOVG_API void plutovg_canvas_set_line_cap(plutovg_canvas_t* canvas, plutovg_line_cap_t line_cap);

	/**
	 * @brief Retrieves the current line cap style.
	 *
	 * If not set, the default line cap is `PLUTOVG_LINE_CAP_BUTT`.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @return The current line cap style.
	 */
	PLUTOVG_API plutovg_line_cap_t plutovg_canvas_get_line_cap(const plutovg_canvas_t* canvas);

	/**
	 * @brief Sets the line join style.
	 *
	 * If not set, the default line join is `PLUTOVG_LINE_JOIN_MITER`.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param line_join The line join style.
	 */
	PLUTOVG_API void plutovg_canvas_set_line_join(plutovg_canvas_t* canvas, plutovg_line_join_t line_join);

	/**
	 * @brief Retrieves the current line join style.
	 *
	 * If not set, the default line join is `PLUTOVG_LINE_JOIN_MITER`.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @return The current line join style.
	 */
	PLUTOVG_API plutovg_line_join_t plutovg_canvas_get_line_join(const plutovg_canvas_t* canvas);

	/**
	 * @brief Sets the miter limit.
	 *
	 * If not set, the default miter limit is 10.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param miter_limit The miter limit value.
	 */
	PLUTOVG_API void plutovg_canvas_set_miter_limit(plutovg_canvas_t* canvas, float miter_limit);

	/**
	 * @brief Retrieves the current miter limit.
	 *
	 * If not set, the default miter limit is 10.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @return The current miter limit value.
	 */
	PLUTOVG_API float plutovg_canvas_get_miter_limit(const plutovg_canvas_t* canvas);

	/**
	 * @brief Sets the dash pattern.
	 *
	 * If not set, the default dash offset is 0, and the default dash array is `NULL`.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param offset The dash offset.
	 * @param dashes Array of dash lengths.
	 * @param ndashes Number of dash lengths.
	 */
	PLUTOVG_API void plutovg_canvas_set_dash(plutovg_canvas_t* canvas, float offset, const float* dashes, int ndashes);

	/**
	 * @brief Sets the dash offset.
	 *
	 * If not set, the default dash offset is 0.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param offset The dash offset.
	 */
	PLUTOVG_API void plutovg_canvas_set_dash_offset(plutovg_canvas_t* canvas, float offset);

	/**
	 * @brief Retrieves the current dash offset.
	 *
	 * If not set, the default dash offset is 0.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @return The current dash offset.
	 */
	PLUTOVG_API float plutovg_canvas_get_dash_offset(const plutovg_canvas_t* canvas);

	/**
	 * @brief Sets the dash pattern.
	 *
	 * If not set, the default dash array is `NULL`.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param dashes Array of dash lengths.
	 * @param ndashes Number of dash lengths.
	 */
	PLUTOVG_API void plutovg_canvas_set_dash_array(plutovg_canvas_t* canvas, const float* dashes, int ndashes);

	/**
	 * @brief Retrieves the current dash pattern.
	 *
	 * If not set, the default dash array is `NULL`.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param dashes Pointer to store the dash array.
	 * @return The number of dash lengths.
	 */
	PLUTOVG_API int plutovg_canvas_get_dash_array(const plutovg_canvas_t* canvas, const float** dashes);

	/**
	 * @brief Translates the current transformation matrix by offsets `tx` and `ty`.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param tx The translation offset in the x-direction.
	 * @param ty The translation offset in the y-direction.
	 */
	PLUTOVG_API void plutovg_canvas_translate(plutovg_canvas_t* canvas, float tx, float ty);

	/**
	 * @brief Scales the current transformation matrix by factors `sx` and `sy`.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param sx The scaling factor in the x-direction.
	 * @param sy The scaling factor in the y-direction.
	 */
	PLUTOVG_API void plutovg_canvas_scale(plutovg_canvas_t* canvas, float sx, float sy);

	/**
	 * @brief Shears the current transformation matrix by factors `shx` and `shy`.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param shx The shearing factor in the x-direction.
	 * @param shy The shearing factor in the y-direction.
	 */
	PLUTOVG_API void plutovg_canvas_shear(plutovg_canvas_t* canvas, float shx, float shy);

	/**
	 * @brief Rotates the current transformation matrix by the specified angle (in radians).
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param angle The rotation angle in radians.
	 */
	PLUTOVG_API void plutovg_canvas_rotate(plutovg_canvas_t* canvas, float angle);

	/**
	 * @brief Multiplies the current transformation matrix with the specified `matrix`.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param matrix A pointer to the `plutovg_matrix_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_transform(plutovg_canvas_t* canvas, const plutovg_matrix_t* matrix);

	/**
	 * @brief Resets the current transformation matrix to the identity matrix.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_reset_matrix(plutovg_canvas_t* canvas);

	/**
	 * @brief Resets the current transformation matrix to the specified `matrix`.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param matrix A pointer to the `plutovg_matrix_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_set_matrix(plutovg_canvas_t* canvas, const plutovg_matrix_t* matrix);

	/**
	 * @brief Stores the current transformation matrix in `matrix`.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param A pointer to the `plutovg_matrix_t` to store the matrix.
	 */
	PLUTOVG_API void plutovg_canvas_get_matrix(const plutovg_canvas_t* canvas, plutovg_matrix_t* matrix);

	/**
	 * @brief Transforms the point `(x, y)` using the current transformation matrix and stores the result in `(xx, yy)`.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param x The x-coordinate of the point to transform.
	 * @param y The y-coordinate of the point to transform.
	 * @param xx A pointer to store the transformed x-coordinate.
	 * @param yy A pointer to store the transformed y-coordinate.
	 */
	PLUTOVG_API void plutovg_canvas_map(const plutovg_canvas_t* canvas, float x, float y, float* xx, float* yy);

	/**
	 * @brief Transforms the `src` point using the current transformation matrix and stores the result in `dst`.
	 * @note `src` and `dst` can be identical.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param src A pointer to the `plutovg_point_t` point to transform.
	 * @param dst A pointer to the `plutovg_point_t` to store the transformed point.
	 */
	PLUTOVG_API void plutovg_canvas_map_point(const plutovg_canvas_t* canvas, const plutovg_point_t* src, plutovg_point_t* dst);

	/**
	 * @brief Transforms the `src` rectangle using the current transformation matrix and stores the result in `dst`.
	 * @note `src` and `dst` can be identical.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param src A pointer to the `plutovg_rect_t` rectangle to transform.
	 * @param dst A pointer to the `plutovg_rect_t` to store the transformed rectangle.
	 */
	PLUTOVG_API void plutovg_canvas_map_rect(const plutovg_canvas_t* canvas, const plutovg_rect_t* src, plutovg_rect_t* dst);

	/**
	 * @brief Moves the current point to a new position.
	 *
	 * Moves the current point to the specified coordinates without adding a line.
	 * This operation is added to the current path. Equivalent to the SVG `M` command.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param x The x-coordinate of the new position.
	 * @param y The y-coordinate of the new position.
	 */
	PLUTOVG_API void plutovg_canvas_move_to(plutovg_canvas_t* canvas, float x, float y);

	/**
	 * @brief Adds a straight line segment to the current path.
	 *
	 * Adds a straight line from the current point to the specified coordinates.
	 * This segment is added to the current path. Equivalent to the SVG `L` command.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param x The x-coordinate of the end point of the line.
	 * @param y The y-coordinate of the end point of the line.
	 */
	PLUTOVG_API void plutovg_canvas_line_to(plutovg_canvas_t* canvas, float x, float y);

	/**
	 * @brief Adds a quadratic Bézier curve to the current path.
	 *
	 * Adds a quadratic Bézier curve from the current point to the specified end point,
	 * using the given control point. This curve is added to the current path. Equivalent to the SVG `Q` command.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param x1 The x-coordinate of the control point.
	 * @param y1 The y-coordinate of the control point.
	 * @param x2 The x-coordinate of the end point of the curve.
	 * @param y2 The y-coordinate of the end point of the curve.
	 */
	PLUTOVG_API void plutovg_canvas_quad_to(plutovg_canvas_t* canvas, float x1, float y1, float x2, float y2);

	/**
	 * @brief Adds a cubic Bézier curve to the current path.
	 *
	 * Adds a cubic Bézier curve from the current point to the specified end point,
	 * using the given control points. This curve is added to the current path. Equivalent to the SVG `C` command.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param x1 The x-coordinate of the first control point.
	 * @param y1 The y-coordinate of the first control point.
	 * @param x2 The x-coordinate of the second control point.
	 * @param y2 The y-coordinate of the second control point.
	 * @param x3 The x-coordinate of the end point of the curve.
	 * @param y3 The y-coordinate of the end point of the curve.
	 */
	PLUTOVG_API void plutovg_canvas_cubic_to(plutovg_canvas_t* canvas, float x1, float y1, float x2, float y2, float x3, float y3);

	/**
	 * @brief Adds an elliptical arc to the current path.
	 *
	 * Adds an elliptical arc from the current point to the specified end point,
	 * defined by radii, rotation angle, and flags for arc type and direction.
	 * This arc segment is added to the current path. Equivalent to the SVG `A` command.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param rx The x-radius of the ellipse.
	 * @param ry The y-radius of the ellipse.
	 * @param angle The rotation angle of the ellipse in degrees.
	 * @param large_arc_flag If true, add the large arc; otherwise, add the small arc.
	 * @param sweep_flag If true, add the arc in the positive-angle direction; otherwise, in the negative-angle direction.
	 * @param x The x-coordinate of the end point.
	 * @param y The y-coordinate of the end point.
	 */
	PLUTOVG_API void plutovg_canvas_arc_to(plutovg_canvas_t* canvas, float rx, float ry, float angle, bool large_arc_flag, bool sweep_flag, float x, float y);

	/**
	 * @brief Adds a rectangle to the current path.
	 *
	 * Adds a rectangle with the specified position and dimensions to the current path.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param x The x-coordinate of the rectangle's origin.
	 * @param y The y-coordinate of the rectangle's origin.
	 * @param w The width of the rectangle.
	 * @param h The height of the rectangle.
	 */
	PLUTOVG_API void plutovg_canvas_rect(plutovg_canvas_t* canvas, float x, float y, float w, float h);

	/**
	 * @brief Adds a rounded rectangle to the current path.
	 *
	 * Adds a rectangle with rounded corners defined by the specified position,
	 * dimensions, and corner radii to the current path.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param x The x-coordinate of the rectangle's origin.
	 * @param y The y-coordinate of the rectangle's origin.
	 * @param w The width of the rectangle.
	 * @param h The height of the rectangle.
	 * @param rx The x-radius of the corners.
	 * @param ry The y-radius of the corners.
	 */
	PLUTOVG_API void plutovg_canvas_round_rect(plutovg_canvas_t* canvas, float x, float y, float w, float h, float rx, float ry);

	/**
	 * @brief Adds an ellipse to the current path.
	 *
	 * Adds an ellipse centered at the specified coordinates with the given radii to the current path.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param cx The x-coordinate of the ellipse's center.
	 * @param cy The y-coordinate of the ellipse's center.
	 * @param rx The x-radius of the ellipse.
	 * @param ry The y-radius of the ellipse.
	 */
	PLUTOVG_API void plutovg_canvas_ellipse(plutovg_canvas_t* canvas, float cx, float cy, float rx, float ry);

	/**
	 * @brief Adds a circle to the current path.
	 *
	 * Adds a circle centered at the specified coordinates with the given radius to the current path.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param cx The x-coordinate of the circle's center.
	 * @param cy The y-coordinate of the circle's center.
	 * @param r The radius of the circle.
	 */
	PLUTOVG_API void plutovg_canvas_circle(plutovg_canvas_t* canvas, float cx, float cy, float r);

	/**
	 * @brief Adds an arc to the current path.
	 *
	 * Adds an arc centered at the specified coordinates, with a given radius,
	 * starting and ending at the specified angles. The direction of the arc is
	 * determined by `ccw`. This arc segment is added to the current path.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param cx The x-coordinate of the arc's center.
	 * @param cy The y-coordinate of the arc's center.
	 * @param r The radius of the arc.
	 * @param a0 The starting angle of the arc in radians.
	 * @param a1 The ending angle of the arc in radians.
	 * @param ccw If true, add the arc counter-clockwise; otherwise, clockwise.
	 */
	PLUTOVG_API void plutovg_canvas_arc(plutovg_canvas_t* canvas, float cx, float cy, float r, float a0, float a1, bool ccw);

	/**
	 * @brief Adds a path to the current path.
	 *
	 * Appends the elements of the specified path to the current path.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param path A pointer to the `plutovg_path_t` object to be added.
	 */
	PLUTOVG_API void plutovg_canvas_add_path(plutovg_canvas_t* canvas, const plutovg_path_t* path);

	/**
	 * @brief Starts a new path on the canvas.
	 *
	 * Begins a new path, clearing any existing path data. The new path starts with no commands.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_new_path(plutovg_canvas_t* canvas);

	/**
	 * @brief Closes the current path.
	 *
	 * Closes the current path by adding a straight line back to the starting point.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_close_path(plutovg_canvas_t* canvas);

	/**
	 * @brief Retrieves the current point of the canvas.
	 *
	 * Gets the coordinates of the current point in the canvas, which is the last point
	 * added or moved to in the current path.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param x The x-coordinate of the current point.
	 * @param y The y-coordinate of the current point.
	 */
	PLUTOVG_API void plutovg_canvas_get_current_point(const plutovg_canvas_t* canvas, float* x, float* y);

	/**
	 * @brief Gets the current path from the canvas.
	 *
	 * Retrieves the path object representing the sequence of path commands added to the canvas.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @return The current path.
	 */
	PLUTOVG_API plutovg_path_t* plutovg_canvas_get_path(const plutovg_canvas_t* canvas);

	/**
	 * @brief Gets the bounding box of the filled region.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param extents The bounding box of the filled region.
	 */
	PLUTOVG_API void plutovg_canvas_fill_extents(const plutovg_canvas_t* canvas, plutovg_rect_t* extents);

	/**
	 * @brief Gets the bounding box of the stroked region.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param extents The bounding box of the stroked region.
	 */
	PLUTOVG_API void plutovg_canvas_stroke_extents(const plutovg_canvas_t* canvas, plutovg_rect_t* extents);

	/**
	 * @brief Gets the bounding box of the clipped region.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param extents The bounding box of the clipped region.
	 */
	PLUTOVG_API void plutovg_canvas_clip_extents(const plutovg_canvas_t* canvas, plutovg_rect_t* extents);

	/**
	 * @brief A drawing operator that fills the current path according to the current fill rule.
	 *
	 * The current path will be cleared after this operation.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_fill(plutovg_canvas_t* canvas);

	/**
	 * @brief A drawing operator that strokes the current path according to the current stroke settings.
	 *
	 * The current path will be cleared after this operation.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_stroke(plutovg_canvas_t* canvas);

	/**
	 * @brief A drawing operator that intersects the current clipping region with the current path according to the current fill rule.
	 *
	 * The current path will be cleared after this operation.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_clip(plutovg_canvas_t* canvas);

	/**
	 * @brief A drawing operator that paints the current clipping region using the current paint.
	 *
	 * @note The current path will not be affected by this operation.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_paint(plutovg_canvas_t* canvas);

	/**
	 * @brief A drawing operator that fills the current path according to the current fill rule.
	 *
	 * The current path will be preserved after this operation.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_fill_preserve(plutovg_canvas_t* canvas);

	/**
	 * @brief A drawing operator that strokes the current path according to the current stroke settings.
	 *
	 * The current path will be preserved after this operation.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_stroke_preserve(plutovg_canvas_t* canvas);

	/**
	 * @brief A drawing operator that intersects the current clipping region with the current path according to the current fill rule.
	 *
	 * The current path will be preserved after this operation.
	 *
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_clip_preserve(plutovg_canvas_t* canvas);

	/**
	 * @brief Fills a rectangle according to the current fill rule.
	 *
	 * @note The current path will be cleared by this operation.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param x The x-coordinate of the rectangle's origin.
	 * @param y The y-coordinate of the rectangle's origin.
	 * @param w The width of the rectangle.
	 * @param h The height of the rectangle.
	 */
	PLUTOVG_API void plutovg_canvas_fill_rect(plutovg_canvas_t* canvas, float x, float y, float w, float h);

	/**
	 * @brief Fills a path according to the current fill rule.
	 *
	 * @note The current path will be cleared by this operation.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param path The `plutovg_path_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_fill_path(plutovg_canvas_t* canvas, const plutovg_path_t* path);

	/**
	 * @brief Strokes a rectangle with the current stroke settings.
	 *
	 * @note The current path will be cleared by this operation.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param x The x-coordinate of the rectangle's origin.
	 * @param y The y-coordinate of the rectangle's origin.
	 * @param w The width of the rectangle.
	 * @param h The height of the rectangle.
	 */
	PLUTOVG_API void plutovg_canvas_stroke_rect(plutovg_canvas_t* canvas, float x, float y, float w, float h);

	/**
	 * @brief Strokes a path with the current stroke settings.
	 *
	 * @note The current path will be cleared by this operation.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param path The `plutovg_path_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_stroke_path(plutovg_canvas_t* canvas, const plutovg_path_t* path);

	/**
	 * @brief Intersects the current clipping region with a rectangle according to the current fill rule.
	 *
	 * @note The current path will be cleared by this operation.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param x The x-coordinate of the rectangle's origin.
	 * @param y The y-coordinate of the rectangle's origin.
	 * @param w The width of the rectangle.
	 * @param h The height of the rectangle.
	 */
	PLUTOVG_API void plutovg_canvas_clip_rect(plutovg_canvas_t* canvas, float x, float y, float w, float h);

	/**
	 * @brief Intersects the current clipping region with a path according to the current fill rule.
	 *
	 * @note The current path will be cleared by this operation.
	 * @param canvas A pointer to a `plutovg_canvas_t` object.
	 * @param path The `plutovg_path_t` object.
	 */
	PLUTOVG_API void plutovg_canvas_clip_path(plutovg_canvas_t* canvas, const plutovg_path_t* path);





#define PLUTOVG_IS_NUM(c) ((c) >= '0' && (c) <= '9')
#define PLUTOVG_IS_ALPHA(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define PLUTOVG_IS_ALNUM(c) (PLUTOVG_IS_ALPHA(c) || PLUTOVG_IS_NUM(c))
#define PLUTOVG_IS_WS(c) ((c) == ' ' || (c) == '\t' || (c) == '\n' || (c) == '\r')

#define plutovg_min(a, b) ((a) < (b) ? (a) : (b))
#define plutovg_max(a, b) ((a) > (b) ? (a) : (b))
#define plutovg_clamp(v, lo, hi) ((v) < (lo) ? (lo) : (hi) < (v) ? (hi) : (v))

#define plutovg_alpha(c) (((c) >> 24) & 0xff)
#define plutovg_red(c) (((c) >> 16) & 0xff)
#define plutovg_green(c) (((c) >> 8) & 0xff)
#define plutovg_blue(c) (((c) >> 0) & 0xff)

#define plutovg_array_init(array) \
    do { \
        (array).data = NULL; \
        (array).size = 0; \
        (array).capacity = 0; \
    } while(0)

#define plutovg_array_ensure(array, count) \
    do { \
        if((array).size + (count) > (array).capacity) { \
            int capacity = (array).size + (count); \
            int newcapacity = (array).capacity == 0 ? 8 : (array).capacity; \
            while(newcapacity < capacity) { newcapacity *= 2; } \
            (array).data = (decltype((array).data))realloc((array).data, newcapacity * sizeof((array).data[0])); \
            (array).capacity = newcapacity; \
        } \
    } while(0)

#define plutovg_array_append_data(array, newdata, count) \
    do { \
        if(newdata && count > 0) { \
            plutovg_array_ensure(array, count); \
            memcpy((array).data + (array).size, newdata, (count) * sizeof((newdata)[0])); \
            (array).size += count; \
        } \
    } while(0)

#define plutovg_array_append(array, other) plutovg_array_append_data(array, (other).data, (other).size)
#define plutovg_array_clear(array) ((array).size = 0)
#define plutovg_array_destroy(array) free((array).data)

	static inline uint32_t plutovg_premultiply_argb(uint32_t color)
	{
		uint32_t a = plutovg_alpha(color);
		uint32_t r = plutovg_red(color);
		uint32_t g = plutovg_green(color);
		uint32_t b = plutovg_blue(color);
		if (a != 255) {
			r = (r * a) / 255;
			g = (g * a) / 255;
			b = (b * a) / 255;
		}

		return (a << 24) | (r << 16) | (g << 8) | (b);
	}

	static inline bool plutovg_parse_number(const char** begin, const char* end, float* number)
	{
		const char* it = *begin;
		float integer = 0;
		float fraction = 0;
		float exponent = 0;
		int sign = 1;
		int expsign = 1;

		if (it < end && *it == '+') {
			++it;
		}
		else if (it < end && *it == '-') {
			++it;
			sign = -1;
		}

		if (it >= end || (*it != '.' && !PLUTOVG_IS_NUM(*it)))
			return false;
		if (PLUTOVG_IS_NUM(*it)) {
			do {
				integer = 10.f * integer + (*it++ - '0');
			} while (it < end && PLUTOVG_IS_NUM(*it));
		}

		if (it < end && *it == '.') {
			++it;
			if (it >= end || !PLUTOVG_IS_NUM(*it))
				return false;
			float divisor = 1.f;
			do {
				fraction = 10.f * fraction + (*it++ - '0');
				divisor *= 10.f;
			} while (it < end && PLUTOVG_IS_NUM(*it));
			fraction /= divisor;
		}

		if (it < end && (*it == 'e' || *it == 'E')) {
			++it;
			if (it < end && *it == '+') {
				++it;
			}
			else if (it < end && *it == '-') {
				++it;
				expsign = -1;
			}

			if (it >= end || !PLUTOVG_IS_NUM(*it))
				return false;
			do {
				exponent = 10 * exponent + (*it++ - '0');
			} while (it < end && PLUTOVG_IS_NUM(*it));
		}

		*begin = it;
		*number = sign * (integer + fraction);
		if (exponent)
			*number *= powf(10.f, expsign * exponent);
		return *number >= -FLT_MAX && *number <= FLT_MAX;
	}

	static inline bool plutovg_skip_delim(const char** begin, const char* end, const char delim)
	{
		const char* it = *begin;
		if (it < end && *it == delim) {
			*begin = it + 1;
			return true;
		}

		return false;
	}

	static inline bool plutovg_skip_string(const char** begin, const char* end, const char* data)
	{
		const char* it = *begin;
		while (it < end && *data && *it == *data) {
			++data;
			++it;
		}

		if (*data == '\0') {
			*begin = it;
			return true;
		}

		return false;
	}

	static inline bool plutovg_skip_ws(const char** begin, const char* end)
	{
		const char* it = *begin;
		while (it < end && PLUTOVG_IS_WS(*it))
			++it;
		*begin = it;
		return it < end;
	}

	static inline bool plutovg_skip_ws_and_delim(const char** begin, const char* end, char delim)
	{
		const char* it = *begin;
		if (plutovg_skip_ws(&it, end)) {
			if (!plutovg_skip_delim(&it, end, delim))
				return false;
			plutovg_skip_ws(&it, end);
		}
		else {
			return false;
		}

		*begin = it;
		return it < end;
	}

	static inline bool plutovg_skip_ws_and_comma(const char** begin, const char* end)
	{
		return plutovg_skip_ws_and_delim(begin, end, ',');
	}

	static inline bool plutovg_skip_ws_or_delim(const char** begin, const char* end, char delim, bool* has_delim)
	{
		const char* it = *begin;
		if (has_delim)
			*has_delim = false;
		if (plutovg_skip_ws(&it, end)) {
			if (plutovg_skip_delim(&it, end, delim)) {
				if (has_delim)
					*has_delim = true;
				plutovg_skip_ws(&it, end);
			}
		}

		if (it == *begin)
			return false;
		*begin = it;
		return it < end;
	}

	static inline bool plutovg_skip_ws_or_comma(const char** begin, const char* end, bool* has_comma)
	{
		return plutovg_skip_ws_or_delim(begin, end, ',', has_comma);
	}


	struct plutovg_surface {
		int ref_count;
		int width;
		int height;
		int stride;
		unsigned char* data;
	};

	struct plutovg_path {
		int ref_count;
		int num_points;
		int num_contours;
		int num_curves;
		plutovg_point_t start_point;
		struct {
			plutovg_path_element_t* data;
			int size;
			int capacity;
		} elements;
	};

	typedef enum {
		PLUTOVG_PAINT_TYPE_COLOR,
		PLUTOVG_PAINT_TYPE_GRADIENT,
		PLUTOVG_PAINT_TYPE_TEXTURE
	} plutovg_paint_type_t;

	struct plutovg_paint {
		int ref_count;
		plutovg_paint_type_t type;
	};

	typedef struct {
		plutovg_paint_t base;
		plutovg_color_t color;
	} plutovg_solid_paint_t;

	typedef enum {
		PLUTOVG_GRADIENT_TYPE_LINEAR,
		PLUTOVG_GRADIENT_TYPE_RADIAL
	} plutovg_gradient_type_t;

	typedef struct {
		plutovg_paint_t base;
		plutovg_gradient_type_t type;
		plutovg_spread_method_t spread;
		plutovg_matrix_t matrix;
		plutovg_gradient_stop_t* stops;
		int nstops;
		float values[6];
	} plutovg_gradient_paint_t;

	typedef struct {
		plutovg_paint_t base;
		plutovg_texture_type_t type;
		float opacity;
		plutovg_matrix_t matrix;
		plutovg_surface_t* surface;
	} plutovg_texture_paint_t;

	typedef struct {
		int x;
		int len;
		int y;
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
	} plutovg_span_buffer_t;

	typedef struct {
		float offset;
		struct {
			float* data;
			int size;
			int capacity;
		} array;
	} plutovg_stroke_dash_t;

	typedef struct {
		float width;
		plutovg_line_cap_t cap;
		plutovg_line_join_t join;
		float miter_limit;
	} plutovg_stroke_style_t;

	typedef struct {
		plutovg_stroke_style_t style;
		plutovg_stroke_dash_t dash;
	} plutovg_stroke_data_t;

	void plutovg_span_buffer_init(plutovg_span_buffer_t* span_buffer);
	void plutovg_span_buffer_init_rect(plutovg_span_buffer_t* span_buffer, int x, int y, int width, int height);
	void plutovg_span_buffer_reset(plutovg_span_buffer_t* span_buffer);
	void plutovg_span_buffer_destroy(plutovg_span_buffer_t* span_buffer);
	void plutovg_span_buffer_copy(plutovg_span_buffer_t* span_buffer, const plutovg_span_buffer_t* source);
	void plutovg_span_buffer_extents(plutovg_span_buffer_t* span_buffer, plutovg_rect_t* extents);
	void plutovg_span_buffer_intersect(plutovg_span_buffer_t* span_buffer, const plutovg_span_buffer_t* a, const plutovg_span_buffer_t* b);

	void plutovg_rasterize(plutovg_span_buffer_t* span_buffer, const plutovg_path_t* path, const plutovg_matrix_t* matrix, const plutovg_rect_t* clip_rect, const plutovg_stroke_data_t* stroke_data, plutovg_fill_rule_t winding);
	void plutovg_blend(plutovg_canvas_t* canvas, const plutovg_span_buffer_t* span_buffer);
	void plutovg_memfill32(unsigned int* dest, int length, unsigned int value);


#define PLUTOSVG_API 

#define PLUTOSVG_VERSION_MAJOR 0
#define PLUTOSVG_VERSION_MINOR 0
#define PLUTOSVG_VERSION_MICRO 7

#define PLUTOSVG_VERSION PLUTOVG_VERSION_ENCODE(PLUTOSVG_VERSION_MAJOR, PLUTOSVG_VERSION_MINOR, PLUTOSVG_VERSION_MICRO)
#define PLUTOSVG_VERSION_STRING PLUTOVG_VERSION_STRINGIZE(PLUTOSVG_VERSION_MAJOR, PLUTOSVG_VERSION_MINOR, PLUTOSVG_VERSION_MICRO)


	/**
	 * @brief Returns the version number of PlutoSVG.
	 *
	 * @return The version number as an integer.
	 */
	PLUTOSVG_API int plutosvg_version(void);

	/**
	 * @brief Returns the version string of PlutoSVG.
	 *
	 * @return The version number as a null-terminated string.
	 */
	PLUTOSVG_API const char* plutosvg_version_string(void);

	/**
	 * @brief Represents an abstract SVG document handle.
	 */
	typedef struct plutosvg_document plutosvg_document_t;

	/**
	 * @brief Callback type for resolving CSS color variables in SVG documents.
	 *
	 * @param closure User-defined data passed to the callback.
	 * @param name Name of the color variable.
	 * @param length Length of the color variable name.
	 * @param color Pointer to a `plutovg_color_t` object where the resolved color will be stored.
	 * @return `true` if the color variable was successfully resolved; `false` otherwise.
	 */
	typedef bool (*plutosvg_palette_func_t)(void* closure, const char* name, int length, plutovg_color_t* color);

	/**
	 * @brief Loads an SVG document from a data buffer.
	 *
	 * @note The buffer pointed to by `data` must remain valid until the returned `plutosvg_document_t` object is destroyed.
	 *
	 * @param data Pointer to the SVG data buffer.
	 * @param length Length of the data buffer.
	 * @param width Container width used to resolve the intrinsic width, or `-1` if unspecified.
	 * @param height Container height used to resolve the intrinsic height, or `-1` if unspecified.
	 * @param destroy_func Custom function called when the document is destroyed.
	 * @param closure User-defined data passed to the `destroy_func` callback.
	 * @return Pointer to the loaded `plutosvg_document_t` object, or `NULL` if loading fails.
	 */
	PLUTOSVG_API plutosvg_document_t* plutosvg_document_load_from_data(const char* data, int length, float width, float height,
		plutovg_destroy_func_t destroy_func, void* closure);

	/**
	 * @brief Loads an SVG document from a file.
	 *
	 * @param filename Path to the SVG file.
	 * @param width Container width used to resolve the intrinsic width, or `-1` if unspecified.
	 * @param height Container height used to resolve the intrinsic height, or `-1` if unspecified.
	 * @return Pointer to the loaded `plutosvg_document_t` object, or `NULL` if loading fails.
	 */
	PLUTOSVG_API plutosvg_document_t* plutosvg_document_load_from_file(const char* filename, float width, float height);


	/**
	 * @brief Renders an SVG document or a specific element to a surface.
	 *
	 * @param document Pointer to the SVG document.
	 * @param id ID of the SVG element to render, or `NULL` to render the entire document.
	 * @param width Expected width of the surface, or `-1` if unspecified.
	 * @param height Expected height of the surface, or `-1` if unspecified.
	 * @param current_color Color used to resolve CSS `currentColor` values.
	 * @param palette_func Callback function for resolving CSS color variables.
	 * @param closure User-defined data passed to the `palette_func` callback.
	 * @return Pointer to the rendered `plutovg_surface_t` object, or `NULL` if rendering fails.
	 */
	PLUTOSVG_API plutovg_surface_t* plutosvg_document_render_to_surface(const plutosvg_document_t* document, const char* id, int width, int height,
		const plutovg_color_t* current_color, plutosvg_palette_func_t palette_func, void* closure);

	/**
	 * @brief Returns the intrinsic width of the SVG document.
	 *
	 * @param document Pointer to the SVG document.
	 * @return The intrinsic width of the SVG document.
	 */
	PLUTOSVG_API float plutosvg_document_get_width(const plutosvg_document_t* document);

	/**
	 * @brief Returns the intrinsic height of the SVG document.
	 *
	 * @param document Pointer to the SVG document.
	 * @return The intrinsic height of the SVG document.
	 */
	PLUTOSVG_API float plutosvg_document_get_height(const plutosvg_document_t* document);

	/**
	 * @brief Retrieves the bounding box of a specific element or the entire SVG document.
	 *
	 * Calculates and retrieves the extents of an element identified by `id`, or the whole document if `id` is `NULL`.
	 *
	 * @param document Pointer to the SVG document.
	 * @param id ID of the element whose extents to retrieve, or `NULL` to retrieve the extents of the entire document.
	 * @param extents Pointer to a `plutovg_rect_t` object where the extents will be stored.
	 * @return `true` if the extents were successfully retrieved; `false` otherwise.
	 */
	PLUTOSVG_API bool plutosvg_document_extents(const plutosvg_document_t* document, const char* id, plutovg_rect_t* extents);

	/**
	 * @brief Destroys an SVG document and frees its resources.
	 *
	 * @param document Pointer to a `plutosvg_document_t` object to be destroyed. If `NULL`, the function does nothing.
	 */
	PLUTOSVG_API void plutosvg_document_destroy(plutosvg_document_t* document);

};

