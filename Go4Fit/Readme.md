# Go4Fit package

This document describes how to obtain and install Go4Fit package.

Package (in tar.gz format) can be obtained from Go4 web site (http://go4.gsi.de)

Copy Go4Fit.tar.gz to destination directory and unpack it by command:

~~~
    tar -xvzf Go4Fit.tar.gz
~~~

This creates two directory Go4Fit/ and Go4FitExample/

To compile Go4Fit, first you should install ROOT and proper sets up PATH and LD_LIBRARY_PATH
enviroment variables to have access to ROOT binaries and shared libraries.

To compile Go4Fit package, you should enter your Go4Fit/ path and
execute "make" command. This compiles all sources and creates "libGo4Fit.so"
shared library. To use it in ROOT, you should specify path Go4Fit/
and Go4FitExample/ in LD_LIBRARY_PATH enviroment variable.
For that "init_script.sh" script can be used.

To compile a set of examples, enter Go4FitExample/ directory
and execute "make" command there. This creates a set of executable programs.
To run them, LD_LIBRARY_PATH should be already specified.

Example3 is only compiled, when f77 and libg2c.so installed in the system.

All examples (except 3-rd one) also can be used in CINT directly, without any modification. To use them in CINT,
just run root, load Go4Fit library:

~~~
  [root] gSystem->Load("libGo4Fit.so");
~~~

and then run any of them:

~~~
  [root] .x Example1.cxx
~~~

For the forth example appropriate library should be loaded first:

~~~
  [root] gSystem->Load("libExample4.so");
~~~

Detailed description of Go4Fit package
can be found on Go4 web site (http://go4.gsi.de)

