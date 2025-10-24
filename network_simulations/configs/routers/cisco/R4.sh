enable
configure terminal

ip domain-name R4.com
crypto key generate rsa modulus 768
username R4_admin privilege 15 secret pNXqw4rt26G3N5HRIKpLN6Ayk
ip ssh version 2
line vty 0 4
transport input ssh
login local
exit
line console 0
login local
exit

interface FastEthernet 0/0
ip address 87.43.45.4 255.255.255.0
no shutdown
exit

interface FastEthernet 1/0
ip address 87.43.24.4 255.255.255.0
no shutdown
exit

interface FastEthernet 2/0
ip address 87.43.46.4 255.255.255.0
no shutdown
exit

router ospf 1
router-id 4.4.4.4
network 87.43.45.4 0.0.0.0 area 0.0.0.0
network 87.43.24.4 0.0.0.0 area 0.0.0.0
network 87.43.46.4 0.0.0.0 area 0.0.0.0

end
write memory