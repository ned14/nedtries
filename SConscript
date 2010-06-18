import os, sys, platform

Import("env")

# Test program
sources = [ "benchmark.cpp" ]
objects = env.Object(source = sources)
testprogram = env.Program("benchmark", source = objects)

Default(testprogram)
