//
//  main.m
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "spr.h"

int main(int argc, const char *argv[])
{
    @autoreleasepool {
        // dont attempt to open the program binary
        for (int i = 1; i < argc; i++)
        {
            printf("%s:\n", argv[i]);
            struct spr_file file;
            spr_open(argv[i], &file);

            for (unsigned int i = 0; i < file.num_ctpks; i++)
            {
                struct ctpk_file *ctpk = file.ctpks[i];
                const char *name = file.ctpk_names[i];
                printf(" - ctpk %u: %s\n", i, name);
                for (unsigned int i = 0; i < ctpk->num_textures; i++)
                {
                    struct ctpk_texture *texture = ctpk->textures[i];
                    printf("    - texture %u: %ux%u\n", i, texture->width, texture->height);
                }
            }

            spr_close(&file);
            printf("\n");
        }
    }

    return 0;
}
