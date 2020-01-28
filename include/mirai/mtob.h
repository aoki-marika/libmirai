//
//  mtob.h
//  libmirai
//
//  Created by Marika on 2020-01-19.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <stdbool.h>

#include "color.h"
#include "vector.h"
#include "matrix.h"

// MARK: - Enumerations

#warning TODO: mtob_blend_function_t -> mtob_blend_factor_t

/// The different cull modes that an MTOB can use for the geometry it is applied to.
enum mtob_cull_mode_t
{
    /// Never cull any faces.
    MTOB_CULL_MODE_NEVER = 0x0,

    /// Only cull the front face.
    MTOB_CULL_MODE_FRONT_FACE = 0x1,

    /// Only cull the back face.
    MTOB_CULL_MODE_BACK_FACE = 0x2,
};

/// The different blend functions that an MTOB can use.
///
/// Each of these correspond to the OpenGL blend function of the same name.
enum mtob_blend_function_t
{
    MTOB_BLEND_FUNCTION_ZERO                     = 0x0,
    MTOB_BLEND_FUNCTION_ONE                      = 0x1,
    MTOB_BLEND_FUNCTION_SRC_COLOR                = 0x2,
    MTOB_BLEND_FUNCTION_ONE_MINUS_SRC_COLOR      = 0x3,
    MTOB_BLEND_FUNCTION_DST_COLOR                = 0x4,
    MTOB_BLEND_FUNCTION_ONE_MINUS_DST_COLOR      = 0x5,
    MTOB_BLEND_FUNCTION_SRC_ALPHA                = 0x6,
    MTOB_BLEND_FUNCTION_ONE_MINUS_SRC_ALPHA      = 0x7,
    MTOB_BLEND_FUNCTION_DST_ALPHA                = 0x8,
    MTOB_BLEND_FUNCTION_ONE_MINUS_DST_ALPHA      = 0x9,
    MTOB_BLEND_FUNCTION_CONSTANT_COLOR           = 0xa,
    MTOB_BLEND_FUNCTION_ONE_MINUS_CONSTANT_COLOR = 0xb,
    MTOB_BLEND_FUNCTION_CONSTANT_ALPHA           = 0xc,
    MTOB_BLEND_FUNCTION_ONE_MINUS_CONSTANT_ALPHA = 0xd,
    MTOB_BLEND_FUNCTION_SRC_ALPHA_SATURATE       = 0xe,
};

/// The different blend equations that an MTOB can use.
///
/// Each of these correspond to the OpenGL blend equation of the same name.
enum mtob_blend_equation_t
{
    MTOB_BLEND_EQUATION_ADD              = 0x0,
    MTOB_BLEND_EQUATION_SUBTRACT         = 0x1,
    MTOB_BLEND_EQUATION_REVERSE_SUBTRACT = 0x2,
    MTOB_BLEND_EQUATION_MIN              = 0x3,
    MTOB_BLEND_EQUATION_MAX              = 0x4,
};

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
    MTOB_TEXTURE_MAPPER_WRAP_MODE_CLAMP_TO_EDGE   = 0x0,
    MTOB_TEXTURE_MAPPER_WRAP_MODE_CLAMP_TO_BORDER = 0x1,
    MTOB_TEXTURE_MAPPER_WRAP_MODE_REPEAT          = 0x2,
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

/// The different sources for a fragment step's constant.
///
/// Each of these map to the property of the same name within the `mtob_colors_t` of the MTOB that contains this fragment step.
enum mtob_fragment_step_constant_t
{
    MTOB_FRAGMENT_STEP_CONSTANT_CONSTANT0 = 0x0,
    MTOB_FRAGMENT_STEP_CONSTANT_CONSTANT1 = 0x1,
    MTOB_FRAGMENT_STEP_CONSTANT_CONSTANT2 = 0x2,
    MTOB_FRAGMENT_STEP_CONSTANT_CONSTANT3 = 0x3,
    MTOB_FRAGMENT_STEP_CONSTANT_CONSTANT4 = 0x4,
    MTOB_FRAGMENT_STEP_CONSTANT_CONSTANT5 = 0x5,
    MTOB_FRAGMENT_STEP_CONSTANT_EMISSION  = 0x6,
    MTOB_FRAGMENT_STEP_CONSTANT_AMBIENT   = 0x7,
    MTOB_FRAGMENT_STEP_CONSTANT_DIFFUSE   = 0x8,
    MTOB_FRAGMENT_STEP_CONSTANT_SPECULAR0 = 0x9,
    MTOB_FRAGMENT_STEP_CONSTANT_SPECULAR1 = 0xa,
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

/// The data structure for describing the operations to perform to get the colour of a single input within a fragment shader step.
struct mtob_fragment_step_input_t
{
    /// The source of the red, green, and alpha channels of this input's colour.
    enum mtob_fragment_source_t rgb_source;

    /// The operation to perform on the red, green, and blue channels of this input's colour.
    enum mtob_fragment_rgb_operation_t rgb_operation;

    /// The source of the alpha channel of this input's colour.
    enum mtob_fragment_source_t alpha_source;

    /// The operation to perform on the alpha channel of this input's colour.
    enum mtob_fragment_alpha_operation_t alpha_operation;
};

/// The data structure for describing the operations to perform within a single step of a fragment shader.
struct mtob_fragment_step_t
{
    /// The constant of this step.
    ///
    /// The value that this refers to is used when the an input inside
    /// this step queries the `MTOB_FRAGMENT_SOURCE_CONSTANT` source.
    enum mtob_fragment_step_constant_t constant;

    /// All the inputs to retrieve within this step.
    ///
    /// Each of these inputs are combined once retrieved using the operations
    /// described by the `rgb_combine` and `alpha_combine` properties to get the
    /// colour of this step.
    struct mtob_fragment_step_input_t inputs[3];

    /// The operation used to combine all the inputs within this step to get
    /// the red, green, and blue channels of this step's colour.
    enum mtob_fragment_rgb_combine_t rgb_combine;

    /// The operation used to combine all the inputs within this step to get
    /// the alpha channel of this step's colour.
    enum mtob_fragment_alpha_combine_t alpha_combine;

    /// The value to multiply this step's red, green, and blue channels by.
    unsigned int rgb_scale;

    /// The value to multiply this step's alpha channel by.
    unsigned int alpha_scale;
};

/// The data structure for describing the fragment shader of an MTOB.
struct mtob_fragment_shader_t
{
    /// The colour to use for buffer input to the first step within this fragment shader.
    struct color4_t base_color;

    /// All the steps within this fragment shader.
    ///
    /// Each of these steps perform a combination operation on multiple inputs,
    /// and then this combination is stored inside a variable which is overwritten for each step.
    /// The fragment colour is decided by the last assigned value to said variable.
    struct mtob_fragment_step_t steps[6];
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

    /// The cull mode that this MTOB uses for geometry that is applied to.
    enum mtob_cull_mode_t cull_mode;

    /// Whether or not this MTOB uses blending.
    bool blend_enabled;

    /// The blend colour of this MTOB.
    ///
    /// This can be applied in OpenGL using `glBlendColor`.
    ///
    /// Only set if blending is enabled.
    struct color4_t blend_color;

    /// Specifies how the red, green, and blue channels of this MTOB's blending factors are computed.
    ///
    /// If using OpenGL, then the OpenGL representation of this value is the first argument to `glBlendFuncSeparate`.
    enum mtob_blend_function_t blend_function_source_rgb;

    /// Specifies how the destination red, green, and blue channels of this MTOB's blending factors are computed.
    ///
    /// If using OpenGL, then the OpenGL representation of this value is the second argument to `glBlendFuncSeparate`.
    enum mtob_blend_function_t blend_function_destination_rgb;

    /// Specifies how the alpha channel of this MTOB's blending factors are computed.
    ///
    /// If using OpenGL, then the OpenGL representation of this value is the third argument to `glBlendFuncSeparate`.
    enum mtob_blend_function_t blend_function_source_alpha;

    /// Specifies how the destination alpha channel of this MTOB's blending factors are computed.
    ///
    /// If using OpenGL, then the OpenGL representation of this value is the fourth argument to `glBlendFuncSeparate`.
    enum mtob_blend_function_t blend_function_destination_alpha;

    /// Specifies how the red, green, and blue channels of this MTOB's source and destination colours are combined.
    ///
    /// If using OpenGL, then the OpenGL representation of this value is the first argument to `glBlendEquationSeparate`.
    enum mtob_blend_equation_t blend_equation_rgb;

    /// Specifies how the alpha channels of this MTOB's source and destination colours are combined.
    ///
    /// If using OpenGL, then the OpenGL representation of this value is the second argument to `glBlendEquationSeparate`.
    enum mtob_blend_equation_t blend_equation_alpha;

    /// The number of active texture coordinators within this MTOB.
    ///
    /// MTOBs always contain three texture coordinators, but a variable amount of
    /// them are actually active. This value determines how many are.
    unsigned int num_active_texture_coordinators;

    /// All the texture coordinators within this MTOB.
    struct mtob_texture_coordinator_t texture_coordinators[3];

    /// The fragment shader of this MTOB.
    struct mtob_fragment_shader_t fragment_shader;
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
