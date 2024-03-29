#!/bin/sh -e
################################################################################
# SPDX-License-Identifier: LGPL-3.0-only
#
# This file is part of CUTe.
# Copyright (C) 2023 Grégor Boirie <gregor.boirie@free.fr>
################################################################################

libexecdir="${LIBEXECDIR:-@@LIBEXECDIR@@/cute}"
testsrcdir="${TESTSRCDIR:-${libexecdir}}"
testdir="${TMPDIR:-/tmp}/cute-utest"

hrule='..........................................................................'

adjust='
BEGIN {
	stat=0
}

/^[[:blank:]]*source: .*/ {
	print gensub("source: [^[:blank:]]+/(test/[^:]+):[0-9]+", "source: \\1",
	             1,
	             $0);
}

/^found:/ || /^[[:blank:]]+actual:/ {
	gsub("0x[0-9a-fA-F]+", "0x????????????????", $0);
}

/^NAME[[:blank:]]+[#[:blank:](.A-Z]+/ {
	stat=1
}

! /^[[:blank:]]*source: .*/ {
	if (stat == 1)
		gsub("[ ]{0,3}[0-9]+.[0-9]{6}", "????.??????", $0);
	print $0;
}
'

pass()
{
	local case="$1"

	printf "%.${cols}s %s\n" "$case $hrule" "pass" >&2
}

fail()
{
	local case="$1"
	local msg="$2"

	printf "%.${cols}s %s\n" "$case $hrule" "fail" >&2

	shift 1
	echo "$msg" >&2
}

clean_testcase()
{
	local case="$1"

	rm -f $testdir/$case-outref.txt \
	      $testdir/$case-stdout.txt.tmp \
	      $testdir/$case-stdout.txt
}

run_testcase()
{
	local case="$1"
	local exp=$2
	local stat=0

	if ! [ -f $libexecdir/$case ]; then
		fail "$case" "missing testcase binary."
		return 1
	fi

	trap "clean_testcase \"$case\"" HUP INT QUIT TERM

	if ! awk "$adjust" \
	          $testsrcdir/$case-outref.txt \
	          > $testdir/$case-outref.txt; then
		fail "$case" "cannot generate output reference."
		return 1
	fi
	
	if ! $libexecdir/$case run \
	     >$testdir/$case-stdout.txt.tmp 2>/dev/null; then
		stat=1
	fi
	if [ $stat -ne $exp ]; then
	        mv $testdir/$case-stdout.txt.tmp $testdir/$case-stdout.txt
		fail "$case" "test case exited with unexpected status."
		return 1
	fi

	if ! awk "$adjust" $testdir/$case-stdout.txt.tmp \
	     > $testdir/$case-stdout.txt; then
		fail "$case" "cannot generate test case output."
		return 1
	fi
	
	if ! cmp -s $testdir/$case-stdout.txt $testdir/$case-outref.txt; then
		fail "$case" "test case output differ from reference output."
		diff $testdir/$case-outref.txt $testdir/$case-stdout.txt
		return 1
	else
		pass "$case"
	fi

	clean_testcase "$case"
	trap - HUP INT QUIT TERM

	return 0
}

ret=0
mkdir -p ${testdir}
cols=30

run_testcase "simple-exec-pass-utest" 0 || ret=1
run_testcase "simple-exec-fail-utest" 1 || ret=1
run_testcase "simple-exec-error-utest" 1 || ret=1
run_testcase "simple-setup-fail-utest" 1 || ret=1
run_testcase "simple-setup-error-utest" 1 || ret=1
run_testcase "simple-teardown-fail-utest" 1 || ret=1
run_testcase "simple-fixture-fail-utest" 1 || ret=1
run_testcase "simple-tmout-utest" 1 || ret=1
run_testcase "simple-iodir-utest" 1 || ret=1
run_testcase "check-bool-utest" 1 || ret=1
run_testcase "check-sint-utest" 1 || ret=1
run_testcase "check-uint-utest" 1 || ret=1
run_testcase "check-hex-utest" 1 || ret=1
run_testcase "check-flt-utest" 1 || ret=1
run_testcase "check-dbl-utest" 1 || ret=1
run_testcase "check-str-utest" 1 || ret=1
run_testcase "check-ptr-utest" 1 || ret=1
run_testcase "check-mem-utest" 1 || ret=1
run_testcase "expect-bool-utest" 1 || ret=1
run_testcase "expect-sint-utest" 1 || ret=1
run_testcase "expect-uint-utest" 1 || ret=1
run_testcase "expect-hex-utest" 1 || ret=1
run_testcase "expect-flt-utest" 1 || ret=1
run_testcase "expect-str-utest" 1 || ret=1
run_testcase "expect-ptr-utest" 1 || ret=1
run_testcase "expect-mem-utest" 1 || ret=1
run_testcase "expect-assert-utest" 1 || ret=1

rmdir --ignore-fail-on-non-empty ${testdir}

exit $ret
