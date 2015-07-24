import os, sys, platform

AddOption('--debugbuild', dest='debugbuild', action='store_const', const=1, default=0, help='Builds output with debug settings')
AddOption('--optdebugbuild', dest='debugbuild', action='store_const', const=2, help='enable optimised debug build')
AddOption('--useclang', dest='useclang', nargs='?', const=True, help='use clang if it is available')
AddOption('--analyze', dest='analyze', nargs='?', const=True, help='have the compiler do static analysis')
env = Environment()

# Force scons to always use absolute paths in everything (helps debuggers to find source files)
env['CCCOM']   =    env['CCCOM'].replace('$CHANGED_SOURCES','$SOURCES.abspath')
env['SHCCCOM'] =  env['SHCCCOM'].replace('$CHANGED_SOURCES','$SOURCES.abspath')
env['CXXCOM']  =   env['CXXCOM'].replace('$CHANGED_SOURCES','$SOURCES.abspath')
env['SHCXXCOM']= env['SHCXXCOM'].replace('$CHANGED_SOURCES','$SOURCES.abspath')
# Always have a colour terminal
try:
    env['ENV']['TERM']=os.environ['TERM']
except:
    pass
architecture="generic"
env['CPPDEFINES']=[]
env['CCFLAGS']=[]
env['LIBS']=[]
env['LINKFLAGS']=[]
env['CCFLAGSFORNEDMALLOC']=[]

# Am I in a 32 or 64 bit environment? Note that not specifying --sse doesn't set any x86 or x64 specific options
# so it's good to go for ANY platform
if sys.platform=="win32":
    # Even the very latest scons still screws this up :(
    if 'INCLUDE' not in os.environ: raise Exception('Are you running this from inside a MSVC tools environment?')
    env['ENV']['INCLUDE']=os.environ['INCLUDE']
    env['ENV']['LIB']=os.environ['LIB']
env['ENV']['PATH']=os.environ['PATH']

# Am I building a debug or release build?
if env.GetOption('debugbuild') is not None:
    env['CPPDEFINES']+=["DEBUG", "_DEBUG"]
    variant=architecture+"/Debug"
else:
    env['CPPDEFINES']+=["NDEBUG"]
    variant=architecture+"/Release"

# Am I building for Windows or POSIX?
if sys.platform=='win32':
    env['CPPDEFINES']+=["WIN32", "_WINDOWS", "UNICODE", "_UNICODE"]
    env['CCFLAGS']+=["/EHsc"]           # Turn on exception handling
    env['CCFLAGS']+=["/GF"]             # Eliminate duplicate strings
    env['CCFLAGS']+=["/Gy"]             # Seperate COMDATs
    env['CCFLAGS']+=["/Zi"]             # Program database debug info
    env['CCFLAGS']+=["/wd4503"]         # Decorated name length exceeded
    if env.GetOption('debugbuild')==1:
        env['CCFLAGS']+=["/Od", "/MDd"]
    elif env.GetOption('debugbuild')==2:
        env['CCFLAGS']+=["/O2", "/MDd"]
    else:
        env['CCFLAGS']+=["/O2", "/MD"]
    env['LIBS']+=["psapi", "user32", "advapi32"]
    env['LINKFLAGS']+=["/DEBUG"]                # Output debug symbols
    env['LINKFLAGS']+=["/LARGEADDRESSAWARE"]    # Works past 2Gb
    env['LINKFLAGS']+=["/NXCOMPAT"]             # Likes no execute

    env['LINKFLAGS']+=["/MANIFEST"]             # Be UAC compatible
    
    if env.GetOption('debugbuild') is None:
        env['LINKFLAGS']+=["/OPT:REF", "/OPT:ICF"]  # Eliminate redundants
    if env.GetOption('analyze'):
        env['CCFLAGS']+=["/analyze"]
else:
    if env.GetOption('useclang'):
        try:
            env['CC']=os.environ['CC']
        except:
            env['CC']="clang"
        try:
            env['CXX']=os.environ['CXX']
        except:
            env['CXX']="clang++"
    env['CPPDEFINES']+=[]
    env['CCFLAGS']+=["-Wall", "-Wextra", "-Wno-unused-variable", "-Wno-unused-function", "-Wno-unused-but-set-variable", "-Wno-unused-parameter"]
    env['CXXFLAGS']+=["-std=gnu++0x"]
    if env.GetOption('debugbuild') is not None:
        env['CCFLAGS']+=["-O0", "-g"]
    else:
        env['CCFLAGS']+=["-O2", "-g"]
    if env.GetOption('analyze'):
        env['CCFLAGS']+=["--analyze"]
    env['LIBS']+=["rt", "m"]
    env['LINKFLAGS']+=[]

# Build
nedmalloclib=SConscript("SConscript", variant_dir=variant, duplicate=False, exports="env")
