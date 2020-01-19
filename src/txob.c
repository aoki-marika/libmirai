//
//  txob.c
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "txob.h"

#include <stdlib.h>
#include <assert.h>

#include "utils.h"

// MARK: - Functions

void txob_open(FILE *file, struct txob_t *txob)
{
    // u32 type, unused
    fseek(file, 4, SEEK_CUR);

    // read the signature
    assert(fgetc(file) == 'T');
    assert(fgetc(file) == 'X');
    assert(fgetc(file) == 'O');
    assert(fgetc(file) == 'B');

    // txob format revision, unused
    fseek(file, 4, SEEK_CUR);

    // read the name and seek back
    uint32_t name_pointer = utils_read_relative_pointer(file);
    long name_return = ftell(file);
    fseek(file, name_pointer, SEEK_SET);
    char *name = utils_read_string(file);
    fseek(file, name_return, SEEK_SET);

    // u32 user data entry count and pointer, unused
    fseek(file, 4 + 4, SEEK_CUR);

    // read the width and height
    uint32_t width, height;
    fread(&height, sizeof(height), 1, file);
    fread(&width, sizeof(width), 1, file);

    // series of unused values
    //  - u32 opengl texture format
    //  - u32 opengl texture type
    //  - u32 mipmap levels
    //  - u32 texture object
    //  - u32 location flags
    fseek(file, 4 + 4 + 4 + 4 + 4, SEEK_CUR);

    // read the texture format
    enum texture_format_t format;
    fread(&format, sizeof(format), 1, file);

    // three unknown u32s
    fseek(file, 4 + 4 + 4, SEEK_CUR);

    // read the data size and pointer
    uint32_t data_size;
    fread(&data_size, sizeof(data_size), 1, file);

    uint32_t data_pointer = utils_read_relative_pointer(file);

    // another series of unused values
    // these are assumingly used for laying out the texture in 3ds memory
    //  - u32 dynamic allocator
    //  - u32 bits per pixel
    //  - u32 location address
    //  - u32 memory address
    fseek(file, 4 + 4 + 4 + 4, SEEK_CUR);

    // create the txob
    txob->name = name;
    texture_create(width,
                   height,
                   data_size,
                   data_pointer,
                   format,
                   &txob->texture);
}

void txob_close(struct txob_t *txob)
{
    free(txob->name);
}
