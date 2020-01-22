//
//  mtob.c
//  libmirai
//
//  Created by Marika on 2020-01-19.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "mtob.h"

#include <stdlib.h>
#include <assert.h>

#include "utils.h"

// MARK: - Functions

void mtob_open(FILE *file, struct mtob_t *mtob)
{
    // u32 preceding flags, unused
    fseek(file, 4, SEEK_CUR);

    // read the signature
    assert(fgetc(file) == 'M');
    assert(fgetc(file) == 'T');
    assert(fgetc(file) == 'O');
    assert(fgetc(file) == 'B');

    // u32 revision, unused
    fseek(file, 4, SEEK_CUR);

    // read the name and seek back
    uint32_t name_pointer = utils_read_relative_pointer(file);
    long name_return = ftell(file);
    fseek(file, name_pointer, SEEK_SET);
    char *name = utils_read_string(file);
    fseek(file, name_return, SEEK_SET);

    // unknown and unused values
    //  - 2x u32 unknown
    //  - u32 flags
    fseek(file, 3 * 4, SEEK_CUR);

    // u32 texcoord config and translucency kind, unused
    fseek(file, 2 * 4, SEEK_CUR);

    // read the colours
    // the colours are stored twice;
    // once as rgba floats, then again as rgba u8s
    // so take the floats and skip the u8s

    // read the float colours
    struct color4_t emission, ambient, diffuse;
    color4_readf(file, &emission);
    color4_readf(file, &ambient);
    color4_readf(file, &diffuse);

    struct color4_t specular0, specular1;
    color4_readf(file, &specular0);
    color4_readf(file, &specular1);

    struct color4_t constant0, constant1, constant2, constant3, constant4, constant5;
    color4_readf(file, &constant0);
    color4_readf(file, &constant1);
    color4_readf(file, &constant2);
    color4_readf(file, &constant3);
    color4_readf(file, &constant4);
    color4_readf(file, &constant5);

    // skip the u8 colours
    // 1 byte per channel, 4 channels, 11 colours
    fseek(file, 11 * 4, SEEK_CUR);

    // u32 command cache, unused
    fseek(file, 4, SEEK_CUR);

    // rasterization options, unused
    //  - u32 flags
    //  - u32 culling mode
    //  - u32 polygon offset unit
    //  - u32 command 1
    //  - u32 command 2
    fseek(file, 5 * 4, SEEK_CUR);

    // fragment operation
    // depth, unused
    //  - u32 flags
    //  - 4x u32 pica command
    fseek(file, 5 * 4, SEEK_CUR);

    // blend, unused
    //  - u32 mode
    //  - rgba floats blend color
    //  - 6x u32 pica command
    fseek(file, 11 * 4, SEEK_CUR);

    // stencil, unused
    //  - 4x u32 pica command
    fseek(file, 4 * 4, SEEK_CUR);

    // initialize the mtob
    mtob->name = name;
    mtob->colors.emission = emission;
    mtob->colors.ambient = ambient;
    mtob->colors.diffuse = diffuse;
    mtob->colors.specular0 = specular0;
    mtob->colors.specular1 = specular1;
    mtob->colors.constant0 = constant0;
    mtob->colors.constant1 = constant1;
    mtob->colors.constant2 = constant2;
    mtob->colors.constant3 = constant3;
    mtob->colors.constant4 = constant4;
    mtob->colors.constant5 = constant5;
}

void mtob_close(struct mtob_t *mtob)
{
    free(mtob->name);
}
