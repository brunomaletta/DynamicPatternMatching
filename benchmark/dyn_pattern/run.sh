for ((i = 1; i <= 5; i++)) do
	echo =========== $i*10^5 ===========
	> input

	# size of string
	let val=$i*100000
	echo $val >> input

	# number of operations
	let val=$i*100000
	echo $val >> input

	for file in dyn_pattern naive
	do
		> output
		# average across 1 runs
		for ((j = 1; j <= 1; j++)) do
			let seed=10*$i+$j
			mem=$( (/usr/bin/time -f "%M" ./$file $seed < input >> output) 2>&1)
			echo "$(($mem/1024))" >> output
		done
		./average 1 < output
		echo ""
	done
done
make clean
