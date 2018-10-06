#ifndef _MYNETWORK_H_
#define _MYNETWORK_H_

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

class COutput;

class CNetwork {
 public:
  CNetwork() : mhSocket(-1) {}
  ~CNetwork() {}

  bool Connect(char *pServerAddr, unsigned short nPort) {
    // Connect to QTM RT server.
    mhSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sAddr;

    // First check if the address is a dotted number "A.B.C.D"
    if (inet_pton(AF_INET, pServerAddr, &(sAddr.sin_addr)) == NULL) {
      // If it wasn't a dotted number lookup the server name

      struct addrinfo hints, *servinfo;

      memset(&hints, 0, sizeof hints);
      hints.ai_family = AF_INET;
      hints.ai_socktype = SOCK_STREAM;

      if (getaddrinfo(pServerAddr, NULL, &hints, &servinfo) != 0) {
        perror("Error looking up host name.");
        close(mhSocket);
        return false;
      }
      if (servinfo == NULL) {
        perror("Error looking up host name.");
        close(mhSocket);
        return false;
      }
      sAddr.sin_addr = ((struct sockaddr_in *)servinfo[0].ai_addr)->sin_addr;
    }
    sAddr.sin_port = htons(nPort);
    sAddr.sin_family = AF_INET;

    if (connect(mhSocket, (struct sockaddr *)(&sAddr), sizeof(sAddr)) == -1) {
      perror("client: connect");
      close(mhSocket);
      return false;
    }

    // Disable Nagle's algorithm
    int bNoDelay = 1;

    if (setsockopt(mhSocket, IPPROTO_TCP, TCP_NODELAY, (void *)&bNoDelay,
                   sizeof(bNoDelay)) < 0) {
      perror("setsockopt failed!");
      close(mhSocket);
      return false;
    }

    return true;
  }

  void Disconnect() {
    // Try to shutdown gracefully
    shutdown(mhSocket, SD_SEND);
    close(mhSocket);
    mhSocket = -1;
  }

  bool Connected() { return mhSocket != -1; }

  // Receive a data packet. Data is stored in a local static buffer
  // Returns number of bytes in received message, 0 on timeout or -1 if there is
  // an error.
  int Receive(char *rtDataBuff, int nDataBufSize, bool bHeader, int nTimeout,
              unsigned int *ipAddr = NULL) {
    int nRecved = 0;
    struct sockaddr_in source_addr;
    int fromlen = sizeof(source_addr);

    fd_set ReadFDs, WriteFDs, ExceptFDs;
    FD_ZERO(&ReadFDs);
    FD_ZERO(&WriteFDs);
    FD_ZERO(&ExceptFDs);

    if (mhSocket != -1) {
      FD_SET(mhSocket, &ReadFDs);
      FD_SET(mhSocket, &ExceptFDs);
    }

    struct timeval *pTimeout;
    struct timeval timeout;

    if (nTimeout < 0) {
      pTimeout = NULL;
    } else {
      timeout.tv_sec = nTimeout / 1000000;
      timeout.tv_usec = nTimeout % 1000000;
      pTimeout = &timeout;
    }

    // Wait for activity on the TCP sockets.
    int nSelectRes = select(0, &ReadFDs, &WriteFDs, &ExceptFDs, pTimeout);

    if (nSelectRes == 0) {
      return 0;  // Select timeout.
    }
    if (nSelectRes > 0) {
      if (FD_ISSET(mhSocket, &ExceptFDs)) {
        // General socket error
        FD_CLR(mhSocket, &ExceptFDs);
        perror("General socket error!");
        nRecved = -1;
        Disconnect();
      } else if (FD_ISSET(mhSocket, &ReadFDs)) {
        nRecved = recv(mhSocket, rtDataBuff, bHeader ? 8 : nDataBufSize, 0);
        FD_CLR(mhSocket, &ReadFDs);
      }
    } else {
      nRecved = -1;
      Disconnect();
    }

    return nRecved;
  }  // RecvMessage

  bool Send(const char *pSendBuf, int nSize) {
    int nSent = 0;
    int nTotSent = 0;

    while (nTotSent < nSize) {
      nSent = send(mhSocket, pSendBuf + nTotSent, nSize - nTotSent, 0);
      if (nSent == -1) {
        perror("send!");
        return false;
      }
      nTotSent += nSent;
    }

    return true;
  }  // Send

 private:
  // get sockaddr, IPv4 or IPv6:
  void *get_in_addr(struct sockaddr *sa) {
    if (sa->sa_family == AF_INET) {
      return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
  }
  void SetErrorString();

 private:
  COutput *mpoOutput;
  int mhSocket;
};

#endif