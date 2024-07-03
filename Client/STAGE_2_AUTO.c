#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h> // 追加

#define MAX_BUF 1024
#define IPADDR "172.29.144.29"

// プロトコル処理関数の宣言
void handle_protocol_B(int fd, const char *sequence);
void handle_protocol_0(int fd);
void handle_protocol_1(int fd);
void handle_protocol_2(int fd);
void handle_protocol_3(int fd);
void handle_protocol_C(int fd);

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
  int n;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("socket");
    exit(1);
  }

  // TCP_NODELAY オプションを設定
  int flag = 1;
  if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int)) == -1)
  {
    perror("setsockopt TCP_NODELAY");
    exit(1);
  }

  // TCP_CORK オプションをオフに設定
  flag = 0;
  if (setsockopt(fd, IPPROTO_TCP, TCP_CORK, (char *)&flag, sizeof(int)) == -1)
  {
    perror("setsockopt TCP_CORK");
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

  // プロトコルバージョンの受信
  n = read(fd, buf, MAX_BUF);
  if (n <= 0)
  {
    perror("read");
    exit(1);
  }
  buf[n] = '\0';
  fprintf(stdout, "Server response: %s\n", buf);

  // ログイン認証
  snprintf(buf, MAX_BUF, "LOGIN BP22043 8666885298");
  write(fd, buf, strlen(buf) + 1);

  // 認証確認メッセージの受信
  n = read(fd, buf, MAX_BUF);
  if (n <= 0)
  {
    perror("read");
    exit(1);
  }
  buf[n] = '\0';
  fprintf(stdout, "Server response: %s\n", buf);

  if (strcmp(buf, "Welcome BP22043, you are correctly authenticated.") != 0)
  {
    fprintf(stderr, "Login failed.\n");
    exit(1);
  }

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

    if (FD_ISSET(fd, &read_fds))
    {
      n = read(fd, buf, MAX_BUF);
      if (n <= 0)
      {
        perror("read");
        exit(1);
      }
      buf[n] = '\0';

      // プロトコル文字列を抽出
      char protocol_b_sequence[6];
      strncpy(protocol_b_sequence, buf + strlen("Your protocol is: "), 5);
      protocol_b_sequence[5] = '\0';
      fprintf(stdout, "Your protocol is: %s\n", protocol_b_sequence);

      // 承認
      snprintf(buf, MAX_BUF, "OK");
      write(fd, buf, strlen(buf) + 1);

      // プロトコル B の処理
      handle_protocol_B(fd, protocol_b_sequence);

      // プロトコル C の処理
      handle_protocol_C(fd);

      close(fd);
      return 0;
    }
  }
}

void handle_protocol_B(int fd, const char *sequence)
{
  for (int i = 0; i < 5; ++i)
  {
    switch (sequence[i])
    {
    case '0':
      handle_protocol_0(fd);
      break;
    case '1':
      handle_protocol_1(fd);
      break;
    case '2':
      handle_protocol_2(fd);
      break;
    case '3':
      handle_protocol_3(fd);
      break;
    default:
      fprintf(stderr, "Unknown protocol %c\n", sequence[i]);
      exit(1);
    }
  }
}

void handle_protocol_0(int fd)
{
  char buf[MAX_BUF];
  int n, x, result;

  // 数値 x
  n = read(fd, buf, MAX_BUF);
  if (n <= 0)
  {
    perror("read x for proto 0");
    exit(1);
  }
  buf[n] = '\0';
  sscanf(buf, "%d", &x);
  fprintf(stdout, "Protocol 0: x = %d\n", x);

  // 計算結果 f(x)
  result = x;
  snprintf(buf, MAX_BUF, "ANSWER %04x", result);
  write(fd, buf, strlen(buf) + 1);

  // 遷移メッセージ
  n = read(fd, buf, MAX_BUF);
  if (n <= 0)
  {
    perror("read transition message for proto 0");
    exit(1);
  }
  buf[n] = '\0';
  fprintf(stdout, "Transition message: %s\n", buf);

  sleep(1);
  // 承認
  snprintf(buf, MAX_BUF, "OK");
  write(fd, buf, strlen(buf) + 1);
}

void handle_protocol_1(int fd)
{
  char buf[MAX_BUF];
  int n, x, result;

  // 数値 x
  n = read(fd, buf, MAX_BUF);
  if (n <= 0)
  {
    perror("read x for proto 1");
    exit(1);
  }
  buf[n] = '\0';
  sscanf(buf, "%d", &x);
  fprintf(stdout, "Protocol 1: x = %d\n", x);

  // 計算結果 f(x)
  result = 2 * x * x * x - 3 * x * x - 5 * x + 4;
  snprintf(buf, MAX_BUF, "ANSWER %04x", result);
  write(fd, buf, strlen(buf) + 1);

  // 遷移メッセージ
  n = read(fd, buf, MAX_BUF);
  if (n <= 0)
  {
    perror("read transition message for proto 1");
    exit(1);
  }
  buf[n] = '\0';
  fprintf(stdout, "Transition message: %s\n", buf);

  // 承認
  snprintf(buf, MAX_BUF, "OK");
  write(fd, buf, strlen(buf) + 1);
  usleep(10000); // 少し間を置いてから2回目を送信
  snprintf(buf, MAX_BUF, "OK");
  write(fd, buf, strlen(buf) + 1);
}

void handle_protocol_2(int fd)
{
  char buf[MAX_BUF];
  int n, x, y, result;

  usleep(10000);

  // 通信開始要求
  snprintf(buf, MAX_BUF, "ENTER PROTO2");
  write(fd, buf, strlen(buf) + 1);

  // 数値 x
  n = read(fd, buf, MAX_BUF);
  if (n <= 0)
  {
    perror("read x for proto 2");
    exit(1);
  }
  buf[n] = '\0';
  sscanf(buf, "%d", &x);
  fprintf(stdout, "Protocol 2: x = %d\n", x);

  // 数値 y
  n = read(fd, buf, MAX_BUF);
  if (n <= 0)
  {
    perror("read y for proto 2");
    exit(1);
  }
  buf[n] = '\0';
  sscanf(buf, "%d", &y);
  fprintf(stdout, "Protocol 2: y = %d\n", y);

  // 計算結果 f(x, y)
  result = x * x + 2 * x * y - y * y;
  snprintf(buf, MAX_BUF, "ANSWER %04x", result);
  write(fd, buf, strlen(buf) + 1);

  // 遷移メッセージ
  n = read(fd, buf, MAX_BUF);
  if (n <= 0)
  {
    perror("read transition message for proto 2");
    exit(1);
  }
  buf[n] = '\0';
  fprintf(stdout, "Transition message: %s\n", buf);

  // 承認
  snprintf(buf, MAX_BUF, "OK");
  write(fd, buf, strlen(buf) + 1);
}

void handle_protocol_3(int fd)
{
  char buf[MAX_BUF];
  int n, x, y, result;

  usleep(10000);

  // 通信開始要求 1
  snprintf(buf, MAX_BUF, "SKY IS LIMIT");
  write(fd, buf, strlen(buf) + 1);

  // 通信開始要求 2
  snprintf(buf, MAX_BUF, "ENTER PROTO3");
  write(fd, buf, strlen(buf) + 1);

  // 数値 x
  n = read(fd, buf, MAX_BUF);
  if (n <= 0)
  {
    perror("read x for proto 3");
    exit(1);
  }
  buf[n] = '\0';
  sscanf(buf, "%d", &x);
  fprintf(stdout, "Protocol 3: x = %d\n", x);

  // 数値 y
  n = read(fd, buf, MAX_BUF);
  if (n <= 0)
  {
    perror("read y for proto 3");
    exit(1);
  }
  buf[n] = '\0';
  sscanf(buf, "%d", &y);
  fprintf(stdout, "Protocol 3: y = %d\n", y);

  // 計算結果 f(x, y)
  result = x > y ? x : y;
  snprintf(buf, MAX_BUF, "ANSWER %04x", result);
  write(fd, buf, strlen(buf) + 1);

  // 遷移メッセージ
  n = read(fd, buf, MAX_BUF);
  if (n <= 0)
  {
    perror("read transition message for proto 3");
    exit(1);
  }
  buf[n] = '\0';
  fprintf(stdout, "Transition message: %s\n", buf);

  // 承認
  snprintf(buf, MAX_BUF, "OK");
  write(fd, buf, strlen(buf) + 1);
  usleep(10000); // 少し間を置いてから2回目を送信
  snprintf(buf, MAX_BUF, "OK");
  write(fd, buf, strlen(buf) + 1);
}

void handle_protocol_C(int fd)
{
  char buf[MAX_BUF];
  int n;

  // メッセージ送信要求
  snprintf(buf, MAX_BUF, "PUT ETHERNET:DVMRP:V2X");
  write(fd, buf, strlen(buf) + 1);

  // メッセージ送信
  for (int i = 0; i < 2; ++i)
  {
    n = read(fd, buf, MAX_BUF);
    if (n <= 0)
    {
      perror("read message for proto C");
      exit(1);
    }
    buf[n] = '\0';
    fprintf(stdout, "Message %d: %s\n", i + 1, buf);
  }

  // 通信切断はサーバが行う
}
