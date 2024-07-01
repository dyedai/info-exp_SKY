#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#define MAX_BUF 1024
#define IPADDR "172.29.144.28"

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    fprintf(stderr, "使用法: %s <ポート番号>\n", argv[0]);
    exit(1);
  }

  int port = atoi(argv[1]);
  struct sockaddr_in saddr;
  int fd;
  char buf[MAX_BUF];

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("ソケットの作成に失敗しました");
    exit(1);
  }

  memset((char *)&saddr, 0, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_addr.s_addr = inet_addr(IPADDR);
  saddr.sin_port = htons(port);

  if (connect(fd, (struct sockaddr *)&saddr, sizeof(saddr)) < 0)
  {
    perror("接続に失敗しました");
    exit(1);
  }
  fprintf(stderr, "接続成功: ソケット %d を使用中。\n", fd);

  char response[MAX_BUF * 3] = {0}; // サーバーからの応答を格納するバッファ
  int total_read_bytes = 0;
  int read_bytes;

  while (1)
  {
    // キーボードからメッセージを入力してサーバーに送信
    fprintf(stdout, "送信するメッセージを入力してください (QUIT で終了): ");
    if (!fgets(buf, MAX_BUF, stdin))
    {
      fprintf(stderr, "標準入力からの読み取りエラーが発生しました。\n");
      break;
    }

    if (buf[strlen(buf) - 1] == '\n')
    {
      buf[strlen(buf) - 1] = '\0';
    }

    if (strcmp(buf, "QUIT") == 0)
    {
      break;
    }

    write(fd, buf, strlen(buf) + 1);
    fsync(fd);

    // fprintf(stderr, "送信: %s\n", buf); // デバッグ: 送信したデータを表示

    // サーバーからの応答をすべて読み取る
    memset(response, 0, sizeof(response)); // 応答バッファをクリア
    total_read_bytes = 0;

    while ((read_bytes = read(fd, buf, MAX_BUF - 1)) > 0)
    {
      buf[read_bytes] = '\0';
      strcat(response, buf);
      total_read_bytes += read_bytes;
      fprintf(stderr, "%s\n", buf); // デバッグ: 受信したデータを表示

      // // 応答の終わりをチェック (改行1つで終了)
      // if (strstr(buf, "\n") != NULL)
      // {
      //   break;
      // }

      // 応答が完全に読み込まれたか確認
      if (total_read_bytes >= sizeof(response) - 1)
      {
        break;
      }
    }

    if (total_read_bytes == 0)
    {
      fprintf(stderr, "サーバーからの応答がありませんでした。\n");
      break;
    }

    // fprintf(stdout, "完全な応答: %s\n", response);
  }

  close(fd);

  return 0;
}
