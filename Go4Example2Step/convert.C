{
//////////////////////////////////////////////////
// NOTE: for Go4 Version > 2.8 and ROOT >= 4.00/06,
// Go4 uses the root library mapping, so it is not
// necessary to use gSystem->Load of required libraries in any
// macro. If you work with older versions of root,
// please uncomment the following lines:
//gSystem->Load("libThread.so");
//gSystem->Load("libMinuit.so");
//gSystem->Load("$GO4SYS/lib/libGo4Base.so");
//gSystem->Load("$GO4SYS/lib/libGo4Fit.so");
//gSystem->Load("$GO4SYS/lib/libGo4ThreadManager.so");
//gSystem->Load("$GO4SYS/lib/libGo4TaskHandler.so");
//gSystem->Load("$GO4SYS/lib/libGo4Version.so");
//gSystem->Load("$GO4SYS/lib/libGo4AnalBase.so");
//gSystem->Load("$GO4SYS/lib/libGo4Analysis.so");
//gSystem->Load("libGo4UserAnalysis.so");


#include <fstream.h>
#include <stdio.h>
#include <stdarg.h>


TString filename="MyAnalysis_ASF.root";
TString histoname="Cr1Ch02";
TString outname=histoname+".dat";

TFile myfile(filename.Data(),"READ");
TFolder* topfold=(TFolder*) myfile.Get("Go4");
if(topfold)
{
  cout <<"Got go4 folder of file "<<filename.Data() << endl;
  TObject* myobject=topfold->FindObjectAny(histoname.Data());
  if(myobject)
    {
      std::ofstream outfile(outname.Data());
      if(!outfile)
        {
          cout <<"Error opening outputfile "<<outname.Data() << endl;
          return;
        }
      if(myobject->InheritsFrom("TH1"))
        {
          TH1* histo= (TH1*) myobject;
          cout <<"Found histogram "<< histo->GetName() << endl;
          Int_t maxbinX=histo->GetNbinsX();
          Int_t maxbinY=histo->GetNbinsY();
          Int_t maxbinZ=histo->GetNbinsZ();
          Int_t globalbin=0;
          Stat_t  cont=0;
          outfile <<"# Histogram "<<histo->ClassName() <<": "<<histo->GetName()<<endl;
          outfile <<"# Xbin \tYbin \tZbin \tContent"<<endl;
          for(Int_t x=0; x<maxbinX; ++x)
            {
              for(Int_t y=0; y<maxbinY; ++y)
                {
                  for(Int_t z=0; z<maxbinZ; ++z)
                    {
                      globalbin=histo->GetBin(x,y,z);
                      cont=histo->GetBinContent(globalbin);
                      outfile <<x<<" \t"<<y<<" \t"<<z<<" \t"<<cont<<endl;
                    }
                }

            }
        }
      else if (myobject->InheritsFrom("TGraph"))
        {
          TGraph* graph= (TGraph*) myobject;
          cout <<"Found graph "<< graph->GetName() << endl;
          Int_t maxpoints=graph->GetN();

          outfile <<"# Graph "<<graph->ClassName() <<": "<<graph->GetName()<<endl;
          outfile <<"# Point \tX \tY"<<endl;
          for(Int_t point=0; point<maxpoints; ++point)
            {
              Double_t xg=0;
              Double_t yg=0;
              graph->GetPoint(point,xg,yg);
              outfile <<point<<" \t\t"<<xg<<" \t"<<yg<<endl;
            }
        }
      outfile.close();

    }
  else
    {
      cout <<"Did not find object "<<histoname.Data() << endl;
    }

}











}
