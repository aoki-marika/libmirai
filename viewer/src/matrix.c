//
//  matrix.c
//  viewer
//
//  Created by Marika on 2020-02-25.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "matrix.h"

#include <string.h>
#include <math.h>

#include "constants.h"

// MARK: - Functions

struct mat4_t mat4_empty()
{
    struct mat4_t matrix;
    memset(&matrix, 0x0, sizeof(matrix));
    return matrix;
}

struct mat4_t mat4_identity()
{
    struct mat4_t matrix = mat4_empty();
    matrix.data[0][0] = 1;
    matrix.data[1][1] = 1;
    matrix.data[2][2] = 1;
    matrix.data[3][3] = 1;
    return matrix;
}

struct mat4_t mat4_multiply(struct mat4_t lhs, struct mat4_t rhs)
{
    struct mat4_t result = mat4_empty();

    for (int c = 0; c < 4; c++)
    {
        for (int r = 0; r < 4; r++)
        {
            float sum = 0;
            for (int i = 0; i < 4; i++)
                sum += lhs.data[i][r] * rhs.data[c][i];

            result.data[c][r] = sum;
        }
    }

    return result;
}

struct mat4_t mat4_translation(struct vec3_t offset)
{
    struct mat4_t result = mat4_identity();
    result.data[3][0] = offset.x;
    result.data[3][1] = offset.y;
    result.data[3][2] = offset.z;
    return result;
}

struct mat4_t mat4_scaling(struct vec3_t scale)
{
    struct mat4_t result = mat4_identity();
    result.data[0][0] = scale.x;
    result.data[1][1] = scale.y;
    result.data[2][2] = scale.z;
    return result;
}

struct mat4_t mat4_rotation_z(float radians)
{
    float sin = sinf(radians);
    float cos = cosf(radians);
    struct mat4_t result = mat4_identity();
    result.data[0][0] = cos;
    result.data[0][1] = sin;
    result.data[1][0] = -sin;
    result.data[1][1] = cos;
    return result;
}

struct mat4_t mat4_ortho(float left,
                         float right,
                         float bottom,
                         float top,
                         float z_near,
                         float z_far)
{
    struct mat4_t result = mat4_identity();
    result.data[0][0] = 2 / (right - left);
    result.data[1][1] = 2 / (top - bottom);
    result.data[2][2] = -2 / (z_far - z_near);
    return result;
}
