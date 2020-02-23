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
///
/// This shader takes in `position` and `texcoord` vertex components,
/// where `position` is a top-left origin pixel-space position mapped using the `viewportSize` uniform,
/// and `texcoord` is passed to the fragment shader's `vertexTexcoord` vertex component.
#define PROGRAM_2D_VERTEX_SOURCE \
{ \
    "#version 150 core\n", \
    "uniform uvec2 viewportSize;", \
    "in vec2 position;", \
    "in vec2 texcoord;", \
    "out vec2 vertexTexcoord;", \
    "void main()", \
    "{", \
    "    float x = -1.0 + (position.x / float(viewportSize.x / uint(2)));" \
    "    float y =  1.0 + ((1.0 - position.y) / float(viewportSize.y / uint(2)));" \
    "    gl_Position = vec4(x, y, 0.0, 1.0);", \
    "    vertexTexcoord = texcoord;", \
    "}", \
}

/// The source for the standard 2D rendering program's fragment shader.
///
/// This shader takes in a `vertexTexcoord` vertex component and uses it to sample the `sampler`
/// uniform, which is then passed out via the `outColor` output.
#define PROGRAM_2D_FRAGMENT_SOURCE \
{ \
    "#version 150 core\n" \
    "uniform sampler2D sampler;", \
    "in vec2 vertexTexcoord;", \
    "out vec4 outColor;", \
    "void main()", \
    "{", \
    "    outColor = texture(sampler, vertexTexcoord.st);", \
    "}", \
}

/// The name of the viewport size uniform within the standard 2D rendering program.
#define PROGRAM_2D_UNIFORM_VIEWPORT_SIZE "viewportSize"

/// The name of the sampler uniform within the standard 2D rendering program.
#define PROGRAM_2D_UNIFORM_SAMPLER "sampler"

/// The name of the position vertex component within the standard 2D rendering program.
#define PROGRAM_2D_COMPONENT_POSITION "position"

/// The name of the texcoord vertex component within the standard 2D rendering program.
#define PROGRAM_2D_COMPONENT_TEXCOORD "texcoord"
