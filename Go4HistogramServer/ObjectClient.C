{
gSystem->SetIncludePath(" -I$GO4SYS ")
# include "TGo4ObjClient.h"
# include "Go4StatusAnalysis/TGo4AnalysisObjectNames.h"

gSystem->Load("libThread.so");
gSystem->Load("libMinuit.so");
gSystem->Load("$GO4SYS/lib/libGo4Base.so");
gSystem->Load("$GO4SYS/lib/libGo4Fit.so");
gSystem->Load("$GO4SYS/lib/libGo4ThreadManager.so");
gSystem->Load("$GO4SYS/lib/libGo4TaskHandler.so");
gSystem->Load("$GO4SYS/lib/libGo4Version.so");
gSystem->Load("$GO4SYS/lib/libGo4AnalBase.so");
gSystem->Load("$GO4SYS/lib/libGo4Analysis.so");

TGo4Log::Instance(); // init logger object
TGo4Log::SetIgnoreLevel(0); // set this to 1 to suppress detailed debug output
                            // set this to 2 to get warnings and errors only
                            // set this to 3 to get errors only

TGo4Log::LogfileEnable(kTRUE); // will enable or disable logging all messages

TCanvas mycanvas("Object client", "Go4 object client test");
mycanvas.cd();
TGo4ObjClient myclient("TestClient", "XXX", "demo", "localhost",5001);
cout <<"getting nameslist" << std::endl;
TGo4AnalysisObjectNames* list=myclient.RequestNamesList();

if(list)
 list->PrintStatus();
else
 cout <<"got zero list!!!" << std::endl;

cout <<"myclient.SetHost('name') sets hostname" << std::endl;
cout <<"myclient.SetPort(int) sets port" << std::endl;
cout <<"myclient.SetBase('name') sets basename" << std::endl;
cout <<"myclient.SetPasswd('name') sets password" << std::endl;
cout <<"Use TGo4AnalysisObjectNames* dir=myclient.RequestNamesList() to receive dir" << std::endl;
cout <<"Use TObject* ob=myclient.RequestObject('name') to receive object" << std::endl;
// now testing repeated access in short time:
TObject* arr[20];
mycanvas.Divide(4,5);
for(Int_t t=0; t<20;++t)
{
    //cout <<"requesting "<<t << std::endl;
    arr[t]=myclient.RequestObject("Cr1Ch02");

}

for(Int_t t=0; t<20;++t)
{
    mycanvas.cd(t+1);
    if(arr[t]) arr[t]->Draw();
    mycanvas.Update();
}


}

