#include "../include/loop.h"
//
// void testtcpserver_sql(tcpsocketserver *t_server, mysql *t_mysql) {
//   t_server->read_fds = t_server->master; // copy it
//   if (select(t_server->fdmax + 1, &t_server->read_fds, NULL, NULL, NULL) ==
//       -1) {
//     perror("select");
//     exit(4);
//   }
//
//   // run through the existing connections looking for data to read
//   for (int i = 0; i <= t_server->fdmax; i++) {
//     if (FD_ISSET(i, &t_server->read_fds)) { // we got one!!
//       if (i == t_server->listener) {
//
//         if (t_server->numexist >= MAXCONNECTION && MAXCONNECTION) {
//           printf("We have reach the max connection!\n ");
//           t_server->addrlen = sizeof t_server->remoteaddr;
//           t_server->newfd = accept(t_server->listener,
//                                    (struct sockaddr *)&t_server->remoteaddr,
//                                    &t_server->addrlen);
//
//           // stop accept new connections, if existing connection equal to max
//           close(t_server->newfd);
//
//         } else {
//           // handle new connections, if existing connection less than maximum
//           t_server->addrlen = sizeof t_server->remoteaddr;
//           t_server->newfd = accept(t_server->listener,
//                                    (struct sockaddr *)&t_server->remoteaddr,
//                                    &t_server->addrlen);
//
//           if (t_server->newfd == -1) {
//             perror("accept");
//           } else {
//             FD_SET(t_server->newfd, &t_server->master); // add to master set
//             // keep track of the max
//             t_server->fdmax = max(t_server->fdmax, t_server->newfd);
//             ++t_server->numexist;
//             printf(
//                 "selectserver: new connection from %s on "
//                 "socket %d\n",
//                 inet_ntop(t_server->remoteaddr.ss_family,
//                           get_in_addr((struct sockaddr
//                           *)&t_server->remoteaddr), t_server->remoteIP,
//                           INET6_ADDRSTRLEN),
//                 t_server->newfd);
//           }
//         }
//
//       } else {
//         // handle data from a client
//         t_server->nbytes = recv(i, t_server->buf, sizeof t_server->buf, 0);
//         if (t_server->nbytes == 0) {
//           // connection closed
//           printf("selectserver: socket %d hung up\n", i);
//           close(i);                     // bye!
//           FD_CLR(i, &t_server->master); // remove from master set
//         } else if (t_server->nbytes < 0) {
//           perror("recv");
//           close(i);                     // bye!
//           FD_CLR(i, &t_server->master); // remove from master set
//         } else {
//           // we got some data from a client
//           for (int j = 0; j <= t_server->fdmax; j++) {
//             // send to everyone!
//             if (FD_ISSET(j, &t_server->master)) {
//               // except the listener and ourselves
//               if (j != t_server->listener && j != i) {
//                 if (send(j, t_server->buf, t_server->nbytes, 0) == -1) {
//                   perror("send");
//                 }
//               }
//
//               //  send message to every listener including ourselves
//               // if (j != t_server->listener) {
//               //   if (send(j, t_server->buf, t_server->nbytes, 0) == -1) {
//               //     perror("send");
//               //   }
//               // }
//             }
//           }
//         }
//       } // END handle data from client
//     }   // END got new incoming connection
//   }     // END looping through file descriptors
// }

int select_accept(tcpsocketserver *t_server, mysql *t_mysql) {
  t_server->read_fds = t_server->master; // copy it
  int results = 0;
  if (select(t_server->fdmax + 1, &t_server->read_fds, NULL, NULL, NULL) ==
      -1) {
    perror("select");
    exit(4);
  }

  // run through the existing connections looking for data to read
  for (int i = 0; i <= t_server->fdmax; i++) {
    if (FD_ISSET(i, &t_server->read_fds)) { // we got one!!
      if (i == t_server->listener) {

        // handle new connections, if existing connection less than maximum
        t_server->addrlen = sizeof t_server->remoteaddr;
        t_server->newfd =
            accept(t_server->listener, (struct sockaddr *)&t_server->remoteaddr,
                   &t_server->addrlen);

        if (t_server->newfd == -1) {
          perror("accept");
        } else {
          FD_SET(t_server->newfd, &t_server->master); // add to master set
          // keep track of the max
          t_server->fdmax = MAX(t_server->fdmax, t_server->newfd);
          ++t_server->numexist;
          // create master table
          update_mastertable(t_mysql, t_server->newfd);
          create_client_table(t_mysql, t_server->newfd);
          printf(
              "selectserver: new connection from %s on "
              "socket %d\n",
              inet_ntop(t_server->remoteaddr.ss_family,
                        get_in_addr((struct sockaddr *)&t_server->remoteaddr),
                        t_server->remoteIP, INET6_ADDRSTRLEN),
              t_server->newfd);

          if (t_server->numexist == MAXCONNECTION) {
            results = 1;
            break;
          }
        }
      }
    } // END got new incoming connection
  }   // END looping through file descriptors
  return results;
}

void receiveallclients(tcpsocketserver *t_server, mysql *t_mysql) {

  t_server->read_fds = t_server->master; // copy it
  if (select(t_server->fdmax + 1, &t_server->read_fds, NULL, NULL, NULL) ==
      -1) {
    perror("select");
    exit(4);
  }

  // run through the existing connections looking for data to read
  for (int i = 0; i <= t_server->fdmax; i++) {
    if (FD_ISSET(i, &t_server->read_fds)) { // we got one!!
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
      } else {                        // we got some data from a client

        if (verify_crc16_checksum(t_server->buf, MAXDATASIZE)) {
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

          // save to sqlite
          double testfloat[CLIENT_DATA_SIZE] = {};
          unpackfloat32array(t_server->buf, testfloat, CLIENT_DATA_SIZE);
          update_client_table(t_mysql, i, false, testfloat);
        } else {
          // send message to clients only
          if (send(i, t_server->buf, t_server->nbytes, 0) == -1) {
            perror("send");
          }
          update_client_table(t_mysql, i, true, NULL);
        }
      }
    } // END got new incoming connection
  }   // END looping through file descriptors
}
