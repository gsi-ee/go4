void setup(Text_t* AutoSaveFile,Text_t* UnpackedFile,Text_t* AnalyzedFile)
{
  TGo4AnalysisStep * step;
  TGo4FileStoreParameter * f1;
  TGo4FileSourceParameter * f2;

  // steering parameters to modify:
  TString unpackProcess("yes");
  TString unpackStore("yes");
  TString unpackOverWrite("yes");

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
  step = go4->GetAnalysisStep("Analysis");
  step->SetProcessEnabled(analysisProcess.BeginsWith("y"));
  // if unpack is disabled, get input from file
  if(unpackProcess.BeginsWith("n")){
    f2 = new TGo4FileSourceParameter(UnpackedFile);
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
  printf("       analysisProcess:   %s\n",analysisProcess.Data());
  if(unpackProcess.BeginsWith("n"))
  printf("       analysisSource:    yes file %s\n",UnpackedFile);
  else
  printf("       analysisSource:    yes from unpack\n");
  printf("       analysisStore:     %s file %s\n",analysisStore.Data(),AnalyzedFile);
  printf("       analysisOverWrite: %s\n",analysisOverWrite.Data());
}
