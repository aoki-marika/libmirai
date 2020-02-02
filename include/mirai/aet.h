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

/// The different types of data that a node can contain.
enum aet_node_contents_type_t
{
    /// This node contains a sprite group that it draws.
    AET_NODE_CONTENTS_TYPE_SPRITE_GROUP = 0x1,

    /// This node contains child nodes that use this node's properties as a base.
    AET_NODE_CONTENTS_TYPE_CHILDREN     = 0x3,
};

// MARK: - Data Structures

/// The data structure for an AET file that has been opened.
struct aet_t
{
    /// The file handle for the file that this AET is reading data from.
    ///
    /// Kept open until `aet_close(aet)` is called with this AET.
    FILE *file;

    /// The total number of scenes within this AET.
    unsigned int num_scenes;

    /// All the scenes within this AET.
    ///
    /// Allocated.
    struct aet_scene_t *scenes;
};

/// The data structure for a single scene within an AET.
struct aet_scene_t
{
    /// The name of this scene.
    ///
    /// Allocated.
    char *name;

    /// The total number of used SCR names within this scene.
    unsigned int num_scr_names;

    /// The names of all the SCRs that this scene uses.
    ///
    /// The array and each item are allocated.
    char **scr_names;

    /// The width of this scene, in pixels.
    unsigned int width;

    /// The height of this scene, in pixels.
    unsigned int height;

    /// The total number of node groups within this scene.
    unsigned int num_node_groups;

    /// All the node groups within this scene.
    ///
    /// Allocated.
    struct aet_node_group_t *node_groups;
};

/// The data structure for a group of nodes within an AET.
struct aet_node_group_t
{
    /// The total number of nodes within this group.
    unsigned int num_nodes;

    /// All the nodes within this group.
    ///
    /// Allocated.
    struct aet_node_t *nodes;
};

/// The data structure for a single node within a scene's graph in an AET.
struct aet_node_t
{
    /// The name of this node.
    ///
    /// Allocated.
    char *name;

    /// The origin of this node's position.
    struct vec2_t origin;

    /// The position of this node.
    struct vec2_t position;

    /// The rotation of this node.
    float rotation;

    /// The scale of this node.
    struct vec2_t scale;

    /// The opacity of this node.
    float opacity;

    /// The type of this node's contents.
    enum aet_node_contents_type_t contents_type;

    /// The sprite group that this node is drawing, if any.
    ///
    /// If `contents_type` is `AET_NODE_CONTENTS_TYPE_SPRITE_GROUP`, then this is allocated.
    /// If not then this is `NULL`.
    struct aet_sprite_group_t *sprite_group;

    /// The total number of child nodes within this node.
    ///
    /// If `contents_type` is `AET_NODE_CONTENTS_TYPE_SPRITE_GROUP` then this is set.
    /// If not then this is `0`.
    unsigned int num_children;

    /// All the child nodes of this node.
    ///
    /// If `contents_type` is `AET_NODE_CONTENTS_TYPE_CHILDREN`, then this is allocated.
    /// If not then this is `NULL`.
    struct aet_node_t *children;
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
    /// The index of the SCR that this sprite uses, within it's containing scene.
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
