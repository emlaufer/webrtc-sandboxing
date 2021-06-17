#!/bin/bash

for i in {1..1}
do
  ./out/Default/modules_unittests --gtest_filter=*Fec* >> bench_out_no_sand.txt
done

