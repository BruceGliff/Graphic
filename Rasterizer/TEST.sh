# This script launches the program with the caches statistic
perf stat -B -e cache-references,cache-misses,cycles,instructions,branches,branch-misses,faults,migrations ./launch
