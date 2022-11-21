#!/bin/bash

COUNT=0
SUCCESS=0
FAIL=0
LSUCCESS=0
LFAIL=0
check_stop=0
check_gnu=0
check_mem=''
NO_MEM_ERROR=''
ALL_FLAG=0
NCOUNT=0

usage()
{
	echo 'test_cat.sh [-sgma]'
	echo '  -s                  stop at the first error'
	echo '  -g                  check GNU options (work only with gnu grep)'
	echo '  -m [valgrind/leaks] check memory errors (so long)'
	echo '  -a                  only together with the -m option. Run all tests, '
	echo '                      by default only every tenth (otherwise very long)'
}

while getopts ":asgm:" o
do
	case "${o}" in
		s)
			check_stop=1
			;;
		g)
			check_gnu=1
			cat --number <<<123 >/dev/null
			[ $? -eq 0 ] || {
				echo 'gnu cat not found'
				exit 1
			}
			;;
		m)
			if [ ${OPTARG} == "leaks" ]
			then
				command -v 'leaks' >/dev/null
				[ $? -eq 0 ] || {
					echo 'leaks not found'
					exit 1
				}
				check_mem='leaks -quiet -atExit -- '
			elif [ ${OPTARG} == "valgrind" ]
			then
				command -v 'valgrind' >/dev/null
				[ $? -eq 0 ] || {
					echo 'valgrind not found'
					exit 1
				}
				check_mem='valgrind --leak-check=full --track-origins=yes '`
						  `'-s --log-file=leaks.log '
				NO_MEM_ERROR='ERROR SUMMARY: 0 errors from 0 contexts '`
								`'(suppressed: 0 from 0)'
			else
				usage
				exit 1
			fi
			echo "check mem with ${check_mem}"
			;;
		a)
			ALL_FLAG=1
			;;
		*)
			usage
			exit 1
			;;
	esac
done

make -C ../../src/cat/ re
[ $? -eq 0 ] || {
	echo 'makefile not found or rule re (fclean all) not exist'
}
cp -f ../../src/cat/s21_cat .
[ $? -eq 0 ] || {
	echo 's21_cat not found'
	exit 1
}

tests=(
'empty'
'nofile'
'bytes'
'long_string'
'new_lines'
'no_new_line'
'text'
'empty nofile'
"empty bytes"
"empty long_string"
"empty new_lines"
"empty no_new_line"
"empty text"
"empty nofile bytes"
"empty nofile long_string"
"empty nofile new_lines"
"empty nofile no_new_line"
"empty nofile text"
"empty nofile bytes long_string"
"empty nofile bytes new_lines"
"empty nofile bytes no_new_line"
"empty nofile bytes text"
"empty nofile bytes long_string new_lines"
"empty nofile bytes long_string no_new_line"
"empty nofile bytes long_string text"
"empty nofile bytes long_string new_lines no_new_line"
"empty nofile bytes long_string new_lines text"
"empty nofile bytes long_string new_lines no_new_line text"
)

flags=(
'-b '
'-e '
'-n '
'-s '
'-t '
'-v '
)

gnu_flags=(
'--number-nonblank '
'--number '
'--squeeze-blank '
'-E '
'-T '
)

testing()
{
	(( NCOUNT++ ))
	if [ -n "${check_mem}" ] && [ $ALL_FLAG -eq 0 ] && [ $NCOUNT -ne 10 ] 
	then
		return 21
	fi

	fail=0
	s21cat="$check_mem ./s21_cat $test_case"
	syscat="cat $test_case"

	(( COUNT++ ))
	echo "test case: cat $test_case"
	$s21cat >s21.log 2>s21err.log
	s21exitstatus="$?"
	$syscat >cat.log 2>caterr.log
	catexitstatus="$?"
	d="$(diff s21.log cat.log)"

	if [[ -z "$d" ]]
	then
		(( SUCCESS++ ))
		echo -e "\033[31m$FAIL\033[0m/\033[32m$SUCCESS\033[0m/$COUNT "\
		"functional test: \033[32msuccess\033[0m"
	else
		(( FAIL++ ))
		fail=1
		echo -e "\033[31m$FAIL\033[0m/\033[32m$SUCCESS\033[0m/$COUNT "\
		"functional test: \033[31mfail\033[0m"
	fi
	if [ "$s21exitstatus" == "$catexitstatus" ]
	then
		echo -e "\033[32mExit status s21_grep and grep are equal\033[0m"
	else
		echo -e "\033[31mExit status s21_grep and grep are not equal\033[0m"
	fi

	if [ -n "$check_mem" ]
	then
		grep -e "$NO_MEM_ERROR" 'leaks.log' >/dev/null
		if [ $? -eq 0 ]
		then
			(( LSUCCESS++ ))
			echo -e "\033[31m$LFAIL\033[0m/\033[32m$LSUCCESS\033[0m/$COUNT "\
			"memory test: \033[32msuccess\033[0m"
		else
			fail=1
			(( LFAIL++ ))
			echo -e "\033[31m$LFAIL\033[0m/\033[32m$LSUCCESS\033[0m/$COUNT "\
			"memory test: \033[31mfail\033[0m"
		fi
		NCOUNT=0
	fi

	if [ $fail -eq 1 ] && [ $check_stop -eq 1 ]
	then
		echo -n 'see s21.log, cat.log, s21err.log, caterr.log'
		if [  -n "$check_mem" ]
		then
			echo -n ', leaks.log'
		fi
		echo
		exit 1
	fi

	rm -f s21.log
	rm -f s21err.log
	rm -f cat.log
	rm -f caterr.log
	rm -f leaks.log
	
	echo
}

for t in "${tests[@]}"
do
	test_case="${t}"
	testing
done

for f in "${flags[@]}"
do
	for t in "${tests[@]}"
	do
		test_case="${f}${t}"
		testing
	done
done

for f1 in "${flags[@]}"
do
	for f2 in "${flags[@]}"
	do
		t="bytes long_string empty nofile new_lines no_new_line text"
		if [ "$f1" != "$f2" ]
		then
			test_case="${f1}${f2}${t}"
			testing
		fi
	done
done

for f1 in "${flags[@]}"
do
	for f2 in "${flags[@]}"
	do
		if [ "$f1" == "$f2" ]; then continue; fi
		for f3 in "${flags[@]}"
		do
			if [ "$f1" == "$f3" ] || [ "$f2" == "$f3" ]; then continue; fi
			t="bytes long_string empty nofile new_lines no_new_line text"
			test_case="${f1}${f2}${f3}${t}"
			testing
		done
	done
done

for f1 in "${flags[@]}"
do
	for f2 in "${flags[@]}"
	do
		if [ "$f1" == "$f2" ]; then continue; fi
		for f3 in "${flags[@]}"
		do
			if [ "$f1" == "$f3" ] || [ "$f2" == "$f3" ]; then continue; fi
			for f4 in "${flags[@]}"
			do
				if [ "$f1" == "$f4" ] || [ "$f2" == "$f4" ] ||
				   [ "$f3" == "$f4" ]
				then
					continue
				fi
				t="bytes long_string empty nofile new_lines no_new_line text"
				test_case="${f1}${f2}${f3}${f4}${t}"
				testing
			done
		done
	done
done

f="-b -e -n -t -s "
t="bytes long_string empty nofile new_lines no_new_line text"
test_case="${f}${t}"
testing

if [ $check_gnu -eq 1 ]
then
	t="bytes long_string empty nofile new_lines no_new_line text"

	for f1 in "${gnu_flags[@]}"
	do
		test_case="${f1}${t}"
		testing
	done

	for f1 in "${gnu_flags[@]}"
	do
		for f2 in "${gnu_flags[@]}"
		do
			if [ "$f1" == "$f2" ]; then continue; fi
			test_case="${f1}${f2}${t}"
			testing
		done
	done

	for f1 in "${gnu_flags[@]}"
	do
		for f2 in "${gnu_flags[@]}"
		do
			if [ "$f1" == "$f2" ]; then continue; fi
			for f3 in "${gnu_flags[@]}"
			do
				if [ "$f1" == "$f3" ] ||  [ "$f2" == "$f3" ]
				then
					continue
				fi
				test_case="${f1}${f2}${f3}${t}"
				testing
			done
		done
	done

	for f1 in "${gnu_flags[@]}"
	do
		for f2 in "${gnu_flags[@]}"
		do
			if [ "$f1" == "$f2" ]; then continue; fi
			for f3 in "${gnu_flags[@]}"
			do
				if [ "$f1" == "$f3" ] ||  [ "$f2" == "$f3" ]
				then
					continue
				fi
				for f4 in "${gnu_flags[@]}"
				do
					if [ "$f1" == "$f4" ] ||  [ "$f2" == "$f4" ] || [ "$f3" == "$f4" ]
					then
						continue;
					fi
					test_case="${f1}${f2}${f3}${f4}${t}"
					testing
				done
			done
		done
	done
		
	f="--number-nonblank -T -E --number --squeeze-blank "
	t="bytes long_string empty nofile new_lines no_new_line text"
	test_case="${f}${t}"
	testing

	f="--number-nonblank -T -E --number --squeeze-blank -b -e -s -n -t "
	t="bytes long_string empty nofile new_lines no_new_line text"
	test_case="${f}${t}"
	testing
fi

f="-b -e -j -t -s "
t="bytes long_string empty nofile new_lines no_new_line text"
test_case="${f}${t}"
testing

f="-b -e -n -ti -s "
t="bytes long_string empty nofile new_lines no_new_line text"
test_case="${f}${t}"
testing

f="--number --no-exist "
t="bytes long_string empty nofile new_lines no_new_line text"
test_case="${f}${t}"
testing

f="-b "
t="bytes long_string empty nofile new_lines no_new_line text"
test_case="${f}${t} -e -t"
testing

f="-best "
t="bytes long_string empty nofile new_lines no_new_line text"
test_case="${f}${t}"
testing

rm -f s21_cat
rm -f s21.log
rm -f cat.log
rm -f s21err.log
rm -f caterr.log
rm -f leaks.log

echo -e "\033[31mFAIL: $FAIL\033[0m"
echo -e "\033[32mSUCCESS: $SUCCESS\033[0m"
echo -e "ALL: $COUNT"
if [ -n "$check_mem" ]
then
	echo -e "\033[31mMEM FAIL: $LFAIL\033[0m"
	echo -e "\033[32mMEM SUCCESS: $LSUCCESS\033[0m"
	echo -e "MEM ALL: $COUNT"
fi

make -C ../../src/cat/ fclean
