//
//  aet_viewer.c
//  viewer
//
//  Created by Marika on 2020-02-25.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "aet_viewer.h"

#include <stdlib.h>

#include "texture.h"

// MARK: - Functions

/// Create a new viewer node from the given AET node.
/// @param backing The backing AET node of the viewer node.
/// @param node The viewer node to load the new node into.
void aet_viewer_node_create(const struct aet_node_t *backing,
                            struct aet_viewer_node_t *node)
{
    node->backing = backing;
    node->num_children = backing->num_children;
    node->children = malloc(backing->num_children * sizeof(struct aet_viewer_node_t));
    for (int i = 0; i < backing->num_children; i++)
    {
        const struct aet_node_t *child_backing = &backing->children[i];
        struct aet_viewer_node_t child;
        aet_viewer_node_create(child_backing, &child);
        node->children[i] = child;
    }
}

/// Create a new viewer node from the given AET node group.
/// @param backing The backing AET node group of the viewer node.
/// @param node The viewer node to load the new node into.
void aet_viewer_node_create_group(const struct aet_node_group_t *backing,
                                  struct aet_viewer_node_t *node)
{
    node->backing = NULL;
    node->num_children = backing->num_nodes;
    node->children = malloc(backing->num_nodes * sizeof(struct aet_viewer_node_t));
    for (int i = 0; i < backing->num_nodes; i++)
    {
        const struct aet_node_t *child_backing = &backing->nodes[i];
        struct aet_viewer_node_t child;
        aet_viewer_node_create(child_backing, &child);
        node->children[i] = child;
    }
}

/// Free the given node and all of it's allocated resources.
/// @param node The node to free.
void aet_viewer_node_destroy(struct aet_viewer_node_t *node)
{
    for (int i = 0; i < node->num_children; i++)
        aet_viewer_node_destroy(&node->children[i]);

    free(node->children);
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
    // create the root node
    // TODO: REMOVEME
    // this is temporary while an interface is not implemented
    const struct aet_t *aet = viewer->aet;
    const unsigned int scene_index = 0;

    const struct aet_scene_t *scene = &aet->scenes[scene_index];
    const unsigned int node_group_index = scene->num_node_groups - 1;

    const struct aet_node_group_t *node_group = &scene->node_groups[node_group_index];
    struct aet_viewer_node_t root;
    aet_viewer_node_create_group(node_group, &root);

    // run the main loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // destroy the root node
    // TODO: REMOVEME
    aet_viewer_node_destroy(&root);
}
