#!/bin/bash
#Script to compile the program, and subit it to the loadleveller queue.

# Enter the path of the GIGAWORD corpus within the quotes in front of
# corpus_path variable in the form /home/sp2/Gigaword/
# Enter the path only up till the parent directory and not that of the
# subdirectories.
# The results for the queries would be created in a directory within the
# program directory.

#corpus_path = /scratch1/cs862108/GIGAWORD-ENGLISH-2/data/
CorpusSize=16000  #in MB (uncompressed)

ProcessorCount=${1-50}
Subset=${2-100}

MemoryNeeded=$(($CorpusSize * 2 * $Subset / ($ProcessorCount - 1) / 100 + 100))

if [ "$MemoryNeeded" -gt "2000" ]; then
  echo "Too few processors. (increaese the number)"
  exit 0
fi

echo "Processor Count: $ProcessorCount"
echo "Corpus Subset:   $Subset%"
echo "Memory Needed:   $MemoryNeeded"
echo ""


if [ -f "a.out" ]; then
  rm a.out
fi

echo "Compiling (please wait)..."

mpCC_r -O4 -bmaxdata:0x80000000 -lz -L"/usr/local/lib/" \
    master.cpp master_routines.cpp common.cpp \
    index.cpp search.cpp &> compile_msgs

if [ "$?" -ne "0" ]; then
  echo "***  Compile failed.  Check file 'compile_msgs' for details.  ***"
  exit 1
fi

echo "Success"

if [ -f "results" ]; then
  echo "Please remove the file named results.  I want a directory with that name."
  exit 2
fi

if [ -d "results" ]; then
  rm -rf results
fi

mkdir results

if [ -f "cerro.cmd" ]; then
  rm -f cerro.cmd
fi

echo "#!/bin/csh" > cerro.cmd
echo "#@ output = master.out" >> cerro.cmd 
echo "#@ error = master.err" >> cerro.cmd
echo "#@ job_type = parallel" >> cerro.cmd
echo "#@ wall_clock_limit = 00:12:00" >> cerro.cmd
echo "#@ resources = ConsumableMemory($MemoryNeeded)" >> cerro.cmd
echo "#@ network.MPI = css0,shared,US" >> cerro.cmd
echo "#@ blocking = unlimited" >> cerro.cmd
echo "#@ total_tasks = $ProcessorCount" >> cerro.cmd
echo "#@ node_usage = shared" >> cerro.cmd
echo "#@ queue" >> cerro.cmd
echo "" >> cerro.cmd
echo "./a.out -index $Indexer -balancer $LoadBalancer $Subset" >> cerro.cmd
echo "" >> cerro.cmd


llsubmit cerro.cmd >& submit_msgs
if [ "$?" -ne "0" ]; then
  echo "***  Submit failed.  Check file 'submit_msgs' for details.  ***"
  exit 1
fi

echo ""
echo "The job has been submitted.  The results will be saved in the results directory"
echo ""
echo "Submitted jobs:"
llq -u "$USER"

#End of script.
