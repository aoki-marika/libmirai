//
//  vertex_array.c
//  viewer
//
//  Created by Marika on 2020-02-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "vertex_array.h"

// MARK: - Functions

void vertex_array_create(const void *data,
                         size_t data_size,
                         unsigned int num_components,
                         const struct vertex_component_t *components,
                         const struct program_t *program,
                         struct vertex_array_t *vertex_array)
{
    // create the buffer
    GLuint buffer_id;
    glGenBuffers(1, &buffer_id);
    glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);

    // create the array
    GLuint id;
    glGenVertexArrays(1, &id);
    glBindVertexArray(id);

    // bind the components
    for (int i = 0; i < num_components; i++)
    {
        const struct vertex_component_t *component = &components[i];
        GLuint attribute = (GLuint)glGetAttribLocation(program->id, component->attribute_name);
        glVertexAttribPointer(attribute,
                              component->num_values,
                              component->data_type,
                              GL_FALSE,
                              component->stride,
                              (void *)((size_t)component->offset));

        glEnableVertexAttribArray(attribute);
    }

    // initialize the array
    vertex_array->id = id;
    vertex_array->buffer_id = buffer_id;
}

void vertex_array_destroy(struct vertex_array_t *vertex_array)
{
    glDeleteBuffers(1, &vertex_array->buffer_id);
    glDeleteVertexArrays(1, &vertex_array->id);
}
