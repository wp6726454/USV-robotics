/*
***********************************************************************
* database.h:
* SQLite3 C/C++ interfaces
*.This header file can be read by both C and C++ compilers
*
*  by Hu.ZH(IPAC@SJTU)
***********************************************************************
*/

#ifndef _DATABASE_H_
#define _DATABASE_H_

#ifdef __cplusplus // call a C function from C++
extern "C" {
#endif

#include <memory.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  sqlite3 *db;
  char *zErrMsg;
  int rc;
} mysql;

static int callback(void *NotUsed, int argc, char **argv, char **azColName) {
  int i;
  for (i = 0; i < argc; i++) {
    printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
  }
  printf("\n");
  return 0;
}

void open_database(mysql *t_mysql) {

  /* Open database */
  t_mysql->rc = sqlite3_open("mytest.db", &t_mysql->db);

  if (t_mysql->rc) {
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(t_mysql->db));
  } else {
    fprintf(stdout, "Opened database successfully\n");
  }

  // create the master table for general information about clients
  const char *sql_table = "CREATE TABLE MASTER("
                          "CLIENT_ID    INT    PRIMARY KEY    NOT NULL,"
                          "TABLE_NAME   TEXT                          ,"
                          "DATETIME     TEXT                         );";

  /* Execute SQL statement */
  t_mysql->rc =
      sqlite3_exec(t_mysql->db, sql_table, callback, 0, &t_mysql->zErrMsg);
  sqlite3_free(t_mysql->zErrMsg);
}

// maseter table: contains general information about all clients
void update_mastertable(mysql *t_mysql, int t_clientid) {
  // convert int to char
  char buffer[20];
  sprintf(buffer, "%d", t_clientid);

  /* Create SQL statement */
  char str[100];
  strcpy(str, "INSERT INTO MASTER (CLIENT_ID,TABLE_NAME,DATETIME) VALUES(");
  strcat(str, buffer);
  strcat(str, ", 'client");
  strcat(str, buffer);
  strcat(str, "', DATETIME('now'));"); // datatime using UTC

  /* Execute SQL statement */
  t_mysql->rc = sqlite3_exec(t_mysql->db, str, callback, 0, &t_mysql->zErrMsg);
  sqlite3_free(t_mysql->zErrMsg);
}

// create each client table
void create_client_table(mysql *t_mysql, int t_clientid) {
  // convert int to char
  char buffer[20];
  sprintf(buffer, "%d", t_clientid);

  // create tables for clients
  const char *sql_start = "CREATE TABLE CLIENT";
  const char *sql_end = "(ID          INTEGER PRIMARY KEY AUTOINCREMENT,"
                        " STATUS      BOOLEAN    NOT NULL,"
                        " DATETIME    TEXT       NOT NULL,"
                        " SURGE       FLOAT, "
                        " SWAY        FLOAT, "
                        " HEAVE       FLOAT, "
                        " ROLL        FLOAT, "
                        " PITCH       FLOAT, "
                        " YAW         FLOAT, "
                        " Ux          FLOAT, "
                        " Uy          FLOAT, "
                        " Utheta      FLOAT);";

  char *combine_char =
      (char *)malloc(1 + strlen(sql_start) + strlen(buffer) + strlen(sql_end));
  strcpy(combine_char, sql_start);
  strcat(combine_char, buffer);
  strcat(combine_char, sql_end);

  /* Execute SQL statement */
  t_mysql->rc =
      sqlite3_exec(t_mysql->db, combine_char, callback, 0, &t_mysql->zErrMsg);

  free(combine_char);
  sqlite3_free(t_mysql->zErrMsg);
}

// convert a float array to char array
void convert_floatp_char(const float *t_motiondata, char *t_str,
                         size_t T_CLIENT_DATA_SIZE) {
  // t_str should be initialized
  for (size_t i = 0; i != T_CLIENT_DATA_SIZE; ++i) {
    char buffer[64];
    snprintf(buffer, sizeof buffer, "%f", *(t_motiondata + i));
    strcat(t_str, ", ");
    strcat(t_str, buffer);
  }
}

// insert a row into some client table
void update_client_table(mysql *t_mysql, int t_clientid, bool t_status,
                         const float *t_motiondata, size_t T_CLIENT_DATA_SIZE) {
  // convert int to char
  char buffer[20];
  sprintf(buffer, "%d", t_clientid);

  char str[500] = "INSERT INTO CLIENT"; // attention !! 500 may be too few
  strcat(str, buffer);
  // t_status=0 means corrected data
  if (t_status) {
    const char *sql_end = "(STATUS, DATETIME) VALUES( 1 , julianday('now'));";
    strcat(str, sql_end);

  } else {
    const char *sql_end = "(STATUS, DATETIME, SURGE, SWAY, HEAVE, ROLL, PITCH, "
                          "YAW, Ux, Uy, Utheta) VALUES( 0 , julianday('now')";
    strcat(str, sql_end);
    convert_floatp_char(t_motiondata, str, T_CLIENT_DATA_SIZE);
    /* Create SQL statement */
    strcat(str, ");");
  }
  /* Execute SQL statement */
  t_mysql->rc = sqlite3_exec(t_mysql->db, str, callback, 0, &t_mysql->zErrMsg);
  sqlite3_free(t_mysql->zErrMsg);
}

// close database and free this resource
void close_database(mysql *t_mysql) {
  if (t_mysql->rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", t_mysql->zErrMsg);
    sqlite3_free(t_mysql->zErrMsg);
  } else {
    fprintf(stdout, "Table created successfully\n");
  }
  sqlite3_close(t_mysql->db);
}

#ifdef __cplusplus
}
#endif

#endif /* _DATABASE_H_ */
