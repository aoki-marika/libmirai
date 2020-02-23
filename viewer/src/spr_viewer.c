//
//  spr_viewer.c
//  viewer
//
//  Created by Marika on 2020-02-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "spr_viewer.h"

// MARK: - Functions

void spr_viewer_run(GLFWwindow *window,
                    struct spr_t *spr,
                    const struct program_t *program2d)
{
    // run the main loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
