
// ========== RFC Generator v1.0 - 2025-08-24 18:15 PM ==========

#ifndef _RFC_H_
#define _RFC_H_ 

#define AMALGAMATED_VERSION


// =========== plutosvg.h ===========

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
		PLUTOVG_PATH_COMMAND_CUBIC_TO, ///< Draws a cubic B�zier curve to a new point.
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
	 * @brief Adds a quadratic B�zier curve to the path.
	 *
	 * This function adds a quadratic B�zier curve segment from the current point
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
	 * @brief Adds a cubic B�zier curve to the path.
	 *
	 * This function adds a cubic B�zier curve segment from the current point
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
	 * @brief Traverses the path with B�zier curves flattened to line segments.
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
	 * @brief Creates a copy of the path with B�zier curves flattened to line segments.
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
	 * @brief Adds a quadratic B�zier curve to the current path.
	 *
	 * Adds a quadratic B�zier curve from the current point to the specified end point,
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
	 * @brief Adds a cubic B�zier curve to the current path.
	 *
	 * Adds a cubic B�zier curve from the current point to the specified end point,
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


// =========== KModuleManager.h ===========

/*
    Copyright (C) 2013-2025 CrownSoft

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

typedef bool (*RFCModuleInitFunc)();
typedef void (*RFCModuleFreeFunc)();

#define MAX_RFC_MODULE_COUNT 4

class KModuleManager {
public:
	static bool registerRFCModule(int index, RFCModuleInitFunc initFunc, RFCModuleFreeFunc freeFunc);
	static RFCModuleInitFunc* rfcModuleInitFuncList();
	static RFCModuleFreeFunc* rfcModuleFreeFuncList();
};

#define REGISTER_RFC_MODULE(index, ModuleObjectType) \
static bool ModuleObjectType##_Registered = KModuleManager::registerRFCModule( index , ModuleObjectType::rfcModuleInit, ModuleObjectType::rfcModuleFree);

// =========== KLeakDetector.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <windows.h>
#include <string.h>  
#include <stdlib.h>  

/**
	Embedding an instance of this class inside another class can be used as a
	low-overhead way of detecting leaked instances.

	This class keeps an internal static count of the number of instances that are
	active, so that when the app is shutdown and the static destructors are called,
	it can check whether there are any left-over instances that may have been leaked.

	To use it, simply declare RFC_LEAK_DETECTOR(YourClassName) inside a private section
	of the class declaration.

	@code
	class MyClass
	{
	public:
		MyClass();
		void method1();

	private:
		RFC_LEAK_DETECTOR (MyClass)
	};
	@endcode
*/
template <class T>
class KLeakDetector
{
public:
	KLeakDetector()
	{
		::InterlockedIncrement(&getCounter().numObjects);
	}

	KLeakDetector(const KLeakDetector&)
	{
		::InterlockedIncrement(&getCounter().numObjects);
	}

	~KLeakDetector()
	{
		::InterlockedDecrement(&getCounter().numObjects);
	}

private:
	class LeakCounter
	{
	public:
		LeakCounter()
		{
			numObjects = 0;
		}

		~LeakCounter()
		{
			if (numObjects > 0) // If you hit this, then you've leaked one or more objects of the type specified by the template parameter
			{
				char textBuffer[128];
				::strcpy_s(textBuffer, "Leaked objects detected : ");

				char intBuffer[16];
				::_itoa_s((int)numObjects, intBuffer, 10);

				::strcat_s(textBuffer, intBuffer);
				::strcat_s(textBuffer, " instance(s) of class ");
				::strcat_s(textBuffer, getLeakedClassName());

				::MessageBoxA(0, textBuffer, "Warning", MB_ICONWARNING);
			}
		}

		volatile long numObjects;
	};

	static const char* getLeakedClassName()
	{
		return T::rfc_GetLeakedClassName();
	}

	static LeakCounter& getCounter()
	{
		static LeakCounter counter;
		return counter;
	}
};

#ifdef _DEBUG
	#define RFC_LEAK_DETECTOR(Class) \
			friend class KLeakDetector<Class>; \
			KLeakDetector<Class> rfc_leakDetector; \
			static const char* rfc_GetLeakedClassName() { return #Class; }
#else 
	#define RFC_LEAK_DETECTOR(Class)
#endif


// =========== KDPIUtility.h ===========

/*
    Copyright (C) 2013-2025 CrownSoft

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <windows.h>
#include <shellscalingapi.h>

typedef HRESULT(WINAPI* KGetDpiForMonitor)(HMONITOR hmonitor, int dpiType, UINT* dpiX, UINT* dpiY);
typedef BOOL(WINAPI* KSetProcessDpiAwarenessContext)(DPI_AWARENESS_CONTEXT value);
typedef HRESULT(STDAPICALLTYPE* KSetProcessDpiAwareness)(PROCESS_DPI_AWARENESS value);
typedef BOOL (WINAPI* KSetProcessDPIAware)(VOID);
typedef DPI_AWARENESS_CONTEXT (WINAPI* KSetThreadDpiAwarenessContext) (DPI_AWARENESS_CONTEXT dpiContext);
typedef BOOL(WINAPI* KAdjustWindowRectExForDpi)(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);


/*
MIXEDMODE_ONLY:	on win10 - all windows are scaled according to the dpi and the mixed mode windows are scaled by the system. 
OS lower than win10 - all windows are scaled by the system.
STANDARD_MODE:	all windows are scaled according to the dpi. cannot have mixed mode windows.
UNAWARE_MODE:	all windows are scaled by the system.
*/

enum class KDPIAwareness
{
	MIXEDMODE_ONLY, // win10 only, app is not dpi aware on other os
	STANDARD_MODE, // win7 or higher
	UNAWARE_MODE
};

class KDPIUtility
{
private: 
    static float getMonitorScalingRatio(HMONITOR monitor);
public:		
	static KGetDpiForMonitor pGetDpiForMonitor;
	static KSetProcessDpiAwarenessContext pSetProcessDpiAwarenessContext;
	static KSetProcessDpiAwareness pSetProcessDpiAwareness;
	static KSetProcessDPIAware pSetProcessDPIAware;
	static KSetThreadDpiAwarenessContext pSetThreadDpiAwarenessContext;
    static KAdjustWindowRectExForDpi pAdjustWindowRectExForDpi;

	static void initDPIFunctions();

    // returns dpi of monitor which our window is in. returns 96 if application is not dpi aware.
	static WORD getWindowDPI(HWND hWnd);

    // automatically fall back to AdjustWindowRectEx when lower than win10
    static BOOL adjustWindowRectExForDpi(LPRECT lpRect, DWORD dwStyle, BOOL bMenu, DWORD dwExStyle, UINT dpi);

	static void makeProcessDPIAware(KDPIAwareness dpiAwareness);

    // gives real value regardless of the process dpi awareness state.
    // if the process is dpi unaware, os will always give 96dpi.
    // so, this method will return correct scale value.
    // it can be used with dpi unaware apps to get the scale of a monitor.
    // https://stackoverflow.com/questions/70976583/get-real-screen-resolution-using-win32-api
    /*
        Example:
        float monitorScale = 1.0f;
     	HMONITOR hmon = ::MonitorFromWindow(compHWND, MONITOR_DEFAULTTONEAREST);
		if (hmon != NULL)
			monitorScale = KDPIUtility::getScaleForMonitor(hmon);
    */
    static float getScaleForMonitor(HMONITOR monitor);

    // scale given 96dpi value according to window current dpi.
    static int scaleToWindowDPI(int valueFor96DPI, HWND window);

    // scale given 96dpi value according to new dpi.
    static int scaleToNewDPI(int valueFor96DPI, int newDPI);
};


// =========== Architecture.h ===========

/*
    Copyright (C) 2013-2025 CrownSoft

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#define RFC_PTR_SIZE sizeof(void*) 

#ifdef _WIN64
	#define RFC64
	#define RFC_NATIVE_INT __int64
#else
	#define RFC32
	#define RFC_NATIVE_INT int
#endif


// =========== KStaticAllocator.h ===========

/*
    Copyright (C) 2013-2025 CrownSoft

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <cstddef>
#include <new>
#include <atomic>

#ifndef KSTATIC_POOL_SIZE
    #define KSTATIC_POOL_SIZE 520
#endif

// thread-safe static allocation. (Lock-free)
class KStaticAllocator
{
private:
    static constexpr size_t POOL_SIZE = KSTATIC_POOL_SIZE; // 1MB pool
    static char memory_pool[POOL_SIZE];
    static std::atomic<size_t> current_offset;

public:
    // once allocated, returned buffer will stay until the application exit.
    // returns nullptr if KSTATIC_POOL_SIZE is not enough.
    static void* allocate(size_t size, size_t alignment = alignof(std::max_align_t));
    
    static void reset();
};


// =========== KScopedStructPointer.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <malloc.h>
#include <Objbase.h>

template<class StructType>
class KReleaseUsingFree
{
public:
	static void release(StructType* structPtr)
	{
		::free(structPtr);
	}
};

template<class StructType>
class KReleaseUsingTaskMemFree
{
public:
	static void release(StructType* memory)
	{
		::CoTaskMemFree(memory);
	}
};

/**
	This class holds a pointer to the struct which is automatically freed when this object goes
	out of scope. 
*/
template<class StructType, class ReleaseMethod = KReleaseUsingFree<StructType>>
class KScopedStructPointer
{
private:
	StructType* structPointer;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedStructPointer()
	{
		structPointer = nullptr;
	}

	inline KScopedStructPointer(StructType* structPointer)
	{
		this->structPointer = structPointer;
	}

	KScopedStructPointer(KScopedStructPointer& structPointerToTransferFrom)
	{
		this->structPointer = structPointerToTransferFrom.structPointer;
		structPointerToTransferFrom.structPointer = nullptr;
	}

	bool isNull()
	{
		return (structPointer == nullptr);
	}

	/** 
		Removes the current struct pointer from this KScopedStructPointer without freeing it.
		This will return the current struct pointer, and set the KScopedStructPointer to a null pointer.
	*/
	StructType* detach()
	{ 
		StructType* m = structPointer;
		structPointer = nullptr;
		return m; 
	}

	~KScopedStructPointer()
	{
		if (structPointer)
			ReleaseMethod::release(structPointer);
	}

	/** 
		Changes this KScopedStructPointer to point to a new struct.

		If this KScopedStructPointer already points to a struct, that struct
		will first be freed.

		The pointer that you pass in may be a nullptr.
	*/
	KScopedStructPointer& operator= (StructType* const newStructPointer)
	{
		if (structPointer != newStructPointer)
		{
			StructType* const oldStructPointer = structPointer;
			structPointer = newStructPointer;

			if (oldStructPointer)
				ReleaseMethod::release(oldStructPointer);
		}

		return *this;
	}

	inline StructType** operator&() { return &structPointer; }

	/** Returns the struct pointer that this KScopedStructPointer refers to. */
	inline operator StructType*() const { return structPointer; }

	/** Returns the struct pointer that this KScopedStructPointer refers to. */
	inline StructType& operator*() const { return *structPointer; }

	/** Lets you access properties of the struct that this KScopedStructPointer refers to. */
	inline StructType* operator->() const { return structPointer; }

};


// =========== KScopedMemoryBlock.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <malloc.h>

/**
	This class holds a memory which is automatically freed when this object goes
	out of scope.

	Once a memory has been passed to a KScopedMemoryBlock, it will make sure that the memory
	gets freed when the KScopedMemoryBlock is deleted. Using the KScopedMemoryBlock on the stack or
	as member variables is a good way to use RAII to avoid accidentally leaking dynamically
	allocated memory.

	A KScopedMemoryBlock can be used in pretty much the same way that you'd use a normal pointer
	to a memory block. If you use the assignment operator to assign a different memory to a
	KScopedMemoryBlock, the old one will be automatically freed.

	If you need to get a memory block out of a KScopedClassPointer without it being freed, you
	can use the detach() method.

	e.g. @code
	KScopedMemoryBlock<float*> a = (float*)malloc(512 * sizeof(float)); // slow
	KScopedMemoryBlock<float*> b( (float*)malloc(512 * sizeof(float)) ); // fast
	@endcode
*/
template<class T>
class KScopedMemoryBlock
{
private:
	T memoryBlock;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedMemoryBlock()
	{
		memoryBlock = nullptr;
	}

	inline KScopedMemoryBlock(T memoryBlock)
	{
		this->memoryBlock = memoryBlock;
	}

	KScopedMemoryBlock(KScopedMemoryBlock& memoryBlockToTransferFrom)
	{
		this->memoryBlock = memoryBlockToTransferFrom.memoryBlock;
		memoryBlockToTransferFrom.memoryBlock = nullptr;
	}

	bool isNull()
	{
		return (memoryBlock == nullptr);
	}

	/** 
		Removes the current memory block from this KScopedMemoryBlock without freeing it.
		This will return the current memory block, and set the KScopedMemoryBlock to a null pointer.
	*/
	T detach()
	{ 
		T m = memoryBlock;
		memoryBlock = nullptr;
		return m; 
	}

	~KScopedMemoryBlock()
	{
		if (memoryBlock)
			::free(memoryBlock);
	}

	/** 
		Changes this KScopedMemoryBlock to point to a new memory block.

		If this KScopedMemoryBlock already points to a memory, that memory
		will first be freed.

		The pointer that you pass in may be a nullptr.
	*/
	KScopedMemoryBlock& operator= (T const newMemoryBlock)
	{
		if (memoryBlock != newMemoryBlock)
		{
			T const oldMemoryBlock = memoryBlock;
			memoryBlock = newMemoryBlock;

			if (oldMemoryBlock)
				::free(oldMemoryBlock);
		}

		return *this;
	}

	/** Returns the memory block that this KScopedMemoryBlock refers to. */
	inline operator T() const { return memoryBlock; }

};


// =========== KScopedMallocPointer.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <malloc.h>
#include <Objbase.h>

/**
	This class holds a pointer which is allocated using malloc and it will automatically freed when this object goes
	out of scope. 
*/
template<class PointerType>
class KScopedMallocPointer
{
private:
	PointerType* pointer;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedMallocPointer()
	{
		pointer = nullptr;
	}

	inline KScopedMallocPointer(PointerType* pointer)
	{
		this->pointer = pointer;
	}

	KScopedMallocPointer(KScopedMallocPointer& pointerToTransferFrom)
	{
		this->pointer = pointerToTransferFrom.pointer;
		pointerToTransferFrom.pointer = nullptr;
	}

	bool isNull()
	{
		return (pointer == nullptr);
	}

	/** 
		Removes the current pointer from this KScopedMallocPointer without freeing it.
		This will return the current pointer, and set the KScopedMallocPointer to a null pointer.
	*/
	PointerType* detach()
	{ 
		PointerType* m = pointer;
		pointer = nullptr;
		return m; 
	}

	~KScopedMallocPointer()
	{
		if (pointer)
			::free(pointer);
	}

	/** 
		Changes this KScopedMallocPointer to point to a new pointer.

		If this KScopedMallocPointer already holds a pointer, that pointer
		will first be freed.

		The pointer that you pass in may be a nullptr.
	*/
	KScopedMallocPointer& operator= (PointerType* const newPointer)
	{
		if (pointer != newPointer)
		{
			PointerType* const oldPointer = pointer;
			pointer = newPointer;

			if (oldPointer)
				::free(oldPointer);
		}

		return *this;
	}

	inline PointerType** operator&() { return &pointer; }

	/** Returns the pointer that this KScopedMallocPointer refers to. */
	inline operator PointerType*() const { return pointer; }

	/** Returns the pointer that this KScopedMallocPointer refers to. */
	inline PointerType& operator*() const { return *pointer; }

};


// =========== KAssert.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/
#pragma once

#include <crtdbg.h>

// spawns crt assertion error gui if condition is false.
#ifdef _DEBUG
#define K_ASSERT(cond, msg) \
    do { \
        if (!(cond)) { \
            _CrtDbgReport(_CRT_ASSERT, __FILE__, __LINE__, nullptr, msg); \
            _CrtDbgBreak(); \
        } \
    } while (0)
#else
#define K_ASSERT(cond, msg) ((void)0)
#endif

// =========== KScopedGdiObject.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <windows.h>

/**
	This class holds a gdi object which is automatically freed when this object goes
	out of scope.

	If you need to get a gdi object out of a KScopedGdiObject without it being freed, you
	can use the detach() method.

	e.g. @code
	KScopedGdiObject<HBRUSH> a = ::CreateSolidBrush(RGB(255, 255, 255)); // slow
	KScopedGdiObject<HBRUSH> b(::CreateSolidBrush(RGB(255, 0, 0))); // fast
	@endcode,
*/
template<class T>
class KScopedGdiObject
{
private:
	T gdiObject;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedGdiObject()
	{
		gdiObject = 0;
	}

	inline KScopedGdiObject(T gdiObject)
	{
		this->gdiObject = gdiObject;
	}

	KScopedGdiObject(KScopedGdiObject& gdiObjectToTransferFrom)
	{
		this->gdiObject = gdiObjectToTransferFrom.gdiObject;
		gdiObjectToTransferFrom.gdiObject = 0;
	}

	/** 
		Removes the current gdi object from this KScopedGdiObject without freeing it.
		This will return the current gdi object, and set the KScopedGdiObject to a null value.
	*/
	T detach()
	{ 
		T g = gdiObject;
		gdiObject = 0;
		return g; 
	}

	~KScopedGdiObject()
	{
		if (gdiObject)
			::DeleteObject(gdiObject);
	}

	/** 
		Changes this KScopedGdiObject to point to a new gdi object.

		If this KScopedGdiObject already points to a gdi object, that object
		will first be freed.

		The object that you pass in may be a zero.
	*/
	KScopedGdiObject& operator= (T const newGdiObject)
	{
		if (gdiObject != newGdiObject)
		{
			T const oldgdiObject = gdiObject;
			gdiObject = newGdiObject;

			if (oldgdiObject)
				::DeleteObject(oldgdiObject);
		}

		return *this;
	}

	/** Returns the gdi object that this KScopedGdiObject refers to. */
	inline operator T() const { return gdiObject; }

};


// =========== KScopedCriticalSection.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <windows.h>

/**
	This class holds a pointer to CRITICAL_SECTION which is automatically released when this object goes
	out of scope.
*/
class KScopedCriticalSection
{
private:
	CRITICAL_SECTION* criticalSection;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	KScopedCriticalSection(CRITICAL_SECTION* criticalSection)
	{
		this->criticalSection = criticalSection;
		::EnterCriticalSection(criticalSection);
	}

	// does not call LeaveCriticalSection
	CRITICAL_SECTION* detach()
	{ 
		CRITICAL_SECTION* c = criticalSection;
		criticalSection = nullptr;
		return c; 
	}

	~KScopedCriticalSection()
	{
		if (criticalSection)
			::LeaveCriticalSection(criticalSection);
	}

	inline operator CRITICAL_SECTION*() const { return criticalSection; }

};


// =========== KScopedComPointer.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <windows.h>

/**
	This class holds a COM pointer which is automatically released when this object goes
	out of scope.
*/
template<class T>
class KScopedComPointer
{
private:
	T* object;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedComPointer()
	{
		object = nullptr;
	}

	inline KScopedComPointer(T* object)
	{
		this->object = object;

		if (this->object)
			this->object->AddRef();
	}

	KScopedComPointer(KScopedComPointer& objectToTransferFrom)
	{
		object = objectToTransferFrom.object;

		if (object)
			object->AddRef();
	}

	bool isNull()
	{
		return (object == nullptr);
	}

	/** 
		Removes the current COM object from this KScopedComPointer without releasing it.
		This will return the current object, and set the KScopedComPointer to a null pointer.
	*/
	T* detach()
	{ 
		T* o = object; 
		object = nullptr;
		return o; 
	}

	~KScopedComPointer()
	{
		if (object)
			object->Release();

		object = nullptr;
	}

	inline T** operator&() { return &object; }

	/** Returns the object that this KScopedComPointer refers to. */
	inline operator T*() const { return object; }

	/** Returns the object that this KScopedComPointer refers to. */
	inline T& operator*() const { return *object; }

	/** Lets you access methods and properties of the object that this KScopedComPointer refers to. */
	inline T* operator->() const { return object; }

};


// =========== KScopedClassPointer.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <windows.h>

/**
	This class holds a pointer which is automatically deleted when this object goes
	out of scope.

	Once a pointer has been passed to a KScopedClassPointer, it will make sure that the pointer
	gets deleted when the KScopedClassPointer is deleted. Using the KScopedClassPointer on the stack or
	as member variables is a good way to use RAII to avoid accidentally leaking dynamically
	created objects.

	A KScopedClassPointer can be used in pretty much the same way that you'd use a normal pointer
	to an object. If you use the assignment operator to assign a different object to a
	KScopedClassPointer, the old one will be automatically deleted.

	Important note: The class is designed to hold a pointer to an object, NOT to an array!
	It calls delete on its payload, not delete[], so do not give it an array to hold!

	If you need to get a pointer out of a KScopedClassPointer without it being deleted, you
	can use the detach() method.

	e.g. @code
	KScopedClassPointer<MyClass> a = new MyClass(); // slow
	a->myMethod();
	a = new MyClass(); // old object will be deleted
	KScopedClassPointer<MyClass> b( new MyClass() ); // fast
	@endcode

*/
template<class T>
class KScopedClassPointer
{
private:
	T* object;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	inline KScopedClassPointer()
	{
		object = nullptr;
	}

	inline KScopedClassPointer(T* object)
	{
		this->object = object;
	}

	KScopedClassPointer(KScopedClassPointer& objectToTransferFrom)
	{
		this->object = objectToTransferFrom.object;
		objectToTransferFrom.object = nullptr;
	}

	bool isNull()
	{
		return (object == nullptr);
	}

	/** 
		Removes the current object from this KScopedClassPointer without deleting it.
		This will return the current object, and set the KScopedClassPointer to a null pointer.
	*/
	T* detach()
	{ 
		T* o = object; 
		object = nullptr;
		return o; 
	}

	~KScopedClassPointer()
	{
		if (object)
			delete object;
	}

	/** 
		Changes this KScopedClassPointer to point to a new object.

		If this KScopedClassPointer already points to an object, that object
		will first be deleted.

		The pointer that you pass in may be a nullptr.
	*/
	KScopedClassPointer& operator= (T* const newObject)
	{
		if (object != newObject)
		{
			T* const oldObject = object;
			object = newObject;

			if (oldObject)
				delete oldObject;
		}

		return *this;
	}

	/** Returns the object that this KScopedClassPointer refers to. */
	inline operator T*() const { return object; }

	/** Returns the object that this KScopedClassPointer refers to. */
	inline T& operator*() const { return *object; }

	/** Lets you access methods and properties of the object that this KScopedClassPointer refers to. */
	inline T* operator->() const { return object; }
};

// =========== KScopedHandle.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <windows.h>

/**
	This class holds a handle which is automatically closed when this object goes
	out of scope.
*/
class KScopedHandle
{
private:
	HANDLE handle;

	// Prevent heap allocation
	void* operator new(size_t);
	void* operator new[](size_t);
	void  operator delete(void*);
	void  operator delete[](void*);

public:
	KScopedHandle()
	{
		handle = 0;
	}

	KScopedHandle(HANDLE handle)
	{
		this->handle = handle;
	}

	HANDLE detach()
	{
		HANDLE h = handle;
		handle = 0;
		return h;
	}

	KScopedHandle& operator= (HANDLE newHandle)
	{
		if (handle != newHandle)
		{
			HANDLE oldHandle = handle;
			handle = newHandle;

			if (oldHandle)
				::CloseHandle(oldHandle);
		}

		return *this;
	}

	bool isNull()
	{
		return (handle == 0);
	}

	~KScopedHandle()
	{
		if (handle)
			::CloseHandle(handle);
	}

	inline operator HANDLE() const { return handle; }

	inline HANDLE* operator&() { return &handle; }

};


// =========== KRefCountedMemory.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once

#include <windows.h>

/**
	This class holds reference counted heap memory which is allocated using malloc.
	when ref count reach zero, the memory will be released using ::free.
*/
template<class T>
class KRefCountedMemory
{
private:
	~KRefCountedMemory() {}

protected:
	volatile LONG refCount;

public:
	T buffer;

	KRefCountedMemory(T buffer) : refCount(1), buffer(buffer) {}
	
	/**
		Make sure to call this method if you construct new KRefCountedMemory or keep reference to another KRefCountedMemory object.
	*/
	void addReference()
	{
		::InterlockedIncrement(&refCount);
	}

	/**
		Make sure to call this method if you clear reference to KRefCountedMemory object. 
		it will release allocated memory for string if ref count is zero.
	*/
	void releaseReference()
	{
		const LONG res = ::InterlockedDecrement(&refCount);
		if (res == 0)
		{
			if (buffer)
				::free(buffer);

			delete this;
		}
	}

private:
	RFC_LEAK_DETECTOR(KRefCountedMemory)
};



// =========== KString.h ===========
/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


#include <windows.h>
#include <string.h>
#include <malloc.h>

/*
	DO_NOT_FREE: supplied pointer is a static string literal(always available). it will not freed on destroy.
	FREE_ON_DESTROY: supplied pointer is a heap memory created using malloc. it will be freed on destroy.
	MAKE_A_COPY: make a copy of supplied string. if string length is larger than 11, heap memory will be allocated.
*/
enum class KStringBehaviour { DO_NOT_FREE,
	FREE_ON_DESTROY, 
	MAKE_A_COPY
};

// define RFC_NO_CHECK_ARRAY_AS_LITERAL if you want to disable the check for array passed as string literal at debug mode.
#ifndef RFC_NO_CHECK_ARRAY_AS_LITERAL
#define RFC_CHECK_ARRAY_AS_LITERAL(literal,N) K_ASSERT(wcslen(literal) == (N - 1), "array used as a string literal. please use the array with KString constructor that accepts a behavior parameter.");
#else
#define RFC_CHECK_ARRAY_AS_LITERAL(literal,N) 
#endif

enum class KStringBufferType : unsigned char { StaticText, SSOText, HeapText };

/**
	Using a reference-counted internal representation for long strings and Small String Optimization (SSO) for short strings.
	KString was optimized to use with unicode strings. So, use unicode strings instead of ansi.
	KString does not support for multiple zero terminated strings.

	SSO Implementation:
	- Strings with 11 characters or fewer are stored directly in the object (Small String Optimization)
	- Longer strings use the heap with KRefCountedMemory mechanism
	- Static text references just use pointer assignment

	Optimization tips:
	use unicode strings instead of ansi.
	try to use strings with length less than 12.
	L"hello"_st is same as CONST_TXT("hello") or KString(L"hello",KString::STATIC_TEXT_DO_NOT_FREE,5)
	use "CompareWithStaticText" method instead of "Compare" if you are comparing statically typed text.

*/
class KString
{
public:
	// SSO buffer size: can fit up to 11 wchar_t characters + null terminator in 24 bytes
	static const int SSO_BUFFER_SIZE = 12;

protected:
	// we try to make sizeof KString to be 32 bytes for better cache align.

	// you can use either data.ssoBuffer or data.staticText or data.refCountedMem. 
	// can use only one at a time. selected by the bufferType.
	union {
		KRefCountedMemory<wchar_t*>* refCountedMem;
		const wchar_t* staticText;
		wchar_t ssoBuffer[SSO_BUFFER_SIZE]; // for small strings
	} data; 

	int characterCount; // character count (empty string has zero characterCount)
	KStringBufferType bufferType;

	inline void markAsEmptyString();

	void initFromLiteral(const wchar_t* literal, size_t N);
	void assignFromLiteral(const wchar_t* literal, size_t N);
	void copyFromOther(const KString& other);
public:

	/**
		Constructs an empty string
	*/
	KString();

	/**
		Constructs copy of another string.
		Same performance as move. lightweight!
	*/
	KString(const KString& other);

	/**
		Move constructor. Same performance as copy. other string will be cleared.
	*/
	KString(KString&& other) noexcept;

	/**
		Constructs String object using ansi string
	*/
	KString(const char* const text, UINT codePage = CP_UTF8);

	/**
		Constructs String object using unicode string literal
	*/
	template<size_t N>
	KString(const wchar_t(&literal)[N])
	{
		RFC_CHECK_ARRAY_AS_LITERAL(literal, N);
		initFromLiteral(literal, N);
	}

	/**
		Constructs String object using unicode string pointer
	*/
	KString(const wchar_t* const text, KStringBehaviour behaviour, int length = -1);

	/**
		Constructs String object using integer
	*/
	KString(const int value, const int radix = 10);

	/**
		Constructs String object using float
		@param compact   removes ending decimal zeros if true
	*/
	KString(const float value, const int numDecimals, bool compact = false);

	/** 
		Replaces this string's contents with another string.
	*/
	const KString& operator= (const KString& other);

	// Move assignment. clears other string.
	KString& operator= (KString&& other);

	/**
		Replaces this string's contents with static unicode string literal.
	*/
	template<size_t N>
	const KString& operator= (const wchar_t(&literal)[N])
	{
		RFC_CHECK_ARRAY_AS_LITERAL(literal, N);
		assignFromLiteral(literal, N);
		return *this;
	}

	// compare with other string
	bool operator==(const KString& other) const;

	/** 
		Appends a string at the end of this one.
		@returns     the concatenated string
	*/
	const KString operator+ (const KString& stringToAppend);

	/**
		Appends a unicode string literal at the end of this one.
		@returns     the concatenated string
	*/
	template<size_t N>
	const KString operator+ (const wchar_t(&literalToAppend)[N])
	{
		RFC_CHECK_ARRAY_AS_LITERAL(literalToAppend, N);
		return appendStaticText(literalToAppend, (int)N - 1);
	}

	/**
		Returns const unicode version of this string
	*/
	operator const wchar_t*()const;

	/** 
		Returns a character from the string.
		@returns -1 if index is out of range
	*/
	const wchar_t operator[](const int index)const;

	/**
		Appends a string at the end of this one.
		@returns     the concatenated string
	*/
	KString append(const KString& otherString)const;

	/**
		Appends a statically typed string to beginning or end of this one.
		@param text			statically typed text
		@param length		text length. should not be zero.
		@param appendToEnd	appends to beginning if false
		@returns			the concatenated string
	*/
	KString appendStaticText(const wchar_t* const text, int length, bool appendToEnd = true)const;

	/**
		Assigns a statically typed string.
		@param text			statically typed text
		@param length		text length. should not be zero.
	*/
	void assignStaticText(const wchar_t* const text, int length);

	// clears the content of the string.
	void clear();

	// the string automatically clears and converted to SSOText when you call accessRawSSOBuffer.
	// sso buffer size is KString::SSO_BUFFER_SIZE in wchars.
	void accessRawSSOBuffer(wchar_t** ssoBuffer, int** ppLength);

	/** 
		Returns a subsection of the string.

		If the range specified is beyond the limits of the string, empty string
		will be return.

		@param start   the index of the start of the substring needed
		@param end     all characters from start up to this index are returned
	*/
	KString subString(int start, int end)const;

	/**
		Case-insensitive comparison with another string. Slower than "Compare" method.
		@returns     true if the two strings are identical, false if not
	*/
	bool compareIgnoreCase(const KString& otherString)const;

	/** 
		Case-sensitive comparison with another string.
		@returns     true if the two strings are identical, false if not
	*/
	bool compare(const KString& otherString)const;

	/** 
		Case-sensitive comparison with statically typed string.
		@param text		statically typed text.
		@returns		true if the two strings are identical, false if not
	*/
	bool compareWithStaticText(const wchar_t* const text)const;

	/**
		Compare first character with given unicode character
	*/
	bool startsWithChar(wchar_t character)const;

	/**
		Compare last character with given unicode character
	*/
	bool endsWithChar(wchar_t character)const;

	/**
		Check if string is quoted or not
	*/
	bool isQuotedString()const;

	/** 
		Returns a character from the string.
		@returns -1 if index is out of range
	*/
	wchar_t getCharAt(int index)const;

	KStringBufferType getBufferType()const;

	/**
		Returns number of characters in string
	*/
	int length()const;

	/**
		Returns true if string is empty
	*/
	bool isEmpty()const;

	bool isNotEmpty()const;

	/**
		Returns value of string
	*/
	int getIntValue()const;

	/** 
		Returns an upper-case version of this string.
	*/
	KString toUpperCase()const;

	/** 
		Returns an lower-case version of this string. 
	*/
	KString toLowerCase()const;

	// free the returned buffer when done.
	static char* toAnsiString(const wchar_t* text);
	static wchar_t* toUnicodeString(const char* text);

	~KString();

private:
	/**
		Returns pointer to the actual string data regardless of storage type
	*/
	const wchar_t* getStringPtr() const;

	RFC_LEAK_DETECTOR(KString)
};

// static text literal operator
namespace kstring_literals {
	KString operator"" _st(const wchar_t* str, size_t len);
}

using namespace kstring_literals;

const KString operator+ (const char* const string1, const KString& string2);

const KString operator+ (const wchar_t* const string1, const KString& string2);

const KString operator+ (const KString& string1, const KString& string2);

#define LEN_UNI_STR(X) (sizeof(X) / sizeof(wchar_t)) - 1

#define LEN_ANSI_STR(X) (sizeof(X) / sizeof(char)) - 1

// do not make a copy + do not free + do not calculate length
#define CONST_TXT(X) KString(L##X, KStringBehaviour::DO_NOT_FREE, LEN_UNI_STR(L##X))

// do not make a copy + do not free + calculate length
#define STATIC_TXT(X) KString(L##X, KStringBehaviour::DO_NOT_FREE, -1)

// do not make a copy + free when done + calculate length
#define BUFFER_TXT(X) KString(X, KStringBehaviour::FREE_ON_DESTROY, -1)

// can be use like this: KString str(CONST_TXT_PARAMS("Hello World"));
#define CONST_TXT_PARAMS(X) L##X, KStringBehaviour::DO_NOT_FREE, LEN_UNI_STR(L##X)

#define TXT_WITH_LEN(X) L##X, LEN_UNI_STR(L##X)

// =========== KApplication.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once

#include <windows.h>

/**
	Derive your application object from this class!
	Override Main method and put your application code there.
	Use START_RFC_APPLICATION macro somewhere in a cpp file to declare an instance of this class.

	e.g. @code
	class MyApp : public KApplication
	{
		public:
			MyApp(){}
			~MyApp(){}

			int main(wchar_t** argv,int argc)
			{
				// your app code goes here...
				return 0;
			}
	};

	START_RFC_APPLICATION(MyApp)
	@endcode
*/
class KApplication
{
public:

	/**
		Use this field to get HINSTANCE of your application.
		(This field will set when calling InitRFC function.)
	*/
	static HINSTANCE hInstance;

	static KDPIAwareness dpiAwareness;
	static bool dpiAwareAPICalled;

	/** 
		Constructs an KApplication object.
	*/
	KApplication();

	/**
		Called before InitRFC function call. Use this method to modify each module InitParams.
		Do not call framework APIs here. Only modify InitParams.
	*/
	virtual void modifyModuleInitParams();

	/** 
		Called when the application starts.
		Put your application code here and if you create a window, 
		then make sure to call messageLoop method before you return.

		@param argv array of command-line arguments! access them like this wchar_t* arg1=argv[0];
		@param argc number of arguments
	*/
	virtual int main(wchar_t** argv, int argc);

	/**
		Return false if your application is single instance only.
		Single instance applications must implement "getApplicationID" method.
	*/
	virtual bool allowMultipleInstances();

	/**
		This method will be called if the application is single instance only and another instance is already running.
		("main" method will not be called.)
	*/
	virtual int anotherInstanceIsRunning(wchar_t** argv, int argc);

	/**
		Unique id of your application which is limited to MAX_PATH characters.
		Single instance applications must implement this method.
	*/
	virtual const wchar_t* getApplicationID();

	static void messageLoop(bool handleTabKey = true);

	/** 
		Destructs an Application object.
	*/
	virtual ~KApplication();

private:
	RFC_LEAK_DETECTOR(KApplication)
};


// =========== Core.h ===========

/*
	Copyright (C) 2013-2025  CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <windows.h>

void InitRFCModules();
void DeInitRFCModules();

// use within a dll functions. do not use inside of DllMain.
void RFCDllInit();
void RFCDllFree();

#define RFC_MAX_PATH 512

#define START_RFC_CONSOLE_APP(AppClass) \
int main() \
{ \
	CoreModuleInitParams::hInstance = 0; \
	CoreModuleInitParams::initCOMAsSTA = true; \
	CoreModuleInitParams::dpiAwareness = KDPIAwareness::UNAWARE_MODE; \
	int retVal = 0; \
	LPWSTR* args = nullptr; \
	{AppClass application; \
	application.modifyModuleInitParams(); \
	::InitRFCModules(); \
	int argc = 0; \
	args = ::CommandLineToArgvW(::GetCommandLineW(), &argc); \
	if (application.allowMultipleInstances()){ \
		retVal = application.main(args, argc); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application.getApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application.main(args, argc); \
		}else{ \
			retVal = application.anotherInstanceIsRunning(args, argc); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	} ::DeInitRFCModules(); \
	::LocalFree(args); \
	return retVal; \
}

// use this macro if you are not using commandline arguments in your app.
#define START_RFC_CONSOLE_APP_NO_CMD_ARGS(AppClass) \
int WINAPI main() \
{ \
	CoreModuleInitParams::hInstance = 0; \
	CoreModuleInitParams::initCOMAsSTA = true; \
	CoreModuleInitParams::dpiAwareness = KDPIAwareness::UNAWARE_MODE; \
	int retVal = 0; \
	{AppClass application; \
	application.modifyModuleInitParams(); \
	::InitRFCModules(); \
	if (application.allowMultipleInstances()){ \
		retVal = application.main(0, 0); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application.getApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application.main(0, 0); \
		}else{ \
			retVal = application.anotherInstanceIsRunning(0, 0); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	}::DeInitRFCModules(); \
	return retVal; \
}

#define START_RFC_APPLICATION(AppClass, DPIAwareness) \
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) \
{ \
	CoreModuleInitParams::hInstance = hInstance; \
	CoreModuleInitParams::initCOMAsSTA = true; \
	CoreModuleInitParams::dpiAwareness = DPIAwareness; \
	int retVal = 0; \
	LPWSTR* args = nullptr; \
	{AppClass application; \
	application.modifyModuleInitParams(); \
	::InitRFCModules(); \
	int argc = 0; \
	args = ::CommandLineToArgvW(::GetCommandLineW(), &argc); \
	if (application.allowMultipleInstances()){ \
		retVal = application.main(args, argc); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application.getApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application.main(args, argc); \
		}else{ \
			retVal = application.anotherInstanceIsRunning(args, argc); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	}\
	::DeInitRFCModules(); \
	::LocalFree(args); \
	return retVal; \
}

// use this macro if you are not using commandline arguments in your app.
#define START_RFC_APPLICATION_NO_CMD_ARGS(AppClass, DPIAwareness) \
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) \
{ \
	CoreModuleInitParams::hInstance = hInstance; \
	CoreModuleInitParams::initCOMAsSTA = true; \
	CoreModuleInitParams::dpiAwareness = DPIAwareness; \
	int retVal = 0; \
	{AppClass application; \
	application.modifyModuleInitParams(); \
	::InitRFCModules(); \
	if (application.allowMultipleInstances()){ \
		retVal = application.main(0, 0); \
	}else{ \
		HANDLE hMutex = ::CreateMutexW(NULL, TRUE, application.getApplicationID()); \
		if ((hMutex != NULL) && (GetLastError() != ERROR_ALREADY_EXISTS)) { \
			retVal = application.main(0, 0); \
		}else{ \
			retVal = application.anotherInstanceIsRunning(0, 0); \
		} \
		if (hMutex){ \
			::ReleaseMutex(hMutex); \
		} \
	} \
	} ::DeInitRFCModules(); \
	return retVal; \
}

// require to support XP/Vista styles.
#ifdef _MSC_VER
	#ifndef STYLE_MANIFEST_DEFINED
		#ifndef RFC_NO_MANIFEST
			#define STYLE_MANIFEST_DEFINED
			#ifdef RFC64
				#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
			#else
				#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
			#endif
		#endif
	#endif
#endif


#ifdef _DEBUG
	#define DEBUG_PRINT(x) OutputDebugStringA(x);
#else 
	#define DEBUG_PRINT(x) 
#endif

// =========== CoreModule.h ===========

/*
    Copyright (C) 2013-2025 CrownSoft

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <windows.h>

// link default libs here so we don't need to link them from commandline(Clang).

#pragma comment(lib,"user32.lib")
#pragma comment(lib,"Shell32.lib")
#pragma comment(lib,"Ole32.lib")
#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"Advapi32.lib")
#pragma comment(lib,"Comdlg32.lib")

class CoreModuleInitParams {
public:
	/**
		If you are in EXE, then hInstance is HINSTANCE provided by WinMain. Or you can use zero.
		If you are in DLL, then hInstance is HINSTANCE provided by DllMain or HMODULE of the DLL. Or you can use zero.
		If you are in Console app, then use zero.
		The defalut value is zero.
	*/
	static HINSTANCE hInstance;

	static bool initCOMAsSTA; // initializes COM as STA. default value is true
	static KDPIAwareness dpiAwareness; // default value is UNAWARE_MODE
};

// =========== KIcon.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once


/**
	Can be use to load icon from file or resource.
*/
class KIcon
{
protected:
	HICON hIcon;
	WORD resourceID;

public:
	KIcon();

	// does not load. to be use with getScaledIcon method.
	void setResource(WORD resourceID);

	// this method scales down a larger image instead of scaling up a smaller image.
	// can be use for high-dpi requirements.
	// must destroy returned icon by calling DestroyIcon
	HICON getScaledIcon(int size);

	/**
		Loads icon from resource with default size given by the system
		@param resourceID resource ID of icon file
		@returns false if icon load fails
	*/
	bool loadFromResource(WORD resourceID);

	/**
		Loads icon from file with default size given by the system
		@param filePath path to icon file
		@returns false if icon load fails
	*/
	bool loadFromFile(const KString& filePath);

	/**
		Returns icon handle
	*/
	HICON getHandle();

	operator HICON()const;

	virtual ~KIcon();

private:
	RFC_LEAK_DETECTOR(KIcon)
};


// =========== KMenuItem.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once

#include <functional>

class KMenuItem
{
protected:
	HMENU hMenu;
	UINT itemID;
	KString itemText;
	bool enabled;
	bool checked;
	void* param;
	int intParam;

public:
	std::function<void(KMenuItem*)> onPress;

	KMenuItem();

	virtual void addToMenu(HMENU hMenu);

	virtual void setParam(void* param);

	virtual void setIntParam(int intParam);

	virtual int getIntParam();

	virtual void* getParam();

	virtual bool isChecked();

	virtual void setCheckedState(bool state);

	virtual bool isEnabled();

	virtual void setEnabled(bool state);

	virtual void setText(const KString& text);

	virtual KString getText();

	virtual UINT getItemID();

	virtual HMENU getMenuHandle();

	virtual void _onPress();

	virtual ~KMenuItem();

private:
	RFC_LEAK_DETECTOR(KMenuItem)
};


// =========== KTransparentBitmap.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once


#pragma comment(lib, "Msimg32.lib") // AlphaBlend

/**
	Can be use to create 32bit bitmap image from data.
*/
class KTransparentBitmap
{
protected:
	int width;
	int height;
	HDC hdcMem;
	HBITMAP hbm;
	HBITMAP hbmPrev;
	void* pvBits;

	void releaseResources();
	void createEmptyBitmap(int width, int height);

public:
	// data must be in 0xaarrggbb format with premultiplied alpha.
	// stride must be equal to width * 4.
	// data will be copied to internal buffer.
	KTransparentBitmap(void* data, int width, int height, int stride);

	// creates a transparent empty image
	KTransparentBitmap(int width, int height);

	// color format: 0xaarrggbb
	unsigned int getPixel(int x, int y);

	bool hitTest(int x, int y);

	int getWidth();

	int getHeight();

	// also clears the content
	void resize(int width, int height);

	// use AlphaBlend to draw
	// standard gdi drawing commands may not work with the returned hdc. (content has premultiplied alpha)
	// copy to secondary hdc using AlphaBlend or use gdi+ with PixelFormat32bppPARGB
	HDC getDC();

	void draw(HDC destHdc, int destX, int destY, BYTE alpha = 255);

	void draw(HDC destHdc, int destX, int destY, int destWidth, int destHeight, BYTE alpha = 255);

	// can copy/scale specific part of the image
	void draw(HDC destHdc, int destX, int destY, int destWidth, int destHeight, int srcX, int srcY, int srcWidth, int srcHeight, BYTE alpha = 255);

	virtual ~KTransparentBitmap();

private:
	RFC_LEAK_DETECTOR(KTransparentBitmap)
};




// =========== KTime.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once


// __int64 is not defined in mingw.
#ifdef __MINGW32__
	#include <stdint.h>
	typedef int64_t _int64;
#endif

class KTime
{
public:

	KTime(){}

	// Returns time difference in units of 100 us.
	static _int64 delta100us(const SYSTEMTIME& time1, const SYSTEMTIME& time2)
	{
		union timeunion {
			FILETIME fileTime;
			ULARGE_INTEGER ul;
		};

		timeunion ft1;
		timeunion ft2;

		::SystemTimeToFileTime(&time1, &ft1.fileTime);
		::SystemTimeToFileTime(&time2, &ft2.fileTime);

		return ft2.ul.QuadPart - ft1.ul.QuadPart;
	}

	// Returns time difference in seconds.
	static _int64 deltaSeconds(const SYSTEMTIME& time1, const SYSTEMTIME& time2)
	{
		return (delta100us(time1, time2) / 10000000);
	}

	// Returns time difference in minutes.
	static _int64 deltaMinutes(const SYSTEMTIME& time1, const SYSTEMTIME& time2)
	{
		return (deltaSeconds(time1, time2) / 60);
	}

	// Returns time difference in hours.
	static _int64 deltaHours(const SYSTEMTIME& time1, const SYSTEMTIME& time2)
	{
		return (deltaMinutes(time1, time2) / 60);
	}

	static void getNow(SYSTEMTIME* time, const bool isLocalTime = true)
	{
		if (isLocalTime)
			::GetLocalTime(time);
		else
			::GetSystemTime(time);
	}

	~KTime(){}

private:
	RFC_LEAK_DETECTOR(KTime)
};


// =========== KStackInfo.h ===========

/*
    Copyright (C) 2013-2025 CrownSoft

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <stdio.h>

// shows current thread stack usage.
class KStackInfo
{
    static void _printUsage(size_t used, size_t total)
    {
        char buf[256];
        sprintf_s(buf, sizeof(buf),
            "Stack: %zu/%zu bytes (%.1f%% used, %.1f MB total)\n",
            used, total, (double)used / total * 100.0, total / (1024.0 * 1024.0));

        ::OutputDebugStringA(buf);
    }

public:
    static size_t getTotalStackSize()
    {
        ULONG_PTR low, high;
        ::GetCurrentThreadStackLimits(&low, &high);
        return high - low;
    }

    static size_t getCurrentStackUsage()
    {
        ULONG_PTR low, high;
        ::GetCurrentThreadStackLimits(&low, &high);

        volatile char dummy;
        void* currentSP = (void*)&dummy;

        return high - (ULONG_PTR)currentSP;
    }

    static void showStackInfo()
    {    
        size_t total = KStackInfo::getTotalStackSize();
        size_t used = KStackInfo::getCurrentStackUsage();

        KStackInfo::_printUsage(used, total);
    }
};

// =========== KRegistry.h ===========

/*
    Copyright (C) 2013-2025 CrownSoft
  
    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.     
*/

#pragma once


class KRegistry
{

public:
	KRegistry();

	// returns true on success or if the key already exists.
	static bool createKey(HKEY hKeyRoot, const KString& subKey);

	// the subkey to be deleted must not have subkeys. 
	static bool deleteKey(HKEY hKeyRoot, const KString& subKey);

	static bool readString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, KString* result);

	static bool writeString(HKEY hKeyRoot, const KString& subKey, const KString& valueName, const KString& value);

	static bool readDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD* result);

	static bool writeDWORD(HKEY hKeyRoot, const KString& subKey, const KString& valueName, DWORD value);

	/**
		you must free the buffer when you are done with it.

		e.g. @code
		void *buffer;
		DWORD bufferSize;
		if(KRegistry::readBinary(xxx, xxx, xxx, &buffer, &buffSize))
		{
			// do your thing here...

			free(buffer);
		}
		@endcode
	*/
	static bool readBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void** buffer, DWORD* buffSize);

	static bool writeBinary(HKEY hKeyRoot, const KString& subKey, const KString& valueName, void* buffer, DWORD buffSize);

	~KRegistry();

};

// =========== KGraphics.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once

class KGraphics
{
public:
	KGraphics();

	virtual ~KGraphics();

	static void draw3dVLine(HDC hdc, int startX, int startY, int height);

	static void draw3dHLine(HDC hdc, int startX, int startY, int width);

	static void draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight);

	static void draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight);

	static void fillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF color);

	static void fillSolidRect(HDC hdc, LPCRECT lpRect, COLORREF color);

	static RECT calculateTextSize(const wchar_t* text, HFONT hFont);

	static int calculateTextHeight(const wchar_t* text, HFONT hFont, int width);

	// This function sets the alpha channel to 255 without affecting any of the color channels.
	// hdc is a memory DC with a 32bpp bitmap selected into it.
	// can be use to fix 32bit bitmap alpha which is destroyed by the gdi operations.
	static void makeBitmapOpaque(HDC hdc, int x, int y, int cx, int cy);

	// hdc is a memory DC with a 32bpp bitmap selected into it.
	// This function sets the alpha channel without affecting any of the color channels.
	static void setBitmapAlphaChannel(HDC hdc, int x, int y, int cx, int cy, BYTE alpha);

private:
	RFC_LEAK_DETECTOR(KGraphics)
};


// =========== KPerformanceCounter.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


class KPerformanceCounter
{
protected:
	double pcFreq;
	__int64 counterStart;

public:
	KPerformanceCounter();

	void startCounter();

	/**
		returns delta time(milliseconds) between startCounter and endCounter calls.
	*/
	double endCounter();

	~KPerformanceCounter();

private:
	RFC_LEAK_DETECTOR(KPerformanceCounter)
};


// =========== KGuid.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once


// RPC_WSTR is not defined in mingw.
#ifdef __MINGW32__
	typedef unsigned short* RPC_WSTR;
#endif

class KGuid
{
public:

	KGuid(){}

	static bool generateGUID(GUID* pGUID)
	{
		return (::CoCreateGuid(pGUID) == S_OK);
	}

	static KString generateGUID()
	{
		GUID guid;

		if (KGuid::generateGUID(&guid))
			return KGuid::guidToString(&guid);

		return KString();
	}

	static KString guidToString(GUID* pGUID)
	{
		wchar_t* strGuid = nullptr;
		::UuidToStringW(pGUID, (RPC_WSTR*)&strGuid);

		KString result(strGuid, KStringBehaviour::MAKE_A_COPY);
		RpcStringFreeW((RPC_WSTR*)&strGuid);

		return result;
	}

	~KGuid(){}

private:
	RFC_LEAK_DETECTOR(KGuid)
};


// =========== KCursor.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


/**
	Can be use to load cursor from file or resource.
*/
class KCursor
{
protected:
	HCURSOR hCursor;

public:
	KCursor();

	/**
		Loads cursor from resource
		@param resourceID resource ID of cursor file
		@returns false if cursor load fails
	*/
	bool loadFromResource(WORD resourceID);

	/**
		Loads cursor from file
		@param filePath path to cursor file
		@returns false if cursor load fails
	*/
	bool loadFromFile(const KString& filePath);

	/**
		Returns cursor handle
	*/
	HCURSOR getHandle();

	/**
		Returns cursor handle
	*/
	operator HCURSOR()const;

	virtual ~KCursor();

private:
	RFC_LEAK_DETECTOR(KCursor)
};


// =========== KPointerList.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/


#pragma once

#include <malloc.h>
#include <functional>

/**
	Helper base class for thread safety - only contains critical section when needed
*/
template<bool IsThreadSafe>
struct KThreadSafetyBase
{
	// Empty base class when thread safety is not needed. ("empty base optimization")
};

template<>
struct KThreadSafetyBase<true>
{
	CRITICAL_SECTION criticalSection;

	KThreadSafetyBase()
	{
		::InitializeCriticalSection(&criticalSection);
	}

	~KThreadSafetyBase()
	{
		::DeleteCriticalSection(&criticalSection);
	}
};

/**
	Holds a resizable list of pointers with small buffer optimization.
	Thread safety is determined at compile time via template parameter.
	Duplicated items allowed!
	index is between 0 to (item count-1)

	@param T The pointer type to store
	@param SmallBufferSize Number of items to store in stack buffer before allocating heap memory
	@param IsThreadSafe Compile-time flag for thread safety

	e.g. @code
	KButton btn1;
	KPointerList<KButton*, 8, false> btnList; // 8 items in small buffer, not thread safe
	KPointerList<KButton*, 8, true> threadSafeBtnList; // 8 items in small buffer, thread safe
	btnList.addPointer(&btn1);
	btnList.addPointer(&btn1);
	@endcode
*/
template<class T, int SmallBufferSize, bool IsThreadSafe>
class KPointerList : private KThreadSafetyBase<IsThreadSafe>
{
protected:
	int itemCount; // current element count in the list
	int roomCount; // maximum element count
	T* list;
	T smallBuffer[SmallBufferSize]; // Stack-allocated small buffer
	bool usingSmallBuffer;

	void resetToSmallBuffer()
	{
		usingSmallBuffer = true;
		list = smallBuffer;
		roomCount = SmallBufferSize;
		itemCount = 0;
	}

	// Thread safety helper methods
	inline void enterCriticalSectionIfNeeded()
	{
		if constexpr (IsThreadSafe)
		{
			::EnterCriticalSection(&this->criticalSection);
		}
	}

	inline void leaveCriticalSectionIfNeeded()
	{
		if constexpr (IsThreadSafe)
		{
			::LeaveCriticalSection(&this->criticalSection);
		}
	}

public:
	/**
		Constructs PointerList object.
		Thread safety is determined at compile time via template parameter.
	*/
	KPointerList()
	{
		resetToSmallBuffer();
		// Critical section initialization is handled by base class constructor
	}

	/**
		Adds new item to the list.
		@returns false if memory allocation failed!
	*/
	bool add(T pointer)
	{
		enterCriticalSectionIfNeeded();

		if (roomCount >= (itemCount + 1)) // no need reallocation. room count is enough!
		{
			list[itemCount] = pointer;
			itemCount++;

			leaveCriticalSectionIfNeeded();
			return true;
		}
		else // require reallocation!
		{
			if (usingSmallBuffer)
			{
				// Switch from small buffer to heap buffer
				roomCount += SmallBufferSize;
				T* newList = (T*)::malloc(roomCount * sizeof(T));

				if (newList)
				{
					// Copy from small buffer to heap buffer
					::memcpy(newList, smallBuffer, SmallBufferSize * sizeof(T));

					list = newList;
					usingSmallBuffer = false;

					list[itemCount] = pointer;
					itemCount++;

					leaveCriticalSectionIfNeeded();
					return true;
				}
				else // memory allocation failed!
				{
					roomCount -= SmallBufferSize;
					leaveCriticalSectionIfNeeded();
					return false;
				}
			}
			else
			{
				// Already using heap buffer, just reallocate
				roomCount += SmallBufferSize;
				void* retVal = ::realloc((void*)list, roomCount * sizeof(T));
				if (retVal)
				{
					list = (T*)retVal;
					list[itemCount] = pointer;
					itemCount++;

					leaveCriticalSectionIfNeeded();
					return true;
				}
				else // memory allocation failed!
				{
					roomCount -= SmallBufferSize;
					leaveCriticalSectionIfNeeded();
					return false;
				}
			}
		}
	}

	/**
		Get pointer at index.
		@returns 0 if index is out of range!
	*/
	T get(const int index)
	{
		enterCriticalSectionIfNeeded();

		if ((0 <= index) && (index < itemCount)) // checks for valid range!
		{
			T object = list[index];
			leaveCriticalSectionIfNeeded();
			return object;
		}
		else // out of range!
		{
			leaveCriticalSectionIfNeeded();
			return nullptr;
		}
	}

	/**
		Get pointer at index.
		@returns 0 if index is out of range!
	*/
	T operator[](const int index)
	{
		return get(index);
	}

	/**
		Replace pointer of given index with new pointer
		@returns false if index is out of range!
	*/
	bool set(const int index, T pointer)
	{
		enterCriticalSectionIfNeeded();

		if ((0 <= index) && (index < itemCount))
		{
			list[index] = pointer;
			leaveCriticalSectionIfNeeded();
			return true;
		}
		else // out of range!
		{
			leaveCriticalSectionIfNeeded();
			return false;
		}
	}

	/**
		Remove pointer of given index
		@returns false if index is out of range!
	*/
	bool remove(const int index)
	{
		enterCriticalSectionIfNeeded();

		if ((0 <= index) && (index < itemCount))
		{
			// Shift all elements after 'index' one position to the left
			for (int i = index; i < itemCount - 1; i++)
			{
				list[i] = list[i + 1];
			}
			itemCount--;

			leaveCriticalSectionIfNeeded();
			return true;
		}
		else // out of range!
		{
			leaveCriticalSectionIfNeeded();
			return false;
		}
	}

	bool remove(T pointer)
	{
		enterCriticalSectionIfNeeded();

		bool retVal = false;
		const int index = getIndex(pointer);
		if (index != -1)
			retVal = remove(index);

		leaveCriticalSectionIfNeeded();
		return retVal;
	}

	/**
		Removes all pointers from the list! Falls back to small buffer.
	*/
	void removeAll()
	{
		enterCriticalSectionIfNeeded();

		if (!usingSmallBuffer)
			::free((void*)list);

		resetToSmallBuffer();

		leaveCriticalSectionIfNeeded();
	}

	/**
		Call destructors of all objects which are pointed by pointers in the list.
		Also clears the list. Falls back to small buffer.
	*/
	void deleteAll()
	{
		enterCriticalSectionIfNeeded();

		for (int i = 0; i < itemCount; i++)
		{
			T object = list[i];
			delete object;
		}

		if (!usingSmallBuffer)
			::free((void*)list);

		resetToSmallBuffer();

		leaveCriticalSectionIfNeeded();
	}

	/**
	 * Safely iterate through all pointers in the list with thread synchronization.
	 * The entire iteration is protected by critical section if thread safety is enabled.
	 * @param func Function/lambda to call for each pointer in the list
	*/
	void forEach(std::function<void(T)> func)
	{
		enterCriticalSectionIfNeeded();
		for (int i = 0; i < itemCount; i++) 
		{
			func(list[i]);
		}
		leaveCriticalSectionIfNeeded();
	}

	/**
	 * Safely iterate with index access. Useful when you need the index as well.
	 * @param func Function/lambda that takes (pointer, index) as parameters
	*/
	void forEachWithIndex(std::function<void(T, int)> func)
	{
		enterCriticalSectionIfNeeded();

		for (int i = 0; i < itemCount; i++)
		{
			func(list[i], i);
		}

		leaveCriticalSectionIfNeeded();
	}

	/**
	 * Safely iterate with early termination support.
	 * @param func Function/lambda that returns bool (true = continue, false = stop)
	 * @returns true if iteration completed, false if stopped early
	 * Example: Early termination (find first disabled button)
		KButton* foundBtn = nullptr;
		buttonList.forEachUntil([&foundBtn](KButton* btn) -> bool {
			if (btn && !btn->isEnabled()) {
				foundBtn = btn;
				return false; // Stop iteration
			}
			return true; // Continue
		});
	*/
	bool forEachUntil(std::function<bool(T)> func)
	{
		enterCriticalSectionIfNeeded();

		bool completed = true;
		for (int i = 0; i < itemCount; i++)
		{
			if (!func(list[i]))
			{
				completed = false;
				break;
			}
		}

		leaveCriticalSectionIfNeeded();
		return completed;
	}

	/**
		Finds the index of the first pointer which matches the pointer passed in.
		@returns -1 if not found!
	*/
	int getIndex(T pointer)
	{
		enterCriticalSectionIfNeeded();

		for (int i = 0; i < itemCount; i++)
		{
			if (list[i] == pointer)
			{
				leaveCriticalSectionIfNeeded();
				return i;
			}
		}

		leaveCriticalSectionIfNeeded();
		return -1;
	}

	/**
		@returns item count in the list
	*/
	int size()
	{
		return itemCount;
	}

	/**
		@returns whether the list is currently using the small buffer optimization
	*/
	bool isUsingSmallBuffer() const
	{
		return usingSmallBuffer;
	}

	/**
		@returns the size of the small buffer
	*/
	static constexpr int getSmallBufferSize()
	{
		return SmallBufferSize;
	}

	/**
		@returns whether this instance is thread-safe (compile-time constant)
	*/
	static constexpr bool isThreadSafeInstance()
	{
		return IsThreadSafe;
	}

	/** Destructs PointerList object.*/
	~KPointerList()
	{
		if (!usingSmallBuffer)
			::free((void*)list);

		// Critical section cleanup is handled by base class destructor
	}

private:
	RFC_LEAK_DETECTOR(KPointerList)
};

// =========== KFile.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once

#include <shlwapi.h>

// macro to specify file format type in the first 4 bytes of file.
// use with KSettingsReader/Writer classes.
#define KFORMAT_ID(ch4) ((((DWORD)(ch4) & 0xFF) << 24) |     \
	(((DWORD)(ch4)& 0xFF00) << 8) | \
	(((DWORD)(ch4)& 0xFF0000) >> 8) | \
	(((DWORD)(ch4)& 0xFF000000) >> 24))

/**
	Can be use to read/write data from a file easily.
*/
class KFile
{
protected:
	HANDLE fileHandle;
	bool autoCloseHandle;
	DWORD desiredAccess;

public:
	KFile();

	/** 
		Used in file opening, to specify whether to open as read or write or both.
	*/
	enum FileAccessTypes
	{
		KREAD = GENERIC_READ,
		KWRITE = GENERIC_WRITE,
		KBOTH = GENERIC_READ | GENERIC_WRITE,
	};

	/** 
		If the file does not exist, it will be created.
	*/
	KFile(const wchar_t* fileName, DWORD desiredAccess = KFile::KBOTH, bool autoCloseHandle = true);

	/** 
		If the file does not exist, it will be created.
	*/
	bool openFile(const wchar_t* fileName, DWORD desiredAccess = KFile::KBOTH, bool autoCloseHandle = true);

	bool closeFile();

	HANDLE getFileHandle();

	operator HANDLE()const;

	/** 
		fills given buffer and returns number of bytes read.
	*/
	DWORD readFile(void* buffer, DWORD numberOfBytesToRead);

	/** 
		You must free the returned buffer yourself. To get the size of buffer, use getFileSize method. return value will be null on read error.
	*/
	void* readAsData();

	KString readAsString(bool isUnicode = true);

	/**
		returns number of bytes written.
	*/
	DWORD writeFile(const void* buffer, DWORD numberOfBytesToWrite);

	bool writeString(const KString& text, bool isUnicode = true);

	bool setFilePointerToStart();

	/**
		moves file pointer to given distance from "startingPoint".
		"startingPoint" can be FILE_BEGIN, FILE_CURRENT or FILE_END
		"distance" can be negative.
	*/
	bool setFilePointerTo(long distance, DWORD startingPoint = FILE_BEGIN);

	DWORD getFilePointerPosition();

	bool setFilePointerToEnd();

	/**
		returns zero on error
	*/
	DWORD getFileSize();

	static bool deleteFile(const wchar_t* fileName);

	static bool isFileExists(const wchar_t* fileName);

	static bool copyFile(const wchar_t* sourceFileName, const wchar_t* destFileName);

	/**
		returns the file name part of the path.
	*/
	static KString getFileNameFromPath(const wchar_t* path);

	static KString getFileExtension(const wchar_t* path);

	~KFile();

private:
	RFC_LEAK_DETECTOR(KFile)
};



// =========== KFont.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


/**
	Can be use to create system default font or custom font. Once created, you cannot change font properties.
*/
class KFont
{
protected:
	HFONT hFont;
	bool customFont;
	KString fontFace;
	int fontSizeFor96DPI;
	int currentDPI;
	bool isBold;
	bool isItalic;
	bool isUnderline;
	bool isAntiAliased;

public:
	/**
		Constructs default system Font object.
	*/
	KFont();

	/**
		Constructs custom font object.
		sizeFor96DPI = 14
		antiAliased = false, 
		requiredDPI = USER_DEFAULT_SCREEN_DPI
	*/
	KFont(const KString& face, int sizeFor96DPI, bool bold = false,
		bool italic = false, bool underline = false, bool antiAliased = true, int requiredDPI = USER_DEFAULT_SCREEN_DPI);

	// destroys the existing font handle.
	virtual bool load(const KString& face, int sizeFor96DPI, bool bold = false,
		bool italic = false, bool underline = false, bool antiAliased = true, int requiredDPI = USER_DEFAULT_SCREEN_DPI);

	virtual void setDPI(int newDPI);

	/**
		If you want to use system default font, then use this static method. Do not delete the returned instance!
	*/
	static KFont* getDefaultFont();

	virtual bool isDefaultFont();

	/**
		Load a font from a file. loaded font only available to this application.
		make sure to call removePrivateFont when done.
	*/
	static bool loadPrivateFont(const KString& path);

	static void removePrivateFont(const KString& path);

	/**
		Returns font handle.
	*/
	virtual HFONT getFontHandle();

	operator HFONT()const;

	virtual ~KFont();

	// Delete copy/move operations to prevent duplication
	KFont(const KFont&) = delete;
	KFont& operator=(const KFont&) = delete;
	KFont(KFont&&) = delete;
	KFont& operator=(KFont&&) = delete;

private:
	RFC_LEAK_DETECTOR(KFont)
};


// =========== KProperty.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <functional>

// assignment is not thread safe! the thread which is assigning will run callbacks.
template<typename T>
class KProperty
{
public:
    std::function<void(const T& oldValue, const T& newValue)> onChangeWithOldValue;
    std::function<void(const T& newValue)> onChange;

    KProperty() = default;

    // Perfect forwarding constructor
    template<typename U, typename = std::enable_if_t<!std::is_same_v<std::decay_t<U>, KProperty>>>
    KProperty(U&& initial) : value(std::forward<U>(initial)) {}

    // Copy assignment operator
    KProperty& operator=(const T& newValue)
    {
        if (value != newValue) // only trigger if different
        {
            if (onChange)
            {
                value = newValue;
                onChange(value);
            }
            else if (onChangeWithOldValue)
            {
                T oldValue = value;
                value = newValue;
                onChangeWithOldValue(oldValue, value);
            }
        }
        return *this;
    }

    // Move assignment operator
    KProperty& operator=(T&& newValue)
    {
        if (value != newValue) // only trigger if different
        {
            if (onChange)
            {
                value = std::move(newValue);
                onChange(value);
            }
            else if (onChangeWithOldValue)
            {
                T oldValue = std::move(value);
                value = std::move(newValue);
                onChangeWithOldValue(oldValue, value);
            }
        }
        return *this;
    }

    // Copy version of setSilent
    void setSilent(const T& newValue)
    {
        value = newValue;
    }

    // Move version of setSilent
    void setSilent(T&& newValue)
    {
        value = std::move(newValue);
    }

    // Get current value
    const T& get() const { return value; }
    operator const T& () const { return value; } // implicit conversion

private:
    T value{};
};

// =========== KBitmap.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


/**
	Can be use to load bitmap image from file or resource.
*/
class KBitmap
{
protected:
	HBITMAP hBitmap;

public:
	KBitmap();

	/**
		Loads bitmap image from resource
		@param resourceID resource ID of image
		@returns false if image load fails
	*/
	bool loadFromResource(WORD resourceID);

	/**
		Loads bitmap image from file
		@param filePath path to image
		@returns false if image load fails
	*/
	bool loadFromFile(const KString& filePath);

	// does not scale
	void drawOnHDC(HDC hdc, int x, int y, int width, int height);

	/**
		Returns bitmap handle
	*/
	HBITMAP getHandle();

	/**
		Returns bitmap handle
	*/
	operator HBITMAP()const;

	virtual ~KBitmap();

private:
	RFC_LEAK_DETECTOR(KBitmap)
};




// =========== KFixedStack.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once


template<typename T, size_t MaxSize>
class KFixedStack
{
private:
    T data[MaxSize];
    size_t count = 0;

public:
    // Push element, returns false if stack is full
    bool push(const T& item)
    {
        if (count >= MaxSize) {
            K_ASSERT(false, "KFixedStack overflow!");
            return false; // Stack overflow
        }
        data[count++] = item;
        return true;
    }

    // Pop element, returns false if stack is empty
    bool pop()
    {
        if (count == 0) {
            K_ASSERT(false, "KFixedStack underflow!");
            return false; // Stack underflow
        }
        --count;
        return true;
    }

    // Get top element, returns pointer (nullptr if empty)
    const T* top() const
    {
        return (count > 0) ? &data[count - 1] : nullptr;
    }

    // Non-const version
    T* top()
    {
        return (count > 0) ? &data[count - 1] : nullptr;
    }

    // Check if empty
    bool isEmpty() const
    {
        return count == 0;
    }

    // Get current size
    size_t size() const
    {
        return count;
    }

    // Check if full
    bool isFull() const
    {
        return count >= MaxSize;
    }

    // Clear all elements
    void clear()
    {
        count = 0;
    }
};

// =========== KSVGImage.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.

*/

#pragma once


// uses modified plutosvg. (removed font support, image element support & image saving)
class KSVGImage
{
protected:
	plutosvg::plutosvg_document_t* document = nullptr;
	plutosvg::plutovg_surface_t* surface = nullptr;

public:
	KSVGImage() {}

	bool loadFromFile(const char* filePath)
	{
		document = plutosvg::plutosvg_document_load_from_file(filePath, -1, -1);
		return document != NULL;
	}

	bool loadFromData(const char* data, int length)
	{
		document = plutosvg::plutosvg_document_load_from_data(data, length, -1, -1, 0, 0);
		return document != NULL;
	}

	// can call multiple times
	void rasterize(int width, int height)
	{
		if (document == NULL)
			return;

		if (surface)
		{
			plutosvg::plutovg_surface_destroy(surface);
			surface = nullptr;
		}

		surface = plutosvg::plutosvg_document_render_to_surface(document, nullptr, width, height, nullptr, nullptr, nullptr);
	}

	// can call multiple times
	void rasterize(int newDPI = USER_DEFAULT_SCREEN_DPI)
	{
		if (document == NULL)
			return;

		unsigned int width = (unsigned int)plutosvg::plutosvg_document_get_width(document);
		unsigned int height = (unsigned int)plutosvg::plutosvg_document_get_height(document);
		unsigned int newWidth = (unsigned int)MulDiv((uint32_t)width, newDPI, USER_DEFAULT_SCREEN_DPI);
		unsigned int newHeight = (unsigned int)MulDiv((uint32_t)height, newDPI, USER_DEFAULT_SCREEN_DPI);
		this->rasterize(newWidth, newHeight);
	}

	bool isImageRasterized()
	{
		return surface != NULL;
	}

	bool isDocumentLoaded()
	{
		return document != NULL;
	}

	// data format is premultiplied gdi 0xaarrggbb or gdiplus PixelFormat32bppPARGB or 
	// direct2d {DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_PREMULTIPLIED}
	void getImageData(int* width, int* height, unsigned char** data, int* stride)
	{
		if (surface == NULL)
			return;

		if(width)
			*width = surface->width;

		if(height)
			*height = surface->height;

		if(data)
			*data = surface->data;

		if(stride)
			*stride = surface->stride;
	}

	// free the generated pixel data. does not free the document.
	// you need to call rasterize method again to generate the image data.
	void freeRasterData()
	{
		if (surface)
		{
			plutosvg::plutovg_surface_destroy(surface);
			surface = nullptr;
		}
	}

	~KSVGImage()
	{
		if (surface)
			plutosvg::plutovg_surface_destroy(surface);

		if (document)
			plutosvg::plutosvg_document_destroy(document);
	}

private:
	RFC_LEAK_DETECTOR(KSVGImage)
};

// =========== KPointerQueue.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/


#pragma once


#ifndef _KQUEUE_H_
#define _KQUEUE_H_

template<class T>
class KQueueNode {
public:
	T data;
	KQueueNode* next;
};

// Queue implemented using a linked list. Can hold unlimited number of items. (assumes T is a pointer type which is allocated using new)
template<class T, bool IsThreadSafe>
class KPointerQueue : private KThreadSafetyBase<IsThreadSafe>
{
protected:
	KQueueNode<T>* firstNode;
	KQueueNode<T>* lastNode;

	// Thread safety helper methods
	inline void enterCriticalSectionIfNeeded()
	{
		if constexpr (IsThreadSafe)
		{
			::EnterCriticalSection(&this->criticalSection);
		}
	}

	inline void leaveCriticalSectionIfNeeded()
	{
		if constexpr (IsThreadSafe)
		{
			::LeaveCriticalSection(&this->criticalSection);
		}
	}

public:
	KPointerQueue()
	{
		firstNode = nullptr;
		lastNode = nullptr;
	}

	void push(T value)
	{
		KQueueNode<T>* newNode = new KQueueNode<T>();
		newNode->data = value;
		newNode->next = nullptr;

		enterCriticalSectionIfNeeded();

		if (firstNode == nullptr)
		{
			firstNode = newNode;
			lastNode = newNode;
		}
		else
		{
			lastNode->next = newNode;
			lastNode = newNode;
		}

		leaveCriticalSectionIfNeeded();
	}

	T pop()
	{
		enterCriticalSectionIfNeeded();

		if (firstNode == nullptr)
		{
			leaveCriticalSectionIfNeeded();
			return nullptr;
		}

		T value = firstNode->data;

		// remove the item
		KQueueNode<T>* tmp = firstNode;
		firstNode = firstNode->next;
		if (firstNode == nullptr) // we had only one item
			lastNode = nullptr;

		delete tmp;

		leaveCriticalSectionIfNeeded();
		return value;
	}

	// calls desctructor of all the T objects in the queue. also clear the queue.
	void deleteAllObjects()
	{
		enterCriticalSectionIfNeeded();

		if (firstNode == nullptr)
		{
			leaveCriticalSectionIfNeeded();
			return;
		}

		KQueueNode<T>* nextNode = firstNode;
		while (nextNode)
		{
			KQueueNode<T>* tmp = nextNode;
			nextNode = nextNode->next;

			delete tmp->data;
			delete tmp;
		}

		firstNode = nullptr;
		lastNode = nullptr;

		leaveCriticalSectionIfNeeded();
	}

	~KPointerQueue()
	{
		// delete all nodes

		if (firstNode == nullptr)
			return;

		KQueueNode<T>* nextNode = firstNode;
		while (nextNode)
		{
			KQueueNode<T>* tmp = nextNode;
			nextNode = nextNode->next;
			delete tmp;
		}
	}
};

#endif

// =========== KSettingsReader.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


/**
	High performance configuration reading class.
*/
class KSettingsReader
{
protected:
	KFile settingsFile;

public:
	KSettingsReader();

	bool openFile(const wchar_t* fileName, int formatID);

	/**
		read struct, array or whatever...
	*/
	void readData(DWORD size, void *buffer);

	KString readString();

	int readInt();

	float readFloat();

	double readDouble();

	bool readBool();

	~KSettingsReader();

private:
	RFC_LEAK_DETECTOR(KSettingsReader)
};


// =========== KSettingsWriter.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once


/**
	High performance configuration writing class.
*/
class KSettingsWriter
{
protected:
	KFile settingsFile;

public:
	KSettingsWriter();

	bool openFile(const wchar_t* fileName, int formatID);

	/**
		save struct, array or whatever...
	*/
	void writeData(DWORD size, void *buffer);

	void writeString(const KString& text);

	void writeInt(int value);

	void writeFloat(float value);

	void writeDouble(double value);

	void writeBool(bool value);

	~KSettingsWriter();

private:
	RFC_LEAK_DETECTOR(KSettingsWriter)
};


// =========== KMenu.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


class KMenu
{
protected:
	HMENU hMenu;

public:
	KMenu();

	virtual void addMenuItem(KMenuItem* menuItem);

	virtual void addSubMenu(const KString& text, KMenu* menu);

	virtual void addSeperator();

	virtual HMENU getMenuHandle();

	// set bringWindowToForeground when showing popup menu for notify icon(systray).
	// does not return until the menu close.
	virtual void popUpMenu(HWND window, bool bringWindowToForeground = false);

	virtual ~KMenu();

private:
	RFC_LEAK_DETECTOR(KMenu)
};


// =========== KVector.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <malloc.h>
#include <utility>

/**
	Holds a resizable list of classes with small buffer optimization.
	item removing can be expensive if T's move is expensive.
	Thread safety is determined at compile time via template parameter.
	Duplicated items allowed!
	index is between 0 to (item count-1)

	@param T The class type to store. T should implement copy/move constructor, (move)assign & compare operators.
	@param SmallBufferSize Number of items to store in stack buffer before allocating heap memory
	@param IsThreadSafe Compile-time flag for thread safety

	e.g. @code
	KString str1, str2;
	KVector<KString, 8, false> strList; // 8 items in small buffer, not thread safe
	strList.add(str1);
	strList.add(str2);
	@endcode
*/
template<class T, int SmallBufferSize, bool IsThreadSafe>
class KVector : private KThreadSafetyBase<IsThreadSafe>
{
protected:
	int itemCount; // current element count in the list
	int roomCount; // maximum element count
	T* list;
	T smallBuffer[SmallBufferSize]; // Stack-allocated small buffer
	bool usingSmallBuffer;

	void resetToSmallBuffer()
	{
		usingSmallBuffer = true;
		list = smallBuffer;
		roomCount = SmallBufferSize;
		itemCount = 0;
	}

	// Thread safety helper methods
	inline void enterCriticalSectionIfNeeded()
	{
		if constexpr (IsThreadSafe)
		{
			::EnterCriticalSection(&this->criticalSection);
		}
	}

	inline void leaveCriticalSectionIfNeeded()
	{
		if constexpr (IsThreadSafe)
		{
			::LeaveCriticalSection(&this->criticalSection);
		}
	}

public:
	/**
		Constructs KVector object.
		Thread safety is determined at compile time via template parameter.
	*/
	KVector()
	{
		resetToSmallBuffer();
		// Critical section initialization is handled by base class constructor
	}

	/**
		Copy constructor
	*/
	KVector(const KVector& other)
	{
		resetToSmallBuffer();

		// If other has more items than our small buffer can hold
		if (other.itemCount > SmallBufferSize)
		{
			roomCount = other.roomCount;
			list = new T[roomCount];
			usingSmallBuffer = false;
		}

		// Copy items
		itemCount = other.itemCount;
		for (int i = 0; i < itemCount; i++)
		{
			list[i] = other.list[i];
		}
	}

	/**
		Assignment operator
	*/
	KVector& operator=(const KVector& other)
	{
		if (this == &other)
			return *this;

		enterCriticalSectionIfNeeded();

		// Clean up current data
		if (!usingSmallBuffer)
		{
			delete[] list;
		}

		resetToSmallBuffer();

		// If other has more items than our small buffer can hold
		if (other.itemCount > SmallBufferSize)
		{
			roomCount = other.roomCount;
			list = new T[roomCount];
			usingSmallBuffer = false;
		}

		// Copy items
		itemCount = other.itemCount;
		for (int i = 0; i < itemCount; i++)
		{
			list[i] = other.list[i]; // copy
		}

		leaveCriticalSectionIfNeeded();
		return *this;
	}

	/**
		Adds new item to the list.
		@returns false if memory allocation failed!
	*/
	bool add(const T& item)
	{
		enterCriticalSectionIfNeeded();

		if (roomCount >= (itemCount + 1)) // no need reallocation. room count is enough!
		{
			list[itemCount] = item; // copy
			itemCount++;

			leaveCriticalSectionIfNeeded();
			return true;
		}
		else // require reallocation!
		{
			int newRoomCount = roomCount + SmallBufferSize;
			T* newList = new T[newRoomCount];

			// Copy existing items to new buffer
			for (int i = 0; i < itemCount; i++)
			{
				newList[i] = std::move(list[i]);
			}

			// Add the new item
			newList[itemCount] = item;
			itemCount++;

			// Free old buffer if it was heap allocated
			if (!usingSmallBuffer)
				delete[] list;

			// Update to use new buffer
			list = newList;
			roomCount = newRoomCount;
			usingSmallBuffer = false;

			leaveCriticalSectionIfNeeded();
			return true;
		}
	}

	T get(const int index)
	{
		enterCriticalSectionIfNeeded();

		if ((0 <= index) && (index < itemCount)) // checks for valid range!
		{
			T object(list[index]);
			leaveCriticalSectionIfNeeded();
			return object;
		}
		else // out of range!
		{
			leaveCriticalSectionIfNeeded();
			return T();
		}
	}

	// avoids extra copy
	bool get(const int index, T& outItem)
	{
		enterCriticalSectionIfNeeded();

		if ((0 <= index) && (index < itemCount)) // checks for valid range!
		{
			outItem = list[index];
			leaveCriticalSectionIfNeeded();
			return true;
		}
		else // out of range!
		{
			leaveCriticalSectionIfNeeded();
			outItem = T();
			return false;
		}
	}

	T operator[](const int index)
	{
		return get(index);
	}

	/**
		@returns false if index is out of range!
	*/
	bool set(const int index, const T& item)
	{
		enterCriticalSectionIfNeeded();

		if ((0 <= index) && (index < itemCount))
		{
			list[index] = item;
			leaveCriticalSectionIfNeeded();
			return true;
		}
		else // out of range!
		{
			leaveCriticalSectionIfNeeded();
			return false;
		}
	}

	/**
		Remove item of given index
		@returns false if index is out of range!
	*/
	bool remove(const int index)
	{
		enterCriticalSectionIfNeeded();

		if ((0 <= index) && (index < itemCount))
		{
			// Shift all elements after 'index' one position to the left
			for (int i = index; i < itemCount - 1; i++)
			{
				list[i] = std::move(list[i + 1]);
			}
			itemCount--;

			leaveCriticalSectionIfNeeded();
			return true;
		}
		else // out of range!
		{
			leaveCriticalSectionIfNeeded();
			return false;
		}
	}

	bool remove(const T& item)
	{
		enterCriticalSectionIfNeeded();

		bool retVal = false;
		const int index = getIndex(item);
		if (index != -1)
			retVal = remove(index);

		leaveCriticalSectionIfNeeded();
		return retVal;
	}

	/**
		Removes all items from the list! Falls back to small buffer.
	*/
	void removeAll()
	{
		enterCriticalSectionIfNeeded();

		if (!usingSmallBuffer)
			delete[] list;

		// we don't clear smallBuffer. 
		// remaining objects on smallBuffer will be destroyed at destructor or freed when adding new items.

		resetToSmallBuffer();

		leaveCriticalSectionIfNeeded();
	}

	/**
		Finds the index of the first item which matches the item passed in.
		@returns -1 if not found!
	*/
	int getIndex(const T& item)
	{
		enterCriticalSectionIfNeeded();

		for (int i = 0; i < itemCount; i++)
		{
			if (list[i] == item)
			{
				leaveCriticalSectionIfNeeded();
				return i;
			}
		}

		leaveCriticalSectionIfNeeded();
		return -1;
	}
	/**
		@returns item count in the list
	*/
	int size() const
	{
		return itemCount;
	}

	/**
	 * Safely iterate through all items in the list with thread synchronization.
	 * The entire iteration is protected by critical section if thread safety is enabled.
	 * @param func Function/lambda to call for each item in the list
	*/
	void forEach(std::function<void(T&)> func)
	{
		enterCriticalSectionIfNeeded();
		for (int i = 0; i < itemCount; i++)
		{
			func(list[i]);
		}
		leaveCriticalSectionIfNeeded();
	}

	/**
	 * Safely iterate with index access. Useful when you need the index as well.
	 * @param func Function/lambda that takes (item, index) as parameters
	*/
	void forEachWithIndex(std::function<void(T&, int)> func)
	{
		enterCriticalSectionIfNeeded();

		for (int i = 0; i < itemCount; i++)
		{
			func(list[i], i);
		}

		leaveCriticalSectionIfNeeded();
	}

	/**
	 * Safely iterate with early termination support.
	 * @param func Function/lambda that returns bool (true = continue, false = stop)
	 * @returns true if iteration completed, false if stopped early
	*/
	bool forEachUntil(std::function<bool(T&)> func)
	{
		enterCriticalSectionIfNeeded();

		bool completed = true;
		for (int i = 0; i < itemCount; i++)
		{
			if (!func(list[i]))
			{
				completed = false;
				break;
			}
		}

		leaveCriticalSectionIfNeeded();
		return completed;
	}

	/**
		@returns whether the list is currently using the small buffer optimization
	*/
	bool isUsingSmallBuffer() const
	{
		return usingSmallBuffer;
	}

	/**
		@returns the size of the small buffer
	*/
	static constexpr int getSmallBufferSize()
	{
		return SmallBufferSize;
	}

	/**
		@returns whether this instance is thread-safe (compile-time constant)
	*/
	static constexpr bool isThreadSafeInstance()
	{
		return IsThreadSafe;
	}

	/** Destructs KVector object.*/
	~KVector()
	{
		if (!usingSmallBuffer)
			delete[] list;

		// Critical section cleanup is handled by base class destructor
	}

private:
	RFC_LEAK_DETECTOR(KVector)
};

// =========== KComponent.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


/**
	Base class of all W32 gui objects.
*/
class KComponent
{
protected:
	KString compClassName;
	KString compText;
	HWND compHWND;
	HWND compParentHWND;
	DWORD compDwStyle;
	DWORD compDwExStyle;
	UINT compCtlID;
	int compX;
	int compY;
	int compWidth;
	int compHeight;
	int compDPI;
	bool compVisible;
	bool compEnabled;
	bool isRegistered;
	KFont *compFont;
	KCursor *cursor;

public:
	WNDCLASSEXW wc;

	/**
		Constructs a standard win32 component.
		@param generateWindowClassDetails	set to false if you are not registering window class and using standard class name like BUTTON, STATIC etc... wc member is invalid if generateWindowClassDetails is false.
	*/
	KComponent(bool generateWindowClassDetails);
	
	/**
		Returns HWND of this component
	*/
	operator HWND()const;

	/**
		Called after hotplugged into a given HWND.
	*/
	virtual void onHotPlug();

	/**
		HotPlugs given HWND. this method does not update current compFont and cursor variables.
		Set fetchInfo to true if you want to acquire all the information about this HWND. (width, height, position etc...)
		Set fetchInfo to false if you just need to receive events. (button click etc...)
	*/
	virtual void hotPlugInto(HWND component, bool fetchInfo = true);

	/**
		Sets mouse cursor of this component.
	*/
	virtual void setMouseCursor(KCursor* cursor);

	/**
		@returns autogenerated unique class name for this component
	*/
	virtual KString getComponentClassName();

	/**
		Registers the class name and creates the component. 
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
		@returns false if registration failed or component creation failed.
	*/
	virtual bool create(bool requireInitialMessages = false);

	// called when parent receive WM_DESTROY message.
	virtual void onParentDestroy();

	virtual void destroy();

	/**
		Handles internal window messages. (subclassed window proc)
		Important: Pass unprocessed messages to parent if you override this method.
	*/
	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	/**
		Receives messages like WM_COMMAND, WM_NOTIFY, WM_DRAWITEM from the parent window. (if it belongs to this component)
		Pass unprocessed messages to parent if you override this method.
		@returns true if msg processed.
	*/
	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result);

	/**
		Identifier of the child component which can be used with WM_MEASUREITEM like messages.
		@returns zero for top level windows
	*/
	virtual UINT getControlID();

	/**
		Sets font of this component. passed font object must live until this component destroy.
	*/
	virtual void setFont(KFont* compFont);

	/**
		Returns font of this component
	*/
	virtual KFont* getFont();

	/**
		Returns caption of this component
	*/
	virtual KString getText();

	/**
		Sets caption of this component
	*/
	virtual void setText(const KString& compText);

	virtual void setHWND(HWND compHWND);

	/**
		Returns HWND of this component
	*/
	virtual HWND getHWND();

	/**
		Changes parent of this component
	*/
	virtual void setParentHWND(HWND compParentHWND);

	/**
		Returns parent of this component
	*/
	virtual HWND getParentHWND();

	/**
		Returns style of this component
	*/
	virtual DWORD getStyle();

	/**
		Sets style of this component
	*/
	virtual void setStyle(DWORD compStyle);

	/**
		Returns exstyle of this component
	*/
	virtual DWORD getExStyle();

	/**
		Sets exstyle of this component
	*/
	virtual void setExStyle(DWORD compExStyle);

	/**
		Returns x position of this component which is relative to parent component.
	*/
	virtual int getX();

	/**
		Returns y position of this component which is relative to parent component.
	*/
	virtual int getY();

	/**
		Returns width of the component.
	*/
	virtual int getWidth();

	/**
		Returns height of the component.
	*/
	virtual int getHeight();

	virtual int getDPI();

	virtual int scaleToCurrentDPI(int valueFor96DPI);

	/**
		Sets width and height of the component.
	*/
	virtual void setSize(int compWidth, int compHeight);

	/**
		Sets x and y position of the component. x and y are relative to parent component
	*/
	virtual void setPosition(int compX, int compY);

	// --- Positioning relative to another component ---
	// The width and height of both this component and the target must be set before calling these methods.
	virtual void placeToRightOf(KComponent& target, int spacing = 10);
	virtual void placeToLeftOf(KComponent& target, int spacing = 10);
	virtual void placeBelow(KComponent& target, int spacing = 10);
	virtual void placeAbove(KComponent& target, int spacing = 10);

	// --- Alignments with another component ---
	// The width and height of both this component and the target must be set before calling these methods.
	virtual void alignTopWith(KComponent& target);
	virtual void alignBottomWith(KComponent& target);
	virtual void alignLeftWith(KComponent& target);
	virtual void alignRightWith(KComponent& target);
	virtual void alignCenterHorizontallyWith(KComponent& target);
	virtual void alignCenterVerticallyWith(KComponent& target);

	virtual void setDPI(int newDPI);

	/**
		Sets visible state of the component
	*/
	virtual void setVisible(bool state);

	/**
		Returns visible state of the component
	*/
	virtual bool isVisible();

	/**
		Returns the component is ready for user input or not
	*/
	virtual bool isEnabled();

	/**
		Sets component's user input reading state
	*/
	virtual void setEnabled(bool state);

	/**
		Brings component to front
	*/
	virtual void bringToFront();

	/**
		Grabs keyboard focus into this component
	*/
	virtual void setKeyboardFocus();

	/**
		Repaints the component
	*/
	virtual void repaint();

	virtual ~KComponent();

private:
	RFC_LEAK_DETECTOR(KComponent)
};


// macros to handle window messages

#define BEGIN_KMSG_HANDLER \
	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override \
	{\
	switch(msg)\
	{

#define ON_KMSG(_KMsg,_KMsgHandler) \
	case _KMsg: return _KMsgHandler(wParam,lParam);

// msvc & clang supports __super keyword

#define END_KMSG_HANDLER \
	default: return __super::windowProc(hwnd,msg,wParam,lParam); \
	}\
	}


// =========== UtilsModule.h ===========

/*
    Copyright (C) 2013-2025 CrownSoft

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once


#pragma comment(lib, "Rpcrt4.lib")

// =========== ContainersModule.h ===========

/*
    Copyright (C) 2013-2025 CrownSoft

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once


// =========== KGridView.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once

#include <functional>

class KGridView : public KComponent
{
protected:
	int colCount;
	int itemCount;

public:
	std::function<void(KGridView*)> onItemSelect;
	std::function<void(KGridView*)> onItemRightClick;
	std::function<void(KGridView*)> onItemDoubleClick;

	KGridView(bool sortItems = false);

	virtual void insertRecord(KString** columnsData);

	virtual void insertRecordTo(int rowIndex, KString** columnsData);

	virtual KString getRecordAt(int rowIndex, int columnIndex);

	/**
		returns -1 if nothing selected.
	*/
	virtual int getSelectedRow();

	virtual void removeRecordAt(int rowIndex);

	virtual void removeAll();

	virtual void updateRecordAt(int rowIndex, int columnIndex, const KString& text);

	virtual void setColumnWidth(int columnIndex, int columnWidth);

	virtual int getColumnWidth(int columnIndex);

	virtual void createColumn(const KString& text, int columnWidth = 100);

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) override;

	virtual bool create(bool requireInitialMessages = false) override;

	virtual void _onItemSelect();

	virtual void _onItemRightClick();

	virtual void _onItemDoubleClick();

	virtual ~KGridView();
};


// =========== KLabel.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


class KLabel : public KComponent
{
protected:
	bool autoResize;

	virtual void resizeToTextSize();
	const int AUTOSIZE_EXTRA_GAP = 10;
public:
	KLabel();

	virtual bool create(bool requireInitialMessages = false) override;
	virtual void enableAutoResize(bool enable);
	virtual void setText(const KString& compText) override;
	virtual void setFont(KFont* compFont) override;
	virtual void setDPI(int newDPI) override;

	virtual ~KLabel();
};


// =========== KProgressBar.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once


class KProgressBar : public KComponent
{
protected:
	int value;

public:
	KProgressBar(bool smooth=true, bool vertical=false);

	virtual int getValue();

	virtual void setValue(int value);

	virtual bool create(bool requireInitialMessages = false) override;

	virtual ~KProgressBar();
};


// =========== KTrackBar.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once

#include <functional>

class KTrackBar : public KComponent
{
protected:
	int rangeMin,rangeMax,value;

public:
	std::function<void(KTrackBar*,int)> onChange;

	KTrackBar(bool showTicks = false, bool vertical = false);

	/**
		Range between 0 to 100
	*/
	virtual void setRange(int min, int max);

	virtual void setValue(int value);

	virtual int getValue();

	virtual void _onChange();

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) override;

	virtual bool create(bool requireInitialMessages = false) override;

	virtual ~KTrackBar();
};




// =========== SVGModule.h ===========

/*
    Copyright (C) 2013-2025 CrownSoft

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once


// =========== KTextBox.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once


class KTextBox : public KComponent
{
public:
	KTextBox(bool readOnly = false);

	virtual KString getText() override;

	virtual bool create(bool requireInitialMessages = false) override;

	virtual ~KTextBox();
};


// =========== KPropertyStorage.h ===========

/*
    Copyright (C) 2013-2025 CrownSoft

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

// Readonly data storage model. Use Property Storage - Editor tool to write.
// Can be used when there is no order of storing data objects.

#pragma once

#include <windows.h>

#pragma comment(lib, "Rpcrt4.lib")

#define PS_V1_HEADER    { 'P','S','0','1'}

namespace KPSPropertyTypes
{
    enum KPSPropertyTypes
    {
        STRING = 0,
        INTEGER = 1,
        DWORD = 2,
        FLOAT = 3,
        INT_ARRAY = 4,
        GUID = 5,
        FILE = 6
    };
}

class KPSProperty
{
public:
    // ======== stuff written to file ============
    wchar_t* name;
    int nameLength;

    int type; // KPSPropertyTypes

    wchar_t* strValue;
    int strValueLength;

    int intValue;
    DWORD dwordValue;
    float floatValue;

    int* intArray;
    int intArraySize;

    GUID guidValue;

    wchar_t* fileName;
    int fileNameLength;
    unsigned char* fileData;
    DWORD fileDataSize;
    // ============================================

    KPSProperty()
    {
        name = NULL;
        strValue = NULL;
        fileName = NULL;
        fileData = NULL;
        intArray = NULL;
        
        nameLength = 0;
        strValueLength = 0;
        type = KPSPropertyTypes::INTEGER;
        intValue = 0;
        dwordValue = 0;
        floatValue = 0.0f;
        fileNameLength = 0;
        fileDataSize = 0;
        intArraySize = 0;
    }

    virtual ~KPSProperty()
    {
        if (name)
            ::free(name);
        if (strValue)
            ::free(strValue);
        if (intArray)
            ::free(intArray);
        if (fileName)
            ::free(fileName);
        if (fileData)
            ::free(fileData);
    }
};

class KPSObject
{
public:
    // ======== stuff written to file ============
    wchar_t* name;
    int nameLength;
    GUID objectID;
    // ============================================

    KPointerList<KPSProperty*, 32, false> propertyList;

    KPSObject()
    {
        name = NULL;
        nameLength = 0;     
    }

    bool compare(const GUID& objectID)
    {
        return (::IsEqualGUID(objectID, this->objectID) == TRUE);
    }

    virtual ~KPSObject()
    {
        if (name)
            ::free(name);

        propertyList.deleteAll();
    }
};

class KPSPropertyView : public KPSProperty {

public:
    wchar_t* intArrayStr;
    wchar_t* guidValueStr;

    KPSObject* parentObject;

    KPSPropertyView()
    {
        parentObject = NULL;
        guidValueStr = NULL;
        intArrayStr = NULL;
    }

    virtual ~KPSPropertyView()
    {
        if (intArrayStr)
            ::free(intArrayStr);
        if (guidValueStr)
            ::RpcStringFreeW((RPC_WSTR*)&guidValueStr);
    }

    void generateIntArrayString()
    {
        if (intArrayStr)
            ::free(intArrayStr);

        intArrayStr = NULL;

        if (intArraySize == 0)
            return;

        intArrayStr = (wchar_t*)::malloc(sizeof(wchar_t) * (16 + 1) * intArraySize);
        intArrayStr[0] = 0;

        for (int i = 0; i < intArraySize; i++)
        {
            wchar_t buffer[16];
            ::_itow_s(intArray[i], buffer, 16, 10);

            ::wcscat_s(intArrayStr, 16, buffer);

            if (i != (intArraySize - 1)) // not the last one
                ::wcscat_s(intArrayStr, 16, L",");
        }
    }

    void generateIntArrayByString(wchar_t* text)
    {
        if (intArray)
            ::free(intArray);

        intArray = NULL;
        intArraySize = 0;

        int textLength = (int)::wcslen(text);
        int commaCount = 0;

        for (int i = 0; i < textLength; i++)
        {
            if (text[i] == L',')
                commaCount++;
        }

        intArray = (int*)::malloc(sizeof(int) * (commaCount + 1));

        wchar_t* str = text;
        wchar_t* end = str;

        while (*end) {
            int n = ::wcstol(str, &end, 10);
            intArray[intArraySize] = n;
            intArraySize++;

            if (intArraySize == (commaCount + 1))
                break;

            // skip non numeric characters
            while ((*end != 0) && ((*end < L'0') || (*end > L'9'))) {
                end++;
            }

            str = end;
        }

        generateIntArrayString();
    }

    void generateGUIDValueString()
    {
        if (guidValueStr)
            ::RpcStringFreeW((RPC_WSTR*)&guidValueStr);

        guidValueStr = NULL;

        ::UuidToStringW(&guidValue, (RPC_WSTR*)&guidValueStr);
    }

    // The string should be in the following form
    // 00000000-0000-0000-0000-000000000000
    bool generateGUIDValueByString(const wchar_t* text)
    {
        bool success = true;

        if (::UuidFromStringW((RPC_WSTR)text, &guidValue) != RPC_S_OK)
        {
            ::UuidFromStringW((RPC_WSTR)L"00000000-0000-0000-0000-000000000000", &guidValue); // default value
            success = false;
        }

        generateGUIDValueString();
        return success;
    }
};

class KPSObjectView : public KPSObject
{
public:
    wchar_t* objectIDStr; 

    KPSObjectView() : KPSObject()
    {
        objectIDStr = NULL;
    }

    void generateObjectID()
    {
        ::CoCreateGuid(&objectID);
    }

    void generateObjectIDString()
    {
        if (objectIDStr)
            ::RpcStringFreeW((RPC_WSTR*)&objectIDStr);

        objectIDStr = nullptr;

        ::UuidToStringW(&objectID, (RPC_WSTR*)&objectIDStr);
    }

    // The string should be in the following form
    // 00000000-0000-0000-0000-000000000000
    bool generateIDByString(const wchar_t* text)
    {
        if (::UuidFromStringW((RPC_WSTR)text, &objectID) != RPC_S_OK)
            return false;

        generateObjectIDString();
        return true;
    }

    virtual ~KPSObjectView()
    {
        if (objectIDStr)
            ::RpcStringFreeW((RPC_WSTR*)&objectIDStr);
    }
};

class KPSReader
{
protected:

public:
    KPointerList<KPSObject*,16, false>* psObjectList;

    KPSReader()
    {
        psObjectList = NULL;
    }

    // do not free returned object.
    KPSObject* getPSObject(const GUID& objectID)
    {
        for (int i = 0; i < psObjectList->size(); i++)
        {
            KPSObject* psObject = psObjectList->get(i);
            if (psObject->compare(objectID))
                return psObject;
        }

        return NULL;
    }

    bool loadFromFile(const wchar_t* path, bool readNames = true)
    {
        if (psObjectList)
        {
            psObjectList->deleteAll();
            delete psObjectList;
            psObjectList = NULL;
        }

        HANDLE fileHandle = ::CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

        if (fileHandle == INVALID_HANDLE_VALUE)
            return false;

        DWORD fileSize = ::GetFileSize(fileHandle, NULL);
        fileSize = (fileSize == INVALID_FILE_SIZE) ? 0 : fileSize;

        if (fileSize == 0)
        {
            ::CloseHandle(fileHandle);
            return false;
        }

        char fileHeader[4];
        DWORD bytesRead;
        ::ReadFile(fileHandle, fileHeader, 4, &bytesRead, NULL);

        char psFileHeader[4] = PS_V1_HEADER;

        for (int i = 0; i < 4; ++i)
        {
            if (psFileHeader[i] != fileHeader[i])
            {
                ::CloseHandle(fileHandle);
                return false;
            }
        }

        unsigned int objectCount = 0;
        ::ReadFile(fileHandle, &objectCount, sizeof(unsigned int), &bytesRead, NULL);

        if (objectCount == 0)
        {
            ::CloseHandle(fileHandle);
            return false;
        }

        psObjectList = new KPointerList<KPSObject*, 16, false>();

        for (unsigned int objectIndex = 0; objectIndex < objectCount; ++objectIndex)
        {
            GUID objectID;
            ::ReadFile(fileHandle, &objectID, sizeof(GUID), &bytesRead, NULL);

            int nameLength;
            ::ReadFile(fileHandle, &nameLength, sizeof(int), &bytesRead, NULL);

            wchar_t* objectName = NULL;
            if (readNames)
            {
                objectName = (wchar_t*)::malloc(sizeof(wchar_t) * (nameLength + 1));
                ::ReadFile(fileHandle, objectName, sizeof(wchar_t) * nameLength, &bytesRead, NULL);
                objectName[nameLength] = 0;
            }
            else // ignore name
            {
                ::SetFilePointer(fileHandle, nameLength * sizeof(wchar_t), 0, FILE_CURRENT); 
            }

            unsigned int propertyCount;
            ::ReadFile(fileHandle, &propertyCount, sizeof(unsigned int), &bytesRead, NULL);


            if (propertyCount == 0) // ignore the objects which doesn't have properties.
            {
                if (objectName)
                    ::free(objectName);

                continue;
            }

            KPSObject* psObject = new KPSObject();
            psObject->objectID = objectID;
            psObject->nameLength = nameLength;
            psObject->name = objectName;

            for (unsigned int propertyIndex = 0; propertyIndex < propertyCount; ++propertyIndex)
            {
                KPSProperty* psProperty = new KPSProperty();

                ::ReadFile(fileHandle, &psProperty->nameLength, sizeof(int), &bytesRead, NULL);

                if (readNames)
                {
                    psProperty->name = (wchar_t*)::malloc(sizeof(wchar_t) * (psProperty->nameLength + 1));
                    ::ReadFile(fileHandle, psProperty->name, sizeof(wchar_t) * psProperty->nameLength, &bytesRead, NULL);
                    psProperty->name[psProperty->nameLength] = 0;
                }
                else // ignore name
                {
                    ::SetFilePointer(fileHandle, psProperty->nameLength * sizeof(wchar_t), 0, FILE_CURRENT);
                }

                ::ReadFile(fileHandle, &psProperty->type, sizeof(int), &bytesRead, NULL);

                if (psProperty->type == KPSPropertyTypes::STRING) // string
                {
                    ::ReadFile(fileHandle, &psProperty->strValueLength, sizeof(int), &bytesRead, NULL);
                    psProperty->strValue = (wchar_t*)::malloc(sizeof(wchar_t) * (psProperty->strValueLength + 1));
                    ::ReadFile(fileHandle, psProperty->strValue, sizeof(wchar_t) * psProperty->strValueLength, &bytesRead, NULL);
                    psProperty->strValue[psProperty->strValueLength] = 0;
                }
                else if (psProperty->type == KPSPropertyTypes::INTEGER) // int
                {
                    ::ReadFile(fileHandle, &psProperty->intValue, sizeof(int), &bytesRead, NULL);
                }
                else if (psProperty->type == KPSPropertyTypes::DWORD) // DWORD
                {
                    ::ReadFile(fileHandle, &psProperty->dwordValue, sizeof(DWORD), &bytesRead, NULL);
                }
                else if (psProperty->type == KPSPropertyTypes::FLOAT) // float
                {
                    ::ReadFile(fileHandle, &psProperty->floatValue, sizeof(float), &bytesRead, NULL);
                }
                else if (psProperty->type == KPSPropertyTypes::INT_ARRAY) // int array
                {
                    ::ReadFile(fileHandle, &psProperty->intArraySize, sizeof(int), &bytesRead, NULL);
                    if (psProperty->intArraySize)
                    {
                        psProperty->intArray = (int*)::malloc(sizeof(int) * psProperty->intArraySize);
                        ::ReadFile(fileHandle, psProperty->intArray, sizeof(int) * psProperty->intArraySize, &bytesRead, NULL);
                    }
                }
                else if (psProperty->type == KPSPropertyTypes::GUID) // guid
                {
                    ::ReadFile(fileHandle, &psProperty->guidValue, sizeof(GUID), &bytesRead, NULL);
                }
                else // file
                {
                    ::ReadFile(fileHandle, &psProperty->fileNameLength, sizeof(int), &bytesRead, NULL);
                    psProperty->fileName = (wchar_t*)::malloc(sizeof(wchar_t) * (psProperty->fileNameLength + 1));
                    ::ReadFile(fileHandle, psProperty->fileName, sizeof(wchar_t) * psProperty->fileNameLength, &bytesRead, NULL);
                    psProperty->fileName[psProperty->fileNameLength] = 0;
                    ::ReadFile(fileHandle, &psProperty->fileDataSize, sizeof(DWORD), &bytesRead, NULL);
                    if (psProperty->fileDataSize)
                    {
                        psProperty->fileData = (unsigned char*)::malloc(psProperty->fileDataSize);
                        ::ReadFile(fileHandle, psProperty->fileData, psProperty->fileDataSize, &bytesRead, NULL);
                    }
                }

                psObject->propertyList.add(psProperty);
            }

            psObjectList->add(psObject);
        }

        ::CloseHandle(fileHandle);
        return true;
    }

    virtual ~KPSReader() 
    {
        if (psObjectList)
        {
            psObjectList->deleteAll();
            delete psObjectList;
        }
    }
};


// =========== KButton.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once

#include <functional>

class KButton : public KComponent
{
public:
	std::function<void(KButton*)> onClick;

	KButton();

	virtual void _onPress();

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result) override;

	virtual bool create(bool requireInitialMessages = false) override;

	virtual ~KButton();
};

// =========== KCheckBox.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


class KCheckBox : public KButton
{
protected:
	bool checked;

public:
	KCheckBox();

	virtual bool create(bool requireInitialMessages = false) override;

	virtual void _onPress() override;

	virtual bool isChecked();

	virtual void setCheckedState(bool state);

	virtual ~KCheckBox();
};


// =========== KListBox.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once

#include <functional>

class KListBox : public KComponent
{
protected:
	KVector<KString, 10, false> stringList;
	int selectedItemIndex;
	int selectedItemEnd;
	bool multipleSelection;

public:
	std::function<void(KListBox*)> onItemSelect;
	std::function<void(KListBox*)> onItemRightClick;
	std::function<void(KListBox*)> onItemDoubleClick;

	KListBox(bool multipleSelection=false, bool sort=false, bool vscroll=true);

	virtual void addItem(const KString& text);

	virtual void removeItem(int index);

	virtual void removeItem(const KString& text);

	virtual void updateItem(int index, const KString& text);

	virtual int getItemIndex(const KString& text);

	virtual int getItemCount();

	virtual int getSelectedItemIndex();

	virtual KString getSelectedItem();

	virtual int getSelectedItems(int* itemArray, int itemCountInArray);

	virtual void clearList();

	virtual void selectItem(int index);

	virtual void selectItems(int start, int end);

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) override;

	virtual bool create(bool requireInitialMessages = false) override;

	virtual void _onItemSelect();

	virtual void _onItemDoubleClick();

	virtual void _onItemRightClick();

	virtual ~KListBox();
};


// =========== KDirectory.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution. 
*/

#pragma once


#include <shlobj.h>

/**
Can be use to manipulate dirs.
*/
class KDirectory
{
public:
	KDirectory();

	static bool isDirExists(const KString& dirName);

	/**
		returns false if directory already exists.
	*/
	static bool createDir(const KString& dirName);

	/**
		deletes an existing empty directory.
	*/
	static bool removeDir(const KString& dirName);

	/**
		returns the directory of given module. if HModule is NULL this function will return dir of exe.
		returns empty string on error.
	*/
	static void getModuleDir(HMODULE hModule, wchar_t* outBuffer, int bufferSizeInWChars);

	static void getModuleFilePath(HMODULE hModule, wchar_t* outBuffer, int bufferSizeInWChars);

	/**
		returns the parent directory of given file.
	*/
	static void getParentDir(const wchar_t* filePath, wchar_t* outBuffer, int bufferSizeInWChars);

	/**
		returns the the directory for temporary files.
		returns empty string on error.
	*/
	static void getTempDir(wchar_t* outBuffer, int bufferSizeInWChars);

	/**
		returns the all user data directory. Requires admin priviledges for writing to this dir.
		returns empty string on error.
		outBuffer size must be MAX_PATH
	*/
	static void getAllUserDataDir(wchar_t* outBuffer);

	/*
		known path for the logged in user of the pc. (not affected by right click -> run as admin)
		outBuffer size must be MAX_PATH
		CSIDL_ADMINTOOLS
		CSIDL_APPDATA
		CSIDL_COMMON_ADMINTOOLS
		CSIDL_COMMON_APPDATA
		CSIDL_COMMON_DOCUMENTS
		CSIDL_COOKIES
		CSIDL_FLAG_CREATE
		CSIDL_FLAG_DONT_VERIFY
		CSIDL_HISTORY
		CSIDL_INTERNET_CACHE
		CSIDL_LOCAL_APPDATA
		CSIDL_MYPICTURES
		CSIDL_PERSONAL
		CSIDL_PROGRAM_FILES
		CSIDL_PROGRAM_FILES_COMMON
		CSIDL_SYSTEM
		CSIDL_WINDOWS
	*/
	static void getLoggedInUserFolderPath(int csidl, wchar_t* outBuffer);

	// path for logged in user of pc (not affected by right click -> run as admin)
	// outBuffer size must be MAX_PATH
	static void getRoamingFolder(wchar_t* outBuffer);

	// path for logged in user of pc (not affected by right click -> run as admin)
	// outBuffer size must be MAX_PATH
	static void getNonRoamingFolder(wchar_t* outBuffer);

	// must delete returned strings and list.
	// extension without dot. ex: "mp3"
	// folderPath is without ending slash
	// returns only file names. not full path.
	// does not scan for child folders.
	static KPointerList<KString*, 32, false>* scanFolderForExtension(const KString& folderPath, const KString& extension);

	~KDirectory();

private:
	RFC_LEAK_DETECTOR(KDirectory)
};


// =========== KLogger.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once



/**
	Super fast logging class for logging within a (audio)loop. (not thread safe)
	Writes logging data into ram & dumps data into file when needed.
	You can use this class instead of OutputDebugString API.(OutputDebugString is too slow & ETW is too complex?)
	Use Log Viewer tool to view generated log file.

	Log File Format:
		file header:				'R' 'L' 'O' 'G'
		event count:				int32					; (event count)
		event start packet format:	byte|short16|short16	; (event type|secs|mills)
		event param number format:	byte|data				; (param type|data)
		event param string format:	byte|byte|data			; (param type|data size[max 255]|data)
		event end packet format:	byte					; (EVT_END)
*/
class KLogger
{
protected:
	DWORD bufferSize;
	DWORD bufferIndex;
	unsigned int totalMills;
	unsigned int totalEvents;
	char *buffer;
	bool bufferFull, isFirstCall;
	KPerformanceCounter pCounter;

public:

	enum ByteSizes
	{
		SZ_MEGABYTE = 1024 * 1024,
		SZ_KILOBYTE = 1024,
	};

	enum EventTypes
	{
		EVT_END = 0,
		EVT_INFORMATION = 1,
		EVT_WARNING = 2,
		EVT_ERROR = 3,
	};

	enum ParamTypes
	{
		// skipped value zero. because parser will fail to recognize EVT_END.
		PARAM_STRING = 1,
		PARAM_INT32 = 2,
		PARAM_SHORT16 = 3,
		PARAM_FLOAT = 4,
		PARAM_DOUBLE = 5,
	};

	KLogger(DWORD bufferSize = (SZ_MEGABYTE * 10));

	bool writeNewEvent(unsigned char eventType = EVT_INFORMATION);

	bool endEvent();

	/**
		textLength is number of chars. max value is 255.
	*/
	bool addTextParam(const char *text, unsigned char textLength);

	bool addIntParam(int value);

	bool addShortParam(unsigned short value);

	bool addFloatParam(float value);
	
	bool addDoubleParam(double value);

	bool isBufferFull();

	bool writeToFile(const KString &filePath);

	~KLogger();

private:
	RFC_LEAK_DETECTOR(KLogger)
};


// =========== KComboBox.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once

#include <functional>

class KComboBox : public KComponent
{
protected:
	KVector<KString, 10, false> stringList;
	int selectedItemIndex;

public:
	std::function<void(KComboBox*)> onItemSelect;

	KComboBox(bool sort=false);

	virtual void addItem(const KString& text);

	virtual void removeItem(int index);

	virtual void removeItem(const KString& text);

	virtual int getItemIndex(const KString& text);

	virtual int getItemCount();

	virtual int getSelectedItemIndex();

	virtual KString getSelectedItem();

	virtual void clearList();

	virtual void selectItem(int index);

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *result) override;

	virtual bool create(bool requireInitialMessages = false) override;

	virtual void _onItemSelect();

	virtual ~KComboBox();
};


// =========== KHostPanel.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once


class KHostPanel : public KComponent
{
protected:
	bool enableDPIUnawareMode;
	KPointerList<KComponent*, 24, false>* componentList;

public:
	KHostPanel();

	// called by the parent
	virtual void setComponentList(KPointerList<KComponent*, 24, false>* componentList);

	// called by the parent
	virtual void setEnableDPIUnawareMode(bool enable);

	/**
		add KHostPanel to window(call create) before adding items to it.
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
	*/
	virtual bool addComponent(KComponent* component, bool requireInitialMessages = false);

	// Can be also use to remove a container. Also destroys the hwnd.
	virtual void removeComponent(KComponent* component);

	/**
		add KHostPanel to window(call create) before adding items to it.
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
	*/
	virtual bool addContainer(KHostPanel* container, bool requireInitialMessages = false);

	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	virtual ~KHostPanel();
};

// =========== KMenuButton.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once


class KMenuButton : public KButton
{
protected:
	KFont arrowFont;
	KFont* glyphFont;
	KMenu* buttonMenu;
	const wchar_t* glyphChar;
	COLORREF glyphColor;
	int glyphLeft;

public:
	KMenuButton();

	virtual ~KMenuButton();

	virtual void setMenu(KMenu* buttonMenu);

	/**
		Use character code for glyphChar. ex: "\x36" for down arrow when using Webdings font.
		You can use "Character Map" tool get character codes.
		Default text color will be used if glyphColor not specified.
	*/
	virtual void setGlyph(const wchar_t* glyphChar, KFont* glyphFont, COLORREF glyphColor = ::GetSysColor(COLOR_BTNTEXT), int glyphLeft = 6);

	virtual void setDPI(int newDPI) override;

	virtual void _onPress() override;

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) override;
};


// =========== KGUIProc.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once


// all the methods must be called only from the gui thread.
class KGUIProc
{
public:
	static ATOM atomComponent;
	static ATOM atomOldProc;

	static LRESULT CALLBACK windowProc(HWND, UINT, WPARAM, LPARAM);
	static INT_PTR CALLBACK dialogProc(HWND, UINT, WPARAM, LPARAM);

	/**
		set requireInitialMessages to true to receive initial messages lke WM_CREATE... (installs a hook)
	*/
	static HWND createComponent(KComponent* component, bool requireInitialMessages);

	/**
		hwnd can be window, custom control, dialog or common control.
		hwnd will be subclassed if it is a common control or dialog.
	*/
	static void attachRFCPropertiesToHWND(HWND hwnd, KComponent* component);

	static int hotPlugAndRunDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component);
	static HWND hotPlugAndCreateDialogBox(WORD resourceID, HWND parentHwnd, KComponent* component);
};


// =========== KWindow.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once

#include <functional>

#define RFC_CUSTOM_MESSAGE WM_APP + 100

enum class KCloseOperation { DestroyAndExit, Hide, Nothing };

class KWindow : public KComponent
{
protected:
	HWND lastFocusedChild;
	bool enableDPIUnawareMode;
	KPointerList<KComponent*, 24, false> componentList; // KHostPanel is also using 24.
	KCloseOperation closeOperation;
	DPI_AWARENESS_CONTEXT prevDPIContext;
	bool dpiAwarenessContextChanged;
	KIcon* windowIcon;
	HICON largeIconHandle, smallIconHandle;

	void updateWindowIconForNewDPI();

public:
	std::function<void(KWindow*)> onDPIChange; // called after dpi change.

	KWindow();

	virtual bool create(bool requireInitialMessages = false) override;

	virtual void flash();

	// can only call after create.
	virtual void setIcon(KIcon* icon);

	virtual void setCloseOperation(KCloseOperation closeOperation);

	virtual void onClose();

	virtual void onDestroy();

	// Custom messages are used to send a signal/data from worker thread to gui thread.
	virtual void postCustomMessage(WPARAM msgID, LPARAM param);

	virtual void onCustomMessage(WPARAM msgID, LPARAM param);

	virtual void centerScreen();

	// puts our window on same monitor as given window + centered
	virtual void centerOnSameMonitor(HWND window);

	/**
		Set requireInitialMessages to true to receive initial messages (WM_CREATE etc.)
		Adding a component does not mean that the window will own or delete the component - it's
		your responsibility to delete the component. you need to remove the component if you are
		deleting it before WM_DESTROY message arrived.
.
	*/
	virtual bool addComponent(KComponent* component, bool requireInitialMessages = false);

	bool addComponent(KComponent& component, bool requireInitialMessages = false);

	template<typename... Components>
	void addComponents(bool requireInitialMessages, Components&... comps)
	{
		(addComponent(comps, requireInitialMessages), ...); // fold expression (C++17+)
	}

	template<typename... Components>
	void addComponents(Components&... comps)
	{
		(addComponent(comps, false), ...); // fold expression (C++17+)
	}

	// Can be also use to remove a container. Also destroys the hwnd.
	// you need to remove the component if you are deleting it before WM_DESTROY message arrived.
	virtual void removeComponent(KComponent* component);

	// use this method to add KHostPanel to the window.
	virtual bool addContainer(KHostPanel* container, bool requireInitialMessages = false);

	virtual bool setClientAreaSize(int width, int height);

	// Mixed-Mode DPI Scaling - window scaled by the system. can only call before create.
	// InitRFC must be called with KDPIAwareness::MIXEDMODE_ONLY
	// Only works with Win10 or higher
	virtual void setEnableDPIUnawareMode(bool enable);

	// In mixed-mode dpi unaware window, before adding any child we need to set current thread dpi mode to unaware mode.
	// by default this method automatically called with AddComponent method.
	// if you add a child without calling AddComponent then you have to call ApplyDPIUnawareModeToThread method first.
	virtual void applyDPIUnawareModeToThread();

	// after adding the child, we need to restore the last dpi mode of the thread.
	// Mixed-Mode only
	virtual void restoreDPIModeOfThread();

	static bool isOffScreen(int posX, int posY);

	virtual bool getClientAreaSize(int* width, int* height);

	// can be use to get the window size even if it were minimized.
	virtual void getNormalSize(int* width, int* height);

	virtual void onMoved();

	// This method will be called on window resize and dpi change.
	// Note: if this method called as a result of dpi change, the dpi of controls in this window are still in old dpi scale.
	// Do not change the control positions/sizes in here if the window and controls are in different dpi scale. (use KDPIChangeListener)
	virtual void onResized();

	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	virtual ~KWindow();
};


// =========== FileModule.h ===========

/*
    Copyright (C) 2013-2025 CrownSoft

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once


#pragma comment(lib,"Shlwapi.lib")



// =========== KGroupBox.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


/*
* Win32 group box is designed to be placed around other controls. Do not use its hwnd
* as a parent/host for other controls. Position other controls over it and then call 
* BringToFront method.
*/
class KGroupBox : public KButton
{
public:
	KGroupBox();

	virtual ~KGroupBox();
};


// =========== KNumericField.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


class KNumericField : public KTextBox
{
public:
	KNumericField();

	virtual ~KNumericField();
};


// =========== KPasswordBox.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


class KPasswordBox : public KTextBox
{
protected:
	wchar_t pwdChar;

public:
	KPasswordBox(bool readOnly=false);

	virtual void setPasswordChar(const wchar_t pwdChar);

	virtual wchar_t getPasswordChar();

	virtual bool create(bool requireInitialMessages = false) override;

	virtual ~KPasswordBox();
};


// =========== KPushButton.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once


class KPushButton : public KCheckBox
{
public:
	KPushButton();

	virtual ~KPushButton();
};


// =========== KRadioButton.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


class KRadioButton : public KCheckBox
{
public:
	KRadioButton();

	virtual ~KRadioButton();
};


// =========== KTextArea.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once


class KTextArea : public KTextBox
{
public:
	KTextArea(bool autoScroll = false, bool readOnly = false);

	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override;

	virtual ~KTextArea();
};


// =========== KToolTip.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once


class KToolTip : public KComponent
{
protected:
	HWND attachedCompHWND;

public:
	KToolTip();

	virtual ~KToolTip();

	/**
		"parentWindow" must be created before you call this method.
		"attachedComponent" must be added to a window before you call this method.
		do not attach same tooltip into multiple components.
	*/
	virtual void attachToComponent(KWindow* parentWindow, KComponent* attachedComponent);

	/**
		calling this method has no effect.
	*/
	virtual bool create(bool requireInitialMessages = false) override;

	virtual void setText(const KString& compText) override;
};




// =========== KWindowTypes.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once

#include <Windowsx.h> // GET_X_LPARAM
#include <type_traits> // std::is_base_of

class KHotPluggedDialog : public KWindow
{
public:
	KHotPluggedDialog();

	virtual void onClose();

	virtual void onDestroy();

	virtual ~KHotPluggedDialog();
};

class KOverlappedWindow : public KWindow
{
public:
	KOverlappedWindow();

	virtual ~KOverlappedWindow();
};

class KFrame : public KWindow
{
public:
	KFrame();

	virtual ~KFrame();
};

class KDialog : public KWindow
{
public:
	KDialog();

	virtual ~KDialog();
};

class KToolWindow : public KWindow
{
public:
	KToolWindow();

	virtual ~KToolWindow();
};

// enables client area dragging. window should not have a title bar.
// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KDraggable : public T
{
protected:
	bool enableClientAreaDragging;
	bool windowDragging;
	short clientAreaDraggingX;
	short clientAreaDraggingY;

	virtual LRESULT onLButtonDown(WPARAM wParam, LPARAM lParam)
	{
		if (enableClientAreaDragging)
		{
			clientAreaDraggingX = GET_X_LPARAM(lParam);
			clientAreaDraggingY = GET_Y_LPARAM(lParam);

			// disabled child will pass left click msg to parent window. we ignore that! 
			// we don't want to drag window by clicking on child.
			POINT point = { clientAreaDraggingX, clientAreaDraggingY };
			if (::RealChildWindowFromPoint(T::compHWND, point) != T::compHWND)
				return 0;

			windowDragging = true;
			::SetCapture(T::compHWND);
		}

		return 0;
	}

	virtual LRESULT onMouseMove(WPARAM wParam, LPARAM lParam)
	{
		if (windowDragging)
		{
			POINT pos;
			::GetCursorPos(&pos);

			this->setPosition(pos.x - clientAreaDraggingX, pos.y - clientAreaDraggingY);
		}

		return 0;
	}

	virtual LRESULT onLButtonUp(WPARAM wParam, LPARAM lParam)
	{
		if (windowDragging)
		{
			::ReleaseCapture();
			windowDragging = false;
		}

		return 0;
	}

public:
	template<typename... Args>
	KDraggable(Args&&... args) : T(std::forward<Args>(args)...)
	{
		enableClientAreaDragging = true;
		windowDragging = false;
		clientAreaDraggingX = 0;
		clientAreaDraggingY = 0;
	}

	virtual void setEnableClientAreaDrag(bool enable)
	{
		enableClientAreaDragging = enable;
	}

	virtual ~KDraggable() {}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_LBUTTONDOWN, onLButtonDown)
		ON_KMSG(WM_MOUSEMOVE, onMouseMove)
		ON_KMSG(WM_LBUTTONUP, onLButtonUp)
	END_KMSG_HANDLER
};

// provides flicker free double buffered drawing method.
// T must be derived from KComponent
template <class T,
	typename = typename std::enable_if<std::is_base_of<KComponent, T>::value>::type>
class KDrawable : public T
{
protected:

	// override this method in subclass and draw your stuff
	virtual void onPaint(HDC hDCMem, RECT* rect, const int width, const int height)
	{
		::FillRect(hDCMem, rect, (HBRUSH)::GetStockObject(WHITE_BRUSH));
		::FrameRect(hDCMem, rect, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	}

	virtual LRESULT onWMPaint(WPARAM wParam, LPARAM lParam)
	{
		RECT rect;
		::GetClientRect(T::compHWND, &rect);

		const int width = rect.right - rect.left;
		const int height = rect.bottom - rect.top;

		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(T::compHWND, &ps);

		// double buffering
		HDC hDCMem = ::CreateCompatibleDC(hdc);
		HBITMAP memBMP = ::CreateCompatibleBitmap(hdc, width, height);;
		::SelectObject(hDCMem, memBMP);

		this->onPaint(hDCMem, &rect, width, height);

		::BitBlt(hdc, 0, 0, width, height, hDCMem, 0, 0, SRCCOPY);

		::DeleteDC(hDCMem);
		::DeleteObject(memBMP);

		::EndPaint(T::compHWND, &ps);

		return 0;
	}

	virtual LRESULT onEraseBackground(WPARAM wParam, LPARAM lParam)
	{
		return 1; // avoids flickering
	}

public:
	template<typename... Args>
	KDrawable(Args&&... args) : T(std::forward<Args>(args)...) {}

	virtual ~KDrawable() {}

	BEGIN_KMSG_HANDLER
		ON_KMSG(WM_PAINT, onWMPaint)
		ON_KMSG(WM_ERASEBKGND, onEraseBackground)
	END_KMSG_HANDLER
};

// adds onCloseEvent to KWindow.
// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KWithOnCloseEvent : public T
{
public:
	std::function<void()> onCloseEvent;

	template<typename... Args>
	KWithOnCloseEvent(Args&&... args) : T(std::forward<Args>(args)...) {}

	virtual ~KWithOnCloseEvent() = default;

	virtual void onClose() override
	{
		if (onCloseEvent)
			onCloseEvent();

		__super::onClose();
	}
};

// adds onDestroyEvent to KWindow.
// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KWithOnDestroyEvent : public T
{
public:
	std::function<void()> onDestroyEvent;

	template<typename... Args>
	KWithOnDestroyEvent(Args&&... args) : T(std::forward<Args>(args)...) {}

	virtual ~KWithOnDestroyEvent() = default;

	virtual void onDestroy() override
	{
		if (onDestroyEvent)
			onDestroyEvent();

		__super::onDestroy();
	}
};

class KWidget : public KDrawable<KDraggable<KWindow>>
{
public:
	KWidget()
	{
		compDwStyle = WS_POPUP;
		compDwExStyle = WS_EX_TOOLWINDOW | WS_EX_CONTROLPARENT;
		compWidth = 128;
		compHeight = 128;
	}

	virtual ~KWidget() {}
};


// =========== KZoomRectEffect.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <type_traits> // std::is_base_of

// shows zooming rectangle effect when window visibility change.
// might be slow on devices where hardware acceleration not present.
// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KZoomRectEffect : public T
{
protected:
	class ZoomRectOverlay : public KComponent
	{
	protected:
		HPEN greyPen, whitePen;

		void updateSizePos(const RECT& startRect, const RECT& endRect)
		{
			// Calculate bounding rectangle that covers the entire animation area
			RECT boundingRect;
			boundingRect.left = min(startRect.left, endRect.left);
			boundingRect.top = min(startRect.top, endRect.top);
			boundingRect.right = max(startRect.right, endRect.right);
			boundingRect.bottom = max(startRect.bottom, endRect.bottom);

			// Add some padding for the pen width and border
			int padding = 15; // Increased padding to accommodate border
			boundingRect.left -= padding;
			boundingRect.top -= padding;
			boundingRect.right += padding;
			boundingRect.bottom += padding;

			const int width = boundingRect.right - boundingRect.left;
			const int height = boundingRect.bottom - boundingRect.top;

			setSize(width, height);
			setPosition(boundingRect.left, boundingRect.top);
		}

		void clearOverlay()
		{
			HDC hdc = ::GetDC(compHWND);

			// Clear the window first (make it transparent)
			RECT clientRect;
			::GetClientRect(compHWND, &clientRect);
			::FillRect(hdc, &clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

			::ReleaseDC(compHWND, hdc);
		}

		void drawRectangleOnOverlay(const RECT& rect)
		{
			HDC hdc = ::GetDC(compHWND);

			// Clear the window first (make it transparent)
			RECT clientRect;
			::GetClientRect(compHWND, &clientRect);
			::FillRect(hdc, &clientRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

			// Define border width
			const int borderWidth = 1;

			// Create grey border rectangle (outer)
			RECT greyRect = rect;
			greyRect.left -= borderWidth;
			greyRect.top -= borderWidth;
			greyRect.right += borderWidth;
			greyRect.bottom += borderWidth;

			// Set transparent brush so we only draw the outlines
			HBRUSH oldBrush = (HBRUSH)::SelectObject(hdc, ::GetStockObject(NULL_BRUSH));

			// Draw grey border (outer rectangle)
			HPEN oldPen = (HPEN)::SelectObject(hdc, greyPen);
			::Rectangle(hdc, greyRect.left, greyRect.top, greyRect.right, greyRect.bottom);

			// Draw white rectangle (inner)
			::SelectObject(hdc, whitePen);
			::Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);

			// Cleanup
			::SelectObject(hdc, oldPen);
			::SelectObject(hdc, oldBrush);
			::ReleaseDC(compHWND, hdc);
		}

		LRESULT onWMPaint(WPARAM wParam, LPARAM lParam)
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(compHWND, &ps);
			// Don't paint anything - keep transparent
			EndPaint(compHWND, &ps);
			return 0;
		}

	public:
		ZoomRectOverlay() : KComponent(true)
		{
			compText.assignStaticText(TXT_WITH_LEN("ZoomRectOverlay"));
			compCtlID = 0; // control id is zero for top level window

			compWidth = 400;
			compHeight = 200;

			compVisible = false;
			compDwStyle = WS_POPUP;
			compDwExStyle = WS_EX_LAYERED | WS_EX_TOPMOST;
			wc.hbrBackground = (HBRUSH)::GetStockObject(NULL_BRUSH);

			greyPen = ::CreatePen(PS_SOLID, 1, RGB(10, 10, 10)); // near black
			whitePen = ::CreatePen(PS_SOLID, 1, RGB(255, 255, 255)); // White pen
		}

		~ZoomRectOverlay()
		{
			::DeleteObject(greyPen);
			::DeleteObject(whitePen);
		}

		bool create(bool requireInitialMessages) override
		{
			if (KComponent::create(requireInitialMessages))
			{
				::SetLayeredWindowAttributes(compHWND, RGB(0, 0, 0), 0, LWA_COLORKEY); // using black as key color
				return true;
			}
			return false;
		}

		void showZoomInEffect(RECT endRect, int steps = 10, int delayMs = 18)
		{
			if (compHWND == NULL)
				return;

			// Calculate center of the end rectangle
			int centerX = (endRect.left + endRect.right) / 2;
			int centerY = (endRect.top + endRect.bottom) / 2;

			// Calculate dimensions of end rectangle
			int endWidth = endRect.right - endRect.left;
			int endHeight = endRect.bottom - endRect.top;

			// Create start rectangle as small version (e.g., 1/4 size) centered at same position
			float startScale = 0.25; // 1/4 of the final size
			int startWidth = (int)(endWidth * startScale);
			int startHeight = (int)(endHeight * startScale);

			RECT startRect;
			startRect.left = centerX - startWidth / 2;
			startRect.top = centerY - startHeight / 2;
			startRect.right = centerX + startWidth / 2;
			startRect.bottom = centerY + startHeight / 2;

			updateSizePos(startRect, endRect);
			clearOverlay(); // clear old stuff before showing!
			setVisible(true);

			// Get overlay window position to adjust rectangle coordinates
			RECT overlayRect;
			::GetWindowRect(compHWND, &overlayRect);

			for (int i = 0; i <= steps; i++)
			{
				float progress = (float)i / steps;

				// Calculate interpolated rectangle
				RECT currentRect;
				currentRect.left = startRect.left + (int)((endRect.left - startRect.left) * progress);
				currentRect.top = startRect.top + (int)((endRect.top - startRect.top) * progress);
				currentRect.right = startRect.right + (int)((endRect.right - startRect.right) * progress);
				currentRect.bottom = startRect.bottom + (int)((endRect.bottom - startRect.bottom) * progress);

				// Convert to overlay window coordinates
				currentRect.left -= overlayRect.left;
				currentRect.top -= overlayRect.top;
				currentRect.right -= overlayRect.left;
				currentRect.bottom -= overlayRect.top;

				// Draw the rectangle
				drawRectangleOnOverlay(currentRect);
				::Sleep(delayMs);
			}

			setVisible(false);
		}

		void showZoomOutEffect(RECT startRect, int steps = 10, int delayMs = 18)
		{
			if (compHWND == NULL)
				return;

			// Calculate center of the start rectangle
			int centerX = (startRect.left + startRect.right) / 2;
			int centerY = (startRect.top + startRect.bottom) / 2;

			// Calculate dimensions of start rectangle
			int startWidth = startRect.right - startRect.left;
			int startHeight = startRect.bottom - startRect.top;

			// Create end rectangle as small version (e.g., 1/4 size) centered at same position
			float endScale = 0.25; // 1/4 of the original size
			int endWidth = (int)(startWidth * endScale);
			int endHeight = (int)(startHeight * endScale);

			RECT endRect;
			endRect.left = centerX - endWidth / 2;
			endRect.top = centerY - endHeight / 2;
			endRect.right = centerX + endWidth / 2;
			endRect.bottom = centerY + endHeight / 2;

			updateSizePos(startRect, endRect);
			clearOverlay(); // clear old stuff before showing!
			setVisible(true);

			// Get overlay window position to adjust rectangle coordinates
			RECT overlayRect;
			::GetWindowRect(compHWND, &overlayRect);

			for (int i = 0; i <= steps; i++)
			{
				float progress = (float)i / steps;

				// Calculate interpolated rectangle (reverse of zoom in)
				RECT currentRect;
				currentRect.left = startRect.left + (int)((endRect.left - startRect.left) * progress);
				currentRect.top = startRect.top + (int)((endRect.top - startRect.top) * progress);
				currentRect.right = startRect.right + (int)((endRect.right - startRect.right) * progress);
				currentRect.bottom = startRect.bottom + (int)((endRect.bottom - startRect.bottom) * progress);

				// Convert to overlay window coordinates
				currentRect.left -= overlayRect.left;
				currentRect.top -= overlayRect.top;
				currentRect.right -= overlayRect.left;
				currentRect.bottom -= overlayRect.top;

				// Draw the rectangle
				drawRectangleOnOverlay(currentRect);

				Sleep(delayMs);
			}

			setVisible(false);
		}

		BEGIN_KMSG_HANDLER
			ON_KMSG(WM_PAINT, onWMPaint)
		END_KMSG_HANDLER
	};

	ZoomRectOverlay zoomRectOverlay;
	int _zoomRectSteps = 10, _zoomRectDelay = 18;

public:
	template<typename... Args>
	KZoomRectEffect(Args&&... args) : T(std::forward<Args>(args)...) {}
	virtual ~KZoomRectEffect() {}

	void setupZoomRect(int steps, int delayMs)
	{
		_zoomRectSteps = steps;
		_zoomRectDelay = delayMs;
	}

	virtual bool create(bool requireInitialMessages = false) override
	{
		if (__super::create(requireInitialMessages))
		{
			zoomRectOverlay.setParentHWND(this->compHWND);
			zoomRectOverlay.create(false);
			return true;
		}
		return false;
	}

	virtual void onDestroy() override
	{
		zoomRectOverlay.destroy();
		__super::onDestroy();
	}

	virtual void setVisible(bool state) override
	{
		// if window minimized or not created, then we don't show the effect!
		if ((this->compHWND == NULL) || ::IsIconic(this->compHWND))
		{
			__super::setVisible(state);
		}
		else if(state != this->compVisible)
		{
			RECT wndRect;
			::GetWindowRect(this->compHWND, &wndRect);

			if (state)
			{
				zoomRectOverlay.showZoomInEffect(wndRect, _zoomRectSteps, _zoomRectDelay);
				__super::setVisible(true);
			}
			else
			{
				__super::setVisible(false);
				zoomRectOverlay.showZoomOutEffect(wndRect, _zoomRectSteps, _zoomRectDelay);
			}
		}
	}
};

// =========== KGlyphButton.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.  
*/

#pragma once


class KGlyphButton : public KButton
{
protected:
	KFont* glyphFont;
	const wchar_t* glyphChar;
	COLORREF glyphColor;
	int glyphLeft;

public:
	KGlyphButton();

	virtual ~KGlyphButton();

	/**
		Use character code for glyphChar. ex: "\x36" for down arrow when using Webdings font.
		You can use "Character Map" tool get character codes.
		Default text color will be used if glyphColor not specified.
	*/
	virtual void setGlyph(const wchar_t* glyphChar, KFont* glyphFont, COLORREF glyphColor = ::GetSysColor(COLOR_BTNTEXT), int glyphLeft = 6);

	virtual bool eventProc(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* result) override;

	virtual void setDPI(int newDPI) override;
};


// =========== KCommonDialogBox.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


// generates filter text for KFILE_FILTER("Text Files", "txt") as follows: L"Text Files\0*.txt\0"
#define KFILE_FILTER(desc, ext) L##desc L"\0*." L##ext L"\0"

class KCommonDialogBox
{
public:
	/**
		Filter string might be like this "Text Files (*.txt)\0*.txt\0"
		You cannot use String object for filter, because filter string contains multiple null characters.
		"dialogGuid" is valid only if "saveLastLocation" is true.
	*/
	static bool showOpenFileDialog(KWindow* window, const KString& title, const wchar_t* filter, KString* fileName, bool saveLastLocation = false, const KString& dialogGuid = KString());

	/**
		Filter string might be like this "Text Files (*.txt)\0*.txt\0"
		You cannot use String object for filter, because filter string contains multiple null characters.
		"dialogGuid" is valid only if "saveLastLocation" is true.
	*/
	static bool showSaveFileDialog(KWindow* window, const KString& title, const wchar_t* filter, KString* fileName, bool saveLastLocation = false, const KString& dialogGuid = KString());
};


// =========== KTimer.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once

#include <functional>

/**
	Encapsulates a timer.

	The timer can be started with the StartTimer() method
	and controlled with various other methods. Before you start timer, you must set 
	timer window by calling SetTimerWindow method.
*/
class KTimer
{
protected:
	UINT timerID;
	int resolution;
	bool started;
	HWND hwndWindow;

public:
	std::function<void(KTimer*)> onTimer;

	KTimer();

	int getInterval();

	void setTimerID(UINT timerID);

	/**
		@returns unique id of this timer
	*/
	UINT getTimerID();

	// Starts timer. window must be created before calling this method. resolution in ms.
	void start(int resolution, KWindow& window);

	// Starts timer. window must be created before calling this method. resolution in ms.
	void start(int resolution, KWindow& window, std::function<void(KTimer*)> onTimerCallback);

	/**
		Stops the timer. You can restart it by calling start() method.
	*/
	void stop();

	bool isRunning();

	void _onTimer();

	~KTimer();

private:
	RFC_LEAK_DETECTOR(KTimer)
};


// =========== KNotifyIconHandler.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft

	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.
*/

#pragma once

#include <shellapi.h>
#include <commctrl.h>
#include <type_traits> // std::is_base_of

#define RFC_NOTIFY_ICON_MESSAGE WM_APP + 101

// adds an icon into the system tray and handles mouse messages.
// detects taskbar re-creation and adds the icon again.
// T must be derived from KWindow
template <class T,
	typename = typename std::enable_if<std::is_base_of<KWindow, T>::value>::type>
class KNotifyIconHandler : public T
{
protected:
	HICON notifyIconHandle;
	KString notifyIconToolTipText;
	UINT taskbarRestartMsg;

	virtual LRESULT onNotifyIconMessage(WPARAM wParam, LPARAM lParam)
	{
		if (lParam == WM_LBUTTONUP)
			this->onNotifyIconLeftClick();
		else if (lParam == WM_RBUTTONUP)
			this->onNotifyIconRightClick();

		return 0;
	}

	virtual void createNotifyIcon(HWND window, HICON icon, const KString& toolTipText)
	{
		NOTIFYICONDATAW nid = { 0 };

		nid.cbSize = sizeof(NOTIFYICONDATAW);
		nid.hWnd = window;
		nid.uID = 1010;
		nid.uVersion = NOTIFYICON_VERSION;
		nid.uCallbackMessage = RFC_NOTIFY_ICON_MESSAGE;
		nid.hIcon = icon;
		::wcscpy_s(nid.szTip, toolTipText);
		nid.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;

		::Shell_NotifyIconW(NIM_ADD, &nid);
	}

	// on explorer crash
	virtual LRESULT onTaskBarReCreate(WPARAM wParam, LPARAM lParam)
	{
		if (notifyIconHandle)
			this->createNotifyIcon(this->compHWND, notifyIconHandle, notifyIconToolTipText);

		return 0;
	}

	// override this method in your subclass and show popup menu.
	virtual void onNotifyIconRightClick()
	{
		::SetForegroundWindow(this->compHWND);
		// show you popup menu here...
	}

	// override this method in your subclass.
	virtual void onNotifyIconLeftClick()
	{
		::SetForegroundWindow(this->compHWND);
	}

public:
	template<typename... Args>
	KNotifyIconHandler(Args&&... args) : T(std::forward<Args>(args)...)
	{
		notifyIconHandle = 0;
		taskbarRestartMsg = ::RegisterWindowMessageW(L"TaskbarCreated");
	}

	virtual ~KNotifyIconHandler()
	{
		if (notifyIconHandle)
			::DestroyIcon(notifyIconHandle);
	}

	// window must be created.
	// maximum tooltip text size is 128
	virtual void addNotifyIcon(WORD iconResourceID, const KString& tooltipText)
	{
		// supports high dpi.
		// LoadIconMetric: only for system tray. cannot use for a window. because multiple window can have different dpi.
		::LoadIconMetric(KApplication::hInstance,
			MAKEINTRESOURCEW(iconResourceID), LIM_SMALL, &notifyIconHandle);

		notifyIconToolTipText = tooltipText;

		this->createNotifyIcon(this->compHWND, notifyIconHandle, notifyIconToolTipText);
	}

	virtual void updateNotifyIcon(WORD iconResourceID)
	{
		if (notifyIconHandle)
			::DestroyIcon(notifyIconHandle);

		::LoadIconMetric(KApplication::hInstance,
			MAKEINTRESOURCEW(iconResourceID), LIM_SMALL, &notifyIconHandle);

		NOTIFYICONDATAW nid = { 0 };

		nid.cbSize = sizeof(NOTIFYICONDATAW);
		nid.hWnd = this->compHWND;
		nid.uID = 1010;
		nid.uVersion = NOTIFYICON_VERSION;
		nid.hIcon = notifyIconHandle;
		nid.uFlags = NIF_ICON;

		::Shell_NotifyIconW(NIM_MODIFY, &nid);
	}

	// maximum tooltip text size is 128
	virtual void updateNotifyIconToolTip(const KString& tooltipText)
	{
		notifyIconToolTipText = tooltipText;

		NOTIFYICONDATAW nid = { 0 };

		nid.cbSize = sizeof(NOTIFYICONDATAW);
		nid.hWnd = this->compHWND;
		nid.uID = 1010;
		nid.uVersion = NOTIFYICON_VERSION;
		::wcscpy_s(nid.szTip, tooltipText);
		nid.uFlags = NIF_TIP;

		::Shell_NotifyIconW(NIM_MODIFY, &nid);
	}

	virtual void destroyNotifyIcon()
	{
		NOTIFYICONDATAW nid = { 0 };
		nid.cbSize = sizeof(NOTIFYICONDATAW);
		nid.hWnd = this->compHWND;
		nid.uID = 1010;

		::Shell_NotifyIconW(NIM_DELETE, &nid);
	}

	virtual LRESULT windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) override
	{
		if (msg == RFC_NOTIFY_ICON_MESSAGE)
			return this->onNotifyIconMessage(wParam, lParam);
		else if (msg == taskbarRestartMsg)
			return this->onTaskBarReCreate(wParam, lParam);
		else
			return T::windowProc(hwnd, msg, wParam, lParam);
	}
};

// =========== KMenuBar.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once


class KMenuBar
{
protected:
	HMENU hMenu;

public:
	KMenuBar();

	virtual void addMenu(const KString& text, KMenu* menu);

	virtual void addToWindow(KWindow* window);

	virtual ~KMenuBar();

private:
	RFC_LEAK_DETECTOR(KMenuBar)
};


// =========== KIDGenerator.h ===========

/*
	Copyright (C) 2013-2025 CrownSoft
  
	This software is provided 'as-is', without any express or implied
	warranty.  In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	3. This notice may not be removed or altered from any source distribution.	  
*/

#pragma once

#include <stdio.h>

/**
	Singleton class which can be used to generate class names, timer ids etc...
	Methods are not thread safe. So, only call from the gui thread.
	(this class is for internal use)
*/
class KIDGenerator
{
private:
	RFC_LEAK_DETECTOR(KIDGenerator)
	KIDGenerator();
	~KIDGenerator();

protected:
	int classCount;
	int timerCount;
	int controlCount;
	UINT menuItemCount;

	static const int rfc_InitialMenuItemCount = 20;
	static const int rfc_InitialTimerCount = 10;

	static const int rfc_InitialControlID = 100;
	static const int rfc_InitialMenuItemID = 30000;
	static const int rfc_InitialTimerID = 1000;

	KPointerList<KMenuItem*, rfc_InitialMenuItemCount, false> menuItemList;
	KPointerList<KTimer*, rfc_InitialTimerCount, false> timerList;

public:
	// do not delete the returned instance.
	static KIDGenerator* getInstance();

	// [gui thread only]
	UINT generateControlID();

	// KApplication:hInstance must be valid before calling this method
	// can generate up to 9999 class names.
	// [gui thread only]
	void generateClassName(KString& stringToModify);

	// [gui thread only]
	UINT generateMenuItemID(KMenuItem* menuItem);

	// [gui thread only]
	KMenuItem* getMenuItemByID(UINT id);

	// [gui thread only]
	UINT generateTimerID(KTimer* timer);

	// [gui thread only]
	KTimer* getTimerByID(UINT id);
};

// =========== GUIModule.h ===========

/*
    Copyright (C) 2013-2025 CrownSoft

    This software is provided 'as-is', without any express or implied
    warranty.  In no event will the authors be held liable for any damages
    arising from the use of this software.

    Permission is granted to anyone to use this software for any purpose,
    including commercial applications, and to alter it and redistribute it
    freely, subject to the following restrictions:

    1. The origin of this software must not be misrepresented; you must not
       claim that you wrote the original software. If you use this software
       in a product, an acknowledgment in the product documentation would be
       appreciated but is not required.
    2. Altered source versions must be plainly marked as such, and must not be
       misrepresented as being the original software.
    3. This notice may not be removed or altered from any source distribution.
*/

#pragma once


#pragma comment(lib, "Comctl32.lib")
#pragma comment(lib, "Shell32.lib")

#endif

