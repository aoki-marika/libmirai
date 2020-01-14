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

/// The data structure for an spr file that has been opened.
///
/// If `spr_open(path, spr)` is called on an spr then `spr_close(spr)` must also be called on it before program execution completes.
struct spr_t
{
    /// The handle to the file that this spr is reading data from.
    ///
    /// Kept open until `spr_close(file)` is called with this spr.
    FILE *handle;

    /// The number of CTPKs within this spr.
    unsigned int num_ctpks;

    /// The name of each CTPK within the `ctpks` array.
    ///
    /// The array and each name are allocated.
    char **ctpk_names;

    /// All of the CTPKs within this spr.
    ///
    /// The array and each CTPK are allocated.
    struct ctpk_t **ctpks;
};

// MARK: - Functions

/// Open the spr file at the given path into the given spr.
/// @param path The path of the spr file to open.
/// @param spr The spr to open the file into.
void spr_open(const char *path, struct spr_t *spr);

/// Close the given spr, releasing all of it's allocated memory.
///
/// This must be called after an spr is opened and before program execution completes.
/// @param spr The spr to close.
void spr_close(struct spr_t *spr);
