// written by macro saveparam.C at Thu May 31 11:27:30 2012
void set_HadaqPar()
{
#ifndef __GO4ANAMACRO__
   cout << "Macro set_HadaqPar can execute only in analysis" << endl;
   return;
#endif
   THadaqParam* param0 = (THadaqParam*) go4->GetParameter("HadaqPar");

   if (param0==0) {
      TGo4Log::Error("Could not find parameter HadaqPar of class THadaqParam");
      return;
   }

   TGo4Log::Info("Set parameter HadaqPar from saveparam.C");

   param0->fillRaw = kTRUE;
   param0->printEvent = kFALSE;
   param0->generalCalibration = kTRUE;
   param0->continuousCalibration = kFALSE;
   param0->calibrationPeriod = 10000;

   // default settings in loop:
   for (int b = 0; b < 9; ++b)
        {
       if(!THadaqUnpackEvent::AssertTRB(b)) continue;
     for(int t=0; t<4; ++t)
        {
           for(int i=0; i<16; ++i)
           {
               param0->deltaTDC[b][t][i]=t;// by default, channels refer to self tdc
               param0->deltaTRB[b][t][i]=b;// by default, channels refer to self trb
               param0->imageMCP[b][t][i]=t+1;
               param0->imageRow[b][t][i]=i+1;
               param0->imageCol[b][t][i]=i+1;

                   if((i%2)==0)
                     param0->deltaChannels[b][t][i]=i+1;
                   else
                     param0->deltaChannels[b][t][i]=-1;
           }
       }
     }



   // indices: [trb][tdc][channel]
   param0->deltaTDC[0][0][0]=0;
   param0->deltaTRB[0][0][0]=0;
   param0->deltaChannels[0][0][0]=7;
   param0->imageMCP[0][0][0]=0;
   param0->imageRow[0][0][0]=1;
   param0->imageCol[0][0][0]=1;

   // add here mapping of MCP and delta channels:

}
