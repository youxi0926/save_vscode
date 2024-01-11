#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 12345

// ユーザ情報
typedef struct {
    char name[20];
    char password[20];
} UserInfo;

// 登録されたユーザ情報
UserInfo registeredUsers[] = {
    {"Mike", "mike926"},
    {"Alex", "alex210"},
    {"Dazy", "dazy1023"},
    {"Sunny", "sunny111"}
};

// ユーザ認証関数
int authenticateUser(char *name, char *password) {
    for (int i = 0; i < sizeof(registeredUsers) / sizeof(UserInfo); ++i) {
        if (strcmp(name, registeredUsers[i].name) == 0 && strcmp(password, registeredUsers[i].password) == 0) {
            return 0; // 認証成功
        }
    }
    return -1; // 認証失敗
}

int main(){
    int ssocket, csocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);

    // ソケットの作成
    ssocket = socket(AF_INET, SOCK_STREAM, 0);
    if(ssocket < 0){
        perror("socket");
        return 1;
    }

    // サーバの設定
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    int bind0 = bind(ssocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if(bind0 < 0){
        perror("bind");
        return 1;
    }

    int listen0 = listen(ssocket, 5);
    if (listen0 < 0) {
        perror("listen");
        return 1;
    }

    printf("Waiting for connections...\n");

    // クライアントからの接続待ち
    csocket = accept(ssocket, (struct sockaddr *)&clientAddr, &addrLen);
    if(csocket < 0){
        perror("accept");
        return 1;
    }

    // クライアントからユーザ名とパスワードを受け取る
    char username[20];
    char userpassword[20];

    read(csocket, username, sizeof(username));
    read(csocket, userpassword, sizeof(userpassword));

    // ユーザ認証
    if (authenticateUser(username, userpassword) == 0) {
        // 認証成功
        write(csocket, "Authentication Successful.", 27);
        printf("Connection established with client %s.\n", username);

        srand(time(NULL));

        int randomNumber = rand() % 9 + 1;
        int guess;
        char buf[1024];

        while(1){
        // クライアントからの推測を受け取る
        read(csocket, buf, 1024);
        sscanf(buf, "%d", &guess);

        // 推測の検証と結果の送信
        if(guess == randomNumber){
            snprintf(buf, 1024, "Congratulations! You're guessing it!\n");
            write(csocket, buf, strlen(buf));
            break;
        }else{
            snprintf(buf, 1024, " x Wrong guess... Try again!\n");
            write(csocket, buf, strlen(buf));
        }
    }
    } else {
        // 認証失敗
        write(csocket, "Authentication failed.", 23);
        printf("Authentication failed for client.\n");
        close(csocket);
    }

    // ソケットのクローズ
    close(csocket);
    close(ssocket);

    return 0;
}
