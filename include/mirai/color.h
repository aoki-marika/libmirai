//
//  color.h
//  libmirai
//
//  Created by Marika on 2020-01-19.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

// MARK: - Data Structures

/// A data structure to represent a colour with a red, green, blue, and alpha channel.
struct color4_t
{
    float r, g, b, a;
};

// MARK: - Functions

/// Read the floating point RGBA colour at the current offset of the given file handle into the given colour.
/// @param file The file handle to read the floating point RGBA colour from.
/// @param color The colour to read the file into.
void color4_readf(FILE *file, struct color4_t *color);

/// Read the unsigned 8-bit RGBA colour at the current offset of the given file handle into the given colour.
/// @param file The file handle to read the unsigned 8-bit RGBA colour from.
/// @param color The colour to read the file into.
void color4_readu8(FILE *file, struct color4_t *color);
