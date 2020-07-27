# Covfefuzz
Experiments in compiler-provided coverage instrumentation for fuzzing long-running programs. All code should be considered work-in-progress.

## Running it
Run `make`, then `make fuzzy`.
To run it, first run your `target` program. In this instance, that will be `./target`.

Get the SHMEM ID that the target creates for itself (TODO: make this process less awkward) and pass it as an argument to the fuzzer (which is `a.out` after you run `make fuzzy`, until I update the Makefile).

# Other experiments
See (TODO: add the files) for stuff about hooking into AFL's coverage mechanism by grabbing `__AFL_SHM_ID` and attaching it yourself. This allows you to pass your own coverage information, possibly gathered from other sources.

This started out as an 'afl-client', which would connect to a long-running process and gather coverage information as it performs a specific action. I realised that there's no point using afl to do this, as I can use fsanitize=trace-pc instead. I'll throw the code up here anyway for prosterity, as it might interest others.
