void evserv(Text_t *name)
{
  TGo4AnalysisStep * step;
  step = go4->GetAnalysisStep("Analysis");
  step->SetEventSource(new TGo4MbsEventServerParameter(name));
  printf("**** evserv.C: Create MBS event server input %s\n",name);
}
