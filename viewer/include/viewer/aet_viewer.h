//
//  aet_viewer.h
//  viewer
//
//  Created by Marika on 2020-02-25.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <mirai/aet.h>

#include "program.h"

// MARK: - Data Structures

/// The data structure for managing the state of an AET viewer across usages.
struct aet_viewer_t
{
    /// The AET that this viewer is viewing.
    const struct aet_t *aet;

    /// The program that this viewer is using to render it's 2D elements.
    const struct program_t *program2d;
};

// MARK: - Functions

/// Create a new AET viewer from the given properties.
/// @param aet The AET that the new viewer will be viewing.
/// It is expected that this AET remains available throughout the lifetime of the viewer.
/// @param program2d The program used to render this viewer's 2D elements, assumed to be the standard.
/// It is expected that this program remains available throughout the lifetime of the viewer.
/// @param viewer The viewer to load the created viewer into.
void aet_viewer_create(const struct aet_t *aet,
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
