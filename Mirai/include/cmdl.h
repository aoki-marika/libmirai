//
//  cmdl.h
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

// MARK: - Data Structures

/// The data structure for a CMDL file that has been opened.
struct cmdl_t
{
};

// MARK: - Functions

/// Open the CMDL file at the given path into the given CMDL.
/// @param file The file handle to open the CMDL file from.
/// @param cmdl The CMDL to open the file into.
void cmdl_open(FILE *file, struct cmdl_t *cmdl);

/// Close the given CMDL, releasing all of it's allocated memory.
///
/// This must be called after a CMDL is opened and before program execution completes.
/// @param cmdl The CMDL to close.
void cmdl_close(struct cmdl_t *cmdl);
