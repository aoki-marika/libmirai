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
struct ctpk_header
{
    /// The `CTPK` signature of this file.
    uint32_t signature;

    /// The CTPK format version that this file uses.
    uint16_t version;

    /// The number of textures within this file.
    uint16_t num_textures;

    /// The pointer to the beginning of the texture data within this file.
    uint32_t textures_pointer;

    /// The size, in bytes, of the texture data within this file.
    uint32_t textures_size;

    /// The pointer to the hash section within this file.
    uint32_t hashes_pointer;

    /// The pointer to the beggining of the hash section within this file.
    uint32_t conversion_pointer;

    /// Reserved
    uint64_t reserved;
};

/// The data structure for the header of a single texture within a CTPK file.
struct ctpk_texture_header
{
    uint32_t file_path_pointer;
    uint32_t data_size;
    uint32_t data_pointer;
    uint32_t format;
    uint16_t width;
    uint16_t height;
    uint8_t mipmap_level;
    uint8_t type;
    uint16_t cube_direction;
    uint32_t bitmap_size_pointer;
    uint32_t timestamp;
};

/// The data structure for the conversion info of a single texture within a CTPK file.
struct ctpk_texture_conversion_info
{
    uint8_t format;
    uint8_t mipmap_level;
    uint8_t compression;
    uint8_t compression_method;
};

// MARK: - Functions

void ctpk_open(FILE *input, struct ctpk_file *output)
{
    // read the start of this file relative to the file handle
    unsigned long long start_pointer = ftell(input);

    // read the header
    // the signature is CTPK in ascii
    // but since the byte order is swapped when reading it,
    // it is actually KPTC
    struct ctpk_header header;
    fread(&header, sizeof(header), 1, input);
    assert(header.signature == 0x4b505443);

    // initialize the output file
    output->num_textures = header.num_textures;
    output->textures = malloc(header.num_textures * sizeof(struct ctpk_texture *));

    // read all the textures
    for (unsigned int i = 0; i < header.num_textures; i++)
    {
        // read the header
        struct ctpk_texture_header texture_header;
        fread(&texture_header, sizeof(texture_header), 1, input);

        // read the conversion info
        struct ctpk_texture_conversion_info conversion_info;
        fread(&conversion_info, sizeof(conversion_info), 1, input);

        // create the texture
        struct ctpk_texture *texture = malloc(sizeof(struct ctpk_texture));
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

        output->textures[i] = texture;
    }
}

void ctpk_close(struct ctpk_file *file)
{
    for (unsigned int i = 0; i < file->num_textures; i++)
        free(file->textures[i]);

    free(file->textures);
}

uint8_t *ctpk_texture_decode(const struct ctpk_texture *texture, FILE *source)
{
    // these are all translations from dnasdws ctpktool project
    // all credits to them for creating the implementations
    // https://github.com/dnasdw/ctpktool/blob/master/src/ctpk.cpp

    // read the data to decode
    uint8_t raw_data[texture->data_size];
    fseek(source, texture->data_pointer, SEEK_SET);
    fread(raw_data, texture->data_size, 1, source);

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

            uint8_t *rgba = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    for (int k = 0; k < 4; k++)
                        rgba[(i * w + j) * 4 + k] = temp[(((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 4 + k];

            return rgba;
        }
        case CTPK_TEXTURE_FORMAT_RGB888:
        {
            uint8_t temp[w * h * 3];
            for (int i = 0; i < w * h / 64; i++)
                for (int j = 0; j < 64; j++)
                    for (int k = 0; k < 3; k++)
                        temp[(i * 64 + j) * 3 + k] = raw_data[(i * 64 + trans_bytes[j]) * 3 + 2 - k];

            uint8_t *rgba = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    for (int k = 0; k < 3; k++)
                        rgba[(i * w + j) * 3 + k] = temp[(((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 3 + k];

            return rgba;
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

            uint8_t *rgba = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    for (int k = 0; k < 2; k++)
                        rgba[(i * w + j) * 2 + k] = temp[(((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 2 + k];

            return rgba;
        }
        case CTPK_TEXTURE_FORMAT_LA88:
        case CTPK_TEXTURE_FORMAT_HL8:
        {
            uint8_t temp[w * h * 2];
            for (int i = 0; i < w * h / 64; i++)
                for (int j = 0; j < 64; j++)
                    for (int k = 0; k < 2; k++)
                        temp[(i * 64 + j) * 2 + k] = raw_data[(i * 64 + trans_bytes[j]) * 2 + 1 - k];

            uint8_t *rgba = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    for (int k = 0; k < 2; k++)
                        rgba[(i * w + j) * 2 + k] = temp[(((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 2 + k];

            return rgba;
        }
        case CTPK_TEXTURE_FORMAT_L8:
        case CTPK_TEXTURE_FORMAT_A8:
        case CTPK_TEXTURE_FORMAT_LA44:
        {
            uint8_t temp[w * h];
            for (int i = 0; i < w * h / 64; i++)
                for (int j = 0; j < 64; j++)
                    temp[i * 64 + j] = raw_data[i * 64 + trans_bytes[j]];

            uint8_t *rgba = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    rgba[i * w + j] = temp[((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8];

            return rgba;
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

            uint8_t *rgba = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    rgba[i * w + j] = temp[((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8];

            return rgba;
        }
        case CTPK_TEXTURE_FORMAT_ETC1:
        {
            uint8_t temp[w * h / 2];
            for (int i = 0; i < w * h / 2 / 8; i++)
                for (int j = 0; j < 8; j++)
                    temp[i * 8 + j] = raw_data[i * 8 + 7 - j];

            uint8_t *rgba = malloc(w * h * 4);
            for (int i = 0; i < h; i += 4)
                for (int j = 0; j < h; j += 4)
                    memcpy(rgba + ((i / 4) * (w / 4) + j / 4) * 8, temp + (((i / 8) * (w / 8) + j / 8) * 4 + (i % 8 / 4 * 2 + j % 8 / 4)) * 8, 8);

            return rgba;
        }
        case CTPK_TEXTURE_FORMAT_ETC1_A4:
        {
            uint8_t rgba_temp[w * h / 2];
            for (int i = 0; i < w * h / 2 / 8; i++)
                for (int j = 0; j < 8; j++)
                    rgba_temp[i * 8 + j] = raw_data[8 + i * 16 + 7 - j];

            uint8_t *rgba = malloc(w * h / 2);
            for (int i = 0; i < h; i += 4)
                for (int j = 0; j < w; j += 4)
                    memcpy(rgba + ((i / 4) * (w / 4) + j / 4) * 8, rgba_temp + (((i / 8) * (w / 8) + j / 8) * 4 + (i % 8 / 4 * 2 + j % 8 / 4)) * 8, 8);

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

            uint8_t *alpha = malloc(w * h);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    alpha[i * w + j] = alpha_temp[(((i / 8) * (w / 8) + j / 8) * 4 + i % 8 / 4 * 2 + j % 8 / 4) * 16 + i % 4 * 4 + j % 4];

            return NULL;
        }
    }
}
