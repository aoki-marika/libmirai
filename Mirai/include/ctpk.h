//
//  ctpk.h
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

// MARK: - Data Structures

/// The data structure for a CTPK file that has been opened.
struct ctpk_file
{
    /// The number of textures within this file.
    unsigned int num_textures;

    /// The array of textures within this file.
    struct ctpk_texture **textures;
};

/// The data structure for a texture that has been read from a CTPK file.
struct ctpk_texture
{
    /// The width of this texture, in pixels.
    uint16_t width;

    /// The height of this texture, in pixels.
    uint16_t height;

    /// The size of the data of this texture, in bytes.
    unsigned long long data_size;

    /// The offset of the data of this texture within the original file handle, in bytes.
    unsigned long long data_pointer;
};

// MARK: - Functions

/// Open the CTPK file from the given file handle into the given `ctpk_file` data structure.
/// @param input The file handle to read the CTPK file from.
/// @param output The `ctpk_file` to read the file into.
void ctpk_open(FILE *input, struct ctpk_file *output);

/// Close the given `ctpk_file`, releasing all of it's memory.
/// This must be called after an `ctpk_file` is opened and before program execution completes.
/// @param file The `ctpk_file` to close.
void ctpk_close(struct ctpk_file *file);
