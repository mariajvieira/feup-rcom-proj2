#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define FTP_PORT 21

// sair do programa em caso de erro
void handle_error(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

// infos modo passivo
void extract_pasv_info(const char *response, char *ip, int *port) {
    int a, b, c, d, p1, p2;
    // IP e porta da resposta PASV
    if (sscanf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &a, &b, &c, &d, &p1, &p2) == 6) {
        sprintf(ip, "%d.%d.%d.%d", a, b, c, d);
        *port = p1 * 256 + p2;
    } else {
        fprintf(stderr, "Invalid PASV response format.\n");
        exit(EXIT_FAILURE);
    }
}

//  analisar a URL FTP e extrair a senha,user,host e path
void parse_ftp_url(const char *url, char *host, char *user, char *pass, char *path) {
    // utilizador e senha
    if (sscanf(url, "ftp://%99[^:]:%99[^@]@%99[^/]/%s", user, pass, host, path) == 4) return;
    // utilizador anónimo sem senha
    if (sscanf(url, "ftp://anonymous@%99[^/]/%s", host, path) == 2) {
        strcpy(user, "anonymous");
        strcpy(pass, "anonymous");
        return;
    }
    // apenas o hostname e path - sem utilizador e senha
    if (sscanf(url, "ftp://%99[^/]/%s", host, path) == 2) {
        strcpy(user, "anonymous");
        strcpy(pass, "anonymous");
        return;
    }
    fprintf(stderr, "URL format error\n");
    exit(EXIT_FAILURE);
}

// conectar ao servidor FTP
int connect_to_ftp_server(const char *hostname) {
    struct hostent *server;
    struct sockaddr_in server_addr;
    int sockfd;

    // checka hostname para obter o endereço IP do servidor
    if ((server = gethostbyname(hostname)) == NULL) {
        herror("gethostbyname");
        exit(EXIT_FAILURE);
    }

    // Cria o socket para a conexão de controle
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        handle_error("socket");
    }

    // Configura a estrutura de endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(FTP_PORT);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Conexão ao FTP
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        handle_error("connect");
    }

    return sockfd;
}

// enviar comandos FTP
void send_ftp_command(int sockfd, const char *command, const char *param) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s %s\r\n", command, param ? param : "");
    if (write(sockfd, buffer, strlen(buffer)) < 0) {
        handle_error("write");
    }
}

//  ler a resposta do servidor FTP
void read_ftp_response(int sockfd, char *buffer) {
    ssize_t bytes_read = read(sockfd, buffer, BUFFER_SIZE - 1);
    if (bytes_read < 0) {
        handle_error("read");
    }
    buffer[bytes_read] = '\0';
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <ftp_url>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char host[BUFFER_SIZE], user[BUFFER_SIZE], pass[BUFFER_SIZE], path[BUFFER_SIZE];
    char ip[BUFFER_SIZE], buffer[BUFFER_SIZE];
    int control_sock, data_sock, port;

    parse_ftp_url(argv[1], host, user, pass, path);
    printf("FTP server: %s\n", host);

    // Conexão ao FTP
    control_sock = connect_to_ftp_server(host);

    // Lê a resposta inicial do servidor
    read_ftp_response(control_sock, buffer);
    printf("Server: %s", buffer);

    // manda user
    send_ftp_command(control_sock, "USER", user);
    read_ftp_response(control_sock, buffer);
    printf("USER Response: %s", buffer);

    // manda pass
    send_ftp_command(control_sock, "PASS", pass);
    read_ftp_response(control_sock, buffer);
    printf("PASS Response: %s", buffer);

    // Entra no modo passivo (PASV)
    send_ftp_command(control_sock, "PASV", NULL);
    read_ftp_response(control_sock, buffer);
    printf("PASV Response: %s", buffer);

    // info do modo passivo
    extract_pasv_info(buffer, ip, &port);
    printf("Passive Mode IP: %s, Port: %d\n", ip, port);

    // Configura a conexão de dados
    struct sockaddr_in data_addr;
    data_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (data_sock < 0) {
        handle_error("data socket");
    }

    memset(&data_addr, 0, sizeof(data_addr));
    data_addr.sin_family = AF_INET;
    data_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &data_addr.sin_addr) <= 0) {
        handle_error("inet_pton");
    }

    // Conexão da socket de dados
    if (connect(data_sock, (struct sockaddr *)&data_addr, sizeof(data_addr)) < 0) {
        handle_error("data connect");
    }

    // Envia o comando RETR para baixar o arquivo
    send_ftp_command(control_sock, "RETR", path);
    read_ftp_response(control_sock, buffer);
    printf("RETR Response: %s", buffer);

    // Ajusta o nome do arquivo para salvar
    const char *output_filename = strrchr(path, '/');
    output_filename = output_filename ? output_filename + 1 : path;

    FILE *file = fopen(output_filename, "wb");
    if (!file) {
        fprintf(stderr, "Error opening file '%s': %s\n", output_filename, strerror(errno));
        exit(EXIT_FAILURE);
    }

    // ler socket e escrever no arquivo
    int bytes_read;
    while ((bytes_read = read(data_sock, buffer, BUFFER_SIZE)) > 0) {
        fwrite(buffer, 1, bytes_read, file);
    }

    fclose(file);
    close(data_sock);

    read_ftp_response(control_sock, buffer);
    printf("Final Response: %s", buffer);

    close(control_sock);
    printf("Download complete!\n");

    return 0;
}