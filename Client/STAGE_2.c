#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include "rmd160.h"

#define MAX_BUF 1024
#define IPADDR "172.29.144.29"

int main(int argc, char *argv[])
{

  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s <port>\n", argv[0]);
    exit(1);
  }

  int port = atoi(argv[1]);
  struct sockaddr_in saddr;
  int fd;
  char buf[MAX_BUF];
  char key[MAX_BUF];
  int key_received = 0;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("socket");
    exit(1);
  }

  memset((char *)&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = inet_addr(IPADDR);
  saddr.sin_port = htons(port);

  if (connect(fd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
  {
    perror("connect");
    exit(1);
  }
  fprintf(stderr, "Connection established: socket %d used.\n", fd);

  fd_set read_fds;
  int max_fd = fd > STDIN_FILENO ? fd : STDIN_FILENO;

  while (1)
  {
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);
    FD_SET(STDIN_FILENO, &read_fds);

    if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0)
    {
      perror("select");
      exit(1);
    }

    if (FD_ISSET(STDIN_FILENO, &read_fds))
    {
      if (fgets(buf, MAX_BUF, stdin))
      {
        if (buf[strlen(buf) - 1] == '\n')
          buf[strlen(buf) - 1] = '\0';

        if (strcmp(buf, "QUIT") == 0)
        {
          break;
        }

        write(fd, buf, strlen(buf) + 1);
        fsync(fd);
      }
    }

    if (FD_ISSET(fd, &read_fds))
    {
      int n = read(fd, buf, MAX_BUF);
      if (n > 0)
      {
        buf[n] = '\0';
        fprintf(stdout, "%s\n", buf);
        // プロトコルごとの処理を記載
      }
      else if (n == 0)
      {
        fprintf(stderr, "Server closed connection\n");
        break;
      }
      else
      {
        perror("read");
        exit(1);
      }
    }
  }

  close(fd);
  return 0;
}
