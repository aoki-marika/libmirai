//
//  mtob.h
//  libmirai
//
//  Created by Marika on 2020-01-19.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

#include "color.h"
#include "vector.h"
#include "matrix.h"

// MARK: - Enumerations

/// The different methods that a texture coordinator can use to map it's texture onto vertices.
///
/// Each of these correspond to the method of the same name within OpenGL and other graphics frameworks.
enum mtob_texture_coordinator_mapping_method_t
{
    MTOB_TEXTURE_COORDINATOR_MAPPING_METHOD_UV         = 0x0,
    MTOB_TEXTURE_COORDINATOR_MAPPING_METHOD_CUBE       = 0x1,
    MTOB_TEXTURE_COORDINATOR_MAPPING_METHOD_SPHERE     = 0x2,
    MTOB_TEXTURE_COORDINATOR_MAPPING_METHOD_PROJECTION = 0x3,
    MTOB_TEXTURE_COORDINATOR_MAPPING_METHOD_SHADOW     = 0x4,
};

/// The different methods that a texture mapper can use for wrapping drawn textures.
///
/// Each of these correspond to the method of the same name within OpenGL and other graphics frameworks.
enum mtob_texture_mapper_wrap_mode_t
{
    MTOB_TEXTURE_MAPPER_WRAP_MODE_CLAMP_TO_EDGE = 0x0,
    MTOB_TEXTURE_MAPPER_WRAP_MODE_CLAMP_TO_BORDER = 0x1,
    MTOB_TEXTURE_MAPPER_WRAP_MODE_REPEAT = 0x2,
    MTOB_TEXTURE_MAPPER_WRAP_MODE_MIRRORED_REPEAT = 0x3,
};

/// The different sources that a fragment step can get a base input from.
enum mtob_fragment_source_t
{
    /// The colour component of the vertex.
    MTOB_FRAGMENT_SOURCE_VERTEX             = 0x0,

    /// The primary lighting colour.
    MTOB_FRAGMENT_SOURCE_LIGHTING_PRIMARY   = 0x1,

    /// The secondary lighting colour.
    MTOB_FRAGMENT_SOURCE_LIGHTING_SECONDARY = 0x2,

    /// The first texture within the containing material, sampled from the first texture coordinate of the vertex.
    MTOB_FRAGMENT_SOURCE_TEXTURE0           = 0x3,

    /// The second texture within the containing material, sampled from the second texture coordinate of the vertex.
    MTOB_FRAGMENT_SOURCE_TEXTURE1           = 0x4,

    /// The third texture within the containing material, sampled from the third texture coordinate of the vertex.
    MTOB_FRAGMENT_SOURCE_TEXTURE2           = 0x5,

    /// The fourth texture within the containing material, sampled from the fourth texture coordinate of the vertex.
    MTOB_FRAGMENT_SOURCE_TEXTURE3           = 0x6,

    /// The previous value within the fragment shader's step buffer.
    MTOB_FRAGMENT_SOURCE_BUFFER             = 0xd,

    /// The constant value of the current fragment step.
    MTOB_FRAGMENT_SOURCE_CONSTANT           = 0xe,

    /// The previous fragment step's colour.
    MTOB_FRAGMENT_SOURCE_PREVIOUS           = 0xf,
};

/// The different operations that can be applied to a single input within a fragment step to get the red, green, and blue channels of said step's colour.
///
/// The documentation comment for each case shows how to apply the operation in a GLSL format.
/// The `input` within each comment is the input to perform the operation on.
enum mtob_fragment_rgb_operation_t
{
    /// `input.rgb`
    MTOB_FRAGMENT_RGB_OPERATION_RGB          = 0x0,

    /// `1.0 - input.rgb`
    MTOB_FRAGMENT_RGB_OPERATION_RGB_INVERTED = 0x1,

    /// `input.aaa`
    MTOB_FRAGMENT_RGB_OPERATION_AAA          = 0x2,

    /// `1.0 - input.aaa`
    MTOB_FRAGMENT_RGB_OPERATION_AAA_INVERTED = 0x3,

    /// `input.rrr`
    MTOB_FRAGMENT_RGB_OPERATION_RRR          = 0x4,

    /// `1.0 - input.rrr`
    MTOB_FRAGMENT_RGB_OPERATION_RRR_INVERTED = 0x5,

    /// `input.ggg`
    MTOB_FRAGMENT_RGB_OPERATION_GGG          = 0x8,

    /// `1.0 - input.ggg`
    MTOB_FRAGMENT_RGB_OPERATION_GGG_INVERTED = 0x9,

    /// `input.bbb`
    MTOB_FRAGMENT_RGB_OPERATION_BBB          = 0xc,

    /// `1.0 - input.bbb`
    MTOB_FRAGMENT_RGB_OPERATION_BBB_INVERTED = 0xd,
};

/// The different operations that can be applied to a single input within a fragment step to get the alpha channel of said step's colour.
///
/// The documentation comment for each case shows how to apply the operation in a GLSL format.
/// The `input` within each comment is the input to perform the operation on.
enum mtob_fragment_alpha_operation_t
{
    /// `input.a`
    MTOB_FRAGMENT_ALPHA_OPERATION_A          = 0x0,

    /// `1.0 - input.a`
    MTOB_FRAGMENT_ALPHA_OPERATION_A_INVERTED = 0x1,

    /// `input.r`
    MTOB_FRAGMENT_ALPHA_OPERATION_R          = 0x2,

    /// `1.0 - input.r`
    MTOB_FRAGMENT_ALPHA_OPERATION_R_INVERTED = 0x3,

    /// `input.g`
    MTOB_FRAGMENT_ALPHA_OPERATION_G          = 0x4,

    /// `1.0 - input.g`
    MTOB_FRAGMENT_ALPHA_OPERATION_G_INVERTED = 0x5,

    /// `input.b`
    MTOB_FRAGMENT_ALPHA_OPERATION_B          = 0x6,

    /// `1.0 - input.g`
    MTOB_FRAGMENT_ALPHA_OPERATION_B_INVERTED = 0x7,
};

/// The different operations that can be performed on a fragment step's inputs to get the red, green, and blue channels of said step's colour.
///
/// The documentation comment for each case shows how to apply the combination in a GLSL format.
/// The `a`, `b`, and `c` within each comment refer to the first, second, and third inputs of the step, respectively.
enum mtob_fragment_rgb_combine_t
{
    /// `a.rgb`
    MTOB_FRAGMENT_RGB_COMBINE_0 = 0x0,

    /// `a.rgb * b.rgb`
    MTOB_FRAGMENT_RGB_COMBINE_1 = 0x1,

    /// `a.rgb + b.rgb`
    MTOB_FRAGMENT_RGB_COMBINE_2 = 0x2,

    /// `a.rgb + b.rgb - vec3(0.5)`
    MTOB_FRAGMENT_RGB_COMBINE_3 = 0x3,

    /// `a.rgb * c.rgb + b.rgb * (vec3(1.0) - c.rgb)`
    MTOB_FRAGMENT_RGB_COMBINE_4 = 0x4,

    /// `a.rgb - b.rgb`
    MTOB_FRAGMENT_RGB_COMBINE_5 = 0x5,

    /// `vec3(4 * ((a.r - 0.5) * (a.r - 0.5) + (a.g - 0.5) * (b.g - 0.5) + (a.b - 0.5) * (b.b - 0.5)))`
    MTOB_FRAGMENT_RGB_COMBINE_6 = 0x6,

    /// `vec3(4 * ((a.r - 0.5) * (a.r - 0.5) + (a.g - 0.5) * (b.g - 0.5) + (a.b - 0.5) * (b.b - 0.5)))`
    MTOB_FRAGMENT_RGB_COMBINE_7 = 0x7,

    /// `(a.rgb * b.rgb) + c.rgb`
    MTOB_FRAGMENT_RGB_COMBINE_8 = 0x8,

    /// `clamp(a.rgb + b.rgb, 0.0, 1.0) * c.rgb`
    MTOB_FRAGMENT_RGB_COMBINE_9 = 0x9,
};

/// The different operations that can be performed on a fragment step's inputs to get the alpha channel of said step's colour.
///
/// The documentation comment for each case shows how to apply the combination in a GLSL format.
/// The `a`, `b`, and `c` within each comment refer to the first, second, and third inputs of the step, respectively.
enum mtob_fragment_alpha_combine_t
{
    /// `a.a`
    MTOB_FRAGMENT_ALPHA_COMBINE_0 = 0x0,

    /// `a.a * b.a`
    MTOB_FRAGMENT_ALPHA_COMBINE_1 = 0x1,

    /// `a.a + b.a`
    MTOB_FRAGMENT_ALPHA_COMBINE_2 = 0x2,

    /// `a.a + b.a - 0.5`
    MTOB_FRAGMENT_ALPHA_COMBINE_3 = 0x3,

    /// `a.a * c.a + b.a * (1.0 - c.a)`
    MTOB_FRAGMENT_ALPHA_COMBINE_4 = 0x4,

    /// `a.a - b.a`
    MTOB_FRAGMENT_ALPHA_COMBINE_5 = 0x5,

    /// `4 * ((a.r - 0.5) * (b.r - 0.5) + (a.g - 0.5) * (b.g - 0.5) + (a.b - 0.5) * (b.b - 0.5))`
    MTOB_FRAGMENT_ALPHA_COMBINE_7 = 0x7,

    /// `(a.a * b.a) + c.a`
    MTOB_FRAGMENT_ALPHA_COMBINE_8 = 0x8,

    /// `clamp(a.a + b.a, 0.0, 1.0) * c.a`
    MTOB_FRAGMENT_ALPHA_COMBINE_9 = 0x9,
};

// MARK: - Data Structures

/// The data structure for the colours within an MTOB.
struct mtob_colors_t
{
    struct color4_t emission;
    struct color4_t ambient;
    struct color4_t diffuse;
    struct color4_t specular0, specular1;
    struct color4_t constant0, constant1, constant2, constant3, constant4, constant5;
};

/// The data structure for a texture mapper within an MTOB.
///
/// Texture mappers are used to determine how to load raw bitmap data into OpenGL and other graphics frameworks.
struct mtob_texture_mapper_t
{
    /// The name of the texture that this mapper is using within the containing CGFX of this mapper's MTOB.
    ///
    /// Allocated.
    char *texture_name;

    /// The wrap mode for the U axis of this mapper's texture.
    enum mtob_texture_mapper_wrap_mode_t wrap_u;

    /// The wrap mode for the V axis of this mapper's texture.
    enum mtob_texture_mapper_wrap_mode_t wrap_v;
};

/// The data structure for a texture coordinator within an MTOB.
///
/// Texture coordinators are used to determine how to coordinate texture mappers onto geometry.
struct mtob_texture_coordinator_t
{
    /// The index of the texture coordinates within a vertex that this coordinator uses.
    unsigned int source_index;

    /// The mapping method that this texture coordinator is using.
    enum mtob_texture_coordinator_mapping_method_t mapping_method;

    /// The values to multiply this coordinator's texture's size by.
    struct vec2_t scale;

    /// The value to rotate this coordinator's texture by, in radians.
    float rotation;

    /// The values to translate this coordinator's texture's position by.
    struct vec2_t translate;

    /// The transform to apply to this coordinator's texture.
    struct mat4_t transform;

    /// The mapper for this coordinator's texture.
    struct mtob_texture_mapper_t mapper;
};

/// The data structure for an MTOB file that has been opened.
struct mtob_t
{
    /// The name of this MTOB.
    ///
    /// Allocated.
    char *name;

    /// The colours within this MTOB.
    struct mtob_colors_t colors;

    /// The number of active texture coordinators within this MTOB.
    ///
    /// MTOBs always contain three texture coordinators, but a variable amount of
    /// them are actually active. This value determines how many are.
    unsigned int num_active_texture_coordinators;

    /// All the texture coordinators within this MTOB.
    struct mtob_texture_coordinator_t texture_coordinators[3];
};

// MARK: - Functions

/// Open the MTOB file at the current offset of the given file handle into the given MTOB.
/// @param file The file handle to open the MTOB file from.
/// @param mtob The MTOB to open the file into.
void mtob_open(FILE *file, struct mtob_t *mtob);

/// Close the given MTOB, releasing all of it's allocated memory.
///
/// This must be called after an MTOB is opened and before program execution completes.
/// @param mtob The MTOB to close.
void mtob_close(struct mtob_t *mtob);
