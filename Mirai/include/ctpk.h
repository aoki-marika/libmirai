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

/// The different formats that a single texture within a CTPK file can be.
///
/// Unless stated otherwise, each format decodes to the same format.
enum ctpk_texture_format
{
    /// Eight bits for the red, green, blue, and alpha channels.
    CTPK_TEXTURE_FORMAT_RGBA8888 = 0x0,

    /// Eight bits for the red, green, and blue channels.
    CTPK_TEXTURE_FORMAT_RGB888   = 0x1,

    /// Five bits for the red, green, and blue channels, and one bit for the alpha channel.
    CTPK_TEXTURE_FORMAT_RGBA5551 = 0x2,

    /// Five bits for the red and blue channels, and six bits for the green channel.
    CTPK_TEXTURE_FORMAT_RGB565   = 0x3,

    /// Four bits for the red, green, blue, and alpha channels.
    CTPK_TEXTURE_FORMAT_RGBA4444 = 0x4,

    /// Eight bits for the luminance and alpha channels.
    CTPK_TEXTURE_FORMAT_LA88     = 0x5,

    /// Eight bits for the hue and luminance channels.
    CTPK_TEXTURE_FORMAT_HL8      = 0x6,

    /// Eight bits for the luminance channel.
    CTPK_TEXTURE_FORMAT_L8       = 0x7,

    /// Eight bits for the alpha channel.
    CTPK_TEXTURE_FORMAT_A8       = 0x8,

    /// Four bits for the luminance and alpha channels.
    CTPK_TEXTURE_FORMAT_LA44     = 0x9,

    /// Four bits for the luminance channel.
    CTPK_TEXTURE_FORMAT_L4       = 0xa,

    /// Four bits for the alpha channel.
    CTPK_TEXTURE_FORMAT_A4       = 0xb,

    /// Ericsson Texture Compression version one.
    /// Decodes to eight bit red, green, and blue channels.
    CTPK_TEXTURE_FORMAT_ETC1     = 0xc,

    /// Ericsson Texture Compression version one, with an additional four bits for the alpha channel.
    /// Decodes to eight bit red, green, blue, and alpha channels.
    CTPK_TEXTURE_FORMAT_ETC1_A4  = 0xd,
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

    /// The size of this texture's data when it has been decoded.
    unsigned long long decoded_data_size;

    /// The format of this texture's data.
    enum ctpk_texture_format data_format;
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

/// Decode the given CTPK texture's data from the given file handle.
/// @param texture The texture to decode the data of.
/// @param source The file handle to read the texture's data from.
/// @returns A pointer to the array of decoded texture data from the given texture. This must be freed before program execution completes.
/// Note that that the data is in different formats for the different texture formats, see `ctpk_texture_format` cases for more information.
uint8_t *ctpk_texture_decode(const struct ctpk_texture *texture, FILE *source);
