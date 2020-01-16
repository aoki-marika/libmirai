//
//  sobj.c
//  Mirai
//
//  Created by Marika on 2020-01-16.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "sobj.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "utils.h"
#include "vector.h"

// MARK: - Functions

/// Read the vertex component at the current offset of the given file handle into the given component.
/// @param file The file handle to read the vertex component from.
/// @param component The component to read the file into.
void sobj_read_component(FILE *file, struct sobj_component_t *component)
{
    // read the signature
    uint32_t signature;
    fread(&signature, sizeof(signature), 1, file);
    assert(signature == 0x40000001);

    // read the type
    enum sobj_component_type_t type = 0x0;
    fread(&type, 4, 1, file);

    // 7x u32 unknown
    fseek(file, 4 + 4 + 4 + 4 + 4 + 4 + 4, SEEK_CUR);

    // read the data type
    enum sobj_component_data_type_t data_type = 0x0;
    fread(&data_type, 1, 1, file);

    // 3x u8 unknown
    fseek(file, 1 + 1 + 1, SEEK_CUR);

    // read the number of values
    uint32_t num_values;
    fread(&num_values, sizeof(num_values), 1, file);

    // read the value multiplier
    float multiplier;
    fread(&multiplier, sizeof(multiplier), 1, file);

    // read the offset
    uint32_t offset;
    fread(&offset, sizeof(offset), 1, file);

    // initialize the component
    component->type = type;
    component->data_type = data_type;
    component->num_values = num_values;
    component->multiplier = multiplier;
    component->offset = offset;
}

/// Read the vertex group at the current offset of the given file handle into the given vertex group.
/// @param file The file handle to read the vertex group from.
/// @param vertex_group The vertex group to read the file info.
/// @returns Whether or not the vertex group was read.
/// Only the `0x40000002` kind can be read, `0x40000001` are undocumented.
bool sobj_read_vertex_group(FILE *file, struct sobj_vertex_group_t *vertex_group)
{
    // read the kind
    // 0x40000002 is the only kind that can be read right now
    // 0x40000001 is an undocumented format, despite it matching
    // the component declaration signature
    uint32_t kind;
    fread(&kind, sizeof(kind), 1, file);

    if (kind != 0x40000002)
        return false;

    // 4x u32 unknowns
    fseek(file, 4 + 4 + 4 + 4, SEEK_CUR);

    // read the data size and pointer
    uint32_t data_size;
    fread(&data_size, sizeof(data_size), 1, file);

    uint32_t data_pointer = utils_read_relative_pointer(file);

    // 2x u32 unknowns
    fseek(file, 4 + 4, SEEK_CUR);

    // read the data stride
    // this stride is used to define the total size of each
    // vertexs component values within the data array
    uint32_t data_stride;
    fread(&data_stride, sizeof(data_stride), 1, file);

    // read the component count and pointer
    uint32_t num_components;
    fread(&num_components, sizeof(num_components), 1, file);

    uint32_t components_pointer = utils_read_relative_pointer(file);

    // initialize the vertex group
    vertex_group->data_size = (size_t)data_size;
    vertex_group->data_pointer = (size_t)data_pointer;

    // read the components
    vertex_group->num_components = num_components;
    vertex_group->components = malloc(num_components * sizeof(struct sobj_component_t *));
    for (int i = 0; i < num_components; i++)
    {
        // pointer table
        fseek(file, components_pointer + (i * 4), SEEK_SET);
        fseek(file, utils_read_relative_pointer(file), SEEK_SET);

        // read the component
        struct sobj_component_t *component = malloc(sizeof(struct sobj_component_t));
        sobj_read_component(file, component);

        // insert the component
        vertex_group->components[i] = component;
    }

    return true;
}

/// Read the shape at the current offset of the given file handle into the given shape.
/// @param file The file handle to read the shape from.
/// @param shape The shape to read the file info.
void sobj_shape_read(FILE *file, struct sobj_shape_t *shape)
{
    // two unused values
    //  - u32 flags
    //  - u32 bounding box pointer
    fseek(file, 4 + 4, SEEK_CUR);

    // read the position offset
    struct vec3_t position_offset;
    vec3_read(file, &position_offset);

    // read the face group count and pointer
    #warning TODO: Face group reading.
    fseek(file, 4 + 4, SEEK_CUR);

//    uint32_t num_face_groups;
//    fread(&num_face_groups, sizeof(num_face_groups), 1, file);
//
//    uint32_t face_groups_pointer = utils_read_relative_pointer(file);

    // u32 unknown
    fseek(file, 4, SEEK_CUR);

    // read the vertex group count and pointer
    uint32_t num_vertex_groups;
    fread(&num_vertex_groups, sizeof(num_vertex_groups), 1, file);

    uint32_t vertex_groups_pointer = utils_read_relative_pointer(file);

    // u32 unknown pointer
    fseek(file, 4, SEEK_CUR);

    // read the vertex groups
    // the vertex groups need to be inserted so that the invalid ones dont leave gaps in the array
    int vertex_group_index = 0;
    int num_valid_vertex_groups = 0;

    shape->vertex_groups = malloc(num_vertex_groups * sizeof(struct sobj_vertex_group_t *));
    for (int i = 0; i < num_vertex_groups; i++)
    {
        // pointer table
        fseek(file, vertex_groups_pointer + (i * 4), SEEK_SET);
        fseek(file, utils_read_relative_pointer(file), SEEK_SET);

        // read the vertex group
        struct sobj_vertex_group_t *vertex_group = malloc(sizeof(struct sobj_vertex_group_t));
        sobj_read_vertex_group(file, vertex_group);

        // insert the vertex group
        shape->vertex_groups[vertex_group_index] = vertex_group;
        vertex_group_index++;
        num_valid_vertex_groups++;
    }

    // resize the vertex groups array to only contain the valid ones
    shape->num_vertex_groups = num_valid_vertex_groups;
    shape->vertex_groups = realloc(shape->vertex_groups, sizeof(num_valid_vertex_groups * sizeof(struct sobj_vertex_group_t *)));
}

void sobj_open(FILE *file, struct sobj_t *sobj)
{
    // read the flags
    //  - bit 24: is mesh
    //  - bit 25: is skeleton
    //  - bit 28: is shape
    // cant use an enum directly as the type flags are combined with other flags
    uint32_t flags;
    fread(&flags, sizeof(flags), 1, file);

    bool is_mesh     = flags & (0x1 << 24);
    bool is_skeleton = flags & (0x1 << 25);
    bool is_shape    = flags & (0x1 << 28);

    assert(is_mesh || is_skeleton || is_shape);

    // read the signature
    assert(fgetc(file) == 'S');
    assert(fgetc(file) == 'O');
    assert(fgetc(file) == 'B');
    assert(fgetc(file) == 'J');

    // u32 unknown
    fseek(file, 4, SEEK_CUR);

    // read the name and seek back
    // this is supposed to be the name, but no tested sobj has one
    // it seems to at least point to a string block, so probably just not named?
    uint32_t name_pointer = utils_read_relative_pointer(file);
    long name_return = ftell(file);
    fseek(file, name_pointer, SEEK_SET);
    char *name = utils_read_string(file);
    fseek(file, name_return, SEEK_SET);

    // unknown 8 bytes
    fseek(file, 8, SEEK_CUR);

    // initialize the sobj
    sobj->name = name;

    // handle the different data types
    if (is_mesh)
    {
        sobj->type = SOBJ_TYPE_MESH;
        sobj->shape = NULL;
    }
    else if (is_skeleton)
    {
        sobj->type = SOBJ_TYPE_SKELETON;
        sobj->shape = NULL;
    }
    else if (is_shape)
    {
        sobj->type = SOBJ_TYPE_SHAPE;
        sobj->shape = malloc(sizeof(struct sobj_shape_t));
        sobj_shape_read(file, sobj->shape);
    }
}

void sobj_close(struct sobj_t *sobj)
{
    switch (sobj->type)
    {
        case SOBJ_TYPE_MESH:
            break;
        case SOBJ_TYPE_SKELETON:
            break;
        case SOBJ_TYPE_SHAPE:
            for (int i = 0; i < sobj->shape->num_vertex_groups; i++)
            {
                struct sobj_vertex_group_t *vertex_group = sobj->shape->vertex_groups[i];
                for (int c = 0; c < vertex_group->num_components; c++)
                    free(vertex_group->components[c]);

                free(vertex_group->components);
                free(vertex_group);
            }

            free(sobj->shape->vertex_groups);
            free(sobj->shape);
            break;
    }

    free(sobj->name);
}
