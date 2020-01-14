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
///
/// If `ctpk_open(file, ctpk)` is called on a CTPK then `ctpk_close(ctpk)` must also be called on it before program execution completes.
struct ctpk_t
{
    /// The number of textures within this CTPK.
    unsigned int num_textures;

    /// All of the textures within this CTPK.
    ///
    /// The array and each texture are allocated.
    struct ctpk_texture_t **textures;
};

/// The different formats that the encoded data of a single texture within a CTPK can be.
///
/// Unless stated otherwise, each format decodes to the same format.
enum ctpk_texture_format_t
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

/// The data structure for a single texture within a CTPK.
struct ctpk_texture_t
{
    /// The width of this texture, in pixels.
    uint16_t width;

    /// The height of this texture, in pixels.
    uint16_t height;

    /// The size of this texture's encoded data within the file, in bytes.
    unsigned long long data_size;

    /// The offset of this texture's encoded data within the file, in bytes.
    unsigned long long data_pointer;

    /// The format of this texture's encoded data within the file.
    enum ctpk_texture_format_t data_format;

    /// The size of this texture's data when it has been decoded.
    unsigned long long decoded_data_size;
};

// MARK: - Functions

/// Open the CTPK file from the given file handle into the given CTPK.
///
/// The CTPK file is read starting from the current position indicator of the given file handle.
/// @param file The file handle to open the CTPK file from.
/// @param ctpk The CTPK to open the file into.
void ctpk_open(FILE *file, struct ctpk_t *ctpk);

/// Close the given CTPK, releasing all of it's allocated memory.
///
/// This must be called after a CTPK is opened and before program execution completes.
/// @param ctpk The CTPK to close.
void ctpk_close(struct ctpk_t *ctpk);

/// Read and decode the given CTPK texture's data from the given file handle.
/// @param texture The texture to read and decode the data of.
/// @param file The file handle to read the texture's data from.
/// @returns A pointer to the array of decoded texture data from the given texture.
/// This data is allocated so it must be freed before program execution completes.
/// Note that that the decoded data is in different formats for each texture format, see `ctpk_texture_format_t` cases for more information.
uint8_t *ctpk_texture_decode(const struct ctpk_texture_t *texture, FILE *file);
