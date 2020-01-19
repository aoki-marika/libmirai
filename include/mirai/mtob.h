//
//  mtob.h
//  libmirai
//
//  Created by Marika on 2020-01-19.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

#include "color.h"

// MARK: - Data Structures

/// The data structure for an MTOB file that has been opened.
struct mtob_t
{
    /// The name of this MTOB.
    ///
    /// Allocated.
    char *name;
};

// MARK: - Functions

/// Open the MTOB file at the current offset of the given file handle into the given MTOB.
/// @param file The file handle to open the MTOB file from.
/// @param mtob The MTOB to open the file into.
void mtob_open(FILE *file, struct mtob_t *mtob);

/// Close the given MTOB, releasing all of it's allocated memory.
///
/// This must be called after an MTOB is opened and before program execution completes.
/// @param mtob The MTOB to close.
void mtob_close(struct mtob_t *mtob);
