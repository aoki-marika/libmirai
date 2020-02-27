//
//  aet_viewer.c
//  viewer
//
//  Created by Marika on 2020-02-25.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "aet_viewer.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "constants.h"
#include "matrix.h"
#include "texture.h"

// MARK: - Functions

/// Create a new viewer node from the given AET node.
/// @param backing The backing AET node of the viewer node.
/// @param scene The scene containing the given AET node.
/// @param spr The SPR containing the SCRs for the given AET node.
/// @param program2d The standard 2D rendering program.
/// @param node The viewer node to load the new node into.
void aet_viewer_node_create(const struct aet_node_t *backing,
                            const struct aet_scene_t *scene,
                            const struct spr_t *spr,
                            const struct program_t *program2d,
                            struct aet_viewer_node_t *node)
{
    // initialize the node
    node->backing = backing;
    node->num_sprite_quads = 0;
    node->sprite_quads_array = NULL;
    node->sprites_texture_index = 0;
    node->num_children = 0;
    node->children = NULL;

    // set the contents based on the backing
    switch (backing->contents_type)
    {
        case AET_NODE_CONTENTS_TYPE_SPRITE_GROUP:
        {
            // create the sprite quads vertices
            // occasionally a sprite can refer to an scr that does not exist,
            // so in that case it needs to be excluded from the vertex array
            struct aet_sprite_group_t *sprite_group = backing->sprite_group;
            unsigned int num_quads = 0;
            float vertices[sprite_group->num_sprites * PROGRAM_2D_QUAD_FLOATS];
            float w = sprite_group->width;
            float h = sprite_group->height;

            unsigned int sprites_texture_index = 0;
            for (int i = 0; i < sprite_group->num_sprites; i++)
            {
                struct aet_sprite_t *sprite = &sprite_group->sprites[i];
                char *scr_name = scene->scr_names[sprite->scr_index];
                struct scr_t *scr = spr_lookup(spr, scr_name);
                if (scr == NULL)
                    continue;

                float sprite_vertices[PROGRAM_2D_QUAD_FLOATS] =
                {
                //  x, y,                   u,                         v
                    0, 0, scr->top_left.x,     1.0 - scr->top_left.y,     // top-left
                    0, h, scr->top_left.x,     1.0 - scr->bottom_right.y, // bottom-left
                    w, h, scr->bottom_right.x, 1.0 - scr->bottom_right.y, // bottom-right

                    0, 0, scr->top_left.x,     1.0 - scr->top_left.y,     // top-left
                    w, 0, scr->bottom_right.x, 1.0 - scr->top_left.y,     // top-right
                    w, h, scr->bottom_right.x, 1.0 - scr->bottom_right.y, // bottom-right
                };

                memcpy(&vertices[i * PROGRAM_2D_QUAD_FLOATS], sprite_vertices, sizeof(sprite_vertices));
                sprites_texture_index = scr->texture_index;
                num_quads++;
            }

            // initialize the node
            node->num_sprite_quads = num_quads;
            node->sprite_quads_array = malloc(sizeof(struct vertex_array_t));
            node->sprites_texture_index = sprites_texture_index;

            // create the vertex array
            struct vertex_component_t vertex_components[] = PROGRAM_2D_VERTEX_COMPONENTS;
            vertex_array_create(vertices,
                                num_quads * PROGRAM_2D_QUAD_FLOATS * sizeof(float),
                                sizeof(vertex_components) / sizeof(struct vertex_component_t),
                                vertex_components,
                                program2d,
                                node->sprite_quads_array);
            break;
        }
        case AET_NODE_CONTENTS_TYPE_CHILDREN:
        {
            // insert the children
            node->num_children = backing->num_children;
            node->children = malloc(backing->num_children * sizeof(struct aet_viewer_node_t));
            for (int i = 0; i < backing->num_children; i++)
            {
                const struct aet_node_t *child_backing = &backing->children[i];
                struct aet_viewer_node_t child;
                aet_viewer_node_create(child_backing, scene, spr, program2d, &child);
                node->children[i] = child;
            }

            break;
        }
    }
}

/// Create a new viewer node from the given AET node group.
/// @param backing The backing AET node group of the viewer node.
/// @param scene The scene containing the given AET group.
/// @param spr The SPR containing the SCRs for the given AET node group's nodes.
/// @param program2d The standard 2D rendering program.
/// @param node The viewer node to load the new node into.
void aet_viewer_node_create_group(const struct aet_node_group_t *backing,
                                  const struct aet_scene_t *scene,
                                  const struct spr_t *spr,
                                  const struct program_t *program2d,
                                  struct aet_viewer_node_t *node)
{
    // initialize the node
    node->backing = NULL;
    node->num_sprite_quads = 0;
    node->sprite_quads_array = NULL;
    node->sprites_texture_index = 0;

    // insert the children
    node->num_children = backing->num_nodes;
    node->children = malloc(backing->num_nodes * sizeof(struct aet_viewer_node_t));
    for (int i = 0; i < backing->num_nodes; i++)
    {
        const struct aet_node_t *child_backing = &backing->nodes[i];
        struct aet_viewer_node_t child;
        aet_viewer_node_create(child_backing, scene, spr, program2d, &child);
        node->children[i] = child;
    }
}

/// Free the given node and all of it's allocated resources.
/// @param node The node to free.
void aet_viewer_node_destroy(struct aet_viewer_node_t *node)
{
    for (int i = 0; i < node->num_children; i++)
        aet_viewer_node_destroy(&node->children[i]);

    if (node->children != NULL)
        free(node->children);

    if (node->sprite_quads_array != NULL)
    {
        vertex_array_destroy(node->sprite_quads_array);
        free(node->sprite_quads_array);
    }
}

/// Update the cached properties of the given node and it's children, such as world transform and opacity.
/// @param origin_x The base X origin of the given node.
/// @param origin_y The base Y origin of the given node.
/// @param position_x The base X position of the given node.
/// @param position_y The base Y position of the given node.
/// @param rotation The base rotation of the given node.
/// @param scale_x The base X scale of the given node.
/// @param scale_y The base Y scale of the given node.
/// @param opacity The base opacity of the given node.
/// @param node The node to update the transforms of.
void aet_viewer_node_update_properties(float origin_x,
                                       float origin_y,
                                       float position_x,
                                       float position_y,
                                       float rotation,
                                       float scale_x,
                                       float scale_y,
                                       float opacity,
                                       struct aet_viewer_node_t *node)
{
    // apply the given nodes properties to the base
    if (node->backing != NULL)
    {
        const struct aet_node_t *aet_node = node->backing;
        float node_origin_x = aet_node->origin_x.values[0];
        float node_origin_y = aet_node->origin_y.values[0];
        float node_position_x = aet_node->position_x.values[0];
        float node_position_y = aet_node->position_y.values[0];
        float node_rotation = aet_node->rotation.values[0];
        float node_scale_x = aet_node->scale_x.values[0];
        float node_scale_y = aet_node->scale_y.values[0];
        float node_opacity = aet_node->opacity.values[0];

        position_x = (position_x - origin_x) + node_position_x;
        position_y = (position_y - origin_y) + node_position_y;
        origin_x = node_origin_x;
        origin_y = node_origin_y;
        rotation += node_rotation;
        scale_x *= node_scale_x;
        scale_y *= node_scale_y;
        opacity *= node_opacity;
    }

    // create the world transform
    struct mat4_t transform_world = mat4_identity();
    struct vec3_t scale = { .x = scale_x, .y = scale_y, .z = 1 };
    float rotation_radians = rotation * (M_PI / 180);
    struct vec3_t position_scaled = { .x = position_x / scale.x, .y = position_y / scale.y, .z = 0 };
    struct vec3_t origin_inverse = { .x = -origin_x, .y = -origin_y, .z = 0 };
    transform_world = mat4_multiply(transform_world, mat4_scaling(scale));
    transform_world = mat4_multiply(transform_world, mat4_translation(position_scaled));
    transform_world = mat4_multiply(transform_world, mat4_rotation_z(rotation_radians));
    transform_world = mat4_multiply(transform_world, mat4_translation(origin_inverse));

    // set the properties
    node->transform_world = transform_world;
    node->opacity = opacity;

    // update all the children
    for (int i = 0; i < node->num_children; i++)
        aet_viewer_node_update_properties(origin_x,
                                          origin_y,
                                          position_x,
                                          position_y,
                                          rotation,
                                          scale_x,
                                          scale_y,
                                          opacity,
                                          &node->children[i]);
}

/// Draw the given node and all of it's children.
/// @param viewer The viewer drawing the given node.
/// @param uniform_model The model transform uniform location within the active program.
/// @param uniform_sampler The texture sampler uniform location within the active program.
/// @param uniform_multiply_color The multiply colour uniform location within the active program.
/// @param node The node to draw.
void aet_viewer_node_draw(struct aet_viewer_t *viewer,
                          GLint uniform_model,
                          GLint uniform_sampler,
                          GLint uniform_multiply_color,
                          struct aet_viewer_node_t *node)
{
    // draw the given nodes contents
    if (node->backing != NULL)
    {
        switch (node->backing->contents_type)
        {
            case AET_NODE_CONTENTS_TYPE_SPRITE_GROUP:
            {
                GLenum unit = viewer->textures_base_unit + node->sprites_texture_index;
                struct color4_t multiply_color = { .r = 1, .g = 1, .b = 1, .a = node->opacity };
                glActiveTexture(unit);
                glBindVertexArray(node->sprite_quads_array->id);
                glUniformMatrix4fv(uniform_model, 1, GL_FALSE, (GLfloat *)&node->transform_world.data);
                glUniform1i(uniform_sampler, unit - GL_TEXTURE0);
                glUniform4fv(uniform_multiply_color, 1, (GLfloat *)&multiply_color);
                glDrawArrays(GL_TRIANGLES, 0, node->num_sprite_quads * PROGRAM_2D_QUAD_VERTICES);
                break;
            }
            default:
                break;
        }
    }

    // children are drawn in reverse order
    for (int i = node->num_children - 1; i >= 0; i--)
        aet_viewer_node_draw(viewer,
                             uniform_model,
                             uniform_sampler,
                             uniform_multiply_color,
                             &node->children[i]);
}

void aet_viewer_create(const struct aet_t *aet,
                       const struct spr_t *spr,
                       const struct program_t *program2d,
                       struct aet_viewer_t *viewer)
{
    // upload all the spr textures
    viewer->textures_base_unit = GL_TEXTURE5;
    viewer->texture_ids = malloc(spr->num_textures * sizeof(GLuint));
    for (int i = 0; i < spr->num_textures; i++)
    {
        glActiveTexture(viewer->textures_base_unit + i);
        texture_upload(&spr->textures[i],
                       spr->file,
                       &viewer->texture_ids[i]);
    }

    // initialize the viewer
    viewer->aet = aet;
    viewer->spr = spr;
    viewer->program2d = program2d;
}

void aet_viewer_destroy(struct aet_viewer_t *viewer)
{
    glDeleteTextures(viewer->spr->num_textures, viewer->texture_ids);
    free(viewer->texture_ids);
}

void aet_viewer_run(GLFWwindow *window, struct aet_viewer_t *viewer)
{
    // get the uniform locations
    glUseProgram(viewer->program2d->id);
    GLint uniform_model = glGetUniformLocation(viewer->program2d->id, PROGRAM_2D_UNIFORM_MODEL);
    GLint uniform_sampler = glGetUniformLocation(viewer->program2d->id, PROGRAM_2D_UNIFORM_SAMPLER);
    GLint uniform_multiply_color = glGetUniformLocation(viewer->program2d->id, PROGRAM_2D_UNIFORM_MULTIPLY_COLOR);

    // create the root node
    // TODO: REMOVEME
    // this is temporary while an interface is not implemented
    const struct aet_t *aet = viewer->aet;
    const unsigned int scene_index = 0;

    const struct aet_scene_t *scene = &aet->scenes[scene_index];
    const unsigned int node_group_index = scene->num_node_groups - 1;

    const struct aet_node_group_t *node_group = &scene->node_groups[node_group_index];
    struct aet_viewer_node_t root;
    aet_viewer_node_create_group(node_group, scene, viewer->spr, viewer->program2d, &root);

    // set the initial properties
    // set the base position to center the scene within the window
    float start_x = (WINDOW_WIDTH - scene->width) / 2;
    float start_y = (WINDOW_HEIGHT - scene->height) / 2;
    aet_viewer_node_update_properties(0, 0, //origin
                                      start_x, start_y, //position
                                      0, //rotation
                                      1, 1, //scale
                                      1, //opacity
                                      &root);

    // run the main loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        // draw the root node
        aet_viewer_node_draw(viewer,
                             uniform_model,
                             uniform_sampler,
                             uniform_multiply_color,
                             &root);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // destroy the root node
    // TODO: REMOVEME
    aet_viewer_node_destroy(&root);
}
