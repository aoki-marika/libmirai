//
//  program.h
//  viewer
//
//  Created by Marika on 2020-02-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include "gl.h"

// MARK: - Data Structures

/// The data structure for a program that has been compiled and linked.
struct program_t
{
    /// The unique identifier of this program.
    GLuint id;

    /// The unique identifier of this program's vertex shader.
    GLuint vertex_id;

    /// The unique identifier of this program's fragment shader.
    GLuint fragment_id;
};

// MARK: - Functions

/// Create, compile, and link a new program from the given properties.
/// @param vertex_source The GLSL source of this program's vertex shader.
/// @param num_vertex_source The total number of items in the vertex shader source array.
/// @param fragment_source The GLSL source of this program's fragment shader.
/// @param num_fragment_source The total number of items in the fragment shader source array.
/// @param fragment_data_location The name of the attribute that this program's fragment shader uses for fragment data.
/// @param program The program to load the created program into.
void program_create(const char **vertex_source,
                    unsigned int num_vertex_source,
                    const char **fragment_source,
                    unsigned int num_fragment_source,
                    const char *fragment_data_location,
                    struct program_t *program);

/// Destroy the given program, releasing all of it's allocated memory.
///
/// This must be called after a program is created and before program execution completes.
/// @param program The program to destroy.
void program_destroy(struct program_t *program);
