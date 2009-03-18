void stream(Text_t *name)
{
  TGo4AnalysisStep * step;
  step = go4->GetAnalysisStep("Analysis");
  step->SetEventSource(new TGo4MbsStreamParameter(name));
  printf("**** stream.C: Create MBS stream input %s\n",name);
}
