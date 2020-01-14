//
//  cgfx.c
//  Mirai
//
//  Created by Marika on 2020-01-14.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "cgfx.h"

#include <stdio.h>
#include <assert.h>

// MARK: - Data Structures

/// The different byte orders that a CGFX file's integers can be in.
enum cgfx_byteorder_t
{
    /// Big endian.
    CGFX_BYTEORDER_BIG = 0xfffe,

    /// Little endian.
    CGFX_BYTEORDER_LITTLE = 0xfeff,
};

/// The data structure for the header of a CGFX file.
struct cgfx_header_t
{
    // each character of the signature is split up to avoid dealing with byte order

    /// The C character of this file's signature.
    uint8_t signatureC;

    /// The G character of this file's signature.
    uint8_t signatureG;

    /// The F character of this file's signature.
    uint8_t signatureF;

    /// The X character of this file's signature.
    uint8_t signatureX;

    /// The byteorder of this file's integers.
    uint16_t byteorder;

    /// The size of this header, in bytes.
    uint16_t size;

    /// The CGFX format revision that this file uses.
    uint32_t revision;

    /// The size of this CGFX file, in bytes;
    uint32_t file_size;

    /// The number of entries within this CGFX file.
    uint32_t num_entries;
};

// MARK: - Functions

void cgfx_open(const char *path)
{
    // open the file for binary reading
    FILE *file = fopen(path, "rb");

    // read the header
    // the signature is CGFX in ascii
    struct cgfx_header_t header;
    fread(&header, sizeof(header), 1, file);
    assert(header.signatureC == 'C');
    assert(header.signatureG == 'G');
    assert(header.signatureF == 'F');
    assert(header.signatureX == 'X');

    // get the byteorder
    // cant use the enum directly as its 32bit but in the file its 16bit
    enum cgfx_byteorder_t byteorder = header.byteorder;
    switch (byteorder)
    {
        case CGFX_BYTEORDER_BIG:    printf(" - big endian\n");
        case CGFX_BYTEORDER_LITTLE: printf(" - little endian\n");
    }

    printf(" - header size: %u\n", header.size);
    printf(" - revision: %u\n", header.revision);
    printf(" - file size: %u\n", header.file_size);
    printf(" - entries: %u\n", header.num_entries);

    // TODO: REMOVEME
    fclose(file);
}
