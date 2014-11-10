//
//  Sql.h
//  SmallScaleSearchEngine
//
//  Created by Yebin Han on 11/8/14.
//  Copyright (c) 2014 Duke University. All rights reserved.
//

#ifndef __SmallScaleSearchEngine__Sql__
#define __SmallScaleSearchEngine__Sql__

#include <stdio.h>
#include <assert.h>

#include <string>
#include <iostream>
#include <vector>

#include <sqlite3.h>

class Sql {
    std::string dbName;
    
    sqlite3 * openDB() const {
        sqlite3 * db;
        // open database
        assert(sqlite3_open(dbName.c_str(), &db) == 0);
        return db;
    }
    
    static int defaultCallback(void * data, int argc, char ** argv, char ** azColName){
        return 1;
    }
    
public:
    static std::vector<std::string> result;
    
    Sql (std::string _dbName) : dbName(_dbName) { }

    static int callback(void * data, int argc, char ** argv, char ** azColName){
        for (int i=0; i<argc; ++i) {
            if (argv[i]) Sql::result.push_back(argv[i]);
        }
        return 0;
    }
    
    void executeSql(const char * query, int (*userCallback)(void *, int, char **, char **) = defaultCallback) const {
        // open database
        sqlite3 * db = openDB();
        // execute sql
        char * zErrMsg = NULL;
        int rc = sqlite3_exec(db, query, userCallback, 0, &zErrMsg);
        if (rc != SQLITE_OK) {
            printf("SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        // close database
        sqlite3_close(db);
    }
    
};


#endif /* defined(__SmallScaleSearchEngine__Sql__) */
