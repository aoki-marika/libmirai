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

// MARK: - Functions

/// Run an SPR viewer for the given SPR within the given window.
/// @param window The window to display the viewer within.
/// @param spr The SPR to view.
void spr_viewer_run(GLFWwindow *window, struct spr_t *spr);
