--------------------------------------------------------------
    Go4 Analysis Mesh example
-------------------------------------------------------------
    The GSI Online Offline Object Oriented (Go4) Project
    Experiment Data Processing at DVEE department, GSI
------------------------------------------------------------
Copyright (C) 2000- Gesellschaft f. Schwerionenforschung, GSI
                    Planckstr. 1, 64291 Darmstadt, Germany
Contact:            http://go4.gsi.de
---------------------------------------------------------------
This software can be used under the license agreements as stated
in Go4License.txt file which is part of the distribution.
---------------------------------------------------------------

This example shows how to set up a go4 analysis of several steps
that build a mesh of parallel analysis branches with different
result generations. Additionally, one can see how the
improved TGo4FileSource class supports partial input of a
ROOT tree.

A) Structure:
The setup of the mesh analysis is done in the constructor
of the TMeshAnalysis class. As in the Go4ExampleSimple,
the general TGo4StepFactory is used to specify the event
objects by name and classname. An overall of 13 analysis
steps is defined for this example. Generally, the
analysis mesh consists in two different kinds of steps,
the execution steps and the provider steps. The unpack
step, however, is as in the other examples just delivering
sample date from a TGo4MbsSource (standard Go4 gauss
example)

The step structure of the example mesh is as sketched in this figure
(top to down dataflow):

________________________________________________________
                     Unpack
                       |
Input1Provider  Input2Provider  Input3Provider
      |              |               |
     Exec1          Exec2          Exec3
      |               |               |
Output1Provider     Output2Provider   Output3Provider
      \               /
       \             /
            Exec12
              |
         Output12Provider
              |

              Final (combines all output provider events)
_________________________________________________________


Execution steps:

These analysis steps do the actual analysis work, i.e. they
convert some input event into the output event. This is the
same as in the more simple examples (2-Step). However, to
realize a mesh structure, the execution steps do not work directly
on their own input event as assigned from the Go4 framework, but use
the input event of one or more provider steps. The execution
steps can access the input event pointers of any provider step
by the provider step name, using the GetInputEvent("stepname")
method. Note that the native input event of the execution
steps is never used here (except for the very first "Unpack" step
that processes the initial mbs event directly, without a provider
step).
There is no histogramming actions in the execution steps;
to view the result data one has to use a dynamic list histogram or
perform a TTree::Draw on the output event's tree, if existing.

Provider steps:

These analysis steps do not perform any analysis work at all,
but only make sure that their own input event is always
set correctly for the following execution steps, depending on
the data flow situation. Generally, there are two cases:
- the provider step reads the input event directly from a
branch of a root tree (TGo4FileSource). In this case, the
input event remains the native input event of this step as
created in the step factory.
- the provider step refers to the result event of a previous
execution step. In this case, the provider processor itself
has to find the correct event pointer by name from the Go4
object management. The default Go4 framework mechanism to handle
these two cases will not suffice here, since it was designed for
a subsequent order of steps and not for a mesh with parallel
execution branches.

To do this job, all provider steps use the
TMeshProviderProc class as general event processor, and
the TMeshDummyEvent class as pseudo output event.
The TMeshDummyEvent is necessary, because the Go4 framework
will always call the Fill() method of the step's output
event to execute any action of the step. So
TMeshDummyEvent::Fill() calls method
TGo4ProviderProc::SetRealInput() to set the pointer to the
desired input event correctly.

If the input event is not read from file (native input event of this step),
the provider processor has to search for it by name using the method
TGo4Analysis::GetEventStructure("name"). However, the Go4 framework so far
does not offer any additonal parameter to specify the
name of the appropriate input for a provider step. Therefore, this
example uses the trick to derive the eventname search string from
the name of the provider processor itself:
The name of this processor (up to the "_") is the name of required event.
Note that TGo4StepFactory forbids to use same name for different objects,
since object name is used as pointer name in the processline call;
therefore the processor name can not be identical with the
input event name, but must differ by the "_" extension.

Additionally, the provider steps use the new partial input
feature of the TGo4FileSource class (since Go4v2.9).
The name of the event structure defines the name of the TTree branch
that should be read from the input file. The first three
provider steps use different parts of the TMeshRawEvent each.
If the input event name is set to the name of the
corresponding tree branch (e.g. "RawEvent.fxSub1"), the file
source will only read this branch from the tree. If the input
event name is set to the full name of the raw event ("RawEvent",
commented out in this example), the complete event is streamed,
including the not used parts. Note that in both cases the
event object must consist in the full TMeshRawEvent, although
in the partial input case only one subevent is filled. This
is required for a proper event reconstruction due to the
root TTree mechanism.
In this example, the partial event input might increase the
process speed by a factor of 2 compared to the full event
input.

B) Configuration:
Although the step configuration can be defined as usual from
the analysis configuration gui, not all combinations of enabled
and disabled steps make sense to process a subpart of the
complete analysis mesh. For example, if execution step2 shall
be processed, the corresponding provider step for its input
event has to be enabled, too. Note that the standard
step consistency check of the Go4 framework is disabled here to
run such a mesh at all (SetStepChecking(kFALSE)). So it is
user responsibility to ensure that all required event objects
are available for a certain setup.

Moreover, with >13 analysis steps the
standard analysis configuration gui becomes quite inconvenient.

Therefore, the example uses a go4 parameter TMeshParameter
for the easy setup of the configuration. This parameter has
just a set of boolean flags to determine which execution
step shall be enabled. Depending on this setup, the
UpdateFrom method of the parameter also enables or disables
the required provider steps.
However, the parameter does not contain the full
information of the input file names for the providers yet
(In a "real" application, this could be implemented in a similar
way though).

Thus the configuration procedure looks like this:
The TMeshParameter is edited on the gui to enable the desired
execution steps. The parameter is send to analysis and switches
the steps on and off. Then the analysis configuration gui has to be
refreshed from the user ("arrow right") to view the new setup.
Here the user may change the names of the event sources for the
provider steps, if necessary. After submitting these settings
again from the configuration gui,
the mesh setup is ready.
Note that once the mesh is configured in this way, the configuration
can be stored completely in the analysis preferences and
restored on the next startup.

One could also think of a user defined gui that handles both
the setup of the TMeshParameter, and the rest of the
analysis configuration in one window. This would offer the
additional advantage that it could show the structure of the
analysis mesh in a graphical way. However, such a user gui
is not delivered here, but can be created according to the
hints given in package Go4UserGUI.

C) Usage of the example:

One way to test the example could look like this:
1) Enable the first unpack step, disable the rest of the mesh.
Use TGo4MbsRandom as event source for the Unpack and fill
the output event TMeshRawEvent into a root tree (switch on
TGo4FileStore of unpack step). Do this until a reasonable
number of events is processed.

2) Disable the unpack step, enable one or more of the subsequent
execution steps. The input for the first 3 provider steps should
be the root file that was produced in 1). Note that the first
providers could also read their subevents from different files.
Eventually, produce further output trees from the execution steps.

3) Change the setup in a way that only one branch of the mesh
is processed, e.g. only Exec3 and Final.

4) Change the setup in a way that only a certain generation of
events is processed, e.g. only Exec1, Exec2, and Exec3, writing
output files of their results. Alternatively, let only Exec12
and Final work, reading their provider inputs from these
output files.

5) Change the example code and recompile
to add another execution branch,
e.g. with new steps for InputProvider4, Exec4, OutputProvider4,
and collect the results in the existing final step. New
classes TMeshB4InputEvent, TMeshB4AnlProc, and TMeshB4OutputEvent
should be defined for this (these can be derived from the
corresponding classes as existing for the Exec3 branch)


6) Create a new mesh analysis from this template
that matches your analysis structure...


__________________________
J. Adamczewski 12-Jan-2005