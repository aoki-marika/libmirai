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

// MARK: - Functions

/// Run an SPR viewer for the given SPR within the given window.
/// @param window The window to display the viewer within.
/// @param spr The SPR to view.
/// @param program2d The program used to render this viewer's 2D elements, assumed to be the standard.
void spr_viewer_run(GLFWwindow *window,
                    struct spr_t *spr,
                    const struct program_t *program2d);
