#!/bin/bash
cd $(dirname $0)
python3 build.py clean
python3 build.py all
cp bin/* ../initrd/bin