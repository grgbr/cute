#!/bin/sh

srcdir=$(dirname $(realpath $0))
builddir=$(realpath $srcdir/../build)

for t in $srcdir/cute_test_*.c; do
	base=$(basename $t)
	echo "======== ${base%%.c} ========"
	$builddir/${base%%.c}
done
