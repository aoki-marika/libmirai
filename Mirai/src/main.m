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
            for (int i = 0; i < cgfx.num_models; i++)
            {
                struct cmdl_t *cmdl = cgfx.models[i];
                printf("    - cmdl %i:\n", i);
                printf("       - name: \"%s\"\n", cmdl->name);
                printf("       - meshes: %u\n", cmdl->num_meshes);

                printf("       - %u shapes:\n", cmdl->num_shapes);
                for (int i = 0; i < cmdl->num_shapes; i++)
                {
                    struct sobj_shape_t *shape = cmdl->shapes[i]->shape;

                    printf("          - shape %i:\n", i);
                    printf("             - %i face groups:\n", i);
                    for (int g = 0; g < shape->num_face_groups; g++)
                    {
                        struct sobj_face_group_t *face_group = shape->face_groups[g];

                        printf("                - face group %i:\n", g);
                        printf("                   - indices: %u\n", face_group->num_indices);
                    }

                    printf("             - %i vertex groups:\n", i);
                    for (int g = 0; g < shape->num_vertex_groups; g++)
                    {
                        struct sobj_vertex_group_t *vertex_group = shape->vertex_groups[g];

                        printf("                - vertex group %i:\n", g);
                        printf("                   - components: %ucpv\n", vertex_group->num_components);
                        printf("                   - data: %zu bytes at %zu\n", vertex_group->data_size, vertex_group->data_pointer);
                    }
                }

                printf("       - skeletons: %i\n", cmdl->skeleton != NULL);
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
