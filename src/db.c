//
//  db.c
//  Mirai
//
//  Created by Marika on 2020-01-14.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "db.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

// MARK: - Data Structures

/// The data structure for the header of a database file.
struct db_header_t
{
    // number of resources in the db?
    // seems to correlate to the number of string triplets suceeding the header
    // only works in sprdb, nwdb breaks after the NULL occurence
    // it most likely instead refers to the number of items in the block the pointer refers to
    uint32_t num_unknown;

    // pointer to beginning of the bottom data block, not sure whats in it
    uint32_t unknown_pointer;

    // unknown data, seems to always be zeroes
    uint8_t unknown[24];
};

// MARK: - Functions

void db_open(const char *path, struct db_t *db)
{
    // open the file for binary reading
    FILE *file = fopen(path, "rb");

    // read the header
    struct db_header_t header;
    fread(&header, sizeof(header), 1, file);

    // read the ???
    // appear to be in triplets again
    int unknown_pointer = header.unknown_pointer;
    printf(" - %u unknowns:\n", header.num_unknown);
    for (int i = 0; i < header.num_unknown; i++)
    {
        printf("    - unknown %i:\n", i);
        while (1)
        {
            // only flags encountered are 0x1
            // which seems to relate to the LOOKUPTABLESETCONTENTCTR files
            // and whenever a 0x1 flag is encountered then the string is incomplete,
            // with an empty then LOOKUPTABLESETCONTENTCTR string above it
            // also of note that only nwdb uses flag 0x1
            uint16_t pointer;
            uint16_t flags;
            fseek(file, unknown_pointer, SEEK_SET);
            fread(&pointer, sizeof(pointer), 1, file);
            fread(&flags, sizeof(flags), 1, file);
            unknown_pointer += sizeof(pointer) + sizeof(flags);

            fseek(file, pointer, SEEK_SET);
            char *string = utils_read_string(file);

            // unsure what null is used for, maybe byte alignment?
            // is always suceeded by two broken items, so on every null skip those
            if (strcmp(string, "NULL") == 0)
            {
                unknown_pointer += sizeof(uint16_t) * 4;
                continue;
            }

            const char *suffix = ".bin";
            if (strlen(string) >= strlen(suffix) && strcmp(string + (strlen(string) - strlen(suffix)), suffix) == 0)
            {
                printf("       - flags %04x uses file \"%s\"\n", flags, string);
                free(string);
                break;
            }
            else {
                printf("       - flags %04x alias \"%s\"\n", flags, string);
                free(string);
            }
        }

//        printf("%i:\t\"%s\"\t\"%s\"\t\"%s\"\n", i, first, second, third);

//        uint32_t string_pointer;
//        fseek(file, header.unknown_pointer + i * sizeof(uint32_t), SEEK_SET);
//        fread(&string_pointer, sizeof(string_pointer), 1, file);
//
//        char name[256];
//        fseek(file, string_pointer, SEEK_SET);
//        db_read_string(file, name, sizeof(name));
//
//        printf("    - unknown %i: \"%s\" at %u\n", i, name, string_pointer);

//        printf("    - ??? %i: ", i);
//        for (int i = 0; i < sizeof(unknown); i++)
//            printf("%02x", unknown[i]);
//        printf("\n");
    }
}

void db_close(struct db_t *db)
{
}
