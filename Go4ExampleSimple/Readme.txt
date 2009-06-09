The Go4 template package

This package contains a simple running 1 step Go4 analysis.

Rebuild the package by

make clean all

Description of the package

A test file is /GSI/lea/gauss.lmd

Main program:
MainUserAnalysis

The main program can be started from the Go4 GUI or by command line:

./MainUserAnalysis -file|-trans|-stream|-evserv|-revserv|-random input [-num events] [-asf output]
./MainUserAnalysis -fi /GSI/lea/gauss.lmd
./MainUserAnalysis -ev r3f-12
./MainUserAnalysis -re lxi007 6009

The events can be read from standard GSI lmd files or MBS or event servers.
For each event the user event processor TXXXProc is called.
This user event processor fills some histograms from the input MBS event.

All classes are defined and declared in two files (*.h and *.cxx)
In MainUserAnalysis the analysis step is created with a standard step factory.

The processor:    TXXXProc

The standard factory created in MainUserAnalysis keeps all
information about the step.
The analysis code is in the event processor TXXXProc. Members are
histogram, condition, and parameter pointers used in the event method
Event. In the constructor of TXXXProc the histograms, parameters and
conditions are created. Function Event - called event by event - gets
the output event pointer as argument (XXXEvent).
The input event pointer is retrieved from the framework.
In the first part, data from the raw input MBS event are copied to
arrays of TXXXProc. Two subevents (crate 1,2) are processed.
Then the histograms are filled, the 2d one with polygon conditions.

A Parameter class TXXXParam
In this class one can store parameters, and use them in all steps.
Parameters can be modified from GUI.

Autosave file mechanism.
By default autosave is enabled in batch mode, disabled in GUI mode,
and all objects are saved into this ROOT file at the end. At startup
the autosave file is read and all objects are restored from that file.
When TXXXAnalysis is created, the autosave file is not yet loaded. Therefore the
objects created here are overwritten by the objects from autosave file (if any), except histograms.
From GUI, objects are loaded from autosave file when Submit button is pressed.
One can inspect the content of the auto save file with the Go4 GUI.
Note that GO4USERLIBRARY=/my/analysis/path/libGo4UserAnalysis.so
should be defined to enable the GUI to read the auto save file.

Creating a new class
Provide the definition and implementation files (.h and .cxx)
Add class in Go4UserAnalysisLinkDef.h
Then make all.

Adapt the example
Most probably you will change TXXXParam to keep useful parameters.
Definitely you will change TXXXProc to create your histograms, conditions,
pictures, and finally write your analysis function BuildEvent.
If you need to fill data in output event, please consider Go4Example1Step example.

H.Essel@gsi.de
06159 712491
