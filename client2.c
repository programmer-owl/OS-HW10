#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const int RCVBUFSIZE = 32;

void DieWithError(char *errorMessage) {
  perror(errorMessage);
  exit(1);
}

int main(int argc, char *argv[]) {
  int socket;
  struct sockaddr_in echoServAddr;
  unsigned short echoServPort;
  char *servIP;
  unsigned int echoStringLen;
  int bytesRcvd, totalBytesRcvd;

  if (argc != 3) {
    fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n", argv[0]);
    exit(1);
  }

  servIP = argv[1];
  echoServPort = atoi(argv[2]);

  if ((socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
    DieWithError("socket() failed");
  } else {
    printf("Socket created successfully.\n");
  }

  memset(&echoServAddr, 0, sizeof(echoServAddr));
  echoServAddr.sin_family = AF_INET;
  echoServAddr.sin_addr.s_addr = inet_addr(servIP);
  echoServAddr.sin_port = htons(echoServPort);

  if (connect(socket, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0) {
    DieWithError("connect() failed");
  } else {
    printf("Connected to server.\n");
  }

  while (1) {
    char echoBuffer[RCVBUFSIZE];
    totalBytesRcvd = 0;
    printf("Received: ");

    while ((bytesRcvd = recv(socket, echoBuffer, RCVBUFSIZE - 1, 0)) <= 0) {}
    echoBuffer[bytesRcvd] = '\0';
    printf("%s\n", echoBuffer);

    if (strcmp(echoBuffer, "The End") == 0) {
      break;
    }
  }

  close(socket);
  printf("Client2 finished.\n");
  exit(0);
}
