//
//  main.c
//  viewer
//
//  Created by Marika on 2020-02-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gl.h"

// MARK: - Enumerations

/// The different file formats that this program can read.
enum file_format_t
{
    /// An SPR file.
    FILE_FORMAT_SPR  = 0x0,

    /// An AET file.
    FILE_FORMAT_AET  = 0x1,

    /// A CGFX file.
    FILE_FORMAT_CGFX = 0x2,
};

// MARK: - Functions

/// Print program usage to standard out.
void print_usage()
{
    printf("usage: [-spr|-aet|-cgfx] [file]\n");
}

int main(int argc, char **argv)
{
    // ensure the proper amount of flags were passed
    if (argc != 3)
    {
        print_usage();
        exit(EXIT_FAILURE);
    }

    // read the format flag
    char *format_flag = argv[1];
    enum file_format_t format;
    if (strcmp(format_flag, "-spr") == 0)
    {
        format = FILE_FORMAT_SPR;
    }
    else if (strcmp(format_flag, "-aet") == 0)
    {
        format = FILE_FORMAT_AET;
    }
    else if (strcmp(format_flag, "-cgfx") == 0)
    {
        format = FILE_FORMAT_CGFX;
    }
    else
    {
        print_usage();
        exit(EXIT_FAILURE);
    }

    // ensure the file exists
    char *path = argv[2];
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    // only need the file handle to check that the file exists
    fclose(file);

    // open the viewer
    // setup glfw
    if (!glfwInit())
        return EXIT_FAILURE;

    // create the window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow(400, 240, path, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // setup opengl
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // run the main loop
    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // terminate glfw
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
