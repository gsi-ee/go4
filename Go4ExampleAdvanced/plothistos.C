void plothistos(const char* file, Stat_t lo, Stat_t up, Bool_t ylog)
{
  // parameters:
  // file= filename with histograms to display (without ".root")
  // lo: x axis lower limit (in axis units); -1 to show full range
  // up: x axis upper limit (in axis units)
  // ylog: 1 to display y in logscale, 0 for linear scale
  //////////////
  // examples  of usage:
  // .L plothistos.C
  // plothistos("mar0492_ASF",500,1000,0);
  // plothistos("mar0492_ASF",-1,0,1);
  //
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

// number of histograms/pad divisions
#define NUMHIS 6

////////////// settings for pad/histogram title box
/////////////
 gStyle->SetTitleX(0.22); // top left corner (ratio of pad width 0...1)
 gStyle->SetTitleY(0.97); // top left corner (ratio of pad height 0...1)
 //gStyle->SetTitleW(0.1); // box width; if not specified, title box will match text
 //gStyle->SetTitleH(0.1); // box height; if not specified, title box will match text
 //gStyle->SetTitleFillColor(1);
 //gStyle->SetTitleTextColor(1);
 //gStyle->SetTitleStyle(1001);
 //gStyle->SetTitleFontSize(0);
 //gStyle->SetTitleBorderSize(2);


//  // settings for histogram statistics box
//  gStyle->SetStatBorderSize(2);
//  gStyle->SetStatFont(62);
//  gStyle->SetStatFontSize(0);
//  gStyle->SetStatFormat("6.4g"); // printf like format string
 gStyle->SetStatX(0.95); // top right corner (ratio of pad size 0...1)
 gStyle->SetStatY(0.95); // top right corner (ratio of pad size 0...1)
 //gStyle->SetStatW(0.19); // relative to pad size
 //gStyle->SetStatH(0.1);  // relative to pad size
 gStyle->SetOptStat("neimr"); // what is in statistic box:
  //  n :  name of histogram is printed
  //     e :  number of entries printed
  //     m :  mean value printed
  //     r :  rms printed

  //     u :  number of underflows printed
  //     o :  number of overflows printed
  //     i :  integral of bins printed


//  // x axis label
//  gStyle->SetLabelFont(62,"X");
//  gStyle->SetLabelOffset(0.005,"X");
//  gStyle->SetLabelSize(0.04,"X");

//  // y axis label
//  gStyle->SetLabelFont(62,"Y");
//  gStyle->SetLabelOffset(0.005,"Y");
//  gStyle->SetLabelSize(0.04,"Y")

//TString filename="test_ASF.root";
 TCanvas* mycan=new TCanvas("printout","myhistograms",1);
 Int_t DivX,DivY;
 DivX=(int)sqrt(NUMHIS);
 DivY=DivX;
 if(DivX*DivY<NUMHIS){
   do{
     DivX=DivX+1;
   }while(DivX*DivY<NUMHIS);
 }
 mycan->Divide(DivX,DivY);
 TString filename=file;
 TString filenameroot = filename+".root";
 TFile* myfile = TFile::Open(filenameroot.Data(),"READ");
 TH1* his[NUMHIS];
 TString hname[NUMHIS];
 for(Int_t i=0; i<NUMHIS; ++i)
   {
     //hname[i]="Ge7_0";
     hname[i]="Cr1Ch0"; // specify "name pattern" for histograms here
       hname[i]+=i+1;   // Cr1Ch01 to Cr1Ch06
       his[i]= (TH1*) myfile->FindObjectAny(hname[i].Data());
       if(his[i])
         {
           cout <<"Loaded "<<hname[i].Data() << endl;
           mycan->cd(i+1);
           // here settings for adjust histogram view:
           ////////// pad settings:
           //gPad->SetLogx(kTRUE);
           gPad->SetLogy(ylog);
           //gPad->SetGridx(kTRUE); // grid background for pad
           //gPad->SetGridy(kTRUE);
           TAxis* xax=his[i]->GetXaxis();
           if(lo>=0) xax->SetRangeUser(lo,up);
           //xax->SetNdivisions(20,kTRUE); // specify axis divisions
           //xax->SetTicks("+-"); // ticks on which side of the axis
           //xax->SetTickLength(0.02); // ratio of pad width for tick length
           ///////// settings for axis title:
           xax->SetTitle("Channels");
           xax->CenterTitle(kTRUE); // default is right just
           //xax->SetTitleOffset(1); // 1 is default distance from axis
           //xax->SetTitleSize(0.05); // ratio of pad width
           //xax->SetTitleFont(62);
           //// axis labels (numbers):
           //xax->SetLabelOffset(1); // 1 is default distance from axis
           //xax->SetLabelSize(0.05); // ratio of pad width
           //xax->SetLabelFont(62);
           TAxis* yax=his[i]->GetYaxis();
           //yax->SetRangeUser(0,1000);
           //xax->SetNdivisions(10,kTRUE); // specify axis divisions
           //yax->SetTicks("+-");// ticks on which side of the axis
           //yax->SetTickLength(0.02); // ratio of pad width for tick length
           //// settings for axis title:
           yax->SetTitle("N");
           //yax->CenterTitle(kTRUE); // default is top just
             //yax->SetTitleOffset(1);
            //yax->SetTitleOffset(1); // 1 is default distance from axis
           //yax->SetTitleSize(0.05); // ratio of pad width
           //yax->SetTitleFont(62);
           //// axis labels (numbers):
           //yax->SetLabelOffset(1); // 1 is default distance from axis
           //yax->SetLabelSize(0.05); // ratio of pad width
           //yax->SetLabelFont(62);
           //// note: settings for colors are omitted here!
           //// please see documentation of class TStyle, TAttAxis ant TAxis
           /////// end settings for histogram

           his[i]->Draw();

         }
       else
         {
           cout <<"Could not read histogram "<<hname[i].Data()<<" from file "<<filenameroot.Data()  << endl;
         }
   }

}
