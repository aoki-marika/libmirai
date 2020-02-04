//
//  program.c
//  viewer
//
//  Created by Marika on 2020-02-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "program.h"

#include <stdio.h>
#include <stdlib.h>

// MARK: - Functions

/// Create and compile a new shader from the given properties.
/// @param source The GLSL source of the shader.
/// @param num_source The total number of items in the source array.
/// @param type The type of this shader, either `GL_VERTEX_SHADER` or `GL_FRAGMENT_SHADER`.
/// @param id The value to set to the created shader's ID.
void program_shader_create(const char **source,
                           unsigned int num_source,
                           GLenum type,
                           GLuint *id)
{
    // create and compile the shader
    *id = glCreateShader(type);
    glShaderSource(*id, num_source, source, NULL);
    glCompileShader(*id);

    // check for errors
    GLint success;
    glGetShaderiv(*id, GL_COMPILE_STATUS, &success);
    if (success != GL_TRUE)
    {
        fprintf(stderr, "ERROR: failed to compile shader of type %08x\n", type);

        // print the source
        fprintf(stderr, "source:\n");
        for (int i = 0; i < num_source; i++)
            fprintf(stderr, "%s", source[i]);
        fprintf(stderr, "\n");

        // print the log
        char log_buffer[1024];
        glGetShaderInfoLog(*id, sizeof(log_buffer), NULL, log_buffer);
        fprintf(stderr, "log:\n%s", log_buffer);

        // exit the program
        // there could be a whole error handling scheme and whatnot but ughhh
        // this is only a demo program so its okay to exit on failures
        exit(EXIT_FAILURE);
    }
}

void program_create(unsigned int num_vertex_source,
                    const char **vertex_source,
                    unsigned int num_fragment_source,
                    const char **fragment_source,
                    const char *fragment_data_location,
                    struct program_t *program)
{
    // create the vertex and fragment shaders
    GLuint vertex_id;
    program_shader_create(vertex_source,
                          num_vertex_source,
                          GL_VERTEX_SHADER,
                          &vertex_id);

    GLuint fragment_id;
    program_shader_create(fragment_source,
                          num_fragment_source,
                          GL_FRAGMENT_SHADER,
                          &fragment_id);

    // create and link the program
    GLuint id = glCreateProgram();
    glAttachShader(id, vertex_id);
    glAttachShader(id, fragment_id);
    glBindFragDataLocation(id, 0, fragment_data_location);
    glLinkProgram(id);

    // initialize the program
    program->id = id;
    program->vertex_id = vertex_id;
    program->fragment_id = fragment_id;
}

void program_destroy(struct program_t *program)
{
    glDetachShader(program->id, program->vertex_id);
    glDetachShader(program->id, program->fragment_id);

    glDeleteShader(program->vertex_id);
    glDeleteShader(program->fragment_id);

    glDeleteProgram(program->id);
}
