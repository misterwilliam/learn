# tun_reader

tun_reader.cc is an example program that reads IP packets from a TUN device. The tun device name is hardcoded to tun0.

Code based upon the following examples:

- https://backreference.org/2010/03/26/tuntap-interface-tutorial/ 
- http://www.cis.syr.edu/~wedu/seed/Labs_12.04/Networking/Firewall_VPN/files/simpletun.c 
- http://www.cis.syr.edu/~wedu/seed/Labs_12.04/Networking/Firewall_VPN/Firewall_VPN.pdf

```sh
# Create tun0 device with permissions to $USER
$ sudo ip tuntap add user $USER mode tun tun0
$ sudo ip link set tun0 up
$ sudo ip addr add 10.0.0.1/24 dev tun0
# Start reading from tun0
$ bazel run :tun_reader
...
Attached to tun: tun0
Read: 48 bytes    <-- Saw an ICMPv6 Router solicitation
Read: 84 bytes    <-- Saw a ping
# In another terminal
$ sudo tshark -i tun0
... In here you can see if all traffic sent to tun0. After you run the ping
    command below you will see ping traffic. Before it, you might other ICMP
    traffic.
# In another terminal
$ ping 10.0.0.2
```
