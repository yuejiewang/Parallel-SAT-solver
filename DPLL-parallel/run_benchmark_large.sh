##########################################################################
# File Name: run_benchmark.sh
# Author: yuejie
# mail: kathywangyuejie@gmail.com
# Created Time: Mon Apr 12 21:22:06 2021
#########################################################################
#!/bin/zsh
PATH=/home/edison/bin:/home/edison/.local/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/work/tools/gcc-3.4.5-glibc-2.3.6/bin
export PATH

for p in 6 5 4 3 2
do
	./run_large.sh -b 4 -p ${p}
done
