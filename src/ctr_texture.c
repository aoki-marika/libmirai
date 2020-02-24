//
//  texture.c
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "ctr_texture.h"

#include <string.h>
#include <math.h>

#include "etcdec.h"

// MARK: - Constants

/// Translation bytes used for decoding texture data.
const int ctr_texture_translation_bytes[64] =
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

// MARK: - Functions

/// Read an ETC compressed word from the given bytes at the given offset.
/// @param source The array of bytes to read the word from.
/// @param offset The offset in the source to read the word from, modified to increment past the word once it's read.
/// @returns The 32bit ETC word from the given source at the given offset.
uint32_t ctr_texture_etc_read_word(const uint8 *source, int *offset)
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

void ctr_texture_create(unsigned int width,
                        unsigned int height,
                        size_t data_size,
                        size_t data_pointer,
                        enum ctr_texture_format_t data_format,
                        struct ctr_texture_t *texture)
{
    // calculate the decoded data size
    size_t w = width;
    size_t h = height;
    size_t decoded_data_size;
    switch (data_format)
    {
        case CTR_TEXTURE_FORMAT_RGBA8888: decoded_data_size = w * h * 4; break;
        case CTR_TEXTURE_FORMAT_RGB888:   decoded_data_size = w * h * 3; break;
        case CTR_TEXTURE_FORMAT_RGBA5551:
        case CTR_TEXTURE_FORMAT_RGB565:
        case CTR_TEXTURE_FORMAT_RGBA4444: decoded_data_size = w * h * 2; break;
        case CTR_TEXTURE_FORMAT_LA88:
        case CTR_TEXTURE_FORMAT_HL8:      decoded_data_size = w * h * 2; break;
        case CTR_TEXTURE_FORMAT_L8:
        case CTR_TEXTURE_FORMAT_A8:
        case CTR_TEXTURE_FORMAT_LA44:     decoded_data_size = w * h;     break;
        case CTR_TEXTURE_FORMAT_L4:
        case CTR_TEXTURE_FORMAT_A4:       decoded_data_size = w * h;     break;
        case CTR_TEXTURE_FORMAT_ETC1:     decoded_data_size = w * h * 3; break;
        case CTR_TEXTURE_FORMAT_ETC1_A4:  decoded_data_size = w * h * 4; break;
    }

    // fill in the given texture
    texture->width = width;
    texture->height = height;
    texture->data_size = data_size;
    texture->data_pointer = data_pointer;
    texture->data_format = data_format;
    texture->decoded_data_size = decoded_data_size;
    texture->unpacked_data_size = width * height * 4;
}

uint8_t *ctr_texture_decode(const struct ctr_texture_t *texture, FILE *file)
{
    // these are all translations from dnasdws ctpktool project
    // all credits to them for the implementations
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
        case CTR_TEXTURE_FORMAT_RGBA8888:
        {
            uint8_t temp[w * h * 4];
            for (int i = 0; i < w * h / 64; i++)
                for (int j = 0; j < 64; j++)
                    for (int k = 0; k < 4; k++)
                        temp[(i * 64 + j) * 4 + k] = raw_data[(i * 64 + ctr_texture_translation_bytes[j]) * 4 + 3 - k];

            uint8_t *decoded = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    for (int k = 0; k < 4; k++)
                        decoded[(i * w + j) * 4 + k] = temp[(((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 4 + k];

            return decoded;
        }
        case CTR_TEXTURE_FORMAT_RGB888:
        {
            uint8_t temp[w * h * 3];
            for (int i = 0; i < w * h / 64; i++)
                for (int j = 0; j < 64; j++)
                    for (int k = 0; k < 3; k++)
                        temp[(i * 64 + j) * 3 + k] = raw_data[(i * 64 + ctr_texture_translation_bytes[j]) * 3 + 2 - k];

            uint8_t *decoded = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    for (int k = 0; k < 3; k++)
                        decoded[(i * w + j) * 3 + k] = temp[(((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 3 + k];

            return decoded;
        }
        case CTR_TEXTURE_FORMAT_RGBA5551:
        case CTR_TEXTURE_FORMAT_RGB565:
        case CTR_TEXTURE_FORMAT_RGBA4444:
        {
            uint8_t temp[w * h * 2];
            for (int i = 0; i < w * h / 64; i++)
                for (int j = 0; j < 64; j++)
                    for (int k = 0; k < 2; k++)
                        temp[(i * 64 + j) * 2 + k] = raw_data[(i * 64 + ctr_texture_translation_bytes[j]) * 2 + k];

            uint8_t *decoded = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    for (int k = 0; k < 2; k++)
                        decoded[(i * w + j) * 2 + k] = temp[(((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 2 + k];

            return decoded;
        }
        case CTR_TEXTURE_FORMAT_LA88:
        case CTR_TEXTURE_FORMAT_HL8:
        {
            uint8_t temp[w * h * 2];
            for (int i = 0; i < w * h / 64; i++)
                for (int j = 0; j < 64; j++)
                    for (int k = 0; k < 2; k++)
                        temp[(i * 64 + j) * 2 + k] = raw_data[(i * 64 + ctr_texture_translation_bytes[j]) * 2 + 1 - k];

            uint8_t *decoded = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    for (int k = 0; k < 2; k++)
                        decoded[(i * w + j) * 2 + k] = temp[(((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 2 + k];

            return decoded;
        }
        case CTR_TEXTURE_FORMAT_L8:
        case CTR_TEXTURE_FORMAT_A8:
        case CTR_TEXTURE_FORMAT_LA44:
        {
            uint8_t temp[w * h];
            for (int i = 0; i < w * h / 64; i++)
                for (int j = 0; j < 64; j++)
                    temp[i * 64 + j] = raw_data[i * 64 + ctr_texture_translation_bytes[j]];

            uint8_t *decoded = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    decoded[i * w + j] = temp[((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8];

            return decoded;
        }
        case CTR_TEXTURE_FORMAT_L4:
        case CTR_TEXTURE_FORMAT_A4:
        {
            uint8_t temp[w * h];
            for (int i = 0; i < w * h / 64; i++)
            {
                for (int j = 0; j < 64; j += 2)
                {
                    temp[i * 64 + j] = (raw_data[i * 32 + ctr_texture_translation_bytes[j] / 2] & 0xF) * 0x11;
                    temp[i * 64 + j + 1] = (raw_data[i * 32 + ctr_texture_translation_bytes[j] / 2] >> 4 & 0xF) * 0x11;
                }
            }

            uint8_t *decoded = malloc(texture->decoded_data_size);
            for (int i = 0; i < h; i++)
                for (int j = 0; j < w; j++)
                    decoded[i * w + j] = temp[((i / 8) * (w / 8) + j / 8) * 64 + i % 8 * 8 + j % 8];

            return decoded;
        }
        case CTR_TEXTURE_FORMAT_ETC1:
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
                    uint32_t block1 = ctr_texture_etc_read_word(compressed, &offset);
                    uint32_t block2 = ctr_texture_etc_read_word(compressed, &offset);
                    decompressBlockETC2(block1, block2, decoded, w, h, x * 4, y * 4);
                }
            }

            return decoded;
        }
        case CTR_TEXTURE_FORMAT_ETC1_A4:
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
                    uint32_t block1 = ctr_texture_etc_read_word(rgb_compressed, &offset);
                    uint32_t block2 = ctr_texture_etc_read_word(rgb_compressed, &offset);
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

/// Convert and rescale an unsigned integer of a variable bit size to an unsigned 8-bit integer.
/// @param un The value to convert.
/// @param size The bit size of the given value.
/// @returns The rescaled 8-bit unsigned value of the given value.
uint8_t ctr_texture_un_to_u8(uint8_t un, unsigned int size)
{
    uint8_t max = pow(2, size) - 1;
    double normalized = (double)un / max;

    uint8_t u8 = (uint8_t)(normalized * UINT8_MAX);
    return u8;
}

/// Unpack a pixel from decoded CTR texture data.
/// @param pixel The index of the pixel to unpack.
/// @param format The data format of the given decoded data.
/// @param decoded The array containing the decoded data to unpack.
/// @param unpacked The 8-bit RGBA array to write the unpacked data to.
void ctr_texture_pixel_unpack(unsigned int pixel,
                              enum ctr_texture_format_t format,
                              const uint8_t *decoded,
                              uint8_t *unpacked)
{
    unsigned int unpacked_offset = pixel * 4;

    // get the red, green, blue, and alpha channels
    uint8_t red, green, blue, alpha;
    switch (format)
    {
        case CTR_TEXTURE_FORMAT_RGBA8888:
        case CTR_TEXTURE_FORMAT_ETC1_A4:
        {
            unsigned int decoded_offset = pixel * 4;
            red   = decoded[decoded_offset + 0];
            blue  = decoded[decoded_offset + 1];
            green = decoded[decoded_offset + 2];
            alpha = decoded[decoded_offset + 3];
            break;
        }
        case CTR_TEXTURE_FORMAT_RGB888:
        case CTR_TEXTURE_FORMAT_ETC1:
        {
            unsigned int decoded_offset = pixel * 3;
            red   = decoded[decoded_offset + 0];
            blue  = decoded[decoded_offset + 1];
            green = decoded[decoded_offset + 2];
            alpha = 0xff;
            break;
        }
        case CTR_TEXTURE_FORMAT_RGBA5551:
        {
            uint16_t value = ((uint16_t *)decoded)[pixel];
            red   = ctr_texture_un_to_u8((value >> 11) & 0b11111, 5);
            green = ctr_texture_un_to_u8((value >> 6) & 0b11111, 5);
            blue  = ctr_texture_un_to_u8((value >> 1) & 0b11111, 5);
            alpha = ctr_texture_un_to_u8((value >> 0) & 0b1, 1);
            break;
        }
        case CTR_TEXTURE_FORMAT_RGB565:
        {
            uint16_t value = ((uint16_t *)decoded)[pixel];
            red   = ctr_texture_un_to_u8((value >> 11) & 0b11111, 5);
            green = ctr_texture_un_to_u8((value >> 5) & 0b111111, 6);
            blue  = ctr_texture_un_to_u8(value & 0b11111, 5);
            alpha = 0xff;
            break;
        }
        case CTR_TEXTURE_FORMAT_RGBA4444:
        {
            uint16_t value = ((uint16_t *)decoded)[pixel];
            red   = ctr_texture_un_to_u8((value >> 12) & 0b1111, 4);
            green = ctr_texture_un_to_u8((value >> 4) & 0b1111, 4);
            blue  = ctr_texture_un_to_u8((value >> 8) & 0b1111, 4);
            alpha = ctr_texture_un_to_u8((value >> 0) & 0b1111, 4);
            break;
        }
        case CTR_TEXTURE_FORMAT_LA88:
        {
            unsigned int decoded_offset = pixel * 2;
            uint8_t luminance = decoded[decoded_offset + 0];
            red   = luminance;
            green = luminance;
            blue  = luminance;
            alpha = decoded[decoded_offset + 1];
            break;
        }
        case CTR_TEXTURE_FORMAT_HL8:
        {
            // unsure how to properly unpack this, leave a warning for now
            fprintf(stderr, "WARNING: unable to correctly unpack HL8 CTR texture data format");
            red =   0xff;
            green = 0x00;
            blue =  0xff;
            alpha = 0xff;
            break;
        }
        case CTR_TEXTURE_FORMAT_L8:
        case CTR_TEXTURE_FORMAT_L4:
        {
            red   = decoded[pixel];
            green = decoded[pixel];
            blue  = decoded[pixel];
            alpha = 0xff;
            break;
        }
        case CTR_TEXTURE_FORMAT_A8:
        case CTR_TEXTURE_FORMAT_A4:
        {
            red   = 0xff;
            green = 0xff;
            blue  = 0xff;
            alpha = decoded[pixel];
            break;
        }
        case CTR_TEXTURE_FORMAT_LA44:
        {
            uint8_t value = decoded[pixel];
            uint8_t luminance = ctr_texture_un_to_u8((value >> 4) & 0b1111, 4);
            red   = luminance;
            green = luminance;
            blue  = luminance;
            alpha = ctr_texture_un_to_u8(value & 0b1111, 4);
            break;
        }
    }

    // set the channels
    unpacked[unpacked_offset + 0] = red;
    unpacked[unpacked_offset + 1] = blue;
    unpacked[unpacked_offset + 2] = green;
    unpacked[unpacked_offset + 3] = alpha;
}

uint8_t *ctr_texture_unpack(const struct ctr_texture_t *texture,
                            const uint8_t *decoded)
{
    // unpack the decoded data
    // width * height * 4 channels (rgba)
    uint8_t *unpacked = malloc(texture->unpacked_data_size);

    for (int y = 0; y < texture->height; y++)
    {
        for (int x = 0; x < texture->width; x++)
        {
            int pixel = (y * texture->width) + x;
            ctr_texture_pixel_unpack(pixel,
                                     texture->data_format,
                                     decoded,
                                     unpacked);
        }
    }

    return unpacked;
}
