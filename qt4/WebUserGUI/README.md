# Example of web-based user GUI for go4

## How to setup standard example outside $GO4SYS directory?

1. Create a working directory for project (e.g. $HOME/UserGUI)

2. Copy in it content of the $(Go4SYS)/Go4UserGUI directory

3. Compile in new location by

    make build
    cmake ..
    make all

4. Set GO4USERGUI variable to location, where user gui is istalled.
For instance:

    export GO4USERGUI=$HOME/UserGUI/build

This directory should contain library with name libGo4UserGui.so.
If name of library was changed, it should be specified in
GO4USERGUI variable:

    export GO4USERGUI=$HOME/UserGUI/build/libAlternativeUserGui.so

5. If you want to use any classes from external libraries in
the user gui, you need to set the GO4USERLIBRARY variable to let
these libraries be loaded.

For example, if the user gui works with a Go4 parameter class that
is defined for a user analysis at $HOME/UserAnalysis directory,
the set up would be:

    export GO4USERLIBRARY=$GO4SYS/lib/libGo4Analysis.so:$HOME/UserAnalysis/libGo4UserAnalysis.so

NOTE that the Go4 analysis library libGo4Analysis.so needs to be specified
here as well, because it is not linked to the Go4 GUI by default.


6. Now run main go4 gui. Use "Tools/User GUI" menu item
   to activate new user gui. If settings was specified wrong,
   standard example will be used.

## How to modify standard example?

There are several limitations, which should be kept in mind:

1. User main widget should always be inherited from QGo4Widget class.
   This class provide interface to functionality of main go4 gui.

2. User gui library should always include StartUserPanel(void*)
   function, which called by main gui once when library is loaded.
   This function should create main user widget and return its pointer
   back. In provided example this function situated in the beginning
   of QUserPanel.cpp file.

22.June.2021                        S.Linev(at)gsi.de
