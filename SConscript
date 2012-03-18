import os, sys, platform

Import("env")

# Test program
sources = [ "test.cpp" ]
objects = env.Object(source = sources)
testprogram = env.Program("test", source = objects)

# Benchmark program
sources = [ "benchmark.cpp" ]
objects = env.Object(source = sources)
benchmarkprogram = env.Program("benchmark", source = objects)

Default([testprogram, benchmarkprogram])
