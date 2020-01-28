//
//  sobj.h
//  Mirai
//
//  Created by Marika on 2020-01-16.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <stdbool.h>

#include "vector.h"
#include "matrix.h"

// MARK: - Enumerations

/// The different types that an SOBJ file can be.
enum sobj_type_t
{
    /// This SOBJ file contains an object.
    ///
    /// An object is the top level object that combines meshes, materials,
    /// and other properties within a CMDL to form a usable model.
    SOBJ_TYPE_OBJECT,

    /// This SOBJ file contains a skeleton.
    ///
    /// A skeleton is an object that works with geometry to create dynamic
    /// animations, based around the idea of the model containing a skeleton.
    SOBJ_TYPE_SKELETON,

    /// This SOBJ file contains a mesh.
    ///
    /// A mesh is an object that contains raw geometry data, such
    /// as vertex data and faces, to form a polygon mesh.
    SOBJ_TYPE_MESH,
};

/// The different types that a vertex component's data can be used for.
enum sobj_component_type_t
{
    /// The X, Y, and Z position of this vertex in 3D space.
    SOBJ_COMPONENT_TYPE_POSITION    = 0x00,

    /// The normal of this vertex.
    SOBJ_COMPONENT_TYPE_NORMAL      = 0x01,

    /// The tangent of this vertex.
    SOBJ_COMPONENT_TYPE_TANGENT     = 0x02,

    /// The red, green, blue, and alpha channels of the vertex's colour.
    SOBJ_COMPONENT_TYPE_COLOR       = 0x03,

    /// The UV coordinates of this vertex within the first texture of the material.
    SOBJ_COMPONENT_TYPE_TEXCOORD0   = 0x04,

    /// The UV coordinates of this vertex within the second texture of the material.
    SOBJ_COMPONENT_TYPE_TEXCOORD1   = 0x05,

    /// The UV coordinates of this vertex within the third texture of the material.
    SOBJ_COMPONENT_TYPE_TEXCOORD2   = 0x06,

    /// The index of this vertex's bone.
    SOBJ_COMPONENT_TYPE_BONE_INDEX  = 0x07,

    /// The weight of this vertex's bone.
    SOBJ_COMPONENT_TYPE_BONE_WEIGHT = 0x08,
};

/// The different formats that a vertex component's values can be in.
enum sobj_component_data_type_t
{
    /// A signed 8-bit integer.
    SOBJ_COMPONENT_DATA_TYPE_S8    = 0x00,

    /// An unsigned 8-bit integer.
    SOBJ_COMPONENT_DATA_TYPE_U8    = 0x01,

    /// A signed 16-bit integer.
    SOBJ_COMPONENT_DATA_TYPE_S16   = 0x02,

    /// An unsigned 16-bit integer.
    SOBJ_COMPONENT_DATA_TYPE_U16   = 0x03,

    /// A signed 32-bit integer.
    SOBJ_COMPONENT_DATA_TYPE_S32   = 0x04,

    /// An unsigned 32-bit integer.
    SOBJ_COMPONENT_DATA_TYPE_U32   = 0x05,

    /// A 32-bit floating point number.
    SOBJ_COMPONENT_DATA_TYPE_FLOAT = 0x06,
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

    /// The object within this SOBJ file, if any.
    ///
    /// If `type` is not `SOBJ_TYPE_OBJECT`, then this is `NULL`.
    /// If `type` is `SOBJ_TYPE_OBJECT`, then this is allocated.
    struct sobj_object_t *object;

    /// The mesh within this SOBJ file, if any.
    ///
    /// If `type` is not `SOBJ_TYPE_MESH`, then this is `NULL`.
    /// If `type` is `SOBJ_TYPE_MESH`, then this is allocated.
    struct sobj_mesh_t *mesh;

    /// The skeleton within this SOBJ file, if any.
    ///
    /// If `type` is not `SOBJ_TYPE_SKELETON`, then this is `NULL`.
    /// If `type` is `SOBJ_TYPE_SKELETON`, then this is allocated.
    struct sobj_skeleton_t *skeleton;
};

/// The data structure for an object within an SOBJ file.
struct sobj_object_t
{
    /// The index of this object's mesh within the parent CMDL's meshes.
    unsigned int mesh_index;

    /// The index of this object's material within the parent CMDL's materials.
    unsigned int material_index;

    /// The rendering priority of this object.
    ///
    /// Objects with a greater rendering priority are rendered last,
    /// rendering on top of other objects.
    unsigned int rendering_priority;
};

/// The data structure for a mesh within an SOBJ file.
struct sobj_mesh_t
{
    /// The 3D vector to offset this mesh's position by.
    struct vec3_t transform_translation;

    /// The number of face groups within this mesh.
    unsigned int num_face_groups;

    /// All the face groups within this mesh.
    ///
    /// Each of these groups are for the vertex group of the same index.
    ///
    /// The array and each item are allocated.
    struct sobj_face_group_t **face_groups;

    /// The number of vertex groups within this mesh.
    unsigned int num_vertex_groups;

    /// All the vertex groups within this mesh.
    ///
    /// Each of these groups are for the face group of the same index.
    ///
    /// The array and each item are allocated.
    struct sobj_vertex_group_t **vertex_groups;
};

/// The data structure for a single group of faces within an SOBJ mesh.
struct sobj_face_group_t
{
    /// The number of joint indices that this group connects it's vertices to.
    unsigned int num_joint_indices;

    /// The indices of each of the joints that this group connect's it's vertices to.
    ///
    /// Allocated.
    unsigned int *joint_indices;

    /// The number of vertex indices within this group.
    unsigned int num_indices;

    /// All the vertex indices within this group.
    ///
    /// These indices are layed out to form triangles, three indices per triangle,
    /// and each index refers to an index within the vertices of the vertex group this group is for.
    ///
    /// Allocated.
    uint16_t *indices;
};

/// The data structure for a single group of vertices within an SOBJ mesh.
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

/// The data structure for a skeleton within an SOBJ file.
struct sobj_skeleton_t
{
    /// The number of joints within this skeleton.
    unsigned int num_joints;

    /// All the joints within this skeleton.
    ///
    /// The array and each item are allocated
    struct sobj_skeleton_joint_t **joints;
};

/// The data structure for a single joint within a skeleton.
struct sobj_skeleton_joint_t
{
    /// The name of this joint.
    ///
    /// Allocated.
    char *name;

    /// The index of this joint's parent joint within the enclosing skeleton's joints array, if any.
    ///
    /// If this index is less than `0` then this joint has no parent.
    int parent_index;

    /// The scale transform of this joint.
    struct vec3_t transform_scale;

    /// The rotation transform of this joint.
    struct vec3_t transform_rotation;

    /// The translation transform of this joint.
    struct vec3_t transform_translation;

    /// The local transform of this joint.
    struct mat4_t transform_local;

    /// The world transform of this joint.
    struct mat4_t transform_world;

    /// The inverse base transform of this joint.
    struct mat4_t transform_inverse_base;
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
