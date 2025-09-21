#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <IP> <PORTA>\n", argv[0]);
        return 1;
    }

    // obtendo parâmetros
    char *ip = argv[1];
    int port = atoi(argv[2]);

    // criação do socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("socket");
        return -1;
    }

    // configuração do servidor
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);

    // conectar ao servidor
    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("connect");
        close(client_socket);
        return 1;
    }

    printf("Conectado ao servidor %s:%d!\n", ip, port);
    printf("Digite uma operação (ou QUIT para sair).\n\n");

    char buffer[256];
    char resposta[256];

while (1) {
    // ler comando do usuário
    printf(">> ");
    fflush(stdout); // garante que ">> " apareça antes do input

    if (!fgets(buffer, sizeof(buffer), stdin)) {
        break;
    }

    if (strncmp(buffer, "QUIT", 4) == 0) {
        break;
    }

    // enviar comando ao servidor
    if (send(client_socket, buffer, strlen(buffer), 0) < 0) {
        perror("send");
        break;
    }

    // receber resposta
    memset(resposta, 0, sizeof(resposta));
    ssize_t bytes = recv(client_socket, resposta, sizeof(resposta) - 1, 0);

    if (bytes <= 0) {
        printf("\nServidor desconectou.\n");
        break;
    }

    printf("%s", resposta);

    // --- novo trecho: se resposta começar com "ERR", encerra ---
    if (strncmp(resposta, "ERR", 3) == 0) {
        break;
    }
}


    close(client_socket);
    return 0;
}
