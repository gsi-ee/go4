//---------------------------------------------
// Go4 2 step analysis
//---------------------------------------------

void setup(Text_t* name)
{
// steering parameters to modify:
  TString inpath("."); // input directory
  TString outpath(".");// output directory

  TString unpackProcess("yes");
  TString unpackStore("no");
  TString unpackOverWrite("yes");

  TString analysisProcess("yes");
  TString analysisStore("no");
  TString analysisOverWrite("yes");

  UInt_t SplitLevel=1;
  UInt_t BufferSize=100000;
  UInt_t Compression=3;

  TString autosave("no");
  Int_t autosaveinterval=0; // after n seconds, 0 = at termination of event loop
// End of settings

//--------------------------------------------
  Text_t asout[512]; // autosave file
  Text_t unpinp[512];// unpack input
  Text_t unpout[512];// unpack output
  Text_t anlinp[512];// analysis input
  Text_t anlout[512];// analysis output
  sprintf(asout,"%s/%s_AS.root",outpath.Data(),name);
  sprintf(unpinp,"%s/%s.lmd",inpath.Data(),name);
  sprintf(unpout,"%s/%s_unpacked.root",outpath.Data(),name);
  sprintf(anlout,"%s/%s_analyzed.root",outpath.Data(),name);
  sprintf(anlinp,"%s/%s_unpacked.root",outpath.Data(),name);
  // First step
  TGo4AnalysisStep * step1 = go4->GetAnalysisStep("Unpack");
  step1->SetProcessEnabled(unpackProcess.BeginsWith("y"));
  TGo4FileStoreParameter * f1 = new TGo4FileStoreParameter(unpout,SplitLevel,BufferSize,Compression);
  f1->SetOverwriteMode(unpackOverWrite.BeginsWith("y"));
  step1->SetEventStore(f1);
  step1->SetStoreEnabled(unpackStore.BeginsWith("y"));
  step1->SetSourceEnabled(kTRUE);
  step1->SetErrorStopEnabled(kTRUE);
  // Second step
  TGo4AnalysisStep * step2 = go4->GetAnalysisStep("Analysis");
  step2->SetProcessEnabled(analysisProcess.BeginsWith("y"));
  // if unpack is disabled, get input from file
  // otherwise from output event of unpack.
  step2->SetSourceEnabled(kFALSE);
  if(unpackProcess.BeginsWith("n")){
	TGo4FileSourceParameter * f2 = new TGo4FileSourceParameter(anlinp);
    step2->SetEventSource(f2);
    step2->SetSourceEnabled(kTRUE);
  }
  f1 = new TGo4FileStoreParameter(anlout,SplitLevel,BufferSize,Compression);
  f1->SetOverwriteMode(analysisOverWrite.BeginsWith("y"));
  step2->SetEventStore(f1);
  step2->SetStoreEnabled(analysisStore.BeginsWith("y"));
  step2->SetErrorStopEnabled(kTRUE);

  // Autosafe
  go4->SetAutoSaveFile(asout);   // optional
  go4->SetAutoSaveInterval(autosaveinterval);
  go4->SetAutoSave(autosave.BeginsWith("y"));    // optional

  printf("Example2Step> setup.C: Setup analysis\n");
  if(go4->IsAutoSaveOn()){
	  if(autosaveinterval==0)
		  printf("       Autosave:    %s once file %s\n",autosave.Data(),asout);
	  else
		  printf("       autosave:    %s every %ds file %s\n",autosave.Data(),autosaveinterval,asout);
  } else
  printf("       Autosave:    off\n");
  printf("         Unpack:    %s\n",unpackProcess.Data());
  if(unpackProcess.BeginsWith("y")){
  printf("         Store:     %s file %s\n",unpackStore.Data(),unpout);
  printf("         OverWrite: %s\n",unpackOverWrite.Data());
  }
  printf("       Analysis:    %s\n",analysisProcess.Data());
  if(analysisProcess.BeginsWith("y")){
  if(unpackProcess.BeginsWith("n"))
  printf("         Source:    yes file %s\n",anlinp);
  else
  printf("         Source:    yes from Unpack\n");
  printf("         Store:     %s file %s\n",analysisStore.Data(),anlout);
  printf("         OverWrite: %s\n",analysisOverWrite.Data());
  }
  printf("       Splitlevel:  %d\n",SplitLevel);
  printf("       Buffersize:  %d\n",BufferSize);
  printf("       Compression: %d\n",Compression);

}
