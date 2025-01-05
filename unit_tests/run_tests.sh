
echo "Runnig from $(pwd)"
echo "Compiling Tests for Utility library"

dllist="utils/containers/tests-double_linked_list"
queues="utils/containers/tests-queues"

logging="utils/logging/tests-logging"

TESTDIR="unit_tests"
BUILDIDR="build/unit_tests"
LOGSDIR="logs"

mkdir -p $BUILDIDR/utils/containers/
mkdir -p $BUILDIDR/utils/logging/
mkdir -p $BUILDIDR/utils/mycelium/
mkdir -p $LOGSDIR


LOGFILE="$LOGSDIR/dllist.log"

echo "Testing $dllist"
date > $LOGFILE
g++ -g -Wall -Werror -Imycelium/src/ $TESTDIR/$dllist.cpp -o $BUILDIDR/$dllist >> $LOGFILE && $BUILDIDR/$dllist >> $LOGFILE
date >> $LOGFILE


LOGFILE="$LOGSDIR/queues.log"

echo "Testing $queues"
date > $LOGFILE
g++ -g -Wall -Werror -Imycelium/src/ $TESTDIR/$queues.cpp -o $BUILDIDR/$queues >> $LOGFILE && $BUILDIDR/$queues >> $LOGFILE
date >> $LOGFILE


LOGFILE="$LOGSDIR/logging.log"

echo "Testing $logging"
date > $LOGFILE
g++ -g -Wall -Werror -Imycelium/src/ $TESTDIR/$logging.cpp -o $BUILDIDR/$logging >> $LOGFILE && $BUILDIDR/$logging >> $LOGFILE
date >> $LOGFILE
