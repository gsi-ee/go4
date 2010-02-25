The Go4 template package

This package contains a simple running 1 step Go4 analysis.

Rebuild the package by

make clean 
make all

Description of the package

All classes separated on two files *.h - class definition and 
*.cxx - implementation

The processor class TXXXProc
The analysis code is in the event processor TXXXProc. Members are
histogram, condition, and parameter pointers used in the event method
Event. In the constructor of TXXXProc the histograms, parameters and
conditions are created. Method BuildEvent() called for every new
input event, which is is retrieved from the framework with GetInputEvent()
call. Output event in this example is dummy TGo4EventElement object.
In the first part of BuildEvent() method data from the raw MBS event 
are copied to the arrays. Two subevents (crate 1,2) are processed.
Then the histograms are filled, the 2d one with polygon conditions.

A Parameter class TXXXParam
In this class one can store parameters, and use them in all steps.
Parameters can be modified from GUI.

Autosave file mechanism
By default autosave is enabled in batch mode, disabled in GUI mode,
and all objects are saved into this ROOT file at the end. At startup
the autosave file is read and all objects are restored from that file.
When TXXXAnalysis is created, the autosave file is not yet loaded. Therefore the
objects created here are overwritten by the objects from autosave file (if any), except histograms.
From GUI, objects are loaded from autosave file when Submit button is pressed.
One can inspect the content of the auto save file with the Go4 GUI.
Note that GO4USERLIBRARY=/my/analysis/path/libGo4UserAnalysis.so
should be defined to enable the GUI to read the auto save file.

Run analysis
Analysis can be started from the Go4 GUI or by command line:
   shell> go4analysis -file /GSI/lea/gauss.lmd
   shell> go4analysis -random
   shell> go4analysis -stream r3f-12
The events can be read from standard GSI lmd files or MBS or event servers.
For each event the user event processor TXXXProc is called. 
It fills some histograms, taking data from input MBS event.
The histograms can be stored in autosave file.

Adopt the example
First of all, it is good idea to rename all classes. 
For this, go4 rename.sh script can be used:
   shell> $GO4SYS/build/rename.sh "XXX" "Myname"
Most probably you will change TXXXParam to keep useful parameters.
Definitely you will change TXXXProc to create your histograms, conditions,
pictures, and finally write your analysis function BuildEvent.
If you need to fill data in output event, please consider Go4Example1Step example.

Creating a new class
Provide the definition and implementation files (.h and .cxx)
Add class in XXXLinkDef.h
Then make all.

H.Essel@gsi.de
06159 712491
