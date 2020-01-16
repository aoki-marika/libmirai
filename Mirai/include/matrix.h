//
//  matrix.h
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

// MARK: - Data Structures

/// A data structure to represent a 4x4 matrix.
struct mat4_t
{
    float data[4][4];
};

// MARK: - Functions

/// Read the 4x3 matrix at the current offset of the given file handle into the given 4x4 matrix.
/// @param file The file handle to read the 4x3 matrix from.
/// @param matrix The 4x4 matrix to read the file into.
void mat4_read43(FILE *file, struct mat4_t *matrix);
