# Simple English Sentence Clustering Tool

This tool can cluster sentences together by using cosine similarity
on the sentence components.

This program can be compiled on my Manjaro. I am not sure about other platforms.

## Compilation
Simply run this command. You will find the final output as `./build/cluster.exe`

```sh
mkdir build
make clusterer
```

## Basic operation

A file can be supplied as a standard input to the program.


```sh
./build/cluster.exe < test.txt
```

However, the output may be useless as it only outputs the clustering information.
Here are the command line arguments that you can customise on the clustering
application.


### `--verbose` (`-V`) - verbose settings

This flag will output the current clustering
settings. By default, this is false.

### `--threshold` (`-t`) - similarity threshold

This argument will override the current clustering similarity threshold. 1 is for clustering the exact copy of the sentence (may vary due to lancaster stemming). 0 is for random clustering. The default value is `0.85`.

### `--shuffle` (`-r`) - shuffle datapoints

This argument will shuffle the clustering datapoints before performing the clustering. The default value is `true`.

### `--strategy` (`-s`) - stemming strategy

There are two acceptable stemming strategy: `stem` and `char-gram`. `stem` uses the sentence's lancaster-stem to compare for their similarity score. `char-gram` adds an extra step to concatenate all stems into one string and perform N-grams on the string. The default value is `stem`.

### `--gram` (`-g`) - gram settings

This setting is only valid when the stemming strategy is set to `char-gram`. Otherwise, the program will raise no notice at all. The argument value must be a series of comma-separated gramming settings. Each gramming settings consists of two value separated by a colon: a gram size, and a gram stride. Here is a valid gram settings:

```
3:2,5:3
```

This means the 3-grams will be performed on the string with 2 characters stride. For example, the string "iwill" (from the sentence "I will.") will produce two sentence components: "iwi", "ill". Then, the similar process on the next settings will be done to generate the complete grammed components for the sentence. The default value is `3:3,5:3`.


### `--stdout` (`-O`) 

This will print all cluster result to the standard output. The default value is `false`.

### `--outdir` (`-o`)

This will create a collection of files for each cluster in the selected output directory. The default value is empty.

## Quick example

Run this to run a quick test on the program.

```sh
mkdir build
make clusterer
./build/clusterer.exe -O < ./test.txt
```