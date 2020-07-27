using coverage-guided fuzzers on long-running programs

compile target.c with trace-cmp and trace-pc
when running target, LD_PRELOAD your hook library

hook library communicates directly with client.c

client.c interfaces with honggfuzz/afl/etc
