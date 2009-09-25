-------------------------------------------------------------
    The GSI Online Offline Object Oriented (Go4) Project
    Experiment Data Processing at DVEE department, GSI
------------------------------------------------------------

***********************************************************
*        Go4 Example of user defined event source         *
***********************************************************
             (15-Apr-2003, J. Adamczewski)
---------------------------------------------------------------
Copyright (C) 2000- Gesellschaft f. Schwerionenforschung, GSI
                    Planckstr. 1, 64291 Darmstadt, Germany
Contact:            http://go4.gsi.de
---------------------------------------------------------------
This software can be used under the license agreements as stated
in Go4License.txt file which is part of the distribution.
---------------------------------------------------------------

This package contains a running 1 step Go4 analysis.

------------------------------------------------------------
There is one name string used for all classes and files: YYY
You should replace this by your own name by

$GO4SYS/build/rename.sh "YYY" "myname"
Example:
> $GO4SYS/build/rename.sh "YYY" "Ship"


Note that "myname" will be part of all filenames! Do not use
a string which is already in the filenames.

Then rebuild the package by

make clean 
make all
------------------------------------------------------------

Description:

The only analysis step gets the input not as usual from Mbs
(MbsEvent structure and eventsources),
but defines the input from a text file with a user format.
Therefore, the classes TYYYRawEvent, TYYYEventSource
are defined. Note that the TYYYUnpackFact implements in
addition the CreateEventSource() and CreateInputEvent() methods.
The TGo4UserSourceParameter tells the factory to initialize
the TYYYEventSource; the user source name, optional portnumber and
argument string can be set from GUI or batch analysis here.
However, it would still be possible to use standard event sources
like TGo4FileSource for the TYYYRawEvent.

This example reads a simple ascii file with columns of numbers
separated by blank spaces. These are filled in the order of
their appeareance into the fdData[] array of the TYYYRawEvent.
The array expands automatically, therefore TYYYEventSource
should be able to treat all ascii files containing values that
can be read as Double_t.
Lines preceded by "!" or "#" are seen as comments and ignored.

The TYYYUnpackProc then interprets the fdData[] according
to the meanings of the columns. These are copied to corresponding
members of the TYYYRawEvent, and some histograms are filled.
Thus TYYYUnpackEvent and TYYYUnpackProc have to be modified
if other ascii data shall be processed.
Note that the default inputfile suffix for batchmode (here: .scf)
can be defined in the MainUserAnalysis.cxx.

In this example, the input files were produced by
multiple scattering simulation SCATT, giving the resulting radius and
velocity vectors, and the number of scattering actions.
(x,y,z,vx,vy,vz,Nscatt). Units are nm and nm/ps, respectively.
This package gives two input files for 3 Mev p+ beam through
50um thick foils of Be and Ta, respectively
(befoil50.scf, tafoil50.scf), with 500 events each.
For further information on the simulation see:
(J.Adamczewski et al., Nucl.Instr.Meth.B 114(1996)p.172).


