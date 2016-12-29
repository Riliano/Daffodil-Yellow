#! /bin/bash
i="0"
RANGE=1000
while [ $i -lt 2000 ]
do
	x=$RANDOM
	y=$RANDOM
	let "x %= 2000"
	let "y %= 800"
	echo "o tree $x $y 40 40 50 0 0 0 50 50 0 0 32 32"
	i=$[$i+50]
done
