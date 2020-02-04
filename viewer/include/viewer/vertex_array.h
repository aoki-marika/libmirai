//
//  vertex_array.h
//  viewer
//
//  Created by Marika on 2020-02-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include "program.h"

// MARK: - Data Structures

/// The data structure for a vertex array that has been created.
struct vertex_array_t
{
    /// The unique identifier of this array.
    GLuint id;

    /// The unique identifier of this array's buffer.
    GLuint buffer_id;
};

/// The data structure for describing a single component within a vertex's data.
struct vertex_component_t
{
    /// The data type of this component's values.
    ///
    /// See `glVertexAttribPointer`'s `type` argument for the accepted types.
    GLenum data_type;

    /// The total number of values within this component.
    GLint num_values;

    /// The offset of this component's values within a single stride.
    GLsizei offset;

    /// The total size of a single vertex's components within this component's data.
    GLsizei stride;

    /// The name of the program attribute that this component is binding to.
    char *attribute_name;
};

// MARK: - Functions

/// Create a new vertex array from the given properties.
/// @param data The vertex data for this array.
/// @param data_size The total size of the given data.
/// @param num_components The number of components within the given data.
/// @param components All the components within the given data.
/// @param program The program for this array to bind it's component's attributes to.
/// @param vertex_array The vertex array to load the created vertex array into.
void vertex_array_create(const void *data,
                         size_t data_size,
                         unsigned int num_components,
                         const struct vertex_component_t *components,
                         const struct program_t *program,
                         struct vertex_array_t *vertex_array);

/// Destroy the given vertex array, releasing all of it's allocated memory.
///
/// This must be called after a vertex array is created and before program execution completes.
/// @param vertex_array The vertex array to destroy.
void vertex_array_destroy(struct vertex_array_t *vertex_array);
