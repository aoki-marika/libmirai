//
//  spr_viewer.h
//  viewer
//
//  Created by Marika on 2020-02-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <mirai/spr.h>

#include "gl.h"
#include "program.h"
#include "vertex_array.h"

// MARK: - Enumerations

/// The different screens that an SPR viewer can be displaying.
enum spr_viewer_screen_t
{
    /// The screen for displaying all the textures within the SPR.
    SPR_VIEWER_SCREEN_TEXTURES = 0x0,

    /// The screen for displaying all the SCRs within the SPR.
    SPR_VIEWER_SCREEN_SCRS     = 0x1,
};

// MARK: - Data Structures

/// The data structure for managing the state of an SPR viewer across usages.
struct spr_viewer_t
{
    /// The SPR that this viewer is viewing.
    const struct spr_t *spr;

    /// The program that this viewer is using to render it's 2D elements.
    const struct program_t *program2d;

    /// The base texture unit that this viewer's textures are uploaded to.
    ///
    /// The first texture is uploaded to this unit, then each subsequent texture is uploaded to the next.
    GLenum textures_base_unit;

    /// The unique identifiers of all the uploaded textures from this viewer's SPR.
    ///
    /// Allocated. Always of length `spr->num_textures`.
    GLuint *texture_ids;

    /// The vertex array for the quads used to display this viewer's textures.
    struct vertex_array_t texture_quads_array;

    /// The vertex array for the quads used to display this viewer's SCRs.
    struct vertex_array_t scr_quads_array;

    /// The current screen that this viewer is displaying.
    enum spr_viewer_screen_t screen;

    /// The index of the current texture that this viewer is displaying in the textures screen.
    unsigned int texture_index;

    /// The index of the current SCR that this viewer is displaying in the SCRs screen.
    unsigned int scr_index;
};

// MARK: - Functions

/// Create a new SPR viewer from the given properties.
/// @param spr The SPR that the new viewer will be viewing.
/// It is expected that this SPR remains available throughout the lifetime of the viewer.
/// @param program2d The program used to render this viewer's 2D elements, assumed to be the standard.
/// It is expected that this program remains available throughout the lifetime of the viewer.
/// @param viewer The viewer to load the created viewer into.
void spr_viewer_create(const struct spr_t *spr,
                       const struct program_t *program2d,
                       struct spr_viewer_t *viewer);

/// Destroy the given SPR viewer, releasing all of it's allocated memory.
///
/// This must be called after a viewer is created and before program execution completes.
/// @param viewer The viewer to destroy.
void spr_viewer_destroy(struct spr_viewer_t *viewer);

/// Run the given SPR viewer within the given window.
/// @param window The window to display the viewer within.
/// @param viewer The viewer to run.
void spr_viewer_run(GLFWwindow *window, struct spr_viewer_t *viewer);
