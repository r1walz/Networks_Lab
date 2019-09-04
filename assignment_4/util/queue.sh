#!/bin/bash

if test $# -ne 1
then
	echo "usage: queue <filename>"
	exit 1
fi

if ! test -f "../src/$1.tcl"
then
	echo "file $1.tcl does not exist"
	exit 1
fi

echo "Running $1.tcl"
echo ""
echo "    |Packet Drops --->"
echo "-------------------------------------------------------------"

xaxis="Queue length |||v   "

for i in {0..19}
do
	ns "../src/$1.tcl" $i 1Mb
	d="$(cat $1.tr | grep -c d)"
	bar=""

	for (( j = 0; j < $d; j++ ))
	do
		bar="#$bar"
	done

	if test $i -lt 10
	then
		echo "${xaxis:$i:1}  $i|$bar($d)"
	else
		echo "${xaxis:$i:1} $i|$bar($d)"
	fi
done

rm $1.{nam,tr}
