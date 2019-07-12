<h1>How to install and use the tool</h1>
<h2>1. Prerequisites</h2>
1) Install clang 5 with libTooling support<br>
2) Install cmake 3.4.1 (There appear to be problems with other versions)<br>
3) Clone this git repository to your machine

<h2>2. Installing the tool</h2>
1) Change path of clang include directory which is stored in the Variable CLANG_INCLUDE_DIR to the path to your clang include directory.
   I.e. the code  <code>set (CLANG_INCLUDE_DIR "/rwthfs/rz/SW/UTIL/clang/7.0.0/lib64/clang/7.0.0" CACHE STRING "Directory where the clang system library header files are located")</code> has to be changed to <code>set (CLANG_INCLUDE_DIR "/path/to/your/clang/installation/clang/7.0.0/lib64/clang/7.0.0" CACHE STRING "Directory where the clang system library header files are located")</code><br>
2) Change into the source directory<br>
3) Create a new directory: <code>mkdir build</code><br>
4) Change into the new directory: <code>cd build</code><br>
5) Create the build system: <code>cmake ..</code><br>
6) Compile the tool: <code>make</code>

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
3) Copy compile_commands.json to the directory containing the sources: <code>cp compilation_commands.json path/to/src</code><br>

<b>If you use make (or another build tool) to build your source code</b><br>
Install the <a href="https://github.com/rizsotto/Bear">Bear Tool</a>.
The tool intercepts the exec calls made by your build tool and creates a compilation database from this.
Make sure to copy this compilation database to the directory of the source code.<br>
ATTENTION: Sometimes the Bear Tool genearates multiple entries for the same file within the compilation database (compile_commands.json). If so delete those entries with non-absolute paths otherwise every Pattern is analyzed twice. Occurences, lines of codes and Fan-In Fan-Out will double.
If you don't want to install the Bear Tool, you can copy the compilation commands from the make file.
The syntax of the compilation database is covered in the <a href="https://clang.llvm.org/docs/JSONCompilationDatabase.html">JSON Compilation Database Format Specification</a>.

<b>If you want to write the compilation database from scratch by yourself</b> consider the following tutorials: <a href="https://eli.thegreenplace.net/2014/05/21/compilation-databases-for-clang-based-tools/">Compilation databases for Clang-based tools</a> and <a href="https://clang.llvm.org/docs/JSONCompilationDatabase.html">JSON Compilation Database Format Specification</a> also make shure to use absolute paths.

<h3>3.3 Running the tool</h3>
You can call the tool from its build directory like this: <code>./HPC-pattern-tool /path/to/compile_commands/file/</code>.
In our example now the compile_commands.json file is in the folder "file". Now every file specified in the compile_commands.json is analysed.
Arbitrary additional arguments can be passed after <code>--extra-arg=</code>.<br><br>
You'll have to tell PInT where the instrumentation header files are located.
You can copy the instrumentation header files to the source directory.
Alternatively, you can add an include flag using <code>--extra-arg=</code>, e.g. <code>./HPC-pattern-tool /path/to/compile_commands/file/ --extra-arg=-I/path/to/headers</code> to your tool call.
<b>If you use cmake</b>, you can instead add the flag to the list of include directories with <code>include_directories(/path/to/headers)</code>.
Finally, you can also add the <code>-I/path/to/headers</code> flag to the compilation database for the files where the instrumentation header is used.

<h3>3.4 Tool options </h3>
<h4> -onlyPattern </h4>
If you want to see the <b>patterntree without the function calls </b> you can use the flag -onlyPattern <br>
<code>./HPC-pattern-tool /path/to/compile_commands/file/ -onlyPattern --extra-arg=-I/path/to/headers</code><br>
<h4> -noTree </h4>
If you want to see <b>no tree </b> you use the Flag -noTree <br>
<code>./HPC-pattern-tool /path/to/compile_commands/file/ -noTree --extra-arg=-I/path/to/headers</code>
<h4> -useSpecFiles </h4>
If you want to analyze specific files and not every file in your Compilation Database (in the compile_commands.json file). You can use the flag -useSpecFiles. Then you can specify
every file that you want to analyze.
<code> ./HPC-pattern-tool /path/to/compile_commands/file/file1.cpp /path/to/compile_commands/file/file2.cpp /path/to/compile_commands/file/file3.cpp -useSpecFiles --extra-arg=-I/path/to/headers</code>
Every file specified has to be in the compilation database (in the compile_commands.json file), if not the tool will crash. When using this flag there will be an additional output
"ANALYZE LIST:" which shows you which files are analyzed, a file which is not in the compilation database is not analyzed.
You should be careful using this flag. The function bodies of the functions in files which where not specified are not analyzed. When a pattern is called from one of those functions
this pattern is not displayed in the tree.
<h4>-maxTreeDisplayDepth</h4>
You can use this flag if you want to cut off the deepest parts of the tree. This is really usefull for large files, to avoid printing the tree for hours. The other statistics are working with a uncut version of the tree.
Per default this is set to 10.
For large codes you can set it to a smaller number for example 5.
<code> ./HPC-pattern-tool /path/to/compile_commands/file/ -maxTreeDisplyDepth=5 --extra-arg=-I/path/to/headers</code>
<h4>-displayCompilationsList</h4>
This flag is usefull for checking if all files you need to analyze are in the compilation database, which means considered by the tool.
<h4>-pintVersion</h4>
This flag shows you which version of the tool (PInT) you are using by displaying the commit hash.
You can use this flag with the following command.
<code>/path/to/your/build/directory/of/the/Tool/./HPC-pattern-tool /path/to/your/build/directory/of/the/Tool -pintVersion</code>
If you are alredy in the build directory of the tool you can use:
<code>./HPC-pattern-tool . -pintVersion </code>

<h3>4. Limitations</h3>
Since our tool is a static analysis tool there are some limitations.
<h4>If-else commands</h4>
It is not allowed to spread pattern parts through if-else commands.
<pre><code>
  #include "PatternInstrumentation.h"

  int main(int argc, const char** argv){
    if(true){
      PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers ifA");
    }
    else{
      PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers elseB");
    }

    if(true){
      PatternInstrumentation::Pattern_End("ifA");
    }
    else{
      PatternInstrumentation::Pattern_End("elseB");
    }
    return 0;
  }
</code></pre>
This code will give you a stack inconsistency warning. We can not deal properly with patterns parts spread over if statements or if-else statements. If you started your pattern part within an if or an else statement, you should end it in the same statement otherwise you can get wrong results (even without warning).
This code for example throws no warning.
<pre><code>
  #include "PatternInstrumentation.h"

  int main(int argc, const char** argv){
    if(true){
      PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers ifA");
    }
    else{
      PatternInstrumentation::Pattern_Begin("FindingConcurrency TypeQualifiers elseB");
    }

    if(true){
      PatternInstrumentation::Pattern_End("elseB");
    }
    else{
        PatternInstrumentation::Pattern_End("ifA");
    }
  	return 0;
  }
</code></pre>
The pattern part of TypeQualifiers, ifA will appear as a parent of elseB.
<h4>Pattern parts which are spread over different functions</h4>
It is not allowed to spread pattern parts over different functions. We consider supporting that but for now we don't. Wrong results can appear with or without warning of the tool when doing this.
<pre><code>
  #include "PatternInstrumentation.h"

  void function1(){
  	PatternInstrumentation::Pattern_Begin("SupportingStructure func1 One");
  };
  void function2(){
  	PatternInstrumentation::Pattern_End("One");
  };
  int main(int argc, const char** argv){
  	function1();
  	function2();
  	return 0;
  }

</code></pre>
This is not allowed and could throw an error or could not. By coincidence the result could be as intended, don't let that fool you.
