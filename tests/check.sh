#!/bin/bash

if [[ -z "$top_builddir" ]]; then
	top_builddir=..
fi

check_prog=${top_builddir}/tests/clexec
comp_prog=${top_builddir}/tests/comp

exit_code=0

set -o pipefail

# set error keys
Kargempty=1000
Kunrecognized=2000
Ksurplus=3000
Ksurplus_=3010
Kproscribed=3100
Kproscribed_=3110
Kstopproscribed=3101
Kstopproscribed_=3111
Kemptyinput=4000
Kemptyinput_=4010
Kbareemptyinput=4100
Kstopemptyinput=4101
Kbareinput=4200
Kbareinput_=4210
Kstopinput=4201
Kstopinput_=4211
Kmissinginput=4300
Kmissinginput_=4310


exists() {
	if [[ ! -x "$1" ]]; then
		echo "E: program not executable: $1" >&2
		exit 2
	fi
}

exp_bare() {
	echo "  -> exp_bare" >&2
	# expects input: N[.M],flag
	#	  flag:   flag
	#		pos:    N
	#   subpos: M or 0

	pos=N
	subpos=M
	getnums "$1"

	flag=""
	getflag "$1"

	exp_out+=( "I:${flag}:${pos}:${subpos}" )
}

exp_input() {
	echo "  -> exp_input" >&2
	# expects input: N[.M],flag:>input
	#	  flag:   flag
	#		pos:    N
	#   subpos: M or 0
	#   input:  input
	#   type:   external
	# expects input: N[.M],flag:<input
	#	  flag:   flag
	#		pos:    N
	#   subpos: M or 0
	#   input:  input
	#   type:   internal

	pos=N
	subpos=M
	getnums "$2"

	flag=""
	input=""
	input_type=$1
	getflaginput "$2"

	exp_out+=( "I:${flag}:${pos}:${subpos}:${input}:${input_type}" )
}

exp_unrec() {
	exp_out+=( "$i" )
}

exp_arg() {
	exp_out+=( "$i" )
}

getnums() {
	echo "  -> getnums: $1" >&2
	local num=${1%%,*}
	case $num in
		*.*)
			pos=${num%%.*}
			subpos=${num##*.}
			;;
		*)
			pos=$num
			subpos=0
			;;
	esac
}

getflag() {
	flag=${1##*,}
}

getflaginput() {
	local flaginput=${1##*,}
	flag=${flaginput%%:*}
	echo "  -> getfi: flag: $flag" >&2
	input=${flaginput##*:}
	echo "  -> getfi: input: $input" >&2
}

expected_output() {
	# N[.M],-a     --> -a is a bare opt
	# N[.M],-a::A  --> -a is a option/mandatory opt with input 'A'
	# N[.M],::A    --> input 'A'
	# N[.M],-a::   --> unrecognized flag '-a'
	for i in $@; do
		echo "  -> $i" >&2
		case $i in
			U*)     exp_unrec "$i" ;;
			A*)     exp_arg "$i" ;;
			*,*::*) exp_input 1 "$i" ;;
			*,*:*)  exp_input 0 "$i" ;;
			*,*)    exp_bare "$i" ;;
		esac
	done
	exp_out+=( "ret:0" )
}

expected_fail() {
	for i in $@; do
		echo "  --> Eval: $i" >&2
		case "$i" in
			E:${Kargempty}:*) fail_p     "$i" ;;
			E:${Ksurplus}:*) fail_ptoi  "$i" ;;
			E:${Ksurplus_}:*) fail_pstoi "$i" ;;
			E:${Kproscribed}:*) fail_pto  "$i" ;;
			E:${Kproscribed_}:*) fail_psto "$i" ;;
			E:${Kunrecognized}:*) fail_pto   "$i" ;;
			E:${Kemptyinput}:*) fail_ptoi  "$i" ;;
			E:${Kemptyinput_}:*) fail_pstoi  "$i" ;;
			E:${Kbareemptyinput}:*) fail_pto   "$i" ;;
			E:${Kmissinginput}:*) fail_pto   "$i" ;;
			E:${Kmissinginput_}:*) fail_psto  "$i" ;;
			E:${Kbareinput}:*) fail_ptoi  "$i" ;;
			E:${Kbareinput_}:*) fail_pstoi "$i" ;;
			E:${Kstopemptyinput}:*) fail_pto   "$i" ;;
			E:${Kstopinput}:*) fail_ptoi  "$i" ;;
			E:${Kstopinput_}:*) fail_pstoi "$i" ;;
			E:${Kstopproscribed}:*) fail_pto  "$i" ;;
			E:${Kstopproscribed_}:*) fail_psto "$i" ;;
			*)
				echo "internal error: unexpected 'expects' declaration" >&2
				exit 3
				;;
		esac
	done
	exp_out+=( "ret:1" )
}

fail_p() {
	local beg=${1:0:6}
	local strip=${1#E:*:}
	exp_out+=( "${beg}:${strip}:%subpos:%type:%opt:%input" )
	echo "  --> ${beg}:${strip}:%subpos:%type:%opt:%input" >&2
}

fail_pto() {
	local beg=${1:0:6}
	local strip=${1#E:*:}
	local pos=${strip%%:*}
	local rest=${strip#*:}
	exp_out+=( "${beg}:${pos}:%subpos:${rest%:*}:${rest#*:}:%input" )
	echo "  --> ${beg}:${pos}:%subpos:${rest%:*}:${rest#*:}:%input" >&2
}

fail_ptoi() {
	local beg=${1:0:6}
	local strip=${1#E:*:}
	local pos=${strip%%:*}
	local rest=${strip#*:}
	local type=${rest%%:*}
	rest=${rest#*:}
	exp_out+=( "${beg}:${pos}:%subpos:${type}:${rest%:*}:${rest#*:}" )
	echo "  --> ${beg}:${pos}:%subpos:${type}:${rest%:*}:${rest#*:}" >&2
}

fail_psto() {
	local beg=${1:0:6}
	local strip=${1#E:*:}
	local pos=${strip%%:*}
	local rest=${strip#*:}
	local subpos=${rest%%:*}
	rest=${rest#*:}
	exp_out+=( "${beg}:${pos}:${subpos}:${rest%:*}:${rest#*:}:%input" )
	echo "  --> ${beg}:${pos}:${subpos}:${rest%:*}:${rest#*:}:%input" >&2
}

fail_pstoi() {
	local beg=${1:0:6}
	local strip=${1#E:*:}
	local pos=${strip%%:*}
	local rest=${strip#*:}
	local subpos=${rest%%:*}
	rest=${rest#*:}
	local type=${rest%%:*}
	rest=${rest#*:}
	exp_out+=( "${beg}:${pos}:${subpos}:${type}:${rest%:*}:${rest#*:}" )
	echo "  --> ${beg}:${pos}:${subpos}:${type}:${rest%:*}:${rest#*:}" >&2
}

expects() {
	exp_out=()
	if [[ ${1:0:1} == "E" ]]; then
		expected_fail "$@" 2>/dev/null
	else
		expected_output "$@" 2>/dev/null
	fi
}


run_check() {
	echo "[$(basename $check_prog)] ${1}: ${@:2}"

	# $check_prog $@ | $comp $exp_out
	if "$check_prog" "$@" | "$comp_prog" "${exp_out[@]}" ; then
		:
	else
		retval="$?"
		echo ">> (returned: $retval)" >&2
		exit_code=1
		return 1
	fi
}


run_config1() {
	# config1: greedy

	expects
	run_check config1

	expects 1,-a
	run_check config1 -a

	expects 1,-a 2,-o 3,-x::A
	run_check config1 -a -o -x A

	expects 1,-a 2,-o 3,-x:A
	run_check config1 -a -o -xA

	expects 1,-a 2,-a 3,-o 4,-o 5,-x:A 6,-x:B
	run_check config1 -a -a -o -o -xA -xB

	expects 1,-o:A 2,-o 3,-a 4,-a 5,-x:B 6,-x::C
	run_check config1 -oA -o -a -a -xB -x C

	expects 1,-x:A 2,-o 3,-a 4,-a 5,-o 6,-x:B
	run_check config1 -xA -o -a -a -o -xB

	expects 1,-o 2,-o:A 3,-p:B 4,-p 5,-x:C 6,-x::D 8,-y::E 10,-y:F
	run_check config1 -o -oA -pB -p -xC -x D -y E -yF

	expects 1.1,-a 1.2,-b 2.1,-b 2.2,-o 3.1,-a 3.2,-x:A
	run_check config1 -ab -bo -axA

	expects 1.1,-a 1.2,-o 2.1,-b 2.2,-a 3.1,-b 3.2,-y::A
	run_check config1 -ao -ba -by A

	expects 1,-a 2,--aflag 3,--cflag 4,--cflag
	run_check config1 -a --aflag --cflag --cflag

	expects 1,--aflag 2,-a 3,-o 4,--oflag 5,--qflag 6,-p 7,-p
	run_check config1 --aflag -a -o --oflag --qflag -p -p

	expects 1,--oflag 2,-o 3,-x::A 5,--xflag:B
	run_check config1 --oflag -o -x A --xflag=B

	expects 1,--oflag:A 2,-o:B 3,-x:C 4,--xflag::D
	run_check config1 --oflag=A -oB -xC --xflag D

	expects 1,--xflag:A 2,-x::B 4,-y::C 6,-y:D 7,--zflag:E 8,--zflag::F
	run_check config1 --xflag=A -x B -y C -yD --zflag=E --zflag F

	expects 1,--xflag::A 3,-x:B 4.1,-a 4.2,-b 4.3,-y::C 6.1,-a 6.2,-b 7,-y:D
	run_check config1 --xflag A -xB -aby C -ab -yD

	expects 1,-x:--xflag 2,-y:-y 3,--zflag:--zflag
	run_check config1 -x--xflag -y-y --zflag=--zflag

	expects 1,--xflag:-y
	run_check config1 --xflag=-y

	expects 1,-x::--xflag 3,-y::--A 5,--zflag::--zflag
	run_check config1 -x --xflag -y --A --zflag --zflag

	expects 1,-x::-x 3,--zflag::-A
	run_check config1 -x -x --zflag -A

	expects 5,-x::-A 7,--xflag::-B 1,-y::-x 3,--zflag::-x
	run_check config1 -y -x --zflag -x -x -A --xflag -B

	expects 2.1,-a 1.1,-b 1.2,-x:-A 2.2,-y::-B
	run_check config1 -bx-A -ay -B

	expects 1.1,-a 2.1,-b 1.2,-x:-x 2.2,-y:-x
	run_check config1 -ax-x -by-x

	expects 1,-a 2,-x::-s
	run_check config1 -a -x -s

	expects E:${Kunrecognized}:1:flag:-A E:${Kunrecognized}:2:flag:--Bflag
	run_check config1 -A --Bflag

	expects E:${Kmissinginput}:1:flag:-x
	run_check config1 -x

	expects E:${Kmissinginput}:2:flag:-x
	run_check config1 -ao -x

	expects E:${Kmissinginput_}:1:3:flag:-y
	run_check config1 -bay

	expects E:${Kunrecognized}:2:arg:-a
	run_check config1 -s -a

	expects 1,-a 2,-s
	run_check config1 -a -s

	expects 1,-a 2,--
	run_check config1 -a --

	expects E:${Kunrecognized}:3:arg:-s
	run_check config1 -a -- -s

	expects E:${Kunrecognized}:2:arg:-x
	run_check config1 -- -x

	expects E:${Kargempty}:2
	run_check config1 -a "" -x A

	expects E:${Kemptyinput}:1:flag:-x:2
	run_check config1 -x ""

	expects E:${Kemptyinput_}:1:2:flag:-x:2
	run_check config1 -ax ""

	expects E:${Kemptyinput}:1:flag:--xflag:2
	run_check config1 --xflag ""

	expects E:${Kunrecognized}:1:flag:-A E:${Kunrecognized}:2:arg:B
	run_check config1 -A B

	expects E:${Kbareemptyinput}:1:flag:--aflag \
          E:${Kbareinput}:2:flag:--aflag:A E:${Kbareinput}:3:flag:-a:A \
	        E:${Kbareinput_}:4:2:flag:-b:B
	run_check config1 --aflag= --aflag=A -aA -abB

	expects 1,-o::A 3,--oflag::B
	run_check config1 -o A --oflag B

	expects 1,-o::-A 3,--oflag::-B 5,--oflag 6,-o 7,--qflag 8,-p
	run_check config1 -o -A --oflag -B --oflag -o --qflag -p

	expects E:${Kunrecognized}:3:arg:B E:${Kunrecognized}:6:arg:D
	run_check config1 -o A B --oflag C D

	# although there is an error, the first stop _is_ set, so the following
	# are unrecognized arguments
	expects E:${Kstopinput}:1:flag:-s:A E:${Kunrecognized}:2:arg:--stop= \
          E:${Kunrecognized}:3:arg:--stop=B E:${Kunrecognized}:4:arg:-asC
	run_check config1 -sA --stop= --stop=B -asC

	expects E:${Kstopemptyinput}:1:flag:--stop E:${Kunrecognized}:2:arg:-sA \
          E:${Kunrecognized}:3:arg:--stop=B E:${Kunrecognized}:4:arg:-asC
	run_check config1 --stop= -sA --stop=B -asC

	expects E:${Kstopinput}:1:flag:--stop:B E:${Kunrecognized}:2:arg:-sA \
          E:${Kunrecognized}:3:arg:--stop= E:${Kunrecognized}:4:arg:-asC
	run_check config1 --stop=B -sA --stop= -asC

	expects E:${Kstopinput_}:1:2:flag:-s:C E:${Kunrecognized}:2:arg:-sA \
          E:${Kunrecognized}:3:arg:--stop= E:${Kunrecognized}:4:arg:--stop=B
	run_check config1 -asC -sA --stop= --stop=B
}

run_config2() {
	# config2: non-greedy, allowing empty arguments and inputs

	expects
	run_check config2

	expects 1,-a
	run_check config2 -a

	expects 1,-a 2,-o 3,-x::A
	run_check config2 -a -o -x A

	expects 1,-a 2,-o 3,-x:A
	run_check config2 -a -o -xA

	expects 1,-a 2,-a 3,-o 4,-o 5,-x:A 6,-x:B
	run_check config2 -a -a -o -o -xA -xB

	expects 1,-o:A 3,-a 4,-a 2,-o 5,-x:B 6,-x::C
	run_check config2 -oA -o -a -a -xB -x C

	expects 1,-x:A 2,-o 3,-a 4,-a 5,-o 6,-x:B
	run_check config2 -xA -o -a -a -o -xB

	expects 1,-o 2,-o:A 3,-p:B 4,-p 5,-x:C 6,-x::D 8,-y::E 10,-y:F
	run_check config2 -o -oA -pB -p -xC -x D -y E -yF

	expects 1.1,-a 1.2,-b 3.1,-a 2.1,-b 2.2,-o 3.2,-x:A
	run_check config2 -ab -bo -axA

	expects 1.1,-a 2.1,-b 2.2,-a 3.1,-b 1.2,-o 3.2,-y::A
	run_check config2 -ao -ba -by A

	expects 1,-a 2,--aflag 3,--cflag 4,--cflag
	run_check config2 -a --aflag --cflag --cflag

	expects 1,--aflag 2,-a 3,-o 4,--oflag 7,-p 5,--qflag 6,-p
	run_check config2 --aflag -a -o --oflag --qflag -p -p

	expects 1,--oflag 2,-o 3,-x::A 5,--xflag:B
	run_check config2 --oflag -o -x A --xflag=B

	expects 1,--oflag:A 2,-o:B 3,-x:C 4,--xflag::D
	run_check config2 --oflag=A -oB -xC --xflag D

	expects 1,--xflag:A 2,-x::B 4,-y::C 6,-y:D 7,--zflag:E 8,--zflag::F
	run_check config2 --xflag=A -x B -y C -yD --zflag=E --zflag F

	expects 1,--xflag::A 3,-x:B 4.1,-a 4.2,-b 4.3,-y::C 6.1,-a 6.2,-b 7,-y:D
	run_check config2 --xflag A -xB -aby C -ab -yD

	expects 1,-x:--xflag 2,-y:-y 3,--zflag:--zflag
	run_check config2 -x--xflag -y-y --zflag=--zflag

	expects 1,--xflag:-y
	run_check config2 --xflag=-y

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:--xflag \
          E:${Kmissinginput}:3:flag:-y E:${Kunrecognized}:4:flag:--A \
          E:${Kmissinginput}:5:flag:--zflag E:${Kmissinginput}:6:flag:--zflag
	run_check config2 -x --xflag -y --A --zflag --zflag

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:-x \
          E:${Kmissinginput}:3:flag:--zflag E:${Kunrecognized}:4:flag:-A
	run_check config2 -x -x --zflag -A

	expects E:${Kmissinginput}:1:flag:-y E:${Kmissinginput}:2:flag:-x \
          E:${Kmissinginput}:3:flag:--zflag E:${Kmissinginput}:4:flag:-x \
          E:${Kmissinginput}:5:flag:-x E:${Kunrecognized}:6:flag:-A \
	        E:${Kmissinginput}:7:flag:--xflag E:${Kunrecognized}:8:flag:-B 
	run_check config2 -y -x --zflag -x -x -A --xflag -B

	expects E:${Kmissinginput_}:2:2:flag:-y E:${Kunrecognized}:3:flag:-B
	run_check config2 -bx-A -ay -B

	expects 1.1,-a 2.1,-b 1.2,-x:-x 2.2,-y:-x
	run_check config2 -ax-x -by-x

	expects E:${Kmissinginput}:2:flag:-x
	run_check config2 -a -x -s

	expects E:${Kunrecognized}:1:flag:-A E:${Kunrecognized}:2:flag:--Bflag
	run_check config2 -A --Bflag

	expects E:${Kmissinginput}:1:flag:-x
	run_check config2 -x

	expects E:${Kmissinginput}:2:flag:-x
	run_check config2 -ao -x

	expects E:${Kmissinginput_}:1:3:flag:-y
	run_check config2 -bay

	expects E:${Kunrecognized}:2:arg:-a
	run_check config2 -s -a

	expects 1,-a 2,-s
	run_check config2 -a -s

	expects 1,-a 2,--
	run_check config2 -a --

	expects E:${Kunrecognized}:3:arg:-s
	run_check config2 -a -- -s

	expects E:${Kunrecognized}:2:arg:-x
	run_check config2 -- -x

	expects E:${Kunrecognized}:2:arg:
	run_check config2 -a "" -x A

	expects E:${Kunrecognized}:1:flag:-A E:${Kunrecognized}:2:arg:B
	run_check config2 -A B

	expects E:${Kbareemptyinput}:1:flag:--aflag \
          E:${Kbareinput}:2:flag:--aflag:A E:${Kbareinput}:3:flag:-a:A \
	        E:${Kbareinput_}:4:2:flag:-b:B
	run_check config2 --aflag= --aflag=A -aA -abB

	expects 1,-o::A 3,--oflag::B
	run_check config2 -o A --oflag B

	expects E:${Kunrecognized}:2:flag:-A E:${Kunrecognized}:4:flag:-B
	run_check config2 -o -A --oflag -B --oflag -o --qflag -p

	expects E:${Kunrecognized}:3:arg:B E:${Kunrecognized}:6:arg:D
	run_check config2 -o A B --oflag C D

	# although there is an error, the first stop _is_ set, so the following
	# are unrecognized arguments
	expects E:${Kstopinput}:1:flag:-s:A E:${Kunrecognized}:2:arg:--stop= \
          E:${Kunrecognized}:3:arg:--stop=B E:${Kunrecognized}:4:arg:-asC
	run_check config2 -sA --stop= --stop=B -asC

	expects E:${Kstopemptyinput}:1:flag:--stop E:${Kunrecognized}:2:arg:-sA \
          E:${Kunrecognized}:3:arg:--stop=B E:${Kunrecognized}:4:arg:-asC
	run_check config2 --stop= -sA --stop=B -asC

	expects E:${Kstopinput}:1:flag:--stop:B E:${Kunrecognized}:2:arg:-sA \
          E:${Kunrecognized}:3:arg:--stop= E:${Kunrecognized}:4:arg:-asC
	run_check config2 --stop=B -sA --stop= -asC

	expects E:${Kstopinput_}:1:2:flag:-s:C E:${Kunrecognized}:2:arg:-sA \
          E:${Kunrecognized}:3:arg:--stop= E:${Kunrecognized}:4:arg:--stop=B
	run_check config2 -asC -sA --stop= --stop=B
}

run_config3() {
	# config3: lax greedy

	expects
	run_check config3

	expects 1,-a
	run_check config3 -a

	expects 1,-a 2,-o 3,-x::A
	run_check config3 -a -o -x A

	expects 1,-a 2,-o 3,-x:A
	run_check config3 -a -o -xA

	expects 1,-a 2,-a 3,-o 4,-o 5,-x:A 6,-x:B
	run_check config3 -a -a -o -o -xA -xB

	expects 1,-o:A 3,-a 4,-a 2,-o 5,-x:B 6,-x::C
	run_check config3 -oA -o -a -a -xB -x C

	expects 1,-x:A 2,-o 3,-a 4,-a 5,-o 6,-x:B
	run_check config3 -xA -o -a -a -o -xB

	expects 1,-o 2,-o:A 3,-p:B 4,-p 5,-x:C 6,-x::D 8,-y::E 10,-y:F
	run_check config3 -o -oA -pB -p -xC -x D -y E -yF

	expects 1.1,-a 1.2,-b 3.1,-a 2.1,-b 2.2,-o 3.2,-x:A
	run_check config3 -ab -bo -axA

	expects 1.1,-a 2.1,-b 2.2,-a 3.1,-b 1.2,-o 3.2,-y::A
	run_check config3 -ao -ba -by A

	expects 1,-a 2,--aflag 3,--cflag 4,--cflag
	run_check config3 -a --aflag --cflag --cflag

	expects 1,--aflag 2,-a 3,-o 4,--oflag 7,-p 5,--qflag 6,-p
	run_check config3 --aflag -a -o --oflag --qflag -p -p

	expects 1,--oflag 2,-o 3,-x::A 5,--xflag:B
	run_check config3 --oflag -o -x A --xflag=B

	expects 1,--oflag:A 2,-o:B 3,-x:C 4,--xflag::D
	run_check config3 --oflag=A -oB -xC --xflag D

	expects 1,--xflag:A 2,-x::B 4,-y::C 6,-y:D 7,--zflag:E 8,--zflag::F
	run_check config3 --xflag=A -x B -y C -yD --zflag=E --zflag F

	expects 1,--xflag::A 4.1,-a 4.2,-b 4.3,-y::C 6.1,-a 6.2,-b 3,-x:B 7,-y:D
	run_check config3 --xflag A -xB -aby C -ab -yD

	expects 1,-x:--xflag 2,-y:-y 3,--zflag:--zflag
	run_check config3 -x--xflag -y-y --zflag=--zflag

	expects 1,--xflag:-y
	run_check config3 --xflag=-y

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:--xflag \
	        E:${Kmissinginput}:5:flag:--zflag E:${Kmissinginput}:6:flag:--zflag
	run_check config3 -x --xflag -y --A --zflag --zflag

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config3 -x -x --zflag -A

	expects E:${Kmissinginput}:1:flag:-y E:${Kmissinginput}:2:flag:-x \
	        E:${Kmissinginput}:3:flag:--zflag E:${Kmissinginput}:4:flag:-x
	run_check config3 -y -x --zflag -x -x -A --xflag -B

	expects 2.1,-a 1.1,-b 1.2,-x:-A 2.2,-y::-B
	run_check config3 -bx-A -ay -B

	expects 1.1,-a 2.1,-b 1.2,-x:-x 2.2,-y:-x
	run_check config3 -ax-x -by-x

	expects E:${Kmissinginput}:2:flag:-x
	run_check config3 -a -x -s

	expects E:${Kunrecognized}:1:flag:-A E:${Kunrecognized}:2:flag:--Bflag
	run_check config3 -A --Bflag

	expects E:${Kmissinginput}:1:flag:-x
	run_check config3 -x

	expects E:${Kmissinginput}:2:flag:-x
	run_check config3 -ao -x

	expects E:${Kmissinginput_}:1:3:flag:-y
	run_check config3 -bay

	expects E:${Kunrecognized}:2:arg:-a
	run_check config3 -s -a

	expects 1,-a 2,-s
	run_check config3 -a -s

	expects 1,-a 2,--
	run_check config3 -a --

	expects E:${Kunrecognized}:3:arg:-s
	run_check config3 -a -- -s

	expects E:${Kunrecognized}:2:arg:-x
	run_check config3 -- -x

	expects E:${Kargempty}:2
	run_check config3 -a "" -x A

	expects E:${Kunrecognized}:1:flag:-A E:${Kunrecognized}:2:arg:B
	run_check config3 -A B

	expects E:${Kbareemptyinput}:1:flag:--aflag \
          E:${Kbareinput}:2:flag:--aflag:A E:${Kbareinput}:3:flag:-a:A \
	        E:${Kbareinput_}:4:2:flag:-b:B
	run_check config3 --aflag= --aflag=A -aA -abB

	expects 1,-o::A 3,--oflag::B
	run_check config3 -o A --oflag B

	expects 1,-o::-A 3,--oflag::-B 5,--oflag 6,-o 7,--qflag 8,-p
	run_check config3 -o -A --oflag -B --oflag -o --qflag -p

	expects E:${Kunrecognized}:3:arg:B E:${Kunrecognized}:6:arg:D
	run_check config3 -o A B --oflag C D

	expects 1,--oflag:-A 2,--oflag::-B 4,-o::-C
	run_check config3 --oflag=-A --oflag -B -o -C

	# although there is an error, the first stop _is_ set, so the following
	# are unrecognized arguments
	expects E:${Kstopinput}:1:flag:-s:A E:${Kunrecognized}:2:arg:--stop= \
          E:${Kunrecognized}:3:arg:--stop=B E:${Kunrecognized}:4:arg:-asC
	run_check config3 -sA --stop= --stop=B -asC

	expects E:${Kstopemptyinput}:1:flag:--stop E:${Kunrecognized}:2:arg:-sA \
          E:${Kunrecognized}:3:arg:--stop=B E:${Kunrecognized}:4:arg:-asC
	run_check config3 --stop= -sA --stop=B -asC

	expects E:${Kstopinput}:1:flag:--stop:B E:${Kunrecognized}:2:arg:-sA \
          E:${Kunrecognized}:3:arg:--stop= E:${Kunrecognized}:4:arg:-asC
	run_check config3 --stop=B -sA --stop= -asC

	expects E:${Kstopinput_}:1:2:flag:-s:C E:${Kunrecognized}:2:arg:-sA \
          E:${Kunrecognized}:3:arg:--stop= E:${Kunrecognized}:4:arg:--stop=B
	run_check config3 -asC -sA --stop= --stop=B
}

run_config4() {
	# config4: non-greedy, allow arguments
	# mandatory opt -x will take A, but not -A and -x

	expects A:A:1 A:B:2
	run_check config4 A B

	expects E:${Kunrecognized}:1:flag:-A E:${Kunrecognized}:2:flag:-B
	run_check config4 -A -B

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config4 -x -x

	expects E:${Kmissinginput}:1:flag:-x
	run_check config4 -x -- -x

	expects E:${Kmissinginput_}:2:2:flag:-x E:${Kmissinginput}:3:flag:-x
	run_check config4 -ab -ax -x

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kmissinginput_}:2:2:flag:-x
	run_check config4 -ax -ax

	expects E:${Kmissinginput_}:1:2:flag:-x
	run_check config4 -ax -- -ax

	expects E:${Kmissinginput_}:1:2:flag:-x
	run_check config4 -ax -- -ab

	expects 1,-a 3,-o 5,-x::C A:A:2 A:B:4 A:D:7
	run_check config4 -a A -o B -x C D

	expects E:${Kunrecognized}:2:flag:-A E:${Kunrecognized}:4:flag:-B \
          E:${Kmissinginput}:5:flag:-x E:${Kunrecognized}:6:flag:-C \
	        E:${Kunrecognized}:7:flag:-D
	run_check config4 -a -A -o -B -x -C -D

	expects 1,-x:-x 2,-y::A 4,--zflag::B A:C:6
	run_check config4 -x-x -y A --zflag B C

	expects E:${Kmissinginput}:2:flag:-y E:${Kunrecognized}:3:flag:-A \
          E:${Kmissinginput}:4:flag:--zflag E:${Kunrecognized}:5:flag:-B
	run_check config4 -x-x -y -A --zflag -B

	expects E:${Kmissinginput}:2:flag:-y E:${Kunrecognized}:3:flag:-A \
          E:${Kmissinginput}:4:flag:--zflag E:${Kunrecognized}:5:flag:-B \
          E:${Kunrecognized}:6:flag:-C
	run_check config4 -x-x -y -A --zflag -B -C

	expects 1,--xflag:A A:B:2
	run_check config4 --xflag=A B

	expects E:${Kunrecognized}:2:flag:-B
	run_check config4 --xflag=-A -B

	expects 2.1,-a 3.1,-b 2.2,-x:B 3.2,-y::C A:A:1 A:D:5
	run_check config4 A -axB -by C D

	expects E:${Kunrecognized}:1:flag:-A E:${Kmissinginput_}:3:2:flag:-y \
          E:${Kunrecognized}:4:flag:-C E:${Kunrecognized}:5:flag:-D
	run_check config4 -A -ax-B -by -C -D

	expects E:${Kmissinginput}:1:flag:-x E:${Kunrecognized}:2:flag:-A
	run_check config4 -x -A

	expects E:${Kmissinginput}:2:flag:--zflag E:${Kunrecognized}:3:flag:-A
	run_check config4 -a --zflag -A

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kunrecognized}:2:flag:-A
	run_check config4 -ax -A

	expects E:${Kbareinput}:1:flag:-a:A
	run_check config4 -aA B

	expects E:${Kbareinput}:1:flag:-a:-A E:${Kunrecognized}:2:flag:-B
	run_check config4 -a-A -B

	expects 2,-- A:A:1 A:B:3 A:--:4 A:-C:5
	run_check config4 A -- B -- -C

	expects E:${Kunrecognized}:1:flag:-A
	run_check config4 -A -- -B -- -C

	expects 2.1,-a 2.2,-s A:A:1 A:B:3 A:-a:4 A:-o:5 A:-x:6 A:-C:7
	run_check config4 A -as B -a -o -x -C

	expects E:${Kunrecognized}:1:flag:-A
	run_check config4 -A -as -B -a -o -x -C
}

run_config5() {
	# config5: lax greedy, allow arguments
	# mandatory opt -x will take A and -A but not -x
	# optional opt -o will take -A but not A and -o

	expects A:A:1 A:B:2
	run_check config5 A B

	expects E:${Kunrecognized}:1:flag:-A E:${Kunrecognized}:2:flag:-B
	run_check config5 -A -B

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config5 -x -x

	expects E:${Kmissinginput}:1:flag:-x
	run_check config5 -x -- -x

	expects E:${Kmissinginput_}:2:2:flag:-x E:${Kmissinginput}:3:flag:-x
	run_check config5 -ab -ax -x

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kmissinginput_}:2:2:flag:-x
	run_check config5 -ax -ax

	expects E:${Kmissinginput_}:1:2:flag:-x
	run_check config5 -ax -- -ax

	expects E:${Kmissinginput_}:1:2:flag:-x
	run_check config5 -ax -- -ab

	expects 1,-a 3,-o 5,-x::C A:A:2 A:B:4 A:D:7
	run_check config5 -a A -o B -x C D

	expects E:${Kunrecognized}:2:flag:-A E:${Kunrecognized}:7:flag:-D
	run_check config5 -a -A -o -B -x -C -D

	expects 1,-x:-x 2,-y::A 4,--zflag::B A:C:6
	run_check config5 -x-x -y A --zflag B C

	expects 1,-x:-x 2,-y::-A 4,--zflag::-B
	run_check config5 -x-x -y -A --zflag -B

	expects E:${Kunrecognized}:6:flag:-C
	run_check config5 -x-x -y -A --zflag -B -C

	expects 1,--xflag:A A:B:2
	run_check config5 --xflag=A B

	expects E:${Kunrecognized}:2:flag:-B
	run_check config5 --xflag=-A -B

	expects 2.1,-a 3.1,-b 2.2,-x:B 3.2,-y::C A:A:1 A:D:5
	run_check config5 A -axB -by C D

	expects E:${Kunrecognized}:1:flag:-A E:${Kunrecognized}:5:flag:-D
	run_check config5 -A -ax-B -by -C -D

	expects 1,-x::-A
	run_check config5 -x -A

	expects 1,-a 2,--zflag::-A
	run_check config5 -a --zflag -A

	expects 1.1,-a 1.2,-x::-A
	run_check config5 -ax -A

	expects E:${Kbareinput}:1:flag:-a:A
	run_check config5 -aA B

	expects E:${Kbareinput}:1:flag:-a:-A E:${Kunrecognized}:2:flag:-B
	run_check config5 -a-A -B

	expects 2,-- A:A:1 A:B:3 A:--:4 A:-C:5
	run_check config5 A -- B -- -C

	expects E:${Kunrecognized}:1:flag:-A
	run_check config5 -A -- -B -- -C

	expects 2.1,-a 2.2,-s A:A:1 A:B:3 A:-a:4 A:-o:5 A:-x:6 A:-C:7
	run_check config5 A -as B -a -o -x -C

	expects E:${Kunrecognized}:1:flag:-A
	run_check config5 -A -as -B -a -o -x -C
}

run_config6() {
	# config6: greedy, allow arguments
	# mandatory opt -x will take anything, A, -A or -x
	# optional opt -o will take -A but not A or -o

	expects A:A:1 A:B:2
	run_check config6 A B

	expects E:${Kunrecognized}:1:flag:-A E:${Kunrecognized}:2:flag:-B
	run_check config6 -A -B

	expects 1,-x::-x
	run_check config6 -x -x

	expects E:${Kmissinginput}:3:flag:-x
	run_check config6 -x -- -x

	expects 1.1,-a 2.1,-a 1.2,-b 2.2,-x::-x
	run_check config6 -ab -ax -x

	expects 1.1,-a 1.2,-x::-ax
	run_check config6 -ax -ax

	expects E:${Kmissinginput_}:3:2:flag:-x
	run_check config6 -ax -- -ax

	expects 1.1,-a 3.1,-a 3.2,-b 1.2,-x::-- 
	run_check config6 -ax -- -ab

	expects 1,-a 3,-o 5,-x::C A:A:2 A:B:4 A:D:7
	run_check config6 -a A -o B -x C D

	expects E:${Kunrecognized}:2:flag:-A E:${Kunrecognized}:7:flag:-D
	run_check config6 -a -A -o -B -x -C -D

	expects 1,-x:-x 2,-y::A 4,--zflag::B A:C:6
	run_check config6 -x-x -y A --zflag B C

	expects 1,-x:-x 2,-y::-A 4,--zflag::-B
	run_check config6 -x-x -y -A --zflag -B

	expects E:${Kunrecognized}:6:flag:-C
	run_check config6 -x-x -y -A --zflag -B -C

	expects 1,--xflag:A A:B:2
	run_check config6 --xflag=A B

	expects E:${Kunrecognized}:2:flag:-B
	run_check config6 --xflag=-A -B

	expects 2.1,-a 3.1,-b 2.2,-x:B 3.2,-y::C A:A:1 A:D:5
	run_check config6 A -axB -by C D

	expects E:${Kunrecognized}:1:flag:-A E:${Kunrecognized}:5:flag:-D
	run_check config6 -A -ax-B -by -C -D

	expects 1,-x::-A
	run_check config6 -x -A

	expects 1,-a 2,--zflag::-A
	run_check config6 -a --zflag -A

	expects 1.1,-a 1.2,-x::-A
	run_check config6 -ax -A

	expects E:${Kbareinput}:1:flag:-a:A
	run_check config6 -aA B

	expects E:${Kbareinput}:1:flag:-a:-A E:${Kunrecognized}:2:flag:-B
	run_check config6 -a-A -B

	expects 2,-- A:A:1 A:B:3 A:--:4 A:-C:5
	run_check config6 A -- B -- -C

	expects E:${Kunrecognized}:1:flag:-A
	run_check config6 -A -- -B -- -C

	expects 2.1,-a 2.2,-s A:A:1 A:B:3 A:-a:4 A:-o:5 A:-x:6 A:-C:7
	run_check config6 A -as B -a -o -x -C

	expects E:${Kunrecognized}:1:flag:-A
	run_check config6 -A -as -B -a -o -x -C
}

run_config7() {
	# config7: non-greedy, allow unrecognized flags
	# mandatory opt -x will take A, but not -A and -x
	# optional opt will take A but not -A or -o

	expects E:${Kunrecognized}:1:arg:A E:${Kunrecognized}:2:arg:B
	run_check config7 A B

	expects U:-A:1 U:-B:2
	run_check config7 -A -B

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config7 -x -x

	expects E:${Kmissinginput}:1:flag:-x E:${Kunrecognized}:3:arg:-x
	run_check config7 -x -- -x

	expects E:${Kmissinginput_}:2:2:flag:-x E:${Kmissinginput}:3:flag:-x
	run_check config7 -ab -ax -x

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kmissinginput_}:2:2:flag:-x
	run_check config7 -ax -ax

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kunrecognized}:3:arg:-ax
	run_check config7 -ax -- -ax

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kunrecognized}:3:arg:-ab
	run_check config7 -ax -- -ab

	expects E:${Kunrecognized}:2:arg:A E:${Kunrecognized}:7:arg:D
	run_check config7 -a A -o B -x C D

	expects E:${Kmissinginput}:5:flag:-x
	run_check config7 -a -A -o -B -x -C -D

	expects E:${Kunrecognized}:6:arg:C
	run_check config7 -x-x -y A --zflag B C

	expects E:${Kmissinginput}:2:flag:-y E:${Kmissinginput}:4:flag:--zflag
	run_check config7 -x-x -y -A --zflag -B

	expects E:${Kmissinginput}:2:flag:-y E:${Kmissinginput}:4:flag:--zflag
	run_check config7 -x-x -y -A --zflag -B -C

	expects E:${Kunrecognized}:2:arg:B
	run_check config7 --xflag=A B

	expects 1,--xflag:-A U:-B:2
	run_check config7 --xflag=-A -B

	expects E:${Kunrecognized}:1:arg:A E:${Kunrecognized}:5:arg:D
	run_check config7 A -axB -by C D

	expects E:${Kmissinginput_}:3:2:flag:-y
	run_check config7 -A -ax-B -by -C -D

	expects 2.1,-a 3.1,-b 2.2,-x:-B 3.2,-y::C U:-A:1 U:-D:5
	run_check config7 -A -ax-B -by C -D

	expects E:${Kmissinginput}:1:flag:-x
	run_check config7 -x -A

	expects 1,-x::A U:-B:3
	run_check config7 -x A -B

	expects 1,-a 2,--zflag::A U:-B:4
	run_check config7 -a --zflag A -B

	expects E:${Kmissinginput}:2:flag:--zflag
	run_check config7 -a --zflag -A

	expects E:${Kunrecognized}:2:arg:B
	run_check config7 -axA B -C

	expects 1.1,-a 1.2,-x:A U:-B:2
	run_check config7 -axA -B

	expects 1.1,-a 1.2,-x::A U:-B:3
	run_check config7 -ax A -B

	expects E:${Kmissinginput_}:1:2:flag:-x
	run_check config7 -ax -A

	expects E:${Kbareinput}:1:flag:-a:A E:${Kunrecognized}:2:arg:B
	run_check config7 -aA B

	expects E:${Kbareinput}:1:flag:-a:-A E:${Kunrecognized}:2:arg:B
	run_check config7 -a-A B

	expects E:${Kbareinput}:1:flag:-a:-A
	run_check config7 -a-A -B

	expects E:${Kunrecognized}:1:arg:A E:${Kunrecognized}:3:arg:B \
	        E:${Kunrecognized}:4:arg:-- E:${Kunrecognized}:5:arg:-C
	run_check config7 A -- B -- -C

	expects E:${Kunrecognized}:3:arg:-B E:${Kunrecognized}:4:arg:-- \
	        E:${Kunrecognized}:5:arg:-C
	run_check config7 -A -- -B -- -C

	expects E:${Kunrecognized}:1:arg:A E:${Kunrecognized}:3:arg:B \
	        E:${Kunrecognized}:4:arg:-a E:${Kunrecognized}:5:arg:-o \
	        E:${Kunrecognized}:6:arg:-x E:${Kunrecognized}:7:arg:-C
	run_check config7 A -as B -a -o -x -C

	expects E:${Kunrecognized}:3:arg:-B E:${Kunrecognized}:4:arg:-a \
	        E:${Kunrecognized}:5:arg:-o E:${Kunrecognized}:6:arg:-x \
	        E:${Kunrecognized}:7:arg:-C
	run_check config7 -A -as -B -a -o -x -C
}

run_config8() {
	# config8: lax greedy, allow unrecognized flags
	# mandatory opt -x will take A and -A but not -x
	# optional opt -o will take A but not -A or -o

	expects E:${Kunrecognized}:1:arg:A E:${Kunrecognized}:2:arg:B
	run_check config8 A B

	expects U:-A:1 U:-B:2
	run_check config8 -A -B

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config8 -x -x

	expects E:${Kmissinginput}:1:flag:-x E:${Kunrecognized}:3:arg:-x
	run_check config8 -x -- -x

	expects E:${Kmissinginput_}:2:2:flag:-x E:${Kmissinginput}:3:flag:-x
	run_check config8 -ab -ax -x

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kmissinginput_}:2:2:flag:-x
	run_check config8 -ax -ax

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kunrecognized}:3:arg:-ax
	run_check config8 -ax -- -ax

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kunrecognized}:3:arg:-ab
	run_check config8 -ax -- -ab

	expects E:${Kunrecognized}:2:arg:A E:${Kunrecognized}:7:arg:D
	run_check config8 -a A -o B -x C D

	expects 1,-a 3,-o 5,-x::-C U:-A:2 U:-B:4 U:-D:7
	run_check config8 -a -A -o -B -x -C -D

	expects E:${Kunrecognized}:6:arg:C
	run_check config8 -x-x -y A --zflag B C

	expects 1,-x:-x 2,-y::-A 4,--zflag::-B
	run_check config8 -x-x -y -A --zflag -B

	expects 1,-x:-x 2,-y::-A 4,--zflag::-B U:-C:6
	run_check config8 -x-x -y -A --zflag -B -C

	expects E:${Kunrecognized}:2:arg:B
	run_check config8 --xflag=A B

	expects 1,--xflag:-A U:-B:2
	run_check config8 --xflag=-A -B

	expects E:${Kunrecognized}:1:arg:A E:${Kunrecognized}:5:arg:D
	run_check config8 A -axB -by C D

	expects 2.1,-a 3.1,-b 2.2,-x:-B 3.2,-y::-C U:-A:1 U:-D:5
	run_check config8 -A -ax-B -by -C -D

	expects 2.1,-a 3.1,-b 2.2,-x:-B 3.2,-y::C U:-A:1 U:-D:5
	run_check config8 -A -ax-B -by C -D

	expects 1,-x::-A
	run_check config8 -x -A

	expects 1,-x::A U:-B:3
	run_check config8 -x A -B

	expects 1,-a 2,--zflag::A U:-B:4
	run_check config8 -a --zflag A -B

	expects 1,-a 2,--zflag::-A
	run_check config8 -a --zflag -A

	expects E:${Kunrecognized}:2:arg:B
	run_check config8 -axA B -C

	expects 1.1,-a 1.2,-x:A U:-B:2
	run_check config8 -axA -B

	expects 1.1,-a 1.2,-x::A U:-B:3
	run_check config8 -ax A -B

	expects 1.1,-a 1.2,-x::-A
	run_check config8 -ax -A

	expects E:${Kbareinput}:1:flag:-a:A E:${Kunrecognized}:2:arg:B
	run_check config8 -aA B

	expects E:${Kbareinput}:1:flag:-a:-A E:${Kunrecognized}:2:arg:B
	run_check config8 -a-A B

	expects E:${Kbareinput}:1:flag:-a:-A
	run_check config8 -a-A -B

	expects E:${Kunrecognized}:1:arg:A E:${Kunrecognized}:3:arg:B \
	        E:${Kunrecognized}:4:arg:-- E:${Kunrecognized}:5:arg:-C
	run_check config8 A -- B -- -C

	expects E:${Kunrecognized}:3:arg:-B E:${Kunrecognized}:4:arg:-- \
	        E:${Kunrecognized}:5:arg:-C
	run_check config8 -A -- -B -- -C

	expects E:${Kunrecognized}:1:arg:A E:${Kunrecognized}:3:arg:B \
	        E:${Kunrecognized}:4:arg:-a E:${Kunrecognized}:5:arg:-o \
	        E:${Kunrecognized}:6:arg:-x E:${Kunrecognized}:7:arg:-C
	run_check config8 A -as B -a -o -x -C

	expects E:${Kunrecognized}:3:arg:-B E:${Kunrecognized}:4:arg:-a \
	        E:${Kunrecognized}:5:arg:-o E:${Kunrecognized}:6:arg:-x \
	        E:${Kunrecognized}:7:arg:-C
	run_check config8 -A -as -B -a -o -x -C
}

run_config9() {
	# config9: greedy, allow unrecognized flags
	# mandatory opt -x will take anything, A, -A or -x
	# optional opt -o will take A but not -A or -o

	expects E:${Kunrecognized}:1:arg:A E:${Kunrecognized}:2:arg:B
	run_check config9 A B

	expects U:-A:1 U:-B:2
	run_check config9 -A -B

	expects 1,-x::-x
	run_check config9 -x -x

	expects E:${Kmissinginput}:3:flag:-x
	run_check config9 -x -- -x

	expects 1.1,-a 2.1,-a 1.2,-b 2.2,-x::-x
	run_check config9 -ab -ax -x

	expects 1.1,-a 1.2,-x::-ax
	run_check config9 -ax -ax

	expects E:${Kmissinginput_}:3:2:flag:-x
	run_check config9 -ax -- -ax

	expects 1.1,-a 3.1,-a 3.2,-b 1.2,-x::--
	run_check config9 -ax -- -ab

	expects E:${Kunrecognized}:2:arg:A E:${Kunrecognized}:7:arg:D
	run_check config9 -a A -o B -x C D

	expects 1,-a 3,-o 5,-x::-C U:-A:2 U:-B:4 U:-D:7
	run_check config9 -a -A -o -B -x -C -D

	expects E:${Kunrecognized}:6:arg:C
	run_check config9 -x-x -y A --zflag B C

	expects 1,-x:-x 2,-y::-A 4,--zflag::-B
	run_check config9 -x-x -y -A --zflag -B

	expects 1,-x:-x 2,-y::-A 4,--zflag::-B U:-C:6
	run_check config9 -x-x -y -A --zflag -B -C

	expects E:${Kunrecognized}:2:arg:B
	run_check config9 --xflag=A B

	expects 1,--xflag:-A U:-B:2
	run_check config9 --xflag=-A -B

	expects E:${Kunrecognized}:1:arg:A E:${Kunrecognized}:5:arg:D
	run_check config9 A -axB -by C D

	expects 2.1,-a 3.1,-b 2.2,-x:-B 3.2,-y::-C U:-A:1 U:-D:5
	run_check config9 -A -ax-B -by -C -D

	expects 2.1,-a 3.1,-b 2.2,-x:-B 3.2,-y::C U:-A:1 U:-D:5
	run_check config9 -A -ax-B -by C -D

	expects 1,-x::-A
	run_check config9 -x -A

	expects 1,-x::A U:-B:3
	run_check config9 -x A -B

	expects 1,-a 2,--zflag::A U:-B:4
	run_check config9 -a --zflag A -B

	expects 1,-a 2,--zflag::-A
	run_check config9 -a --zflag -A

	expects E:${Kunrecognized}:2:arg:B
	run_check config9 -axA B -C

	expects 1.1,-a 1.2,-x:A U:-B:2
	run_check config9 -axA -B

	expects 1.1,-a 1.2,-x::A U:-B:3
	run_check config9 -ax A -B

	expects 1.1,-a 1.2,-x::-A
	run_check config9 -ax -A

	expects E:${Kbareinput}:1:flag:-a:A E:${Kunrecognized}:2:arg:B
	run_check config9 -aA B

	expects E:${Kbareinput}:1:flag:-a:-A E:${Kunrecognized}:2:arg:B
	run_check config9 -a-A B

	expects E:${Kbareinput}:1:flag:-a:-A
	run_check config9 -a-A -B

	expects E:${Kunrecognized}:1:arg:A E:${Kunrecognized}:3:arg:B \
	        E:${Kunrecognized}:4:arg:-- E:${Kunrecognized}:5:arg:-C
	run_check config9 A -- B -- -C

	expects E:${Kunrecognized}:3:arg:-B E:${Kunrecognized}:4:arg:-- \
	        E:${Kunrecognized}:5:arg:-C
	run_check config9 -A -- -B -- -C

	expects E:${Kunrecognized}:1:arg:A E:${Kunrecognized}:3:arg:B \
	        E:${Kunrecognized}:4:arg:-a E:${Kunrecognized}:5:arg:-o \
	        E:${Kunrecognized}:6:arg:-x E:${Kunrecognized}:7:arg:-C
	run_check config9 A -as B -a -o -x -C

	expects E:${Kunrecognized}:3:arg:-B E:${Kunrecognized}:4:arg:-a \
	        E:${Kunrecognized}:5:arg:-o E:${Kunrecognized}:6:arg:-x \
	        E:${Kunrecognized}:7:arg:-C
	run_check config9 -A -as -B -a -o -x -C
}

run_config10() {
	# config10: greedy, limits on flags, allow args
	#
	# -a: none			-o: none			-x: none			--: 1
	# -b: 2					-p: 2					-y: 2
	# --cflag: 2		--qflag: 1		--zflag: 3
	# -d: 1					-r: 3					-w: 1
	#
	# args: 3

	expects
	run_check config10

	expects 1,-a 2,-a 3,-a 4,-o 5,-o 6,-o 7,-o 8,-x:A 9,-x:B 10,-x:C 11,-x:D \
          12,-- A:--:13 A:--:14
	run_check config10 -a -a -a -o -o -o -o -xA -xB -xC -xD -- -- --

	expects A:A:1 A:B:2 A:C:3
	run_check config10 A B C

	expects 1,-- A:-a:2 A:-o:3 A:-x:4
	run_check config10 -- -a -o -x

	expects 2,-- A:A:1 A:-a:3 A:-b:4
	run_check config10 A -- -a -b

	expects 2.1,-a 2.2,-s A:A:1 A:-as:3 A:B:4
	run_check config10 A -as -as B

	expects 1,-b 2,-b 3,-p 4,-p 5,-y:A 6,-y:B
	run_check config10 -b -b -p -p -yA -yB

	expects 1.1,-b 1.2,-p 3.1,-b 3.2,-y::A 2,-p 5,-y:B
	run_check config10 -bp -p -by A -yB

	expects 1,--zflag:A 2,--cflag 4,--zflag::C 6,--cflag 3,--qflag:B 7,--zflag:D
	run_check config10 --zflag=A --cflag --qflag=B --zflag C --cflag --zflag=D

	expects 1.1,-d 1.2,-w:A 2,-r 3,-r 4,-r
	run_check config10 -dwA -r -r -r

	expects E:${Ksurplus}:3:flag:-b:2 E:${Ksurplus}:4:flag:-b:2 \
	        E:${Ksurplus}:8:flag:--cflag:2 E:${Ksurplus_}:7:2:flag:-d:1 \
					E:${Ksurplus_}:9:1:flag:-d:1 E:${Ksurplus_}:9:2:flag:-d:1 \
          E:${Ksurplus_}:9:3:flag:-d:1 E:${Ksurplus}:10:flag:-d:1
	run_check config10 -b -b -b -b --cflag --cflag -dd --cflag -ddd -d

	expects E:${Ksurplus}:2:flag:-d:1 E:${Kbareinput}:2:flag:-d:A
	run_check config10 -d -dA

	expects E:${Ksurplus}:3:flag:-p:2
	run_check config10 -p -pp -p 

	expects E:${Ksurplus}:3:flag:-p:2
	run_check config10 -p -p -p

	expects 1,-r 2,-r:r 3,-r
	run_check config10 -r -rr -r

	expects 1,-p:p 2,-p:r
	run_check config10 -pp -pr

	expects E:${Ksurplus}:3:flag:-p:2
	run_check config10 -p -p -pp

	expects E:${Ksurplus_}:3:4:flag:-p:2
	run_check config10 -p -p -aaapp

	expects E:${Ksurplus}:2:flag:--qflag:1
	run_check config10 --qflag --qflag

	expects 1,--qflag:--qflag
	run_check config10 --qflag=--qflag

	expects E:${Ksurplus}:2:flag:--qflag:1
	run_check config10 --qflag=--qflag --qflag

	expects E:${Ksurplus}:4:flag:-r:3 E:${Ksurplus}:5:flag:--rflag:3
	run_check config10 -r --rflag -r -r --rflag

	expects 1,-r:r 2,--rflag:--rflag 3,-r:--rflag
	run_check config10 -rr --rflag=--rflag -r--rflag

	expects 1,-y:y 2,-y:y
	run_check config10 -yy -yy

	expects 1,-y:y 2,-y::-y
	run_check config10 -yy -y -y

	expects 1.1,-a 1.2,-y::-aayy
	run_check config10 -ay -aayy

	expects 1.1,-a 1.2,-y::-aayy 3,-y:y
	run_check config10 -ay -aayy -yy

	expects 1.1,-a 1.2,-y::-aayy 3.1,-a 3.2,-y::-ay
	run_check config10 -ay -aayy -ay -ay

	expects E:${Ksurplus_}:5:2:flag:-y:2
	run_check config10 -ay -aayy -ay -ay -aya

	expects 1.1,-a 1.2,-x::-yy 3.1,-a 3.2,-x::--yflag 5.1,-a 5.2,-y::-ay \
          7.1,-a 7.2,-y:a
	run_check config10 -ax -yy -ax --yflag -ay -ay -aya

	expects E:${Ksurplus_}:8:2:flag:-y:2
	run_check config10 -ax -yy -ax --yflag -ay -ay -aya -aya

	expects 1,--zflag::--zflag 3,--zflag::--zflag 5,--zflag::--zflag
	run_check config10 --zflag --zflag --zflag --zflag --zflag --zflag

	expects 1,--zflag:--zflag 2,--zflag:-w 3,--zflag::--zflag
	run_check config10 --zflag=--zflag --zflag=-w --zflag --zflag

	expects E:${Ksurplus_}:3:3:flag:-w:1
	run_check config10 -w A -aaw B

	expects E:${Ksurplus_}:2:2:flag:-w:1
	run_check config10 -awA -awB

	expects E:${Ksurplus_}:1:3:flag:-b:2 E:${Ksurplus_}:1:4:flag:-b:2 \
	        E:${Ksurplus_}:1:5:flag:-b:2 E:${Ksurplus_}:1:6:flag:-b:2
	run_check config10 -bbbbbb

	expects E:${Ksurplus_}:1:3:flag:-b:2 E:${Ksurplus_}:1:4:flag:-b:2 \
	        E:${Kbareinput_}:1:4:flag:-b:A
	run_check config10 -bbbbA

	expects E:${Kunrecognized}:1:flag:-Aa E:${Kunrecognized}:2:flag:-Bp \
	        E:${Kunrecognized}:3:flag:-Cx
	run_check config10 -Aa -Bp -Cx

	expects A:A:1 A:B:2 A:C:3
	run_check config10 A B C

	expects E:${Ksurplus}:4:arg:D:3
	run_check config10 A B C D

	expects A:A:1 A:B:2 3,-- A:-ax:4
	run_check config10 A B -- -ax

	expects E:${Ksurplus}:5:flag:-b:2 E:${Ksurplus}:7:flag:-b:2 \
	        E:${Ksurplus}:8:arg:D:3
	run_check config10 -b A -b B -b C -b D

	expects E:${Ksurplus}:5:flag:--cflag:2 E:${Ksurplus}:7:flag:--cflag:2 \
	        E:${Ksurplus}:8:arg:D:3
	run_check config10 --cflag A --cflag B --cflag C --cflag D

	expects E:${Ksurplus_}:3:1:flag:-b:2 E:${Ksurplus_}:3:2:flag:-b:2 \
	        E:${Ksurplus}:6:arg:D:3
	run_check config10 -bb A -bb B C D

	expects E:${Ksurplus}:4:flag:--dflag:1 E:${Ksurplus}:6:arg:D:3 \
	        E:${Ksurplus}:7:arg:E:3
	run_check config10 -d A B --dflag C D E

	expects E:${Ksurplus}:5:flag:-p:2 E:${Ksurplus}:7:arg:D:3
	run_check config10 -p A -p B -p C D

	expects E:${Ksurplus}:6:flag:--qflag:1 E:${Ksurplus}:5:arg:D:3
	run_check config10 A B --qflag C D --qflag

	expects E:${Ksurplus}:5:flag:-p:2 E:${Ksurplus}:7:arg:G:3 \
	        E:${Ksurplus}:8:arg:H:3
	run_check config10 -pA B -pC D -pE F G H

	expects E:${Ksurplus}:4:flag:--qflag:1 E:${Ksurplus}:6:arg:F:3 \
	        E:${Ksurplus}:7:arg:G:3
	run_check config10 --qflag=A B C --qflag=D E F G

	expects E:${Ksurplus}:7:flag:--rflag:3 E:${Ksurplus}:8:arg:E:3 \
	        E:${Ksurplus}:9:arg:F:3
	run_check config10 -r A -rr B --rflag C --rflag=D E F

	expects E:${Ksurplus}:7:flag:-y:2 E:${Ksurplus}:10:arg:G:3 \
	        E:${Ksurplus}:11:arg:H:3
	run_check config10 -y A B -y C D -y E F G H

	expects E:${Ksurplus}:4:flag:--wflag:1 E:${Ksurplus}:8:arg:F:3 \
	        E:${Ksurplus}:9:arg:G:3
	run_check config10 A --wflag B --wflag C D E F G

	expects E:${Ksurplus}:5:flag:-y:2 E:${Ksurplus}:7:arg:G:3 \
	        E:${Ksurplus}:8:arg:H:3
	run_check config10 -yA B -yC D -yE F G H

	expects E:${Ksurplus}:5:flag:--wflag:1 E:${Ksurplus}:6:arg:F:3 \
	        E:${Ksurplus}:7:arg:G:3
	run_check config10 A --wflag=B C D --wflag=E F G

	expects E:${Ksurplus}:5:flag:--wflag:1 E:${Ksurplus}:7:arg:F:3
	run_check config10 A B -w C --wflag=D E F

	expects E:${Ksurplus_}:9:1:flag:-b:2 E:${Ksurplus_}:10:1:flag:-b:2 \
          E:${Ksurplus_}:9:2:flag:-p:2 E:${Ksurplus_}:10:2:flag:-p:2 \
          E:${Ksurplus}:7:flag:-w:1 E:${Ksurplus}:11:arg:H:3
	run_check config10 -bp A -bpB C -w D -wE F -bp -bpG H

	expects E:${Ksurplus_}:9:1:flag:-b:2 E:${Ksurplus_}:10:1:flag:-b:2 \
          E:${Ksurplus_}:9:2:flag:-p:2 E:${Ksurplus_}:10:2:flag:-p:2 \
          E:${Ksurplus}:7:flag:-w:1 E:${Ksurplus}:11:arg:H:3 \
					E:${Ksurplus}:12:arg:I:3
	run_check config10 -bp A -bpB C -w D -wE F -bp -bpG H I

	expects E:${Ksurplus_}:7:2:flag:-w:1 E:${Ksurplus_}:10:1:flag:-b:2 \
          E:${Ksurplus_}:10:2:flag:-p:2 E:${Ksurplus_}:12:1:flag:-b:2 \
					E:${Ksurplus_}:12:2:flag:-p:2 E:${Ksurplus}:13:arg:J:3 \
					E:${Kunrecognized}:14:flag:-K E:${Ksurplus}:15:flag:-b:2 \
					E:${Kbareinput}:15:flag:-b:L
	run_check config10 -bp -A B -bpC D -awE -aw F G -bp -H -bpI J -K -bL

	expects E:${Kunrecognized}:1:flag:-A E:${Ksurplus_}:2:2:flag:-d:1 \
	        E:${Ksurplus}:6:flag:-p:2 E:${Ksurplus}:11:arg:H:3 \
					E:${Kunrecognized}:12:flag:-I E:${Ksurplus}:14:arg:-d:3 \
					E:${Ksurplus}:15:arg:-p:3 E:${Ksurplus}:16:arg:-w:3 \
					E:${Ksurplus}:17:arg:--:3
	run_check config10 -A -dd -pB -p C -pD E -w F G H -I -- -d -p -w --

	expects E:${Kunrecognized}:3:flag:-B E:${Ksurplus}:5:flag:--dflag:1 \
	        E:${Kunrecognized}:6:flag:--Cflag E:${Ksurplus}:12:flag:--wflag:1 \
					E:${Ksurplus}:14:flag:--rflag:3 E:${Ksurplus}:16:arg:I:3 \
					E:${Kunrecognized}:17:flag:--Jflag E:${Ksurplus}:18:arg:K:3 \
					E:${Ksurplus}:19:arg:L:3
	run_check config10 --dflag A -B --rflag --dflag --Cflag --rflag D \
                     --rflag=E --wflag F --wflag G --rflag H I --Jflag K L

	expects E:${Kunrecognized}:3:flag:-B E:${Ksurplus}:5:flag:--dflag:1 \
	        E:${Kunrecognized}:6:flag:--Cflag E:${Ksurplus}:11:flag:--wflag:1 \
					E:${Ksurplus}:13:flag:--rflag:3 E:${Ksurplus}:15:arg:I:3 \
					E:${Kunrecognized}:16:flag:--Jflag E:${Ksurplus}:17:arg:K:3 \
					E:${Ksurplus}:18:arg:L:3
	run_check config10 --dflag A -B --rflag --dflag --Cflag --rflag D \
                     --rflag=E --wflag=F --wflag G --rflag H I --Jflag K L
}

run_config11() {
	# config11: +/- opts, not greedy, limits on some, no args allowed

	expects 1,-a 2,+a 3,--aflag 4.1,-a 4.2,-b 5.1,+a 5.2,+b
	run_check config11 -a +a --aflag -ab +ab

	expects 1,-o 2,+o 3,--oflag 4,-o:A 5,+o:B 6.1,-a 6.2,-o:C 7.1,+b 7.2,+p:D
	run_check config11 -o +o --oflag -oA +oB -aoC +bpD

	expects 1,-x::A 3,+x::B 5,--xflag::C 7.1,-a 7.2,-y:D 8.1,+b 8.2,+y:E
	run_check config11 -x A +x B --xflag C -ayD +byE

	expects E:${Kbareinput}:1:flag:+a:c E:${Kbareinput}:2:flag:+b:qA \
	        E:${Kbareinput}:3:flag:+a:z E:${Kunrecognized}:4:arg:B
	run_check config11 +ac +bqA +az B

	expects E:${Kunrecognized}:1:flag:+c E:${Kunrecognized}:2:flag:+q \
	        E:${Kunrecognized}:3:flag:+z E:${Kunrecognized}:4:arg:A
	run_check config11 +c +q +z A

	expects E:${Kunrecognized}:1:flag:_A E:${Kunrecognized}:2:flag:_B \
	        E:${Kunrecognized}:3:flag:-C E:${Kunrecognized}:4:flag:+D
	run_check config11 _A _B -C +D

	expects E:${Kmissinginput}:1:flag:+x E:${Kmissinginput}:2:flag:+y \
	        E:${Kmissinginput}:3:flag:-x E:${Kmissinginput}:4:flag:+y \
          E:${Kmissinginput}:5:flag:-x E:${Kunrecognized}:6:flag:_A \
					E:${Kmissinginput}:7:flag:+y E:${Kunrecognized}:8:flag:_B
	run_check config11 +x +y -x +y -x _A +y _B

	expects E:${Ksurplus}:3:flag:+b:2
	run_check config11 +b +b +b

	expects E:${Ksurplus}:3:flag:-b:2
	run_check config11 -b +b -b

	expects E:${Ksurplus}:3:flag:+b:2
	run_check config11 +b -b +b

	expects E:${Ksurplus}:3:flag:-p:2
	run_check config11 -p +pA -p

	expects E:${Ksurplus}:3:flag:+p:2
	run_check config11 +p -pA +p

	expects E:${Ksurplus}:3:flag:+p:2
	run_check config11 -p -p +pA

	expects E:${Kunrecognized}:1:flag:++aflag \
	        E:${Kunrecognized}:2:flag:++oflag \
					E:${Kunrecognized}:3:flag:++xflag E:${Kunrecognized}:4:arg:A
	run_check config11 ++aflag ++oflag ++xflag A

	expects E:${Kunrecognized}:1:flag:-+aflag \
	        E:${Kunrecognized}:2:flag:-+oflag \
					E:${Kunrecognized}:3:flag:-+xflag=A
	run_check config11 -+aflag -+oflag -+xflag=A

	expects E:${Kunrecognized}:1:flag:+-aflag \
	        E:${Kunrecognized}:2:flag:+-oflag \
					E:${Kunrecognized}:3:flag:+-xflag=A
	run_check config11 +-aflag +-oflag +-xflag=A

	expects 1,+a 2,- 3,--pflag 4,-
	run_check config11 +a - --pflag -

	expects 1,- 2,--xflag:A 3,--xflag::B 5,-
	run_check config11 - --xflag=A --xflag B -

	expects E:${Kmissinginput}:1:flag:--xflag E:${Kmissinginput_}:3:2:flag:+x
	run_check config11 --xflag - +ax -

	expects E:${Kbareinput}:1:flag:-a:- E:${Kbareinput}:2:flag:+a:-
	run_check config11 -a- +a-

	expects E:${Kbareinput_}:1:2:flag:-a:- E:${Kbareinput_}:2:2:flag:+a:-
	run_check config11 -aa- +ba-

	expects 1,-o:- 2,+p:-
	run_check config11 -o- +p-

	expects 1,-p 2,+p
	run_check config11 -p +p

	expects 1,+p:A 2,-p:B
	run_check config11 +pA -pB

	expects E:${Ksurplus}:3:flag:+p:2
	run_check config11 -p -pA +p

	expects E:${Ksurplus}:3:flag:-p:2
	run_check config11 +pA +p -pC

	expects 1.1,-a 1.2,-p 2.1,+b 2.2,+p:A
	run_check config11 -ap +bpA

	expects 1.1,-a 1.2,-p:A 2.1,+b 2.2,+p:y
	run_check config11 -apA +bpy

	expects E:${Kbareinput}:1:flag:+a:q
	run_check config11 +aq

	expects E:${Kbareinput}:1:flag:+a:qA
	run_check config11 +aqA

	expects 1,-y::A 3,+y::B
	run_check config11 -y A +y B 

	expects 1,+y:A 2,-y:B
	run_check config11 +yA -yB

	expects E:${Ksurplus}:4:flag:+y:2
	run_check config11 +yA -y B +yC

	expects E:${Ksurplus}:4:flag:-y:2
	run_check config11 -yA +y B -y C

	expects E:${Kmissinginput_}:1:2:flag:+y
	run_check config11 +ay

	expects 1.1,+a 1.2,+y::A
	run_check config11 +ay A

	expects 1,-o 2,-o:A 3,+o:B 4,+o
	run_check config11 -o -oA +oB +o

	expects 1,-x:A 2,-x::B 4,+x:C 5,+x::D
	run_check config11 -xA -x B +xC +x D

	expects 1.1,-c 1.2,-q 2.1,-c 2.2,-q:A 3.1,-c 3.2,-z:B 4.1,-c 4.2,-z::C
	run_check config11 -cq -cqA -czB -cz C

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:+y \
	        E:${Kmissinginput}:3:flag:-z
	run_check config11 -x +y -z +p

	expects E:${Kmissinginput}:1:flag:+x E:${Kmissinginput}:3:flag:-y \
	        E:${Kmissinginput}:5:flag:-z
	run_check config11 +x -a -y +b -z +a

	expects E:${Kmissinginput}:1:flag:+x E:${Kmissinginput}:3:flag:+y \
	        E:${Kmissinginput}:5:flag:-z
	run_check config11 +x -o +y +o -z -o

	expects E:${Kmissinginput}:1:flag:-x E:${Kunrecognized}:2:flag:_A \
	        E:${Kmissinginput}:3:flag:-x E:${Kunrecognized}:4:flag:-B \
          E:${Kmissinginput}:5:flag:-x E:${Kunrecognized}:6:flag:+B
	run_check config11 -x _A -x -B -x +B

	expects E:${Kmissinginput}:1:flag:+x E:${Kunrecognized}:2:flag:_A \
	        E:${Kmissinginput}:3:flag:+x E:${Kunrecognized}:4:flag:-B \
          E:${Kmissinginput}:5:flag:+x E:${Kunrecognized}:6:flag:+B
	run_check config11 +x _A +x -B +x +B

	expects 1,-x::A 3,-o::B
	run_check config11 -x A -o B

	expects 1,+x::A 3,+o::B
	run_check config11 +x A +o B

	expects E:${Kmissinginput}:1:flag:+x E:${Kunrecognized}:2:flag:+A \
	        E:${Kunrecognized}:4:flag:+B
	run_check config11 +x +A +o +B

	expects E:${Kmissinginput}:1:flag:+x E:${Kunrecognized}:2:flag:-A \
	        E:${Kunrecognized}:4:flag:-B
	run_check config11 +x -A +o -B
}

run_config12() {
	# config12: +/- opts, greedy, limits on some, 2 args allowed

	expects 1,-z::+z
	run_check config12 -z +z

	expects 1,+x::-x
	run_check config12 +x -x

	expects 1,-x::+x
	run_check config12 -x +x

	expects 1,+x::+x
	run_check config12 +x +x

	expects 1,+a 2,+x::+z A:A:4 A:B:5
	run_check config12 +a +x +z A B

	expects 1,+x::_A A:B:3
	run_check config12 +x _A B

	expects E:${Kunrecognized}:3:flag:_B
	run_check config12 +x _A _B
}

run_config13() {
	# config13: greedy, no flag marker opts, "~" input marker,
	#           mixed short/long, 2 args
	# a
	# bflag
	# c (short), cflag (long)
	# d (long), dflag (short)
	# 0: e, eflag
	#
	# o
	# pflag
	# q (short), qflag (long)
	# r (long), rflag (short)
	# 0: n (long), nflag (short)
	#
	# x
	# yflag
	# z (short), zflag (long)
	# w (long), wflag (short)
	# 0: v, vflag
	#
	# s, stop
	# 0: t, tstop
	# u (long), ustop (short)

	expects 1,a 2,bflag 3,o 4,pflag 5,x::A 7,yflag:B
	run_check config13 a bflag o pflag x A yflag~B

	expects A:yflag=A:1
	run_check config13 yflag=A

	expects 1.1,a 1.2,a 1.3,c 2.1,a 2.2,o:A 3.1,a 3.2,x::B 5.1,a 5.2,z:C \
          6.1,a 6.2,q
	run_check config13 aac aoA ax B azC aq

	expects E:${Kmissinginput_}:1:5:arg:x
	run_check config13 aaaax

	expects E:${Kbareinput_}:1:4:arg:a:A
	run_check config13 aaaaA

	expects 1.1,c 1.2,dflag
	run_check config13 cdflag

	expects 1.1,c 1.2,dflag 1.3,c
	run_check config13 cdflagc

	expects 1.1,c 1.2,dflag 1.3,q
	run_check config13 cdflagq

	expects 1.1,dflag 1.2,q:A
	run_check config13 dflagqA

	expects 1.1,c 1.2,rflag
	run_check config13 crflag

	expects 1.1,c 1.2,rflag:A
	run_check config13 crflagA

	expects 1.1,dflag 1.2,rflag:A
	run_check config13 dflagrflagA

	expects 1.1,a 1.2,a 1.3,a 1.4,wflag::A
	run_check config13 aaawflag A

	expects 1.1,a 1.2,a 1.3,dflag 1.4,dflag 1.5,a 1.6,a 1.7,wflag:A
	run_check config13 aadflagdflagaawflagA

	expects 1,pflag:A 2,qflag:B 3,r:C
	run_check config13 pflag~A qflag~B r~C

	expects 1,yflag:A 2,zflag:B 3,w:C
	run_check config13 yflag~A zflag~B w~C

	expects 1.1,a 1.2,ustop A:stop:2
	run_check config13 austop stop

	expects E:${Kstopproscribed}:2:arg:t
	run_check config13 a t

	expects E:${Kstopproscribed}:2:arg:tstop
	run_check config13 a tstop

	expects E:${Kstopproscribed_}:1:2:arg:t
	run_check config13 at

	expects E:${Kstopinput}:1:arg:s:A
	run_check config13 sA

	expects E:${Kstopinput_}:1:2:arg:s:A
	run_check config13 asA

	expects E:${Kbareinput}:1:arg:a:~A E:${Kbareinput}:2:arg:bflag:B \
          E:${Kbareinput}:3:arg:c:~C E:${Kbareinput}:4:arg:cflag:D \
          E:${Kbareinput}:5:arg:d:E E:${Kbareinput}:6:arg:dflag:~F 
	run_check config13 a~A bflag~B c~C cflag~D d~E dflag~F

	# Note: acflag~D: 'c' is a short flag, so the error is input for the 'c'
	# flag
	expects E:${Kbareinput_}:1:2:arg:a:~A E:${Kbareinput}:2:arg:a:bflag~B \
          E:${Kbareinput_}:3:2:arg:c:~C E:${Kbareinput_}:4:2:arg:c:flag~D \
          E:${Kbareinput}:5:arg:a:d~E E:${Kbareinput_}:6:2:arg:dflag:~F 
	run_check config13 aa~A abflag~B ac~C acflag~D ad~E adflag~F

	expects 1,pflag:A 2,qflag:B 3,r:C
	run_check config13 pflag~A qflag~B r~C

	expects 1,o:~A 2,q:~B 3,rflag:~C
	run_check config13 o~A q~B rflag~C

	expects 1,yflag:A 2,zflag:B 3,w:C
	run_check config13 yflag~A zflag~B w~C

	expects 1,x:~A 2,z:~B 3,wflag:~C
	run_check config13 x~A z~B wflag~C

	expects E:${Kproscribed_}:1:3:arg:e E:${Kproscribed}:2:arg:eflag \
          E:${Kproscribed}:3:arg:e
	run_check config13 aae eflag e

	expects E:${Kproscribed}:3:arg:n E:${Kproscribed_}:4:2:arg:nflag \
          E:${Kproscribed}:5:arg:nflag
	run_check config13 oa on n anflag nflag

	expects E:${Kproscribed_}:1:2:arg:v E:${Kproscribed}:2:arg:v \
          E:${Kproscribed}:4:arg:vflag E:${Kproscribed}:6:arg:vflag \
          E:${Kproscribed_}:8:7:arg:v E:${Kmissinginput_}:8:7:arg:v
	run_check config13 avA v B vflag C vflag D cccacav

	expects 1.1,a 1.2,s A:as:2 A:yflag~A:3
	run_check config13 as as yflag~A

	expects 1,w:A 2,w::B 4.1,a 4.2,c 4.3,ustop A:-C:5 A:a:6
	run_check config13 w~A w B acustop -C a

	expects E:${Kemptyinput}:1:arg:x:2
	run_check config13 x ""

	expects E:${Kemptyinput_}:4:6:arg:x:5
	run_check config13 a x A accaax ""

	expects E:${Kemptyinput}:1:arg:yflag:2
	run_check config13 yflag ""

	expects E:${Kemptyinput}:2:arg:yflag:3 E:${Kmissinginput}:4:arg:yflag
	run_check config13 a yflag "" yflag

	expects E:${Ksurplus}:4:arg:C:2
	run_check config13 A stop B C

	expects E:${Kstopinput}:1:arg:stop:A
	run_check config13 stop~A

	expects 1,rflag A:A:2
	run_check config13 rflag A

	expects 1,pflag A:A:2
	run_check config13 pflag A

	expects 1,o A:A:2
	run_check config13 o A

	expects 1,rflag::-A
	run_check config13 rflag -A

	expects 1,pflag::-A
	run_check config13 pflag -A

	expects 1,o::-A
	run_check config13 o -A
}

run_config14() {
	# config14: +/-/no opts, def greedy (no), allow empty inputs
	#
	# -a, a, aa, --aa
	# aaa, --aaa
	# aaaa (short)
	#
	# +o, o, oo, ++oo
	# ooo, ++ooo
	# oooo (short)
	#
	# -x, +x, x, xx, --xx, ++xx
	# xxx
	# xxxx (short)

	expects 1,a 2,aa 3,aaa 4,aaaa 5.1,aaaa 5.2,a 6.1,aaaa 6.2,a 6.3,a \
          7.1,aaaa 7.2,a 7.3,a 7.4,a 8.1,aaaa 8.2,aaaa \
          9.1,aaaa 9.2,aaaa 9.3,a
	run_check config14 a aa aaa aaaa aaaaa aaaaaa aaaaaaa aaaaaaaa aaaaaaaaa

	expects 1.1,-a 1.2,-a 2.1,-a 2.2,-a 2.3,-a 3.1,-a 3.2,-a 3.3,-a 3.4,-a \
          4.1,-a 4.2,-a 4.3,-a 4.4,-a 4.5,-a
	run_check config14 -aa -aaa -aaaa -aaaaa

	expects 1,o 2,o:A 3,+o 4,+o:B 5,oo 6,ooo 7,oooo 8,oooo:o
	run_check config14 o oA +o +oB oo ooo oooo ooooo

	expects 1,-x:x 2,+x:x 3,xx::A 5,xxx::B 7,xxxx::C 9,xxxx:x
	run_check config14 -xx +xx xx A xxx B xxxx C xxxxx

	expects E:${Kmissinginput}:3:arg:xx E:${Kmissinginput}:5:arg:xxx \
          E:${Kmissinginput}:7:arg:xxxx
	run_check config14 -xx +xx xx a xxx a xxxx a xxxxx

	expects E:${Kmissinginput}:1:arg:x E:${Kunrecognized}:2:flag:-A
	run_check config14 x -A

	expects E:${Kargempty}:2
	run_check config14 a "" a

	expects 1,xx:_empty_
	run_check config14 xx=

	expects 1,xx::_empty_
	run_check config14 xx ""

	expects E:${Kargempty}:2
	run_check config14 xx= ""

	expects E:${Kargempty}:3
	run_check config14 xx "" ""

	expects 1,oo 2,ooo 3,oo:_empty_ 4,ooo:_empty_
	run_check config14 oo ooo oo= ooo=

	expects 1,oo::_empty_
	run_check config14 oo ""

	expects 1,ooo::_empty_
	run_check config14 ooo ""

	expects E:${Kargempty}:2
	run_check config14 oo= ""

	expects E:${Kargempty}:2
	run_check config14 ooo= ""
}

run_config15() {
	# config15: +/-/no opts, def greedy (no), allow empty inputs, empty args
	#
	# -a, a, aa, --aa
	# aaa, --aaa
	# aaaa (short)
	#
	# +o, o, oo, ++oo
	# ooo, ++ooo
	# oooo (short)
	#
	# -x, +x, x, xx, --xx, ++xx
	# xxx
	# xxxx (short)

	expects 1,a A:_empty_:2 3,a
	run_check config15 a "" a

	expects 1,xx:_empty_
	run_check config15 xx=

	expects 1,xx::_empty_
	run_check config15 xx ""

	expects 1,xx:_empty_ A:_empty_:2
	run_check config15 xx= ""

	expects E:${Kmissinginput}:1:arg:xx
	run_check config15 xx a

	expects 1,xx::_empty_ A:_empty_:3
	run_check config15 xx "" ""

	expects 1,oo 2,ooo 3,oo:_empty_ 4,ooo:_empty_
	run_check config15 oo ooo oo= ooo=

	expects 1,oo A:_empty_:2
	run_check config15 oo ""

	expects 1,ooo A:_empty_:2
	run_check config15 ooo ""

	expects 1,oo:_empty_ A:_empty_:2
	run_check config15 oo= ""

	expects 1,ooo:_empty_ A:_empty_:2
	run_check config15 ooo= ""

	expects A:_empty_:1 2,-a A:_empty_:3 4,a A:_empty_:5 6,+o A:_empty_:7 \
	        8,o A:_empty_:9 10,x::_empty_ A:_empty_:12 13,-x::_empty_ \
					A:_empty_:15 16,+x::_empty_ A:_empty_:18
	run_check config15 "" -a "" a "" +o "" o "" x "" "" -x "" "" +x "" ""
}

run_config16() {
	# config16 def greedy (not greedy), chaining, allow unrecognized
	#
	# -a, a, aa, --aa
	# aaa, --aaa
	# aaaa (short)
	#
	# +o, o, oo, ++oo
	# ooo, ++ooo
	# oooo (short)
	#
	# -x, +x, x, xx, --xx, ++xx
	# xxx
	# xxxx (short)

	expects 1,a 2,o 3,x::A 5,aa 6,oo 7,xx::B 9,aaa 10,ooo 11,xxx::C \
	        13,aaaa 14,oooo 15,xxxx::D
	run_check config16 a o x A aa oo xx B aaa ooo xxx C aaaa oooo xxxx D

	expects 1.1,aaaa 1.2,a 2,oooo:o 3,xxxx:x
	run_check config16 aaaaa ooooo xxxxx

	expects 1.1,a 1.2,o
	run_check config16 ao

	expects 1,o:x
	run_check config16 ox

	expects 1,x:a
	run_check config16 xa

	expects 1.1,-a 1.2,-x:a
	run_check config16 -axa

	expects 1,+o:x
	run_check config16 +ox

	expects U:-A:1 U:--aaA:2 U:+A:3 U:++aaA:4
	run_check config16 -A --aaA +A ++aaA 

	expects 1,++ooo
	run_check config16 ++ooo

	expects U:++oooo:1
	run_check config16 ++oooo

	expects U:--xxx:1
	run_check config16 --xxx

	expects U:++xxx:1
	run_check config16 ++xxx

	expects E:${Kbareinput}:1:flag:-a:A
	run_check config16 -aA

	expects E:${Kbareinput}:1:arg:a:A
	run_check config16 aA

	expects 
	run_check config16 

	expects 
	run_check config16 
}

run_config17() {
	# config17 def greedy (not greedy), no chaining, allow unrecognized
	#
	# -a, a, aa, --aa
	# aaa, --aaa
	# aaaa (short)
	#
	# +o, o, oo, ++oo
	# ooo, ++ooo
	# oooo (short)
	#
	# -x, +x, x, xx, --xx, ++xx
	# xxx
	# xxxx (short)

	expects 1,a 2,o 3,x::A 5,aa 6,oo 7,xx::B 9,aaa 10,ooo 11,xxx::C \
	        13,aaaa 14,oooo 15,xxxx::D
	run_check config17 a o x A aa oo xx B aaa ooo xxx C aaaa oooo xxxx D

	expects E:${Kbareinput}:1:arg:aaaa:a
	run_check config17 aaaaa ooooo xxxxx

	expects E:${Kbareinput}:1:arg:a:o
	run_check config17 ao

	expects 1,o:x
	run_check config17 ox

	expects 1,x:a
	run_check config17 xa

	expects E:${Kbareinput}:1:flag:-a:xa
	run_check config17 -axa

	expects 1,+o:x
	run_check config17 +ox

	expects U:-A:1 U:--aaA:2 U:+A:3 U:++aaA:4
	run_check config17 -A --aaA +A ++aaA 

	expects 1,++ooo
	run_check config17 ++ooo

	expects U:++oooo:1
	run_check config17 ++oooo

	expects U:--xxx:1
	run_check config17 --xxx

	expects U:++xxx:1
	run_check config17 ++xxx

	expects E:${Kbareinput}:1:flag:-a:A
	run_check config17 -aA

	expects E:${Kbareinput}:1:arg:a:A
	run_check config17 aA
}

run_config18() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    no    |    no    |     .      |      .       |
	# ---------+----------+------------+--------------+-
	#

	expects 1,-o::A
	run_check config18 -o A

	expects E:${Kunrecognized}:2:flag:-A
	run_check config18 -o -A

	expects 1,-o 2,-o
	run_check config18 -o -o

	expects 1,--oflag::A
	run_check config18 --oflag A

	expects E:${Kunrecognized}:2:flag:-A
	run_check config18 --oflag -A

	expects 1,--oflag 2,--oflag
	run_check config18 --oflag --oflag

	expects 1.1,-a 1.2,-o::A
	run_check config18 -ao A

	expects E:${Kunrecognized}:2:flag:-A
	run_check config18 -ao -A

	expects 1.1,-a 1.2,-o 2,-o
	run_check config18 -ao -o

	expects 1,-x::A
	run_check config18 -x A

	expects E:${Kmissinginput}:1:flag:-x E:${Kunrecognized}:2:flag:-A
	run_check config18 -x -A

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config18 -x -x

	expects 1,--xflag::A
	run_check config18 --xflag A

	expects E:${Kmissinginput}:1:flag:--xflag E:${Kunrecognized}:2:flag:-A
	run_check config18 --xflag -A

	expects E:${Kmissinginput}:1:flag:--xflag E:${Kmissinginput}:2:flag:--xflag
	run_check config18 --xflag --xflag

	expects 1.1,-a 1.2,-x::A
	run_check config18 -ax A

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kunrecognized}:2:flag:-A
	run_check config18 -ax -A

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config18 -ax -x
}

run_config19() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    no    |    no    |     X      |      .       |
	# ---------+----------+------------+--------------+-
	#

	expects 1,-o A:A:2
	run_check config19 -o A

	expects E:${Kunrecognized}:2:flag:-A
	run_check config19 -o -A

	expects 1,-o 2,-o
	run_check config19 -o -o

	expects 1,--oflag A:A:2
	run_check config19 --oflag A

	expects E:${Kunrecognized}:2:flag:-A
	run_check config19 --oflag -A

	expects 1,--oflag 2,--oflag
	run_check config19 --oflag --oflag

	expects 1.1,-a 1.2,-o A:A:2
	run_check config19 -ao A

	expects E:${Kunrecognized}:2:flag:-A
	run_check config19 -ao -A

	expects 1.1,-a 1.2,-o 2,-o
	run_check config19 -ao -o

	expects 1,-x::A
	run_check config19 -x A

	expects E:${Kmissinginput}:1:flag:-x E:${Kunrecognized}:2:flag:-A
	run_check config19 -x -A

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config19 -x -x

	expects 1,--xflag::A
	run_check config19 --xflag A

	expects E:${Kmissinginput}:1:flag:--xflag E:${Kunrecognized}:2:flag:-A
	run_check config19 --xflag -A

	expects E:${Kmissinginput}:1:flag:--xflag E:${Kmissinginput}:2:flag:--xflag
	run_check config19 --xflag --xflag

	expects 1.1,-a 1.2,-x::A
	run_check config19 -ax A

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kunrecognized}:2:flag:-A
	run_check config19 -ax -A

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config19 -ax -x
}

run_config20() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    no    |    no    |     .      |      X       |
	# ---------+----------+------------+--------------+-
	#

	expects 1,-o::A
	run_check config20 -o A

	expects 1,-o U:-A:2
	run_check config20 -o -A

	expects 1,-o 2,-o
	run_check config20 -o -o

	expects 1,--oflag::A
	run_check config20 --oflag A

	expects 1,--oflag U:-A:2
	run_check config20 --oflag -A

	expects 1,--oflag 2,--oflag
	run_check config20 --oflag --oflag

	expects 1.1,-a 1.2,-o::A
	run_check config20 -ao A

	expects 1.1,-a 1.2,-o U:-A:2
	run_check config20 -ao -A

	expects 1.1,-a 1.2,-o 2,-o
	run_check config20 -ao -o

	expects 1,-x::A
	run_check config20 -x A

	expects E:${Kmissinginput}:1:flag:-x
	run_check config20 -x -A

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config20 -x -x

	expects 1,--xflag::A
	run_check config20 --xflag A

	expects E:${Kmissinginput}:1:flag:--xflag
	run_check config20 --xflag -A

	expects E:${Kmissinginput}:1:flag:--xflag E:${Kmissinginput}:2:flag:--xflag
	run_check config20 --xflag --xflag

	expects 1.1,-a 1.2,-x::A
	run_check config20 -ax A

	expects E:${Kmissinginput_}:1:2:flag:-x
	run_check config20 -ax -A

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config20 -ax -x
}

run_config21() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    no    |    no    |     X      |      X       |
	# ---------+----------+------------+--------------+-
	#

	expects 1,-o A:A:2
	run_check config21 -o A

	expects 1,-o U:-A:2
	run_check config21 -o -A

	expects 1,-o 2,-o
	run_check config21 -o -o

	expects 1,--oflag A:A:2
	run_check config21 --oflag A

	expects 1,--oflag U:-A:2
	run_check config21 --oflag -A

	expects 1,--oflag 2,--oflag
	run_check config21 --oflag --oflag

	expects 1.1,-a 1.2,-o A:A:2
	run_check config21 -ao A

	expects 1.1,-a 1.2,-o U:-A:2
	run_check config21 -ao -A

	expects 1.1,-a 1.2,-o 2,-o
	run_check config21 -ao -o

	expects 1,-x::A
	run_check config21 -x A

	expects E:${Kmissinginput}:1:flag:-x
	run_check config21 -x -A

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config21 -x -x

	expects 1,--xflag::A
	run_check config21 --xflag A

	expects E:${Kmissinginput}:1:flag:--xflag
	run_check config21 --xflag -A

	expects E:${Kmissinginput}:1:flag:--xflag E:${Kmissinginput}:2:flag:--xflag
	run_check config21 --xflag --xflag

	expects 1.1,-a 1.2,-x::A
	run_check config21 -ax A

	expects E:${Kmissinginput_}:1:2:flag:-x
	run_check config21 -ax -A

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config21 -ax -x
}

run_config22() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    lax   |    no    |     .      |      .       |
	# ---------+----------+------------+--------------+-
	#

	expects 1,-o::A
	run_check config22 -o A

	expects 1,-o::-A
	run_check config22 -o -A

	expects 1,-o 2,-o
	run_check config22 -o -o

	expects 1,--oflag::A
	run_check config22 --oflag A

	expects 1,--oflag::-A
	run_check config22 --oflag -A

	expects 1,--oflag 2,--oflag
	run_check config22 --oflag --oflag

	expects 1.1,-a 1.2,-o::A
	run_check config22 -ao A

	expects 1.1,-a 1.2,-o::-A
	run_check config22 -ao -A

	expects 1.1,-a 1.2,-o 2,-o
	run_check config22 -ao -o

	expects 1,-x::A
	run_check config22 -x A

	expects 1,-x::-A
	run_check config22 -x -A

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config22 -x -x

	expects 1,--xflag::A
	run_check config22 --xflag A

	expects 1,--xflag::-A
	run_check config22 --xflag -A

	expects E:${Kmissinginput}:1:flag:--xflag E:${Kmissinginput}:2:flag:--xflag
	run_check config22 --xflag --xflag

	expects 1.1,-a 1.2,-x::A
	run_check config22 -ax A

	expects 1.1,-a 1.2,-x::-A
	run_check config22 -ax -A

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config22 -ax -x
}

run_config23() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    lax   |    no    |     X      |      .       |
	# ---------+----------+------------+--------------+-
	#

	expects 1,-o A:A:2
	run_check config23 -o A

	expects 1,-o::-A
	run_check config23 -o -A

	expects 1,-o 2,-o
	run_check config23 -o -o

	expects 1,--oflag A:A:2
	run_check config23 --oflag A

	expects 1,--oflag::-A
	run_check config23 --oflag -A

	expects 1,--oflag 2,--oflag
	run_check config23 --oflag --oflag

	expects 1.1,-a 1.2,-o A:A:2
	run_check config23 -ao A

	expects 1.1,-a 1.2,-o::-A
	run_check config23 -ao -A

	expects 1.1,-a 1.2,-o 2,-o
	run_check config23 -ao -o

	expects 1,-x::A
	run_check config23 -x A

	expects 1,-x::-A
	run_check config23 -x -A

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config23 -x -x

	expects 1,--xflag::A
	run_check config23 --xflag A

	expects 1,--xflag::-A
	run_check config23 --xflag -A

	expects E:${Kmissinginput}:1:flag:--xflag E:${Kmissinginput}:2:flag:--xflag
	run_check config23 --xflag --xflag

	expects 1.1,-a 1.2,-x::A
	run_check config23 -ax A

	expects 1.1,-a 1.2,-x::-A
	run_check config23 -ax -A

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config23 -ax -x
}

run_config24() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    lax   |    no    |     .      |      X       |
	# ---------+----------+------------+--------------+-
	#

	expects 1,-o::A
	run_check config24 -o A

	expects 1,-o U:-A:2
	run_check config24 -o -A

	expects 1,-o 2,-o
	run_check config24 -o -o

	expects 1,--oflag::A
	run_check config24 --oflag A

	expects 1,--oflag U:-A:2
	run_check config24 --oflag -A

	expects 1,--oflag 2,--oflag
	run_check config24 --oflag --oflag

	expects 1.1,-a 1.2,-o::A
	run_check config24 -ao A

	expects 1.1,-a 1.2,-o U:-A:2
	run_check config24 -ao -A

	expects 1.1,-a 1.2,-o 2,-o
	run_check config24 -ao -o

	expects 1,-x::A
	run_check config24 -x A

	expects 1,-x::-A
	run_check config24 -x -A

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config24 -x -x

	expects 1,--xflag::A
	run_check config24 --xflag A

	expects 1,--xflag::-A
	run_check config24 --xflag -A

	expects E:${Kmissinginput}:1:flag:--xflag E:${Kmissinginput}:2:flag:--xflag
	run_check config24 --xflag --xflag

	expects 1.1,-a 1.2,-x::A
	run_check config24 -ax A

	expects 1.1,-a 1.2,-x::-A
	run_check config24 -ax -A

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config24 -ax -x
}

run_config25() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    lax   |    no    |     X      |      X       |
	# ---------+----------+------------+--------------+-
	#

	expects 1,-o A:A:2
	run_check config25 -o A

	expects 1,-o U:-A:2
	run_check config25 -o -A

	expects 1,-o 2,-o
	run_check config25 -o -o

	expects 1,--oflag A:A:2
	run_check config25 --oflag A

	expects 1,--oflag U:-A:2
	run_check config25 --oflag -A

	expects 1,--oflag 2,--oflag
	run_check config25 --oflag --oflag

	expects 1.1,-a 1.2,-o A:A:2
	run_check config25 -ao A

	expects 1.1,-a 1.2,-o U:-A:2
	run_check config25 -ao -A

	expects 1.1,-a 1.2,-o 2,-o
	run_check config25 -ao -o

	expects 1,-x::A
	run_check config25 -x A

	expects 1,-x::-A
	run_check config25 -x -A

	expects E:${Kmissinginput}:1:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config25 -x -x

	expects 1,--xflag::A
	run_check config25 --xflag A

	expects 1,--xflag::-A
	run_check config25 --xflag -A

	expects E:${Kmissinginput}:1:flag:--xflag E:${Kmissinginput}:2:flag:--xflag
	run_check config25 --xflag --xflag

	expects 1.1,-a 1.2,-x::A
	run_check config25 -ax A

	expects 1.1,-a 1.2,-x::-A
	run_check config25 -ax -A

	expects E:${Kmissinginput_}:1:2:flag:-x E:${Kmissinginput}:2:flag:-x
	run_check config25 -ax -x
}

run_config26() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    yes   |    no    |     .      |      .       |
	# ---------+----------+------------+--------------+-
	#

	expects 1,-o::A
	run_check config26 -o A

	expects 1,-o::-A
	run_check config26 -o -A

	expects 1,-o 2,-o
	run_check config26 -o -o

	expects 1,--oflag::A
	run_check config26 --oflag A

	expects 1,--oflag::-A
	run_check config26 --oflag -A

	expects 1,--oflag 2,--oflag
	run_check config26 --oflag --oflag

	expects 1.1,-a 1.2,-o::A
	run_check config26 -ao A

	expects 1.1,-a 1.2,-o::-A
	run_check config26 -ao -A

	expects 1.1,-a 1.2,-o 2,-o
	run_check config26 -ao -o

	expects 1,-x::A
	run_check config26 -x A

	expects 1,-x::-A
	run_check config26 -x -A

	expects 1,-x::-x
	run_check config26 -x -x

	expects 1,--xflag::A
	run_check config26 --xflag A

	expects 1,--xflag::-A
	run_check config26 --xflag -A

	expects 1,--xflag::--xflag
	run_check config26 --xflag --xflag

	expects 1.1,-a 1.2,-x::A
	run_check config26 -ax A

	expects 1.1,-a 1.2,-x::-A
	run_check config26 -ax -A

	expects 1.1,-a 1.2,-x::-x
	run_check config26 -ax -x
}

run_config27() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    yes   |    no    |     X      |      .       |
	# ---------+----------+------------+--------------+-
	#

	expects 1,-o A:A:2
	run_check config27 -o A

	expects 1,-o::-A
	run_check config27 -o -A

	expects 1,-o 2,-o
	run_check config27 -o -o

	expects 1,--oflag A:A:2
	run_check config27 --oflag A

	expects 1,--oflag::-A
	run_check config27 --oflag -A

	expects 1,--oflag 2,--oflag
	run_check config27 --oflag --oflag

	expects 1.1,-a 1.2,-o A:A:2
	run_check config27 -ao A

	expects 1.1,-a 1.2,-o::-A
	run_check config27 -ao -A

	expects 1.1,-a 1.2,-o 2,-o
	run_check config27 -ao -o

	expects 1,-x::A
	run_check config27 -x A

	expects 1,-x::-A
	run_check config27 -x -A

	expects 1,-x::-x
	run_check config27 -x -x

	expects 1,--xflag::A
	run_check config27 --xflag A

	expects 1,--xflag::-A
	run_check config27 --xflag -A

	expects 1,--xflag::--xflag
	run_check config27 --xflag --xflag

	expects 1.1,-a 1.2,-x::A
	run_check config27 -ax A

	expects 1.1,-a 1.2,-x::-A
	run_check config27 -ax -A

	expects 1.1,-a 1.2,-x::-x
	run_check config27 -ax -x
}

run_config28() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    yes   |    no    |     .      |      X       |
	# ---------+----------+------------+--------------+-
	#

	expects 1,-o::A
	run_check config28 -o A

	expects 1,-o U:-A:2
	run_check config28 -o -A

	expects 1,-o 2,-o
	run_check config28 -o -o

	expects 1,--oflag::A
	run_check config28 --oflag A

	expects 1,--oflag U:-A:2
	run_check config28 --oflag -A

	expects 1,--oflag 2,--oflag
	run_check config28 --oflag --oflag

	expects 1.1,-a 1.2,-o::A
	run_check config28 -ao A

	expects 1.1,-a 1.2,-o U:-A:2
	run_check config28 -ao -A

	expects 1.1,-a 1.2,-o 2,-o
	run_check config28 -ao -o

	expects 1,-x::A
	run_check config28 -x A

	expects 1,-x::-A
	run_check config28 -x -A

	expects 1,-x::-x
	run_check config28 -x -x

	expects 1,--xflag::A
	run_check config28 --xflag A

	expects 1,--xflag::-A
	run_check config28 --xflag -A

	expects 1,--xflag::--xflag
	run_check config28 --xflag --xflag

	expects 1.1,-a 1.2,-x::A
	run_check config28 -ax A

	expects 1.1,-a 1.2,-x::-A
	run_check config28 -ax -A

	expects 1.1,-a 1.2,-x::-x
	run_check config28 -ax -x
}

run_config29() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    yes   |    no    |     X      |      X       |
	# ---------+----------+------------+--------------+-
	#

	expects 1,-o A:A:2
	run_check config29 -o A

	expects 1,-o U:-A:2
	run_check config29 -o -A

	expects 1,-o 2,-o
	run_check config29 -o -o

	expects 1,--oflag A:A:2
	run_check config29 --oflag A

	expects 1,--oflag U:-A:2
	run_check config29 --oflag -A

	expects 1,--oflag 2,--oflag
	run_check config29 --oflag --oflag

	expects 1.1,-a 1.2,-o A:A:2
	run_check config29 -ao A

	expects 1.1,-a 1.2,-o U:-A:2
	run_check config29 -ao -A

	expects 1.1,-a 1.2,-o 2,-o
	run_check config29 -ao -o

	expects 1,-x::A
	run_check config29 -x A

	expects 1,-x::-A
	run_check config29 -x -A

	expects 1,-x::-x
	run_check config29 -x -x

	expects 1,--xflag::A
	run_check config29 --xflag A

	expects 1,--xflag::-A
	run_check config29 --xflag -A

	expects 1,--xflag::--xflag
	run_check config29 --xflag --xflag

	expects 1.1,-a 1.2,-x::A
	run_check config29 -ax A

	expects 1.1,-a 1.2,-x::-A
	run_check config29 -ax -A

	expects 1.1,-a 1.2,-x::-x
	run_check config29 -ax -x
}

run_config30() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    no    |    yes   |     .      |      .       |
	# ---------+----------+------------+--------------+-
	#

	expects 
	run_check config30 -o A

	expects 
	run_check config30 -o -A

	expects 
	run_check config30 -o -o

	expects 
	run_check config30 --oflag A

	expects 
	run_check config30 --oflag -A

	expects 
	run_check config30 --oflag --oflag

	expects 
	run_check config30 -ao A

	expects 
	run_check config30 -ao -A

	expects 
	run_check config30 -ao -o

	expects 
	run_check config30 -x A

	expects 
	run_check config30 -x -A

	expects 
	run_check config30 -x -x

	expects 
	run_check config30 --xflag A

	expects 
	run_check config30 --xflag -A

	expects 
	run_check config30 --xflag --xflag

	expects 
	run_check config30 -ax A

	expects 
	run_check config30 -ax -A

	expects 
	run_check config30 -ax -x
}

run_config31() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    no    |    yes   |     X      |      .       |
	# ---------+----------+------------+--------------+-
	#

	expects 
	run_check config31 -o A

	expects 
	run_check config31 -o -A

	expects 
	run_check config31 -o -o

	expects 
	run_check config31 --oflag A

	expects 
	run_check config31 --oflag -A

	expects 
	run_check config31 --oflag --oflag

	expects 
	run_check config31 -ao A

	expects 
	run_check config31 -ao -A

	expects 
	run_check config31 -ao -o

	expects 
	run_check config31 -x A

	expects 
	run_check config31 -x -A

	expects 
	run_check config31 -x -x

	expects 
	run_check config31 --xflag A

	expects 
	run_check config31 --xflag -A

	expects 
	run_check config31 --xflag --xflag

	expects 
	run_check config31 -ax A

	expects 
	run_check config31 -ax -A

	expects 
	run_check config31 -ax -x
}

run_config32() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    no    |    yes   |     .      |      X       |
	# ---------+----------+------------+--------------+-
	#

	expects 
	run_check config32 -o A

	expects 
	run_check config32 -o -A

	expects 
	run_check config32 -o -o

	expects 
	run_check config32 --oflag A

	expects 
	run_check config32 --oflag -A

	expects 
	run_check config32 --oflag --oflag

	expects 
	run_check config32 -ao A

	expects 
	run_check config32 -ao -A

	expects 
	run_check config32 -ao -o

	expects 
	run_check config32 -x A

	expects 
	run_check config32 -x -A

	expects 
	run_check config32 -x -x

	expects 
	run_check config32 --xflag A

	expects 
	run_check config32 --xflag -A

	expects 
	run_check config32 --xflag --xflag

	expects 
	run_check config32 -ax A

	expects 
	run_check config32 -ax -A

	expects 
	run_check config32 -ax -x
}

run_config33() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    no    |    yes   |     X      |      X       |
	# ---------+----------+------------+--------------+-
	#

	expects 
	run_check config33 -o A

	expects 
	run_check config33 -o -A

	expects 
	run_check config33 -o -o

	expects 
	run_check config33 --oflag A

	expects 
	run_check config33 --oflag -A

	expects 
	run_check config33 --oflag --oflag

	expects 
	run_check config33 -ao A

	expects 
	run_check config33 -ao -A

	expects 
	run_check config33 -ao -o

	expects 
	run_check config33 -x A

	expects 
	run_check config33 -x -A

	expects 
	run_check config33 -x -x

	expects 
	run_check config33 --xflag A

	expects 
	run_check config33 --xflag -A

	expects 
	run_check config33 --xflag --xflag

	expects 
	run_check config33 -ax A

	expects 
	run_check config33 -ax -A

	expects 
	run_check config33 -ax -x
}

run_config34() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    lax   |    yes   |     .      |      .       |
	# ---------+----------+------------+--------------+-
	#

	expects 
	run_check config34 -o A

	expects 
	run_check config34 -o -A

	expects 
	run_check config34 -o -o

	expects 
	run_check config34 --oflag A

	expects 
	run_check config34 --oflag -A

	expects 
	run_check config34 --oflag --oflag

	expects 
	run_check config34 -ao A

	expects 
	run_check config34 -ao -A

	expects 
	run_check config34 -ao -o

	expects 
	run_check config34 -x A

	expects 
	run_check config34 -x -A

	expects 
	run_check config34 -x -x

	expects 
	run_check config34 --xflag A

	expects 
	run_check config34 --xflag -A

	expects 
	run_check config34 --xflag --xflag

	expects 
	run_check config34 -ax A

	expects 
	run_check config34 -ax -A

	expects 
	run_check config34 -ax -x
}

run_config35() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    lax   |    yes   |     X      |      .       |
	# ---------+----------+------------+--------------+-
	#

	expects 
	run_check config35 -o A

	expects 
	run_check config35 -o -A

	expects 
	run_check config35 -o -o

	expects 
	run_check config35 --oflag A

	expects 
	run_check config35 --oflag -A

	expects 
	run_check config35 --oflag --oflag

	expects 
	run_check config35 -ao A

	expects 
	run_check config35 -ao -A

	expects 
	run_check config35 -ao -o

	expects 
	run_check config35 -x A

	expects 
	run_check config35 -x -A

	expects 
	run_check config35 -x -x

	expects 
	run_check config35 --xflag A

	expects 
	run_check config35 --xflag -A

	expects 
	run_check config35 --xflag --xflag

	expects 
	run_check config35 -ax A

	expects 
	run_check config35 -ax -A

	expects 
	run_check config35 -ax -x
}

run_config36() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    lax   |    yes   |     .      |      X       |
	# ---------+----------+------------+--------------+-
	#

	expects 
	run_check config36 -o A

	expects 
	run_check config36 -o -A

	expects 
	run_check config36 -o -o

	expects 
	run_check config36 --oflag A

	expects 
	run_check config36 --oflag -A

	expects 
	run_check config36 --oflag --oflag

	expects 
	run_check config36 -ao A

	expects 
	run_check config36 -ao -A

	expects 
	run_check config36 -ao -o

	expects 
	run_check config36 -x A

	expects 
	run_check config36 -x -A

	expects 
	run_check config36 -x -x

	expects 
	run_check config36 --xflag A

	expects 
	run_check config36 --xflag -A

	expects 
	run_check config36 --xflag --xflag

	expects 
	run_check config36 -ax A

	expects 
	run_check config36 -ax -A

	expects 
	run_check config36 -ax -x
}

run_config37() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    lax   |    yes   |     X      |      X       |
	# ---------+----------+------------+--------------+-
	#

	expects 
	run_check config37 -o A

	expects 
	run_check config37 -o -A

	expects 
	run_check config37 -o -o

	expects 
	run_check config37 --oflag A

	expects 
	run_check config37 --oflag -A

	expects 
	run_check config37 --oflag --oflag

	expects 
	run_check config37 -ao A

	expects 
	run_check config37 -ao -A

	expects 
	run_check config37 -ao -o

	expects 
	run_check config37 -x A

	expects 
	run_check config37 -x -A

	expects 
	run_check config37 -x -x

	expects 
	run_check config37 --xflag A

	expects 
	run_check config37 --xflag -A

	expects 
	run_check config37 --xflag --xflag

	expects 
	run_check config37 -ax A

	expects 
	run_check config37 -ax -A

	expects 
	run_check config37 -ax -x
}

run_config38() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    yes   |    yes   |     .      |      .       |
	# ---------+----------+------------+--------------+-
	#

	expects 
	run_check config38 -o A

	expects 
	run_check config38 -o -A

	expects 
	run_check config38 -o -o

	expects 
	run_check config38 --oflag A

	expects 
	run_check config38 --oflag -A

	expects 
	run_check config38 --oflag --oflag

	expects 
	run_check config38 -ao A

	expects 
	run_check config38 -ao -A

	expects 
	run_check config38 -ao -o

	expects 
	run_check config38 -x A

	expects 
	run_check config38 -x -A

	expects 
	run_check config38 -x -x

	expects 
	run_check config38 --xflag A

	expects 
	run_check config38 --xflag -A

	expects 
	run_check config38 --xflag --xflag

	expects 
	run_check config38 -ax A

	expects 
	run_check config38 -ax -A

	expects 
	run_check config38 -ax -x
}

run_config39() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    yes   |    yes   |     X      |      .       |
	# ---------+----------+------------+--------------+-
	#

	expects 
	run_check config39 -o A

	expects 
	run_check config39 -o -A

	expects 
	run_check config39 -o -o

	expects 
	run_check config39 --oflag A

	expects 
	run_check config39 --oflag -A

	expects 
	run_check config39 --oflag --oflag

	expects 
	run_check config39 -ao A

	expects 
	run_check config39 -ao -A

	expects 
	run_check config39 -ao -o

	expects 
	run_check config39 -x A

	expects 
	run_check config39 -x -A

	expects 
	run_check config39 -x -x

	expects 
	run_check config39 --xflag A

	expects 
	run_check config39 --xflag -A

	expects 
	run_check config39 --xflag --xflag

	expects 
	run_check config39 -ax A

	expects 
	run_check config39 -ax -A

	expects 
	run_check config39 -ax -x
}

run_config40() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    yes   |    yes   |     .      |      X       |
	# ---------+----------+------------+--------------+-
	#

	expects 
	run_check config40 -o A

	expects 
	run_check config40 -o -A

	expects 
	run_check config40 -o -o

	expects 
	run_check config40 --oflag A

	expects 
	run_check config40 --oflag -A

	expects 
	run_check config40 --oflag --oflag

	expects 
	run_check config40 -ao A

	expects 
	run_check config40 -ao -A

	expects 
	run_check config40 -ao -o

	expects 
	run_check config40 -x A

	expects 
	run_check config40 -x -A

	expects 
	run_check config40 -x -x

	expects 
	run_check config40 --xflag A

	expects 
	run_check config40 --xflag -A

	expects 
	run_check config40 --xflag --xflag

	expects 
	run_check config40 -ax A

	expects 
	run_check config40 -ax -A

	expects 
	run_check config40 -ax -x
}

run_config41() {
	#   greedy   optional   allow args   allow unrec.
	#          |  greedy  |            |    flags     |
	# ---------+----------+------------+--------------+-
	#    yes   |    yes   |     X      |      X       |
	# ---------+----------+------------+--------------+-
	#

	expects 
	run_check config41 -o A

	expects 
	run_check config41 -o -A

	expects 
	run_check config41 -o -o

	expects 
	run_check config41 --oflag A

	expects 
	run_check config41 --oflag -A

	expects 
	run_check config41 --oflag --oflag

	expects 
	run_check config41 -ao A

	expects 
	run_check config41 -ao -A

	expects 
	run_check config41 -ao -o

	expects 
	run_check config41 -x A

	expects 
	run_check config41 -x -A

	expects 
	run_check config41 -x -x

	expects 
	run_check config41 --xflag A

	expects 
	run_check config41 --xflag -A

	expects 
	run_check config41 --xflag --xflag

	expects 
	run_check config41 -ax A

	expects 
	run_check config41 -ax -A

	expects 
	run_check config41 -ax -x
}

run_config42() {

	expects 
	run_check config42 
}

run_config43() {

	expects 
	run_check config43 
}

run_config44() {

	expects 
	run_check config44 
}

run_config45() {

	expects 
	run_check config45 
}

run_config46() {

	expects 
	run_check config46 
}

run_config47() {

	expects 
	run_check config47 
}

run_config48() {

	expects 
	run_check config48 
}

run_config49() {

	expects 
	run_check config49 
}

run_config50() {

	expects 
	run_check config50 
}


#expects 
#run_check config2 


# 1000: empty argument .  .  .  .  .  .  .  (pos)
# 2000: unrecognized flag/arg               (pos,         type, opt)
# 3000: surplus input  .  .  .  .  .  .  .  (pos,         type, opt, input)
# 3010: surplus input                       (pos, subpos, type, opt, input)
# 3100: proscribed flag   .  .  .  .  .  .  (pos,         type, opt)
# 3110: proscribed flag                     (pos, subpos, type, opt)
# 3101: stop proscribed flag .  .  .  .  .  (pos,         type, opt)
# 3111: stop proscribed flag                (pos, subpos, type, opt)
# 4000: empty input .  .  .  .  .  .  .  .  (pos,         type, opt)
# 4010: empty input                         (pos, subpos, type, opt)
# 4100: does not expect an input   .  .  .  (pos,         type, opt)
# 4101: stop flag does not expect an input  (pos,         type, opt)
# 4200: does not take an input  .  .  .  .  (pos,         type, opt, input)
# 4210: does not take an input              (pos, subpos, type, opt, input)
# 4201: stop flag does not take an input .  (pos,         type, opt, input)
# 4211: stop flag does not take an input    (pos, subpos, type, opt, input)
# 4300: expected input missing  .  .  .  .  (pos,         type, opt)
# 4310: expected input missing              (pos, subpos, type, opt)


run_tests() {
	if [[ -z "$@" ]]; then
		run_config1
		run_config2
		run_config3
		run_config4
		run_config5
		run_config6
		run_config7
		run_config8
		run_config9
		run_config10
		run_config11
		run_config12
		run_config13
		run_config14
		run_config15
		run_config16
		run_config17
		run_config18
		run_config19
		run_config20
		run_config21
		run_config22
		run_config23
		run_config24
		run_config25
		run_config26
		run_config27
		run_config28
		run_config29
		#run_config30
		#run_config31
		#run_config32
		#run_config33
		#run_config34
		#run_config35
		#run_config36
		#run_config37
		#run_config38
		#run_config39
		#run_config40
		#run_config41
	else
		for i in "$@"; do
			case "$i" in
				config1) run_config1 ;;
				config2) run_config2 ;;
				config3) run_config3 ;;
				config4) run_config4 ;;
				config5) run_config5 ;;
				config6) run_config6 ;;
				config7) run_config7 ;;
				config8) run_config8 ;;
				config9) run_config9 ;;
				config10) run_config10 ;;
				config11) run_config11 ;;
				config12) run_config12 ;;
				config13) run_config13 ;;
				config14) run_config14 ;;
				config15) run_config15 ;;
				config16) run_config16 ;;
				config17) run_config17 ;;
				config18) run_config18 ;;
				config19) run_config19 ;;
				config20) run_config20 ;;
				config21) run_config21 ;;
				config22) run_config22 ;;
				config23) run_config23 ;;
				config24) run_config24 ;;
				config25) run_config25 ;;
				config26) run_config26 ;;
				config27) run_config27 ;;
				config28) run_config28 ;;
				config29) run_config29 ;;
				config30) run_config30 ;;
				config31) run_config31 ;;
				config32) run_config32 ;;
				config33) run_config33 ;;
				config34) run_config34 ;;
				config35) run_config35 ;;
				config36) run_config36 ;;
				config37) run_config37 ;;
				config38) run_config38 ;;
				config39) run_config39 ;;
				config40) run_config40 ;;
				config41) run_config41 ;;
				*)
					echo "error: configuration set '$i' not recognized." >&2
					exit 4
					;;
			esac
		done
	fi
}

exists $check_prog
run_tests "$@"
exit $exit_code

