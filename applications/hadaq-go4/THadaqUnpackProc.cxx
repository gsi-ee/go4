#include "THadaqUnpackProc.h"
#if __GO4BUILDVERSION__ > 40502
#include "go4iostream.h"
#else
#include "Riostream.h"
#endif
#include "TClass.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TGo4MbsEvent.h"
#include "TGo4MbsSubEvent.h"
#include "TGo4UserException.h"
#include "TGo4Log.h"

#include "THadaqUnpackEvent.h"

#include "TGo4Picture.h"

#define EPRINT( args ...) if(fPar->printEvent) printf( args )

//***********************************************************
THadaqUnpackProc::THadaqUnpackProc() :
    TGo4EventProcessor()
{
}
//***********************************************************
THadaqUnpackProc::THadaqUnpackProc(const char* name) :
    TGo4EventProcessor(name)

{
  cout << "**** THadaqUnpackProc: Create" << endl;
  TString obname;
  TString obtitle;
  TString foldername;
  TString legendx;
  TString legendy;

  //// init user analysis objects:
  fPar = (THadaqParam *) MakeParameter("HadaqPar", "THadaqParam");

  // here we optionally override parameter values with setup macro, if existing:
    TString setupmacro = "set_HadaqPar.C";
    if (!gSystem->AccessPathName(setupmacro.Data())) {
       TGo4Log::Info("Executing hadaq parameter setup script %s", setupmacro.Data());
       gROOT->ProcessLine(Form(".x %s", setupmacro.Data()));
    } else {
       TGo4Log::Info("NO Hadaq parameter setup script %s. Use previous values!", setupmacro.Data());
    }


  fEventCount = 0;
  //// histograms:

#ifdef HAD_USE_CAHIT
  fbHasCalibration = kFALSE;
  TString dirname;
  TString hname;
  TString leadingcoarseallname = "LeadingCoarseAllChans";
  TString leadingfineallname = "LeadingFineAllChans";
  TString trailingcoarseallname = "TrailingCoarseAllChans";
  TString trailingfineallname = "TrailingFineAllChans";

  Int_t tbins = 0, trange = 0;

  for (Int_t m = 0; m < HAD_TIME_NUMMCP; ++m)
      {
      obname.Form("MCP/MCP_%d",m);
       obtitle.Form("Hit Image of MCP %d",m);
       legendx.Form("Row");
       legendy.Form("Column");
       tbins = HAD_TIME_MCPBINS; // 9
       trange = HAD_TIME_MCPRANGE; // 9
       hImagingMCP[m] = MakeTH2('I', obname.Data(), obtitle.Data(), tbins, 0, trange, tbins, 0, trange, legendx.Data(), legendy.Data());
      }



  for (Int_t b = 0; b < HAD_TIME_NUMBOARDS; ++b)
    {
      if(!THadaqUnpackEvent::AssertTRB(b)) continue;
      for (Int_t t = 0; t < HAD_TIME_NUMTDC; ++t)
        {
          dirname.Form("Timetest/TRB%02d/TDC%02d", b,t);
          obname.Form("%s/ChannelCountTRB%02d_TDC%02d", dirname.Data(), b,t);
          obtitle.Form("Channel hit count TRB %02d TDC %02d", b,t);
          hChannelCount[b][t] = MakeTH1('I', obname.Data(), obtitle.Data(),
              HAD_TIME_CHANNELS, 0, HAD_TIME_CHANNELS, "channel number");

          obname.Form("%s/%s_%02d_%02d", dirname.Data(), leadingcoarseallname.Data(),
              b,t);
          obtitle.Form("Leading Coarse time  TRB %02d TDC %02d all channels", b, t);
          tbins = HAD_TIME_COARSEHISTBINS;
          trange = HAD_TIME_COARSEBINS * HAD_TIME_COARSEUNIT;// / 1000; // scale to ns
          hLeadingCoarseAll[b][t] = MakeTH1('I', obname.Data(), obtitle.Data(),
              tbins, 0, trange, "t (bin)");

          obname.Form("%s/%s_%02d_%02d", dirname.Data(), leadingfineallname.Data(),
              b,t);
          obtitle.Form("Leading Fine time TRB %02d TDC %02d all channels", b,t);
          tbins = HAD_TIME_FINEBINS;
          trange = HAD_TIME_FINEBINS; // scale to ns?
          hLeadingFineAll[b][t] = MakeTH1('I', obname.Data(), obtitle.Data(),
              tbins, 0, trange, "t (bin) ");

          obname.Form("%s/%s_%02d_%02d", dirname.Data(),
              trailingcoarseallname.Data(), b, t);
          obtitle.Form("Trailing Coarse time TRB %02d TDC %02d all channels", b,t);
          tbins = HAD_TIME_COARSEHISTBINS;
          trange = HAD_TIME_COARSEBINS * HAD_TIME_COARSEUNIT;// / 1000; // scale to ns
          hTrailingCoarseAll[b][t] = MakeTH1('I', obname.Data(), obtitle.Data(),
              tbins, 0, trange, "t (bin)");

          obname.Form("%s/%s_%02d_%02d", dirname.Data(), trailingfineallname.Data(),
              b,t);
          obtitle.Form("Trailing Fine time TRB %02d TDC %02d all channels", b,t);
          tbins = HAD_TIME_FINEBINS;
          trange = HAD_TIME_FINEBINS; // scale to ns?
          hTrailingFineAll[b][t] = MakeTH1('I', obname.Data(), obtitle.Data(),
              tbins, 0, trange, "t (bin) ");

          for (Int_t i = 0; i < HAD_TIME_CHANNELS; ++i)
            {
              dirname.Form("Timetest/TRB%02d/TDC%02d", b, t);

              ////// Leading edges: /////////////////////////////////
              obname.Form("%s/Chan%02d/LeadingCoarseTime_%02d_%02d_%02d",
                  dirname.Data(), i, b, t, i);
              obtitle.Form("Coarse time  Leading edge TRB %02d TDC %02d Channel %02d", b, t,
                  i);
              tbins = HAD_TIME_COARSEHISTBINS;
              trange = HAD_TIME_COARSEBINS * HAD_TIME_COARSEUNIT;// / 1000; // scale to ns
              hLeadingCoarse[b][t][i] = MakeTH1('I', obname.Data(), obtitle.Data(),
                  tbins, 0, trange, "t (ns)");

              obname.Form("%s/Chan%02d/LeadingFineTime_%02d_%02d_%02d",
                  dirname.Data(), i, b, t, i);
              obtitle.Form("Fine time Leading edge TRB %02d TDC %02d Channel %02d", b, t,
                  i);
              tbins = HAD_TIME_FINEBINS;
              trange = HAD_TIME_FINEBINS; // scale to ns?
              hLeadingFine[b][t][i] = MakeTH1('I', obname.Data(), obtitle.Data(),
                  tbins, 0, trange, "t (bin)");

              obname.Form("%s/Chan%02d/LeadingFineTimeBuffer_%02d_%02d_%02d",
                  dirname.Data(), i, b, t, i);
              obtitle.Form(
                  "Fine time Leading edge TRB %02d TDC %02d Channel %02d (calibration buffer)",
                  b, t, i);
              tbins = HAD_TIME_FINEBINS;
              trange = HAD_TIME_FINEBINS; // scale to ns?
              hLeadingFineBuffer[b][t][i] = MakeTH1('I', obname.Data(),
                  obtitle.Data(), tbins, 0, trange, "t (bin)");

              obname.Form("%s/Chan%02d/LeadingDeltaTimeRaw_%02d_%02d_%02d",
                  dirname.Data(), i, b, t, i);
              obtitle.Form(
                  "Delta time Leading edge not calibrated TRB %02d TDC %02d Channel %02d",
                  b,t, i);
              tbins = HAD_TIME_DELTAHISTBINS;
              //tbins=8 * HAD_TIME_FINEBINS;
              trange = 500 * HAD_TIME_COARSEUNIT; // scale to ns
              //trange=4 * HAD_TIME_COARSEUNIT; // scale to ns
              hLeadingDeltaRaw[b][t][i] = MakeTH1('I', obname.Data(),
                  obtitle.Data(), tbins, -trange / 2, trange / 2, "t (ps)");

              obname.Form("%s/Chan%02d/LeadingDeltaTimeCal_%02d_%02d_%02d",
                  dirname.Data(), i, b, t, i);
              obtitle.Form(
                  "Delta time Leading edge calibrated TRB %02d TDC %02d Channel %02d", b, t,
                  i);
              tbins = HAD_TIME_DELTAHISTBINS;
              //tbins=8 * HAD_TIME_FINEBINS;
              trange = 500 * HAD_TIME_COARSEUNIT; // scale to ns?
              //trange=4 * HAD_TIME_COARSEUNIT; // scale to ns?
              hLeadingDeltaCal[b][t][i] = MakeTH1('I', obname.Data(),
                  obtitle.Data(), tbins, -trange / 2, trange / 2, "t (ps)");

              obname.Form("%s/Chan%02d/LeadingCorrelationRaw_%02d_%02d_%02d",
                  dirname.Data(), i, b, t, i);
              obtitle.Form(
                  "Raw Correlation Leading edge TRB %02d TDC %02d to reference of channel %02d",
                  b, t, i);
              legendx.Form("Channel %d", i);
              legendy.Form("TRB %d TDC %d Channel %d",
                    fPar->deltaTRB[b][t][i], fPar->deltaTDC[b][t][i], fPar->deltaChannels[b][t][i]);
              tbins = HAD_TIME_FINEBINS;
              trange = HAD_TIME_FINEBINS;
              hLeadingCorrelFine[b][t][i] = MakeTH2('I', obname.Data(),
                  obtitle.Data(), tbins, 0, trange, tbins, 0, trange,
                  legendx.Data(), legendy.Data());
              hLeadingCorrelFine[b][t][i]->SetYTitle(legendy.Data());

              ///////////////////////// trailing edges:
              obname.Form("%s/Chan%02d/TrailingCoarseTime_%02d_%02d_%02d",
                  dirname.Data(), i, b, t, i);
              obtitle.Form("Coarse time Trailing edge TRB %02d TDC %02d Channel %02d", b, t,
                  i);
              tbins = HAD_TIME_COARSEHISTBINS;
              trange = HAD_TIME_COARSEBINS * HAD_TIME_COARSEUNIT;// / 1000; // scale to ns
              hTrailingCoarse[b][t][i] = MakeTH1('I', obname.Data(),
                  obtitle.Data(), tbins, 0, trange, "t (ns)");

              obname.Form("%s/Chan%02d/TrailingFineTime_%02d_%02d_%02d",
                  dirname.Data(), i, b, t, i);
              obtitle.Form("Fine time Trailing edge TRB %02d TDC %02d Channel %02d", b, t,
                  i);
              tbins = HAD_TIME_FINEBINS;
              trange = HAD_TIME_FINEBINS; // scale to ns?
              hTrailingFine[b][t][i] = MakeTH1('I', obname.Data(), obtitle.Data(),
                  tbins, 0, trange, "t (bin)");

              obname.Form("%s/Chan%02d/TrailingFineTimeBuffer_%02d_%02d_%02d",
                  dirname.Data(), i, b, t, i);
              obtitle.Form(
                  "Fine time Trailing edge TRB %02d TDC %02d Channel %02d (calibration buffer)",
                  b, t, i);
              tbins = HAD_TIME_FINEBINS;
              trange = HAD_TIME_FINEBINS; // scale to ns?
              hTrailingFineBuffer[b][t][i] = MakeTH1('I', obname.Data(),
                  obtitle.Data(), tbins, 0, trange, "t (bin)");

              obname.Form("%s/Chan%02d/TrailingDeltaTimeRaw_%02d_%02d_%02d",
                  dirname.Data(), i, b, t, i);
              obtitle.Form(
                  "Delta time Trailing edge not calibrated TRB %02d TDC %02d Channel %02d",
                  b, t, i);
              tbins = HAD_TIME_DELTAHISTBINS;
              //tbins=8 * HAD_TIME_FINEBINS;
              trange = 20 * HAD_TIME_COARSEUNIT; // scale to ns
              //trange=4 * HAD_TIME_COARSEUNIT; // scale to ns
              hTrailingDeltaRaw[b][t][i] = MakeTH1('I', obname.Data(),
                  obtitle.Data(), tbins, -trange / 2, trange / 2, "t (ps)");

              obname.Form("%s/Chan%02d/TrailingDeltaTimeCal_%02d_%02d_%02d",
                  dirname.Data(), i, b, t, i);
              obtitle.Form(
                  "Delta time Trailing edge calibrated  TRB %02d TDC %02d Channel %02d",
                  b, t, i);
              tbins = HAD_TIME_DELTAHISTBINS;
              //tbins=8 * HAD_TIME_FINEBINS;
              trange = 20 * HAD_TIME_COARSEUNIT; // scale to ns?
              //trange=4 * HAD_TIME_COARSEUNIT; // scale to ns?
              hTrailingDeltaCal[b][t][i] = MakeTH1('I', obname.Data(),
                  obtitle.Data(), tbins, -trange / 2, trange / 2, "t (ps)");

              obname.Form("%s/Chan%02d/TrailingCorrelationRaw_%02d_%02d_%02d",
                  dirname.Data(), i, b, t, i);
              obtitle.Form(
                  "Raw Correlation Trailing edge TRB %02d TDC %02d to reference of channel %02d",
                  b, t, i);
              legendx.Form("Channel %d", i);
              legendy.Form("TRB %d TDC %d Channel %d",
                                  fPar->deltaTRB[b][t][i], fPar->deltaTDC[b][t][i], fPar->deltaChannels[b][t][i]);

              tbins = HAD_TIME_FINEBINS;
              trange = HAD_TIME_FINEBINS;
              hTrailingCorrelFine[b][t][i] = MakeTH2('I', obname.Data(),
                  obtitle.Data(), tbins, 0, trange, tbins, 0, trange,
                  legendx.Data(), legendy.Data());
              hTrailingCorrelFine[b][t][i]->SetYTitle(legendy.Data());

              // calibration histograms: ///////////////////////

              dirname.Form("TimeCalibration/TRB%02d/TDC%02d", b, t);

              obname.Form("%s/Chan%02d/BinWidth_%02d_%02d_%02d", dirname.Data(), i,
                  b, t, i);
              obtitle.Form("Binwidth Histogram of TRB %02d TDC %02d Channel %02d", b, t, i);
              hCalcBinWidth[b][t][i] = MakeTH1('D', obname.Data(), obtitle.Data(),
                  HAD_TIME_FINEBINS, 0, HAD_TIME_FINEBINS, "bin", "ps");
              if (IsObjMade())
                {
                  // reset time bin correction arrays:
                  for (int fb = 0; fb < HAD_TIME_FINEBINS; ++fb)
                    {
                      hCalcBinWidth[b][t][i]->SetBinContent(fb + 1, 1.0);
                    }
                }

              obname.Form("%s/Chan%02d/CalTableBinTime_%02d_%02d_%02d",
                  dirname.Data(), i, b, t, i);
              obtitle.Form("Calibration Table of TRB %02d TDC %02d Channel %02d", b, t, i);
              hCalBinTime[b][t][i] = MakeTH1('D', obname.Data(), obtitle.Data(),
                  HAD_TIME_FINEBINS, 0, HAD_TIME_FINEBINS, "bin", "ps");
              if (IsObjMade())
                {
                  // reset time bin correction arrays:
                  for (int fb = 0; fb < HAD_TIME_FINEBINS; ++fb)
                    {
                      hCalBinTime[b][t][i]->SetBinContent(fb + 1,
                          fb * HAD_TIME_COARSEUNIT / HAD_TIME_FINEBINS);
                    }
                }



              obname.Form("%s/Chan%02d/UnCalTableBinTime_%02d_%02d_%02d",
                  dirname.Data(), i, b, t, i);
              obtitle.Form("UnCalibrated Table of TRB %02d TDC %02d Channel %02d", b, t, i);
              hUnCalBinTime[b][t][i] = MakeTH1('D', obname.Data(), obtitle.Data(),
                  HAD_TIME_FINEBINS, 0, HAD_TIME_FINEBINS, "bin", "ps");
              if (IsObjMade())
                {
                  // reset time bin correction arrays:
                  for (int fb = 0; fb < HAD_TIME_FINEBINS; ++fb)
                    {
                      hUnCalBinTime[b][t][i]->SetBinContent(fb + 1,
                          fb * HAD_TIME_COARSEUNIT / HAD_TIME_FINEBINS);
                    }
                }

            } // for i

          dirname.Form("Timetest/TRB%02d/TDC%02d", b, t);

          // leading edges map:
          obname.Form("%s/LeadingCoarseChannelsMap_%02d_%02d", dirname.Data(), b, t);
          obtitle.Form("Coarse times Leading edges of all channels TRB %02d TDC %02d",
              b, t);
          tbins = HAD_TIME_COARSEHISTBINS / 2;
          trange = HAD_TIME_COARSEBINS * HAD_TIME_COARSEUNIT;
          hLeadingCoarseAllChans[b][t] = MakeTH2('I', obname.Data(),
              obtitle.Data(), HAD_TIME_CHANNELS, 0, HAD_TIME_CHANNELS, tbins, 0,
              trange, "channel", "t (ns)");

          obname.Form("%s/LeadingFineChannelsMap_%02d_%02d", dirname.Data(), b, t);
          obtitle.Form("Fine times Leading edges of all channels TRB %02d TDC %02d", b, t);
          tbins = HAD_TIME_FINEBINS / 4;
          trange = HAD_TIME_FINEBINS;
          hLeadingFineAllChans[b][t] = MakeTH2('I', obname.Data(), obtitle.Data(),
              HAD_TIME_CHANNELS, 0, HAD_TIME_CHANNELS, tbins, 0, trange,
              "channel", "t");

          // trailing edges map:
          obname.Form("%s/TrailingCoarseChannelsMap_%02d_%02d", dirname.Data(), b, t);
          obtitle.Form("Coarse times Trailing edges of all channels  TRB %02d TDC %02d",
              b, t);
          tbins = HAD_TIME_COARSEHISTBINS / 2;
          trange = HAD_TIME_COARSEBINS * HAD_TIME_COARSEUNIT;
          hTrailingCoarseAllChans[b][t] = MakeTH2('I', obname.Data(),
              obtitle.Data(), HAD_TIME_CHANNELS, 0, HAD_TIME_CHANNELS, tbins, 0,
              trange, "channel", "t (ns)");

          obname.Form("%s/TrailingFineChannelsMap_%02d_%02d", dirname.Data(), b, t);
          obtitle.Form("Fine times Trailing edges of all channels  TRB %02d TDC %02d", b, t);
          tbins = HAD_TIME_FINEBINS / 4;
          trange = HAD_TIME_FINEBINS;
          hTrailingFineAllChans[b][t] = MakeTH2('I', obname.Data(), obtitle.Data(),
              HAD_TIME_CHANNELS, 0, HAD_TIME_CHANNELS, tbins, 0, trange,
              "channel", "t");

          obname.Form("%s/TriggerScaler_%02d_%02d", dirname.Data(), b, t);
          obtitle.Form("Empty vs Filled events TRB %02d TDC %02d", b, t);

          hTriggerCount[b][t] = MakeTH1('I', obname.Data(), obtitle.Data(),
              3 + HAD_TIME_CHANNELS, 0, 3 + HAD_TIME_CHANNELS);
          if (IsObjMade())
            {

              hTriggerCount[b][t]->GetXaxis()->SetBinLabel(1, "ALL");
              hTriggerCount[b][t]->GetXaxis()->SetBinLabel(2, "LEADING");
              hTriggerCount[b][t]->GetXaxis()->SetBinLabel(3, "TRAILING");
              TString text;
              for (int c = 0; c < HAD_TIME_CHANNELS; ++c)
                {
                  text.Form("PAIRS-CH%02d", c);
                  hTriggerCount[b][t]->GetXaxis()->SetBinLabel(c + 4, text.Data());
                }
            }

          //// conditions:

          obname.Form("%s/LeadingCoarseTimeGate_%02d_%02d", dirname.Data(), b, t);
          trange = HAD_TIME_COARSEBINS * HAD_TIME_COARSEUNIT;
          hname.Form("%s_%02d_%02d", leadingcoarseallname.Data(), b, t);
          cLeadingCoarseTimeGate[b][t] = MakeWinCond(obname.Data(), 0, trange,
              hname.Data());
          obname.Form("%s/LeadingFineTimeGate_%02d_%02d", dirname.Data(), b, t);
          trange = HAD_TIME_FINEBINS;
          hname.Form("%s_%02d_%02d", leadingfineallname.Data(), b, t);
          cLeadingFineTimeGate[b][t] = MakeWinCond(obname.Data(), 0, trange,
              hname.Data());

          obname.Form("%s/TrailingCoarseTimeGate_%02d_%02d", dirname.Data(), b, t);
          trange = HAD_TIME_COARSEBINS * HAD_TIME_COARSEUNIT;
          hname.Form("%s_%02d_%02d", trailingcoarseallname.Data(), b, t);
          cTrailingCoarseTimeGate[b][t] = MakeWinCond(obname.Data(), 0, trange,
              hname.Data());
          obname.Form("%s/TrailingFineTimeGate_%02d_%02d", dirname.Data(), b, t);
          trange = HAD_TIME_FINEBINS;
          hname.Form("%s_%02d_%02d", trailingfineallname.Data(), b, t);
          cTrailingFineTimeGate[b][t] = MakeWinCond(obname.Data(), 0, trange,
              hname.Data());

          // pictures:
// NOTE: displaying this picture in GO4 may crash the X-server
// probably due to large number of bins for each histogram. JAM
//      obname.Form("LeadingCoarseTimes_%02d_%02d", b, t);
//      if (GetPicture(obname.Data()) == 0)
//        {
//          obtitle.Form("Overview all Leading edge Coarse Times TRB %02d TDC %02d", t);
//          TGo4Picture* pic = new TGo4Picture(obname.Data(), obtitle.Data());
//          int numy = 4;
//          int numx = HAD_TIME_CHANNELS / numy;
//          if (numx * numy < (HAD_TIME_CHANNELS))
//            numx++;
//          pic->SetDivision(numy, numx);
//          for (int ch = 0; ch < HAD_TIME_CHANNELS; ch++)
//            {
//              int nx = (ch) / numy;
//              int ny = (ch) % numy;
//              pic->Pic(ny, nx)->AddObject(hLeadingCoarse[b][t][ch]);
//              pic->Pic(ny, nx)->SetFillAtt(4, 3001); // pattern
//              pic->Pic(ny, nx)->SetLineAtt(4, 1, 1);
//
//              //pic->Pic(ny, nx)->SetLogScale(1);
//            }
//          AddPicture(pic, dirname.Data());
//        }

          obname.Form("LeadingFineTimes_%02d_%02d", b, t);
          if (GetPicture(obname.Data()) == 0)
            {
              obtitle.Form("Overview all Leading edge Fine Times TRB %02d TDC %02d", b, t);
              TGo4Picture* pic = new TGo4Picture(obname.Data(), obtitle.Data());
              int numy = 4;
              int numx = HAD_TIME_CHANNELS / numy;
              if (numx * numy < (HAD_TIME_CHANNELS))
                numx++;
              pic->SetDivision(numy, numx);
              for (int ch = 0; ch < HAD_TIME_CHANNELS; ch++)
                {
                  int nx = (ch) / numy;
                  int ny = (ch) % numy;
                  pic->Pic(ny, nx)->AddObject(hLeadingFine[b][t][ch]);
                  pic->Pic(ny, nx)->SetFillAtt(6, 3001); // pattern
                  pic->Pic(ny, nx)->SetLineAtt(6, 1, 1);
                  //pic->Pic(ny, nx)->SetLogScale(1);
                }
              AddPicture(pic, dirname.Data());
            }
//      obname.Form("LeadingDeltaTimesRaw_%02d_%02d", b, t);
//      if (GetPicture(obname.Data()) == 0)
//        {
//          obtitle.Form(
//              "Overview all Uncalibrated Leading edge Time deltas TRB %02d TDC %02d", b, t);
//          TGo4Picture* pic = new TGo4Picture(obname.Data(), obtitle.Data());
//          int numy = 4;
//          int numx = HAD_TIME_CHANNELS / numy;
//          if (numx * numy < (HAD_TIME_CHANNELS))
//            numx++;
//          pic->SetDivision(numy, numx);
//          for (int ch = 0; ch < HAD_TIME_CHANNELS; ch++)
//            {
//              int nx = (ch) / numy;
//              int ny = (ch) % numy;
//              pic->Pic(ny, nx)->AddObject(hLeadingDeltaRaw[b][t][ch]);
//              pic->Pic(ny, nx)->SetFillAtt(4, 3001); // pattern
//              pic->Pic(ny, nx)->SetLineAtt(4, 1, 1);
//              //pic->Pic(ny, nx)->SetLogScale(1);
//            }
//          AddPicture(pic, dirname.Data());
//        }

//      obname.Form("LeadingDeltaTimesCal_%02d_%02d", b, t);
//      if (GetPicture(obname.Data()) == 0)
//        {
//          obtitle.Form(
//              "Overview all Calibrated Leading edge Time deltas TRB %02d TDC %02d", b, t);
//          TGo4Picture* pic = new TGo4Picture(obname.Data(), obtitle.Data());
//          int numy = 4;
//          int numx = HAD_TIME_CHANNELS / numy;
//          if (numx * numy < (HAD_TIME_CHANNELS))
//            numx++;
//          pic->SetDivision(numy, numx);
//          for (int ch = 0; ch < HAD_TIME_CHANNELS; ch++)
//            {
//              int nx = (ch) / numy;
//              int ny = (ch) % numy;
//              pic->Pic(ny, nx)->AddObject(hLeadingDeltaCal[b][t][ch]);
//              pic->Pic(ny, nx)->SetFillAtt(6, 3001); // pattern
//              pic->Pic(ny, nx)->SetLineAtt(6, 1, 1);
//              //pic->Pic(ny, nx)->SetLogScale(1);
//            }
//          AddPicture(pic, dirname.Data());
//        }

//      obname.Form("LeadingCorrelationsRaw__%02d_%02d", b, t);
//      if (GetPicture(obname.Data()) == 0)
//        {
//          obtitle.Form(
//              "Overview all leading edge fine time correlations TRB %02d TDC %02d", b, t);
//          TGo4Picture* pic = new TGo4Picture(obname.Data(), obtitle.Data());
//          int numy = 4;
//          int numx = HAD_TIME_CHANNELS / numy;
//          if (numx * numy < (HAD_TIME_CHANNELS))
//            numx++;
//          pic->SetDivision(numy, numx);
//          for (int ch = 0; ch < HAD_TIME_CHANNELS; ch++)
//            {
//              int nx = (ch) / numy;
//              int ny = (ch) % numy;
//              pic->Pic(ny, nx)->AddObject(hLeadingCorrelFine[b][t][ch]);
//              pic->Pic(ny, nx)->SetFillAtt(4, 3001); // pattern
//              pic->Pic(ny, nx)->SetLineAtt(4, 1, 1);
//              //pic->Pic(ny, nx)->SetLogScale(1);
//            }
//          AddPicture(pic, dirname.Data());
//        }

          /// trailing:

          obname.Form("TrailingFineTimes_%02d_%02d", b, t);
          if (GetPicture(obname.Data()) == 0)
            {
              obtitle.Form("Overview all Trailing edge Fine Times TRB %02d TDC %02d", b, t);
              TGo4Picture* pic = new TGo4Picture(obname.Data(), obtitle.Data());
              int numy = 4;
              int numx = HAD_TIME_CHANNELS / numy;
              if (numx * numy < (HAD_TIME_CHANNELS))
                numx++;
              pic->SetDivision(numy, numx);
              for (int ch = 0; ch < HAD_TIME_CHANNELS; ch++)
                {
                  int nx = (ch) / numy;
                  int ny = (ch) % numy;
                  pic->Pic(ny, nx)->AddObject(hTrailingFine[b][t][ch]);
                  pic->Pic(ny, nx)->SetFillAtt(6, 3001); // pattern
                  pic->Pic(ny, nx)->SetLineAtt(6, 1, 1);
                  //pic->Pic(ny, nx)->SetLogScale(1);
                }
              AddPicture(pic, dirname.Data());
            }
//         obname.Form("TrailingDeltaTimesRaw_%02d_%02d", b, t);
//         if (GetPicture(obname.Data()) == 0)
//           {
//             obtitle.Form(
//                 "Overview all Uncalibrated Trailing edge Time deltas TRB %02d TDC %02d", b, t);
//             TGo4Picture* pic = new TGo4Picture(obname.Data(), obtitle.Data());
//             int numy = 4;
//             int numx = HAD_TIME_CHANNELS / numy;
//             if (numx * numy < (HAD_TIME_CHANNELS))
//               numx++;
//             pic->SetDivision(numy, numx);
//             for (int ch = 0; ch < HAD_TIME_CHANNELS; ch++)
//               {
//                 int nx = (ch) / numy;
//                 int ny = (ch) % numy;
//                 pic->Pic(ny, nx)->AddObject(hTrailingDeltaRaw[b][t][ch]);
//                 pic->Pic(ny, nx)->SetFillAtt(4, 3001); // pattern
//                 pic->Pic(ny, nx)->SetLineAtt(4, 1, 1);
//                 //pic->Pic(ny, nx)->SetLogScale(1);
//               }
//             AddPicture(pic, dirname.Data());
//           }

//         obname.Form("TrailingDeltaTimesCal_%02d_%02d", b, t);
//         if (GetPicture(obname.Data()) == 0)
//           {
//             obtitle.Form(
//                 "Overview all Calibrated Trailing edge Time deltas TRB %02d TDC %02d", b, t);
//             TGo4Picture* pic = new TGo4Picture(obname.Data(), obtitle.Data());
//             int numy = 4;
//             int numx = HAD_TIME_CHANNELS / numy;
//             if (numx * numy < (HAD_TIME_CHANNELS))
//               numx++;
//             pic->SetDivision(numy, numx);
//             for (int ch = 0; ch < HAD_TIME_CHANNELS; ch++)
//               {
//                 int nx = (ch) / numy;
//                 int ny = (ch) % numy;
//                 pic->Pic(ny, nx)->AddObject(hTrailingDeltaCal[b][t][ch]);
//                 pic->Pic(ny, nx)->SetFillAtt(6, 3001); // pattern
//                 pic->Pic(ny, nx)->SetLineAtt(6, 1, 1);
//                 //pic->Pic(ny, nx)->SetLogScale(1);
//               }
//             AddPicture(pic, dirname.Data());
//           }

//         obname.Form("TrailingCorrelationsRaw_%02d_%02d", b, t);
//         if (GetPicture(obname.Data()) == 0)
//           {
//             obtitle.Form(
//                 "Overview all leading edge fine time correlations TRB %02d TDC %02d", b, t);
//             TGo4Picture* pic = new TGo4Picture(obname.Data(), obtitle.Data());
//             int numy = 4;
//             int numx = HAD_TIME_CHANNELS / numy;
//             if (numx * numy < (HAD_TIME_CHANNELS))
//               numx++;
//             pic->SetDivision(numy, numx);
//             for (int ch = 0; ch < HAD_TIME_CHANNELS; ch++)
//               {
//                 int nx = (ch) / numy;
//                 int ny = (ch) % numy;
//                 pic->Pic(ny, nx)->AddObject(hTrailingCorrelFine[b][t][ch]);
//                 pic->Pic(ny, nx)->SetFillAtt(4, 3001); // pattern
//                 pic->Pic(ny, nx)->SetLineAtt(4, 1, 1);
//                 //pic->Pic(ny, nx)->SetLogScale(1);
//               }
//             AddPicture(pic, dirname.Data());
//           }

        } // for t

    }

#endif

#ifdef HAD_USE_TDC
  TString leadhistoname;
  TString trailhistoname;

  Int_t tbins=HAD_TDC_BINS / 64;

  leadhistoname="LeadingTime_All"; // need this name later to link with condition
  obname.Form("TDC/%s",leadhistoname.Data());
  obtitle.Form("Leading Edge deltaT all channels");
  hLeadingDeltaTAll=MakeTH1('I', obname.Data(), obtitle.Data(),tbins,-HAD_TDC_BINS/2, HAD_TDC_BINS/2,"t-t(31)");
  trailhistoname="TrailingTime_All";// need this name later to link with condition
  obname.Form("TDC/%s",trailhistoname.Data());

  obtitle.Form("Trailing Edge deltaT all channels");
  hTrailingDeltaTAll=MakeTH1('I', obname.Data(), obtitle.Data(),tbins,-HAD_TDC_BINS/2, HAD_TDC_BINS/2,"t-t(31)");

  for(int tdc=0; tdc<HAD_TDC_NUMTDC;++tdc)
    {
      obname.Form("TDC/TDC%2d/LeadingHits_TDC%2d", tdc,tdc);
      obtitle.Form("Leading Edge Channel hits accumulated TDC%2d",tdc);
      hLeadingChannelHits[tdc]=MakeTH1('I', obname.Data(), obtitle.Data(),HAD_TDC_CHANNELS,0,HAD_TDC_CHANNELS,"channel","hit count");

      obname.Form("TDC/TDC%2d/TrailingHits_TDC%2d", tdc,tdc);
      obtitle.Form("Trailng Edge Channel hits accumulated TDC%2d",tdc);
      hTrailingChannelHits[tdc]=MakeTH1('I', obname.Data(), obtitle.Data(),HAD_TDC_CHANNELS,0,HAD_TDC_CHANNELS,"channel","hit count");

      //Int_t tbins=HAD_TDC_BINS;
      for(int ch=0; ch<HAD_TDC_CHANNELS;++ch)
        {
          obname.Form("TDC/TDC%2d/CH%2d/LeadingTime_TDC%2d_CH%2d", tdc,ch,tdc,ch);
          obtitle.Form("Leading Edge deltaT TDC%2d CH%2d",tdc,ch);
          hLeadingDeltaT[tdc][ch]=MakeTH1('I', obname.Data(), obtitle.Data(),tbins,-HAD_TDC_BINS/2, HAD_TDC_BINS/2,"t-t(31)");
          obname.Form("TDC/TDC%2d/CH%2d/TrailingTime_TDC%2d_CH%2d", tdc,ch,tdc,ch);
          obtitle.Form("Trailing Edge deltaT TDC%2d CH%2d",tdc,ch);
          hTrailingDeltaT[tdc][ch]=MakeTH1('I', obname.Data(), obtitle.Data(),tbins,-HAD_TDC_BINS/2, HAD_TDC_BINS/2,"t-t(31)");
        }
    }
  obname.Form("TDC/LeadingTimeMap");
  obtitle.Form("Leading Edge times of all channels");
  hLeadingDtAllChans=MakeTH2('I', obname.Data(), obtitle.Data(),HAD_TDC_CHANNELS*HAD_TDC_NUMTDC,0,HAD_TDC_CHANNELS*HAD_TDC_NUMTDC,100,-10000,10000,"global channel","#Delta t");
  obname.Form("TDC/TrailingTimeMap");
  obtitle.Form("Trailing Edge times of all channels");
  hTrailingDtAllChans=MakeTH2('I', obname.Data(), obtitle.Data(),HAD_TDC_CHANNELS*HAD_TDC_NUMTDC,0,HAD_TDC_CHANNELS*HAD_TDC_NUMTDC,100,-10000,10000,"global channel","#Delta t");

  obname.Form("TDC/DeltaTimeMap");
  obtitle.Form("Leading-Trailing Edge times of all channels");
  hDeltaDtAllChans=MakeTH2('I', obname.Data(), obtitle.Data(),HAD_TDC_CHANNELS*HAD_TDC_NUMTDC,0,HAD_TDC_CHANNELS*HAD_TDC_NUMTDC, 2000, 0 ,2000,"global channel","#Delta t");

  //// conditions:
  obname.Form("TDC/LeadingTimeRange");
  cLeadingDtRange=MakeWinCond(obname.Data(), -5000, 5000,leadhistoname.Data());
  obname.Form("TDC/TrailingTimeRange");
  cTrailingDtRange=MakeWinCond(obname.Data(), -5000, 5000,trailhistoname.Data());

  //// pictures:
  obname.Form("TDC_AllHits");
  if (GetPicture(obname.Data())==0)
    {
      TGo4Picture* pic=new TGo4Picture(obname.Data(), "Hit Overview of all TDCS");
      int numy=4;
      int numx=HAD_TDC_NUMTDC / numy;
      if (numx*numy < (HAD_TDC_NUMTDC)) numx++;
      pic->SetDivision(numy, numx);
      for (int tdc=0; tdc < HAD_TDC_NUMTDC; tdc++)
        {
          int nx=(tdc) / numy;
          int ny=(tdc) % numy;
          pic->Pic(ny, nx)->AddObject(hLeadingChannelHits[tdc]);
          pic->Pic(ny, nx)->AddObject(hTrailingChannelHits[tdc]);
          //pic->Pic(ny, nx)->SetLogScale(1);
        }
      AddPicture(pic,"TDC");
    }

  for(int tdc=0; tdc<HAD_TDC_NUMTDC;++tdc)
    {
      obname.Form("TDC%2d_Channels",tdc);
      obtitle.Form("Leading/Trailing Edges times TDC%2d",tdc);
      if (GetPicture(obname.Data())==0)
        {
          TGo4Picture* pic=new TGo4Picture(obname.Data(), obtitle.Data());
          int numy=4;
          int numx=HAD_TDC_CHANNELS / numy;
          if (numx*numy < HAD_TDC_CHANNELS) numx++;
          pic->SetDivision(numy, numx);
          for (int ch=0; ch < HAD_TDC_CHANNELS; ch++)
            {
              int nx=(ch) / numy;
              int ny=(ch) % numy;
              pic->Pic(ny, nx)->AddObject(hLeadingDeltaT[tdc][ch]);
              pic->Pic(ny, nx)->AddObject(hTrailingDeltaT[tdc][ch]);
              //pic->Pic(ny, nx)->AddCondition(cLeadingDtRange);
              //pic->Pic(ny, nx)->AddCondition(cTrailingDtRange);
              pic->Pic(ny, nx)->SetRangeX(-10000, 10000);
              //pic->Pic(ny, nx)->SetLogScale(1);

            }
          foldername.Form("TDC/TDC%2d",tdc);
          AddPicture(pic,foldername.Data());
        }
    }

#endif

}
//***********************************************************
THadaqUnpackProc::~THadaqUnpackProc()
{

}
//***********************************************************

Bool_t
THadaqUnpackProc::CheckEventClass(TClass* cl)
{
  return cl->InheritsFrom(THadaqUnpackEvent::Class());
}

//-----------------------------------------------------------

Bool_t
THadaqUnpackProc::BuildEvent(TGo4EventElement* dest)
{

  TGo4MbsEvent *inp = dynamic_cast<TGo4MbsEvent*>(GetInputEvent());
  fOutEvent = dynamic_cast<THadaqUnpackEvent*>(dest);

  if ((inp == 0) || (fOutEvent == 0))
    {
      cout << "HadaqUnpackProc: events are not specified!" << endl;
      return kFALSE;
    }
  inp->ResetIterator();
  TGo4MbsSubEvent* psubevt = 0;
  // we should have only one mbs subevent from hadaq, but we keep loop anyway for zero checking!
  while ((psubevt = inp->NextSubEvent()) != 0)
    {
      // loop over subevents
      Int_t *pdata = psubevt->GetDataField();
      Int_t lwords = psubevt->GetIntLen();
      if (lwords * sizeof(Int_t) < sizeof(Hadaq_Event))
        {
          GO4_STOP_ANALYSIS_MESSAGE(
              "Problem: subevent length 0x%x not big enough for hadaq event header!!", lwords*sizeof(Int_t));
        }
      char* cursor = (char*) pdata;
      char* datastart = (char*) pdata;	
       EPRINT("*** Start cursor #0x%x\n",  (unsigned long) cursor);
	

      Int_t bufbytes=lwords*sizeof(Int_t);
      while(bufbytes>0)
        {
      // here the full hadaq event is available at pdata:
      Hadaq_Event* hadevent = (Hadaq_Event*) (cursor);
      EPRINT(
          "*** Found hadaq event #0x%x, full id=0x%x , size 0x%x\n", hadevent->GetSeqNr(), hadevent->GetId(), (unsigned) hadevent->GetSize());

      cursor += sizeof(Hadaq_Event);
      int evlen = hadevent->GetSize();
      evlen -= sizeof(Hadaq_Event);
      bufbytes -= sizeof(Hadaq_Event);
      int sub = 0; // subevent index for Go4 output event
      while (evlen > 0)
        {
          Hadaq_Subevent* hadsubevent = (Hadaq_Subevent*) (cursor);
          EPRINT(
              "*** --- hadaq subevent no.0x%x of size 0x%x, decoding=0x%x, id=0x%x, trig=0x%x, swapped=0x%x \n", sub, (unsigned) hadsubevent->GetSize(), hadsubevent->GetDecoding(), hadsubevent->GetId(), hadsubevent->GetTrigNr(), hadsubevent->IsSwapped());
          ProcessSubevent(hadsubevent);
          cursor += hadsubevent->GetSize();
          evlen -= hadsubevent->GetSize();
          bufbytes -= hadsubevent->GetSize();
          /* skip possible padding bytes before next subevent:*/
          while (((((long) cursor - (long) datastart)) % 8) != 0) // hadaq padding is relative to begin of payload data field!
            {
              EPRINT("*** --- skipping subevent padding byte..\n");
              cursor++;
              evlen--;
              bufbytes--;
            }
          sub++;

        }


      //cout <<"HadaqUnpackProc - subevent of id "<< (int) psubevt->GetSubcrate() << endl;
        } // while bufbytes>0

    } // while next subevent
  return kTRUE;
}

void
THadaqUnpackProc::ProcessSubevent(Hadaq_Subevent* hadsubevent)
{
  if (hadsubevent == 0)
    return;

  switch (hadsubevent->GetId())
    {
  case 0x368:
    DumpData(hadsubevent);
    ProcessTimeTest(hadsubevent);
    EvaluateTDCData();
    break;
  case 0x8c00:
    DumpData(hadsubevent);
    ProcessTimeTestV3(hadsubevent);
    for (int b = 0; b < HAD_TIME_NUMBOARDS; ++b)
      {
        if(!THadaqUnpackEvent::AssertTRB(b)) continue;
        for (int t = 0; t < HAD_TIME_NUMTDC; ++t)
          {
            EvaluateTDCData(b, t);
          }
      }
    break;
  case 0x8800:
    DumpData(hadsubevent);
    ProcessTDC(hadsubevent);
    break;
  default:
    DumpData(hadsubevent);
    break;
    };
  fEventCount++;
}

void
THadaqUnpackProc::ProcessTimeTestV3(Hadaq_Subevent* hadsubevent)
{
  int align = hadsubevent->Alignment();
  int ixlen = hadsubevent->GetSize() - sizeof(Hadaq_Subevent);
  ixlen = ixlen / (align); // 1,2,4 bytes
  UChar_t trb = 0;
  UChar_t tdc = 0;
  UShort_t dlen = 0;
  UShort_t totlen = 0;
  //Bool_t hasdata = kFALSE;
  for (int ix = 0; ix < ixlen; ++ix)
    {
      //if (totlen && ix >= totlen)
      //  break; // ignore data after tdc block
      Int_t data = hadsubevent->Data(ix);

#ifdef  HAD_USE_MULTITRB
      Bool_t istdchead = ((data >> 8) & 0xFF) == 0xC0;
#else
      Bool_t istdchead = ((data >> 4) & 0xFFF) == 0xF30;
#endif
      if((dlen--) > 0) istdchead=kFALSE; // suppress "false" headers in data stream
      if (istdchead)
        {
         tdc = (data & 0xF);
#ifdef  HAD_USE_MULTITRB
         trb=   ((data >> 4 ) & 0xF);
#endif
          dlen = (data >> 16);


          EPRINT(
                       "***  --- tdc header: 0x%x, trb=%d tdc=%d, data length=%d\n", data, trb, tdc, dlen);


#ifdef  HAD_USE_MULTITRB
          if (tdc == 0)
#else
          if (tdc == 5)
#endif
            {
              totlen = dlen;
              EPRINT("***  found tdc subevt data length=%d\n", totlen);
              dlen=0;
            }
#ifdef  HAD_USE_MULTITRB
          else
          {
                tdc-=1; // indices begin with 1 in this format
          }
#endif



          continue;


        }


      Bool_t isdataheader = ((data >> 29) & 0x7) == 0x1;
      if (isdataheader)
        {
          UShort_t res = (data >> 16) & 0x1FFF;
          UShort_t err = data & 0xFFFF;
          EPRINT(
              "***  --- data header: 0x%x, reserved=0x%x, errorbits=0x%x\n", data, res, err);
          if (err)
            {
              printf("!!! found error bits: 0x%x at tdc 0x%x\n", err, tdc);
            }
          continue;
        }
      Bool_t isdebug = ((data >> 29) & 0x7) == 0x2;
      if (isdebug)
        {
          UShort_t mode = (data >> 24) & 0x1F;
          UShort_t bits = data & 0xFFFFFF;
          EPRINT(
              "***  --- debug word: 0x%x, mode=0x%x, bits=0x%x\n", data, mode, bits);
          continue;
        }
      Bool_t isreserved = ((data >> 29) & 0x7) == 0x3;
      if (isreserved)
        {
          UShort_t res = data & 0x1FFFFFFF;
          EPRINT("***  --- reserved word: 0x%x, res=0x%x\n", data, res);
          continue;
        }
      Bool_t isdata = ((data >> 31) & 0x1) == 0x1;
      if (isdata)
        {
	  UShort_t res = (data >> 28) & 0x7;
          UChar_t chan = (data >> 22) & 0x3F;
          UShort_t tcoarse = data & 0x7FF;
          UShort_t tfine = (data >> 12) & 0x3FF;
          Bool_t isrising = ((data >> 11) & 0x1) == 0x1;

          EPRINT(
		 "***  --- data word: 0x%x, resserved=0x%x, chan=0x%x, isrising=%d, tc= 0x%x, tf=0x%x\n", data, res, chan, isrising, tcoarse, tfine);

          if (THadaqUnpackEvent::AssertTRB(trb) && (tdc < HAD_TIME_NUMTDC) && (chan < HAD_TIME_CHANNELS))
            {
              if (isrising)
                {
                  fOutEvent->fLeadingCoarseTime[trb][tdc][chan].push_back(tcoarse);
                  fOutEvent->fLeadingFineTime[trb][tdc][chan].push_back(tfine);
                  hTriggerCount[trb][tdc]->Fill(1); // record that we have data from this tdc
                }
              else
                {
                  fOutEvent->fTrailingCoarseTime[trb][tdc][chan].push_back(tcoarse);
                  fOutEvent->fTrailingFineTime[trb][tdc][chan].push_back(tfine);
                  hTriggerCount[trb][tdc]->Fill(2); // record that we have data from this tdc
                }

            }
          else
            {
              GO4_STOP_ANALYSIS_MESSAGE(
                  "Data index out of range error trb:0x%x tdc:0x%x channel:0x%x !!", trb,tdc, chan);
            }

        }

      //hasdata=kTRUE;

    } // for ix

  for (int b = 0; b < HAD_TIME_NUMBOARDS; ++b)
    {
      if (!THadaqUnpackEvent::AssertTRB(b))
        continue;
      for (int t = 0; t < HAD_TIME_NUMTDC; ++t)
        {
//          for (int c = 0; c < HAD_TIME_CHANNELS; ++c)
//            {
              hTriggerCount[b][t]->Fill(0); // account this event for all tdcs which may have fired
//            }
        }
    }

}

void
THadaqUnpackProc::ProcessTimeTest(Hadaq_Subevent* hadsubevent)
{
#ifdef HAD_USE_CAHIT
  int align = hadsubevent->Alignment();
  int ixlen = hadsubevent->GetSize() - sizeof(Hadaq_Subevent);
  ixlen = ixlen / (align); // 1,2,4 bytes
  UShort_t oldlecount = 0;
  UShort_t lecount = 0;
  UInt_t err = 0;
  UChar_t chan = 0;
  UShort_t tcoarse = 0;
  UShort_t tfine = 0;
  Bool_t hasbegin = kFALSE;
  Bool_t haserror = kFALSE;
  Bool_t hasend = kFALSE;
  Bool_t hasdata = kFALSE;
  for (int ix = 0; ix < ixlen; ++ix)
    {
      Int_t data = hadsubevent->Data(ix);
      // we see two additional words in front of data, meaning?
      // assume some internal header reflecting trigger count
      if (ix < 2)
        {
          Int_t header = hadsubevent->Data(ix);
          EPRINT("***  --- Local subheader: 0x%x\n", header);
          continue;
        }

      UShort_t id = (data >> 24) & 0xFF;
      //EPRINT("***  ID: 0x%x\n",id);

      switch (id)
        {
      case 0xaa:
        lecount = data & 0xFFFF;
        oldlecount = lecount;
        hasbegin = kTRUE;
        EPRINT("***  --- Begin word, Local Event counter:0x%x\n", lecount);
        break;
      case 0xbb:
        lecount = data & 0xFFFF;
        EPRINT("***  --- End word, Local Event counter:0x%x\n", lecount);
        hasend = kTRUE;
        if (lecount != oldlecount)
          GO4_STOP_ANALYSIS_MESSAGE(
              "Data Error: local event count inconsistent 0x%x != 0x%x!!", oldlecount, lecount);
        if (!hasbegin)
          GO4_STOP_ANALYSIS_MESSAGE(
              "Data Error: end word without begin word!");
        if (!haserror)
          GO4_STOP_ANALYSIS_MESSAGE(
              "Data Error: end word without error word!");
        break;
      case 0xee:
        err = (data & 0xFFFFFF);
        EPRINT("***  --- Error word, error 0x%x\n", err);
        haserror = kTRUE;
        if (!hasbegin)
          GO4_STOP_ANALYSIS_MESSAGE(
              "Data Error: error word without begin word!");
        if (lecount != oldlecount)
          GO4_STOP_ANALYSIS_MESSAGE(
              "Data Error: local event count inconsistent 0x%x != 0x%x!!", oldlecount, lecount);
        break;

      default:
        // all other words should contain channel data
        if (!hasbegin)
          GO4_STOP_ANALYSIS_MESSAGE(
              "Data Error: data channel word without begin word!");
        hasdata = kTRUE;
        chan = (data >> 26) & 0x3F;
        tcoarse = (data >> 10) & 0xFFFF;
        tfine = data & 0x3FF;
        EPRINT(
            "***  --- Data, ch:0x%x, tcoarse:0x%x, tfine:0x%x\n", chan, tcoarse, tfine);
        if ((chan < HAD_TIME_CHANNELS))
          {
            fOutEvent->fLeadingCoarseTime[0][0][chan].push_back(tcoarse);
            fOutEvent->fLeadingFineTime[0][0][chan].push_back(tfine);
          }
        else
          {
            GO4_STOP_ANALYSIS_MESSAGE(
                "Data channel out of range error  channel:0x%x !!", chan);
          }

        break;
        }; // switch

    } // for
  if (!hasend)
    GO4_STOP_ANALYSIS_MESSAGE( "Data Error: end of subevent without end word!");

  // if (fOutEvent->fCoarseTime[0].size() != fOutEvent->fCoarseTime[7].size())
//     GO4_STOP_ANALYSIS_MESSAGE("Data Error: unequal hit number per channel!");

  hTriggerCount[0][0]->Fill(0);
  if (hasdata)
    hTriggerCount[0][0]->Fill(1);

  // now process all channels:
//   for (int ch = 0; ch < HAD_TIME_CHANNELS; ++ch) {
//
//      for (unsigned int i = 0; i < fOutEvent->fCoarseTime[ch].size(); i++) {
//         Double_t val = fOutEvent->fCoarseTime[ch].at(i) * HAD_TIME_COARSEUNIT;
//         //hCoarseAll->Fill(val); don't need it
//         if (cCoarseTimeGate->Test(val)) {
//            hCoarse[ch]->Fill(val);
//            hCoarseAllChans->Fill(ch, val);
//            hChannelCount->Fill(ch);
//         }
//      }
//      for (unsigned int i = 0; i < fOutEvent->fLeadingFineTime[tdc][ch].size(); i++) {
//         Double_t val = fOutEvent->fLeadingFineTime[tdc][ch].at(i);
//         //hFineAll->Fill(val); don't need it
//         if (cFineTimeGate->Test(val)) {
//            hFine[ch]->Fill(val);
//            hFineBuffer[ch]->Fill(val);
//            hFineAllChans->Fill(ch, val);
//         }
//      }
//
//      if (fEventCount && ((fEventCount % fPar->calibrationPeriod) == 0)) {
//         DoCalibration(ch, fPar->generalCalibration,
//               fPar->continuousCalibration);
//      }
//
//   } // channels first loop
//
//   if (fPar->continuousCalibration && !fbHasCalibration)
//      return;
//
//   // after calibration, we process channels again using calibrated times:
//
//   for (int ch = 0; ch < HAD_TIME_CHANNELS; ++ch) {
//      // calculate delta ts with setup
//      Short_t ref = fPar->deltaChannels[ch];
//      if (ref < 0)
//         continue; // skip not configured delta channels
//      for (unsigned int i = 0; i < fOutEvent->fCoarseTime[ch].size(); i++) {
//         Double_t delta = 0, cdelta = 0, fdelta = 0, coarse = 0, fine = 0,
//               rcoarse = 0, rfine = 0;
//         Double_t delta_cal = 0, fdelta_cal = 0, fine_cal = 0, rfine_cal = 0;
//         UShort_t binindex = 0, rbinindex = 0;
//
//         coarse = fOutEvent->fCoarseTime[ch].at(i) * HAD_TIME_COARSEUNIT;
//         if (i < fOutEvent->fCoarseTime[ref].size()) {
//            hTriggerCount->Fill(ch + 2); //account the matching pairs found
//            rcoarse = fOutEvent->fCoarseTime[ref].at(i) * HAD_TIME_COARSEUNIT;
//            cdelta = coarse - rcoarse;
//         }
//
//         if (i < fOutEvent->fLeadingFineTime[tdc][ch].size()) {
//            binindex = fOutEvent->fLeadingFineTime[tdc][ch].at(i);
//            fine = hUnCalBinTime[ch]->GetBinContent(binindex + 1);
//            fine_cal = hCalBinTime[ch]->GetBinContent(binindex + 1);
//         }
//
//         if (i < fOutEvent->fLeadingFineTime[ref].size()) {
//            rbinindex = fOutEvent->fLeadingFineTime[ref].at(i);
//            rfine = hUnCalBinTime[ref]->GetBinContent(rbinindex + 1);
//            rfine_cal = hCalBinTime[ref]->GetBinContent(rbinindex + 1);
//            fdelta = fine - rfine;
//            fdelta_cal = fine_cal - rfine_cal;
//         }
//
//         delta = cdelta - fdelta;
//         if (delta)
//            hDeltaRaw[ch]->Fill(delta);
//
//         delta_cal = cdelta - fdelta_cal;
//         // test of initial artefacts: JAM
//         //	  if(cdelta!=0 && fdelta_cal==0)
//         //	  	  {
//         //		  	  cout <<"At Eventcount:"<<fEventCount << endl;
//         //		  	  cout <<"found delta_cal["<< ch<<"]="<<delta_cal<<", cdelta="<<cdelta<<", fdelta_cal="<<fdelta_cal << endl;
//         //		  	  cout <<"fine:"<<fine<<", rfine:"<<rfine<<", coarse:"<<coarse<<", rcoarse"<<rcoarse << endl;
//         //		  	  cout <<"binindex:"<<binindex<<", rbinindex:" <<rbinindex<< endl;
//         //	  	  }
//         if (delta_cal)
//            hDeltaCal[ch]->Fill(delta_cal);
//
//         hCorrelFine[ch]->Fill(binindex, rbinindex);
//
//      } // for i
//   } // channels second loop

#else
  EPRINT("*** ProcessTimeTest() is disabled, please recompile with HAD_USE_CAHIT\n");
#endif
}

void
THadaqUnpackProc::EvaluateTDCData(UShort_t board, UShort_t tdc)
{
#ifdef HAD_USE_CAHIT

  if (fPar->generalCalibration == fPar->continuousCalibration)
    GO4_STOP_ANALYSIS_MESSAGE(
        "Parameter Error: General Calibration and Continious Calibration can't be activated together!");

  // first process all raw channels:
  for (int ch = 0; ch < HAD_TIME_CHANNELS; ++ch)
    {

      Int_t mcp = fPar->imageMCP[board][tdc][ch];
      Int_t row = fPar->imageRow[board][tdc][ch];
      Int_t col = fPar->imageCol[board][tdc][ch];
      if(mcp>=HAD_TIME_NUMMCP)
         {
         GO4_STOP_ANALYSIS_MESSAGE(
                "mcp index %d out of range, max is %d!",mcp, HAD_TIME_NUMMCP-1);

         }

      for (unsigned int i = 0; i < fOutEvent->fLeadingCoarseTime[board][tdc][ch].size(); i++)
        {
          Double_t val = fOutEvent->fLeadingCoarseTime[board][tdc][ch].at(
              i) * HAD_TIME_COARSEUNIT;
          hLeadingCoarseAll[board][tdc]->Fill(val); // for condition display
	       hImagingMCP[mcp]->Fill(row, col); // filling up the imaging histogram
	       //if(ch>0)printf ("++++ fill leading mcp%d, row %d, col %d from trb:%d, tdc:%d, ch%d \n",mcp,row,col,board,tdc,ch);
	       //printf(" **** array content of parameter 0x%x is: mcp %d row %d col %d \n",
//	                          fPar,
//	                          imageMCP[board][tdc][ch],
//	                          imageRow[board][tdc][ch],
//	                          imageCol[board][tdc][ch]);
	       if (cLeadingCoarseTimeGate[board][tdc]->Test(val))
            {
              hLeadingCoarse[board][tdc][ch]->Fill(val);
              hLeadingCoarseAllChans[board][tdc]->Fill(ch, val);
              hChannelCount[board][tdc]->Fill(ch);
            }
        }
      for (unsigned int i = 0; i < fOutEvent->fLeadingFineTime[board][tdc][ch].size();
          i++)
        {
	  Double_t val = fOutEvent->fLeadingFineTime[board][tdc][ch].at(i);
          hLeadingFineAll[board][tdc]->Fill(val); // for condition display
          if (cLeadingFineTimeGate[board][tdc]->Test(val))
            {
              hLeadingFine[board][tdc][ch]->Fill(val);
              hLeadingFineBuffer[board][tdc][ch]->Fill(val);
              hLeadingFineAllChans[board][tdc]->Fill(ch, val);
            }
        }

      for (unsigned int i = 0;
          i < fOutEvent->fTrailingCoarseTime[board][tdc][ch].size(); i++)
        {
          Double_t val = fOutEvent->fTrailingCoarseTime[board][tdc][ch].at(
              i) * HAD_TIME_COARSEUNIT;
          hTrailingCoarseAll[board][tdc]->Fill(val); // for condition display
	       hImagingMCP[mcp]->Fill(row, col); // filling up the imaging histogram
	       printf ("++++ fill trailing mcp%d, row %d, col %d from trb:%d, tdc:%d, ch%d \n",mcp,row,col,board,tdc,ch);

          if (cTrailingCoarseTimeGate[board][tdc]->Test(val))
            {
              hTrailingCoarse[board][tdc][ch]->Fill(val);
              hTrailingCoarseAllChans[board][tdc]->Fill(ch, val);
              hChannelCount[board][tdc]->Fill(ch);
            }
        }
      for (unsigned int i = 0; i < fOutEvent->fTrailingFineTime[board][tdc][ch].size();
          i++)
        {
          Double_t val = fOutEvent->fTrailingFineTime[board][tdc][ch].at(i);
          hTrailingFineAll[board][tdc]->Fill(val); // for condition display
          if (cTrailingFineTimeGate[board][tdc]->Test(val))
            {
              hTrailingFine[board][tdc][ch]->Fill(val);
              hTrailingFineBuffer[board][tdc][ch]->Fill(val);
              hTrailingFineAllChans[board][tdc]->Fill(ch, val);
            }
        }

      // update calibration if necessary:
      if (fEventCount && ((fEventCount % fPar->calibrationPeriod) == 0))
        {
          DoCalibration(board,tdc, ch, fPar->generalCalibration,
              fPar->continuousCalibration);
        }

    } // channels first loop

  if (fPar->continuousCalibration && !fbHasCalibration)
    return;

  // after calibration, we process channels again using calibrated times:

  for (int ch = 0; ch < HAD_TIME_CHANNELS; ++ch)
    {
      // calculate delta ts with setup
      Short_t ref = fPar->deltaChannels[board][tdc][ch];
      if (ref < 0)
        continue; // skip not configured delta channel
      Short_t rboard = fPar->deltaTRB[board][tdc][ch];
      if (rboard < 0 || rboard > HAD_TIME_NUMBOARDS)
        continue; // skip not configured delta channel
      Short_t rtdc = fPar->deltaTDC[board][tdc][ch];
      if (rtdc < 0 || rtdc > HAD_TIME_NUMTDC)
        continue; // skip not configured delta channel
      // first leading edges:

      for (unsigned int i = 0;
          i < fOutEvent->fLeadingCoarseTime[board][tdc][ch].size(); i++)
        {
          Double_t delta = 0, cdelta = 0, fdelta = 0, coarse = 0, fine = 0,
              rcoarse = 0, rfine = 0;
          Double_t delta_cal = 0, fdelta_cal = 0, fine_cal = 0, rfine_cal = 0;
          UShort_t binindex = 0, rbinindex = 0;

          coarse = fOutEvent->fLeadingCoarseTime[board][tdc][ch].at(
              i) * HAD_TIME_COARSEUNIT;
          if (i < fOutEvent->fLeadingCoarseTime[rboard][rtdc][ref].size())
            {
              hTriggerCount[board][tdc]->Fill(ch + 3); //account the matching pairs found
              rcoarse = fOutEvent->fLeadingCoarseTime[rboard][rtdc][ref].at(
                  i) * HAD_TIME_COARSEUNIT;
              cdelta = coarse - rcoarse;
            }

          if (i < fOutEvent->fLeadingFineTime[board][tdc][ch].size())
            {
              binindex = fOutEvent->fLeadingFineTime[board][tdc][ch].at(i);
              fine = hUnCalBinTime[board][tdc][ch]->GetBinContent(binindex + 1);
              fine_cal = hCalBinTime[board][tdc][ch]->GetBinContent(binindex + 1);
            }

          if (i < fOutEvent->fLeadingFineTime[rboard][rtdc][ref].size())
            {
              rbinindex = fOutEvent->fLeadingFineTime[rboard][rtdc][ref].at(i);
              rfine = hUnCalBinTime[rboard][rtdc][ref]->GetBinContent(rbinindex + 1);
              rfine_cal = hCalBinTime[rboard][rtdc][ref]->GetBinContent(rbinindex + 1);
              fdelta = fine - rfine;
              fdelta_cal = fine_cal - rfine_cal;
            }

          delta = cdelta - fdelta;
          if (delta)
            hLeadingDeltaRaw[board][tdc][ch]->Fill(delta);

          delta_cal = cdelta - fdelta_cal;
          if (delta_cal)
            hLeadingDeltaCal[board][tdc][ch]->Fill(delta_cal);

          hLeadingCorrelFine[board][tdc][ch]->Fill(binindex, rbinindex);

        } // i leading
          // trailing edges:
      for (unsigned int i = 0;
          i < fOutEvent->fTrailingCoarseTime[board][tdc][ch].size(); i++)
        {
          Double_t delta = 0, cdelta = 0, fdelta = 0, coarse = 0, fine = 0,
              rcoarse = 0, rfine = 0;
          Double_t delta_cal = 0, fdelta_cal = 0, fine_cal = 0, rfine_cal = 0;
          UShort_t binindex = 0, rbinindex = 0;

          coarse = fOutEvent->fTrailingCoarseTime[board][tdc][ch].at(
              i) * HAD_TIME_COARSEUNIT;
          if (i < fOutEvent->fTrailingCoarseTime[rboard][rtdc][ref].size())
            {
              hTriggerCount[board][tdc]->Fill(ch + 3); //account the matching pairs found
              rcoarse = fOutEvent->fTrailingCoarseTime[rboard][rtdc][ref].at(
                  i) * HAD_TIME_COARSEUNIT;
              cdelta = coarse - rcoarse;
            }

          if (i < fOutEvent->fTrailingFineTime[board][tdc][ch].size())
            {
              binindex = fOutEvent->fTrailingFineTime[board][tdc][ch].at(i);
              fine = hUnCalBinTime[board][tdc][ch]->GetBinContent(binindex + 1);
              fine_cal = hCalBinTime[board][tdc][ch]->GetBinContent(binindex + 1);
            }

          if (i < fOutEvent->fTrailingFineTime[rboard][rtdc][ref].size())
            {
              rbinindex = fOutEvent->fTrailingFineTime[rboard][rtdc][ref].at(i);
              rfine = hUnCalBinTime[rboard][rtdc][ref]->GetBinContent(rbinindex + 1);
              rfine_cal = hCalBinTime[rboard][rtdc][ref]->GetBinContent(rbinindex + 1);
              fdelta = fine - rfine;
              fdelta_cal = fine_cal - rfine_cal;
            }

          delta = cdelta - fdelta;
          if (delta)
            hTrailingDeltaRaw[board][tdc][ch]->Fill(delta);

          delta_cal = cdelta - fdelta_cal;
          if (delta_cal)
            hTrailingDeltaCal[board][tdc][ch]->Fill(delta_cal);

          hTrailingCorrelFine[board][tdc][ch]->Fill(binindex, rbinindex);

        } // for i trailing

    } // channels second loop

#else
  EPRINT("*** ProcessTimeTest() is disabled, please recompile with HAD_USE_CAHIT\n");
#endif

}

#ifdef HAD_USE_CAHIT

void THadaqUnpackProc::DoCalibration(UShort_t board, UShort_t tdc, UShort_t ch, Bool_t generalcal,
    Bool_t contcal)
{
  TH1* hfinecal = 0;
  //cout <<"DO CALIBRATION" << endl;

  if (generalcal)
    {
      hfinecal = (TH1*) hLeadingFine[board][tdc][ch]->Clone("calbuf");
      hfinecal->Add(hTrailingFine[board][tdc][ch]); // we use sum of leading and trailing times for calib.
    }
  else if (contcal)
    {
      hfinecal = (TH1*) hLeadingFineBuffer[board][tdc][ch]->Clone("calbuf");
      hfinecal->Add(hTrailingFineBuffer[board][tdc][ch]);
    }
  else
    {
      return;
    }

  for (int fb = 0; fb < HAD_TIME_FINEBINS; ++fb)
    {

      Int_t sum = hfinecal->GetEntries();
      Double_t binwidth = 1;
      Double_t binhits = hfinecal->GetBinContent(fb + 1);
      if (sum)
        binwidth = (HAD_TIME_COARSEUNIT * binhits) / sum;
      hCalcBinWidth[board][tdc][ch]->SetBinContent(fb + 1, binwidth);

      Double_t calbintime = 1;
      if (fb == 0)
        calbintime = hCalcBinWidth[board][tdc][ch]->GetBinContent(fb + 1) / 2;
      else
        calbintime = hCalBinTime[board][tdc][ch]->GetBinContent(fb)
            + (hCalcBinWidth[board][tdc][ch]->GetBinContent(fb)
                + hCalcBinWidth[board][tdc][ch]->GetBinContent(fb + 1)) / 2;

      hCalBinTime[board][tdc][ch]->SetBinContent(fb + 1, calbintime);

      Double_t uncalbintime = 1;
      if (fb == 0)
        uncalbintime = hCalcBinWidth[board][tdc][ch]->GetBinContent(fb + 1);
      else
        uncalbintime = hUnCalBinTime[board][tdc][ch]->GetBinContent(fb)
            + hCalcBinWidth[board][tdc][ch]->GetBinContent(fb + 1);

      hUnCalBinTime[board][tdc][ch]->SetBinContent(fb + 1, uncalbintime);
    }
  if (contcal)
    {
      hLeadingFineBuffer[board][tdc][ch]->Reset();
      hTrailingFineBuffer[board][tdc][ch]->Reset();
    }
  delete hfinecal; // since we did clone originals here!
  fbHasCalibration = kTRUE;
}

#endif

void
THadaqUnpackProc::ProcessTDC(Hadaq_Subevent* hadsubevent)
{
#ifdef HAD_USE_TDC
  //int dtype= (hadsubevent->GetDecoding() >> 8 ) & 0xFF ;
  int align=hadsubevent->Alignment();
  int ixlen=hadsubevent->GetSize()-sizeof(Hadaq_Subevent);
  ixlen=ixlen/(align);// 1,2,4 bytes
  for(int ix=0; ix<ixlen;++ix)
    {
      Int_t data=hadsubevent->Data(ix);
      Short_t id= (data >> 28) & 0xF;
      Short_t tdc=(data >> 24) & 0xF;
      // following for leading/trailing edges:
      Short_t ch=(data >> 19) & 0x1F;
      Int_t time= data & 0x3FFFF;
      // following for headers:
      Short_t evid=(data >> 12) & 0xFFF;
      Short_t bunchid=data & 0xFFF;
      EPRINT("*** ---  tdc id:0x%x\n",id);
      switch(id)
        {
          case 0x0:
          EPRINT("***  --- Group Header, tdc:0x%x, evid:0x%x, bunchid:0x%x\n",tdc,evid,bunchid);
          break;
          case 0x1:
          EPRINT("***  --- Group Trailer, tdc:0x%x, evid:0x%x, bunchid:0x%x\n",tdc,evid,bunchid);
          break;

          case 0x2:
          EPRINT("***  --- TDC Header, tdc:0x%x, evid:0x%x, bunchid:0x%x\n",tdc,evid,bunchid);
          break;

          case 0x3:
          EPRINT("***  --- TDC Trailer, tdc:0x%x, evid:0x%x, word count:0x%x\n",tdc,evid,bunchid);
          break;

          case 0x4:
          EPRINT("***  --- Leading edge, tdc:0x%x, ch:0x%x, time:0x%x\n",tdc,ch,time);
          if((tdc<HAD_TDC_NUMTDC) && (ch < HAD_TDC_CHANNELS))
          fOutEvent->fTDC_Leading[tdc][ch].push_back(time);
          else
          GO4_STOP_ANALYSIS_MESSAGE("Leading edge Out of range error for tdc:0x%x, channel:0x%x !!",tdc,ch);

          break;

          case 0x5:
          EPRINT("***  --- Trailing edge, tdc:0x%x, ch:0x%x, time:0x%x\n",tdc,ch,time);
          if((tdc<HAD_TDC_NUMTDC) && (ch < HAD_TDC_CHANNELS))
          fOutEvent->fTDC_Trailing[tdc][ch].push_back(time);
          else
          GO4_STOP_ANALYSIS_MESSAGE("Trailing edge Out of range error for tdc:0x%x, channel:0x%x !!",tdc,ch);

          break;

          default:
          break;

        };
    } // for ix

  if(fPar->fillRaw)
    {
      // now loop again over data and fill histograms:

      for(int tdc=0; tdc<HAD_TDC_NUMTDC;++tdc)
        {
          // get time of channel 31 as reference for each tdc:
          Int_t leadreftime=0, trailreftime=0;
          for (unsigned int i=0; i<fOutEvent->fTDC_Leading[tdc][HAD_TDC_REFCHANNEL].size(); i++)
            {
              leadreftime=fOutEvent->fTDC_Leading[tdc][HAD_TDC_REFCHANNEL].at(i);
            }
          for (unsigned int i=0; i<fOutEvent->fTDC_Trailing[tdc][HAD_TDC_REFCHANNEL].size(); i++)
            {
              trailreftime=fOutEvent->fTDC_Trailing[tdc][HAD_TDC_REFCHANNEL].at(i);
            }

          // now process all channels:
          for(int ch=0; ch<HAD_TDC_CHANNELS;++ch)
            {
              std::vector<Int_t> leadingdts;
              for (unsigned int i=0; i<fOutEvent->fTDC_Leading[tdc][ch].size(); i++)
                {
                  Int_t tm=fOutEvent->fTDC_Leading[tdc][ch].at(i);
                  hLeadingChannelHits[tdc]->Fill(ch);
                  Int_t deltaT=tm-leadreftime;
                  hLeadingDeltaTAll->Fill(deltaT);
                  if(!cLeadingDtRange->Test(deltaT)) continue; // suppress times outside validity gate

                  leadingdts.push_back(deltaT);
                  hLeadingDeltaT[tdc][ch]->Fill(deltaT);
                  hLeadingDtAllChans->Fill(tdc*ch,deltaT);

                }
              for (unsigned int i=0; i<fOutEvent->fTDC_Trailing[tdc][ch].size(); i++)
                {
                  Int_t tm=fOutEvent->fTDC_Trailing[tdc][ch].at(i);
                  hTrailingChannelHits[tdc]->Fill(ch);
                  Int_t deltaT=tm-leadreftime; // all refer to trailing edge channel 31
                  hTrailingDeltaTAll->Fill(deltaT);
                  if(!cTrailingDtRange->Test(deltaT)) continue;// suppress times outside validity gate

                  hTrailingDeltaT[tdc][ch]->Fill(deltaT);
                  hTrailingDtAllChans->Fill(tdc*ch,deltaT);
                  Int_t leadminustrail=0;
                  if(i<leadingdts.size())
                  leadminustrail=deltaT - leadingdts.at(i);
                  hDeltaDtAllChans->Fill(tdc*ch,leadminustrail);

                }
            } // for ch
        } // for tdc
    } // if fillRaw
#else
  EPRINT("*** ProcessTDC() is disabled, please recompile with HAD_USE_TDC\n");

#endif

}

void
THadaqUnpackProc::DumpData(Hadaq_Subevent* hadsubevent)
{
  if (!fPar->printEvent)
    return;
  int dtype = (hadsubevent->GetDecoding() >> 8) & 0xFF;
  int align = hadsubevent->Alignment();
  EPRINT("*** ---  data align:%d, data type:%d\n", align, dtype);
  int ixlen = hadsubevent->GetSize() - sizeof(Hadaq_Subevent);
  ixlen = ixlen / (align); // 1,2,4 bytes
  for (int ix = 0; ix < ixlen; ++ix)
    {
      EPRINT("*** %08x\t", hadsubevent->Data(ix));
      if (((ix + 1) % 8) == 0)
        EPRINT(("\n"));
    }
  EPRINT("\n");
}

