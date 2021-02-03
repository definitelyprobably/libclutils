#!/bin/bash

srcdir=.
testsdir=.
if [[ -n "${top_srcdir}" ]]; then
	srcdir=${top_srcdir}/src
	testsdir=${top_srcdir}/tests
fi

if [[ -z "$CXX" ]]; then
	CXX=g++
fi

if [[ -z "$CXXFLAGS" ]]; then
	CXXFLAGS="-Wall -Wfatal-errors"
fi

if [[ -z "$RM" ]]; then
	RM="rm -f"
fi

echo "CXX: $CXX"
echo "CXXFLAGS: $CXXFLAGS"
echo "top_srcdir: $top_srcdir"
echo "RM: $RM"

xsuccess=( cgood1 cgood2 )
xfail=( cfail1 cfail2 cfail3 cfail4 cfail5 cfail6 cfail7 cfail8 )
error=0

for i in "${xsuccess[@]}"; do
	echo "[compile] ${i}.cpp"
	if $CXX $CXXFLAGS -I${srcdir} -o $i ${testsdir}/${i}.cpp; then
		if ./$i ; then
			$RM ./$i
		else
			ret=$?
			echo "-- $CXX $CXXFLAGS -I${srcdir} -o $i ${testsdir}/${i}.cpp"
			echo "-- binary exited with code: $ret"
			error=1
			$RM ./$i
			break
		fi
	else
		ret=$?
		echo "-- $CXX $CXXFLAGS -I${srcdir} -o $i ${testsdir}/${i}.cpp"
		echo "-- compilation unexpectedly failed: returned: $ret"
		error=1
		break
	fi
done

for i in "${xfail[@]}"; do
	echo "[compile] ${i}.cpp"
	if $CXX $CXXFLAGS -I${srcdir} -o /dev/null ${testsdir}/${i}.cpp; then
		echo "-- $CXX $CXXFLAGS -I${srcdir} -o /dev/null ${testsdir}/${i}.cpp"
		echo "-- compilation unexpectedly succeeded"
		error=1
		break
	else
		:
	fi
done

exit $error
