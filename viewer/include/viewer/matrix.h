//
//  matrix.h
//  viewer
//
//  Created by Marika on 2020-02-25.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <mirai/vector.h>
#include <mirai/matrix.h>

// MARK: - Functions

/// Create an empty 4x4 matrix.
/// @returns An empty 4x4 matrix.
struct mat4_t mat4_empty(void);

/// Create a 4x4 identity matrix.
/// @returns A 4x4 identity matrix.
struct mat4_t mat4_identity(void);

/// Multiply two 4x4 matrices and get the result.
/// @param lhs The left-hand side of the operation.
/// @param rhs The right-hand side of the operation.
/// @returns The result of multiplying the two given matrices.
struct mat4_t mat4_multiply(struct mat4_t lhs, struct mat4_t rhs);

/// Create a 4x4 translation matrix from the given 3D offset.
/// @param offset The 3D offset to create a matrix for.
/// @returns The translation matrix for the given offset.
struct mat4_t mat4_translation(struct vec3_t offset);

/// Create a 4x4 scaling matrix from the given 3D scale.
/// @param scale The 3D scale to create a matrix for.
/// @returns The scaling matrix for the given scale.
struct mat4_t mat4_scaling(struct vec3_t scale);

/// Create a 4x4 orthographic projection matrix from the given properties.
/// @param left The left vertical clipping plane.
/// @param right The right vertical clipping plane.
/// @param top The top horizontal clipping plane.
/// @param bottom The bottom horizontal clipping plane.
/// @param z_near The near clipping plane.
/// @param z_far The far clipping plane.
/// @returns The orthographic projection matrix for the given properties.
struct mat4_t mat4_ortho(float left,
                         float right,
                         float top,
                         float bottom,
                         float z_near,
                         float z_far);
