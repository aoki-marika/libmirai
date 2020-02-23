//
//  txob.h
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

#include "ctr_texture.h"

// MARK: - Data Structures

/// The data structure for a TXOB file that has been opened.
struct txob_t
{
    /// The name of this TXOB.
    ///
    /// Allocated.
    char *name;

    /// The texture of this TXOB.
    struct ctr_texture_t texture;
};

// MARK: - Functions

/// Open the TXOB file at the current offset of the given file handle into the given TXOB.
/// @param file The file handle to open the TXOB file from.
/// @param txob The TXOB to open the file into.
void txob_open(FILE *file, struct txob_t *txob);

/// Close the given TXOB, releasing all of it's allocated memory.
///
/// This must be called after a TXOB is opened and before program execution completes.
/// @param txob The TXOB to close.
void txob_close(struct txob_t *txob);
