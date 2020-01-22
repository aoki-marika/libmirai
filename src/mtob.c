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

    // read the active texture coordinator count
    uint32_t num_active_texture_coordinators;
    fread(&num_active_texture_coordinators, sizeof(num_active_texture_coordinators), 1, file);

    // read the texture coordinators
    mtob->num_active_texture_coordinators = num_active_texture_coordinators;
    for (int i = 0; i < 3; i++)
    {
        // read the source index
        uint32_t source_index;
        fread(&source_index, sizeof(source_index), 1, file);

        // read the mapping method
        enum mtob_texture_coordinator_mapping_method_t mapping_method;
        fread(&mapping_method, sizeof(mapping_method), 1, file);

        // u32 reference camera and matrix mode, unused
        fseek(file, 2 * 4, SEEK_CUR);

        // read the scale, rotation, and translate
        struct vec2_t scale, translate;
        float rotation;
        vec2_read(file, &scale);
        fread(&rotation, sizeof(rotation), 1, file);
        vec2_read(file, &translate);

        // u32 flags, unused
        fseek(file, 4, SEEK_CUR);

        // read the transform
        struct mat4_t transform;
        mat4_read43(file, &transform);

        // insert the coordinator
        struct mtob_texture_coordinator_t *coordinator = &mtob->texture_coordinators[i];
        coordinator->source_index = source_index;
        coordinator->mapping_method = mapping_method;
        coordinator->scale = scale;
        coordinator->rotation = rotation;
        coordinator->translate = translate;
        coordinator->transform = transform;
    }

    // read the texture mappers
    // keep a pointer to the pointer table to be able to continue reading after each texture
    long texture_table_pointer = ftell(file);
    int texture_mapper_index = 0;
    for (int i = 0; i < 3; i++)
    {
        // read the pointer from the table
        // if the relative pointer is zero then there is no texture at this index
        fseek(file, texture_table_pointer + (i * 4), SEEK_SET);

        uint32_t relative_pointer;
        fread(&relative_pointer, sizeof(relative_pointer), 1, file);
        if (relative_pointer == 0)
            continue;

        fseek(file, -4, SEEK_CUR);
        uint32_t pointer = utils_read_relative_pointer(file);
        fseek(file, pointer, SEEK_SET);

        // u32 type and dynamic allocator, unused
        fseek(file, 2 * 4, SEEK_CUR);

        // read the texture pointer
        uint32_t texture_pointer = utils_read_relative_pointer(file);

        // multiple unused values
        //  - u32 sampler pointer
        //  - 5x u32 pica command
        fseek(file, 6 * 4, SEEK_CUR);

        // read the wrap modes
        uint32_t wrap_flags;
        fread(&wrap_flags, sizeof(wrap_flags), 1, file);

        enum mtob_texture_mapper_wrap_mode_t wrap_u = (wrap_flags >> 12) & 0xf;
        enum mtob_texture_mapper_wrap_mode_t wrap_v = (wrap_flags >> 8) & 0xf;

        // 8x u32 pica command and u32 "command size to send"
        fseek(file, 9 * 4, SEEK_CUR);

        // read the texture txob
        // txobs have support for different types, one of which being a reference
        // which then refers to another txob by name
        // this is what mtobs use to refer to their textures
        // so instead of updating txob with all the clutter of supporting these types,
        // instead just read the referenced name here and store it in the mtob to be handled by the cgfx consumer
        // +6x u32 to skip the header and go straight to the txob name
        fseek(file, texture_pointer + (6 * 4), SEEK_SET);
        uint32_t texture_name_pointer = utils_read_relative_pointer(file);
        long texture_name_return = ftell(file);
        fseek(file, texture_name_pointer, SEEK_SET);
        char *texture_name = utils_read_string(file);
        fseek(file, texture_name_return, SEEK_SET);

        // insert the texture mapper
        struct mtob_texture_mapper_t *mapper = &mtob->texture_coordinators[texture_mapper_index].mapper;
        mapper->texture_name = texture_name;
        mapper->wrap_u = wrap_u;
        mapper->wrap_v = wrap_v;
        texture_mapper_index++;
    }
}

void mtob_close(struct mtob_t *mtob)
{
    for (int i = 0; i < mtob->num_active_texture_coordinators; i++)
        free(mtob->texture_coordinators[i].mapper.texture_name);

    free(mtob->name);
}
