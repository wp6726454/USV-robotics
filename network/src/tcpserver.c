#include "../include/tcpserver.h"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void setup_saddr_info(tcpsocketserver *t_server) {
  FD_ZERO(&t_server->master); // clear the master and temp sets
  FD_ZERO(&t_server->read_fds);
  // get us a socket and bind it
  memset(&t_server->hints, 0, sizeof t_server->hints);
  t_server->hints.ai_family = AF_UNSPEC;
  t_server->hints.ai_socktype = SOCK_STREAM;
  t_server->hints.ai_flags = AI_PASSIVE;
  t_server->numexist = 0;
  int rv = 0;
  if ((rv = getaddrinfo(NULL, PORT, &t_server->hints, &t_server->ai)) != 0) {
    fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
    exit(1);
  }
}

void open_listenfd(tcpsocketserver *t_server) {
  for (t_server->p = t_server->ai; t_server->p != NULL;
       t_server->p = (*t_server).p->ai_next) {
    t_server->listener =
        socket((*t_server).p->ai_family, (*t_server).p->ai_socktype,
               (*t_server).p->ai_protocol);
    if (t_server->listener < 0) {
      continue;
    }

    // lose the pesky "address already in use" error message
    setsockopt(t_server->listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    if (bind(t_server->listener, (*t_server).p->ai_addr,
             (*t_server).p->ai_addrlen) < 0) {
      close(t_server->listener);
      continue;
    }

    break;
  }
  // if we got here, it means we didn't get bound
  if (t_server->p == NULL) {
    fprintf(stderr, "selectserver: failed to bind\n");
    exit(2);
  }

  freeaddrinfo(t_server->ai); // all done with this

  // listen
  if (listen(t_server->listener, BACKLOG) == -1) {
    perror("listen");
    exit(3);
  }

  // add the listener to the master set
  FD_SET(t_server->listener, &t_server->master);

  // keep track of the biggest file descriptor
  t_server->fdmax = t_server->listener; // so far, it's this one
}

void select_rvmessage(tcpsocketserver *t_server) {

  t_server->read_fds = t_server->master; // copy it
  if (select(t_server->fdmax + 1, &t_server->read_fds, NULL, NULL, NULL) ==
      -1) {
    perror("select");
    exit(4);
  }

  // run through the existing connections looking for data to read
  for (int i = 0; i <= t_server->fdmax; i++) {
    if (FD_ISSET(i, &t_server->read_fds)) { // we got one!!
      if (i == t_server->listener) {

        if (t_server->numexist >= MAXCONNECTION && MAXCONNECTION) {
          printf("We have reach the max connection!\n ");
          t_server->addrlen = sizeof t_server->remoteaddr;
          t_server->newfd = accept(t_server->listener,
                                   (struct sockaddr *)&t_server->remoteaddr,
                                   &t_server->addrlen);

          // stop accept new connections, if existing connection equal to max
          close(t_server->newfd);

        } else {
          // handle new connections, if existing connection less than maximum
          t_server->addrlen = sizeof t_server->remoteaddr;
          t_server->newfd = accept(t_server->listener,
                                   (struct sockaddr *)&t_server->remoteaddr,
                                   &t_server->addrlen);

          if (t_server->newfd == -1) {
            perror("accept");
          } else {
            FD_SET(t_server->newfd, &t_server->master); // add to master set
            // keep track of the max
            t_server->fdmax = max(t_server->fdmax, t_server->newfd);
            ++t_server->numexist;
            printf(
                "selectserver: new connection from %s on "
                "socket %d\n",
                inet_ntop(t_server->remoteaddr.ss_family,
                          get_in_addr((struct sockaddr *)&t_server->remoteaddr),
                          t_server->remoteIP, INET6_ADDRSTRLEN),
                t_server->newfd);
          }
        }

      } else {
        // handle data from a client
        t_server->nbytes = recv(i, t_server->buf, sizeof t_server->buf, 0);
        if (t_server->nbytes == 0) {
          // connection closed
          printf("selectserver: socket %d hung up\n", i);
          close(i);                     // bye!
          FD_CLR(i, &t_server->master); // remove from master set
        } else if (t_server->nbytes < 0) {
          perror("recv");
          close(i);                     // bye!
          FD_CLR(i, &t_server->master); // remove from master set
        } else {
          // we got some data from a client
          for (int j = 0; j <= t_server->fdmax; j++) {
            // send to everyone!
            if (FD_ISSET(j, &t_server->master)) {
              // except the listener and ourselves
              if (j != t_server->listener && j != i) {
                if (send(j, t_server->buf, t_server->nbytes, 0) == -1) {
                  perror("send");
                }
              }

              //  send message to every listener including ourselves
              // if (j != t_server->listener) {
              //   if (send(j, t_server->buf, t_server->nbytes, 0) == -1) {
              //     perror("send");
              //   }
              // }
            }
          }
        }
      } // END handle data from client
    }   // END got new incoming connection
  }     // END looping through file descriptors
}
