//
//  aet.h
//  libmirai
//
//  Created by Marika on 2020-01-28.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

#include "color.h"
#include "vector.h"

// MARK: - Enumerations

/// The different commonly used types of markers.
enum aet_marker_type_t
{
    /// Unknown.
    AET_MARKER_TYPE_UNKNOWN     = 0x0,

    /// The beginning of the in transition.
    ///
    /// `ST_IN`.
    AET_MARKER_TYPE_IN_START    = 0x1,

    /// The end of the in transition.
    ///
    /// `ED_IN`.
    AET_MARKER_TYPE_IN_END      = 0x2,

    /// The beginning of the loop animation.
    ///
    /// `ST_LP`.
    AET_MARKER_TYPE_LOOP_START  = 0x3,

    /// The end of the loop animation.
    ///
    /// `ED_LP`.
    AET_MARKER_TYPE_LOOP_END    = 0x4,

    /// The beginning of the out transition.
    ///
    /// `ST_OUT`.
    AET_MARKER_TYPE_OUT_START   = 0x5,

    /// The end of the out transition.
    ///
    /// `ED_OUT`.
    AET_MARKER_TYPE_OUT_END     = 0x6,

    /// The beginning of the press animation.
    ///
    /// `ST_SP`.
    AET_MARKER_TYPE_PRESS_START = 0x7,

    /// The end of the press animation.
    ///
    /// `ED_SP`.
    AET_MARKER_TYPE_PRESS_END   = 0x8,
};

/// The different blend modes that a layer can use to blend it's source's colours with colours behind it.
enum aet_layer_blend_mode_t
{
    /// Normal alpha blending.
    AET_LAYER_BLEND_MODE_NORMAL = 0x3,

    /// Additive blending.
    AET_LAYER_BLEND_MODE_ADD    = 0x5,

    /// Unknown blending.
    AET_LAYER_BLEND_MODE_UNK1   = 0x6,

    /// Unknown blending.
    AET_LAYER_BLEND_MODE_UNK2   = 0xb,
};

/// The different types that a temporal properties keyframes can be.
enum aet_layer_keyframes_type_t
{
    /// There is a single keyframe which forms a static value, and has no frame number.
    AET_LAYER_KEYFRAMES_TYPE_SINGLE   = 0x0,

    /// There is a series of keyframes which form an animation.
    AET_LAYER_KEYFRAMES_TYPE_MULTIPLE = 0x1,
};

/// The different types that a layer can be.
enum aet_layer_type_t
{
    /// This layer sources a sprite group.
    AET_LAYER_TYPE_SOURCE_SPRITE_GROUP = 0x1,

    /// This layer does not have a source, but has children which use this layer's properties as a base.
    AET_LAYER_TYPE_NULL_OBJECT         = 0x3,
};

// MARK: - Data Structures

/// The data structure for an AET file that has been opened.
struct aet_t
{
    /// The file handle for the file that this AET is reading data from.
    ///
    /// Kept open until `aet_close(aet)` is called with this AET.
    FILE *file;

    /// The total number of compositions within this AET.
    unsigned int num_compositions;

    /// All the compositions within this AET.
    ///
    /// Allocated.
    struct aet_composition_t *compositions;
};

/// The data structure for a single composition within an AET.
///
/// Compositions are used to create self-contained independent timelines and layers.
/// Generally multiple compositions are used within an AET to separate the two screens.
struct aet_composition_t
{
    /// The name of this compositions.
    ///
    /// Allocated.
    char *name;

    /// The total number of used SCR names within this composition.
    unsigned int num_scr_names;

    /// The names of all the SCRs that this composition's sprites use.
    ///
    /// The array and each item are allocated.
    char **scr_names;

    /// The absolute frame number at which this compositions's timeline begins.
    float timeline_start_frame;

    /// The absolute frame number at which this composition's timeline ends.
    float timeline_end_frame;

    /// The frame rate (frames per second) at which this composition's timeline is defined in.
    ///
    /// All layers within this composition take on this frame rate, and do not define their own.
    float timeline_frame_rate;

    /// The width of this composition, in pixels.
    unsigned int width;

    /// The height of this composition, in pixels.
    unsigned int height;

    /// The total number of layers within this composition.
    unsigned int num_layers;

    /// All the layers within this composition.
    ///
    /// Allocated.
    struct aet_layer_t *layers;

    /// The total number of sprite groups within this composition.
    unsigned int num_sprite_groups;

    /// All the sprite groups within this composition.
    ///
    /// This array should not need to be used directly,
    /// instead using the sprite group pointers within this composition's layers.
    /// Allocated.
    struct aet_sprite_group_t *sprite_groups;
};

/// The data structure for the set of keyframes of a single temporal property within a layer.
struct aet_layer_keyframes_t
{
    /// The type of this keyframe set.
    enum aet_layer_keyframes_type_t type;

    /// The total number of keyframes within this set.
    unsigned int num_keyframes;

    /// All the keyframe values within this set.
    ///
    /// Allocated.
    float *values;

    /// All the keyframe frame numbers within this set.
    ///
    /// If `type` is `AET_LAYER_KEYFRAMES_TYPE_SINGLE`, then this is `NULL`.
    /// If `type` is `AET_LAYER_KEYFRAMES_TYPE_MULTIPLE`, then this is allocated.
    float *frames;
};

/// The data structure for a single layer within a composition.
struct aet_layer_t
{
    /// The name of this layer.
    ///
    /// Allocated.
    char *name;

    /// The unique number identifying this layer within the containing composition.
    unsigned int number;

    /// The frame number at which this layer's timeline begins.
    ///
    /// Relative to the parent layer's timeline.
    /// If this layer has no parent, then this is relative to the containing composition's timeline.
    float timeline_start_frame;

    /// The frame number at which this layer's timeline ends.
    ///
    /// Relative to the parent layer's timeline.
    /// If this layer has no parent, then this is relative to the containing composition's timeline.
    float timeline_end_frame;

    /// The speed, in percentage, at which this layer's timeline plays at.
    float timeline_speed;

    /// The parent layer of this layer, if any.
    ///
    /// If this is not `NULL` then it points to a layer within the containing
    /// composition's layers array.
    const struct aet_layer_t *parent;

    /// The total number of markers within this layer.
    unsigned int num_markers;

    /// All the markers within this layer.
    ///
    /// Allocated.
    struct aet_marker_t *markers;

    /// The blend mode of this layer's source.
    enum aet_layer_blend_mode_t blend_mode;

    /// The keyframes for the X axis of this layer's transform anchor point, relative to scale.
    struct aet_layer_keyframes_t anchor_point_x;

    /// The keyframes for the Y axis of this layer's transform anchor point, relative to scale.
    struct aet_layer_keyframes_t anchor_point_y;

    /// The keyframes for the X axis of this layer's position, in pixels.
    struct aet_layer_keyframes_t position_x;

    /// The keyframes for the Y axis of this layer's position, in pixels.
    struct aet_layer_keyframes_t position_y;

    /// The keyframes for the rotation of this layer, in clockwise degrees.
    struct aet_layer_keyframes_t rotation;

    /// The keyframes for the X axis of this layer's scale, in percentage.
    struct aet_layer_keyframes_t scale_x;

    /// The keyframes for the Y axis of this layer's scale, in percentage.
    struct aet_layer_keyframes_t scale_y;

    /// The keyframes for the opacity of this layer.
    struct aet_layer_keyframes_t opacity;

    /// The type of this layer.
    enum aet_layer_type_t type;

    /// The sprite group that this layer is sourcing, if any.
    ///
    /// If `contents_type` is `AET_LAYER_TYPE_SOURCE_SPRITE_GROUP` then this
    /// points to an item within the containing composition's sprite groups array.
    /// If not then this is `NULL`.
    const struct aet_sprite_group_t *sprite_group;

    /// The total number of child layers within this layer.
    unsigned int num_children;

    /// All the child layers of this layer, if any.
    ///
    /// If `contents_type` is `AET_LAYER_TYPE_NULL_OBJECT` then each
    /// item points to an item within the containing composition's layers array,
    /// and the array is allocated.
    /// If not then this is `NULL`.
    const struct aet_layer_t **children;
};

/// The data structure for marking an animation point or event within a layer's timeline.
struct aet_marker_t
{
    /// The frame number, within the containing layer's timeline, at which this marker occurs.
    float frame;

    /// The name of this marker.
    ///
    /// Allocated.
    char *name;

    /// The known type of this marker.
    ///
    /// This is read by comparing the name of this marker against the known names of various types.
    enum aet_marker_type_t type;
};

/// The data structure for a group of sprites within an AET.
struct aet_sprite_group_t
{
    /// The multiply colour to apply to all of this group's sprites.
    struct color4_t multiply_color;

    /// The width of this group, in pixels.
    ///
    /// Although this should always match the width of this group's sprites,
    /// if one does not then it should be stretched to this width.
    unsigned int width;

    /// The height of this group, in pixels.
    ///
    /// Although this should always match the height of this group's sprites,
    /// if one does not then it should be stretched to this height.
    unsigned int height;

    /// The total number of sprites within this group.
    unsigned int num_sprites;

    /// All the sprites within this group
    ///
    /// Allocated.
    struct aet_sprite_t *sprites;
};

/// The data structure for a single sprite within an AET.
struct aet_sprite_t
{
    /// The index of the SCR that this sprite uses, within it's containing composition.
    unsigned int scr_index;
};

// MARK: - Functions

/// Open the AET file at the given path into the given AET.
/// @param path The path of the AET file to open.
/// @param aet The AET to open the file into.
void aet_open(const char *path, struct aet_t *aet);

/// Close the given AET, releasing all of it's allocated memory.
///
/// This must be called after an AET is opened and before program execution completes.
/// @param aet The AET to close.
void aet_close(struct aet_t *aet);

/// Get the given frame number in milliseconds.
/// @param frame The frame number to convert.
/// @param framerate The frame rate of the given frame number.
/// @param speed The normalized playback speed of the given frame.
/// @returns The millisecond of the given frame number accounting for the given properties.
double aet_frame_to_ms(float frame, float framerate, float speed);
