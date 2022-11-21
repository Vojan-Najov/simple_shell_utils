INTERACTIVE=0

while getopts ":i" o
do
	case "${o}" in
		i)
			INTERACTIVE=1
			;;
		*)
			echo "option i -- make tests interactive"
			;;
	esac 
done

if [ $INTERACTIVE -eq 1 ]; then
	echo; echo -e "\033[32mSTART CAT TESTS\033[0m (yes/no)?"
	tmpans=''
	read 'tmpans'
	if [[ "$tmpans" == "y" || "$tmpans" == "yes" ]]
	then
		cd ./test_cat
		echo
		./test_cat.sh -s -g
		cd ..
	fi
else
	echo; echo -e "\033[32mSTART CAT TESTS\033[0m"
	sleep 10
	cd ./test_cat
	echo
	./test_cat.sh -s -g
	cd ..
fi

if [ $INTERACTIVE -eq 1 ]; then
	echo; echo -e "\033[32mSTART CAT MEMORY TESTS\033[0m (yes/no)?"
	tmpans=''
	read 'tmpans'
	if [[ "$tmpans" == "y" || "$tmpans" == "yes" ]]
	then
		cd ./test_cat
		echo; ./test_cat.sh -s -g -m valgrind
		cd ..
	fi
else
	echo; echo -e "\033[32mSTART CAT MEMORY TESTS\033[0m"
	sleep 10
	cd ./test_cat
	echo
	./test_cat.sh -s -g -m valgrind -a
	cd ..
fi


if [ $INTERACTIVE -eq 1 ]; then
	echo; echo -e "\033[32mSTART GREP TESTS\033[0m (yes/no)?"
	tmpans=''
	read 'tmpans'
	if [[ "$tmpans" == "y" || "$tmpans" == "yes" ]]
	then
		cd ./test_grep
		echo; ./test_grep.sh -s
		cd ..
	fi
else
	echo; echo -e "\033[32mSTART GREP TESTS\033[0m"
	sleep 10
	cd ./test_grep
	echo; ./test_grep.sh -s
	cd ..
fi
	
if [ $INTERACTIVE -eq 1 ]; then
	echo; echo -e "\033[32mSTART GREP MEMORY TESTS\033[0m (yes/no)?"
	tmpans=''
	read 'tmpans'
	if [[ "$tmpans" == "y" || "$tmpans" == "yes" ]]
	then
		cd ./test_grep
		echo; ./test_grep.sh -s -m valgrind
		cd ..
	fi
else
	echo; echo -e "\033[32mSTART GREP MEMORY TESTS\033[0m"
	sleep 10
	cd ./test_grep
	echo; ./test_grep.sh -s -m valgrind -a
	cd ..
fi
