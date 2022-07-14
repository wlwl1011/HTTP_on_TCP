#!/bin/bash

for trial in 1 2 3 4 5
do
    sudo systemctl restart NetworkManager
    sleep 5
    sudo route del -net 0.0.0.0 dev wlo1
    sudo route add -net 0.0.0.0 gw 192.168.34.1 dev wlo1 metric 50
    ./http_examples_client | tee experiment_${trial}.out
    
    
done