//
//  constants.h
//  viewer
//
//  Created by Marika on 2020-02-22.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

/// The width of the content area of the main window displayed within a single viewer instance.
#define WINDOW_WIDTH 1280

/// The height of the content area of the main window displayed within a single viewer instance.
#define WINDOW_HEIGHT 720

/// The source for the standard 2D rendering program's vertex shader.
#define PROGRAM_2D_VERTEX_SOURCE \
{ \
    "#version 150 core\n", \
    "uniform mat4 model;", \
    "uniform mat4 projection;", \
    "in vec2 position;", \
    "in vec2 texcoord;", \
    "out vec2 vertexTexcoord;", \
    "void main()", \
    "{", \
    "    mat4 mvp = projection * model;", \
    "    gl_Position = mvp * vec4(position.x, position.y, 0.0, 1.0);", \
    "    vertexTexcoord = texcoord;", \
    "}", \
}

/// The source for the standard 2D rendering program's fragment shader.
#define PROGRAM_2D_FRAGMENT_SOURCE \
{ \
    "#version 150 core\n" \
    "uniform sampler2D sampler;", \
    "uniform vec4 colorMultiplier;", \
    "in vec2 vertexTexcoord;", \
    "out vec4 outColor;", \
    "void main()", \
    "{", \
    "    vec4 sampled = texture(sampler, vertexTexcoord.st);", \
    "    outColor = sampled * colorMultiplier;", \
    "}", \
}

/// The name of the "model" mat4 uniform within the standard 2D rendering program.
///
/// This uniform is used to provide the model portion of the MVP matrix.
#define PROGRAM_2D_UNIFORM_MODEL "model"

/// The name of the "projection" mat4 uniform within the standard 2D rendering program.
///
/// This uniform is used to provide the projection portion of the MVP matrix.
#define PROGRAM_2D_UNIFORM_PROJECTION "projection"

/// The name of the "sampler" sampler2D uniform within the standard 2D rendering program.
///
/// This uniform is used to define the sampler which the texture coordinates are used to sample.
#define PROGRAM_2D_UNIFORM_SAMPLER "sampler"

/// The name of the "colorMultiplier" vec4 uniform within the standard 2D rendering program.
///
/// This uniform is used to provide an RGBA colour which each sampled colour is multiplied by.
#define PROGRAM_2D_UNIFORM_COLOR_MULTIPLIER "colorMultiplier"

/// The name of the "position" vec2 vertex component within the standard 2D rendering program.
///
/// This component is used to define the X and Y positions of a vertex.
#define PROGRAM_2D_COMPONENT_POSITION "position"

/// The name of the "texcoord" vec2 vertex component within the standard 2D rendering program.
///
/// This component is used to define the U and V texture coordinates sampled by a vertex.
#define PROGRAM_2D_COMPONENT_TEXCOORD "texcoord"

/// The total number of vertices used to create a quad within the standard 2D rendering program.
///
/// 2 triangles * 3 vertices per triangle.
#define PROGRAM_2D_QUAD_VERTICES 2 * 3

/// The total number of floats used within a single vertex within the standard 2D rendering program.
///
/// X, Y, U, and V.
#define PROGRAM_2D_VERTEX_FLOATS 4

/// The total number of floats needed for the vertices of a quad within the standard 2D rendering program.
#define PROGRAM_2D_QUAD_FLOATS PROGRAM_2D_QUAD_VERTICES * PROGRAM_2D_VERTEX_FLOATS

/// The standard vertex array components used for vertices drawn by the standard 2D rendering program.
///
/// X float, Y float, U float, and V float.
#define PROGRAM_2D_VERTEX_COMPONENTS \
{ \
    { \
        .data_type = GL_FLOAT, \
        .num_values = 2, \
        .offset = 0 * sizeof(float), \
        .stride = 4 * sizeof(float), \
        .attribute_name = PROGRAM_2D_COMPONENT_POSITION, \
    }, \
    { \
        .data_type = GL_FLOAT, \
        .num_values = 2, \
        .offset = 2 * sizeof(float), \
        .stride = 4 * sizeof(float), \
        .attribute_name = PROGRAM_2D_COMPONENT_TEXCOORD, \
    }, \
}
