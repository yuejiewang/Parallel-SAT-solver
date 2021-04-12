##########################################################################
# File Name: run.sh
# Author: yuejie
# mail: kathywangyuejie@gmail.com
# Created Time: Sat Apr 10 19:53:57 2021
#########################################################################
#!/bin/zsh
PATH=/home/edison/bin:/home/edison/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/work/tools/gcc-3.4.5-glibc-2.3.6/bin
export PATH

POW=3
V=0
while getopts "vp:" opt; do
	case $opt in
		v)
			V=1
			;;
		p)
			POW=$OPTARG
			;;
	esac
done
make clean
make FPOW=$POW FV=$V
echo "start running"

for file in ./input/*
do
	filename=${file%.*}
	filename=${filename##*/}
	./dpll-parallel $file > ./output/$filename.txt
	echo "------------ ${filename}"
	python3 check_sat.py -c input/$filename.cnf -o $filename.txt
done
