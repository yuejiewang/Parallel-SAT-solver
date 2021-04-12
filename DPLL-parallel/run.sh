##########################################################################
# File Name: run.sh
# Author: yuejie
# mail: kathywangyuejie@gmail.com
# Created Time: Sat Apr 10 19:53:57 2021
#########################################################################
#!/bin/zsh
PATH=/home/edison/bin:/home/edison/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/work/tools/gcc-3.4.5-glibc-2.3.6/bin
export PATH

benchmark_file=""
POW=3
V=0
B=0
while getopts "o:b:vp:" opt; do
	case $opt in
		v)
			V=1
			;;
		p)
			POW=$OPTARG
			;;
		b)
			B=$OPTARG
			V=0
			;;
		o)
			benchmark_file=$OPTARG
			;;
	esac
done
# make clean
# make FPOW=$POW FV=$V
echo "p${POW} start"

if [ ${B} -ne 0 ]; then
	if [ -z ${benchmark_file} ]
	then
		benchmark_file="benchmark_small_p${POW}_b${B}.txt"
		rm -f $benchmark_file
	else
		rm -f $benchmark_file
	fi
fi
for file in ./input/*
do
	filename=${file%.*}
	filename=${filename##*/}
	if [ ${B} -eq 0 ]; then
		./dpll-parallel-p${POW} $file > ./output/$filename.txt
	else
		i=1
		while [ ${i} -le ${B} ]
		do
			./dpll-parallel-p${POW} $file $benchmark_file > ./output/$filename.txt 
			i=`expr ${i} + 1`
		done
	fi
	echo "------------ ${filename}"
	# python3 check_sat.py -c input/$filename.cnf -o $filename.txt
done
