
void file(Text_t *name)
{
  TGo4AnalysisStep * step;
  step = go4->GetAnalysisStep("Analysis");
  step->SetEventSource(new TGo4MbsFileParameter(name));
  printf("**** file.C: Create LMD file input %s\n",name);
}
