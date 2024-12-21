#!/bin/bash

# 要运行的程序
PROGRAM="./antivirus_trie_parallel"

# 运行次数
RUNS=10

# 总时间初始化为0
total_time=0

# 运行程序并累加时间
for i in $(seq 1 $RUNS); do
    start_time=$(date +%s.%N)
    $PROGRAM
    end_time=$(date +%s.%N)
    elapsed=$(echo "$end_time - $start_time" | bc)
    total_time=$(echo "$total_time + $elapsed" | bc)
    echo "Run $i: $elapsed seconds"
done

# 计算平均时间
average_time=$(echo "$total_time / $RUNS" | bc -l)
echo "Average time: $average_time seconds"