void trans(Text_t *name)
{
  TGo4AnalysisStep * step;
  step = go4->GetAnalysisStep("Analysis");
  step->SetEventSource(new TGo4MbsTransportParameter(name));
  printf("**** trans.C: Create MBS transport input %s\n",name);
}
