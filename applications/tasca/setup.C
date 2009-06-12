//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

void setup(Text_t* AutoSaveFile,
		Text_t* UnpackedFile,
		Text_t* CalibratedFile,
		Text_t* CheckedFile,
		Text_t* AnalyzedFile)
{
  TGo4AnalysisStep * step;
  TGo4FileStoreParameter * f1;
  TGo4FileSourceParameter * f2;

//--------------------------------------------
// steering parameters to modify:
  TString unpackProcess("no");
  TString unpackStore("no");
  TString unpackOverWrite("yes");

  TString caliProcess("no");
  TString caliStore("no");
  TString caliOverWrite("yes");

  TString checkProcess("no");
  TString checkStore("yes");
  TString checkOverWrite("yes");

  TString analysisProcess("yes");
  TString analysisStore("no");
  TString analysisOverWrite("yes");

  UInt_t SplitLevel=1;
  UInt_t BufferSize=100000;
  UInt_t Compression=3;

  TString autosave("yes");
  Int_t autosaveinterval=0; // after n seconds, 0 = at termination of event loop
//--------------------------------------------

  // First step
  step = go4->GetAnalysisStep("Unpacker");
  step->SetProcessEnabled(unpackProcess.BeginsWith("y"));
  f1 = new TGo4FileStoreParameter(UnpackedFile,SplitLevel,BufferSize,Compression);
  f1->SetOverwriteMode(unpackOverWrite.BeginsWith("y"));
  step->SetEventStore(f1);
  step->SetStoreEnabled(unpackStore.BeginsWith("y"));
  step->SetSourceEnabled(kTRUE);
  step->SetErrorStopEnabled(kTRUE);

  // Second step
  step = go4->GetAnalysisStep("Calibrator");
  step->SetProcessEnabled(caliProcess.BeginsWith("y"));
  // if unpack is disabled, get input from file
  if(unpackProcess.BeginsWith("n")){
    f2 = new TGo4FileSourceParameter(UnpackedFile);
    step->SetEventSource(f2);
  }
  f1 = new TGo4FileStoreParameter(CalibratedFile,SplitLevel,BufferSize,Compression);
  f1->SetOverwriteMode(caliOverWrite.BeginsWith("y"));
  step->SetEventStore(f1);
  step->SetStoreEnabled(caliStore.BeginsWith("y"));
  step->SetSourceEnabled(kTRUE);
  step->SetErrorStopEnabled(kTRUE);

  // Third step
  step = go4->GetAnalysisStep("Checker");
  step->SetProcessEnabled(checkProcess.BeginsWith("y"));
  // if cali is disabled, get input from file
  if(caliProcess.BeginsWith("n")){
    f2 = new TGo4FileSourceParameter(CalibratedFile);
    step->SetEventSource(f2);
  }
  f1 = new TGo4FileStoreParameter(CheckedFile,SplitLevel,BufferSize,Compression);
  f1->SetOverwriteMode(checkOverWrite.BeginsWith("y"));
  step->SetEventStore(f1);
  step->SetStoreEnabled(checkStore.BeginsWith("y"));
  step->SetSourceEnabled(kTRUE);
  step->SetErrorStopEnabled(kTRUE);

  // Fourth step
  step = go4->GetAnalysisStep("Analyzer");
  step->SetProcessEnabled(analysisProcess.BeginsWith("y"));
  // if check is disabled, get input from file
  if(checkProcess.BeginsWith("n")){
    f2 = new TGo4FileSourceParameter(CheckedFile);
    step->SetEventSource(f2);
  }
  f1 = new TGo4FileStoreParameter(AnalyzedFile,SplitLevel,BufferSize,Compression);
  f1->SetOverwriteMode(analysisOverWrite.BeginsWith("y"));
  step->SetEventStore(f1);
  step->SetStoreEnabled(analysisStore.BeginsWith("y"));
  step->SetSourceEnabled(kTRUE);
  step->SetErrorStopEnabled(kTRUE);

  // Autosafe
  go4->SetAutoSaveFile(AutoSaveFile);   // optional
  go4->SetAutoSaveInterval(autosaveinterval);
  go4->SetAutoSave(autosave.BeginsWith("y"));    // optional

  printf("Tasca> setup.C: Setup analysis\n");
  if(go4->IsAutoSaveOn()){
	  if(autosaveinterval==0)
		  printf("       Autosave:    %s once file %s\n",autosave.Data(),AutoSaveFile);
	  else
		  printf("       autosave:    %s every %ds file %s\n",autosave.Data(),autosaveinterval,AutoSaveFile);
  } else
  printf("       Autosave:    off\n");
  printf("       Unpacker:    %s\n",unpackProcess.Data());
  if(unpackProcess.BeginsWith("y")){
  printf("         Store:     %s file %s\n",unpackStore.Data(),UnpackedFile);
  printf("         OverWrite: %s\n",unpackOverWrite.Data());
  }
  printf("       Calibrator:  %s\n",caliProcess.Data());
  if(caliProcess.BeginsWith("y")){
  if(unpackProcess.BeginsWith("n"))
  printf("         Source:    yes file %s\n",UnpackedFile);
  else
  printf("         Source:    yes from unpack\n");
  printf("         Store:     %s file %s\n",caliStore.Data(),CalibratedFile);
  printf("         OverWrite: %s\n",caliOverWrite.Data());
  }
  printf("       Checker:     %s\n",checkProcess.Data());
  if(checkProcess.BeginsWith("y")){
  if(caliProcess.BeginsWith("n"))
  printf("         Source:    yes file %s\n",CalibratedFile);
  else
  printf("         Source:    yes from unpack\n");
  printf("         Store:     %s file %s\n",checkStore.Data(),CheckedFile);
  printf("         OverWrite: %s\n",checkOverWrite.Data());
  }
  printf("       Analyzer:    %s\n",analysisProcess.Data());
  if(analysisProcess.BeginsWith("y")){
  if(checkProcess.BeginsWith("n"))
  printf("         Source:    yes file %s\n",CheckedFile);
  else
  printf("         Source:    yes from cali\n");
  printf("         Store:     %s file %s\n",analysisStore.Data(),AnalyzedFile);
  printf("         OverWrite: %s\n",analysisOverWrite.Data());
  }
  printf("       Splitlevel:  %d\n",SplitLevel);
  printf("       Buffersize:  %d\n",BufferSize);
  printf("       Compression: %d\n",Compression);

}
