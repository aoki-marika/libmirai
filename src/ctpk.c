//
//  ctpk.c
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "ctpk.h"

#include <stdlib.h>
#include <assert.h>

// MARK: - Data Structures

/// The data structure for the header of a CTPK file.
struct ctpk_header_t
{
    // the signature is separated into multiple fields so that
    // signature checking doesnt have to account for byte order swapping

    /// The `C` character of this file's signature.
    uint8_t signatureC;

    /// The `T` character of this file's signature.
    uint8_t signatureT;

    /// The `P` character of this file's signature.
    uint8_t signatureP;

    /// The `K` character of this file's signature.
    uint8_t signatureK;

    /// The CTPK format version that this file uses.
    uint16_t version;

    /// The number of textures within this file.
    uint16_t num_textures;

    /// The offset of the beginning of the textures within this file, in bytes.
    uint32_t textures_pointer;

    /// The combined size of all the textures within this file, in bytes.
    uint32_t textures_size;

    /// The offset of the hash section within this file, in bytes.
    uint32_t hashes_pointer;

    /// The offset of the beginning of the texture conversion data within this file, in bytes.
    uint32_t conversion_pointer;

    /// Reserved data.
    uint64_t reserved;
};

/// The data structure for the header of a single texture within a CTPK file.
struct ctpk_texture_header_t
{
    uint32_t file_path_pointer;

    /// The size of this texture's encoded data within the file, in bytes.
    uint32_t data_size;

    /// The offset of the beginning of this texture's encoded data within the file, in bytes.
    uint32_t data_pointer;

    /// The format of this texture's encoded data.
    enum texture_format_t format;

    /// The width of this texture, in pixels.
    uint16_t width;

    /// The height of this texture, in pixels.
    uint16_t height;

    /// The mipmap level of this texture.
    uint8_t mipmap_level;

    uint8_t type;
    uint16_t cube_direction;
    uint32_t bitmap_size_pointer;
    uint32_t timestamp;
};

/// The data structure for the conversion info of a single texture within a CTPK file.
///
/// Unsure exactly what this does and it's not necessary at the moment so leaving undocumented.
struct ctpk_texture_conversion_info_t
{
    uint8_t format;
    uint8_t mipmap_level;
    uint8_t compression;
    uint8_t compression_method;
};

// MARK: - Functions

void ctpk_open(FILE *file, struct ctpk_t *ctpk)
{
    // read the start of this file relative to the file handle
    // this is used to store the absolute position of texture data
    unsigned long long start_pointer = ftell(file);

    // read the header
    // the signature is CTPK in ascii
    struct ctpk_header_t header;
    fread(&header, sizeof(header), 1, file);
    assert(header.signatureC == 'C');
    assert(header.signatureT == 'T');
    assert(header.signatureP == 'P');
    assert(header.signatureK == 'K');

    // initialize the output file
    ctpk->num_textures = header.num_textures;
    ctpk->textures = malloc(header.num_textures * sizeof(struct ctpk_texture_t *));

    // read all the textures
    for (unsigned int i = 0; i < header.num_textures; i++)
    {
        // read the header
        struct ctpk_texture_header_t texture_header;
        fread(&texture_header, sizeof(texture_header), 1, file);

        // read the conversion info
        struct ctpk_texture_conversion_info_t conversion_info;
        fread(&conversion_info, sizeof(conversion_info), 1, file);

        // create the texture
        struct texture_t *texture = malloc(sizeof(struct texture_t));
        texture_create(texture_header.width,
                       texture_header.height,
                       texture_header.data_size,
                       start_pointer + header.textures_pointer + texture_header.data_pointer,
                       texture_header.format,
                       texture);

        ctpk->textures[i] = texture;
    }
}

void ctpk_close(struct ctpk_t *ctpk)
{
    for (unsigned int i = 0; i < ctpk->num_textures; i++)
        free(ctpk->textures[i]);

    free(ctpk->textures);
}
