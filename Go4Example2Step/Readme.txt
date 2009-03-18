The Go4 template package

This package contains a running 2 step Go4 analysis.
There is one name string used for all classes and files: XXX
You should replace this by your own name by

./rename.sh "XXX" "myname"
Example:
> ./rename.sh "XXX" "Ship"


Note that "myname" will be part of all filenames! Do not use
a string which is already in the filenames.

Then rebuild the package by

make clean all


Description of the package

A test file is /s/goofy/gauss.lmd

Main program and analysis:
MainUserAnalysis and TXXXAnalysis

The main program can be started from the Go4 GUI or by command line:
./MainUserAnalysis inputfile [events]
The events are read from standard GSI lmd files (in the GUI one can
switch to MBS or event servers). Then the first user event processor is
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

All classes are defined and declared in two files (*.h and *.cxx)
In TXXXAnalysis the two steps are created with their factories and input and output
parameters. Here the defaults are set concerning the event IO.
Two parameter objects are created (TXXXParameter). They can be used in both steps.

Step one: Unpack

The factory:      TXXXUnpackFact
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

The factory:      TXXXAnlFact
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
When autosave is enabled (MainUserAnalysis), all objects are saved into a ROOT file
after n events, and at the end. At startup the autosave file is read and all objects are restored
from that file.
When TXXXAnalysis is created, autosave file is not yet loaded. Therefore are the
objects created here overwritten by the objects from autosave file (if any), except histograms.
The histogram "EventSize" is created in PreLoop of TXXXAnalysis, because at this point
it is already rebuilt from autosave file (if any).
From GUI, objects are loaded from autosave file when Submit button is pressed.

----------------------------------------------
Parameter containing fitters for calibration

 Class TXXXCalibPar is another parameter class with the purpose of
 performing calibration. This parameter is used in the TXXXAnlProc.cxx
 processor of the second step. Example reads names of the calibration lines,
 the energies and initial channel numbers from file calilines.txt
 on initialization. These are present in arrays inside the parameter.
 First fitter "linesfinder" is used to find out true channel numbers
 for corresponding lines in calibration spectrum. This fit should
 be done interactively on the gui side:
 - Get parameter from analysis
 - Get calibration spectrum ("Cr1Ch01" here, note that this does not
 match the physics of the x-ray lines example, but it is sufficient to show the principle)
 - Double click on the fxLinesFinder fitter in the parameter editor.
 Fitpanel will open showing the current settings of the fitter. Use "Copy to pad"
 of the fitpanel to assign this fitter to the viewpanel containing the calibration
 spectrum.
 - In the models field of the fitpanel, enable or disable the lines
 that are present in the calibration spectrum. Only enabled lines are
 fitted.
 - Do the fit of the linesfinder to the calibration spectrum.
 - If the positions of the lines are fitted correctly,
    copy the fitter back to the calibration parameter:
    right mouse button click on fxLinesfinder, select "Refresh fitter from fitgui
    workspace"
 - Set boolean field "fbRecalibrate" in parameter to 1 (true).
 - Now press "Apply". This will perform fit of the calibration curve
   (linear by default, but any polynom is possible) in the
   UpdateFrom() method on the analysis side.
 - Pressing "Refresh" will get the results of the calibration, present
   in the polynomial coefficients fdA[0]...fdA[3] and in the fxCalibrator
   fitter.
 - The corresponding TGraph can be viewed under "UserObjects/Calibration".
   Get this from analysis, double click on the fxCalibrator fitter in
   the parameter editor and move the fitter from the fitpanel to the
   viewpanel which shows the TGraph. Pressing "Draw" on the fitpanel
   will draw the calibration polynomial over the points which indicate
   the energy/channel of the calibration lines.
 - Once the calibration is correct, fbRecalibrate can be set to false
   to prevent recalibration every time the parameter is updated
   (i.e. reloading it from autosave file or sending it to analysis)
 - The calibration is used in the method float TXXXCalibPar::Energy(int)
   which calculates the energy from a given channel number.
   Histogram fCaliSum in TXXXAnlProc is filled using this method.
 - Set boolean field "fbReadDatabase" in parameter to 1 (true) if
   the calibration lines shall be reread from file. This will overwrite
   any settings and fits performed with the linesfinder fitter on
   the gui before! Note that channel numbers of lines are not found
   automatically in UpdateFrom, this is left to the interactive gui.
 - Note that calibration coefficients fdA[] in parameter may be entered
   also manually, i.e. with fbRecalibrate set to false, the user has
   full control of the calibration function

-------------------------------------------
Example root macros in the package:

"browse.C": To be executed in ROOTCINT session.
    Will load all required Go4 libraries and
    opens the TBrowser. So access to all Go4 objects
    (fitters, conditions, dynamic list entries, markers)
    and user defined objects (parameters, event structures)
    is possible from a plain rootcint session. This is
    meant as a template for own macros.

"plothistos.C": To be executed in ROOTCINT session.
    Example how to access histograms by name from a Go4
    autosave file and how to plot them in a user defined
    way. Usage: ".L plothistos.C" to load the function. Then
    subsequent calls "plothistos("myfile",500,1000,1);" can be
    done, with     "myfile.root" the histogram file;
                   500,1000 the lower and upper x-axis range;
                   1(or 0) a switch for lin/log scale
    The macro contains examples for lots of other root
    plotting options (some commented out). Please see
    macro source for details.

"convertfile.C": To be executed in ROOTCINT session.
    Converts all histograms and graphs of a file recursively
    into ascii format. Usage: ".L convertfile.C" to load
    function. Call "convertfile("myfile");" to do conversion of
    complete myfile.root.
    Subdirectories or subfolders of the rootfile are converted
    into subdirectories of the current file system working directory.
    Each converted object will be a single ascii file with
    suffixes .hdat or .gdat, for histograms, or graphs, respectively.
    Note that similar conversions can be done using the go4
    export manager class TGo4ExportManager, which in addition
    has filters for root, radware, and ascii types. However,
    this macro offers possibility to adjust output format
    explicitly. Please see macro source for details.

"findobjects.C": To be excuted in the Go4 analysis process
    via the command line of the Go4 GUI analysis window.
    (".x findobjects.C").
    Searches for all objects in the analysis objects manager
    which names match a certain regular expression and
    send the requested objects to the GUI. Found objects
    will appear in the memory tab of the GUI browser.
    Example of methods "NextMatchingObject" and
    "SendObjectToGUI". Please edit macro to change
    search pattern.

    