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
    /// The file handle for the file that this spr is reading data from.
    ///
    /// Kept open until `spr_close(spr)` is called with this spr.
    FILE *file;

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

    /// The number of images within this spr.
    unsigned int num_images;

    /// All of the images within this spr.
    ///
    /// The array and each image are allocated.
    struct spr_image_t **images;
};

/// The data structure for a single image within a texture of a CTPK within an spr.
///
/// When referring to normalized coordinates within an image that means that it is on a scale of `0` to `1`,
/// then at runtime it is mapped to the actual size of the texture.
struct spr_image_t
{
    /// The index of the CTPK within this image's spr that this image is within.
    ///
    /// Sprs don't seem to utilise multi-texture CTPKs, so an image applies to all textures within the CTPK.
    uint8_t ctpk_index;

    /// The name of this image.
    ///
    /// Allocated.
    char *name;

    /// The normalized X position of the top-left corner of this image within it's texture.
    float start_x;

    /// The normalized Y position of the top-left corner of this image within it's texture.
    float start_y;

    /// The normalized X position of the bottom-left corner of this image within it's texture.
    float end_x;

    /// The normalized Y position of the bottom-right corner of this image within it's texture.
    float end_y;
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
