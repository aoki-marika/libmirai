//
//  aet_viewer.h
//  viewer
//
//  Created by Marika on 2020-02-25.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <mirai/aet.h>
#include <mirai/spr.h>
#include <mirai/matrix.h>

#include "program.h"
#include "vertex_array.h"

// MARK: - Data Structures

/// The data structure for managing the state of an AET viewer across usages.
struct aet_viewer_t
{
    /// The AET that this viewer is viewing.
    const struct aet_t *aet;

    /// The SPR that contains the SCRs used by this viewer's AET.
    const struct spr_t *spr;

    /// The program that this viewer is using to render it's 2D elements.
    const struct program_t *program2d;

    /// The first texture unit that SPR textures are uploaded to within this viewer.
    GLenum textures_base_unit;

    /// The unique identifiers of all the uploaded textures from this viewer's SPR.
    ///
    /// Allocated. Always of length `spr->num_textures`.
    GLuint *texture_ids;
};

/// A data structure for wrapping around AET nodes and provide viewer-specific functionality.
struct aet_viewer_node_t
{
    /// The backing AET node of this node, if any.
    ///
    /// If this is a root node then this is `NULL`.
    /// Else this is a pointer within the containing viewer's AET.
    const struct aet_node_t *backing;

    /// The cached world-space model transform for this node's vertices.
    struct mat4_t transform_world;

    /// The cached absolute opacity of this node's contents.
    float opacity;

    /// The total number of quads within this node's sprite quads vertex array.
    unsigned int num_sprite_quads;

    /// The vertex array containing the quads used to draw this node's sprites.
    ///
    /// If `backing->contents_type` is `AET_NODE_CONTENTS_TYPE_SPRITE_GROUP`,
    /// then this is allocated.
    /// Else this is `NULL`.
    ///
    /// Each quad is of the size of it's sprite, and placed at `0,0`.
    struct vertex_array_t *sprite_quads_array;

    /// The index of the texture within the SPR that this node's sprites are in.
    unsigned int sprites_texture_index;

    /// The total number of child nodes within this node.
    unsigned int num_children;

    /// All the child nodes of this node.
    ///
    /// If `backing->contents_type` is `AET_NODE_CONTENTS_TYPE_CHILDREN`,
    /// or this is a root node, then this is allocated.
    /// Else this is `NULL`.
    struct aet_viewer_node_t *children;
};

// MARK: - Functions

/// Create a new AET viewer from the given properties.
/// @param aet The AET that the new viewer will be viewing.
/// It is expected that this AET remains available throughout the lifetime of the viewer.
/// @param spr The SPR containing the SCRs used by the given AET.
/// It is expected that this SPR remains available throughout the lifetime of the viewer.
/// @param program2d The program used to render this viewer's 2D elements, assumed to be the standard.
/// It is expected that this program remains available throughout the lifetime of the viewer.
/// @param viewer The viewer to load the created viewer into.
void aet_viewer_create(const struct aet_t *aet,
                       const struct spr_t *spr,
                       const struct program_t *program2d,
                       struct aet_viewer_t *viewer);

/// Destroy the given AET viewer, releasing all of it's allocated memory.
///
/// This must be called after a viewer is created and before program execution completes.
/// @param viewer The viewer to destroy.
void aet_viewer_destroy(struct aet_viewer_t *viewer);

/// Run the given AET viewer within the given window.
/// @param window The window to display the viewer within.
/// @param viewer The viewer to run.
void aet_viewer_run(GLFWwindow *window, struct aet_viewer_t *viewer);
