void random(Text_t *name)
{
  TGo4AnalysisStep * step;
  TGo4MbsRandomParameter * par;
  par = new TGo4MbsRandomParameter(name);
  par->SetTimeout(100);
  step = go4->GetAnalysisStep("Analysis");
  step->SetEventSource(par);
  printf("**** stream.C: Create MBS random generator %s\n",name);
}
