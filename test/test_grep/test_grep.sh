#!/bin/bash

COUNT=0
SUCCESS=0
FAIL=0
MSUCCESS=0
MFAIL=0

check_stop=0
check_mem=''
NO_MEM_ERROR=''
ALL_FLAG=0
NCOUNT=0
ALL_TESTS=1
NOFLAG=0
EFLAG=0
IFLAG=0
VFLAG=0
CFLAG=0
LFLAG=0
NFLAG=0
HFLAG=0
SFLAG=0
FFLAG=0
OFLAG=0
MULTIFLAG=0

usage()
{
	echo 'test_grep.sh [-stma]'
	echo '  -s                  stop at the first error'
	echo '  -t [0eivclnhsfo]    test this option (-t 0 is no option)'
	echo '                                       (-t m is multi flags)'
	echo '  -m [valgrind/leaks] check memory errors (so long)'
	echo '  -a                  only together with the -m option. Run all tests, '
	echo '                      by default only every tenth (otherwise very long)'
}

while getopts ":at:sm:" o
do
	case "${o}" in
		s)
			check_stop=1
			;;
		t)
			ALL_TESTS=0
			if [ "${OPTARG}" == "0" ]; then NOFLAG=1; fi
			if [ "${OPTARG}" == "e" ]; then EFLAG=1; fi
			if [ "${OPTARG}" == "i" ]; then IFLAG=1; fi
			if [ "${OPTARG}" == "v" ]; then VFLAG=1; fi
			if [ "${OPTARG}" == "c" ]; then CFLAG=1; fi
			if [ "${OPTARG}" == "l" ]; then LFLAG=1; fi
			if [ "${OPTARG}" == "n" ]; then NFLAG=1; fi
			if [ "${OPTARG}" == "h" ]; then HFLAG=1; fi
			if [ "${OPTARG}" == "s" ]; then SFLAG=1; fi
			if [ "${OPTARG}" == "f" ]; then FFLAG=1; fi
			if [ "${OPTARG}" == "o" ]; then OFLAG=1; fi
			if [ "${OPTARG}" == "m" ]; then MULTIFLAG=1; fi
			;;
		m)
			if [ ${OPTARG} == "leaks" ]
			then
				command -v 'leaks'
				[ $? -eq 0 ] || {
					echo 'leaks not found'
					exit 1
				}
				check_mem='leaks -quiet -atExit -- '
			elif [ ${OPTARG} == "valgrind" ]
			then
				command -v 'valgrind'
				[ $? -eq 0 ] || {
					echo 'valgrind not found'
					exit 1
				}
				check_mem='valgrind --leak-check=full --track-origins=yes '`
						 `'-s --log-file=memerror.log '
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

make -C ../../src/grep re
[ $? -eq 0 ] || {
	echo 'makefile not found or rule re (fclean all) not exist'
}
cp ../../src/grep/s21_grep .
[ $? -eq 0 ] || {
	echo 's21_cat not found'
	exit 1
}

tests=(
'empty'
'nofile'
'tiger'
'digits'
'string'
'long_string'
'nofile tiger'
'nofile empty'
'nofile digits'
'tiger digits'
'nofile tiger empty'
'nofile tiger digits'
'nofile tiger empty digits'
'tiger digits tiger_and_digits long_string'
)

temps=(
'Tiger'
'tigeR'
'2'
'notemp'
'ti.ge.'
'.'
'[1-9][2-4][3-9]'
'[a-z]er'
'['
)

ftemps=(
'templates_1'
'templates_2'
'templates_3'
'templates_4'
'no_file'
)

flags=(
'i'
'c'
'l'
'n'
'v'
'h'
's'
'o'
)

testing()
{
	(( NCOUNT++ ))
	if [ -n "${check_mem}" ] && [ ${ALL_FLAG} -eq 0 ] && [ ${NCOUNT} -ne 10 ]
	then
		return 21
	fi

	fail=0
	s21grep="$check_mem ./s21_grep $test_case"
	sysgrep="grep $test_case"

	(( COUNT++ ))
	echo "test_case: grep ""$test_case"" "
	$s21grep >'s21.log' 2>'s21err.log'
	s21exitstatus="$?"
	$sysgrep >'grep.log' 2>'syserr.log'
	grepexitstatus="$?"
	d="$(diff 's21.log' 'grep.log')"

	if [ ${SFLAG} -eq 1 ]
	then
		err="$(grep '*No such file*' 's21err.log' 'grep.log')"
		if [ -n "$err" ]
		then
			d="err"
		fi
	fi

	if [ -z "$d" ]
	then
		(( SUCCESS++ ))
		echo -e "\033[31m$FAIL\033[m/\033[32m$SUCCESS\033[0m/$COUNT "\
		"functional test: \033[32msuccess\033[0m"
	else
		fail=1
		(( FAIL++ ))
		echo -e "\033[31m$FAIL\033[m/\033[32m$SUCCESS\033[0m/$COUNT "\
		"functional test: \033[31mfail\033[0m"
	fi
	if [ "$s21exitstatus" == "$grepexitstatus" ]
	then
		echo -e "\033[32mExit status s21_grep and grep are equal\033[0m"
	else
		echo -e "\033[31mExit status s21_grep and grep are not equal\033[0m"
	fi

	if [ -n "$check_mem" ]
	then
		grep -e "$NO_MEM_ERROR" 'memerror.log' >/dev/null
		if [ $? -eq 0 ]
		then
			(( MSUCCESS++ ))
			echo -e "\033[31m$MFAIL\033[m/\033[32m$MSUCCESS\033[0m/$COUNT "\
			"memory test: \033[32msuccess\033[0m"
		else
			fail=1
			(( MFAIL++ ))
			echo -e "\033[31m$MFAIL\033[m/\033[32m$MSUCCESS\033[0m/$COUNT "\
			"memory test: \033[31mfail\033[0m"
		fi
		NCOUNT=0
	fi

	if [ $fail -eq 1 ] && [ $check_stop -eq 1 ]
	then
		echo -n 'see s21.log, grep.log, s21err.log syserr.log'
		if [ -n "$check_mem" ]
		then
			echo -n ', memerror.log'
		fi
		echo
		exit 1
	fi

	rm -f 's21.log'
	rm -f 'grep.log'
	rm -f 's21err.log'
	rm -f 'syserr.log'
	rm -f 'memerror.log'

	echo
}


# NO FLAG TEST
no_flag_test()
{
	temp='Tiger'
	for t in "${tests[@]}"
	do
		test_case="${temp} ${t}"
		testing
	done
	temp='2'
	for t in "${tests[@]}"
	do
		test_case="${temp} ${t}"
		testing
	done
	temp='notemp'
	for t in "${tests[@]}"
	do
		test_case="${temp} ${t}"
		testing
	done
	temp='.'
	for t in "${tests[@]}"
	do
		test_case="${temp} ${t}"
		testing
	done
}

e_flag_test()
{
	for f in "${tests[@]}"
	do
		for temp in "${temps[@]}"
		do
			test_case="-e $temp $f"
			testing
		done

		for temp1 in "${temps[@]}"
		do
			for temp2 in "${temps[@]}"
			do
				if [ "$temp1" != "\[" ] && [ "$temp2" != "\[" ]
				then
					test_case="-e $temp1 -e$temp2 $f"
					testing
				fi
			done	
		done	
	done	
}

flag_test()
{
	for f in "${tests[@]}"
	do
		for temp in "${temps[@]}"
		do
			test_case="${option} $temp $f"
			testing
		done
	done	
}

f_flag_test()
{
	for f in "${tests[@]}"
	do
		for temp in "${ftemps[@]}"
		do
			test_case="-f $temp $temp $f"
			testing
		done
		for temp1 in "${ftemps[@]}"
		do
			for temp2 in "${ftemps[@]}"
			do
				test_case="-f $temp1 -f$temp2 $temp1 $temp2 $f"
				testing
			done
		done	
	done	
}

if [ ${ALL_TESTS} -eq 1 ] || [ ${NOFLAG} -eq 1 ]
then
	no_flag_test
fi
if [ ${ALL_TESTS} -eq 1 ] || [ ${EFLAG} -eq 1 ]
then
	e_flag_test
fi
if [ ${ALL_TESTS} -eq 1 ] || [ ${IFLAG} -eq 1 ]
then
	option="-i"
	flag_test
fi
if [ ${ALL_TESTS} -eq 1 ] || [ ${VFLAG} -eq 1 ]
then
	option="-v"
	flag_test
fi
if [ ${ALL_TESTS} -eq 1 ] || [ ${CFLAG} -eq 1 ]
then
	option="-c"
	flag_test
fi
if [ ${ALL_TESTS} -eq 1 ] || [ ${LFLAG} -eq 1 ]
then
	option="-l"
	flag_test
fi
if [ ${ALL_TESTS} -eq 1 ] || [ ${NFLAG} -eq 1 ]
then
	option="-n"
	flag_test
fi
if [ ${ALL_TESTS} -eq 1 ] || [ ${HFLAG} -eq 1 ]
then
	option="-h"
	flag_test
fi
if [ ${ALL_TESTS} -eq 1 ] || [ ${SFLAG} -eq 1 ]
then
	SFLAG=1
	option="-s"
	flag_test
fi
if [ ${ALL_TESTS} -eq 1 ] || [ ${FFLAG} -eq 1 ]
then
	f_flag_test
fi
if [ ${ALL_TESTS} -eq 1 ] || [ ${OFLAG} -eq 1 ]
then
	option="-o"
	flag_test
fi
if [ ${ALL_TESTS} -eq 1 ] || [ ${MULTIFLAG} -eq 1 ]
then
	for opt1 in "${flags[@]}"
	do
		for opt2 in "${flags[@]}"
		do
			if [ "$opt1" != "$opt2" ]
			then
				for temp in "${temps[@]}"
				do
					files="tiger digits tiger_and_digits string long_string"
					test_case="-${opt1}${opt2} $temp $files"
					testing
				done
			fi
		done
	done

	for opt1 in "${flags[@]}"
	do
		for opt2 in "${flags[@]:1}"
		do
			for opt3 in "${flags[@]:2}"
			do
				if [ "$opt1" != "$opt2" ] && [ "$opt1" != "$opt3" ] &&
				   [ "$opt2" != "$opt3" ]
				then
					for temp in "${ftemps[@]}"
					do
						files="tiger digits tiger_and_digits string long_string"
						test_case="-${opt1}${opt2}${opt3} -f$temp $files"
						testing
					done
				fi
			done
		done
	done

	for opt1 in "${flags[@]}"
	do
		for opt2 in "${flags[@]:1}"
		do
			if [ "$opt1" == "$opt2" ]; then continue; fi
			for opt3 in "${flags[@]:2}"
			do
				if [ "$opt1" == "$opt3" ] || [ "$opt2" == "$opt3" ]; then continue; fi
				for opt4 in "${flags[@]:3}"
				do
					if [ "$opt1" == "$opt4" ] || [ "$opt2" == "$opt4" ] || 
					   [ "$opt3" == "$opt4" ]
					then
						continue;
					fi
						files="tiger digits tiger_and_digits string long_string"
						test_case="-${opt1}${opt2}${opt3}${opt4} -ftemplates_4 $files"
						testing
				done
			done
		done
	done
fi

echo -e "\033[31mFAIL: $FAIL\033[0m"
echo -e "\033[32mSUCCESS: $SUCCESS\033[0m"
echo -e "ALL: $COUNT"
if [ -n "$check_mem" ]
then
	echo -e "\033[31mMEM FAIL: $LFAIL\033[0m"
	echo -e "\033[32mMEM SUCCESS: $LSUCCESS\033[0m"
	echo -e "MEM ALL: $COUNT"
fi

rm -f s21_grep
rm -f s21.log
rm -f s21err.log
rm -f grep.log
rm -f syserr.log
make -C ../../src/grep/ fclean
