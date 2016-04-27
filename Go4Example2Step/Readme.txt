The Go4 template package

This package contains a running 2 step Go4 analysis.
There is one name string used for all classes and files: XXX
You should replace this by your own name by

$GO4SYS/build/rename.sh "XXX" "myname"
Example:
> $GO4SYS/build/rename.sh "XXX" "Ship"


Note that "myname" will be part of all filenames! Do not use
a string which is already in the filenames.

Then rebuild the package by

make clean 
make all


Description of the package

A test file is $GO4SYS/data/test.lmd


Analysis:  TXXXAnalysis

All classes are defined and declared in two files (*.h and *.cxx)
In TXXXAnalysis the two steps are created with their factories and input and output
parameters. Here the defaults are set concerning the event IO.
Two parameter objects are created (TXXXParameter). They can be used in both steps.

Step one: Unpack

The event filled: TXXXUnpackEvent
The processor:    TXXXUnpackProc

The factory TXXXUnpackFact normally need not to be changed. The TXXXUnpackEvent
contains the data members to be filled from the input event (MBS 10,1).
Only the Clear method must be changed to clear all these members.
The unpacking code is in the event processor TXXXUnpackProc. Members are
histograms, conditions, and parameter pointers used in the event method
XXXUnpack. In the constructor of TXXXUnpackProc the histograms and
conditions are created, and the pointers to the parameter objects (created in
TXXXAnalysis) are set. XXXUnpack - called event by event - gets the output
event TXXXUnpackEvent as argument (poutevt).
The input event is retrieved from the framework. The histograms are filled,
the 2d one with a polygon condition pc1, and the output event members are set.

Step two: Analysis

The event filled: TXXXAnlEvent
The processor:    TXXXAnlProc

The step two is build in the same way as step one.
Note that the TXXXUnpackEvent is used two times: once as output of step one,
and once as input of step two.
Therefore the Fill method checks if TXXXUnpackEvent has to be filled by XXXUnpack
in step one or retrieved from input file of step two which should be an output file of step one.
Step one must be disabled in the last case.
The user method XXXEventAnalysis always gets the pointer to the correct event.

A Parameter class
 TXXXParameter
In this class one can store parameters, and use them in all steps.
Parameters can be modified from GUI.

Autosave file mechanism.
When autosave is enabled, all objects are saved into a ROOT file
after n events, and at the end. At startup the autosave file is read and all objects are restored
from that file.
When TXXXAnalysis is created, autosave file is not yet loaded. Therefore are the
objects created here overwritten by the objects from autosave file (if any), except histograms.
The histogram "EventSize" is created in PreLoop of TXXXAnalysis, because at this point
it is already rebuilt from autosave file (if any).
From GUI, objects are loaded from autosave file when Submit button is pressed.


Run analysis.
The analysis can be started from the Go4 GUI or by command line:

  shell> go4analysis -file inputfile
  
When starting from command line, user-specific arguments can be specified:

  shell> go4analysis -args customname

All arguments, placed after "-args" string will be delivered to TXXXAnalysis
constructor and can be freely interpreted by user.
   
The MBS events are read from one of standard event sources (lmd files,
or MBS servers, or random generator). Then the first user event processor is
called (unpack). This user event processor fills some histograms
and the first user event (unpacked event) from MBS input event.
Then the second user event processor is called (analysis).
This user event processor fills some other histograms and the second
user event from the first event. The events from
the first and second step can optionally be stored in root files (from GUI).
When a root file with unpacked events exists, the first step can be disabled,
and this file can be selected as input for the second step (from GUI).
The main program builds the files names needed and creates the TXXXAnalysis.
Then it either starts the GUI client or the event loop.
    