//
//  aet.h
//  libmirai
//
//  Created by Marika on 2020-01-28.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

// MARK: - Data Structures

/// The data structure for an AET file that has been opened.
struct aet_t
{
    /// The file handle for the file that this AET is reading data from.
    ///
    /// Kept open until `aet_close(aet)` is called with this AET.
    FILE *file;
};

// MARK: - Functions

/// Open the AET file at the given path into the given AET.
/// @param path The path of the AET file to open.
/// @param aet The AET to open the file into.
void aet_open(const char *path, struct aet_t *aet);

/// Close the given AET, releasing all of it's allocated memory.
///
/// This must be called after an AET is opened and before program execution completes.
/// @param aet The AET to close.
void aet_close(struct aet_t *aet);
