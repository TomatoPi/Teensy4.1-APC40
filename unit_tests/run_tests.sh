
echo "Runnig from $(pwd)"
echo "Compiling Tests for Utility library"

dllist="utils/containers/tests-double_linked_list"
queues="utils/containers/tests-queues"

TESTDIR="unit_tests"
BUILDIDR="build/unit_tests"
LOGSDIR="logs"

mkdir -p $BUILDIDR/utils/containers/
mkdir -p $LOGSDIR


LOGFILE="$LOGSDIR/dllist.log"

date > $LOGFILE
g++ -g -Wall -Werror -Imycelium/src/ $TESTDIR/$dllist.cpp -o $BUILDIDR/$dllist >> $LOGFILE && $BUILDIDR/$dllist >> $LOGFILE
date >> $LOGFILE


LOGFILE="$LOGSDIR/queues.log"

date > $LOGFILE
g++ -g -Wall -Werror -Imycelium/src/ $TESTDIR/$queues.cpp -o $BUILDIDR/$queues >> $LOGFILE && $BUILDIDR/$queues >> $LOGFILE
date >> $LOGFILE
