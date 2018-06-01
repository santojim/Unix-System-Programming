#!/bin/bash

function is_int() { return $(test "$@" -eq "$@" > /dev/null 2>&1); }


while read -r line; 
do
	temp=($line)
    add=${temp[1]}
	if $(is_int "${temp[1]}");
		then
		echo "commander stop $add"
		./commander stop $add
	fi
done < <(./commander poll queued)
################################################################################
while read -r line; 
do
	temp=($line)
	add=${temp[1]}
	if $(is_int "${temp[1]}");
		then
		echo "commander stop $add"
		./commander stop $add
	fi
	done < <(./commander poll running)

./commander exit

