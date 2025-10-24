enable
configure terminal

ip domain-name R2.com
crypto key generate rsa modulus 768
username R2_admin privilege 15 secret CixE6Mio2LSA1RrA57JJtaH2I
ip ssh version 2
line vty 0 4
transport input ssh
login local
exit
line console 0
login local
exit

interface FastEthernet 0/0
ip address 87.43.12.2 255.255.255.0
no shutdown
exit

interface FastEthernet 1/0
ip address 87.43.24.2 255.255.255.0
no shutdown
exit

interface FastEthernet 2/0
ip address 87.43.23.2 255.255.255.0
no shutdown
exit

router ospf 1
router-id 2.2.2.2
network 87.43.12.2 0.0.0.0 area 0.0.0.0
network 87.43.24.2 0.0.0.0 area 0.0.0.0
network 87.43.23.2 0.0.0.0 area 0.0.0.0

end
write memory