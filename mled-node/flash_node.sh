#!/bin/bash

# Upload tool by mangtronix

NODE=$1

echo "Flashing node $1"


pio run -e node$1 -t upload
