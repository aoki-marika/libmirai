//
//  dsc.h
//  libmirai
//
//  Created by Marika on 2020-03-03.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

// MARK: - Data Structures

struct dsc_t
{
    /// The file handle for the file that this DSC is reading data from.
    ///
    /// Kept open until `dsc_close(dsc)` is called with this DSC.
    FILE *file;
};

// MARK: - Functions

/// Open the DSC file at the given path into the given DSC.
/// @param path The path of the DSC file to open.
/// @param dsc The DSC to open the file into.
void dsc_open(const char *path, struct dsc_t *dsc);

/// Close the given DSC, releasing all of it's allocated memory.
///
/// This must be called after a DSC is opened and before program execution completes.
/// @param dsc The DSC to close.
void dsc_close(struct dsc_t *dsc);
