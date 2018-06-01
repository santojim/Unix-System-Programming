#!/bin/bash
if (($# ==0))
then	echo "wrong number of arguments ($#)"
fi

for FILE in "$@"
do 
	while read line
	do
		name=$line
		./commander issuejob "$name"
	done < $FILE
done
