enable
configure terminal

ip domain-name R1.com
crypto key generate rsa modulus 768
username R1_admin privilege 15 secret cDngfo9Yf83s6LaxzYf2oITU8
ip ssh version 2
line vty 0 4
transport input ssh
login local
exit
line console 0
login local
exit

interface FastEthernet 0/0
ip address 87.43.12.1 255.255.255.0
no shutdown
exit

interface FastEthernet 1/0
ip address 87.43.13.1 255.255.255.0
no shutdown
exit

interface FastEthernet 2/0
ip address 87.43.1.1 255.255.255.0
no shutdown
exit

router ospf 1
router-id 1.1.1.1
network 87.43.12.1 0.0.0.0 area 0.0.0.0
network 87.43.13.1 0.0.0.0 area 0.0.0.0
network 87.43.1.1 0.0.0.0 area 0.0.0.1

end
write memory