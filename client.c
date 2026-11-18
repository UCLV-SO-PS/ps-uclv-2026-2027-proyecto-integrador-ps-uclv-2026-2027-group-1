#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 6666
#define BUFFER_SIZE 1024

int main(int argc, char **argv) {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    char code[7];
    ssize_t bytes_received;
    
    if (argc < 2) {
        printf("Uso: %s <CODIGO_EPISODIO>\n", argv[0]);
        printf("Ejemplo: %s S01E01\n", argv[0]);
        return 1;
    }
    
    strncpy(code, argv[1], 6);
    code[6] = '\0';
    
    if (strlen(code) != 6) {
        printf("Error: El código debe tener exactamente 6 caracteres\n");
        return 1;
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error al crear socket");
        return 1;
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error al conectar con el servidor");
        close(sockfd);
        return 1;
    }
    
    if (send(sockfd, code, strlen(code), 0) < 0) {
        perror("Error al enviar código");
        close(sockfd);
        return 1;
    }
    
    memset(buffer, 0, BUFFER_SIZE);
    bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received < 0) {
        perror("Error al recibir respuesta");
    } else if (bytes_received == 0) {
        printf("El servidor cerró la conexión\n");
    } else {
        buffer[bytes_received] = '\0';
        printf("%s", buffer);
    }
    
    close(sockfd);
    return 0;
}
