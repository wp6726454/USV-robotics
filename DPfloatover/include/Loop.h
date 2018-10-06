/*
***********************************************************************
* loop.h:
* function to run the whole loop on server (including receive from clients,
* crccheck, unpack, 6D motion capture, LQE, LQR, thruster allocation, pack, send
* to clients, save2sqlite, viewer).
* This header file can be read by both C and C++ compilers
*
*  by Hu.ZH(Mr.SJTU)
***********************************************************************
*/

#ifndef _LOOP_H_
#define _LOOP_H_

#ifdef __cplusplus  // call a C function from C++
extern "C" {
#endif

#include "../network/include/crccheck.h"
#include "../network/include/datapack.h"
#include "../network/include/tcpserver.h"
#include "../sql/include/database.h"
#include "constants.h"

// void testtcpserver_sql(tcpsocketserver *t_server, mysql *t_mysql);

int select_accept(tcpsocketserver *t_server, mysql *t_mysql);

void receiveallclients(tcpsocketserver *t_server, mysql *t_mysql);

#ifdef __cplusplus
}
#endif

#endif /* _LOOP_H_ */
