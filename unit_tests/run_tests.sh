
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

if [ $? -eq 0 ]; then
    echo " ... passed"
else
    echo " ... failed"
fi

date >> $LOGFILE


LOGFILE="$LOGSDIR/queues.log"

echo "Testing $queues"
date > $LOGFILE
g++ -g -Wall -Werror -Imycelium/src/ $TESTDIR/$queues.cpp -o $BUILDIDR/$queues >> $LOGFILE && $BUILDIDR/$queues >> $LOGFILE

if [ $? -eq 0 ]; then
    echo " ... passed"
else
    echo " ... failed"
fi

date >> $LOGFILE


LOGFILE="$LOGSDIR/logging.log"

echo "Testing $logging"
echo "" > $LOGFILE
g++ -pg -g -Wall -Werror -Imycelium/src/ $TESTDIR/$logging.cpp -o $BUILDIDR/$logging && $BUILDIDR/$logging >> $LOGFILE

diff $LOGFILE $LOGFILE.expected

if [ $? -eq 0 ]; then
    echo " ... passed"
else
    echo " ... failed: missmatch between expected output and result output"
fi
