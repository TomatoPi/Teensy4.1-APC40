
echo "Runnig from $(pwd)"
echo "Compiling Tests for Utility library"

dllist="utils/containers/tests-double_linked_list"

TESTDIR="unit_tests"
BUILDIDR="build/unit_tests"
LOGSDIR="logs"

mkdir -p $BUILDIDR/utils/containers/
mkdir -p $LOGSDIR

g++ -Wall -Werror -Isrc/ $TESTDIR/$dllist.cpp -o $BUILDIDR/$dllist && $BUILDIDR/$dllist > $LOGSDIR/dllist.log
