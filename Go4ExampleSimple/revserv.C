void revserv(Text_t *name, Int_t port)
{
  TGo4AnalysisStep * step;
  TGo4RevServParameter *par;
  par=new TGo4RevServParameter(name);
  par->SetPort(port);
  step = go4->GetAnalysisStep("Analysis");
  step->SetEventSource(par);
  printf("**** revserv.C: Create remote event server input %s port %d\n",name,port);
}
