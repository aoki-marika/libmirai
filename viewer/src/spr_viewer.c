//
//  spr_viewer.c
//  viewer
//
//  Created by Marika on 2020-02-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "spr_viewer.h"

#include "texture.h"

// MARK: - Textures

void spr_viewer_run(GLFWwindow *window,
                    struct spr_t *spr,
                    const struct program_t *program2d)
{
    // upload each texture to a separate unit
    // use a base unit to allow the first n units to be used for debug ui
    const int spr_base_unit = GL_TEXTURE5;
    GLuint texture_ids[spr->num_textures];
    for (int i = 0; i < spr->num_textures; i++)
    {
        glActiveTexture(spr_base_unit + i);
        texture_upload(&spr->textures[i],
                       spr->file,
                       &texture_ids[i]);
    }

    // run the main loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // delete all the textures
    glDeleteTextures(spr->num_textures, texture_ids);
}
