/*
***********************************************************************
* tcpserver.h:
*	a cheezy multiperson chat server for static or dynamic clients using
* "select".  This header file can be read by both C and C++ compilers
*
*  by Hu.ZH(IPAC@SJTU)
***********************************************************************
*/

#ifndef _TCPSERVER_H_
#define _TCPSERVER_H_

#ifdef __cplusplus // call a C function from C++
extern "C" {
#endif

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT "9035"     // port we're listening on
#define MAXDATASIZE 100 // max number of bytes we can get at once
#define BACKLOG 10      // how many pending connections queue will hold
#define MAXCONNECTION 0
// maximum number of clients connected, "0" means unlimited number of clients
const int yes = 1; // for setsockopt() SO_REUSEADDR, below

#define max(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a > _b ? _a : _b;                                                         \
  })

typedef struct {
  fd_set master;                      // master file descriptor list
  fd_set read_fds;                    // temp file descriptor list for select()
  int fdmax;                          // maximum file descriptor number
  int numexist;                       // number of existing connections
  int listener;                       // listening socket descriptor
  int newfd;                          // newly accept()ed socket descriptor
  struct sockaddr_storage remoteaddr; // client address
  socklen_t addrlen;
  unsigned char buf[MAXDATASIZE]; // buffer for client data
  int nbytes;

  char remoteIP[INET6_ADDRSTRLEN];

  struct addrinfo hints, *ai, *p;
} tcpsocketserver;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa);

void setup_saddr_info(tcpsocketserver *t_server);

void open_listenfd(tcpsocketserver *t_server);

void select_rvmessage(tcpsocketserver *t_server);

#ifdef __cplusplus
}
#endif

#endif /*_TCPSERVER_H_*/
