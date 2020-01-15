//
//  main.m
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "cgfx.h"

void print_dict(struct cgfx_dict_t *dict, const char *name)
{
    printf("    - %s:\n", name);

    for (int i = 0; i < dict->num_entries; i++)
    {
        struct cgfx_dict_entry_t *entry = dict->entries[i];
        printf("      - entry %i:\n", i);
        printf("         - name: \"%s\"\n", entry->name);
        printf("         - data pointer: %u (%08x)\n", entry->data_pointer, entry->data_pointer);
    }
}

int main(int argc, const char *argv[])
{
    @autoreleasepool {
        // dont attempt to open the program binary
        for (int i = 1; i < argc; i++)
        {
            printf("%s:\n", argv[i]);
            struct cgfx_t cgfx;
            cgfx_open(argv[i], &cgfx);
            print_dict(cgfx.data->models, "models");
            print_dict(cgfx.data->textures, "textures");
            print_dict(cgfx.data->lookup_tables, "lookup tables");
            print_dict(cgfx.data->materials, "materials");
            print_dict(cgfx.data->shaders, "shaders");
            print_dict(cgfx.data->cameras, "cameras");
            print_dict(cgfx.data->lights, "lights");
            print_dict(cgfx.data->fogs, "fogs");
            print_dict(cgfx.data->scenes, "scenes");
            print_dict(cgfx.data->skeletal_animations, "skeletal animations");
            print_dict(cgfx.data->material_animations, "material animations");
            print_dict(cgfx.data->visibility_animations, "visibility animations");
            print_dict(cgfx.data->camera_animations, "camera animations");
            print_dict(cgfx.data->light_animations, "light animations");
            print_dict(cgfx.data->emitters, "emitters");
            cgfx_close(&cgfx);
            printf("\n");
        }
    }

    return 0;
}
