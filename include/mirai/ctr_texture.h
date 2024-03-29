//
//  texture.h
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

// MARK: - Enumerations

/// The different formats that a CTR texture's encoded data can be in.
///
/// Unless stated otherwise, each format decodes to the same format.
enum ctr_texture_format_t
{
    /// Eight bits for the red, green, blue, and alpha channels.
    CTR_TEXTURE_FORMAT_RGBA8888 = 0x0,

    /// Eight bits for the red, green, and blue channels.
    CTR_TEXTURE_FORMAT_RGB888   = 0x1,

    /// Five bits for the red, green, and blue channels, and one bit for the alpha channel.
    CTR_TEXTURE_FORMAT_RGBA5551 = 0x2,

    /// Five bits for the red and blue channels, and six bits for the green channel.
    CTR_TEXTURE_FORMAT_RGB565   = 0x3,

    /// Four bits for the red, green, blue, and alpha channels.
    CTR_TEXTURE_FORMAT_RGBA4444 = 0x4,

    /// Eight bits for the luminance and alpha channels.
    CTR_TEXTURE_FORMAT_LA88     = 0x5,

    /// Eight bits for the hue and lightness channels.
    CTR_TEXTURE_FORMAT_HL8      = 0x6,

    /// Eight bits for the luminance channel.
    CTR_TEXTURE_FORMAT_L8       = 0x7,

    /// Eight bits for the alpha channel.
    CTR_TEXTURE_FORMAT_A8       = 0x8,

    /// Four bits for the luminance and alpha channels.
    CTR_TEXTURE_FORMAT_LA44     = 0x9,

    /// Four bits for the luminance channel.
    /// Decodes to an eight bit luminance channel.
    CTR_TEXTURE_FORMAT_L4       = 0xa,

    /// Four bits for the alpha channel.
    /// Decodes to an eight bit alpha channel.
    CTR_TEXTURE_FORMAT_A4       = 0xb,

    /// Ericsson Texture Compression version one.
    /// Decodes to eight bit red, green, and blue channels.
    CTR_TEXTURE_FORMAT_ETC1     = 0xc,

    /// Ericsson Texture Compression version one, with an additional four bits for the alpha channel.
    /// Decodes to eight bit red, green, blue, and alpha channels.
    CTR_TEXTURE_FORMAT_ETC1_A4  = 0xd,
};

// MARK: - Data Structures

/// The data structure for metadata about a CTR texture.
///
/// CTR textures refer to the standard texture encoding format used across various CTR games.
/// CTR being the internal name of the 3DS.
struct ctr_texture_t
{
    /// The width of this CTR texture, in pixels.
    unsigned int width;

    /// The height of this CTR texture, in pixels.
    unsigned int height;

    /// The size of this CTR texture's encoded data, in bytes.
    size_t data_size;

    /// The offset of this CTR texture's encoded data within it's file, in bytes.
    size_t data_pointer;

    /// The format of this CTR texture's encoded data.
    enum ctr_texture_format_t data_format;

    /// The size of this CTR texture's decoded data, in bytes.
    size_t decoded_data_size;

    /// The size of this CTR texture's unpacked decoded data, in bytes.
    ///
    /// Although this is always a constant `width * height * 4 channels (RGBA)`,
    /// it can be useful for defining fixed length arrays in usage.
    size_t unpacked_data_size;
};

// MARK: - Functions

/// Fill in the given CTR texture data structure's properties with the given parameters.
///
/// This function should always be used over creating the data structure manually,
/// as it automatically calculates fields such as `decoded_data_size`.
///
/// See `struct ctr_texture_t` property documentation for parameter information.
/// @param texture The CTR texture data structure to fill in.
void ctr_texture_create(unsigned int width,
                        unsigned int height,
                        size_t data_size,
                        size_t data_pointer,
                        enum ctr_texture_format_t data_format,
                        struct ctr_texture_t *texture);

/// Read and decode the given CTR texture's data from the given file handle.
/// @param texture The CTR texture to read and decode the data of.
/// @param file The file handle to read the CTR texture's data from.
/// @returns A pointer to the array of decoded texture data from the given CTR texture.
/// This data is allocated so it must be freed before program execution completes.
/// Note that that the decoded data is in different formats for each texture format, see `texture_format_t` cases for more information.
/// Also note that this data starts from the **upper-left corner**, with each row being left to right.
/// If this is uploaded directly to OpenGL, then it will be upside down.
uint8_t *ctr_texture_decode(const struct ctr_texture_t *texture, FILE *file);

/// Unpack the given decoded CTR texture data to 8-bit red, green, blue, and alpha channels.
///
/// This is a compatibility feature for directly using OpenGL where luminance and alpha texture data is not supported.
/// @param texture The CTR texture of which the given decoded data originated from.
/// @param decoded The decoded CTR texture data to unpack.
/// @returns The unpacked 8-bit red, green, blue, and alpha channels of the given decoded CTR texture data.
/// Allocated.
/// Note that, the same as `ctr_texture_decode(const struct ctr_texture_t *, FILE *)`, the unpacked data is ordered top to bottom,
/// with rows being ordered left to right.
uint8_t *ctr_texture_unpack(const struct ctr_texture_t *texture,
                            const uint8_t *decoded);
