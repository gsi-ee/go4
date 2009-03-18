The Go4 template package

This package contains a simple running 1 step Go4 analysis.

Rebuild the package by

make clean all

Description of the package

A test file is /s/goofy/gauss.lmd

Main program and analysis:
MainUserAnalysis and TXXXAnalysis

The main program can be started from the Go4 GUI or by command line:

./MainUserAnalysis -file|-trans|-stream|-evserv|-revserv input [-output] [events]
./MainUserAnalysis -f myfile.lmd
./MainUserAnalysis -e r3f-12 1000

The events can be read from standard GSI lmd files or MBS or event servers.
For each event the user event processor TXXXProc is
called. This user event processor fills some histograms
and an output event TXXXEvent (raw event) from the input event.
The output events can optionally be stored in root files.
When a root file with raw events exists, it can be viewd by the
Go4 GUI using the tree viewer. Define a shell variable
GO4USERLIBRARY=/mypath/libGo4UserAnalysis.so
that the viewer knows TXXXEvent.

All classes are defined and declared in two files (*.h and *.cxx)
In TXXXAnalysis the analysis step is created with the step factory and input and output
parameters. Here the defaults are set concerning the event IO.
Two parameter objects are created (TXXXParam and TXXXControl).

The event filled: TXXXEvent
The processor:    TXXXProc

The factory created in TXXXAnalysis keeps all information about the step. The TXXXEvent
contains the data members to be filled in TXXXProc from the input event (MBS 10,1).
The Clear method must clear all these members (an array for each crate in the example).
The analysis code is in the event processor TXXXProc. Members are
histograms, conditions, and parameter pointers used in the event method
Event. In the constructor of TXXXProc the histograms and
conditions are created, and the pointers to the parameter objects (created in
TXXXAnalysis) are retrieved. Function BuildEvent - called event by event - gets the output
event pointer as argument (AnEvent).
The input event pointer is retrieved from the framework.
In the first part, data from the raw input MBS event are copied to the members of
output event TXXXEvent. Two subevents (crate 1,2) are processed.
Then the histograms are filled, the 2d one with polygon conditions.

A Parameter class TXXXParam
In this class one can store parameters, and use them in all steps.
Parameters can be modified from GUI.
Parameter class TXXXControl
This class has one member "fill" which is checked in TXXXProc->Event
to fill histograms or not. The macro setfill.C(n), n=0,1 can be used
in the GUI to switch the filling on or off. It creates macro histofill.C() which
is actually used to set filling on or off (in TXXXProc). You can modify histofill.C
by editor.

Autosave file mechanism.
By default autosave is enabled in batch mode, disabled in GUI mode,
and all objects are saved into this ROOT file
at the end. At startup the autosave file is read and all objects are restored
from that file.
When TXXXAnalysis is created, the autosave file is not yet loaded. Therefore the
objects created here are overwritten by the objects from autosave file (if any), except histograms.
From GUI, objects are loaded from autosave file when Submit button is pressed.
One can inspect the content of the auto save file with the Go4 GUI.
Note that
GO4USERLIBRARY=/mypath/libGo4UserAnalysis.so
should be defined to enable the GUI to read the auto save file.

Creating a new class
Provide the definition and implementation files (.h and .cxx)
Add class in Makefile and Go4UserAnalysisLinkDef.h
Then make all.

Adapt the example
Most probably you will change TXXXParam to keep useful parameters.
Then you might change TXXXEvent to represent your event data.
Keep the Clear method consistent with the data members!
Then definitely you will change TXXXProc to create your histograms, conditions,
pictures, and finally write your analysis function BuildEvent.

In TXXXAnalysis there are three more functions which eventually can be useful:
UserPreLoop (called before event loop starts),
UserEventFunc (called after TXXXProc->Event),
UserPostLoop (called after event loop stopped).

H.Essel@gsi.de
06159 712491
