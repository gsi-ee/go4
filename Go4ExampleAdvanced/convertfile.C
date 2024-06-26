TString CurrentDir;
TString StartDir;

void convertfile(const char *file)
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


//TString filename="test_ASF.root";
TString filename = file;
TString com1 = "mkdir "+filename;

StartDir=gSystem->WorkingDirectory();
filenameroot = filename+".root";
TFile myfile(filenameroot.Data(),"READ");
std::cout <<"getting objects from file  "<<filenameroot.Data() << std::endl;
gSystem->Exec(com1);
gSystem->cd(filename.Data());
CurrentDir=gSystem->WorkingDirectory();
// gSystem->Exec("pwd");
gSystem->cd(StartDir.Data());
convertdir(&myfile);
}


void converthisto(TH1 *histo)
{
  TString objectname = histo->GetName();
  TString outname = objectname+".hdat";
  gSystem->cd(CurrentDir.Data());
  std::ofstream outfile(outname.Data());
  if(!outfile) {
      std::cout <<"Error opening outputfile "<< outname << std::endl;
      return;
  }

  std::cout <<"Converting histogram "<< histo->GetName() << std::endl;
  Int_t maxbinX=histo->GetNbinsX();
  Int_t maxbinY=histo->GetNbinsY();
  Int_t maxbinZ=histo->GetNbinsZ();
  Int_t globalbin = 0;
  Stat_t  cont = 0;
  outfile <<"# Histogram "<<histo->ClassName() <<": "<<histo->GetName()<< std::endl;
  outfile <<"# Xbin \tYbin \tZbin \tContent"<< std::endl;
  //outfile <<"# Xbin \tContent"<< std::endl;
  for(Int_t x = 0; x < maxbinX; ++x)
    {
      for(Int_t y = 0; y < maxbinY; ++y)
        {
          for(Int_t z = 0; z < maxbinZ; ++z)
            {
              globalbin = histo->GetBin(x,y,z);
              cont = histo->GetBinContent(globalbin);
              outfile <<x<<" \t"<<y<<" \t"<<z<<" \t"<<cont<< std::endl;
              //outfile <<x<<" \t"<<cont<< std::endl;
            }
        }

    }
  outfile.close();
  gSystem->cd(StartDir.Data());
}

void convertgraph(TGraph *graph)
{
  TString objectname = graph->GetName();
  TString outname = objectname+".gdat";
  gSystem->cd(CurrentDir.Data());
  std::ofstream outfile(outname.Data());
  if(!outfile) {
      std::cout <<"Error opening outputfile " << outname << std::endl;
      return;
  }
  std::cout <<"Converting graph "<< graph->GetName() << std::endl;
  Int_t maxpoints=graph->GetN();
  outfile <<"# Graph "<<graph->ClassName() <<": "<<graph->GetName()<< std::endl;
  outfile <<"# Point \tX \tY"<< std::endl;
  for(Int_t point = 0; point < maxpoints; ++point) {
      Double_t xg = 0;
      Double_t yg = 0;
      graph->GetPoint(point,xg,yg);
      outfile <<point<<" \t\t"<<xg<<" \t"<<yg<< std::endl;
  }
  outfile.close();
  gSystem->cd(StartDir.Data());
}

void convertobject(TObject *myobject)
{
  TString objectname = myobject->GetName();
  if(myobject->InheritsFrom("TDirectory")) {
     convertdir((TDirectory *) myobject);
  } else if (myobject->InheritsFrom("TFolder")) {
     convertfolder((TFolder *) myobject);
  } else if(myobject->InheritsFrom("TH1")) {
     converthisto((TH1 *) myobject);
  } else if (myobject->InheritsFrom("TGraph")) {
     convertgraph((TGraph *) myobject);
  } else {
      std::cout <<"NOT converting object"<< myobject->GetName();
      std::cout <<" of class "<<myobject->ClassName() <<" to ASCII."<< std::endl;
    }
}


void convertfolder(TFolder *fold)
{
  std::cout <<"Converting contents of folder "<<fold->GetName())<<"..."  << std::endl;
  TString dirname=fold->GetName();
  gSystem->cd(CurrentDir.Data()); // create subdirectory in file system
  TString com="mkdir "+dirname;
  gSystem->Exec(com);
  gSystem->cd(dirname.Data());
  CurrentDir=gSystem->WorkingDirectory();
  TObject *myobject = nullptr;
  TObject *ob = nullptr;
  TIter iter(fold->GetListOfFolders());
  while((myobject = iter()) != nullptr) {
      convertobject(myobject);
  }
  gSystem->cd(CurrentDir.Data());
  gSystem->cd("..");
  CurrentDir=gSystem->WorkingDirectory(); // go up one directory level again
  gSystem->cd(StartDir.Data());
}


void convertdir(TDirectory *source)
{
  std::cout <<"Converting contents of directory "<<source->GetName()<<"..." << std::endl;
  TString dirname=source->GetName();
  if(!dirname.Contains(".root")) {
     gSystem->cd(CurrentDir.Data()); // create subdirectory in file system
     TString com="mkdir "+dirname;
     gSystem->Exec(com);
     gSystem->cd(dirname.Data());
     CurrentDir=gSystem->WorkingDirectory();
  }
  TObject *myobject = nullptr;
  source->cd();
  gSystem->cd(StartDir.Data());
  TIter iter(source->GetListOfKeys());
  TKey *mykey = nullptr;
  while((mykey = (TKey *) iter()) != nullptr) {
    myobject= mykey->ReadObj();
    if(myobject)
      {
        convertobject(myobject);
      }
    else
      {
        std::cout <<"Could not read object "<<mykey->GetName() << std::endl;
      }
  } // while
  if(!dirname.Contains(".root")) {
    gSystem->cd(CurrentDir.Data());
    gSystem->cd("..");
    CurrentDir=gSystem->WorkingDirectory(); // go up one directory level again
    gSystem->cd(StartDir.Data());
  }
}

