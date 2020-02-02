//
//  spr.h
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

#include "texture.h"
#include "vector.h"

// MARK: - Data Structures

/// The data structure for an SPR file that has been opened.
struct spr_t
{
    /// The file handle for the file that this SPR is reading data from.
    ///
    /// Kept open until `spr_close(spr)` is called with this SPR.
    FILE *file;

    /// The total number of textures within this SPR.
    unsigned int num_textures;

    /// All the textures within this SPR.
    ///
    /// Allocated.
    struct texture_t *textures;

    /// The names of all the textures within this SPR.
    ///
    /// The array and each item are allocated.
    char **texture_names;

    /// The total number of SCRs within this SPR.
    unsigned int num_scrs;

    /// All the SCRs within this SPR.
    ///
    /// Allocated.
    struct scr_t *scrs;
};

/// The data structure for a single SCR within an SPR.
///
/// SCRs are used to define named subsections of CTPK textures, like an atlas.
struct scr_t
{
    /// The name of this SCR.
    ///
    /// Allocated.
    char *name;

    /// The index of the CTPK that this SCR is within, within this SCR's containing SPR.
    unsigned int ctpk_index;

    /// The top left coordinate of this SCR's bounds.
    ///
    /// Normalized with top-left origin.
    struct vec2_t top_left;

    /// The bottom right coordinate of this SCR's bounds.
    ///
    /// Normalized with top-left origin.
    struct vec2_t bottom_right;
};

// MARK: - Functions

/// Open the SPR file at the given path into the given SPR.
/// @param path The path of the SPR file to open.
/// @param spr The SPR to open the file into.
void spr_open(const char *path, struct spr_t *spr);

/// Close the given SPR, releasing all of it's allocated memory.
///
/// This must be called after an SPR is opened and before program execution completes.
/// @param spr The SPR to close.
void spr_close(struct spr_t *spr);
