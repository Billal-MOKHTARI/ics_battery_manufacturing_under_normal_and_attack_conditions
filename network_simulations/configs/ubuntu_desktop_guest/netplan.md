# Network Plan
follow these instructions to modify and apply your desired network plan on your **Ubuntu Desktop Guest 24.04** machine:
1. Open the file `/etc/netplan/*yaml`.
2. Modify it with the appropriate code

### Customer Service

```yaml
network:
  version: 2
  renderer: NetworkManager
  ethernets:
    ens3:
      addresses:
        - 192.168.10.x/24
      gateway4: 192.168.10.254
      nameservers:
        addresses:
          - 8.8.8.8
          - 8.8.4.4
    routes:
        - to: 192.168.20.0/24
          via: 192.168.10.254
        - to: 192.168.30.0/24
          via: 192.168.10.254
        - to: 192.168.40.0/24
          via: 192.168.10.254  
```

### Human Resources
```yaml
network:
  version: 2
  renderer: NetworkManager
  ethernets:
    ens3:
      addresses:
        - 192.168.20.x/24
      gateway4: 192.168.20.254
      nameservers:
        addresses:
          - 8.8.8.8
          - 8.8.4.4
      routes:
        - to: 192.168.10.0/24
          via: 192.168.20.254
        - to: 192.168.30.0/24
          via: 192.168.20.254
        - to: 192.168.40.0/24
          via: 192.168.20.254  
```

### Accounting and Finance
```yaml
network:
  version: 2
  renderer: NetworkManager
  ethernets:
    ens3:
      addresses:
        - 192.168.30.x/24
      gateway4: 192.168.30.254
      nameservers:
        addresses:
          - 8.8.8.8
          - 8.8.4.4
      routes:
        - to: 192.168.10.0/24
          via: 192.168.30.254
        - to: 192.168.20.0/24
          via: 192.168.30.254
        - to: 192.168.40.0/24
          via: 192.168.30.254  
```

### Sales
```yaml
network:
  version: 2
  renderer: NetworkManager
  ethernets:
    ens3:
      addresses:
        - 192.168.40.x/24
      gateway4: 192.168.40.254
      nameservers:
        addresses:
          - 8.8.8.8
          - 8.8.4.4
      routes:
        - to: 192.168.10.0/24
          via: 192.168.40.254
        - to: 192.168.20.0/24
          via: 192.168.40.254
        - to: 192.168.30.0/24
          via: 192.168.40.254  
```

### Supervision Network SCADA

```yaml
network:
  version: 2
  renderer: NetworkManager
  ethernets:
    ens3:
      addresses:
        - 192.168.50.x/24
      gateway4: 192.168.50.254
      nameservers:
        addresses:
          - 8.8.8.8
          - 8.8.4.4
      routes:
        - to: 192.168.60.0/24
          via: 192.168.50.254
```

### Production Network

```yaml
network:
  version: 2
  renderer: NetworkManager
  ethernets:
    ens3:
      addresses:
        - 192.168.60.x/24
      gateway4: 192.168.60.254
      nameservers:
        addresses:
          - 8.8.8.8
          - 8.8.4.4
      routes:
        - to: 192.168.50.0/24
          via: 192.168.60.254
```
