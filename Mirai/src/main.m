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

            printf(" - models:\n");
            for (int m = 0; m < cgfx.num_models; m++)
            {
                struct cmdl_t *cmdl = cgfx.models[m];
                printf("    - cmdl %i:\n", m);
                printf("       - name: \"%s\"\n", cmdl->name);
                printf("       - objects: %u\n", cmdl->num_objects);

                printf("       - %u meshes:\n", cmdl->num_meshes);
                for (int m = 0; m < cmdl->num_meshes; m++)
                {
                    struct sobj_mesh_t *mesh = cmdl->mesh_sobjs[m]->mesh;

                    printf("          - mesh %i:\n", m);
                    printf("             - %i face groups:\n", mesh->num_face_groups);
                    for (int g = 0; g < mesh->num_face_groups; g++)
                    {
                        struct sobj_face_group_t *face_group = mesh->face_groups[g];

                        printf("                - face group %i:\n", g);
                        printf("                   - indices: %u\n", face_group->num_indices);
                    }

                    printf("             - %i vertex groups:\n", mesh->num_vertex_groups);
                    for (int g = 0; g < mesh->num_vertex_groups; g++)
                    {
                        struct sobj_vertex_group_t *vertex_group = mesh->vertex_groups[g];

                        printf("                - vertex group %i:\n", g);
                        printf("                   - components: %ucpv\n", vertex_group->num_components);
                        printf("                   - data: %zu bytes at %zu\n", vertex_group->data_size, vertex_group->data_pointer);
                    }
                }

                printf("       - skeletons: %i\n", cmdl->skeleton_sobj != NULL);
            }

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

            cgfx_close(&cgfx);
            printf("\n");
        }
    }

    return 0;
}
