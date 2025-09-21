#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <float.h>
#include <math.h>

int server_socket = -1;
int client_socket = -1;

// Controle de encerramento
void handle_sigint(int sig) {
    (void)sig;
    printf("\nEncerrando servidor...\n");

    if (client_socket >= 0) {
        close(client_socket);
        client_socket = -1;
    }
    if (server_socket >= 0) {
        close(server_socket);
        server_socket = -1;
    }

    exit(0); // encerra de forma limpa
}

// Função para parsing
int parse_double(const char *s, double *out) { 
    char *endptr;
    errno = 0;
    double val = strtod(s, &endptr);

    if (errno != 0 || endptr == s || *endptr != '\0') {
        return 0; // inválido
    }
    if (val < -DBL_MAX || val > DBL_MAX) {
        return 0; // fora do range
    }

    *out = val;
    return 1;
}

// Remove o "." quando for inteiro
void format_number(double val, char *buf, size_t size) {
    if (fabs(val - round(val)) < 1e-9) {
        snprintf(buf, size, "%d", (int)round(val));
    } else {
        // imprime como decimal
        snprintf(buf, size, "%.6f", val);

        // remover zeros à direita
        char *p = buf + strlen(buf) - 1;
        while (p > buf && *p == '0') {
            *p-- = '\0';
        }
        if (*p == '.') {
            *p = '\0'; // remove ponto final se sobrar
        }
    }
}


int main() {

    signal(SIGINT, handle_sigint);

    // criação do socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    // endereço do servidor
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(5050); // porta fixa
    server_address.sin_addr.s_addr = INADDR_ANY; // todas as interfaces

    // bind
    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("bind");
        return 1;
    }

    // escutar
    listen(server_socket, 5);
    printf("Aguardando conexão...\n");

    // aceitar conexões
    int client_socket = accept(server_socket, NULL, NULL);
    if (client_socket < 0) {
        perror("accept");
        return 1;
    }

    char buffer[256];
    while (1) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            printf("Cliente desconectado.\n");
            break;
        }

        printf("Recebido: %s", buffer);

        // Tokenizar entrada
        char *tokens[4];
        int ntokens = 0;
        char *tok = strtok(buffer, " \t\r\n");
        while (tok && ntokens < 4) {
            tokens[ntokens++] = tok;
            tok = strtok(NULL, " \t\r\n");
        }

        char response[256];

        if (ntokens != 3) {
            snprintf(response, sizeof(response), "ERR EINV Entrada inválida (esperado: CMD A B)\n");
        } else {
            double a, b, result;
            char *cmd = tokens[0];
            char *op   = tokens[1];
            char *arg2 = tokens[2];
            char numbuf[64]; // buffer para resultado formatado
            
            // caso padrão
            if ((strcmp(cmd, "ADD") == 0 || strcmp(cmd, "SUB") == 0 ||
                 strcmp(cmd, "MUL") == 0 || strcmp(cmd, "DIV") == 0)) {

                if (!parse_double(tokens[1], &a) || !parse_double(tokens[2], &b)) {
                    snprintf(response, sizeof(response), "ERR EINV Argumento(s) não numérico(s)\n");
                } else if (strcmp(cmd, "ADD") == 0) {
                    result = a + b;
                    format_number(result, numbuf, sizeof(numbuf));
                    snprintf(response, sizeof(response), "OK %s\n", numbuf);
                } else if (strcmp(cmd, "SUB") == 0) {
                    result = a - b;
                    format_number(result, numbuf, sizeof(numbuf));
                    snprintf(response, sizeof(response), "OK %s\n", numbuf);
                } else if (strcmp(cmd, "MUL") == 0) {
                    result = a * b;
                    format_number(result, numbuf, sizeof(numbuf));
                    snprintf(response, sizeof(response), "OK %s\n", numbuf);
                } else if (strcmp(cmd, "DIV") == 0) {
                    if (b == 0.0) {
                        snprintf(response, sizeof(response), "ERR EZDV divisao_por_zero\n");
                    } else {
                        result = a / b;
                        format_number(result, numbuf, sizeof(numbuf));
                        snprintf(response, sizeof(response), "OK %s\n", numbuf);
                    }
                }
            }
            // caso infixo
            else {
                if (!parse_double(cmd, &a) || !parse_double(arg2, &b)) {
                    snprintf(response, sizeof(response), "ERR EINV Argumento(s) não numérico(s)\n");
                } else if (strcmp(op, "+") == 0) {
                    result = a + b;
                    format_number(result, numbuf, sizeof(numbuf));
                    snprintf(response, sizeof(response), "OK %s\n", numbuf);
                } else if (strcmp(op, "-") == 0) {
                    result = a - b;
                    format_number(result, numbuf, sizeof(numbuf));
                    snprintf(response, sizeof(response), "OK %s\n", numbuf);
                } else if (strcmp(op, "*") == 0) {
                    result = a * b;
                    format_number(result, numbuf, sizeof(numbuf));
                    snprintf(response, sizeof(response), "OK %s\n", numbuf);
                } else if (strcmp(op, "/") == 0) {
                    if (b == 0.0) {
                        snprintf(response, sizeof(response), "ERR EZDV divisao_por_zero\n");
                    } else {
                        result = a / b;
                        format_number(result, numbuf, sizeof(numbuf));
                        snprintf(response, sizeof(response), "OK %s\n", numbuf);
                    }
                } else {
                    snprintf(response, sizeof(response), "ERR EINV Operador desconhecido\n");
                }
            }
        }

        // enviar resposta
        send(client_socket, response, strlen(response), 0);
    }

    handle_sigint(0); // fecha limpo ao sair do loop

    return 0;
}
