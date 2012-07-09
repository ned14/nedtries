import os, sys, platform

Import("env")

# Test program
sources = [ "test.c" ]
objects = env.Object("test_c", source = sources)
testprogram_c = env.Program("test_c", source = objects)
sources = Command('test.cpp', 'test.c', Copy('test.cpp', 'test.c'))
objects = env.Object("test_cpp", source = sources)
testprogram_cpp = env.Program("test_cpp", source = objects)

# Benchmark program
sources = Command('benchmark.c', 'benchmark.cpp', Copy('benchmark.c', 'benchmark.cpp'))
objects = env.Object("benchmark_c", source = sources)
benchmarkprogram_c = env.Program("benchmark_c", source = objects)
sources = [ "benchmark.cpp" ]
objects = env.Object("benchmark_cpp", source = sources)
benchmarkprogram_cpp = env.Program("benchmark_cpp", source = objects)

Default([testprogram_c, benchmarkprogram_c, testprogram_cpp, benchmarkprogram_cpp])
