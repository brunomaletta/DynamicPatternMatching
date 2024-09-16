for ((i = 1; ; i++)) do
	./$3 $i > in
	./$1 < in > out_base.txt
	./$2 < in > out_test.txt
	if (! cmp -s out_base.txt out_test.txt) then
		echo "--> input:"
		cat in
		echo "--> output base:"
		cat out_base.txt
		echo "--> output test:"
		cat out_test.txt
		break;
	fi
	echo $i
done
