//
//  sobj.h
//  Mirai
//
//  Created by Marika on 2020-01-16.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

#include "vector.h"

// MARK: - Enumerations

/// The different types that an SOBJ file can be.
enum sobj_type_t
{
    /// This SOBJ file contains a mesh.
    SOBJ_TYPE_MESH,

    /// This SOBJ file contains a skeleton.
    SOBJ_TYPE_SKELETON,

    /// This SOBJ file contains a shape.
    SOBJ_TYPE_SHAPE,
};

/// The different types that a vertex component's data can be used for.
enum sobj_component_type_t
{
    #warning TODO: Come back and document these all.

    /// The X, Y, and Z position of the vertex in 3D space.
    SOBJ_COMPONENT_TYPE_POSITION = 0x00,

    SOBJ_COMPONENT_TYPE_NORMAL   = 0x01,

    /// The red, green, blue, and alpha channels of the vertex's colour.
    SOBJ_COMPONENT_TYPE_COLOR    = 0x03,

    /// The first UV texture coordinate of the vertex.
    SOBJ_COMPONENT_TYPE_UV0      = 0x04,

    /// The second UV texture coordinate of the vertex.
    SOBJ_COMPONENT_TYPE_UV1      = 0x05,

    SOBJ_COMPONENT_TYPE_WEIGHT   = 0x07,

    SOBJ_COMPONENT_TYPE_INDEX    = 0x08,
};

/// The different formats that a vertex component's values can be in.
enum sobj_component_data_type_t
{
    /// A signed 8-bit integer.
    SOBJ_COMPONENT_DATA_TYPE_SBYTE =  0x00,

    /// An unsigned 8-bit integer.
    SOBJ_COMPONENT_DATA_TYPE_BYTE  =  0x01,

    /// A signed 16-bit integer.
    SOBJ_COMPONENT_DATA_TYPE_SHORT =  0x02,

    /// An unsigned 16-bit integer.
    SOBJ_COMPONENT_DATA_TYPE_USHORT = 0x03,

    /// A signed 32-bit integer.
    SOBJ_COMPONENT_DATA_TYPE_INT =    0x04,

    /// An unsigned 32-bit integer.
    SOBJ_COMPONENT_DATA_TYPE_UINT =   0x05,

    /// A 32-bit floating point number.
    SOBJ_COMPONENT_DATA_TYPE_FLOAT =  0x06,
};

// MARK: - Data Structures

/// The data structure for an SOBJ file that has been opened.
struct sobj_t
{
    /// The name of this SOBJ.
    ///
    /// Allocated.
    char *name;

    /// The type of this SOBJ.
    enum sobj_type_t type;

    /// The shape within this SOBJ file, if any.
    ///
    /// If `type` is not `SOBJ_TYPE_SHAPE`, then this is `NULL`.
    /// If `type` is `SOBJ_TYPE_SHAPE`, then this is allocated.
    struct sobj_shape_t *shape;
};

/// The data structure for a shape within an SOBJ file.
struct sobj_shape_t
{
    /// The 3D vector to offset this SOBJ's position by.
    struct vec3_t transform_translation;

    /// The number of vertex groups within this shape.
    unsigned int num_vertex_groups;

    /// All the vertex groups within this shape.
    ///
    /// The array and each item are allocated.
    struct sobj_vertex_group_t **vertex_groups;
};

/// The data structure for a single group of vertices within an SOBJ shape.
struct sobj_vertex_group_t
{
    /// The number of vertices within this group.
    unsigned int num_vertices;

    /// The number of components for each vertex in this group.
    unsigned int num_components;

    /// All the data structures representing the format of each vertex component within a vertex in this group.
    ///
    /// The array and each item are allocated.
    struct sobj_component_t **components;

    /// The size of this group's vertex data within the SOBJ file, in bytes.
    size_t data_size;

    /// The offset of this group's vertex data within the SOBJ file, in bytes.
    size_t data_pointer;

    /// The combined size of each of a single vertex's components within the SOBJ file, in bytes.
    size_t data_stride;

    /// The size of this group's decoded vertex data, in bytes.
    size_t decoded_data_size;

    /// The combined size of each of a single vertex's components within this group's decoded data, in bytes.
    size_t decoded_data_stride;
};

/// The data structure for describing the contents of one of a vertex's components.
struct sobj_component_t
{
    /// The type of this component.
    enum sobj_component_type_t type;

    /// The type of the values within this component.
    enum sobj_component_data_type_t data_type;

    /// The number of values within this component.
    unsigned int num_values;

    /// The value to multiply each value of this component by to get the real value.
    ///
    /// For example, colour RGBA values are stored as bytes, and the multiplier converts them from `0-255` to `0.0-1.0`.
    float multiplier;

    /// The offset of this component's values within a single stride.
    unsigned int offset;
};

// MARK: - Functions

/// Open the SOBJ file at the current offset of the given file handle into the given SOBJ.
/// @param file The file handle to open the SOBJ file from.
/// @param sobj The SOBJ to open the file into.
void sobj_open(FILE *file, struct sobj_t *sobj);

/// Close the given SOBJ, releasing all of it's allocated memory.
///
/// This must be called after an SOBJ is opened and before program execution completes.
/// @param sobj The SOBJ to close.
void sobj_close(struct sobj_t *sobj);

/// Read and decode the given vertex group's data from the given file handle.
/// @param vertex_group The vertex group to read and decode the data of.
/// @param file The file handle to read the vertex group's data from.
/// @returns A pointer to the array of decoded vertex data from the given vertex group.
/// This data is allocated so it must be freed before program execution completes.
/// Note that that the decoded data is layed out differently depending on the components of the given vertex group.
/// Each value is converted to a 32-bit float, so the stride is different, but the values are layed out in the same order.
/// Use `sobj_vertex_group_t.decoded_data_stride` to then read the decoded data into a renderer.
float *sobj_vertex_group_decode(const struct sobj_vertex_group_t *vertex_group, FILE *file);
