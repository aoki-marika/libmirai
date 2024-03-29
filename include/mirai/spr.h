//
//  spr.h
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

#include "ctr_texture.h"

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
    struct ctr_texture_t *textures;

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

    /// The index of the texture that this SCR is within, within this SCR's containing SPR.
    unsigned int texture_index;

    /// The U of the top left UV coordinate of this SCR's bounds.
    ///
    /// Despite being a UV coordinate, this uses a top-left origin.
    float start_u;

    /// The V of the top left UV coordinate of this SCR's bounds.
    ///
    /// Despite being a UV coordinate, this uses a top-left origin.
    float start_v;

    /// The U of the bottom right UV coordinate of this SCR's bounds.
    ///
    /// Despite being a UV coordinate, this uses a top-left origin.
    float end_u;

    /// The V of the bottom right UV coordinate of this SCR's bounds.
    ///
    /// Despite being a UV coordinate, this uses a top-left origin.
    float end_v;

    /// The top left U coordinate of this SCR's bounds, in pixels.
    ///
    /// Top-left origin.
    uint16_t x;

    /// The top left V coordinate of this SCR's bounds, in pixels.
    ///
    /// Top-left origin.
    uint16_t y;

    /// The width of this SCR's bounds, in pixels.
    uint16_t width;

    /// The height of this SCR's bounds, in pixels.
    uint16_t height;
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

/// Attempt to get the SCR from the given SPR of the given name.
/// @param spr The SPR to lookup the SCR within.
/// @param scr_name The name of the SCR to lookup within the given SPR.
/// @returns The SCR of the given name from the given SCR, if any.
/// Note that this SCR is a pointer within the given SPR, so the SPR must be within scope where it is used.
/// If there is no SCR of the given name within the given SPR, then this is `NULL`.
struct scr_t *spr_lookup(const struct spr_t *spr, char *scr_name);
