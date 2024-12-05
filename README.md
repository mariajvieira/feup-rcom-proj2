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

















