void setup(Text_t* AutoSaveFile,Text_t* UnpackedFile,Text_t* CalibratedFile,Text_t* AnalyzedFile)
{
  TGo4AnalysisStep * step;
  TGo4FileStoreParameter * f1;
  TGo4FileSourceParameter * f2;

  // steering parameters to modify:
  TString unpackProcess("yes");
  TString unpackStore("no");
  TString unpackOverWrite("yes");

  TString caliProcess("no");
  TString caliStore("no");
  TString caliOverWrite("yes");

  TString analysisProcess("no");
  TString analysisStore("no");
  TString analysisOverWrite("yes");

  TString autosave("yes");
  Int_t autosaveinterval=0; // after n seconds, 0 = at termination of event loop

  // First step
  step = go4->GetAnalysisStep("Unpack");
  step->SetProcessEnabled(unpackProcess.BeginsWith("y"));
  f1 = new TGo4FileStoreParameter(UnpackedFile);
  f1->SetOverwriteMode(unpackOverWrite.BeginsWith("y"));
  step->SetEventStore(f1);
  step->SetStoreEnabled(unpackStore.BeginsWith("y"));
  step->SetSourceEnabled(kTRUE);
  step->SetErrorStopEnabled(kTRUE);

  // Second step
  step = go4->GetAnalysisStep("Calibration");
  step->SetProcessEnabled(caliProcess.BeginsWith("y"));
  // if unpack is disabled, get input from file
  if(unpackProcess.BeginsWith("n")){
    f2 = new TGo4FileSourceParameter(UnpackedFile);
    step->SetEventSource(f2);
  }
  f1 = new TGo4FileStoreParameter(CalibratedFile);
  f1->SetOverwriteMode(caliOverWrite.BeginsWith("y"));
  step->SetEventStore(f1);
  step->SetStoreEnabled(caliStore.BeginsWith("y"));
  step->SetSourceEnabled(kTRUE);
  step->SetErrorStopEnabled(kTRUE);

  // Third step
  step = go4->GetAnalysisStep("Analysis");
  step->SetProcessEnabled(analysisProcess.BeginsWith("y"));
  // if cali is disabled, get input from file
  if(caliProcess.BeginsWith("n")){
    f2 = new TGo4FileSourceParameter(CalibratedFile);
    step->SetEventSource(f2);
  }
  f1 = new TGo4FileStoreParameter(AnalyzedFile);
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
  if(autosaveinterval==0)
  printf("       autosave:          %s once file %s\n",autosave.Data(),AutoSaveFile);
  else
  printf("       autosave:          %s every %ds file %s\n",autosave.Data(),autosaveinterval,AutoSaveFile);
  printf("       unpackProcess:     %s\n",unpackProcess.Data());
  printf("       unpackStore:       %s file %s\n",unpackStore.Data(),UnpackedFile);
  printf("       unpackOverWrite:   %s\n",unpackOverWrite.Data());

  printf("       caliProcess:   %s\n",caliProcess.Data());
  if(unpackProcess.BeginsWith("n"))
  printf("       caliSource:    yes file %s\n",UnpackedFile);
  else
  printf("       caliSource:    yes from unpack\n");
  printf("       caliStore:     %s file %s\n",caliStore.Data(),CalibratedFile);
  printf("       caliOverWrite: %s\n",caliOverWrite.Data());

  printf("       analysisProcess:   %s\n",analysisProcess.Data());
  if(caliProcess.BeginsWith("n"))
  printf("       analysisSource:    yes file %s\n",CalibratedFile);
  else
  printf("       analysisSource:    yes from cali\n");
  printf("       analysisStore:     %s file %s\n",analysisStore.Data(),AnalyzedFile);
  printf("       analysisOverWrite: %s\n",analysisOverWrite.Data());

}
