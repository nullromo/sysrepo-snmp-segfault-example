#!/bin/bash

# download dependencies
wget https://agentpp.com/download/snmp++-3.4.7.tar.gz 
wget https://agentpp.com/download/agent++-4.5.1.tar.gz 

# extract
mkdir -p dependencies snmp++-3.4.7/build agent++-4.5.1/build build
tar -xf snmp++-3.4.7.tar.gz 
tar -xf agent++-4.5.1.tar.gz 

# build dependencies
cd snmp++-3.4.7/build
cmake .. -DCMAKE_INSTALL_PREFIX=../../dependencies
make -j8
make install 
cd ../..

cd agent++-4.5.1/build
cmake .. -DCMAKE_INSTALL_PREFIX=../../dependencies -DAGENTPP_USE_THREAD_POOL=1
make -j8
make install 
cd ../..

# build code
cd build 
cmake .. -DCMAKE_INSTALL_PREFIX=../dependencies
make -j8
cd ..

echo "finished building"
rm snmp++-3.4.7.tar.gz 
rm agent++-4.5.1.tar.gz 