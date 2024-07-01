#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define STAGE1 "172.29.144.28"
#define STAGE2 "172.29.144.29"

#define MIN_PORT 29000
#define MAX_PORT 31000
#define MAX_BUF 1024

int main(int argc, char *argv[])
{
  struct sockaddr_in saddr;
  int fd;
  char buf[MAX_BUF];
  int port;
  int found = 0;
  const char *ipaddr;

  if (argc != 2)
  {
    fprintf(stderr, "Usage: %s STAGE1|STAGE2\n", argv[0]);
    exit(1);
  }
  if (strcmp(argv[1], "STAGE1") == 0)
  {
    ipaddr = STAGE1;
  }
  else if (strcmp(argv[1], "STAGE2") == 0)
  {
    ipaddr = STAGE2;
  }
  else
  {
    fprintf(stderr, "Invalid argument. Use STAGE1 or STAGE2.\n");
    exit(1);
  }

  for (port = MIN_PORT; port <= MAX_PORT; port++)
  {
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
      perror("socket");
      exit(1);
    }

    memset((char *)&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr(ipaddr);
    saddr.sin_port = htons(port);

    if (connect(fd, (struct sockaddr *)&saddr, sizeof(saddr)) == 0)
    {
      // Connected successfully
      printf("Connected to port %d\n", port);

      // Send STAT command to check if it's the correct port
      snprintf(buf, sizeof(buf), "STAT");
      write(fd, buf, strlen(buf) + 1);
      fsync(fd);

      int read_bytes = read(fd, buf, MAX_BUF);
      if (read_bytes > 0)
      {
        buf[read_bytes] = '\0';
        printf("Response from port %d: %s\n", port, buf);
        if (strstr(buf, "SKY: IEXP1-2410-M1 Server ver. 1.52") != NULL)
        {
          found = 1;
          break;
        }
      }

      close(fd);
    }
  }

  if (found)
  {
    printf("\x1b[42mfound!!!!! PORT:%d\n", port);
  }
  else
  {
    printf("SKY-1.52 server not found in the port range %d-%d\n", MIN_PORT, MAX_PORT);
  }

  return 0;
}
