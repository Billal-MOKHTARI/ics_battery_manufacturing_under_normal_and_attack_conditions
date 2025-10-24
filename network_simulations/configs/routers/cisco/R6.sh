enable
configure terminal

ip domain-name R6.com
crypto key generate rsa modulus 768
username R6_admin privilege 15 secret NhZ8UELfA1fZAB4VoJ6D6tSYf
ip ssh version 2
line vty 0 4
transport input ssh
login local
exit
line console 0
login local
exit

interface FastEthernet 0/0
ip address 87.43.36.6 255.255.255.0
no shutdown
exit

interface FastEthernet 1/0
ip address 87.43.56.6 255.255.255.0
no shutdown
exit

interface FastEthernet 2/0
ip address 87.43.46.6 255.255.255.0
no shutdown
exit

router ospf 1
router-id 6.6.6.6
network 87.43.36.6 0.0.0.0 area 0.0.0.0
network 87.43.56.6 0.0.0.0 area 0.0.0.0
network 87.43.46.6 0.0.0.0 area 0.0.0.0

end
write memory