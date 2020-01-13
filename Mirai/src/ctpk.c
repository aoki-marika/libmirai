//
//  ctpk.c
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "ctpk.h"

#include <assert.h>
#include <stdlib.h>

// MARK: - Data Structures

/// The data structure for the header of a CTPK file.
struct ctpk_header
{
    /// The `CTPK` signature of this file.
    uint32_t signature;

    /// The CTPK format version that this file uses.
    uint16_t version;

    /// The number of textures within this file.
    uint16_t num_textures;

    /// The pointer to the beginning of the texture data within this file.
    uint32_t textures_pointer;

    /// The size, in bytes, of the texture data within this file.
    uint32_t textures_size;

    /// The pointer to the hash section within this file.
    uint32_t hashes_pointer;

    /// The pointer to the beggining of the hash section within this file.
    uint32_t conversion_pointer;

    /// Reserved
    uint64_t reserved;
};

/// The data structure for the header of a single texture within a CTPK file.
struct ctpk_texture_header
{
    uint32_t file_path_pointer;
    uint32_t data_size;
    uint32_t data_pointer;
    uint32_t format;
    uint16_t width;
    uint16_t height;
    uint8_t mipmap_level;
    uint8_t type;
    uint16_t cube_direction;
    uint32_t bitmap_size_pointer;
    uint32_t timestamp;
};

/// The data structure for the conversion info of a single texture within a CTPK file.
struct ctpk_texture_conversion_info
{
    uint8_t format;
    uint8_t mipmap_level;
    uint8_t compression;
    uint8_t compression_method;
};

// MARK: - Functions

void ctpk_open(FILE *input, struct ctpk_file *output)
{
    // read the start of this file relative to the file handle
    unsigned long long start_pointer = ftell(input);

    // read the header
    // the signature is CTPK in ascii
    // but since the byte order is swapped when reading it,
    // it is actually KPTC
    struct ctpk_header header;
    fread(&header, sizeof(header), 1, input);
    assert(header.signature == 0x4b505443);

    // initialize the output file
    output->num_textures = header.num_textures;
    output->textures = malloc(header.num_textures * sizeof(struct ctpk_texture *));

    // read all the textures
    for (int i = 0; i < header.num_textures; i++)
    {
        // read the header
        struct ctpk_texture_header texture_header;
        fread(&texture_header, sizeof(texture_header), 1, input);

        // read the conversion info
        struct ctpk_texture_conversion_info conversion_info;
        fread(&conversion_info, sizeof(conversion_info), 1, input);

        // create the texture
        struct ctpk_texture *texture = malloc(sizeof(struct ctpk_texture));
        texture->width = texture_header.width;
        texture->height = texture_header.height;
        texture->data_size = texture_header.data_size;
        texture->data_pointer = start_pointer + header.textures_pointer + texture_header.data_pointer;
        output->textures[i] = texture;
    }
}

void ctpk_close(struct ctpk_file *file)
{
    for (unsigned int i = 0; i < file->num_textures; i++)
        free(file->textures[i]);

    free(file->textures);
}
