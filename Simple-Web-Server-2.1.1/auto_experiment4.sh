#!/bin/bash

for trial in 1 2 3 4 5
do
    sudo systemctl restart NetworkManager
    sleep 5
    sudo route del -net 0.0.0.0 dev wlo1
    sudo route add -net 0.0.0.0 gw 192.168.34.1 dev wlo1 metric 50
    sudo ./epoll_quic_client --disable_certificate_verification --host=155.230.34.228 --port=6200 --from=wlo1 --to=eth0 --enable_watcher --multi_packet_chlo quic.smalldragon.net/ | tee experiment_${trial}.out
    
    
done