# Calculadora com sockets

Projeto acadêmico de uma aplicação client-servidor, usando sockets TCP(IPv4), na qual o cliente envia operações matemáticas e o servidor executa e retorna o resultado. 

## Compilação e execução

Para compilar e executar o proejto, siga as diretivas abaixo.

### Compilar

```bash
make            # compila tudo
```

### Exemplo de uso

Servidor (porta padrão 5050):
```bash
make run-server
```

Cliente (porta e IP padrões - 127.0.0.1:5050):
```bash
make run-client
ADD 10 3
SUB 12.3 -4
-12.2 * 3
30.2 / 34
```
### Decisões

Decidido manter o IP e porta padrões dentro do makefile