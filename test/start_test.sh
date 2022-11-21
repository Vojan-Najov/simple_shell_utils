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

echo; echo -e "\033[32mSTART CAT MEMORY TESTS\033[0m (yes/no)?"
tmpans=''
read 'tmpans'
if [[ "$tmpans" == "y" || "$tmpans" == "yes" ]]
then
	cd ./test_cat
	echo
	./test_cat.sh -s -g -m valgrind
	cd ..
fi


echo; echo -e "\033[32mSTART GREP TESTS\033[0m (yes/no)?"
tmpans=''
read 'tmpans'
if [[ "$tmpans" == "y" || "$tmpans" == "yes" ]]
then
	cd ./test_grep
	echo; ./test_grep.sh -s
	cd ..
fi

echo; echo -e "\033[32mSTART GREP MEMORY TESTS\033[0m (yes/no)?"
tmpans=''
read 'tmpans'
if [[ "$tmpans" == "y" || "$tmpans" == "yes" ]]
then
	cd ./test_grep
	echo; ./test_grep.sh -s -m valgrind
	cd ..
fi
