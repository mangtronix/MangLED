#!/bin/bash

# Upload tool by mangtronix
#
# The OTA password needs to match the password in private_config.h that is
# flashed to each node.
#
# e.g. $ ./upload_to_node.sh 5 myotapassword

if [ $# -eq 0 ]
  then
    echo "Usage: upload_to_node.sh node_number [password]"
    exit -1;
fi

NODE=$1
OTA_AUTH=superinsecure
OTA_TARGET=mled-$NODE.local

if [ $# -gt 1 ]
  then
    OTA_AUTH=$2
fi

echo "Flashing node $1 at $OTA_TARGET"

export PLATFORMIO_UPLOAD_FLAGS=--auth=$OTA_AUTH

pio run -e d1_mini_node$1 -t upload --upload-port=$OTA_TARGET
