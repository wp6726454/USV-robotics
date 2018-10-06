/*
***********************************************************************
* tcpserver_t.h:
*	a cheezy multiperson chat server for static or dynamic clients using
* "select".  This header file can be read by both C and C++ compilers
*
*  by Hu.ZH(Mr. SJTU)
***********************************************************************
*/

#ifndef _TCPSERVERT_H_
#define _TCPSERVERT_H_

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <Eigen/Core>
#include "constants.h"

#define BACKLOG 10  // how many pending connections queue will hold

typedef Eigen::Matrix<int, MAXCONNECTION, 1> Vectfd;

class threadtcpsocketserver {
 public:
  explicit threadtcpsocketserver() : numexist(0), listener(0) {}
  ~threadtcpsocketserver() {}

  void initializesocketserver(FILE *t_file) {
    setup_saddr_info(t_file);
    open_listenfd(t_file);
  }
  // accept client's request
  int acceptclients(FILE *t_file) {
    addrlen = sizeof remoteaddr;
    int newfd = accept(listener, (struct sockaddr *)&remoteaddr, &addrlen);
    if (newfd == -1) {
      // print error information
      if (FILEORNOT) {
        t_file = fopen(logsavepath.c_str(), "a+");
        fprintf(t_file, "error in accept!\n");
        fclose(t_file);
      } else
        perror("accept");
    } else {
      SetClientsFd(newfd);
      increaseNumexist();

      // print connected clients information
      if (FILEORNOT) {
        t_file = fopen(logsavepath.c_str(), "a+");
        fprintf(t_file,
                "thread-server: new connection from %s on "
                "socket %d\n",
                inet_ntop(remoteaddr.ss_family,
                          get_in_addr((struct sockaddr *)&remoteaddr), remoteIP,
                          INET6_ADDRSTRLEN),
                newfd);
        fclose(t_file);
      } else
        printf(
            "thread-server: new connection from %s on "
            "socket %d\n",
            inet_ntop(remoteaddr.ss_family,

                      get_in_addr((struct sockaddr *)&remoteaddr), remoteIP,
                      INET6_ADDRSTRLEN),
            newfd);
    }
    return newfd;
  }

  Vectfd GetClientsFd() const { return clients_fd; }
  int GetexistnumClients() const { return numexist; }
  int Getlistenerfd() const { return listener; }
  void SetClientsFd(int t_newfd) {
    if (numexist < MAXCONNECTION) clients_fd(numexist) = t_newfd;
  }
  void increaseNumexist() { ++numexist; }
  void clearclients_fd(int sockfd_index) {
    if (sockfd_index < MAXCONNECTION) clients_fd(sockfd_index) = 0;
  }

 private:
  Vectfd clients_fd = Vectfd::Zero();
  int numexist;                        // number of existing connections
  int listener;                        // listening socket descriptor
  struct sockaddr_storage remoteaddr;  // client address
  socklen_t addrlen;
  char remoteIP[INET6_ADDRSTRLEN];
  struct addrinfo hints;
  struct addrinfo *ai = NULL;
  struct addrinfo *p = NULL;

  // get sockaddr, IPv4 or IPv6:
  void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
      return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
  }
  // setup socket address
  void setup_saddr_info(FILE *file) {
    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    int rv = 0;
    if ((rv = getaddrinfo(NULL, PORT, &hints, &ai)) != 0) {
      if (FILEORNOT) {
        file = fopen(logsavepath.c_str(), "a+");
        fprintf(file, "selectserver: %s\n", gai_strerror(rv));
        fclose(file);
      } else {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
      }
    }
  }
  // setup socket listener(a.k.a server)
  void open_listenfd(FILE *file) {
    for (p = ai; p != NULL; p = p->ai_next) {
      listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if (listener < 0) {
        continue;
      }

      // lose the pesky "address already in use" error message
      int yes = 1;
      setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

      if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
        close(listener);
        continue;
      }
      break;
    }
    // if we got here, it means we didn't get bound
    if (p == NULL) {
      if (FILEORNOT) {
        file = fopen(logsavepath.c_str(), "a+");
        fprintf(file, "selectserver: failed to bind\n");
        fclose(file);
      } else {
        fprintf(stderr, "selectserver: failed to bind\n");
        exit(2);
      }
    }

    freeaddrinfo(ai);  // all done with this

    // listen
    if (listen(listener, BACKLOG) == -1) {
      if (FILEORNOT) {
        file = fopen(logsavepath.c_str(), "a+");
        fprintf(file, "fail to listen\n");
        fclose(file);
      } else {
        perror("listen");
        exit(3);
      }
    }
  }
};

#endif /*_TCPSERVERT_H_*/
