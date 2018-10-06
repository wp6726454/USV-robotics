#include "../include/database.h"

void open_database(mysql *t_mysql, FILE *file) {
  /* Open database */
  t_mysql->rc = sqlite3_open("./data/mytest.db", &t_mysql->db);

  if (t_mysql->rc) {
    if (FILEORNOT) {
      file = fopen("./data/log", "a+");
      fprintf(file, "Can't open database: %s\n", sqlite3_errmsg(t_mysql->db));
      fclose(file);
    } else
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(t_mysql->db));
  } else {
    if (FILEORNOT) {
      file = fopen("./data/log", "a+");
      fprintf(file, "Opened database successfully\n");
      fclose(file);
    } else
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
                        " SURGE       DOUBLE, "
                        " SWAY        DOUBLE, "
                        " HEAVE       DOUBLE, "
                        " ROLL        DOUBLE, "
                        " PITCH       DOUBLE, "
                        " YAW         DOUBLE, "
                        " Ux          DOUBLE, "
                        " Uy          DOUBLE, "
                        " Utheta      DOUBLE);";

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
void convert_floatp_char(const float *t_motiondata, char *t_str) {
  // t_str should be initialized
  for (size_t i = 0; i != CLIENT_DATA_SIZE; ++i) {
    char buffer[32];
    snprintf(buffer, sizeof buffer, "%f", *(t_motiondata + i));
    strcat(t_str, ", ");
    strcat(t_str, buffer);
  }
}

// convert a float array to char array
void convert_doublep_char(const double *t_motiondata, char *t_str) {
  // t_str should be initialized
  for (size_t i = 0; i != CLIENT_DATA_SIZE; ++i) {
    char buffer[64];
    snprintf(buffer, sizeof buffer, "%lf", *(t_motiondata + i));
    strcat(t_str, ", ");
    strcat(t_str, buffer);
  }
}
// insert a row into some client table
void update_client_table(mysql *t_mysql, int t_clientid, bool t_status,
                         const double *t_motiondata) {
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
    convert_doublep_char(t_motiondata, str);
    /* Create SQL statement */
    strcat(str, ");");
  }
  /* Execute SQL statement */
  t_mysql->rc = sqlite3_exec(t_mysql->db, str, callback, 0, &t_mysql->zErrMsg);
  sqlite3_free(t_mysql->zErrMsg);
}

// close database and free this resource
void close_database(mysql *t_mysql, FILE *file) {
  if (t_mysql->rc != SQLITE_OK) {
    if (FILEORNOT) {
      file = fopen("./data/log", "a+");
      fprintf(file, "SQL error: %s\n", t_mysql->zErrMsg);
      fclose(file);
    } else
      fprintf(stderr, "SQL error: %s\n", t_mysql->zErrMsg);
  } else {
    if (FILEORNOT) {
      file = fopen("./data/log", "a+");
      fprintf(file, "Table created successfully\n");
      fclose(file);
    } else
      fprintf(stdout, "Table created successfully\n");
  }
  sqlite3_free(t_mysql->zErrMsg);
  sqlite3_close(t_mysql->db);
}
