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
#include "program.h"
#include "spr_viewer.h"
#include "aet_viewer.h"

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

    glClearColor(0.2, 0.2, 0.2, 1);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // create the shared 2d program and set the constant uniforms
    const char *vertex_source[] = PROGRAM_2D_VERTEX_SOURCE;
    const char *fragment_source[] = PROGRAM_2D_FRAGMENT_SOURCE;

    struct program_t program2d;
    program_create(sizeof(vertex_source) / sizeof(char *),
                   vertex_source,
                   sizeof(fragment_source) / sizeof(char *),
                   fragment_source,
                   "outColor",
                   &program2d);

    glUseProgram(program2d.id);
    GLint uniform_viewport_size = glGetUniformLocation(program2d.id, PROGRAM_2D_UNIFORM_VIEWPORT_SIZE);
    glUniform2ui(uniform_viewport_size, WINDOW_WIDTH, WINDOW_HEIGHT);

    // run the viewer
    // each viewer is repsonsible for its own main loop and event handling
    // as to make it easier to have custom functionality per-viewer
    switch (format)
    {
        case FILE_FORMAT_SPR:
        {
            struct spr_t spr;
            struct spr_viewer_t viewer;
            spr_open(path, &spr);
            spr_viewer_create(&spr, &program2d, &viewer);
            spr_viewer_run(window, &viewer);
            spr_viewer_destroy(&viewer);
            spr_close(&spr);
            break;
        }
        case FILE_FORMAT_AET:
        {
            struct aet_t aet;
            struct aet_viewer_t viewer;
            aet_open(path, &aet);
            aet_viewer_create(&aet, &program2d, &viewer);
            aet_viewer_run(window, &viewer);
            aet_viewer_destroy(&viewer);
            aet_close(&aet);
            break;
        }
        case FILE_FORMAT_CGFX:
        {
            fprintf(stderr, "ERROR: cgfx viewer is not implemented\n");
            return EXIT_FAILURE;
        }
    }

    // free all the allocated resources and exit
    program_destroy(&program2d);
    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
