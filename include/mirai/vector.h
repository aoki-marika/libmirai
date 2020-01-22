//
//  vector.h
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

// MARK: - Data Structures

/// A data structure to represent a two-dimensional vector.
struct vec2_t
{
    float x, y;
};

/// A data structure to represent a three-dimensional vector.
struct vec3_t
{
    float x, y, z;
};

// MARK: - Functions

/// Read the 2D vector at the current offset of the given file handle into the given 2D vector.
/// @param file The file handle to read the 2D vector from.
/// @param vector The 2D vector to read the file into.
void vec2_read(FILE *file, struct vec2_t *vector);

/// Read the 3D vector at the current offset of the given file handle into the given 3D vector.
/// @param file The file handle to read the 3D vector from.
/// @param vector The 3D vector to read the file into.
void vec3_read(FILE *file, struct vec3_t *vector);
