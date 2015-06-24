// $Id$
//-----------------------------------------------------------------------
//       The GSI Online Offline Object Oriented (Go4) Project
//         Experiment Data Processing at EE department, GSI
//-----------------------------------------------------------------------
// Copyright (C) 2000- GSI Helmholtzzentrum für Schwerionenforschung GmbH
//                     Planckstr. 1, 64291 Darmstadt, Germany
// Contact:            http://go4.gsi.de
//-----------------------------------------------------------------------
// This software can be used under the license agreements as stated
// in Go4License.txt file which is part of the distribution.
//-----------------------------------------------------------------------

#include "TGo4ServerProxy.h"

#include <string>
#include <map>

#include "TRegexp.h"
#include "TSystem.h"
#include "Riostream.h"

#include "TGo4Log.h"
#include "TGo4Slot.h"
#include "TGo4ServerTask.h"

// ********************************************************************

class TGo4Prefs {
   protected:
      std::map<std::string, std::string> fPars;
   public:
      TGo4Prefs(const char* hostname)
      {
         SetPar("hostname", hostname);
         #ifdef WIN32
         SetPar("os", "win32");
         #else
         SetPar("os", "linux");
         #endif
      }

      void AddFile(const char* fname, bool errorout = false)
      {
         std::ifstream f(fname);
         if(!f) {
            if (errorout) TGo4Log::Debug("ERROR: Preferences file %s not existing",fname);
            return;
         }

         std::string hostname = GetPar("hostname");

         char formatstring[4096];

         while (!f.eof()) {

           f.getline(formatstring, sizeof(formatstring), '\n' );
           if ((f.gcount()==0) || (strlen(formatstring)==0)) continue;

           const char* sbuf = formatstring;

           while (*sbuf != 0) {
              if (*sbuf==' ') { sbuf++; continue; }
              if (*sbuf=='#') break;

              const char* separ = strchr(sbuf, ':');
              if (separ==0) break;

              std::string name(sbuf, separ-sbuf);

              size_t pos = name.find('=');
              if (pos!=name.npos) {
                 std::string subname(name, 0, pos);
                 std::string mask(name, pos+1);

                 if ((subname.length()==0) || (mask.length()==0)) break;

                 const char* subvalue = GetPar(subname.c_str());

                 if (subvalue==0) break;

                 // if mask didnot match, ignore string
                 // check mask with regular expression
                 TRegexp re(mask.c_str(), kTRUE);
                 Int_t len(0);
                 if (re.Index(subvalue, &len)!=0) break;
                 if (len != (Int_t) strlen(subvalue)) break;

                 // take rest of buffer for analysis
                 sbuf = separ+1;
                 continue;
              }

              if (!HasPar(name.c_str()))
                 SetPar(name.c_str(), separ+1);

              break;
           }
         }
      }

      /** Return true if more than two parameter exists, hostname and os is default*/
      bool IsOk() const { return fPars.size()>2; }

      void SetPar(const char* name, const char* value, bool force = true)
      {
         std::string dname = Form("%s%s%s", "%", name, "%");
         if (force || (fPars.find(dname) == fPars.end()))
            fPars[dname] = value;
      }

      const char* GetPar(const char* name)
      {
         std::string dname = Form("%s%s%s", "%", name, "%");
         if (fPars.find(dname) == fPars.end()) return 0;
         return fPars[dname].c_str();
      }

      bool HasPar(const char* name)
      {
         return GetPar(name)!=0;
      }

      void ReplacePars(std::string& str)
      {
         size_t pos;
         bool isany = false;
         int cnt = 0;

         do {
            isany = false;
            std::map<std::string,std::string>::iterator iter = fPars.begin();
            while (iter != fPars.end()) {
               pos = 0;
               while ((pos = str.find(iter->first, pos)) != str.npos) {
                  str.replace(pos, iter->first.length(), iter->second);
                  isany = true;
               }
               iter++;
            }
            if (cnt++>100000) {
               std::cerr << "Syntax error in go4.prefs files - endless recursion" << std::endl;
               std::cerr << "Program aborted, please fix an error" << std::endl;
               exit(-1);
            }
         } while (isany);
      }

      void ReplaceEnvPars(std::string& str)
      {
         size_t pos1, pos2;

         while ((pos1 = str.find("${")) != str.npos) {

            pos2 = str.find("}");

            if ((pos1>pos2) || (pos2==str.npos)) {
               TGo4Log::Debug("ERROR: Wrong variable parenthesis %s",str.c_str());
               return;
            }

            std::string var(str, pos1+2, pos2-pos1-2);

            str.erase(pos1, pos2-pos1+1);

            const char* value = gSystem->Getenv(var.c_str());
            if (value!=0) str.insert(pos1, value);
         }
      }

      /** return option value with parameters replaced */
      std::string GetOpt(const char* prefix)
      {
         const char* opt = GetPar(prefix);
         if (opt==0) return std::string("");
         std::string res = opt;
         ReplacePars(res);
         ReplaceEnvPars(res);
         return res;
      }

};

Bool_t TGo4ServerProxy::GetLaunchString(TString& launchcmd,
                                        TString& killcmd,
                                        Int_t serverkind,
                                        Int_t shellkind,
                                        Int_t konsole,
                                        const char* name,
                                        const char* remotehost,
                                        const char* remotedir,
                                        const char* remoteexe,
                                        Int_t guiport,
                                        Int_t exe_kind,
                                        const char* exeargs)
{
   const char* serverhost = gSystem->HostName();
   const char* sdisplay   = gSystem->Getenv("DISPLAY");
   const char* go4sys     = TGo4Log::GO4SYS();
   const char* rootsys    = gSystem->Getenv("ROOTSYS");
   const char* path       = gSystem->Getenv("PATH");
   const char* ldpath     = gSystem->Getenv("LD_LIBRARY_PATH");

   if ((name==0) || (strlen(name)==0)) name = "UserAnalysis";
   if ((serverhost==0) || (strlen(serverhost)==0)) serverhost = "localhost";

   if (gSystem->Getenv("GO4OLDLAUNCH")==0) {
      TGo4Prefs prefs(remotehost);

      const char* shellname = "exec";
      if (shellkind==1) shellname = "rsh"; else
      if (shellkind==2) shellname = konsole==1 ? "ssh" : "sshX";
      prefs.SetPar("shellkind", shellname, false);
      prefs.SetPar("exekind", Form("%d", exe_kind), false);
      prefs.SetPar("clientkind", serverkind>0 ? "Go4Server" : "Go4Client", false);

      prefs.AddFile("go4.prefs", false);
      prefs.AddFile(TGo4Log::subGO4SYS("etc/go4.prefs"), true);
      if (!prefs.IsOk()) {
         std::cout << "Cannot find prefs file" << std::endl;
         return kFALSE;
      }

      prefs.SetPar("guihost", serverhost, false);
      //if (!server)
      prefs.SetPar("guiport", Form("%d", guiport));
      prefs.SetPar("guigo4sys", go4sys, false);
      prefs.SetPar("analysisname", name, false);
      prefs.SetPar("workdir", remotedir, false);
      prefs.SetPar(exe_kind==0 ? "exename" : "libname", remoteexe, false);


      if ((exe_kind==1) && (exeargs!=0) && (strlen(exeargs)>0))
         prefs.SetPar("userargs", Form("%s", exeargs), false);
      else
         prefs.SetPar("userargs", "", false);


      const char* termname = "qtwindow";
      if (konsole==2) termname = "xterm"; else
      if (konsole==3) termname = "konsole";

      // no need to change into local directory with exec and qtwinow - it happens automatically
      if ((shellkind==0) && (konsole==1))
         prefs.SetPar("cd_workdir", "");

      std::string executable;
      bool is_exe = prefs.GetOpt("exekind") != "1";
      if (is_exe) {
         if (prefs.GetOpt("exename").empty())
            executable = prefs.GetOpt("analysis_default_exe");
         else
            executable = prefs.GetOpt("analysis_exe");
      } else {
         if (prefs.GetOpt("libname").empty())
            executable = prefs.GetOpt("analysis_default_lib");
         else
            executable = prefs.GetOpt("analysis_lib");
      }
      prefs.SetPar("analysis", executable.c_str());

      if (!is_exe) prefs.SetPar("killexename", "go4analysis", false); else {
         #ifdef WIN32
         char symbol = '\\';
         #else
         char symbol = '/';
         #endif
         const char* runname = strrchr(remoteexe, symbol);
         prefs.SetPar("killexename", runname ? runname+1 : remoteexe, false);
      }

      std::string initcmd = prefs.GetOpt(shellkind==0 ? "execinitcmd" : "shellinitcmd");
      prefs.SetPar("initcmd", initcmd.c_str());

      std::string progcmd = prefs.GetOpt((serverkind>0) ? ((serverkind==2) ? "httpcmd" : "servercmd") : "clientcmd");
      prefs.SetPar("progcmd", progcmd.c_str());

      std::string hostcmd = prefs.GetOpt(termname);
      prefs.SetPar("hostcmd", hostcmd.c_str());

      std::string cmd = prefs.GetOpt(shellname);
      std::cout << "cmd: " << cmd << std::endl;
      launchcmd = cmd.c_str();

      std::string dkill = prefs.GetOpt("kill");
      prefs.SetPar("hostcmd", dkill.c_str());
      cmd = prefs.GetOpt(shellname);
      std::cout << "killcmd: " << cmd << std::endl;
      killcmd = cmd.c_str();

      return kTRUE;
   }

   if ((go4sys==0) || (strlen(go4sys)==0)) return kFALSE;

   TString filename = TGo4Log::subGO4SYS(TGo4ServerTask::Get_fgcLAUNCHPREFSFILE());

   std::ifstream launchprefs(filename.Data());
   if(!launchprefs) {
      TGo4Log::Debug("Master -- ERROR: Preferences file %s not existing, could not launch client ",
                  filename.Data());
      return kFALSE;
   }

   char formatstring[1000];

   if ((konsole<1) || (konsole>3)) konsole = 1;
   Int_t num = konsole;
   if (serverkind>0) num+=3;

   for(int n=0;n<num;n++)
     launchprefs.getline(formatstring, 1000, '\n' );

   const char* sh_com = "";
   const char* sh_host = remotehost;
   TString serverdisplay = "";

   switch (shellkind) {
      case 1:
         sh_com = "rsh -n";
         serverdisplay = "-display ";
         serverdisplay += sdisplay;
         break;
      case 2:
         sh_com = (konsole == 0) ? "ssh -x " : "ssh -X ";
         break;
      default:
         sh_com = "";
         sh_host = "";
         break;
   }

   killcmd = "killall ";
   killcmd += remoteexe;

   if((shellkind>0) && (strcmp(remotehost, gSystem->HostName())!=0) && (strcmp(remotehost,"localhost")!=0)) {
       TString precmd = sh_com;
       precmd += " ";
       precmd += remotehost;
       precmd += " ";
       killcmd.Prepend(precmd);
   }

   launchcmd = "";

   switch(konsole) {
     case 2: { // xterm
       launchcmd.Form(formatstring,
         sh_com, sh_host, serverdisplay.Data(), name, remotehost, go4sys, go4sys, rootsys,
         path, ldpath, remotedir, remoteexe, name, serverhost, guiport, remotehost);
       break;
     }

     case 3: { // konsole
       launchcmd.Form(formatstring,
         sh_com, sh_host, name, go4sys, go4sys, rootsys,
         path, ldpath, remotedir, remoteexe, name, serverhost, guiport, remotehost);
       break;
     }

     default: {  // Qt

      launchcmd.Form(formatstring,
         sh_com, sh_host, go4sys, go4sys, rootsys,
         path, ldpath, remotedir, remoteexe, name, serverhost, guiport, remotehost);
      break;
     }
   }

   return kTRUE;
}

// ==============================================================================

TGo4ServerProxy::TGo4ServerProxy() :
   TGo4Proxy(),
   fxParentSlot(0),
   fbAnalysisReady(kFALSE),
   fbAnalysisSettingsReady(kFALSE),
   fAnalysisLaunched(0),
   fNodeName(),
   fInfoStr()
{
}

TGo4ServerProxy::~TGo4ServerProxy()
{
}

TGo4Slot* TGo4ServerProxy::SettingsSlot()
{
   return fxParentSlot==0 ? 0 : fxParentSlot->FindChild("Settings");
}

TGo4Slot* TGo4ServerProxy::RatemeterSlot()
{
   return fxParentSlot==0 ? 0 : fxParentSlot->FindChild("Ratemeter");
}

TGo4Slot* TGo4ServerProxy::LoginfoSlot()
{
   return fxParentSlot==0 ? 0 : fxParentSlot->FindChild("Loginfo");
}

TGo4Slot* TGo4ServerProxy::DebugOutputSlot()
{
   return fxParentSlot==0 ? 0 : fxParentSlot->FindChild("Debugoutput");
}

const char* TGo4ServerProxy::GetContainedObjectInfo()
{
   fInfoStr = "";
   if (!IsConnected()) fInfoStr = "Not connected"; else
   if (IsViewer()) fInfoStr = "Observer"; else
   if (IsController()) fInfoStr = "Controller"; else
   if (IsAdministrator()) fInfoStr = "Administrator";
   return fInfoStr.Data();
}

