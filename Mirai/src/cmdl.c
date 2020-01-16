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
#include "utils.h"
#include "vector.h"
#include "matrix.h"

// MARK: - Functions

/// Read the given number of SOBJs from the given pointer table, of the given type, into the given array.
/// @param file The file handle to read the SOBJs from.
/// @param num_sobjs The number of SOBJs to read.
/// @param pointer The pointer to the pointer table for the SOBJs.
/// @param expected_type The expected type that each SOBJ should be.
/// @param out The array to insert the read SOBJs into.
/// This array must of be size `num_sobjs * sizeof(struct sobj_t *)`.
/// Each item is allocated.
void cmdl_read_sobj_table(FILE *file,
                          unsigned int num_sobjs,
                          unsigned int pointer,
                          enum sobj_type_t expected_type,
                          struct sobj_t **out)
{
    for (int i = 0; i < num_sobjs; i++)
    {
        // pointer table
        fseek(file, pointer + (i * 4), SEEK_SET);
        fseek(file, utils_read_relative_pointer(file), SEEK_SET);

        // read the sobj
        struct sobj_t *sobj = malloc(sizeof(struct sobj_t));
        sobj_open(file, sobj);
        assert(sobj->type == expected_type);

        // add the sobj
        out[i] = sobj;
    }
}

void cmdl_open(FILE *file, struct cmdl_t *cmdl)
{
    // read the preceeding flags
    //  - bit 7: whether there is an sobj for a skeleton
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

    // read the animation types dict
    // TODO: Read the animation types dict
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

    // read the mesh count and pointer
    uint32_t num_meshes;
    fread(&num_meshes, sizeof(num_meshes), 1, file);

    uint32_t meshes_pointer = utils_read_relative_pointer(file);

    // read the mtobs dict
    struct dict_t mtobs;
    dict_open(file, &mtobs);

    // read the shape count and pointer
    uint32_t num_shapes;
    fread(&num_shapes, sizeof(num_shapes), 1, file);

    uint32_t shapes_pointer = utils_read_relative_pointer(file);

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

    // initialize the cmdl
    cmdl->name = name;

    // read the meshes
    cmdl->num_meshes = num_meshes;
    cmdl->meshes = malloc(num_meshes * sizeof(struct sobj_t *));
    cmdl_read_sobj_table(file,
                         num_meshes,
                         meshes_pointer,
                         SOBJ_TYPE_MESH,
                         cmdl->meshes);

    // read the shapes
    cmdl->num_shapes = num_shapes;
    cmdl->shapes = malloc(num_shapes * sizeof(struct sobj_t *));
    cmdl_read_sobj_table(file,
                         num_shapes,
                         shapes_pointer,
                         SOBJ_TYPE_SHAPE,
                         cmdl->shapes);

    // read the skeleton
    if (has_skeleton_sobj)
    {
        fseek(file, skeleton_sobj_pointer, SEEK_SET);

        struct sobj_t *sobj = malloc(sizeof(struct sobj_t));
        sobj_open(file, sobj);
        cmdl->skeleton = sobj;
    }
    else
    {
        cmdl->skeleton = NULL;
    }

    // TODO: Remove when MTOB and animation types dict reading is implemented
    dict_close(&mtobs);
    dict_close(&animation_types_dict);
}

void cmdl_close(struct cmdl_t *cmdl)
{
    for (int i = 0; i < cmdl->num_meshes; i++)
    {
        struct sobj_t *sobj = cmdl->meshes[i];
        sobj_close(sobj);
        free(sobj);
    }

    for (int i = 0; i < cmdl->num_shapes; i++)
    {
        struct sobj_t *sobj = cmdl->shapes[i];
        sobj_close(sobj);
        free(sobj);
    }

    if (cmdl->skeleton != NULL)
        free(cmdl->skeleton);

    free(cmdl->shapes);
    free(cmdl->meshes);
    free(cmdl->name);
}
