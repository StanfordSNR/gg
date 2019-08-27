# Fibonacci Calculation Using `gg`

[Fibonacci number](https://en.wikipedia.org/wiki/Fibonacci_number)

## Prerequisites

Follow [the instructions](https://github.com/StanfordSNR/gg/blob/master/README.md#build-directions) to compile and install the latest version of `gg` on your machine.

## Calculation

To demonstrate the way that gg handles dynamic dataflows, we used the C++ SDK to implement a classic recursive Fibonacci program in the gg IR. You can read more on page ten of [the paper](https://www.usenix.org/system/files/atc19-fouladi.pdf).

### Run

There is one simple entry point to run the demo:

```sh
./bin/run.sh <N> <JOBS-COUNT>
```

Example:

```sh
./bin/run.sh 10 10
# 1. Clear workspace
# 2. Initialize gg
# Initialized gg directory at: /app/gg/examples/fibonacci/.gg
# 3. Create thunks for number 10
# TIUi7P.HZPSfs0EsjWHBuJ5E8fItQQrIC_Z_b8eo05mM00000125
# VOIOYOzqPDKzUblafpIDyiGTKyymk0dPT_CKLaRdBFZA004c5670
# V5tpA.GDOUdjLVlBznCj61Fi8HvqGhIcV67Ni3YvOkEE001bc520
# 4. Upload bins to storage (s3\Redis) to use them from serverless functions
# 5. Run calculation
# → Loading the thunks...  done (26 ms).
# No files to upload.
# ↘ Downloading output file (3.0 B)... done (596 ms).
# 6. Result: 55
```
