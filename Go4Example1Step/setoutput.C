void setoutput(bool enable = true)
{
  TGo4AnalysisStep *step = go4->GetAnalysisStep("Analysis");
  if(step != 0) step->SetStoreEnabled(enable);  // enable/disable output
           else cout << "Analysis step not found!" << endl;
}
