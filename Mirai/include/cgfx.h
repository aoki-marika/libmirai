//
//  cgfx.h
//  Mirai
//
//  Created by Marika on 2020-01-14.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

#include <stdint.h>

// MARK: - Data Structures

/// The data structure for a CGFX file that has been opened.
struct cgfx_t
{
    /// The data section of this CGFX.
    ///
    /// Allocated.
    struct cgfx_data_t *data;
};

/// The data structure for the data section within a CGFX file.
struct cgfx_data_t
{
    /// The dictionary of model data within this section.
    struct cgfx_dict_t *models;

    /// The dictionary of texture data within this section.
    struct cgfx_dict_t *textures;

    /// The dictionary of lookup table data within this section.
    struct cgfx_dict_t *lookup_tables;

    /// The dictionary of material data within this section.
    struct cgfx_dict_t *materials;

    /// The dictionary of shader data within this section.
    struct cgfx_dict_t *shaders;

    /// The dictionary of camera data within this section.
    struct cgfx_dict_t *cameras;

    /// The dictionary of light data within this section.
    struct cgfx_dict_t *lights;

    /// The dictionary of fog data within this section.
    struct cgfx_dict_t *fogs;

    /// The dictionary of scene data within this section.
    struct cgfx_dict_t *scenes;

    /// The dictionary of skeletal animation data within this section.
    struct cgfx_dict_t *skeletal_animations;

    /// The dictionary of material animation data within this section.
    struct cgfx_dict_t *material_animations;

    /// The dictionary of visibility animation data within this section.
    struct cgfx_dict_t *visibility_animations;

    /// The dictionary of camera animation data within this section.
    struct cgfx_dict_t *camera_animations;

    /// The dictionary of light animation data within this section.
    struct cgfx_dict_t *light_animations;

    /// The dictionary of emitter data within this section.
    struct cgfx_dict_t *emitters;
};

/// The data structure for a single dictionary within a CGFX data section.
struct cgfx_dict_t
{
    /// The number of entries within this dictionary
    unsigned int num_entries;

    /// All of the entries within this dictionary.
    ///
    /// The array and each entry are allocated.
    struct cgfx_dict_entry_t **entries;
};

/// The data structure for a single entry within a dictionary.
struct cgfx_dict_entry_t
{
    /// The name of this entry.
    ///
    /// Allocated.
    char *name;

    /// The offset of this entry's data in the file, in bytes.
    ///
    /// This data can be in any format or of any length, both are determined by which dictionary it belongs to.
    uint32_t data_pointer;
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
