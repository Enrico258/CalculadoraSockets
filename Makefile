CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lm

SRC_DIR = src
BIN_DIR = bin

CLIENT_SRC = $(SRC_DIR)/client_math.c
SERVER_SRC = $(SRC_DIR)/server_math.c
CLIENT_BIN = $(BIN_DIR)/client_math
SERVER_BIN = $(BIN_DIR)/server_math

all: $(CLIENT_BIN) $(SERVER_BIN)

$(CLIENT_BIN): $(CLIENT_SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

$(SERVER_BIN): $(SERVER_SRC)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

run-server: $(SERVER_BIN)
	@echo ">> Iniciando servidor na porta 5050..."
	@$(SERVER_BIN)

run-client: $(CLIENT_BIN)
	@echo ">> Iniciando cliente (conectando em 127.0.0.1:5050)..."
	@$(CLIENT_BIN) 127.0.0.1 5050

clean:
	rm -f $(BIN_DIR)/*
