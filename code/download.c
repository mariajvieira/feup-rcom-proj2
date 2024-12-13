#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define FTP_PORT 21
#define BUFFER_SIZE 1024

typedef struct {
    char user[BUFFER_SIZE];
    char password[BUFFER_SIZE];
    char host[BUFFER_SIZE];
    char path[BUFFER_SIZE];
} FTP_URL;

void handle_error(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

int parse_ftp_url(const char *url, FTP_URL *ftp) {
    // Exemplo: ftp://user:pass@host/path
    const char *prefix = "ftp://";
    if (strncmp(url, prefix, strlen(prefix)) != 0) {
        fprintf(stderr, "Invalid URL format. Must start with ftp://\n");
        return -1;
    }

    const char *start = url + strlen(prefix);
    const char *at = strchr(start, '@');
    const char *slash = strchr(at ? at : start, '/');

    if (at) {
        sscanf(start, "%[^:]:%[^@]@", ftp->user, ftp->password);
    } else {
        strcpy(ftp->user, "anonymous");
        strcpy(ftp->password, "anonymous");
    }

    if (slash) {
        sscanf(at ? at + 1 : start, "%[^/]/%s", ftp->host, ftp->path);
    } else {
        fprintf(stderr, "Invalid URL format. Missing path.\n");
        return -1;
    }

    return 0;
}

int connect_to_server(const char *hostname, int port) {
    struct hostent *server;
    struct sockaddr_in server_addr;
    int sockfd;

    // Resolver hostname
    if ((server = gethostbyname(hostname)) == NULL) {
        herror("gethostbyname");
        exit(EXIT_FAILURE);
    }

    // Criar socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        handle_error("socket");
    }

    // Configurar endereço do servidor
    bzero((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);

    // Conectar ao servidor
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        handle_error("connect");
    }

    return sockfd;
}

void send_command(int sockfd, const char *command, const char *param) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s %s\r\n", command, param ? param : "");
    if (write(sockfd, buffer, strlen(buffer)) < 0) {
        handle_error("write");
    }
}

void read_response(int sockfd) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read = read(sockfd, buffer, sizeof(buffer) - 1);
    if (bytes_read < 0) {
        handle_error("read");
    }
    buffer[bytes_read] = '\0';
    printf("Server Response: %s\n", buffer);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ftp-url>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    FTP_URL ftp;
    if (parse_ftp_url(argv[1], &ftp) < 0) {
        fprintf(stderr, "Failed to parse URL.\n");
        exit(EXIT_FAILURE);
    }

    printf("Parsed URL:\n");
    printf("  User: %s\n", ftp.user);
    printf("  Password: %s\n", ftp.password);
    printf("  Host: %s\n", ftp.host);
    printf("  Path: %s\n", ftp.path);

    // Conectar ao servidor FTP
    int sockfd = connect_to_server(ftp.host, FTP_PORT);

    // Ler mensagem de boas-vindas do servidor
    read_response(sockfd);

    // Login
    send_command(sockfd, "USER", ftp.user);
    read_response(sockfd);
    send_command(sockfd, "PASS", ftp.password);
    read_response(sockfd);

    // TODO: Implementar comando PASV e RETR para transferir o ficheiro

    // Fechar conexão
    send_command(sockfd, "QUIT", NULL);
    close(sockfd);

    return 0;
}
