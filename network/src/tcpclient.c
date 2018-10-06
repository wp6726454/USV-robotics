#include "../include/tcpclient.h"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void setup_caddr_info(tcpsocketclient *t_client, char *t_server_add) {
  memset(&t_client->hints, 0, sizeof t_client->hints);
  t_client->hints.ai_family = AF_UNSPEC;
  t_client->hints.ai_socktype = SOCK_STREAM;
  if ((t_client->rv = getaddrinfo(t_server_add, PORT, &t_client->hints,
                                  &t_client->servinfo)) != 0) {
    fprintf(stderr, "getaddinfo: %s\n", gai_strerror(t_client->rv));
    exit(1); // in main function, return 1 is the same to exit(1)
  }
}

void create_socket_and_connect(tcpsocketclient *t_client) {
  // loop through all the results and connect to the first we can
  for (t_client->p = t_client->servinfo; t_client->p != NULL;
       t_client->p = (*t_client).p->ai_next) {
    if ((t_client->sockfd =
             socket((*t_client).p->ai_family, (*t_client).p->ai_socktype,
                    (*t_client).p->ai_protocol)) == -1) {
      perror("client: socket creation error");
      continue;
    }
    if (connect(t_client->sockfd, (*t_client).p->ai_addr,
                (*t_client).p->ai_addrlen) == -1) {
      perror("client: connection error");
      close(t_client->sockfd);
      continue;
    }
    break;
  }

  if (t_client->p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    exit(2);
  }
}

void get_serveradd_print(tcpsocketclient *t_client) {
  // convert the binary IP address to human-readable form
  inet_ntop((*t_client).p->ai_family,
            get_in_addr((struct sockaddr *)t_client->p->ai_addr), t_client->s,
            sizeof t_client->s);
  printf("client: connecting to %s\n", t_client->s);
  freeaddrinfo(t_client->servinfo);
}

void sr_messsages(tcpsocketclient *t_client) {

  if (send(t_client->sockfd, t_client->send_buf, MAXDATASIZE, 0) == -1)
    perror("send");

  if ((t_client->numbytes =
           recv(t_client->sockfd, t_client->recv_buf, MAXDATASIZE, 0)) == -1) {
    close(t_client->sockfd);
    perror("recv");
    exit(1);
  }
}
