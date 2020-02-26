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
    // run the main loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
