for ((i = 1; i <= 5; i++)) do
	echo =========== $i*10^6 ===========
	> input

	# size of string
	let val=$i*1000000
	echo $val >> input

	# number of operations
	let val=$i*1000000
	echo $val >> input

	for file in dyn_pattern naive
	do
		> output
		# average across 5 runs
		for ((j = 1; j <= 5; j++)) do
			let seed=10*$i+$j
			mem=$( (/usr/bin/time -f "%M" ./$file $seed < input >> output) 2>&1)
			echo "$(($mem/1024))" >> output
		done
		./average 5 < output
		echo ""
	done
done
make clean
