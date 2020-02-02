//
//  ctpk.c
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "ctpk.h"

#include <stdlib.h>
#include <assert.h>

// MARK: - Functions

void ctpk_open(FILE *file, struct ctpk_t *ctpk)
{
    // read the pointer of this ctpk relative to the file
    // this is used to get the absolute position of texture data
    long pointer = ftell(file);

    // read the signature
    assert(fgetc(file) == 'C');
    assert(fgetc(file) == 'T');
    assert(fgetc(file) == 'P');
    assert(fgetc(file) == 'K');

    // u16 version, unused
    fseek(file, 2, SEEK_CUR);

    // read the texture count and data base pointer
    uint16_t num_textures;
    fread(&num_textures, sizeof(num_textures), 1, file);

    uint32_t data_base_pointer;
    fread(&data_base_pointer, sizeof(data_base_pointer), 1, file);

    // multiple unused values
    //  - u32 total texture data size
    //  - u32 hash section pointer
    //  - u32 conversion info pointer
    //  - 8x byte padding
    fseek(file, (3 * 4) + 8, SEEK_CUR);

    // read the textures
    ctpk->num_textures = num_textures;
    ctpk->textures = malloc(num_textures * sizeof(struct texture_t));
    for (int i = 0; i < num_textures; i++)
    {
        // array
        // +32 for ctpk header size
        fseek(file, pointer + 32 + (i * 36), SEEK_SET);

        // u32 file path pointer, unused
        fseek(file, 4, SEEK_CUR);

        // read the data size and pointer
        // note that the data pointer is relative to the data base pointer
        uint32_t data_size, data_pointer;
        fread(&data_size, sizeof(data_size), 1, file);
        fread(&data_pointer, sizeof(data_pointer), 1, file);

        // read the data format
        enum texture_format_t data_format;
        fread(&data_format, sizeof(data_format), 1, file);

        // read the size
        uint16_t width, height;
        fread(&width, sizeof(width), 1, file);
        fread(&height, sizeof(height), 1, file);

        // multiple unused values
        //  - u8 mip level
        //  - u8 type
        //  - u16 cube map info
        //  - u32 bitmap size array pointer
        //  - u32 timestamp
        fseek(file, (1 * 2) + 2 + (2 * 4), SEEK_CUR);

        // insert the texture
        struct texture_t texture;
        texture_create(width,
                       height,
                       data_size,
                       pointer + data_base_pointer + data_pointer,
                       data_format,
                       &texture);

        ctpk->textures[i] = texture;
    }
}

void ctpk_close(struct ctpk_t *ctpk)
{
    free(ctpk->textures);
}
