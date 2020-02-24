//
//  spr_viewer.c
//  viewer
//
//  Created by Marika on 2020-02-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "spr_viewer.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "constants.h"
#include "vertex_array.h"
#include "texture.h"

// MARK: - Functions

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

void spr_viewer_create(const struct spr_t *spr,
                       const struct program_t *program2d,
                       struct spr_viewer_t *viewer)
{
    // upload each texture to a separate unit
    // use a base unit to allow the first n units to be used for debug ui
    viewer->textures_base_unit = GL_TEXTURE5;
    viewer->texture_ids = malloc(spr->num_textures * sizeof(GLuint));
    for (int i = 0; i < spr->num_textures; i++)
    {
        glActiveTexture(viewer->textures_base_unit + i);
        texture_upload(&spr->textures[i],
                       spr->file,
                       &viewer->texture_ids[i]);
    }

    // create the quad arrays for each texture and scr
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

    vertex_array_create(texture_quads_vertices,
                        sizeof(texture_quads_vertices),
                        sizeof(vertex_components) / sizeof(struct vertex_component_t),
                        vertex_components,
                        program2d,
                        &viewer->texture_quads_array);

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

    vertex_array_create(scr_quads_vertices,
                        sizeof(scr_quads_vertices),
                        sizeof(vertex_components) / sizeof(struct vertex_component_t),
                        vertex_components,
                        program2d,
                        &viewer->scr_quads_array);

    // initialize the viewer
    viewer->spr = spr;
    viewer->program2d = program2d;
    viewer->screen = SPR_VIEWER_SCREEN_TEXTURES;
    viewer->texture_index = 0;
    viewer->scr_index = 0;
}

void spr_viewer_destroy(struct spr_viewer_t *viewer)
{
    vertex_array_destroy(&viewer->scr_quads_array);
    vertex_array_destroy(&viewer->texture_quads_array);

    glDeleteTextures(viewer->spr->num_textures, viewer->texture_ids);
    free(viewer->texture_ids);
}

void spr_viewer_key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    // only handle key presses and repeats
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    // get the viewer
    struct spr_viewer_t *viewer = (struct spr_viewer_t *)glfwGetWindowUserPointer(window);

    // get the index to modify in the case that the pressed key does so
    // also get the maximum value to ensure it never goes over, half-open
    unsigned int *index;
    unsigned int index_max;
    switch (viewer->screen)
    {
        case SPR_VIEWER_SCREEN_TEXTURES:
            index = &viewer->texture_index;
            index_max = viewer->spr->num_textures;
            break;
        case SPR_VIEWER_SCREEN_SCRS:
            index = &viewer->scr_index;
            index_max = viewer->spr->num_scrs;
            break;
    }

    // handle the different keys
    switch (key)
    {
        case GLFW_KEY_F1:
            viewer->screen = SPR_VIEWER_SCREEN_TEXTURES;
            break;
        case GLFW_KEY_F2:
            viewer->screen = SPR_VIEWER_SCREEN_SCRS;
            break;
        case GLFW_KEY_LEFT:
            if (*index > 0)
                *index -= 1;
            break;
        case GLFW_KEY_RIGHT:
            if (*index + 1 < index_max)
                *index += 1;
            break;
        default:
            break;
    }
}

void spr_viewer_run(GLFWwindow *window, struct spr_viewer_t *viewer)
{
    // create a shorthand for the spr
    const struct spr_t *spr = viewer->spr;

    // setup key handling
    // use the windows user pointer to pass the viewer to the key callback
    glfwSetWindowUserPointer(window, viewer);
    glfwSetKeyCallback(window, spr_viewer_key_callback);

    // get the used uniforms from the program
    const struct program_t *program = viewer->program2d;
    glUseProgram(program->id);
    GLint uniform_sampler = glGetUniformLocation(program->id, PROGRAM_2D_UNIFORM_SAMPLER);

    // run the main loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // get the index of the texture unit and quad to draw
        // the unit is relative to the base unit of the viewer
        // is valid is used to ensure that there are any vertices to draw
        int unit_index, quad_index;
        bool is_valid;
        switch (viewer->screen)
        {
            case SPR_VIEWER_SCREEN_TEXTURES:
            {
                glBindVertexArray(viewer->texture_quads_array.id);
                unit_index = viewer->texture_index;
                quad_index = viewer->texture_index;
                is_valid = viewer->texture_index < spr->num_textures;
                break;
            }
            case SPR_VIEWER_SCREEN_SCRS:
            {
                glBindVertexArray(viewer->scr_quads_array.id);
                unit_index = spr->scrs[viewer->scr_index].texture_index;
                quad_index = viewer->scr_index;
                is_valid = viewer->scr_index < spr->num_scrs;
                break;
            }
        }

        // bind the sampler and draw the quad if there is one
        if (is_valid)
        {
            glUniform1i(uniform_sampler, (viewer->textures_base_unit - GL_TEXTURE0) + unit_index);
            glDrawArrays(GL_TRIANGLES, quad_index * 6, 6); //6 vertices per quad
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
