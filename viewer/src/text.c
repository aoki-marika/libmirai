//
//  text.c
//  viewer
//
//  Created by Marika on 2020-02-23.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "text.h"

#include <stdlib.h>

#include "constants.h"

// MARK: - Constants

/// The width of a single character within a text, in pixels.
const unsigned int text_character_width = 8;

/// The height of a single character within a text, in pixels.
const unsigned int text_character_height = 8;

/// The number of pixels that the cursor advances horizontally for each drawn character within a text.
const unsigned int text_character_advance = 6;

// MARK: - Functions

void text_create(unsigned int width,
                 unsigned int height,
                 const struct program_t *program2d,
                 struct text_t *text)
{
    // create empty base data for the texture
    const unsigned int w = width * text_character_width;
    const unsigned int h = height * text_character_height;
    uint8_t *empty_data = calloc(1, w * h * 4); //*4 for rgba channels

    // create the texture
    glGenTextures(1, &text->texture_id);
    glBindTexture(GL_TEXTURE_2D, text->texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA8,
                 width * text_character_advance,
                 height * text_character_height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 empty_data);

    // free the empty data now that its uploaded
    free(empty_data);

    // create the vertex array
    const int vertex_stride = 4 * sizeof(float);
    const struct vertex_component_t vertex_components[] =
    {
        {
            .data_type = GL_FLOAT,
            .num_values = 2,
            .offset = 0 * sizeof(float),
            .stride = vertex_stride,
            .attribute_name = PROGRAM_2D_COMPONENT_POSITION,
        },
        {
            .data_type = GL_FLOAT,
            .num_values = 2,
            .offset = 2 * sizeof(float),
            .stride = vertex_stride,
            .attribute_name = PROGRAM_2D_COMPONENT_TEXCOORD,
        },
    };

    const float vertices[] =
    {
    //  x  y    u    v
        0, 0, 0.0, 1.0, //top-left
        0, h, 0.0, 0.0, //bottom-left
        w, h, 1.0, 0.0, //bottom-right

        0, 0, 0.0, 1.0, //top-left
        w, 0, 1.0, 1.0, //top-right
        w, h, 1.0, 0.0, //bottom-right
    };

    vertex_array_create(vertices,
                        sizeof(vertices),
                        sizeof(vertex_components) / sizeof(struct vertex_component_t),
                        vertex_components,
                        program2d,
                        &text->quad_array);

    // initialize the text
    text->width = width;
    text->height = height;
}

void text_destroy(struct text_t *text)
{
    vertex_array_destroy(&text->quad_array);
    glDeleteTextures(1, &text->texture_id);
}

void text_draw(struct text_t *text)
{
    glBindVertexArray(text->quad_array.id);
    glDrawArrays(GL_TRIANGLES, 0, 6); //6 vertices per quad
}
