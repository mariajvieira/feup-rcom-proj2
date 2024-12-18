# RCOM PROJECT 2

* [Part 1](#PART-1) 
* [Part 2](#PART-2)
  * [Experience 1](#Exp-1)
  * [Experience 2](#Exp-2)
  * [Experience 3](#Exp-3)
  * [Experience 4](#Exp-4)



## PART 1

**EXPLICAÇÃO DO CÓDIGO**

Este código em C implementa um cliente FTP (File Transfer Protocol) que permite baixar um arquivo de um servidor FTP, utilizando o modo passivo (PASV). Abaixo, explico cada parte em detalhes:
- Função handle_error
void handle_error(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}
Imprime uma mensagem de erro e encerra o programa com código de erro.
Usada para simplificar o tratamento de erros ao longo do código.


- Função extract_pasv_info
void extract_pasv_info(const char *response, char *ip, int *port) {
    int a, b, c, d, p1, p2;
    if (sscanf(response, "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &a, &b, &c, &d, &p1, &p2) == 6) {
        sprintf(ip, "%d.%d.%d.%d", a, b, c, d);
        *port = p1 * 256 + p2;
    } else {
        fprintf(stderr, "Invalid PASV response format.\n");
        exit(EXIT_FAILURE);
    }
}
Extrai o endereço IP e a porta do modo passivo a partir da resposta do servidor (227 Entering Passive Mode).
A resposta contém o IP e a porta no formato (a,b,c,d,p1,p2). A porta é calculada como p1 * 256 + p2.


- Função parse_ftp_url
void parse_ftp_url(const char *url, char *host, char *user, char *pass, char *path) {
    if (sscanf(url, "ftp://%99[^:]:%99[^@]@%99[^/]/%s", user, pass, host, path) == 4) return;
    if (sscanf(url, "ftp://anonymous@%99[^/]/%s", host, path) == 2) {
        strcpy(user, "anonymous");
        strcpy(pass, "anonymous");
        return;
    }
    if (sscanf(url, "ftp://%99[^/]/%s", host, path) == 2) {
        strcpy(user, "anonymous");
        strcpy(pass, "anonymous");
        return;
    }
    fprintf(stderr, "URL format error\n");
    exit(EXIT_FAILURE);
}
Analisa uma URL FTP e extrai:
Host: Nome ou endereço do servidor.
User: Nome de usuário.
Pass: Senha.
Path: Caminho do arquivo.
Suporta URLs com:
Usuário e senha explícitos (ftp://user:pass@host/path).
Usuário anônimo (ftp://anonymous@host/path).
Apenas host e caminho (ftp://host/path).


- Função connect_to_ftp_server
int connect_to_ftp_server(const char *hostname) {
    struct hostent *server;
    struct sockaddr_in server_addr;
    int sockfd;

    if ((server = gethostbyname(hostname)) == NULL) {
        herror("gethostbyname");
        exit(EXIT_FAILURE);
    }

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        handle_error("socket");
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(FTP_PORT);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        handle_error("connect");
    }

    return sockfd;
}
Conecta-se ao servidor FTP na porta 21.
Usa gethostbyname para resolver o nome do host em um endereço IP.
Cria um socket para a comunicação de controle e estabelece a conexão.



- Funções para Comandos e Respostas
void send_ftp_command(int sockfd, const char *command, const char *param) {
    char buffer[BUFFER_SIZE];
    snprintf(buffer, sizeof(buffer), "%s %s\r\n", command, param ? param : "");
    if (write(sockfd, buffer, strlen(buffer)) < 0) {
        handle_error("write");
    }
}

void read_ftp_response(int sockfd, char *buffer) {
    ssize_t bytes_read = read(sockfd, buffer, BUFFER_SIZE - 1);
    if (bytes_read < 0) {
        handle_error("read");
    }
    buffer[bytes_read] = '\0';
}
send_ftp_command: Envia comandos FTP ao servidor.
read_ftp_response: Lê as respostas do servidor e armazena no buffer.



- Função Principal
int main(int argc, char *argv[]) {
Resumo do Fluxo:
Parse da URL FTP:

Extrai host, usuário, senha e caminho do arquivo.
Conexão ao Servidor de Controle:

Estabelece comunicação com o servidor FTP na porta 21.
Lê a mensagem inicial do servidor.
Autenticação:

Envia os comandos USER e PASS com o nome de usuário e senha.
Modo Passivo:

Entra no modo passivo (PASV) para abrir uma nova conexão de dados.
Extrai o IP e a porta.
Conexão de Dados:

Conecta-se ao socket de dados no IP/porta obtidos.
Baixa o Arquivo:

Envia o comando RETR para iniciar o download.
Lê os dados do socket e salva no arquivo local.
Finalização:

Fecha os sockets e encerra o programa.



- Exemplo de Uso
./ftp_client ftp://user:password@ftp.example.com/path/to/file.txt
Conecta ao servidor ftp.example.com como user com a senha password.
Baixa o arquivo file.txt do diretório /path/to/.





## PART 2

- em cada TUX: ```systemctl restart networking```   (ou reiniciar todos os tuxs)
- gtk : ```/system reset-configuration```
  
### EXP 1

![image](https://github.com/user-attachments/assets/8508334f-cea8-42a6-9152-7f54292e4341)

#### Step1: 
Ligar cabo ao E1 tux3 e E1 tux4
  
#### Step2:
- TUX3: ifconfig eth1 172.16.Y0.1/24
- TUX4: ifconfig eth1 172.16.Y0.254/24
     
#### Step3:
TUX3:
- IP 172.16.30.1/24
- MAC 00:50:fc:ee:0e:93
  
TUX4:
- IP 172.16.30.254/24
- MAC 00:08:54:50:3f:2c

#### Step4:
- TUX3: ```ping 172.16.30.254``` para verificar conexão
- ou TUX4: ```ping 172.16.30.1```

(no ping não se mete /24)

#### Step5:
```route -n``` e ```arp -a``` para ver rotas

#### Step6:
```arp -a (ip) ``` apagar todas as entradas


Questions
- What are the ARP packets and what are they used for?
- What are the MAC and IP addresses of ARP packets and why?
- What packets does the ping command generate?
- What are the MAC and IP addresses of the ping packets?
- How to determine if a receiving Ethernet frame is ARP, IP, ICMP?
- How to determine the length of a receiving frame?
- What is the loopback interface and why is it important?


### EXP2

![image](https://github.com/user-attachments/assets/b321d3eb-eafb-409b-8700-ef2572f44d07)


#### Step1:
Ligar cabo E1 no TUX2
- TUX2: ```ifconfig eth1 172.16.Y1.1/24```
- IP 172.16.71.1
- MAC 00:e0:7d:b5:8c:8f

#### Step2:
GTK: 
- ```/interface bridge add name=bridgeY0```
- ```/interface bridge add name=bridgeY1```

### Step3

- Ver portas em que os cabos do TUX2, TUX3 e TUX4 estão ligadas.
- Remover portas da bridge default com: ```/interface bridge port remove [find interface =ether[NUMEROPORTA]]```

- Adicionar as mesmas portas nas bridges criadas:
- PORTA DO TUX2: ```/interface bridge port add bridge=bridgeY1 interface=ether[NUMEROPORTA]```
- PORTA DO TUX3: ```/interface bridge port add bridge=bridgeY0 interface=ether[NUMEROPORTA]```
- PORTA DO TUX4: ```/interface bridge port add bridge=bridgeY0 interface=ether[NUMEROPORTA]```

#### Step5:
TUX3: 
- pingar TUX4: ```ping 172.16.Y0.254```
- pingar TUX2: ```ping 172.16.Y1.1``` (suposto nao funcionar - bridges diferentes) 

### Step8:
TUX3:
- pingar broadcast ```ping -b 172.16.Y0.255```


Questions
- How to configure bridgeY0?
- How many broadcast domains are there? How can you conclude it from the logs?


## EXP 3
![image](https://github.com/user-attachments/assets/4cf409c5-ba7d-4692-83ab-287944c90cad)

![image](https://github.com/user-attachments/assets/c89c2df1-2de7-4ffe-9bf3-9295282c9e5d)

### Step1:
Tornar TUX4 num router:
- Ligar cabo E2 TUX4 ao switch
- TUX4: ```ifconfig eth2 172.16.Y1.253```
- adicionar à bridge Y1: GTK ```/interface bridge port add bridge=bridgeY1 interface=ether[NUMEROPORTA]```
- Enable IP forwarding: ```sysctl net.ipv4.ip_forward=1```
- Disable ICMP echo ignore broadcast: ```sysctl net.ipv4.icmp_echo_ignore_broadcasts=0```

### Step2:
TUX4 ETH1:
- IP 172.16.70.254
- MAC 00:c0:df:02:55:95
  
TUX4 ETH2:
- IP 172.16.71.253
- MAC 00:01:02:a0:ad:91

### Step3:
Para o TUX2 e o TUX3 serem conectados, temos de criar routes:
  - comando de rota: ```route add -net [IP DESTINO COM .0 (PARA ABRANGIR ESSA REDE) ] gw [IP INTERMEDIARIO]```
  - TUX2: ```route add -net 172.16.70.0/24 gw 172.16.71.253```
  - TUX3: ```route add -net 172.16.71.0/24 gw 172.16.70.254```
 
### Step6:
TUX3: 
- ```ping 172.16.70.254```
- ```ping 172.16.71.253```
- ```ping 172.16.71.1```

### Step9:
Limpar tabela arp:
- ```arp -d [IP]```


Questions
- What routes are there in the tuxes? What are their meaning?
- What information does an entry of the forwarding table contain?
- What ARP messages, and associated MAC addresses, are observed and why?
- What ICMP packets are observed and why?
- What are the IP and MAC addresses associated to ICMP packets and why?

## EXP4

![image](https://github.com/user-attachments/assets/0a776a55-e78b-43db-b450-4e36cd19b668)

![image](https://github.com/user-attachments/assets/0130710b-c24d-4d1a-8d89-c068d6bb7a77)

![image](https://github.com/user-attachments/assets/a2984168-9c36-40a0-9404-004a96085d21)

### Step1:
- Ligar cabo que esta na consola (verde ou amarelo) ao router MTIK
- Ligar um novo cabo no eth1 do routerboard ao PY.12 em cima
- Ligar um novo cabo no eth2 do routerboard ao switch para fazer a ligação
- ver se NAT está enabled (?????) - PERGUNTAR
  
Configurar router no GTK:
- ```ìp adress add address=172.16.1.71/24 interface=ether1```
- ```ìp adress add address=172.16.71.254/24 interface=ether2```


### Step2:
Verificar rotas: ```route -n```
– tuxY4 as default router of tuxY3;
– RC as default router for tuxY2 and tuxY4
– in tuxY2 and RC add routes for 172.16.Y0.0/24

### Step3:
TUX3:
- ping 172.16.71.1 (tux2)
- ping 172.16.70.254 (tux4)
- ping 172.16.71.254 (router)

### Step4:
TUX2:


- ```sysctl net.ipv4.conf.eth1.accept_redirects=0```
- ```sysctl net.ipv4.conf.all.accept_redirects=0```
-
- remover rota direta para o tux3 pelo tux4: ```route del -net 172.16.70.0/24 gw 172.16.71.253```
- ping 172.16.70.1 (tux3)  -> agora a rota vai pelo router
- 
- ```traceroute 172.16.70.1```(tux3)  : tem 3 entradas porque vai pelo router
- voltar a adicionar rota direta para o tux3 pelo tux4:```route add -net 172.16.70.0/24 gw 172.16.71.253```
- ```traceroute 172.16.70.1```(tux3)  : tem 2 entradas porque vai direto pelo tux4
- 
- ```sysctl net.ipv4.conf.eth1.accept_redirects=1```
- remover rota direta para o tux3 pelo tux4: ```route del -net 172.16.70.0/24 gw 172.16.71.253```
- ```traceroute 172.16.70.1```(tux3)  : 3 entradas novamente porque vai por default pelo router quando nao ha tux4
  



















