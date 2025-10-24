## ONOS Configuration
To correctly connect the Openv Switches to the ONOS controller, it is necessary to follow these instructions.

1. ONOS a sowftware accessible using either CLI or GUI (HTTP/HTTPS protocol).
2. To access ONOS in GUI mode, we attach its interface (eth1 for example, assigned an IPv4 address using dhcp4) to cloud via virb0 bridge. 
3. Note that, to link the OVS to the controller, it is necessary to use the static configurations. That means, both of OVS and ONOS interfaces that will be linked together, should have static addresses.
4. In case we have multiple OVS that we want to connect to a controller, we prefer use an ethernet switch to aggregate the connections.
5. Important: when the controller is up, then the flow is controlled and the traffic may not pass.

## Switch configurations
Layer 2 (L2) switches operate using physical network addresses. These physical addresses, also known as link-layer, hardware, or MAC-level addresses, uniquely identify devices. During the manufacturing process, each hardware device is assigned a permanent number. It is the case of Cisco vIOS switches used in our case. 

The interfaces of the Cisco vIOS Switches can be categorized into two types:
1. **1/x interfaces**: they are recommended to use.
2. **0/x interfaces**: we should avoid using these interfaces, because they are operating like a router (it doesn't work with MAC addresse).


## Router and Firewall configurations
- When a host `A` sends a packet to another host `B`, `B` replies to `A` with an acknowledgment. That is why, the traffic should not be blocked from the other direction. This can be realized by defining the gateways on every machine.

## ELK Stack
### Logstash
When the switch is directly connected to another interface of Logstash, the communication fails. To resolve this issue, I connected the Cisco switch to an Ethernet switch, which is then connected to the switch where the Logstash server is attached.

### Elasticsearch
- Once the devices are connected to the ELK stack (I mean the switch that connects the ELK nodes) and configured, you should verify if these devices are correctly recoded in the index list of elasticsearch either in Kibana UI or using this command line (it should be run on the elasticsearch auxiliary console):
```bash
curl --cacert /etc/elasticsearch/certs/http_ca.crt -u elastic http://10.1.1.2:9200/_cat/indices?v
``` 
- Check this [script folder](network_simulations/configs/docker_containers/elasticsearch/scripts) to find all the scripts that we need in this project. These command lines are executable on Elasticsearch terminal (`Auxiliary console` in **GNS3**).


### Kibana
If there is an error in creating the **Index Pattern**, you should follow these instructions:
1. [http://@ip:5603/app/kibana#/dev_tools/console?_g=()](http://@ip:5603/app/kibana#/dev_tools/console?_g=()) (replace `@ip` by your Kibana server ipv4 address).
2. Run the script `unlock_kibana_index.sh` located in the `BaTTwin/network_simulations/configs/docker_containers/kibana/scripts`.

### MySQL Server
When the MySQL Server is installed, the default username is `root`. But, the default password is generated the moment of the instanciation of the container. To retrieve it, you should execute the script [configs/docker_containers/mysql-server/retrieve_container_id.sh](configs/docker_containers/mysql-server/retrieve_container_id.sh). Then, change the password using this script [configs/docker_containers/mysql-server/change_password.sql](configs/docker_containers/mysql-server/change_password.sql).

## Additional Instructions
- Most of errors are due to the fact the devices having a network layer in OSI model don't communicate together. That is why, you should first verufy that the ICMP packets are transferred successfully via `ping` command.
- If a firewall is installed, the problems may be related to the rules fixed by it. For example, if we use **Modbus TCP** protocol and we enabled ICMP protocol, we should realize in advance that the other protocols cannot work.

