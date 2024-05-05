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
  char echoBuffer[RCVBUFSIZE];
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
  }

  memset(&echoServAddr, 0, sizeof(echoServAddr));
  echoServAddr.sin_family = AF_INET;
  echoServAddr.sin_addr.s_addr = inet_addr(servIP);
  echoServAddr.sin_port = htons(echoServPort);

  if (connect(socket, (struct sockaddr *)&echoServAddr, sizeof(echoServAddr)) < 0) {
    DieWithError("connect() failed");
  }

  while (1) {
    char str[100];
    printf("Enter a string to send to the server (100 symbols max): ");
    fgets(str, sizeof(str), stdin);
    str[strlen(str) - 1] = '\0';
    char *echoString = str;

    echoStringLen = strlen(echoString);

    if (send(socket, echoString, echoStringLen, 0) != echoStringLen) {
      DieWithError("send() sent a different number of bytes than expected");
    }

    printf("Successfully sent: %s\n", echoString);

    if (strcmp(str, "The End") == 0) {
      break;
    }
  }

  close(socket);
  printf("Client1 finished.\n");
  exit(0);
}
