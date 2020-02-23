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
#include "constants.h"
#include "spr_viewer.h"

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
    // note that forward compatibility is needed for gl3 on macOS
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, path, NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    // setup opengl
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // run the viewer
    // each viewer is repsonsible for its own main loop and event handling
    // as to make it easier to have custom functionality per-viewer
    switch (format)
    {
        case FILE_FORMAT_SPR:
        {
            struct spr_t spr;
            spr_open(path, &spr);
            spr_viewer_run(window, &spr);
            spr_close(&spr);
            break;
        }
        case FILE_FORMAT_AET:
        {
            fprintf(stderr, "ERROR: aet viewer is not implemented\n");
            return EXIT_FAILURE;
        }
        case FILE_FORMAT_CGFX:
        {
            fprintf(stderr, "ERROR: cgfx viewer is not implemented\n");
            return EXIT_FAILURE;
        }
    }

    // teardown glfw
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
