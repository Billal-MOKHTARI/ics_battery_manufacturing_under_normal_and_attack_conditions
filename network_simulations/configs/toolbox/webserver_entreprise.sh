apt-get upgrade -y
apt-get update -y
apt-get install iptables -y

iptables -V
iptables -L

iptables --policy FORWARD DROP
iptables --policy INPUT  DROP
iptables --policy OUTPUT  DROP

iptables --append INPUT --protocol tcp --dport 80 -m conntrack --ctstate NEW,ESTABLISHED -j ACCEPT
iptables --append OUTPUT -p tcp --sport 80 -m conntrack --ctstate ESTABLISHED -j ACCEPT

iptables -L -v