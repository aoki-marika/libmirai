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

/// Put a description of the given texture onto the given text.
/// @param x The X position to put the description, in characters.
/// @param y The Y position to put the description, in characters.
/// @param texture The texture to put the description of.
/// @param name The name of the given texture.
/// @param text The text to put the description onto.
void spr_viewer_put_texture(unsigned int x,
                            unsigned int y,
                            const struct ctr_texture_t *texture,
                            const char *name,
                            struct text_t *text)
{
    unsigned int header_x = x;
    unsigned int contents_x = x + 1;
    unsigned int current_y = y;

    // put the header and name
    text_put_string(header_x, current_y++, "TEXTURE", text);
    text_put_string(contents_x, current_y++, name, text);

    // put the data format
    switch (texture->data_format)
    {
        case CTR_TEXTURE_FORMAT_RGBA8888: text_put_string(contents_x, current_y++, "RGBA8888", text); break;
        case CTR_TEXTURE_FORMAT_RGB888:   text_put_string(contents_x, current_y++, "RGB888", text); break;
        case CTR_TEXTURE_FORMAT_RGBA5551: text_put_string(contents_x, current_y++, "RGBA5551", text); break;
        case CTR_TEXTURE_FORMAT_RGB565:   text_put_string(contents_x, current_y++, "RGB565", text); break;
        case CTR_TEXTURE_FORMAT_RGBA4444: text_put_string(contents_x, current_y++, "RGBA4444", text); break;
        case CTR_TEXTURE_FORMAT_LA88:     text_put_string(contents_x, current_y++, "LA88", text); break;
        case CTR_TEXTURE_FORMAT_HL8:      text_put_string(contents_x, current_y++, "HL8", text); break;
        case CTR_TEXTURE_FORMAT_L8:       text_put_string(contents_x, current_y++, "L8", text); break;
        case CTR_TEXTURE_FORMAT_A8:       text_put_string(contents_x, current_y++, "A8", text); break;
        case CTR_TEXTURE_FORMAT_LA44:     text_put_string(contents_x, current_y++, "LA44", text); break;
        case CTR_TEXTURE_FORMAT_L4:       text_put_string(contents_x, current_y++, "L4", text); break;
        case CTR_TEXTURE_FORMAT_A4:       text_put_string(contents_x, current_y++, "A4", text); break;
        case CTR_TEXTURE_FORMAT_ETC1:     text_put_string(contents_x, current_y++, "ETC1", text); break;
        case CTR_TEXTURE_FORMAT_ETC1_A4:  text_put_string(contents_x, current_y++, "ETC1 A4", text); break;
    }

    // put the size
    text_put_int(contents_x, current_y, texture->width, 4, text);
    text_put_character(contents_x + 5, current_y, 'X', text);
    text_put_int(contents_x + 7, current_y, texture->height, 4, text);
}

/// Put a description of the given SCR onto the given text.
/// @param x The X position to put the description, in characters.
/// @param y The Y position to put the description, in characters.
/// @param scr The SCR to put the description of.
/// @param texture The texture containing the given SCR.
/// @param text The text to put the description onto.
void spr_viewer_put_scr(unsigned int x,
                        unsigned int y,
                        const struct scr_t *scr,
                        const struct ctr_texture_t *texture,
                        struct text_t *text)
{
    unsigned int header_x = x;
    unsigned int contents_x = x + 1;
    unsigned int current_y = y;

    // put the header and name
    text_put_string(header_x, current_y++, "SCR", text);
    text_put_string(contents_x, current_y++, scr->name, text);

    // put the size
    unsigned int width = (scr->bottom_right.x - scr->top_left.x) * texture->width;
    unsigned int height = (scr->bottom_right.y - scr->top_left.y) * texture->height;
    text_put_int(contents_x, current_y, width, 4, text);
    text_put_character(contents_x + 5, current_y, 'X', text);
    text_put_int(contents_x + 7, current_y, height, 4, text);
}

/// Update the debug interface text of the given viewer.
/// @param viewer The viewer to update.
void spr_viewer_update_text(struct spr_viewer_t *viewer)
{
    // update the textures text
    glActiveTexture(viewer->textures_text_unit);
    text_clear(&viewer->textures_text);

    if (viewer->texture_index < viewer->spr->num_textures)
    {
        struct ctr_texture_t *texture = &viewer->spr->textures[viewer->texture_index];
        char *texture_name = viewer->spr->texture_names[viewer->texture_index];
        spr_viewer_put_texture(1, 1, texture, texture_name, &viewer->textures_text);
    }

    // update the scrs text
    glActiveTexture(viewer->scrs_text_unit);
    text_clear(&viewer->scrs_text);

    if (viewer->scr_index < viewer->spr->num_scrs)
    {
        struct scr_t *scr = &viewer->spr->scrs[viewer->scr_index];
        struct ctr_texture_t *scr_texture = &viewer->spr->textures[scr->texture_index];
        char *scr_texture_name = viewer->spr->texture_names[scr->texture_index];
        spr_viewer_put_scr(1, 1, scr, scr_texture, &viewer->scrs_text);
        spr_viewer_put_texture(1, 5, scr_texture, scr_texture_name, &viewer->scrs_text);
    }
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

    // create the texts
    viewer->textures_text_unit = GL_TEXTURE0;
    glActiveTexture(viewer->textures_text_unit);
    text_create(80,
                60,
                program2d,
                &viewer->textures_text);

    viewer->scrs_text_unit = GL_TEXTURE1;
    glActiveTexture(viewer->scrs_text_unit);
    text_create(80,
                60,
                program2d,
                &viewer->scrs_text);

    // initialize the viewer
    viewer->spr = spr;
    viewer->program2d = program2d;
    viewer->screen = SPR_VIEWER_SCREEN_TEXTURES;
    viewer->texture_index = 0;
    viewer->scr_index = 0;
}

void spr_viewer_destroy(struct spr_viewer_t *viewer)
{
    text_destroy(&viewer->scrs_text);
    text_destroy(&viewer->textures_text);

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
    bool display_changed = false;
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
            {
                *index -= 1;
                display_changed = true;
            }
            break;
        case GLFW_KEY_RIGHT:
            if (*index + 1 < index_max)
            {
                *index += 1;
                display_changed = true;
            }
            break;
        default:
            break;
    }

    // update the debug interface text if the displayed item changed
    if (display_changed)
        spr_viewer_update_text(viewer);
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

    // set the initial text
    spr_viewer_update_text(viewer);

    // run the main loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // get the index of the texture unit and quad to draw
        // the unit is relative to the base unit of the viewer
        // is valid is used to ensure that there are any vertices to draw
        // also get the text and its texture unit to draw the debug interface
        int unit_index, quad_index;
        bool is_valid;
        struct text_t *text;
        GLenum text_unit;
        switch (viewer->screen)
        {
            case SPR_VIEWER_SCREEN_TEXTURES:
            {
                glBindVertexArray(viewer->texture_quads_array.id);
                unit_index = viewer->texture_index;
                quad_index = viewer->texture_index;
                is_valid = viewer->texture_index < spr->num_textures;
                text = &viewer->textures_text;
                text_unit = viewer->textures_text_unit;
                break;
            }
            case SPR_VIEWER_SCREEN_SCRS:
            {
                glBindVertexArray(viewer->scr_quads_array.id);
                unit_index = spr->scrs[viewer->scr_index].texture_index;
                quad_index = viewer->scr_index;
                is_valid = viewer->scr_index < spr->num_scrs;
                text = &viewer->scrs_text;
                text_unit = viewer->scrs_text_unit;
                break;
            }
        }

        // bind the sampler and draw the quad if there is one
        if (is_valid)
        {
            glUniform1i(uniform_sampler, (viewer->textures_base_unit - GL_TEXTURE0) + unit_index);
            glDrawArrays(GL_TRIANGLES, quad_index * 6, 6); //6 vertices per quad
        }

        // draw the debug interface
        glActiveTexture(text_unit);
        glUniform1i(uniform_sampler, text_unit - GL_TEXTURE0);
        text_draw(text);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
