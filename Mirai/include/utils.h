//
//  utils.h
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

#include "vector.h"
#include "matrix.h"

// MARK: - Functions

/// Read the relative offset at the current offset of the given file handle, and convert it to an absolute pointer within the file.
///
/// Some 3DS file formats use this for pointers, where there is an offset u32 for the pointer, which is relative to the beginning of said u32.
/// So to get the absolute offset of that offset within the file, it can add the offset to the current offset of the file handle before reading the offset.
/// @param file The file handle to read the offset from and use the base offset of.
/// @returns The absolute pointer within the file of the offset at the current offset of the given file handle.
uint32_t utils_read_relative_pointer(FILE *file);

/// Read the null terminated string at the current offset of the given file handle.
/// @param file The file handle to read the string from.
/// @returns The null terminated string at the current offset of the given file handle.
/// This string is allocated and must be freed.
char *utils_read_string(FILE *file);

/// Read the 3D vector at the current offset of the given file handle into the given 3D vector.
/// @param file The file handle to read the 3D vector from.
/// @param vector The 3D vector to load the data into.
void utils_read_vec3(FILE *file, struct vec3_t *vector);

/// Read the 4x3 matrix at the current offset of the given file handle into the given 4x4 matrix.
/// @param file The file handle to read the 4x3 matrix from.
/// @param matrix The 4x4 matrix to load the data into.
void utils_read_mat4(FILE *file, struct mat4_t *matrix);
