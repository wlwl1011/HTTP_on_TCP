#!/bin/bash

for trial in 1 2 3 4 5
do
    sudo systemctl restart NetworkManager
    sleep 5
    ./http_examples_client | tee experiment_${trial}.out
    
    
done