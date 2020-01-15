//
//  texture.h
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#include <stdio.h>

// MARK: - Enumerations

/// The different formats that a texture's encoded data can be in.
///
/// Unless stated otherwise, each format decodes to the same format.
enum texture_format_t
{
    /// Eight bits for the red, green, blue, and alpha channels.
    TEXTURE_FORMAT_RGBA8888 = 0x0,

    /// Eight bits for the red, green, and blue channels.
    TEXTURE_FORMAT_RGB888   = 0x1,

    /// Five bits for the red, green, and blue channels, and one bit for the alpha channel.
    TEXTURE_FORMAT_RGBA5551 = 0x2,

    /// Five bits for the red and blue channels, and six bits for the green channel.
    TEXTURE_FORMAT_RGB565   = 0x3,

    /// Four bits for the red, green, blue, and alpha channels.
    TEXTURE_FORMAT_RGBA4444 = 0x4,

    /// Eight bits for the luminance and alpha channels.
    TEXTURE_FORMAT_LA88     = 0x5,

    /// Eight bits for the hue and luminance channels.
    TEXTURE_FORMAT_HL8      = 0x6,

    /// Eight bits for the luminance channel.
    TEXTURE_FORMAT_L8       = 0x7,

    /// Eight bits for the alpha channel.
    TEXTURE_FORMAT_A8       = 0x8,

    /// Four bits for the luminance and alpha channels.
    TEXTURE_FORMAT_LA44     = 0x9,

    /// Four bits for the luminance channel.
    TEXTURE_FORMAT_L4       = 0xa,

    /// Four bits for the alpha channel.
    TEXTURE_FORMAT_A4       = 0xb,

    /// Ericsson Texture Compression version one.
    /// Decodes to eight bit red, green, and blue channels.
    TEXTURE_FORMAT_ETC1     = 0xc,

    /// Ericsson Texture Compression version one, with an additional four bits for the alpha channel.
    /// Decodes to eight bit red, green, blue, and alpha channels.
    TEXTURE_FORMAT_ETC1_A4  = 0xd,
};

// MARK: - Data Structures

/// The data structure for metadata about a texture.
struct texture_t
{
    /// The width of this texture, in pixels.
    unsigned int width;

    /// The height of this texture, in pixels.
    unsigned int height;

    /// The size of this texture's encoded data, in bytes.
    size_t data_size;

    /// The offset of this texture's encoded data within it's file, in bytes.
    size_t data_pointer;

    /// The format of this texture's encoded data.
    enum texture_format_t data_format;

    /// The size of this texture's decoded data, in bytes.
    size_t decoded_data_size;
};

// MARK: - Functions

/// Fill in the given texture data structure's properties with the given parameters.
///
/// This function should always be used over creating the data structure manually,
/// as it automatically calculates fields such as `decoded_data_size`.
///
/// See `struct texture_t` property documentation for parameter information.
/// @param texture The texture data structure to fill in.
void texture_create(unsigned int width,
                    unsigned int height,
                    size_t data_size,
                    size_t data_pointer,
                    enum texture_format_t data_format,
                    struct texture_t *texture);

/// Read and decode the given texture's data from the given file handle.
/// @param texture The texture to read and decode the data of.
/// @param file The file handle to read the texture's data from.
/// @returns A pointer to the array of decoded texture data from the given texture.
/// This data is allocated so it must be freed before program execution completes.
/// Note that that the decoded data is in different formats for each texture format, see `texture_format_t` cases for more information.
uint8_t *texture_decode(const struct texture_t *texture, FILE *file);
