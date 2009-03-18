void setup(Text_t* asf)
{
  TGo4AnalysisStep * step;
  step = go4->GetAnalysisStep("Analysis");
  step->SetSourceEnabled(kTRUE);
  step->SetStoreEnabled(kFALSE);  // en-disable output
  step->SetProcessEnabled(kTRUE);
  step->SetErrorStopEnabled(kTRUE);
  go4->SetAutoSaveFile(asf);   // optional
  go4->SetAutoSaveInterval(0); // after n seconds , 0 = at termination of event loop
  go4->SetAutoSave(kFALSE);    // optional
  printf("**** setup.C: Setup analysis\n");
}
