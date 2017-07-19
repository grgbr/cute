#!/bin/sh

if [ -z "$testbindir" ]; then
	testbindir=__LIBEXECDIR__/cute
fi

for t in $testbindir/static/cute_test_*; do
	base=$(basename $t)
	echo "======== static $base ========"
	$t
	echo
done

for t in $testbindir/shared/cute_test_*; do
	base=$(basename $t)
	echo "======== shared $base ========"
	$t
	echo
done
