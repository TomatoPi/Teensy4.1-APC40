
set -e

echo "Runnig from $(pwd)"
echo "Compiling Tests for Utility library"

DL_LIST="utils/containers/tests-double_linked_list"
QUEUES="utils/containers/tests-queues"
SETS="utils/containers/tests-set"

ASYNC="utils/async/tests-async"

LOGGING="utils/logging/tests-logging"

TESTDIR="unit_tests"
BUILDIDR="build/unit_tests"
LOGSDIR="logs"

mkdir -p $BUILDIDR/utils/containers/
mkdir -p $BUILDIDR/utils/logging/
mkdir -p $BUILDIDR/utils/mycelium/
mkdir -p $BUILDIDR/utils/async/
mkdir -p $LOGSDIR

INCLUDES="-Imycelium/ \
    -Imycelium/src \
    -Imycelium/src/utils \
    -Imycelium/src/utils/containers \
    -Imycelium/src/utils/mycelium \
    -Imycelium/src/utils/async \
"

# ===== DOUBLE LINKED LIST =====

LOGFILE="$LOGSDIR/double-linked-list.log"

echo "Testing $DL_LIST"
date > $LOGFILE
g++ -g -Wall -Werror $INCLUDES $TESTDIR/$DL_LIST.cpp -o $BUILDIDR/$DL_LIST >> $LOGFILE && $BUILDIDR/$DL_LIST >> $LOGFILE

if [ $? -eq 0 ]; then
    echo " ... passed"
else
    echo " ... failed"
    exit
fi

date >> $LOGFILE

# ===== QUEUE =====

LOGFILE="$LOGSDIR/queues.log"

echo "Testing $QUEUES"
date > $LOGFILE
g++ -g -Wall -Werror $INCLUDES $TESTDIR/$QUEUES.cpp -o $BUILDIDR/$QUEUES >> $LOGFILE && $BUILDIDR/$QUEUES >> $LOGFILE

if [ $? -eq 0 ]; then
    echo " ... passed"
else
    echo " ... failed"
    exit
fi

date >> $LOGFILE

# ===== SET =====

LOGFILE="$LOGSDIR/sets.log"

echo "Testing $SETS"
date > $LOGFILE
g++ -g -Wall -Werror $INCLUDES $TESTDIR/$SETS.cpp -o $BUILDIDR/$SETS >> $LOGFILE && $BUILDIDR/$SETS >> $LOGFILE

if [ $? -eq 0 ]; then
    echo " ... passed"
else
    echo " ... failed"
    exit
fi

date >> $LOGFILE

# ===== ASYNC =====

LOGFILE="$LOGSDIR/async.log"

echo "Testing $ASYNC"
date > $LOGFILE
g++ -g -Wall -Werror $INCLUDES $TESTDIR/$ASYNC.cpp -o $BUILDIDR/$ASYNC >> $LOGFILE && $BUILDIDR/$ASYNC >> $LOGFILE

if [ $? -eq 0 ]; then
    echo " ... passed"
else
    echo " ... failed"
    exit
fi

date >> $LOGFILE
exit


LOGFILE="$LOGSDIR/logging.log"

echo "Testing $logging"
echo "" > $LOGFILE
g++ -pg -g -Wall -Werror $INCLUDES $TESTDIR/$logging.cpp -o $BUILDIDR/$logging && $BUILDIDR/$logging >> $LOGFILE

diff $LOGFILE $LOGFILE.expected

if [ $? -eq 0 ]; then
    echo " ... passed"
else
    echo " ... failed: missmatch between expected output and result output"
    exit
fi
