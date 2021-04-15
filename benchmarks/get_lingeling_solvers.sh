#!/usr/bin/bash

git clone https://github.com/arminbiere/lingeling.git

cd lingeling

./configure.sh && make

echo 'Done compiling lingeling and plingeling!'

