//
//  main.m
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "cgfx.h"

int main(int argc, const char *argv[])
{
    @autoreleasepool {
        // dont attempt to open the program binary
        for (int i = 1; i < argc; i++)
        {
            printf("%s:\n", argv[i]);
            struct cgfx_t cgfx;
            cgfx_open(argv[i], &cgfx);

            printf(" - textures:\n");
            for (int i = 0; i < cgfx.num_textures; i++)
            {
                struct txob_t *txob = cgfx.textures[i];
                printf("    - txob %i:\n", i);
                printf("       - name: \"%s\"\n", txob->name);
                printf("       - size: %ux%u\n", txob->texture.width, txob->texture.height);
                printf("       - format: %08x\n", txob->texture.data_format);
                printf("       - data: %zu bytes at %zu (%08zx)\n", txob->texture.data_size, txob->texture.data_pointer, txob->texture.data_pointer);
            }

            printf(" - models:\n");
            for (int i = 0; i < cgfx.data->models->num_entries; i++)
            {
                struct cgfx_dict_entry_t *entry = cgfx.data->models->entries[i];
                printf("    - cmdl %i:\n", i);
                printf("       - dict name: \"%s\"\n", entry->name);
                printf("       - data pointer: %u (%08x)\n", entry->data_pointer, entry->data_pointer);

                // read the flags and cmdl
                uint32_t flags;
                fseek(cgfx.file, entry->data_pointer, SEEK_SET);
                fread(&flags, sizeof(flags), 1, cgfx.file);

                printf("       - has skeleton: %i\n", (flags & 0x80) > 0);
            }

            cgfx_close(&cgfx);
            printf("\n");
        }
    }

    return 0;
}
