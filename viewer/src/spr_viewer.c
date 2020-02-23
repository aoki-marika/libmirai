//
//  spr_viewer.c
//  viewer
//
//  Created by Marika on 2020-02-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "spr_viewer.h"

#include <string.h>

#include "constants.h"
#include "vertex_array.h"
#include "texture.h"

// MARK: - Textures

/// Create the vertices for a quad displaying a sampled texture.
/// @param uv_bottom_left The bottom left UV coordinate of the quad.
/// @param uv_top_right The top right UV coordinate of the quad.
/// @param width The width of the quad.
/// @param height The height of the quad.
/// @param out The array to write the new vertices to, must have 24 entries available.
/// The data is formatted as X, Y, U, and V floats for each vertex, with there being 6 vertices (two triangles) per quad.
void spr_viewer_texture_quad_create(struct vec2_t uv_bottom_left,
                                    struct vec2_t uv_top_right,
                                    unsigned int width,
                                    unsigned int height,
                                    float *out)
{
    // create the vertices so that the texture is centered
    const float w = width, h = height;
    const float x = (WINDOW_WIDTH - w) / 2;
    const float y = (WINDOW_HEIGHT - h) / 2;
    const float vertices[] =
    {
        //  x,     y,                u,                v
        x,     y,     uv_bottom_left.x, uv_top_right.y,   // top-left
        x,     y + h, uv_bottom_left.x, uv_bottom_left.y, // bottom-left
        x + w, y + h, uv_top_right.x,   uv_bottom_left.y, // bottom-right

        x,     y,     uv_bottom_left.x, uv_top_right.y,   // top-left
        x + w, y,     uv_top_right.x,   uv_top_right.y,   // top-right
        x + w, y + h, uv_top_right.x,   uv_bottom_left.y, // bottom-right
    };

    // copy the vertices to the output
    memcpy(out, vertices, sizeof(vertices));
}

void spr_viewer_run(GLFWwindow *window,
                    struct spr_t *spr,
                    const struct program_t *program2d)
{
    // get the used uniforms from the program
    glUseProgram(program2d->id);
    GLint uniform_sampler = glGetUniformLocation(program2d->id, PROGRAM_2D_UNIFORM_SAMPLER);

    // upload each texture to a separate unit
    // use a base unit to allow the first n units to be used for debug ui
    const int texture_base_unit = GL_TEXTURE5;
    GLuint texture_ids[spr->num_textures];
    for (int i = 0; i < spr->num_textures; i++)
    {
        glActiveTexture(texture_base_unit + i);
        texture_upload(&spr->textures[i],
                       spr->file,
                       &texture_ids[i]);
    }

    // create the vertex arrays for each texture and scr
    // these only need to be created at load time, then they can be drawn whenever
    // create the vertex components once as they are shared
    const int vertex_stride = 4 * sizeof(float);
    const int num_quad_vertices = 2 * 3; //2 triangles * 3 points per triangle
    const int num_vertex_floats = 4; //x, y, u, v
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

    float texture_quads_vertices[num_quad_vertices * num_vertex_floats * spr->num_textures];
    for (int i = 0; i < spr->num_textures; i++)
    {
        struct ctr_texture_t *texture = &spr->textures[i];
        struct vec2_t uv_bottom_left = { .x = 0, .y = 0 };
        struct vec2_t uv_top_right = { .x = 1, .y = 1 };
        spr_viewer_texture_quad_create(uv_bottom_left,
                                       uv_top_right,
                                       texture->width,
                                       texture->height,
                                       &texture_quads_vertices[i * num_quad_vertices * num_vertex_floats]);
    }

    struct vertex_array_t texture_quads_array;
    vertex_array_create(texture_quads_vertices,
                        sizeof(texture_quads_vertices),
                        sizeof(vertex_components) / sizeof(struct vertex_component_t),
                        vertex_components,
                        program2d,
                        &texture_quads_array);

    float scr_quads_vertices[num_quad_vertices * num_vertex_floats * spr->num_scrs];
    for (int i = 0; i < spr->num_scrs; i++)
    {
        // invert the v coordinates to convert from top-left origin to bottom-left
        struct scr_t *scr = &spr->scrs[i];
        struct ctr_texture_t *texture = &spr->textures[scr->texture_index];
        struct vec2_t uv_bottom_left = { .x = scr->top_left.x, .y = 1.0 - scr->bottom_right.y };
        struct vec2_t uv_top_right = { .x = scr->bottom_right.x, .y = 1.0 - scr->top_left.y };
        float width = (scr->bottom_right.x - scr->top_left.x) * texture->width;
        float height = (scr->bottom_right.y - scr->top_left.y) * texture->height;
        spr_viewer_texture_quad_create(uv_bottom_left,
                                       uv_top_right,
                                       width,
                                       height,
                                       &scr_quads_vertices[i * num_quad_vertices * num_vertex_floats]);
    }

    struct vertex_array_t scr_quads_array;
    vertex_array_create(scr_quads_vertices,
                        sizeof(scr_quads_vertices),
                        sizeof(vertex_components) / sizeof(struct vertex_component_t),
                        vertex_components,
                        program2d,
                        &scr_quads_array);

    // run the main loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // free all the allocated resources
    vertex_array_destroy(&scr_quads_array);
    vertex_array_destroy(&texture_quads_array);
    glDeleteTextures(spr->num_textures, texture_ids);
}
