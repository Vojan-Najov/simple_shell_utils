echo
echo -e "\033[32mSTART CAT TESTS\033[0m"
sleep 10
cd ./test_cat
./test_cat.sh -s -g
echo ; echo
echo -e "\033[32mSTART CAT MEMORY TESTS\033[0m"
sleep 10
./test_cat.sh -s -g -m valgrind

sleep 5

echo -e "\033[32mSTART GREP TESTS\033[0m"
sleep 10
cd ../test_grep
./test_grep.sh -s
echo -e "\033[32mSTART GREP MEMORY TESTS\033[0m"
sleep 10
./test_grep.sh -s -m valgrind
