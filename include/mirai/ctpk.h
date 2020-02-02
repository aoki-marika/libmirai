//
//  ctpk.h
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

#include "texture.h"

// MARK: - Data Structures

/// The data structure for a CTPK file that has been opened.
struct ctpk_t
{
    /// The total number of textures within this CTPK.
    unsigned int num_textures;

    /// All the textures within this CTPK.
    ///
    /// Allocated.
    struct texture_t *textures;
};

// MARK: - Functions

/// Open the CTPK file at the current offset of the given file handle into the given CTPK.
/// @param file The file handle to read the CTPK from.
/// @param ctpk The CTPK to open the file into.
void ctpk_open(FILE *file, struct ctpk_t *ctpk);

/// Close the given CTPK, releasing all of it's allocated memory.
///
/// This must be called after a CTPK is opened and before program execution completes.
/// @param ctpk The CTPK to close.
void ctpk_close(struct ctpk_t *ctpk);
