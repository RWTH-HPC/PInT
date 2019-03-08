<h1>How to install and use the tool</h1>
<h2>1. Prerequisites</h2>
1) Install clang 5 with libTooling support<br>
2) Install cmake 3.4.1 (There appear to be problems with other versions)<br>
3) Clone this git repository to your machine

<h2>2. Installing the tool</h2>
1) Change into the source directory<br>
2) Create a new directory: <code>mkdir build</code><br>
3) Change into the new directory: <code>cd build</code><br>
4) Create the build system: <code>cmake ..</code><br>
5) Compile the tool: <code>make</code>

<h2>3. Using the tool</h2>
<h3>3.1 Instrumentation of your code</h3>
1) Include pattern instrumentation headers (Mind the C and CPP versions) in your source file<br>
2) Add instrumenation commands at the beginning and end of code regions that belong to a pattern

<figure>
  <figcaption>Example for code instrumentation</figcaption>
  <pre>
    <code>
    PatternInstrumentation::PatternBegin("SupportingStructure LoopParallelism MainParLoop");
    #pragma omp for
    for (int i = 0; i < MAX_ITERATIONS; i++)
        someCode();
    PatternInstrumentation::PatternEnd("MainParLoop");
    </code>
  </pre>
</figure>

The formal Syntax of the PatternBegin expression is as follows:<br>
<b>'DesignSpace PatternName Identifier'</b><br>
where DesignSpace is either FindingConcurrency, AlgorithmStructure, SupportingStructure or ImplementationMechanism,<br>
PatternName is the name of the pattern employed in this code region,<br>
and the Identifier is a name for this exact occurence of the pattern.<br>
Identifiers can be re-used to indicate to the tool, that two (or more) code regions belong together.<br><br>
Please note that patterns that due to implementation, pattern regions have to be closed in the opposite order in which they are opened (First Opened - Last Closed).

<h3>3.2 Creating a Compilation Database</h3>
PInT is a clang-based tool.
Therefore, it requires compilation databases in order to obtain the compiler flags used to build your source code.<br>

<b>If you have a CMake project for your source code</b><br>
1) In your CMakeLists.txt, set the variable "EXPORT_COMPILE_COMMANDS" to 1 or ON: <code>SET(EXPORT_COMPILE_COMMANDS 1)</code><br>
2) Create/update your build system: <code>mkdir build && cd build && cmake ..</code><br>
3) Copy compile_commands.json to the directory containing the sources: <code>cp compilation_commands.json path/to/src<br>

<b>If you use make (or another build tool) to build your source code</b><br>
Install the <a href="https://github.com/rizsotto/Bear">Bear Tool</a>.
The tool intercepts the exec calls made by your build tool and creates a compilation database from this.
Make sure to copy this compilation database to the directory of the source code.
If you don't want to install the Bear Tool, you can copy the compilation commands from the make file.
The syntax of the compilation database is covered in the <a href="https://clang.llvm.org/docs/JSONCompilationDatabase.html">JSON Compilation Database Format Specification</a>.

<b>If you want to write the compilation database from scratch by yourself</b> consider the following tutorials: <a href="https://eli.thegreenplace.net/2014/05/21/compilation-databases-for-clang-based-tools/">Compilation databases for Clang-based tools</a> and <a href="https://clang.llvm.org/docs/JSONCompilationDatabase.html">JSON Compilation Database Format Specification</a>

<h3>3.3 Running the tool</h3>
You can call the tool from its build directory like this: <code>./HPC-pattern-tool /path/to/your/code.cpp</code>.
Arbitrary additional arguments can be passed after <code>--extra-arg=</code>.<br><br>
You'll have to tell PInT where the instrumentation header files are located.
You can copy the instrumentation header files to the source directory.
Alternatively, you can add an include flag using <code>--extra-arg=</code>, e.g. <code>./HPC-pattern-tool /path/to/your/code.cpp --extra-arg=-I/path/to/headers</code> to your tool call.
<b>If you use cmake</b>, you can instead add the flag to the list of include directories with <code>include_directories(/path/to/headers)</code>.
Finally, you can also add the <code>-I/path/to/headers</code> flag to the compilation database for the files where the instrumentation header is used.
<br>
If you want to see the <b>Patterntree without the function calls </b> you can set the onption -onlyPattern=1. <br>
<code>./HPC-pattern-tool /path/to/your/code.cpp -onlyPattern=1 --extra-arg=-I/path/to/headers</code><br>
If you want <b>no tree </b> you can set the onption -noTree=1. <br>
<code>./HPC-pattern-tool /path/to/your/code.cpp -noTree=1 --extra-arg=-I/path/to/headers</code>