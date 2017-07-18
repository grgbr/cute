#!/bin/sh

srcdir=$(dirname $(realpath $0))
builddir=$(realpath $srcdir/../build)

for t in $srcdir/cute_test_*.c; do
	base=$(basename $t)
	echo "======== static ${base%%.c} ====================================="
	$builddir/static/${base%%.c}
	echo
	echo "======== shared ${base%%.c} ====================================="
	env LD_LIBRARY_PATH=$builddir $builddir/shared/${base%%.c}
	echo
done
