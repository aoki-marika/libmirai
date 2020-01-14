//
//  db.h
//  Mirai
//
//  Created by Marika on 2020-01-14.
//  Copyright © 2020 Marika. All rights reserved.
//

#pragma once

// MARK: - Data Structures

/// The data structure for a database file that has been opened.
struct db_t
{
};

// MARK: - Functions

/// Open the database file at the given path into the given database.
/// @param path The path of the database file to open.
/// @param db The database to open the file into.
void db_open(const char *path, struct db_t *db);

/// Close the given database, releasing all of it's allocated memory.
///
/// This must be called after a database is opened and before program execution completes.
/// @param db The database to close.
void db_close(struct db_t *db);
