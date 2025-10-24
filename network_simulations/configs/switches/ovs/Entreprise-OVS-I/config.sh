ovs-vsctl set bridge br0 stp_enable=true

ovs-vsctl del-controller br0
ovs-vsctl set bridge br0 protocols=OpenFlow13
ovs-vsctl set-controller br0 tcp:87.45.10.254:6633