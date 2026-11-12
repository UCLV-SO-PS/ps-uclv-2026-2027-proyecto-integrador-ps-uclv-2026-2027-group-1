#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include "monitor.h"

#define PORT 6666
#define BUFFER_SIZE 1024

static int server_sockfd = -1;

void sigint_handler(int sig) {
    (void)sig;
    /* Ignorar SIGINT */
}

void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0) {
        /* Cosechar hijos zombies */
    }
}

int main(int argc, char **argv) {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    int client_sockfd;
    pid_t pid;
    int pipefd[2];
    struct sigaction sa_int, sa_chld;
    char client_ip[INET_ADDRSTRLEN];
    
    /* Verificar bandera --test-run */
    if (argc >= 2 && strcmp(argv[1], "--test-run") == 0) {
        init_database();
        int test_sock = socket(AF_INET, SOCK_STREAM, 0);
        if (test_sock < 0) {
            perror("Error en test-run: socket");
            return 1;
        }
        close(test_sock);
        printf("Test run exitoso: Base de datos inicializada y socket TCP válido\n");
        return 0;
    }
    
    /* Inicializar base de datos */
    init_database();
    
    /* Configurar manejo de señales */
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    if (sigaction(SIGINT, &sa_int, NULL) < 0) {
        perror("Error al configurar sigaction para SIGINT");
        return 1;
    }
    
    sa_chld.sa_handler = sigchld_handler;
    sigemptyset(&sa_chld.sa_mask);
    sa_chld.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    if (sigaction(SIGCHLD, &sa_chld, NULL) < 0) {
        perror("Error al configurar sigaction para SIGCHLD");
        return 1;
    }
    
    /* Crear tubería sin nombre */
    if (pipe(pipefd) < 0) {
        perror("Error al crear tubería");
        return 1;
    }
    
    /* Crear socket TCP */
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sockfd < 0) {
        perror("Error al crear socket");
        return 1;
    }
    
    int opt = 1;
    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("Error en setsockopt");
        return 1;
    }
    
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    if (bind(server_sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error en bind");
        return 1;
    }
    
    if (listen(server_sockfd, 10) < 0) {
        perror("Error en listen");
        return 1;
    }
    
    printf("Servidor escuchando en puerto %d...\n", PORT);
    
    /* Loop principal */
    while (1) {
        client_len = sizeof(client_addr);
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_len);
        if (client_sockfd < 0) {
            if (errno == EINTR) {
                continue;
            }
            perror("Error en accept");
            continue;
        }
        
        /* Obtener IP del cliente */
        inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        
        /* Crear tubería para esta conexión */
        if (pipe(pipefd) < 0) {
            perror("Error al crear tubería");
            close(client_sockfd);
            continue;
        }
        
        pid = fork();
        if (pid < 0) {
            perror("Error en fork");
            close(client_sockfd);
            close(pipefd[0]);
            close(pipefd[1]);
            continue;
        } else if (pid == 0) {
            /* Proceso hijo */
            close(server_sockfd);
            close(pipefd[0]); /* Cerrar lectura en hijo */
            
            /* Enviar IP del cliente al padre */
            write(pipefd[1], client_ip, strlen(client_ip));
            write(pipefd[1], "\n", 1);
            close(pipefd[1]);
            
            /* Recibir código del cliente */
            char code_buffer[BUFFER_SIZE];
            memset(code_buffer, 0, BUFFER_SIZE);
            ssize_t bytes_received = recv(client_sockfd, code_buffer, 6, 0);
            if (bytes_received > 0) {
                code_buffer[bytes_received] = '\0';
                /* Buscar episodio */
                Episode *ep = find_episode_by_code(code_buffer);
                if (ep != NULL) {
                    char response[BUFFER_SIZE];
                    format_episode_response(ep, response, BUFFER_SIZE);
                    send(client_sockfd, response, strlen(response), 0);
                } else {
                    const char *error_msg = "Error: Episodio no encontrado\n";
                    send(client_sockfd, error_msg, strlen(error_msg), 0);
                }
            } else {
                const char *error_msg = "Error: No se recibió código válido\n";
                send(client_sockfd, error_msg, strlen(error_msg), 0);
            }
            
            close(client_sockfd);
            exit(0);
        } else {
            /* Proceso padre */
            close(client_sockfd);
            close(pipefd[1]); /* Cerrar escritura en padre */
            
            /* Leer IP del hijo */
            char ip_buffer[INET_ADDRSTRLEN + 2];
            ssize_t bytes_read = read(pipefd[0], ip_buffer, sizeof(ip_buffer) - 1);
            if (bytes_read > 0) {
                ip_buffer[bytes_read] = '\0';
                /* Remover newline si existe */
                if (ip_buffer[bytes_read - 1] == '\n') {
                    ip_buffer[bytes_read - 1] = '\0';
                }
                printf("IP %s PID %d\n", ip_buffer, pid);
            }
            close(pipefd[0]);
            
            /* Continuar aceptando conexiones */
        }
    }
    
    close(server_sockfd);
    return 0;
}
