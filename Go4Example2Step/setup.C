//---------------------------------------------
// Go4 2 step analysis
//---------------------------------------------

void setup(const char* name)
{
// name is either:
// MBS node, or
// LMD file name without .lmd or
// LML file starting with @ without .lml
// output file names are are built like:
// idir/name.lmd
// @idir/name.lml
// odir/name_AS
// odir/name_unpacked
// odir/name_analyzed

// steering parameters to modify:
  TString inpath("/GSI/lea"); // input directory
  //TString inpath("."); // input directory
  TString outpath(".");       // output directory

  TString unpackSource("random"); // file, stream, transport, random
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
  TString autosaveOverWrite("yes");
  Int_t autosaveinterval=0; // after n seconds, 0 = at termination of event loop
// End of settings

// programmer part ----------------------------------------
  TString source(name);
  Text_t asout[512]; // autosave file
  Text_t unpinp[512];// unpack input
  Text_t unpout[512];// unpack output
  Text_t anlinp[512];// analysis input
  Text_t anlout[512];// analysis output
  Text_t nameonly[512];// name only
  strcpy(unpinp,name);
  strcpy(nameonly,source.Strip(TString::kLeading,'@').Data());
  sprintf(asout,"%s/%s_AS.root",outpath.Data(),nameonly);
  sprintf(unpout,"%s/%s_unpacked.root",outpath.Data(),nameonly);
  sprintf(anlout,"%s/%s_analyzed.root",outpath.Data(),nameonly);
  sprintf(anlinp,"%s/%s_unpacked.root",outpath.Data(),nameonly);
  // First step
  TGo4AnalysisStep * step1 = go4->GetAnalysisStep("Unpack");
  step1->SetProcessEnabled(kFALSE);
  if(unpackProcess.BeginsWith("y")){
     step1->SetProcessEnabled(kTRUE);
     if(unpackSource.BeginsWith("f")){
    	 if(source.BeginsWith("@"))
       	      sprintf(unpinp,"@%s/%s.lml",inpath.Data(),nameonly);
    	 else sprintf(unpinp,"%s/%s.lmd",inpath.Data(),name);
    	  step1->SetEventSource(new TGo4MbsFileParameter(unpinp));
     }
     else if(unpackSource.BeginsWith("t"))
     step1->SetEventSource(new TGo4MbsTransportParameter(name));
     else if(unpackSource.BeginsWith("s"))
     step1->SetEventSource(new TGo4MbsStreamParameter(name));
     else if(unpackSource.BeginsWith("r"))
     step1->SetEventSource(new TGo4MbsRandomParameter(name));
  }
  TGo4FileStoreParameter * f1 = new TGo4FileStoreParameter(unpout,SplitLevel,BufferSize,Compression);
  f1->SetOverwriteMode(unpackOverWrite.BeginsWith("y"));
  step1->SetEventStore(f1);
  step1->SetStoreEnabled(unpackStore.BeginsWith("y"));
  step1->SetSourceEnabled(kTRUE);
  step1->SetErrorStopEnabled(kTRUE);
  // Second step
  TGo4AnalysisStep * step2 = go4->GetAnalysisStep("Analysis");
  step2->SetProcessEnabled(analysisProcess.BeginsWith("y"));
  TGo4FileSourceParameter * f2 = new TGo4FileSourceParameter(anlinp);
  step2->SetEventSource(f2);
  step2->SetSourceEnabled(unpackProcess.BeginsWith("n"));
  f1 = new TGo4FileStoreParameter(anlout,SplitLevel,BufferSize,Compression);
  f1->SetOverwriteMode(analysisOverWrite.BeginsWith("y"));
  step2->SetEventStore(f1);
  step2->SetStoreEnabled(analysisStore.BeginsWith("y"));
  step2->SetErrorStopEnabled(kTRUE);

  // Autosafe
  go4->SetAutoSaveFile(asout,autosaveOverWrite.BeginsWith("y"));   // optional
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
  printf("       Unpack:      %s\n",unpackProcess.Data());
  if(unpackProcess.BeginsWith("y")){
	  printf("         Input:     %s %s \n",unpackSource.Data(),unpinp);
	  printf("         Store:     %s, file %s\n",unpackStore.Data(),unpout);
	  printf("         OverWrite: %s\n",unpackOverWrite.Data());
  }
  printf("       Analysis:    %s\n",analysisProcess.Data());
  if(analysisProcess.BeginsWith("y")){
  if(unpackProcess.BeginsWith("n"))
  printf("         Source:    yes, file %s\n",anlinp);
  else
  printf("         Source:    from Unpack (no, file %s) \n",anlinp);
  printf("         Store:     %s, file %s\n",analysisStore.Data(),anlout);
  printf("         OverWrite: %s\n",analysisOverWrite.Data());
  }
  printf("       Splitlevel:  %d\n",SplitLevel);
  printf("       Buffersize:  %d\n",BufferSize);
  printf("       Compression: %d\n",Compression);

}
