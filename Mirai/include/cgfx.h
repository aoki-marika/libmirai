//
//  cgfx.h
//  Mirai
//
//  Created by Marika on 2020-01-14.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdio.h>

#include "txob.h"
#include "dict.h"

// MARK: - Data Structures

/// The data structure for a CGFX file that has been opened.
struct cgfx_t
{
    /// The file handle for the file that this CGFX is reading data from.
    ///
    /// Kept open until `cgfx_close(cgfx)` is called with this CGFX.
    FILE *file;

    /// The number of textures within this CGFX file.
    unsigned int num_textures;

    /// All of the textures within this CGFX file.
    ///
    /// The array and each TXOB are allocated.
    struct txob_t **textures;

    /// The data section of this CGFX.
    ///
    /// Allocated.
    struct cgfx_data_t *data;
};

/// The data structure for the data section within a CGFX file.
struct cgfx_data_t
{
    /// The dictionary of model data within this section.
    struct dict_t *models;

    /// The dictionary of texture data within this section.
    struct dict_t *textures;

    /// The dictionary of lookup table data within this section.
    struct dict_t *lookup_tables;

    /// The dictionary of material data within this section.
    struct dict_t *materials;

    /// The dictionary of shader data within this section.
    struct dict_t *shaders;

    /// The dictionary of camera data within this section.
    struct dict_t *cameras;

    /// The dictionary of light data within this section.
    struct dict_t *lights;

    /// The dictionary of fog data within this section.
    struct dict_t *fogs;

    /// The dictionary of scene data within this section.
    struct dict_t *scenes;

    /// The dictionary of skeletal animation data within this section.
    struct dict_t *skeletal_animations;

    /// The dictionary of material animation data within this section.
    struct dict_t *material_animations;

    /// The dictionary of visibility animation data within this section.
    struct dict_t *visibility_animations;

    /// The dictionary of camera animation data within this section.
    struct dict_t *camera_animations;

    /// The dictionary of light animation data within this section.
    struct dict_t *light_animations;

    /// The dictionary of emitter data within this section.
    struct dict_t *emitters;
};

// MARK: - Functions

/// Open the CGFX file at the given path into the given CGFX.
/// @param path The path of the CGFX file to open.
/// @param cgfx The CGFX to open the file into.
void cgfx_open(const char *path, struct cgfx_t *cgfx);

/// Close the given CGFX, releasing all of it's allocated memory.
///
/// This must be called after a CGFX is opened and before program execution completes.
/// @param cgfx The CGFX to close.
void cgfx_close(struct cgfx_t *cgfx);
