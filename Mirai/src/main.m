//
//  main.m
//  Mirai
//
//  Created by Marika on 2020-01-13.
//  Copyright © 2020 Marika. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "db.h"

int main(int argc, const char *argv[])
{
    @autoreleasepool {
        // dont attempt to open the program binary
        for (int i = 1; i < argc; i++)
        {
            printf("%s:\n", argv[i]);
            struct db_t db;
            db_open(argv[i], &db);
            db_close(&db);
            printf("\n");
        }
    }

    return 0;
}
