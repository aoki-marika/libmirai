//
//  spr.c
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "spr.h"

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// MARK: - Constants

/// The maximum length of a CTPK name, in bytes.
const int max_ctpk_name_length = 32;

/// The maximum length of an image name, in bytes.
const int max_image_name_length = 71;

// MARK: - Data Structures

/// The data structure for the header of an spr file.
struct spr_header_t
{
    /// The signature for this file, must be `0x0000`.
    uint32_t signature;

    /// The pointer to the beginning of the CTPK data within this file.
    uint32_t ctpks_pointer;

    /// The number of CTPK files within this file.
    uint32_t num_ctpks;

    /// The pointer to the beginning of the CTPK names data within this file.
    uint32_t names_pointer;

    // have no idea what this is, seems to always be all zeroes
    uint8_t unknown[8];

    /// The number of images within this file.
    uint32_t num_images;

    /// The offset of the image data within this file, in bytes.
    uint32_t images_pointer;
};

// MARK: - Functions

/// Read an spr name from the given file handle, of the given max length.
///
/// Sprs store names in a semi-odd way where they allocate a fixed amount of space,
/// then all whitespace is replaced by terminator characters.
///
/// The name is read starting from the current position of the given file handle.
/// @param file The file handle to read the name from.
/// @param allocated_size The size of the allocated space for the name, in bytes.
char *spr_read_name(FILE *file, int allocated_size)
{
    char characters[allocated_size];
    fread(characters, allocated_size, 1, file);

    // allocate the maximum size first,
    // then find the real size and reallocate it
    char *name = malloc(allocated_size + 1);
    int name_index = 0;
    int name_length = 0;
    for (int i = 0; i < allocated_size; i++)
    {
        if (characters[i] != '\0')
        {
            name[name_index] = characters[i];
            name_index++;
            name_length++;
        }
    }

    // +1 for the terminator character
    name = realloc(name, name_length + 1);
    name[name_length] = '\0';
    return name;
}

void spr_open(const char *path, struct spr_t *spr)
{
    // open the file for binary reading
    spr->file = fopen(path, "rb");

    // read the header
    struct spr_header_t header;
    fread(&header, sizeof(struct spr_header_t), 1, spr->file);
    assert(header.signature == 0x0000);

    // initialize the output spr
    spr->num_ctpks = header.num_ctpks;
    spr->ctpk_names = malloc(header.num_ctpks * sizeof(char *));
    spr->ctpks = malloc(header.num_ctpks * sizeof(struct ctpk_t *));
    spr->num_images = header.num_images;
    spr->images = malloc(header.num_images * sizeof(struct spr_image_t *));

    // read each image
    fseek(spr->file, header.images_pointer, SEEK_SET);
    for (int i = 0; i < header.num_images; i++)
    {
        // read the ctpk index
        uint8_t ctpk_index;
        fread(&ctpk_index, sizeof(ctpk_index), 1, spr->file);

        // read the name
        char *name = spr_read_name(spr->file, max_image_name_length);

        // read the top left coordinates
        float start_x, start_y;
        fread(&start_x, sizeof(start_x), 1, spr->file);
        fread(&start_y, sizeof(start_y), 1, spr->file);

        // read the bottom right coordinates
        float end_x, end_y;
        fread(&end_x, sizeof(end_x), 1, spr->file);
        fread(&end_y, sizeof(end_y), 1, spr->file);

        // assert that the coordinates are proper, mostly for debugging
        // potentially remove later?
        assert(start_x <= 1.0 && start_y <= 1.0 && end_x <= 1.0 && end_y <= 1.0);
        assert(end_x >= start_x && end_y >= start_y);

        // unsure what these bytes are, so skip them
        fseek(spr->file, 8, SEEK_CUR);

        // add the image
        struct spr_image_t *image = malloc(sizeof(struct spr_image_t));
        image->ctpk_index = ctpk_index;
        image->name = name;
        image->start_x = start_x;
        image->start_y = start_y;
        image->end_x = end_x;
        image->end_y = end_y;
        spr->images[i] = image;
    }

    // read each ctpk file
    unsigned long long names_pointer = header.names_pointer;
    unsigned long long ctpks_pointer = header.ctpks_pointer;
    for (unsigned int i = 0; i < header.num_ctpks; i++)
    {
        // read the name
        fseek(spr->file, names_pointer, SEEK_SET);
        spr->ctpk_names[i] = spr_read_name(spr->file, max_ctpk_name_length);

        // read the ctpk file
        // not sure what this is but theres an additional 4 bytes before the ctpk header
        // so read those and ignore them
        fseek(spr->file, ctpks_pointer, SEEK_SET);
        fseek(spr->file, 4, SEEK_CUR);

        struct ctpk_t *ctpk = malloc(sizeof(struct ctpk_t));
        ctpk_open(spr->file, ctpk);
        spr->ctpks[i] = ctpk;

        // increment the pointers for the next ctpk
        // ctpk pointer needs to be handled specially as it should skip past texture data without reading it
        // but if there are no textures then the next ctpk should be immediately after
        names_pointer += max_ctpk_name_length;

        if (ctpk->num_textures > 0)
        {
            struct ctpk_texture_t *last_texture = ctpk->textures[ctpk->num_textures - 1];
            ctpks_pointer = last_texture->data_pointer + last_texture->data_size;
        }
        else
        {
            ctpks_pointer = ftell(spr->file);
        }
    }
}

void spr_close(struct spr_t *spr)
{
    for (unsigned int i = 0; i < spr->num_ctpks; i++)
    {
        free(spr->ctpk_names[i]);
        ctpk_close(spr->ctpks[i]);
    }

    for (unsigned int i = 0; i < spr->num_images; i++)
    {
        free(spr->images[i]->name);
        free(spr->images[i]);
    }

    free(spr->ctpk_names);
    free(spr->ctpks);
    free(spr->images);
    fclose(spr->file);
}
