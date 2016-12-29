#! /bin/bash

cat template.lvl

i="0"

while [ $i -lt $1 ]
do
	x=$RANDOM
	y=$RANDOM
	let "x %= 2000"
	let "y %= 800"
	echo "o tree $x $y 40 40 20 0 0 0 50 50 0 0 32 32"
	i=$[$i+1]
done

i="0"
while [ $i -lt $2 ]
do
	x=$RANDOM
	y=$RANDOM
	spell=$RANDOM
	let "x %= 2000"
	let "y %= 800"
	let "spell %= 1"
	echo "h knight 1 $x $y 30 30 30 5 50 50 50 50 0 0 32 32 $spell"
	i=$[$i+1]
done
