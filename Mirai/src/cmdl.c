//
//  cmdl.c
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "cmdl.h"

#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include "dict.h"
#include "sobj.h"
#include "utils.h"
#include "vector.h"
#include "matrix.h"

// MARK: - Functions

void cmdl_open(FILE *file, struct cmdl_t *cmdl)
{
    // read the preceeding flags
    //  - bit 7: whether there is a special sobj for a skeleton
    uint32_t flags;
    fread(&flags, sizeof(flags), 1, file);

    bool has_skeleton_sobj = flags & 0b10000000;

    // read the signature
    assert(fgetc(file) == 'C');
    assert(fgetc(file) == 'M');
    assert(fgetc(file) == 'D');
    assert(fgetc(file) == 'L');

    // u32 unknown
    fseek(file, 4, SEEK_CUR);

    // read the name and seek back
    uint32_t name_pointer = utils_read_relative_pointer(file);
    long name_return = ftell(file);
    fseek(file, name_pointer, SEEK_SET);
    char *name = utils_read_string(file);
    fseek(file, name_return, SEEK_SET);

    // 24 unknown bytes
    fseek(file, 24, SEEK_CUR);

    // read the 0animation types dict
    struct dict_t animation_types_dict;
    dict_open(file, &animation_types_dict);

    // read the scale, rotation, and translation vectors
    struct vec3_t scale, rotation, translation;
    utils_read_vec3(file, &scale);
    utils_read_vec3(file, &rotation);
    utils_read_vec3(file, &translation);

    // read the matrices
    struct mat4_t matrix_local, matrix_world;
    utils_read_mat4(file, &matrix_local);
    utils_read_mat4(file, &matrix_world);

    // read the vertex info sobj count and pointer
    uint32_t num_vertex_info_sobjs;
    fread(&num_vertex_info_sobjs, sizeof(num_vertex_info_sobjs), 1, file);

    uint32_t vertex_info_sobjs_pointer = utils_read_relative_pointer(file);

    // read the mtobs dict
    struct dict_t mtobs;
    dict_open(file, &mtobs);

    // read the vertex sobj count and pointer
    uint32_t num_vertex_sobjs;
    fread(&num_vertex_sobjs, sizeof(num_vertex_sobjs), 1, file);

    uint32_t vertex_sobjs_pointer = utils_read_relative_pointer(file);

    // dict unknown and 12 unknown bytes
    // ohana3ds refers to this dict as object nodes,
    // each entry is a name and visibility flag
    fseek(file, 8 + 12, SEEK_CUR);

    // read the skeleton sobj pointer
    // only present if the skeleton sobj flag was set
    uint32_t skeleton_sobj_pointer = 0;
    if (has_skeleton_sobj)
        skeleton_sobj_pointer = utils_read_relative_pointer(file);

    // according to 3dbrew, the next 0x4*N bytes are "Vertex Info SOBJ self-relative offset list"
    // pretty sure this is legacy as theres already a vertex info dict with all the offsets needed
    // so ignore it and use the dict instead

    // TODO: REMOVEME
    printf("       - name: \"%s\"\n", name);
    printf("       - has skeleton: %i\n", has_skeleton_sobj);
    printf("       - animation types: %i\n", animation_types_dict.num_entries);
    printf("       - scale: %f,%f,%f\n", scale.x, scale.y, scale.z);
    printf("       - rotation: %f,%f,%f\n", rotation.x, rotation.y, rotation.z);
    printf("       - translation: %f,%f,%f\n", translation.x, translation.y, translation.z);
    printf("       - vertex info sobjs: %u at %u (%08x)\n", num_vertex_info_sobjs, vertex_info_sobjs_pointer, vertex_info_sobjs_pointer);
    printf("       - mtobs: %u\n", mtobs.num_entries);
    printf("       - vertex sobjs: %u at %u (%08x)\n", num_vertex_sobjs, vertex_sobjs_pointer, vertex_sobjs_pointer);

    if (has_skeleton_sobj)
        printf("       - skeleton sobj: 1 at %u (%08x)\n", skeleton_sobj_pointer, skeleton_sobj_pointer);

    printf("       - %u vertex info sobjs:\n", num_vertex_info_sobjs);
    for (int i = 0; i < num_vertex_info_sobjs; i++)
    {
        fseek(file, vertex_info_sobjs_pointer + (i * 4), SEEK_SET);
        fseek(file, utils_read_relative_pointer(file), SEEK_SET);

        printf("          - vertex info sobj %u:\n", i);
        struct sobj_t sobj;
        sobj_open(file, &sobj);
    }

    printf("       - %u vertex sobjs:\n", num_vertex_sobjs);
    for (int i = 0; i < num_vertex_sobjs; i++)
    {
        fseek(file, vertex_sobjs_pointer + (i * 4), SEEK_SET);
        fseek(file, utils_read_relative_pointer(file), SEEK_SET);

        printf("          - vertex sobj %u:\n", i);
        struct sobj_t sobj;
        sobj_open(file, &sobj);
    }

    if (has_skeleton_sobj)
    {
        printf("       - 1 skeleton sobj:\n");
        fseek(file, skeleton_sobj_pointer, SEEK_SET);

        printf("          - skeleton sobj 1:\n");
        struct sobj_t sobj;
        sobj_open(file, &sobj);
    }

    dict_close(&mtobs);
    dict_close(&animation_types_dict);
    free(name);
}

void cmdl_close(struct cmdl_t *cmdl)
{
}
