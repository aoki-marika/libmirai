//
//  aet.c
//  libmirai
//
//  Created by Marika on 2020-01-28.
//  Copyright © 2020 Marika. All rights reserved.
//

#include "aet.h"

#include <stdlib.h>

#include "utils.h"

void aet_open(const char *path, struct aet_t *aet)
{
    // open the file for binary reading
    FILE *file = fopen(path, "rb");

    // read the size of the header, in bytes
    uint32_t header_size;
    fread(&header_size, sizeof(header_size), 1, file);

    // read the section pointers
    //  - scr names: the scr names section, contains data for the names of each of the scrs that each child uses
    //     - this pointer is not used on its own yet, but it appears to contain enums at the beginning
    //  - data: the data section, contains all the actual aet data
    //  - children: the children header section, contains headers for all the child aets within this file
    uint32_t scr_names_pointer, data_pointer, children_pointer;
    fread(&scr_names_pointer, sizeof(scr_names_pointer), 1, file);
    fread(&data_pointer, sizeof(data_pointer), 1, file);
    fread(&children_pointer, sizeof(children_pointer), 1, file);

    // read the child count
    // this is the number of child aets with this file
    // this value is always at the end of the header, regardless of size
    uint32_t num_children;
    fseek(file, header_size, SEEK_SET);
    fread(&num_children, sizeof(num_children), 1, file);

    // TODO: REMOVEME
    printf("%s:\n", path);
    printf(" - children: %u at %u\n", num_children, data_pointer);

    // read the children
    for (int i = 0; i < num_children; i++)
    {
        // read the name of this child
        // the data section begins with pointers to the names of each child
        fseek(file, data_pointer + (i * sizeof(uint32_t)), SEEK_SET);

        uint32_t name_pointer;
        fread(&name_pointer, sizeof(name_pointer), 1, file);
        fseek(file, name_pointer, SEEK_SET);

        char *name = utils_read_string(file);

        // read the name of each of the scrs that this child uses
        // each child has a pointer to the first and last items in the scr name pointer table that it uses
        // its unclear at the moment exactly what these names are for,
        // but they all reference individual scrs within sprs
        // and the data also appears to refer to indices within this array
        fseek(file, children_pointer + (i * sizeof(uint32_t) * 2), SEEK_SET);

        uint32_t first_scr_name_pointer, last_scr_name_pointer;
        fread(&first_scr_name_pointer, sizeof(first_scr_name_pointer), 1, file);
        fread(&last_scr_name_pointer, sizeof(last_scr_name_pointer), 1, file);

        // read all the used scr names of this child
        // calculate the scr name count by dividing the start and end difference by the size of each pointer
        const int num_scr_names = (last_scr_name_pointer - first_scr_name_pointer) / sizeof(uint32_t);
        char *scr_names[num_scr_names];
        for (int i = 0; i < num_scr_names; i++)
        {
            // scr names use a pointer table, so seek to the entry and then to
            // its pointer to get the string
            fseek(file, first_scr_name_pointer + (i * sizeof(uint32_t)), SEEK_SET);

            uint32_t scr_name_pointer;
            fread(&scr_name_pointer, sizeof(scr_name_pointer), 1, file);

            fseek(file, scr_name_pointer, SEEK_SET);
            char *scr_name = utils_read_string(file);
            scr_names[i] = scr_name;
        }

        // TODO: REMOVEME
        printf("    - child %i:\n", i);
        printf("       - name: \"%s\"\n", name);
        printf("       - scr names: %i at %u to %u\n", num_scr_names, first_scr_name_pointer, last_scr_name_pointer);
        for (int i = 0; i < num_scr_names; i++)
            printf("          - scr name %i: \"%s\"\n", i, scr_names[i]);

        // free all the allocated resources
        for (int i = 0; i < num_scr_names; i++)
            free(scr_names[i]);

        free(name);
    }

    // initialize the aet
    aet->file = file;
}

void aet_close(struct aet_t *aet)
{
    fclose(aet->file);
}
