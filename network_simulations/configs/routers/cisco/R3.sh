enable
configure terminal

ip domain-name R3.com
crypto key generate rsa modulus 768
username R3_admin privilege 15 secret 2h640og2jndj7OAEhKCGk0gNh
ip ssh version 2
line vty 0 4
transport input ssh
login local
exit
line console 0
login local
exit

interface FastEthernet 0/0
ip address 87.43.13.3 255.255.255.0
no shutdown
exit

interface FastEthernet 1/0
ip address 87.43.36.3 255.255.255.0
no shutdown
exit

interface FastEthernet 2/0
ip address 87.43.23.3 255.255.255.0
no shutdown
exit

router ospf 1
router-id 3.3.3.3
network 87.43.13.3 0.0.0.0 area 0.0.0.0
network 87.43.36.3 0.0.0.0 area 0.0.0.0
network 87.43.23.3 0.0.0.0 area 0.0.0.0

end
write memory