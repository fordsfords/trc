#!/bin/bash
# tst.sh - build and test the project.

ASSRT() {
  eval "test $1"

  if [ $? -ne 0 ]; then
    echo "ASSRT ERROR `basename ${BASH_SOURCE[1]}`:${BASH_LINENO[0]}, not true: '$1'" >&2
    exit 1
  fi
}  # ASSRT


# Update doc table of contents (see https://github.com/fordsfords/mdtoc).
if which mdtoc.pl >/dev/null; then mdtoc.pl -b "" README.md;
elif [ -x ../mdtoc/mdtoc.pl ]; then ../mdtoc/mdtoc.pl -b "" README.md;
else echo "FYI: mdtoc.pl not found; see https://github.com/fordsfords/mdtoc"
fi


gcc -Wall -pthread -o trc_test cprt.c trc.c trc_test.c -l pthread ; ASSRT "$? -eq 0"

./trc_test -h >x.1 2>&1 ; ASSRT "$? -eq 0"
egrep "^[Ww]here:" x.1 >/dev/null ; ASSRT "$? -eq 0"


# This test ignores the env vars.
TRC_NUM_ENTRIES=11 TRC_CREATE_FLAGS=0x0f ./trc_test -t 1 >x.1 2>&1 ; ASSRT "$? -eq 0"
# Check for unexpected lines
egrep -v "^Test [0-9]*...OK$" x.1 >x.2 ; ASSRT "! -s x.2"
egrep "^Test [0-9]*\.\.\.OK$" x.1 ; ASSRT "! -s x.2"


# This test uses the env vars (atomic inc).
TRC_NUM_ENTRIES=11 TRC_CREATE_FLAGS=0x0f ./trc_test -t 2 >x.1 2>&1 ; ASSRT "$? -eq 0"
# Check for unexpected lines
egrep -v "^Test [0-9]*...OK$" x.1 >x.2 ; ASSRT "! -s x.2"
egrep "^Test [0-9]*\.\.\.OK$" x.1 ; ASSRT "! -s x.2"
