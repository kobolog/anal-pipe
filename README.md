Log parser concept with pluggable analysis modules
--------------------------------------------------

Exemplary usage:

```(sh)
cmake . && make
./parser --help
./parser /path/to/apache.log --distribution-of code --distribution-of method \
  --top-of ip --errors-by-url --qps-by-url --windowed-qps 3600
```

Prerequisites: Boost 1.40+.

Comments on the code
--------------------

It's not super-fast as it uses stringstreams to parse the log file, and, of course, it can be done much faster using simple FSM-like tokenizer based on "store-up-to"/"skip-up-to" command pair.

Leftover features: partial time-based log analysis (trivial via bisect), file tailing (trivial via inotify/kqueue with file truncation/inode change checking for rotating logs).

The approach chosen is a dynamic iterative analyzer pipeline, with analysis objects attachable at runtime. Obvious possible improvements are: analysis object composition (a-la ``top-of`` + ``distribution-of``).