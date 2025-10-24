enable 
conf t

int FastEthernet 0/0
no shutdown
ip address 10.1.1.239 255.255.255.0
exit

int FastEthernet 0/1
no shutdown
exit

int FastEthernet 1/0
no shutdown
exit

int FastEthernet 1/1
no shutdown
exit

int FastEthernet 1/2
no shutdown
exit

int FastEthernet 1/3
no shutdown
exit

int FastEthernet 1/4
no shutdown
exit

int FastEthernet 1/5
no shutdown
exit

int FastEthernet 1/6
no shutdown
exit

int FastEthernet 1/7
no shutdown
exit

int FastEthernet 1/8
no shutdown
exit

int FastEthernet 1/9
no shutdown
exit

int FastEthernet 1/10
no shutdown
exit

int FastEthernet 1/11
no shutdown
exit

int FastEthernet 1/12
no shutdown
exit

int FastEthernet 1/13
no shutdown
exit

int FastEthernet 1/14
no shutdown
exit

int FastEthernet 1/15
no shutdown
exit

logging host 10.1.1.4 transport udp port 5014
logging trap debugging
logging origin-id hostname
service timestamps log datetime msec localtime 
service sequence-numbers

archive
log config
logging enable
logging size 200
notify syslog
hidekeys
end

wr mem