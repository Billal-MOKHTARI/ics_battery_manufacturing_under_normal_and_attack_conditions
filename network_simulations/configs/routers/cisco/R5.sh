enable
configure terminal

ip domain-name R5.com
crypto key generate rsa modulus 768
username R5_admin privilege 15 secret 8biw2a3f0ZhhH82eLjdeWvI4O
ip ssh version 2
line vty 0 4
transport input ssh
login local
exit
line console 0
login local
exit

interface FastEthernet 0/0
ip address 87.43.56.5 255.255.255.0
no shutdown
exit

interface FastEthernet 1/0
ip address 87.43.45.5 255.255.255.0
no shutdown
exit

interface FastEthernet 2/0
ip address 87.43.2.5 255.255.255.0
no shutdown
exit

router ospf 1
router-id 5.5.5.5
network 87.43.56.5 0.0.0.0 area 0.0.0.0
network 87.43.45.5 0.0.0.0 area 0.0.0.0
network 87.43.2.5 0.0.0.0 area 0.0.0.2

end
write memory