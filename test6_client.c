#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345

int main(){
    int sock;
    struct sockaddr_in serverAddr;
    struct sockaddr_in server;
    char buf[1024];

    // ソケットの作成
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    // サーバの設定
    memset(&serverAddr, 0, sizeof(serverAddr));
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    /* 127.0.0.1 は localhost */
    inet_pton(AF_INET, SERVER_IP, &server.sin_addr.s_addr); 

    // サーバへの接続
    int connect0 = connect(sock, (struct sockaddr *)&server, sizeof(server));
    if (connect0 < 0) {
        perror("connect");
        return 1;
    }

    // ユーザ名とパスワードの入力
    char username[20];
    char password[20];

    printf("Enter your username: ");
    scanf("%s", username);
    printf("Enter your password: ");
    scanf("%s", password);

    // ユーザ名とパスワードをサーバに送信
    write(sock, username, sizeof(username));
    write(sock, password, sizeof(password));

    // サーバからの認証結果を受け取る
    char authResult[100];
    read(sock, authResult, sizeof(authResult));
    printf("%s\n", authResult);

    if (strcmp(authResult, "Authentication failed.") == 0) {
        // 認証失敗時の処理
        printf("Sorry. Try again!\n");
        close(sock);
        return 1;
    }

    int guess;

    
    printf("Guess a number between 1 and 9.\n");
    while(1){
        printf("\nEnter your guess-> ");
        scanf("%d", &guess);

        // 推測をサーバに送信
        int len = snprintf(buf, sizeof(buf), "%d", guess);
        write(sock, buf, len);

        // サーバからの結果を受信
        int bytesRead = read(sock, buf, sizeof(buf) - 1);
        buf[bytesRead] = '\0';  // 受信したデータを文字列に変換
        printf("%s", buf);

        // 正解した場合はゲーム終了
        if(strstr(buf, "Congratulations! You're guessing it!")){
            break;
        }
    }

    close(sock);

    return 0;
}
