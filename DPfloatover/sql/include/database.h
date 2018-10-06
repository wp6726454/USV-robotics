/*
***********************************************************************
* database.h:
* SQLite3 C/C++ interfaces
*.This header file can be read by both C and C++ compilers
*
*  by Hu.ZH(Mr. SKLOE@SJTU)
***********************************************************************
*/

#ifndef _DATABASE_H_
#define _DATABASE_H_

#ifdef __cplusplus  // call a C function from C++
extern "C" {
#endif

#include <memory.h>
#include <sqlite3.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "constants.h"

// we should not initialize elements inside the structure
typedef struct {
  sqlite3 *db;
  char *zErrMsg;
  int rc;
} mysql;

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  for (int i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

void open_database(mysql *t_mysql, FILE *file);

// maseter table: contains general information about all clients
void update_mastertable(mysql *t_mysql, int t_clientid);

// create each client table
void create_client_table(mysql *t_mysql, int t_clientid);

// convert a float array to char array
void convert_floatp_char(const float *t_motiondata, char *t_str);

// convert a float array to char array
void convert_doublep_char(const double *t_motiondata, char *t_str);

// insert a row into some client table
void update_client_table(mysql *t_mysql, int t_clientid, bool t_status,
                         const double *t_motiondata);

// close database and free this resource
void close_database(mysql *t_mysql, FILE *file);

#ifdef __cplusplus
}
#endif

#endif /* _DATABASE_H_ */
