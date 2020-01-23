//
//  sobj.c
//  Mirai
//
//  Created by Marika on 2020-01-16.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "sobj.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "utils.h"

// MARK: - Enumerations

/// The different formats that an SOBJ face's vertex indices can be in.
enum sobj_face_index_format_t
{
    /// An unsigned 8-bit integer.
    SOBJ_FACE_INDEX_FORMAT_U8  = 0,

    /// An unsigned 16-bit integer.
    SOBJ_FACE_INDEX_FORMAT_U16 = 1,
};

// MARK: - Functions

/// Read the object at the current offset of the given file handle into the given object.
/// @param file The file handle to read the object from.
/// @param object The object to read the file info.
void sobj_object_read(FILE *file, struct sobj_object_t *object)
{
    // read the mesh and material indices
    // these are the indices, in their respective array
    // properties of the parent cmdl, for this object to use
    uint32_t mesh_index;
    fread(&mesh_index, sizeof(mesh_index), 1, file);

    uint32_t material_index;
    fread(&material_index, sizeof(material_index), 1, file);

    // u32 unknown
    // ohana refers to this is an "owner model offset"
    // but never uses it
    fseek(file, 4, SEEK_CUR);

    // read the flags
    //  - bit 0: is visible
    uint8_t flags;
    fread(&flags, sizeof(flags), 1, file);

    bool is_visible = flags & 0b1;

    // read the rendering priority
    // this is the order that this objects mesh is drawn,
    // relative to other meshes
    // objects with greater rendering orders are rendered last
    uint8_t rendering_priority;
    fread(&rendering_priority, sizeof(rendering_priority), 1, file);

    // u32 visibility index, unused
    // for whatever reason, despite objects having a visibility flag,
    // objects can use an additional dictionary in the cmdl that contains
    // whether or not each object is visible
    // this u32 is the index inside that dictionary for the visibility of this object
    // implementing this would be so annoying that its just getting ignored
    fseek(file, 4, SEEK_CUR);

    // u16 "current primitive index", unused
    // another property that ohana reads and does not use
    // unsure of what this is, could be referring to the
    // index of the vertex group within the mesh of this object
    // for this object to render with
    fseek(file, 2, SEEK_CUR);

    // there are many more properties but none of them are useful here

    // initialize the object
    object->mesh_index = mesh_index;
    object->material_index = material_index;
    object->is_visible = is_visible;
    object->rendering_priority = rendering_priority;
}

/// Read the face group at the current offset of the given file handle into the given face group.
/// @param file The file handle to read the face group from.
/// @param face_group The face group to read the file into.
void sobj_read_face_group(FILE *file, struct sobj_face_group_t *face_group)
{
    // multiple unused values
    //  - u32 bone node count and pointer
    //  - u32 skinning mode
    fseek(file, 4 + 4 + 4, SEEK_CUR);

    // read the face group count and pointer, then each groups indices
    // faces are grouped on two levels; once in first level groups,
    // then again in a second level where they are grouped with bone nodes
    // here they all the first level groups are expanded into the second level
    // just to simplify the consumer facing api
    uint32_t num_face_groups;
    fread(&num_face_groups, sizeof(num_face_groups), 1, file);

    uint32_t face_groups_pointer = utils_read_relative_pointer(file);

    // the indices array is reallocated and appended for each group
    // malloc(1) is used instead of zero because malloc(0)
    // returns either null or a pointer that can only be pretend deallocated
    // depending on the platform
    int indices_index = 0;
    int num_indices = 0;
    uint16_t *indices = malloc(1);
    for (int g = 0; g < num_face_groups; g++)
    {
        // pointer table
        fseek(file, face_groups_pointer + (g * 4), SEEK_SET);
        fseek(file, utils_read_relative_pointer(file), SEEK_SET);

        // read the face count and pointer
        uint32_t num_faces;
        fread(&num_faces, sizeof(num_faces), 1, file);

        uint32_t faces_pointer = utils_read_relative_pointer(file);

        // seek to the face data
        // pointer to a pointer
        fseek(file, faces_pointer, SEEK_SET);
        fseek(file, utils_read_relative_pointer(file), SEEK_SET);

        // read the index format
        // the format is a flag (bit 1), so mask and shift it
        // to convert it to the indices used in the enumeration
        uint32_t index_format_raw;
        fread(&index_format_raw, sizeof(index_format_raw), 1, file);

        enum sobj_face_index_format_t index_format = (index_format_raw & 0b11) >> 1;

        // u32 unknown
        fseek(file, 4, SEEK_CUR);

        // read the face indices size and pointer of this group
        // note that this is the size (in bytes) of the array,
        // *not* the number of items within it
        uint32_t group_indices_size;
        fread(&group_indices_size, sizeof(group_indices_size), 1, file);

        uint32_t indices_pointer = utils_read_relative_pointer(file);

        // ensure there are any indices to read
        // this is to avoid having to specially handle appending empty indices
        if (group_indices_size <= 0)
            continue;

        // get the size of a single index within the indices
        int group_index_size;
        switch (index_format)
        {
            case SOBJ_FACE_INDEX_FORMAT_U8:  group_index_size = sizeof(uint8_t);  break;
            case SOBJ_FACE_INDEX_FORMAT_U16: group_index_size = sizeof(uint16_t); break;
        }

        // read the face indices in this group
        int num_group_indices = group_indices_size / group_index_size;
        fseek(file, indices_pointer, SEEK_SET);

        uint16_t group_indices[num_group_indices];
        for (int i = 0; i < num_group_indices; i++)
        {
            switch (index_format)
            {
                case SOBJ_FACE_INDEX_FORMAT_U8:
                {
                    uint8_t raw;
                    fread(&raw, sizeof(raw), 1, file);
                    group_indices[i] = (uint16_t)raw;
                    break;
                }
                case SOBJ_FACE_INDEX_FORMAT_U16:
                {
                    uint16_t raw;
                    fread(&raw, sizeof(raw), 1, file);
                    group_indices[i] = raw;
                    break;
                }
            }
        }

        // insert the indices into the total array
        num_indices += num_group_indices;
        indices = realloc(indices, num_indices * sizeof(uint16_t));
        memcpy(&indices[indices_index], group_indices, num_group_indices * sizeof(uint16_t));
        indices_index += num_group_indices;
    }

    face_group->num_indices = num_indices;
    face_group->indices = indices;
}

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
    vertex_group->num_vertices = data_size / data_stride;
    vertex_group->data_size = (size_t)data_size;
    vertex_group->data_pointer = (size_t)data_pointer;
    vertex_group->data_stride = (size_t)data_stride;

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

    // calculate the decoded data size and stride
    int num_values = 0;
    for (int c = 0; c < vertex_group->num_components; c++)
    {
        struct sobj_component_t *component = vertex_group->components[c];
        for (int v = 0; v < component->num_values; v++)
            num_values++;
    }

    int decoded_data_stride = num_values * sizeof(float);
    vertex_group->decoded_data_size = decoded_data_stride * vertex_group->num_vertices;
    vertex_group->decoded_data_stride = decoded_data_stride;

    // indicate that the vertex group was valid
    return true;
}

/// Read the mesh at the current offset of the given file handle into the given mesh.
/// @param file The file handle to read the mesh from.
/// @param mesh The mesh to read the file info.
void sobj_mesh_read(FILE *file, struct sobj_mesh_t *mesh)
{
    // two unused values
    //  - u32 flags
    //  - u32 bounding box pointer
    fseek(file, 4 + 4, SEEK_CUR);

    // read the translation transform
    struct vec3_t transform_translation;
    vec3_read(file, &transform_translation);

    // read the face group count and pointer
    uint32_t num_face_groups;
    fread(&num_face_groups, sizeof(num_face_groups), 1, file);

    uint32_t face_groups_pointer = utils_read_relative_pointer(file);

    // u32 unknown
    fseek(file, 4, SEEK_CUR);

    // read the vertex group count and pointer
    uint32_t num_vertex_groups;
    fread(&num_vertex_groups, sizeof(num_vertex_groups), 1, file);

    uint32_t vertex_groups_pointer = utils_read_relative_pointer(file);

    // u32 unknown pointer
    fseek(file, 4, SEEK_CUR);

    // initialize the mesh
    mesh->transform_translation = transform_translation;

    // read the face groups
    mesh->num_face_groups = num_face_groups;
    mesh->face_groups = malloc(num_face_groups * sizeof(struct sobj_face_group_t *));
    for (int i = 0; i < num_face_groups; i++)
    {
        // pointer table
        fseek(file, face_groups_pointer + (i * 4), SEEK_SET);
        fseek(file, utils_read_relative_pointer(file), SEEK_SET);

        // read the face group
        struct sobj_face_group_t *face_group = malloc(sizeof(struct sobj_face_group_t));
        sobj_read_face_group(file, face_group);

        // insert the face group
        mesh->face_groups[i] = face_group;
    }

    // read the vertex groups
    // the vertex groups need to be inserted such that the invalid ones dont leave gaps in the array
    int vertex_group_index = 0;
    int num_valid_vertex_groups = 0;

    mesh->vertex_groups = malloc(num_vertex_groups * sizeof(struct sobj_vertex_group_t *));
    for (int i = 0; i < num_vertex_groups; i++)
    {
        // pointer table
        fseek(file, vertex_groups_pointer + (i * 4), SEEK_SET);
        fseek(file, utils_read_relative_pointer(file), SEEK_SET);

        // read the vertex group
        // skip any invalid ones
        struct sobj_vertex_group_t *vertex_group = malloc(sizeof(struct sobj_vertex_group_t));
        if (!sobj_read_vertex_group(file, vertex_group))
            continue;

        // insert the vertex group
        mesh->vertex_groups[vertex_group_index] = vertex_group;
        vertex_group_index++;
        num_valid_vertex_groups++;
    }

    // resize the vertex groups array to only contain the valid ones
    mesh->num_vertex_groups = num_valid_vertex_groups;
    mesh->vertex_groups = realloc(mesh->vertex_groups, sizeof(num_valid_vertex_groups * sizeof(struct sobj_vertex_group_t *)));
}

/// Read the skeleton at the current offset of the given file handle into the given skeleton.
/// @param file The file handle to read the skeleton from.
/// @param skeleton The skeleton to read the file info.
void sobj_skeleton_read(FILE *file, struct sobj_skeleton_t *skeleton)
{
}

void sobj_open(FILE *file, struct sobj_t *sobj)
{
    // read the flags
    //  - bit 24: is object
    //  - bit 25: is skeleton
    //  - bit 28: is mesh
    // cant use an enum directly as the type flags are combined with other flags
    uint32_t flags;
    fread(&flags, sizeof(flags), 1, file);

    bool is_object     = flags & (0x1 << 24);
    bool is_skeleton = flags & (0x1 << 25);
    bool is_mesh    = flags & (0x1 << 28);

    assert(is_object || is_skeleton || is_mesh);

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
    if (is_object)
    {
        sobj->type = SOBJ_TYPE_OBJECT;
        sobj->object = malloc(sizeof(struct sobj_object_t *));
        sobj->mesh = NULL;
        sobj->skeleton = NULL;
        sobj_object_read(file, sobj->object);
    }
    else if (is_skeleton)
    {
        sobj->type = SOBJ_TYPE_SKELETON;
        sobj->object = NULL;
        sobj->mesh = NULL;
        sobj->skeleton = malloc(sizeof(struct sobj_skeleton_t *));;
        sobj_skeleton_read(file, sobj->skeleton);
    }
    else if (is_mesh)
    {
        sobj->type = SOBJ_TYPE_MESH;
        sobj->object = NULL;
        sobj->mesh = malloc(sizeof(struct sobj_mesh_t));
        sobj->skeleton = NULL;
        sobj_mesh_read(file, sobj->mesh);
    }
}

void sobj_close(struct sobj_t *sobj)
{
    switch (sobj->type)
    {
        case SOBJ_TYPE_OBJECT:
        {
            free(sobj->object);
            break;
        }
        case SOBJ_TYPE_SKELETON:
        {
            free(sobj->skeleton);
            break;
        }
        case SOBJ_TYPE_MESH:
            for (int i = 0; i < sobj->mesh->num_face_groups; i++)
            {
                struct sobj_face_group_t *face_group = sobj->mesh->face_groups[i];
                free(face_group->indices);
                free(face_group);
            }

            for (int i = 0; i < sobj->mesh->num_vertex_groups; i++)
            {
                struct sobj_vertex_group_t *vertex_group = sobj->mesh->vertex_groups[i];
                for (int c = 0; c < vertex_group->num_components; c++)
                    free(vertex_group->components[c]);

                free(vertex_group->components);
                free(vertex_group);
            }

            free(sobj->mesh->vertex_groups);
            free(sobj->mesh->face_groups);
            free(sobj->mesh);
            break;
    }

    free(sobj->name);
}

/// Small macro for decoding the various SOBJ component data types into floats within `sobj_vertex_group_decode(vertex_group, file)`.
#define SOBJ_DATA_TYPE_DECODE(type) \
{ \
    type raw; \
    memcpy(&raw, &raw_data[(v * vertex_group->data_stride) + component->offset + (i * sizeof(raw))], sizeof(raw)); \
    value = (float)raw; \
    break; \
}

float *sobj_vertex_group_decode(const struct sobj_vertex_group_t *vertex_group, FILE *file)
{
    // read the data to decode
    uint8_t raw_data[vertex_group->data_size];
    fseek(file, vertex_group->data_pointer, SEEK_SET);
    fread(raw_data, vertex_group->data_size, 1, file);

    // decode the data
    // for each vertex:
    //  for each component:
    //   for each value:
    //    read the original value
    //    convert to float
    //    apply multiplier
    //    increment index
    float *decoded_data = malloc(vertex_group->decoded_data_size);

    unsigned int decoded_index = 0;
    for (int v = 0; v < vertex_group->num_vertices; v++)
    {
        for (int c = 0; c < vertex_group->num_components; c++)
        {
            struct sobj_component_t *component = vertex_group->components[c];
            for (int i = 0; i < component->num_values; i++)
            {
                float value;

                // handle the size of the different data types
                switch (component->data_type)
                {
                    case SOBJ_COMPONENT_DATA_TYPE_S8:    SOBJ_DATA_TYPE_DECODE(int8_t);
                    case SOBJ_COMPONENT_DATA_TYPE_U8:    SOBJ_DATA_TYPE_DECODE(uint8_t);
                    case SOBJ_COMPONENT_DATA_TYPE_S16:   SOBJ_DATA_TYPE_DECODE(int16_t);
                    case SOBJ_COMPONENT_DATA_TYPE_U16:   SOBJ_DATA_TYPE_DECODE(uint16_t);
                    case SOBJ_COMPONENT_DATA_TYPE_S32:   SOBJ_DATA_TYPE_DECODE(int32_t);
                    case SOBJ_COMPONENT_DATA_TYPE_U32:   SOBJ_DATA_TYPE_DECODE(uint32_t);
                    case SOBJ_COMPONENT_DATA_TYPE_FLOAT: SOBJ_DATA_TYPE_DECODE(float);
                }

                decoded_data[decoded_index] = value * component->multiplier;
                decoded_index++;
            }
        }
    }

    return decoded_data;
}
