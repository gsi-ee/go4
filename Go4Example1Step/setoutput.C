
void setoutput(Int_t true)
{
  TGo4AnalysisStep * step;
  step = go4->GetAnalysisStep("Analysis");
  if(step != 0)
    {
      if(true)step->SetStoreEnabled(kTRUE);  // enable output
      else    step->SetStoreEnabled(kFALSE); // disable output
    }
  else cout "Analysis step not found!" << endl;
}
