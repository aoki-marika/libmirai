//
//  sobj.h
//  Mirai
//
//  Created by Marika on 2020-01-16.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

// MARK: - Data Structures

/// The data structure for an SOBJ file that has been opened.
struct sobj_t
{
};

// MARK: - Functions

/// Open the SOBJ file at the current offset of the given file handle into the given SOBJ.
/// @param file The file handle to open the SOBJ file from.
/// @param sobj The SOBJ to open the file into.
void sobj_open(FILE *file, struct sobj_t *sobj);

/// Close the given SOBJ, releasing all of it's allocated memory.
///
/// This must be called after an SOBJ is opened and before program execution completes.
/// @param sobj The SOBJ to close.
void sobj_close(struct sobj_t *sobj);
