//
//  spr.h
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include "ctpk.h"

// MARK: - Data Structures

/// The data structure for a sprite AET file that has been opened.
struct spr_t
{
    /// The handle to the underlying filesystem file of this file.
    FILE *handle;

    /// The number of underlying CTPK files within this file.
    unsigned int num_ctpks;

    /// The name of each CTPK file within the `ctpks` array.
    char **ctpk_names;

    /// The underlying CTPK files within this file.
    struct ctpk_t **ctpks;
};

// MARK: - Functions

/// Open the sprite AET file at the given path into the given `spr_t` data structure.
/// @param path The path of the sprite AET file to open.
/// @param output The `spr_t` to open the file into.
void spr_open(const char *path, struct spr_t *output);

/// Close the given `spr_t`, releasing all of it's memory.
/// This must be called after an `spr_t` is opened and before program execution completes.
/// @param file The `spr_t` to close.
void spr_close(struct spr_t *file);
