#ifndef QTMNETWORK_H
#define QTMNETWORK_H

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

#include <sys/select.h>
#include <unistd.h>

class COutput;

class CNetwork {
 public:
  CNetwork();
  ~CNetwork();
  bool Connect(char* pServerAddr, unsigned short nPort);
  void Disconnect();
  bool Connected();
  // int Receive(char* rtDataBuff, int nDataBufSize, bool bHeader, int
  // nTimeout);
  int Receive(char* rtDataBuff, int nDataBufSize, bool bHeader);
  bool Send(const char* pSendBuf, int nSize);
  char* GetErrorString();
  int GetError();

 private:
  void SetErrorString();

 private:
  COutput* mpoOutput;
  int mhSocket;
  char maErrorStr[256];
  unsigned long mnLastError;
};

#endif