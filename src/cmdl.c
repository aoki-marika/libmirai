//
//  cmdl.c
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "cmdl.h"

#include <stdlib.h>
#include <assert.h>

#include "dict.h"
#include "utils.h"

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
    #warning TODO: Read the animation types dict.
    struct dict_t animation_types_dict;
    dict_open(file, &animation_types_dict);

    // read the transforms
    struct vec3_t transform_scale, transform_rotation, transform_translation;
    struct mat4_t transform_local, transform_world;
    vec3_read(file, &transform_scale);
    vec3_read(file, &transform_rotation);
    vec3_read(file, &transform_translation);
    mat4_read43(file, &transform_local);
    mat4_read43(file, &transform_world);

    // read the object count and pointer
    uint32_t num_objects;
    fread(&num_objects, sizeof(num_objects), 1, file);

    uint32_t objects_pointer = utils_read_relative_pointer(file);

    // read the materials dict
    struct dict_t materials;
    dict_open(file, &materials);

    // read the mesh count and pointer
    uint32_t num_meshes;
    fread(&num_meshes, sizeof(num_meshes), 1, file);

    uint32_t meshes_pointer = utils_read_relative_pointer(file);

    // dict pointer unknown
    // ohana3ds refers to this dict as object nodes,
    // each entry is a name and visibility flag
    fseek(file, 8, SEEK_CUR);

    // read the flags
    //  - bit 0: is visible
    //  - bit 8: is non-uniform scalable, unused
    fread(&flags, sizeof(flags), 1, file);

    bool is_visible = flags & 0b1;

    // two unused values
    //  - u32 culling mode
    //  - u32 layer id
    fseek(file, 4 + 4, SEEK_CUR);

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
    cmdl->transform_scale = transform_scale;
    cmdl->transform_rotation = transform_rotation;
    cmdl->transform_translation = transform_translation;
    cmdl->transform_local = transform_local;
    cmdl->transform_world = transform_world;
    cmdl->is_visible = is_visible;

    // read the objects
    cmdl->num_objects = num_objects;
    cmdl->object_sobjs = malloc(num_objects * sizeof(struct sobj_t *));
    cmdl_read_sobj_table(file,
                         num_objects,
                         objects_pointer,
                         SOBJ_TYPE_OBJECT,
                         cmdl->object_sobjs);

    // read the meshes
    cmdl->num_meshes = num_meshes;
    cmdl->mesh_sobjs = malloc(num_meshes * sizeof(struct sobj_t *));
    cmdl_read_sobj_table(file,
                         num_meshes,
                         meshes_pointer,
                         SOBJ_TYPE_MESH,
                         cmdl->mesh_sobjs);

    // read the skeleton
    if (has_skeleton_sobj)
    {
        fseek(file, skeleton_sobj_pointer, SEEK_SET);

        struct sobj_t *sobj = malloc(sizeof(struct sobj_t));
        sobj_open(file, sobj);
        cmdl->skeleton_sobj = sobj;
    }
    else
    {
        cmdl->skeleton_sobj = NULL;
    }

    #warning TODO: Remove this when material and animation types dict reading is implemented.
//    for (int i = 0; i < materials.num_entries; i++)
//        printf(" - mtob %i: \"%s\"\n", i, materials.entries[i]->name);

    // close the dicts only needed for reading the file
    dict_close(&materials);
    dict_close(&animation_types_dict);
}

void cmdl_close(struct cmdl_t *cmdl)
{
    for (int i = 0; i < cmdl->num_objects; i++)
    {
        struct sobj_t *sobj = cmdl->object_sobjs[i];
        sobj_close(sobj);
        free(sobj);
    }

    for (int i = 0; i < cmdl->num_meshes; i++)
    {
        struct sobj_t *sobj = cmdl->mesh_sobjs[i];
        sobj_close(sobj);
        free(sobj);
    }

    if (cmdl->skeleton_sobj != NULL)
        free(cmdl->skeleton_sobj);

    free(cmdl->mesh_sobjs);
    free(cmdl->object_sobjs);
    free(cmdl->name);
}
