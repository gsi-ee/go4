//---------------------------------------------
// Go4 Tasca analysis
// Author: Hans G. Essel
//         H.Essel@gsi.de
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

void setup(Text_t* InputPath, Text_t* OutputPath,
		Text_t* AutoSaveFile,
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
  TString checkStore("no");
  TString checkOverWrite("yes");

  TString analysisProcess("yes");
  TString analysisStore("no");
  TString analysisOverWrite("yes");

  UInt_t SplitLevel=1;
  UInt_t BufferSize=100000;
  UInt_t Compression=3;

  TString autosave("no");
  Int_t autosaveinterval=0; // after n seconds, 0 = at termination of event loop
//--------------------------------------------

  Text_t asout[512];
  Text_t unpout[512];
  Text_t calinp[512];
  Text_t calout[512];
  Text_t chkinp[512];
  Text_t chkout[512];
  Text_t anlinp[512];
  Text_t anlout[512];
  strcpy(asout,OutputPath);
  strcpy(unpout,OutputPath);
  strcpy(calinp,InputPath);
  strcpy(calout,OutputPath);
  strcpy(chkinp,InputPath);
  strcpy(chkout,OutputPath);
  strcpy(anlinp,InputPath);
  strcpy(anlout,OutputPath);

  strcat(asout,AutoSaveFile);
  strcat(unpout,UnpackedFile);
  strcat(calinp,UnpackedFile);
  strcat(calout,CalibratedFile);
  strcat(chkinp,CalibratedFile);
  strcat(chkout,CheckedFile);
  strcat(anlinp,CheckedFile);
  strcat(anlout,AnalyzedFile);
  // First step
  TGo4AnalysisStep * step1 = go4->GetAnalysisStep("Unpacker");
  step1->SetProcessEnabled(unpackProcess.BeginsWith("y"));
  f1 = new TGo4FileStoreParameter(unpout,SplitLevel,BufferSize,Compression);
  f1->SetOverwriteMode(unpackOverWrite.BeginsWith("y"));
  step1->SetEventStore(f1);
  step1->SetStoreEnabled(unpackStore.BeginsWith("y"));
  step1->SetSourceEnabled(kTRUE);
  step1->SetErrorStopEnabled(kTRUE);

  // Second step
  step = go4->GetAnalysisStep("Calibrator");
  step->SetProcessEnabled(caliProcess.BeginsWith("y"));
  // if unpack is disabled, get input from file
  if(unpackProcess.BeginsWith("n")){
    f2 = new TGo4FileSourceParameter(calinp);
    step->SetEventSource(f2);
  }
  f1 = new TGo4FileStoreParameter(calout,SplitLevel,BufferSize,Compression);
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
    f2 = new TGo4FileSourceParameter(chkinp);
    step->SetEventSource(f2);
  }
  f1 = new TGo4FileStoreParameter(chkout,SplitLevel,BufferSize,Compression);
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
    f2 = new TGo4FileSourceParameter(anlinp);
    step->SetEventSource(f2);
  }
  f1 = new TGo4FileStoreParameter(anlout,SplitLevel,BufferSize,Compression);
  f1->SetOverwriteMode(analysisOverWrite.BeginsWith("y"));
  step->SetEventStore(f1);
  step->SetStoreEnabled(analysisStore.BeginsWith("y"));
  step->SetSourceEnabled(kTRUE);
  step->SetErrorStopEnabled(kTRUE);

  // Autosafe
  go4->SetAutoSaveFile(asout);   // optional
  go4->SetAutoSaveInterval(autosaveinterval);
  go4->SetAutoSave(autosave.BeginsWith("y"));    // optional

  printf("Tasca> setup.C: Setup analysis\n");
  if(go4->IsAutoSaveOn()){
	  if(autosaveinterval==0)
		  printf("       Autosave:    %s once file %s\n",autosave.Data(),asout);
	  else
		  printf("       autosave:    %s every %ds file %s\n",autosave.Data(),autosaveinterval,asout);
  } else
  printf("       Autosave:    off\n");
  printf("       Unpacker:    %s\n",unpackProcess.Data());
  if(unpackProcess.BeginsWith("y")){
  printf("         Store:     %s file %s\n",unpackStore.Data(),unpout);
  printf("         OverWrite: %s\n",unpackOverWrite.Data());
  }
  printf("       Calibrator:  %s\n",caliProcess.Data());
  if(caliProcess.BeginsWith("y")){
  if(unpackProcess.BeginsWith("n"))
  printf("         Source:    yes file %s\n",calinp);
  else
  printf("         Source:    yes from Unpacker\n");
  printf("         Store:     %s file %s\n",caliStore.Data(),calout);
  printf("         OverWrite: %s\n",caliOverWrite.Data());
  }
  printf("       Checker:     %s\n",checkProcess.Data());
  if(checkProcess.BeginsWith("y")){
  if(caliProcess.BeginsWith("n"))
  printf("         Source:    yes file %s\n",chkinp);
  else
  printf("         Source:    yes from Calibrator\n");
  printf("         Store:     %s file %s\n",checkStore.Data(),chkout);
  printf("         OverWrite: %s\n",checkOverWrite.Data());
  }
  printf("       Analyzer:    %s\n",analysisProcess.Data());
  if(analysisProcess.BeginsWith("y")){
  if(checkProcess.BeginsWith("n"))
  printf("         Source:    yes file %s\n",anlinp);
  else
  printf("         Source:    yes from Checker\n");
  printf("         Store:     %s file %s\n",analysisStore.Data(),anlout);
  printf("         OverWrite: %s\n",analysisOverWrite.Data());
  }
  printf("       Splitlevel:  %d\n",SplitLevel);
  printf("       Buffersize:  %d\n",BufferSize);
  printf("       Compression: %d\n",Compression);

}
