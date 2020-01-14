//
//  ctpk.c
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "ctpk.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "etcdec.h"

// MARK: - Constants

/// Translation bytes used for decoding texture data from a CTPK file.
const int trans_bytes[64] =
{
     0,  1,  4,  5, 16, 17, 20, 21,
     2,  3,  6,  7, 18, 19, 22, 23,
     8,  9, 12, 13, 24, 25, 28, 29,
    10, 11, 14, 15, 26, 27, 30, 31,
    32, 33, 36, 37, 48, 49, 52, 53,
    34, 35, 38, 39, 50, 51, 54, 55,
    40, 41, 44, 45, 56, 57, 60, 61,
    42, 43, 46, 47, 58, 59, 62, 63
};

// MARK: - Data Structures

/// The data structure for the header of a CTPK file.
struct ctpk_header_t
{
    // the signature is separated into multiple fields so that
    // signature checking doesnt have to account for byte order swapping

    /// The `C` character of this file's signature.
    uint8_t signatureC;

    /// The `T` character of this file's signature.
    uint8_t signatureT;

    /// The `P` character of this file's signature.
    uint8_t signatureP;

    /// The `K` character of this file's signature.
    uint8_t signatureK;

    /// The CTPK format version that this file uses.
    uint16_t version;

    /// The number of textures within this file.
    uint16_t num_textures;

    /// The offset of the beginning of the textures within this file, in bytes.
    uint32_t textures_pointer;

    /// The combined size of all the textures within this file, in bytes.
    uint32_t textures_size;

    /// The offset of the hash section within this file, in bytes.
    uint32_t hashes_pointer;

    /// The offset of the beginning of the texture conversion data within this file, in bytes.
    uint32_t conversion_pointer;

    /// Reserved data.
    uint64_t reserved;
};

/// The data structure for the header of a single texture within a CTPK file.
struct ctpk_texture_header_t
{
    uint32_t file_path_pointer;

    /// The size of this texture's encoded data within the file, in bytes.
    uint32_t data_size;

    /// The offset of the beginning of this texture's encoded data within the file, in bytes.
    uint32_t data_pointer;

    /// The format of this texture's encoded data.
    enum ctpk_texture_format_t format;

    /// The width of this texture, in pixels.
    uint16_t width;

    /// The height of this texture, in pixels.
    uint16_t height;

    /// The mipmap level of this texture.
    uint8_t mipmap_level;

    uint8_t type;
    uint16_t cube_direction;
    uint32_t bitmap_size_pointer;
    uint32_t timestamp;
};

/// The data structure for the conversion info of a single texture within a CTPK file.
///
/// Unsure exactly what this does and it's not necessary at the moment so leaving undocumented.
struct ctpk_texture_conversion_info_t
{
    uint8_t format;
    uint8_t mipmap_level;
    uint8_t compression;
    uint8_t compression_method;
};

// MARK: - Functions

void ctpk_open(FILE *file, struct ctpk_t *ctpk)
{
    // read the start of this file relative to the file handle
    // this is used to store the absolute position of texture data
    unsigned long long start_pointer = ftell(file);

    // read the header
    // the signature is CTPK in ascii
    struct ctpk_header_t header;
    fread(&header, sizeof(header), 1, file);
    assert(header.signatureC == 'C');
    assert(header.signatureT == 'T');
    assert(header.signatureP == 'P');
    assert(header.signatureK == 'K');

    // initialize the output file
    ctpk->num_textures = header.num_textures;
    ctpk->textures = malloc(header.num_textures * sizeof(struct ctpk_texture_t *));

    // read all the textures
    for (unsigned int i = 0; i < header.num_textures; i++)
    {
        // read the header
        struct ctpk_texture_header_t texture_header;
        fread(&texture_header, sizeof(texture_header), 1, file);

        // read the conversion info
        struct ctpk_texture_conversion_info_t conversion_info;
        fread(&conversion_info, sizeof(conversion_info), 1, file);

        // create the texture
        struct ctpk_texture_t *texture = malloc(sizeof(struct ctpk_texture_t));
        texture->width = texture_header.width;
        texture->height = texture_header.height;
        texture->data_size = texture_header.data_size;
        texture->data_pointer = start_pointer + header.textures_pointer + texture_header.data_pointer;
        texture->data_format = texture_header.format;

        unsigned long long w = texture_header.width;
        unsigned long long h = texture_header.height;
        switch (texture_header.format)
        {
            case CTPK_TEXTURE_FORMAT_RGBA8888: texture->decoded_data_size = w * h * 4;
            case CTPK_TEXTURE_FORMAT_RGB888:   texture->decoded_data_size = w * h * 3;
            case CTPK_TEXTURE_FORMAT_RGBA5551:
            case CTPK_TEXTURE_FORMAT_RGB565:
            case CTPK_TEXTURE_FORMAT_RGBA4444: texture->decoded_data_size = w * h * 2;
            case CTPK_TEXTURE_FORMAT_LA88:
            case CTPK_TEXTURE_FORMAT_HL8:      texture->decoded_data_size = w * h * 2;
            case CTPK_TEXTURE_FORMAT_L8:
            case CTPK_TEXTURE_FORMAT_A8:
            case CTPK_TEXTURE_FORMAT_LA44:     texture->decoded_data_size = w * h;
            case CTPK_TEXTURE_FORMAT_L4:
            case CTPK_TEXTURE_FORMAT_A4:       texture->decoded_data_size = w * h;
            case CTPK_TEXTURE_FORMAT_ETC1:     texture->decoded_data_size = w * h * 3;
            case CTPK_TEXTURE_FORMAT_ETC1_A4:  texture->decoded_data_size = w * h * 4;
        }

        ctpk->textures[i] = texture;
    }
}

void ctpk_close(struct ctpk_t *ctpk)
{
    for (unsigned int i = 0; i < ctpk->num_textures; i++)
        free(ctpk->textures[i]);

    free(ctpk->textures);
}

/// Read an ETC compressed word from the given bytes at the given offset.
/// @param source The array of bytes to read the word from.
/// @param offset The offset in the source to read the word from, modified to increment past the word once it's read.
/// @returns The 32bit ETC word from the given source at the given offset.
uint32_t read_etc_word(const uint8 *source, int *offset)
{
    uint8_t bytes[sizeof(uint32_t)];
    memcpy(bytes, source + *offset, sizeof(uint32_t));
    *offset += sizeof(uint32_t);

    uint32_t word = 0;
    word |= bytes[0];
    word <<= 8;
    word |= bytes[1];
    word <<= 8;
    word |= bytes[2];
    word <<= 8;
    word |= bytes[3];

    return word;
}

uint8_t *ctpk_texture_decode(const struct ctpk_texture_t *texture, FILE *file)
{
    // these are all translations from dnasdws ctpktool project
    // all credits to them for creating the implementations
    // https://github.com/dnasdw/ctpktool/blob/master/src/ctpk.cpp

    // read the data to decode
    uint8_t raw_data[texture->data_size];
    fseek(file, texture->data_pointer, SEEK_SET);
    fread(raw_data, texture->data_size, 1, file);

    // decode and return the data
    int w = (int)texture->width;
    int h = (int)texture->height;
    switch (texture->data_format)
    {
        case CTPK_TEXTURE_FORMAT_RGBA8888:
        {
            uint8_t temp[w * h * 4];
            for (int i = 0; i < w * h / 64; i++)
                for (int j = 0; j < 64; j++)
                    for (int k = 0; k < 4; k++)
                        temp[(i * 64 + j) * 4 + k] = raw_data[(i * 64 + trans_bytes[j]) * 4 + 3 - k];

            uint8_t *decoded = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    for (int k = 0; k < 4; k++)
                        decoded[(i * w + j) * 4 + k] = temp[(((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 4 + k];

            return decoded;
        }
        case CTPK_TEXTURE_FORMAT_RGB888:
        {
            uint8_t temp[w * h * 3];
            for (int i = 0; i < w * h / 64; i++)
                for (int j = 0; j < 64; j++)
                    for (int k = 0; k < 3; k++)
                        temp[(i * 64 + j) * 3 + k] = raw_data[(i * 64 + trans_bytes[j]) * 3 + 2 - k];

            uint8_t *decoded = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    for (int k = 0; k < 3; k++)
                        decoded[(i * w + j) * 3 + k] = temp[(((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 3 + k];

            return decoded;
        }
        case CTPK_TEXTURE_FORMAT_RGBA5551:
        case CTPK_TEXTURE_FORMAT_RGB565:
        case CTPK_TEXTURE_FORMAT_RGBA4444:
        {
            uint8_t temp[w * h * 2];
            for (int i = 0; i < w * h / 64; i++)
                for (int j = 0; j < 64; j++)
                    for (int k = 0; k < 2; k++)
                        temp[(i * 64 + j) * 2 + k] = raw_data[(i * 64 + trans_bytes[j]) * 2 + k];

            uint8_t *decoded = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    for (int k = 0; k < 2; k++)
                        decoded[(i * w + j) * 2 + k] = temp[(((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 2 + k];

            return decoded;
        }
        case CTPK_TEXTURE_FORMAT_LA88:
        case CTPK_TEXTURE_FORMAT_HL8:
        {
            uint8_t temp[w * h * 2];
            for (int i = 0; i < w * h / 64; i++)
                for (int j = 0; j < 64; j++)
                    for (int k = 0; k < 2; k++)
                        temp[(i * 64 + j) * 2 + k] = raw_data[(i * 64 + trans_bytes[j]) * 2 + 1 - k];

            uint8_t *decoded = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    for (int k = 0; k < 2; k++)
                        decoded[(i * w + j) * 2 + k] = temp[(((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 2 + k];

            return decoded;
        }
        case CTPK_TEXTURE_FORMAT_L8:
        case CTPK_TEXTURE_FORMAT_A8:
        case CTPK_TEXTURE_FORMAT_LA44:
        {
            uint8_t temp[w * h];
            for (int i = 0; i < w * h / 64; i++)
                for (int j = 0; j < 64; j++)
                    temp[i * 64 + j] = raw_data[i * 64 + trans_bytes[j]];

            uint8_t *decoded = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    decoded[i * w + j] = temp[((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8];

            return decoded;
        }
        case CTPK_TEXTURE_FORMAT_L4:
        case CTPK_TEXTURE_FORMAT_A4:
        {
            uint8_t temp[w * h];
            for (int i = 0; i < w * h / 64; i++)
            {
                for (int j = 0; j < 64; j += 2)
                {
                    temp[i * 64 + j] = (raw_data[i * 32 + trans_bytes[j] / 2] & 0xF) * 0x11;
                    temp[i * 64 + j + 1] = (raw_data[i * 32 + trans_bytes[j] / 2] >> 4 & 0xF) * 0x11;
                }
            }

            uint8_t *decoded = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    decoded[i * w + j] = temp[((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8];

            return decoded;
        }
        case CTPK_TEXTURE_FORMAT_ETC1:
        {
            uint8_t temp[w * h / 2];
            for (int i = 0; i < w * h / 2 / 8; i++)
                for (int j = 0; j < 8; j++)
                    temp[i * 8 + j] = raw_data[i * 8 + 7 - j];

            uint8_t compressed[w * h / 2];
            for (int i = 0; i < h; i += 4)
                for (int j = 0; j < w; j += 4)
                    memcpy(compressed + ((i / 4) * (w / 4) + j / 4) * 8, temp + (((i / 8) * (w / 8) + j / 8) * 4 + (i % 8 / 4 * 2 + j % 8 / 4)) * 8, 8);

            int offset = 0;
            uint8_t *decoded = malloc(texture->decoded_data_size);
            for (int y = 0; y < h / 4; y++)
            {
                for (int x = 0; x < w / 4; x++)
                {
                    uint32_t block1 = read_etc_word(compressed, &offset);
                    uint32_t block2 = read_etc_word(compressed, &offset);
                    decompressBlockETC2(block1, block2, decoded, w, h, x * 4, y * 4);
                }
            }

            return decoded;
        }
        case CTPK_TEXTURE_FORMAT_ETC1_A4:
        {
            uint8_t rgb_compressed_temp[w * h / 2];
            for (int i = 0; i < w * h / 2 / 8; i++)
                for (int j = 0; j < 8; j++)
                    rgb_compressed_temp[i * 8 + j] = raw_data[8 + i * 16 + 7 - j];

            uint8_t rgb_compressed[w * h / 2];
            for (int i = 0; i < h; i += 4)
                for (int j = 0; j < w; j += 4)
                    memcpy(rgb_compressed + ((i / 4) * (w / 4) + j / 4) * 8, rgb_compressed_temp + (((i / 8) * (w / 8) + j / 8) * 4 + (i % 8 / 4 * 2 + j % 8 / 4)) * 8, 8);

            uint8_t alpha_temp[w * h];
            for (int i = 0; i < w * h / 16; i++)
            {
                for (int j = 0; j < 4; j++)
                {
                    alpha_temp[i * 16 + j] = (raw_data[i * 16 + j * 2] & 0x0f) * 0x11;
                    alpha_temp[i * 16 + j + 4] = (raw_data[i * 16 + j * 2] >> 4 & 0x0f) * 0x11;
                    alpha_temp[i * 16 + j + 8] = (raw_data[i * 16 + j * 2 + 1] & 0x0f) * 0x11;
                    alpha_temp[i * 16 + j + 12] = (raw_data[i * 16 + j * 2 + 1] >> 4 & 0x0f) * 0x11;
                }
            }

            uint8_t alpha[w * h];
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    alpha[i * w + j] = alpha_temp[(((i / 8) * (w / 8) + j / 8) * 4 + i % 8 / 4 * 2 + j % 8 / 4) * 16 + i % 4 * 4 + j % 4];

            int offset = 0;
            uint8_t rgb[w * h * 3];
            for (int y = 0; y < h / 4; y++)
            {
                for (int x = 0; x < w / 4; x++)
                {
                    uint32_t block1 = read_etc_word(rgb_compressed, &offset);
                    uint32_t block2 = read_etc_word(rgb_compressed, &offset);
                    decompressBlockETC2(block1, block2, rgb, w, h, x * 4, y * 4);
                }
            }

            uint8_t *decoded = malloc(texture->decoded_data_size);
            for (int i = 0; i < w * h; i++)
            {
                int rgb_index = i * 3;
                int alpha_index = i;
                int decoded_index = i * 4;

                decoded[decoded_index + 0] = rgb[rgb_index + 0];
                decoded[decoded_index + 1] = rgb[rgb_index + 1];
                decoded[decoded_index + 2] = rgb[rgb_index + 2];
                decoded[decoded_index + 3] = alpha[alpha_index];
            }

            return decoded;
        }
    }
}
