/*
***********************************************************************
* tcpclient.h:
*	a stream socket client like telnet
* This header file can be read by both C and C++ compilers
*
*  by Hu.ZH(IPAC@SJTU)
***********************************************************************
*/

#ifndef _TCPCLIENT_H_
#define _TCPCLIENT_H_

#ifdef __cplusplus // call a C function from C++
extern "C" {
#endif

#include "../../include/constants.h"
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef struct {
  int sockfd;
  int numbytes;
  unsigned char send_buf[MAXDATASIZE];
  unsigned char recv_buf[MAXDATASIZE];
  struct addrinfo hints, *servinfo, *p;
  int rv;
  char s[INET6_ADDRSTRLEN];
} tcpsocketclient;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);

void setup_caddr_info(tcpsocketclient *t_client, char *t_server_add);

void create_socket_and_connect(tcpsocketclient *t_client);

void get_serveradd_print(tcpsocketclient *t_client);

void sr_messsages(tcpsocketclient *t_client);

#ifdef __cplusplus
}
#endif

#endif /*_TCPCLIENT_H_*/
