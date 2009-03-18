{
// this macro loads all required libraries for Go4
// and starts the root browser.
// NOTE: for Go4 Version > 2.8 and ROOT >= 4.00/06,
// Go4 uses the root library mapping, so it is not
// necessary to use gSystem->Load of required libraries in any
// macro
gSystem->Load("libThread.so");
gSystem->Load("libMinuit.so");
gSystem->Load("$GO4SYS/lib/libGo4Base.so");
gSystem->Load("$GO4SYS/lib/libGo4Fit.so");
gSystem->Load("$GO4SYS/lib/libGo4ThreadManager.so");
gSystem->Load("$GO4SYS/lib/libGo4TaskHandler.so");
gSystem->Load("$GO4SYS/lib/libGo4Version.so");
gSystem->Load("$GO4SYS/lib/libGo4AnalBase.so");
gSystem->Load("$GO4SYS/lib/libGo4Analysis.so");
gSystem->Load("libGo4UserAnalysis.so");

TBrowser b;
}
