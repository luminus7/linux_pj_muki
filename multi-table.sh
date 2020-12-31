#!/bin/bash

#less than 1 might be exit
if [ $1 -lt 1 -o $2 -lt 1 ]
then
    echo "input must be equal or bigger than 1"
    exit 10
fi

for ((var1=1;var1<=$1;var1++));
do
	string=""
	for ((var2=1;var2<=$2;var2++));
	do
		let "re=$var1 * $var2"
		string=$string" ""$var1*$var2=$re"
	done
	echo -e "$string\n"
done


