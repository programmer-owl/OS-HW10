#include <stdio.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const int RCVBUFSIZE = 512;
const int MAXPENDING = 3;

void DieWithError(char *errorMessage) {
  perror(errorMessage);
  exit(1);
}

int main(int argc, char *argv[]) {
  int servSock, servSockSend, clntSock, clntSockSend;
  char *serverIP;
  struct sockaddr_in echoServAddr;
  struct sockaddr_in echoServAddrSend;
  struct sockaddr_in echoClntAddr;
  struct sockaddr_in echoClntAddrSend;
  unsigned short echoServPort, echoServPortSend;
  unsigned int clntLen, clntLenSend;

  if (argc != 4) {
    fprintf(stderr, "Usage:  %s <IP-address> <Send Port> <Receive Port>\n", argv[0]);
    exit(1);
  }

  serverIP = argv[1];
  echoServPortSend = atoi(argv[2]);
  echoServPort = atoi(argv[3]);

  if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    DieWithError("socket() failed");
  }

  memset(&echoServAddr, 0, sizeof(echoServAddr));
  echoServAddr.sin_family = AF_INET;
  echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  echoServAddr.sin_port = htons(echoServPort);

  if (bind(servSock, (struct sockaddr*)&echoServAddr, sizeof(echoServAddr)) < 0) {
    DieWithError("bind() failed");
  }

  if ((servSockSend = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    DieWithError("socket() failed");
  }

  memset(&echoServAddrSend, 0, sizeof(echoServAddrSend));
  echoServAddrSend.sin_family = AF_INET;
  echoServAddrSend.sin_addr.s_addr = htonl(INADDR_ANY);
  echoServAddrSend.sin_port = htons(echoServPortSend);

  if (bind(servSockSend, (struct sockaddr *)&echoServAddrSend, sizeof(echoServAddrSend)) < 0) {
    DieWithError("bind() failed");
  }

  printf("Server IP-address: %s.\n", inet_ntoa(echoServAddr.sin_addr));

  if (listen(servSock, MAXPENDING) < 0) {
    DieWithError("listen() on receiving port failed");
  }

  if (listen(servSockSend, MAXPENDING) < 0) {
    DieWithError("listen() on sending port failed");
  }

  if ((clntSock = accept(servSock, (struct sockaddr *)&echoClntAddr, &clntLen)) < 0) {
    DieWithError("accept() failed");
  }
  printf("Handling client to receive a message: %s\n",
         inet_ntoa(echoClntAddr.sin_addr));

  if ((clntSockSend = accept(servSockSend, (struct sockaddr *)&echoClntAddrSend, &clntLenSend)) < 0) {
    DieWithError("accept() failed");
  }
  printf("Handling client to send a message: %s\n", inet_ntoa(echoClntAddrSend.sin_addr));

  while (1) {
    char echoBuffer[RCVBUFSIZE];
    char echoBufferSend[RCVBUFSIZE];
    memset(echoBuffer, 0, sizeof(echoBuffer));
    memset(echoBufferSend, 0, sizeof(echoBuffer));

    printf("Awaiting a message.\n");

    int recvMsgSize;
    if ((recvMsgSize = recv(clntSock, echoBuffer, RCVBUFSIZE, 0)) < 0) {
      DieWithError("Failed to receive message.");
    }

    while (recvMsgSize <= 0) {
      if ((recvMsgSize = recv(clntSock, echoBuffer, RCVBUFSIZE, 0)) < 0) {
        DieWithError("Failed to receive message.");
      }
    }

    echoBuffer[recvMsgSize] = '\0';
    printf("Successfully received: %s\n", echoBuffer);

    for (size_t i = 0; i < strlen(echoBuffer); i++) {
      echoBufferSend[i] = echoBuffer[i];
    }

    if ((recvMsgSize = send(clntSockSend, echoBufferSend, strlen(echoBufferSend), 0)) < 0) {
      DieWithError("send() failed");
    }
    if (strcmp(echoBufferSend, "") == 0) {
      DieWithError("send() failed");
    }

    while (recvMsgSize > 0) {
      if (send(clntSockSend, echoBufferSend, recvMsgSize, 0) != recvMsgSize) {
        DieWithError("send() failed");
      }
    }
    printf("Successfully sent: %s\n", echoBufferSend);

    if (strcmp(echoBuffer, "The End") == 0) {
      break;
    }
  }

  close(servSock);
  close(clntSock);
  close(clntSockSend);
  printf("Server finished.\n");
}
