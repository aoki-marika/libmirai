//
//  cmdl.h
//  Mirai
//
//  Created by Marika on 2020-01-15.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

#include "sobj.h"

// MARK: - Data Structures

/// The data structure for a CMDL file that has been opened.
struct cmdl_t
{
    /// The name of this CMDL.
    ///
    /// Allocated.
    char *name;

    /// The number of mesh SOBJs within this CMDL.
    unsigned int num_meshes;

    /// All the mesh SOBJs within this CMDL.
    ///
    /// The type of each of these SOBJs is always `SOBJ_TYPE_MESH`.
    ///
    /// The array and each item are allocated.
    struct sobj_t **meshes;

    /// The number of shape SOBJs within this CMDL.
    unsigned int num_shapes;

    /// All the shape SOBJs within this CMDL.
    ///
    /// The type of each of these SOBJs is always `SOBJ_TYPE_SHAPE`.
    ///
    /// The array and each item are allocated.
    struct sobj_t **shapes;

    /// The skeleton SOBJ of this CMDL, if any.
    ///
    /// The type of this SOBJ is always `SOBJ_TYPE_SKELETON`.
    ///
    /// If this is `NULL`, then this CMDL has no skeleton.
    /// If this is not `NULL`, then this is allocated.
    struct sobj_t *skeleton;
};

// MARK: - Functions

/// Open the CMDL file at the current offset of the given file handle into the given CMDL.
/// @param file The file handle to open the CMDL file from.
/// @param cmdl The CMDL to open the file into.
void cmdl_open(FILE *file, struct cmdl_t *cmdl);

/// Close the given CMDL, releasing all of it's allocated memory.
///
/// This must be called after a CMDL is opened and before program execution completes.
/// @param cmdl The CMDL to close.
void cmdl_close(struct cmdl_t *cmdl);
