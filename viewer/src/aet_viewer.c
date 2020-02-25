//
//  aet_viewer.c
//  viewer
//
//  Created by Marika on 2020-02-25.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "aet_viewer.h"

// MARK: - Functions

void aet_viewer_create(const struct aet_t *aet,
                       const struct program_t *program2d,
                       struct aet_viewer_t *viewer)
{
    viewer->aet = aet;
    viewer->program2d = program2d;
}

void aet_viewer_destroy(struct aet_viewer_t *viewer)
{
}

void aet_viewer_run(GLFWwindow *window, struct aet_viewer_t *viewer)
{
    // run the main loop
    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}
