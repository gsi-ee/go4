
#include "THadaqParam.h"
#if __GO4BUILDVERSION__ > 40502
#include "go4iostream.h"
#else
#include "Riostream.h"
#endif




// mcp mapping doc:
//1. Die Gerade nummerte Kanäle sind immer die Rising Edge und die Ungerade
//nummerte Kanäle sind immer die Falling Edge von die vorherige Kanal,
//z.B. tdc1 ch02 12(rising), tdc1 ch03 12(falling).
//2. Die erste Zahl im MCP Spalten zeigt die Reihe des MCPs und die zweite Zahl
//ist die Spalte Nummer des MCPs.
//3. MCP Pixel 11, 13, 15, 17 sind nicht verbunden.
//4. TRB1-MCP1, TRB2-MCP2, TRB3-MCP3, TRB4-MCP4 sind verbunden, wie in dem
//Exemplar gezeigt wird.
//
//         TRB1 - MCP1
//
//TDC1  MCP   TDC2    MCP TDC3    MCP     TDC5    MCP
//---   ---   ---     --- ---     ---     ---     ---
//ch00  Ref   ch00    Ref ch00    Ref     ch00    Ref
//ch01     ch01        ch01            ch01
//ch02  12 ch02    14  ch02    16      ch02    18
//ch03     ch03        ch03            ch03
//ch04  22 ch04    24  ch04    26      ch04    28
//ch05     ch05        ch05            ch05
//ch06  32 ch06    34  ch06    36      ch06    38
//ch07     ch07        ch07            ch07
//ch08  42 ch08    44  ch08    46      ch08    48
//ch09     ch09        ch09            ch09
//ch10  52 ch10    54  ch10    56      ch10    58
//ch11     ch11        ch11            ch11
//ch12  62 ch12    64  ch12    66      ch12    68
//ch13     ch13        ch13            ch13
//ch14  72 ch14    74  ch14    76      ch14    78
//ch15     ch15        ch15            ch15
//ch16  82 ch16    84  ch16    86      ch16    88
//ch17     ch17        ch17            ch17
//ch18  81 ch18    83  ch18    85      ch18    87
//ch19     ch19        ch19            ch19
//ch20  71 ch20    73  ch20    75      ch20    77
//ch21     ch21        ch21            ch21
//ch22  61 ch22    63  ch22    65      ch22    67
//ch23     ch23        ch23            ch23
//ch24  51 ch24    53  ch24    55      ch24    57
//ch25     ch25        ch25            ch25
//ch26  41 ch26    43  ch26    45      ch26    47
//ch27     ch27        ch27            ch27
//ch28  31 ch28    33  ch28    35      ch28    37
//ch29     ch29        ch29            ch29
//ch30  21 ch30    23  ch30    25      ch30    27
//ch31        ch31        ch31            ch31





THadaqParam::THadaqParam(const char* name) :
   TGo4Parameter(name)
{
   UInt_t trbids[HAD_TIME_NUMMCP]={3,5,8,9}; // assign mcps 0..3 to trbs


   fillRaw = kTRUE;
   printEvent=kFALSE;

#ifdef HAD_USE_CAHIT
   printDebugGate=kFALSE;
   // default for time difference: use neighbouring channels
  for (int b = 0; b < HAD_TIME_NUMBOARDS; ++b) {
      if (!THadaqUnpackEvent::AssertTRB(b))
         continue;
      for (int t = 0; t < HAD_TIME_NUMTDC; ++t) {
         for (int i = 0; i < HAD_TIME_CHANNELS; ++i) {
            deltaTDC[b][t][i] = t; // by default, channels refer to self tdc
            deltaTRB[b][t][i] = b; // by default, channels refer to self trb
	     //if (i%2 == 0)
	      deltaChannels[b][t][i] = 0; // reference channel is 0
         }
      }
   }
// may modify default setup for testing here:
  //   deltaChannels[0][0][0]=7; // first test setup
//   deltaTDC[0][0][0]=1;  // refer to tdc1, channel 7
//   deltaChannels[0][1][7]=0;
//   deltaTDC[0][1][7]=0; // back reference
   generalCalibration=kTRUE;
   continuousCalibration=kFALSE;
   calibrationPeriod=HAD_TIME_CALIBFREQ;
   calibrationEntries=HAD_CALIBENTRIES;

// NOTE: mappings in ctor are overridden by setup script!
   // here mcp mappings (JAM 1-June2012):
    for (Short_t mcp = 0; mcp < HAD_TIME_NUMMCP; ++mcp) {
    Int_t trb = trbids[mcp];
    if (!THadaqUnpackEvent::AssertTRB(trb))
           continue;
    for (Int_t tdc = 0; tdc < HAD_TIME_NUMTDC; ++tdc) {
        Short_t row = 8;
        Short_t deltarow = +1;
        Short_t col = 2 * (4 - tdc);
        for (Int_t ch = 0; ch < HAD_TIME_CHANNELS; ++ch) {
   	 imageMCP[trb][tdc][ch] = mcp;
   		 imageRow[trb][tdc][ch] = 0; // initialize everything to 0
          	 imageCol[trb][tdc][ch] = 0;
         if ((ch==0) || (ch % 2) != 0)
   		continue;
          imageRow[trb][tdc][ch] = row;
          imageCol[trb][tdc][ch] = col;
          printf(" **** Mapped trb %d, tdc %d, ch %d to mcp %d row %d col %d \n",trb,tdc,ch,mcp,row,col);
          // add here mapping of trailing channels to same pixels:
   		Int_t chn=ch+1;
   	    if(chn<HAD_TIME_CHANNELS)
   		{
          		imageRow[trb][tdc][chn] = row;
          		imageCol[trb][tdc][chn] = col;
          		printf(" **** Mapped trb %d, tdc %d, ch %d to mcp %d row %d col %d \n",trb,tdc,chn,mcp,row,col);
   		}
               row -= deltarow;
          if (row < 1) {
             row = 1;
             deltarow = -1;
             col -= 1;
          }
       }
    }
   } // mcp


// for (Short_t mcp = 0; mcp < HAD_TIME_NUMMCP; ++mcp) {
   // Int_t trb = trbids[mcp];
   //   if (!THadaqUnpackEvent::AssertTRB(trb))
   //     continue;
   //   for (Int_t tdc = 0; tdc < HAD_TIME_NUMTDC; ++tdc) {
   //     Short_t row = 1;
   //     Short_t col = 2 * (tdc + 1);
   //     for (Int_t ch = 0; ch < HAD_TIME_CHANNELS; ++ch) {
   //       imageMCP[trb][tdc][ch] = mcp;
   //       imageRow[trb][tdc][ch] = 0; // initialize everything to 0
   //       imageCol[trb][tdc][ch] = 0;
   //       if ((ch==0) || (ch % 2) != 0)
   //	 continue;
   //       imageRow[trb][tdc][ch] = row;
   //       imageCol[trb][tdc][ch] = col;
   //       printf(" **** Mapped trb %d, tdc %d, ch %d to mcp %d row %d col %d \n",trb,tdc,ch,mcp,row,col);
   //       // add here mapping of trailing channels to same pixels:
   //       Int_t chn=ch+1;
   //       if(chn<HAD_TIME_CHANNELS)
   //	 {
   //	   imageRow[trb][tdc][chn] = row;
   //	   imageCol[trb][tdc][chn] = col;
   //	   printf(" **** Mapped trb %d, tdc %d, ch %d to mcp %d row %d col %d \n",trb,tdc,chn,mcp,row,col);
   //	 }
   //
   //       if ((ch % 4) == 0){
   //	 row += 1;
   //	 col += 1;
   //       }
   //       else if((ch % 2) == 0)
   //	 col -= 1;
   //     }
   //   }
   // } // mcp
 
// cout << endl;

#endif
}

Bool_t THadaqParam::UpdateFrom(TGo4Parameter *pp)
{
  THadaqParam* from = dynamic_cast<THadaqParam*> (pp);
   if (from==0) {
      cout << "Wrong parameter object: " << pp->ClassName() << endl;
      return kFALSE;
   }
   cout << "**** THadaqParam:UpdateFrom ... " << endl;
    fillRaw=from->fillRaw;
    printEvent=from->printEvent;

#ifdef HAD_USE_CAHIT
     printDebugGate=from->printDebugGate;
    generalCalibration=from->generalCalibration;
    continuousCalibration=from->continuousCalibration;
    calibrationPeriod=from->calibrationPeriod;
    calibrationEntries=from->calibrationEntries;
// exclude this for the moment, is not handled correctly by go4 gui JAM
// we therefore need to implement UpdateFrom and suppress changing of these arrays from gui
//    for (int trb = 0; trb < HAD_TIME_NUMBOARDS; ++trb)
//    {
//      if(!THadaqUnpackEvent::AssertTRB(trb)) continue;
//      for (int tdc = 0; tdc < HAD_TIME_NUMTDC; ++tdc)
//        {
//          for (int ch = 0; ch < HAD_TIME_CHANNELS; ++ch)
//            {
//
//              deltaChannels[trb][tdc][ch] = from->deltaChannels[trb][tdc][ch];
//              deltaTDC[trb][tdc][ch] = from->deltaTDC[trb][tdc][ch];
//              deltaTRB[trb][tdc][ch] = from->deltaTRB[trb][tdc][ch];
//              imageMCP[trb][tdc][ch] = from->imageMCP[trb][tdc][ch];
//              imageRow[trb][tdc][ch] = from->imageRow[trb][tdc][ch];
//              imageCol[trb][tdc][ch] = from->imageCol[trb][tdc][ch];
//
//            }
//        }
//    }
#endif


return kTRUE;

     }

