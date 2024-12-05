# RCOM PROJECT 2

## PART 2

- em cada TUX: ```systemctl restart networking```   (ou reiniciar todos os tuxs)
- gtk : /system reset-configuration
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
- TUX3: 





















