#!/bin/bash

if test $# -ne 1
then
	echo "usage: bandwidth <filename>"
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

xaxis=" Bandwidth in Mb ||v"

for i in {1..19}
do
	unit="Mb"
	ns "../src/$1.tcl" 6 "$i$unit"
	d=$(cat $1.tr | grep -c d)
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
