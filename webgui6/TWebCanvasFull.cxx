#include "TWebCanvasFull.h"

#include "TWebMenuItem.h"
#include "TCanvas.h"
#include "TROOT.h"
#include "TPave.h"
#include "TFrame.h"
#include "TBufferJSON.h"
#include "TError.h"

#include <sstream>

TWebCanvasFull::TWebCanvasFull(TCanvas *c, const char *name, Int_t x, Int_t y, UInt_t width, UInt_t height) :
   TWebCanvas(c,name,x,y,width,height)
{
   printf("CREATING FULL CANVAS\n");
}


Bool_t TWebCanvasFull::ProcessData(unsigned connid, const std::string &arg)
{
   if (TWebCanvas::ProcessData(connid, arg))
      return kTRUE;

   if (arg.compare(0, 8, "GETMENU:") == 0) {

      TObject *obj = FindPrimitive(arg.c_str() + 8);
      if (!obj)
         obj = Canvas();

      TWebMenuItems items(arg.c_str() + 8);
      items.PopulateObjectMenu(obj, obj->IsA());
      std::string buf = "MENU:";
      buf.append(TBufferJSON::ToJSON(&items, 103).Data());

      AddToSendQueue(connid, buf);

   } else if (arg.compare(0, 8, "PRIMIT6:") == 0) {

      if (IsFirstConn(connid) && !IsReadOnly()) { // only first connection can modify object

         auto opt = TBufferJSON::FromJSON<TWebObjectOptions>(arg.c_str() + 8);

         if (opt) {
            TPad *modpad = ProcessObjectOptions(*opt, nullptr);

            // indicate that pad was modified
            if (modpad)
               modpad->Modified();
         }
      }
   } else if (arg.compare(0, 11, "PADCLICKED:") == 0) {

      auto click = TBufferJSON::FromJSON<TWebPadClick>(arg.c_str() + 11);

      if (click && IsFirstConn(connid) && !IsReadOnly()) {

         TPad *pad = dynamic_cast<TPad*> (FindPrimitive(click->padid.c_str()));
         if (pad && (pad != gPad)) {
            Info("ProcessData", "Activate pad %s", pad->GetName());
            gPad = pad;
            Canvas()->SetClickSelectedPad(pad);
            if (fActivePadChangedSignal) fActivePadChangedSignal(pad);
         }

         if (!click->objid.empty()) {
            TObject *selobj = FindPrimitive(click->objid.c_str());
            Canvas()->SetClickSelected(selobj);
            if (pad && selobj && fObjSelectSignal) fObjSelectSignal(pad, selobj);
         }

         if ((click->x >= 0) && (click->y >= 0)) {
            if (click->dbl && fPadDblClickedSignal)
               fPadDblClickedSignal(pad, click->x, click->y);
            else if (fPadClickedSignal)
               fPadClickedSignal(pad, click->x, click->y);
         }
      }
   }

   return kTRUE;
}

Bool_t TWebCanvasFull::DecodePadOptions(const char * msg)
{
   if (!msg || !*msg)
      return kFALSE;

   auto arr = TBufferJSON::FromJSON<std::vector<TWebPadOptions>>(msg);

   if (!arr)
      return kFALSE;

   for (unsigned n = 0; n < arr->size(); ++n) {
      auto &r = arr->at(n);
      TPad *pad = dynamic_cast<TPad *>(FindPrimitive(r.snapid.c_str()));

      if (!pad)
         continue;

      if (pad == Canvas()) AssignStatusBits(r.bits);

      if (r.active && (pad != gPad)) gPad = pad;

      pad->SetTicks(r.tickx, r.ticky);
      pad->SetGrid(r.gridx, r.gridy);
      if (r.logx != pad->GetLogx())
         pad->SetLogx(r.logx);
      if (r.logy != pad->GetLogy())
         pad->SetLogy(r.logy);
      if (r.logz != pad->GetLogz())
         pad->SetLogz(r.logz);

      pad->SetLeftMargin(r.mleft);
      pad->SetRightMargin(r.mright);
      pad->SetTopMargin(r.mtop);
      pad->SetBottomMargin(r.mbottom);

      if (r.ranges) {

         Double_t ux1_, ux2_, uy1_, uy2_, px1_, px2_, py1_, py2_;

         pad->GetRange(px1_, py1_, px2_, py2_);
         pad->GetRangeAxis(ux1_, uy1_, ux2_, uy2_);

         bool same_range = (r.ux1 == ux1_) && (r.ux2 == ux2_) && (r.uy1 == uy1_) && (r.uy2 == uy2_) &&
                           (r.px1 == px1_) && (r.px2 == px2_) && (r.py1 == py1_) && (r.py2 == py2_);

         if (!same_range) {
            pad->Range(r.px1, r.py1, r.px2, r.py2);
            pad->RangeAxis(r.ux1, r.uy1, r.ux2, r.uy2);

            if (gDebug > 1)
               Info("DecodeAllRanges", "Change ranges for pad %s", pad->GetName());
         }
      }

      for (auto &item : r.primitives)
         ProcessObjectOptions(item, pad);

      // without special objects no need for explicit update of the pad
      if (fHasSpecials)
         pad->Modified(kTRUE);
   }

   if (fUpdatedSignal) fUpdatedSignal(); // invoke signal

   return kTRUE;
}

//////////////////////////////////////////////////////////////////////////////////////////
/// Process data for single primitive
/// Returns object pad if object was modified

TPad *TWebCanvasFull::ProcessObjectOptions(TWebObjectOptions &item, TPad *pad)
{
   TObjLink *lnk = nullptr;
   TPad *objpad = nullptr;
   TObject *obj = FindPrimitive(item.snapid.c_str(), pad, &lnk, &objpad);

   if (item.fcust.compare("exec") == 0) {
      auto pos = item.opt.find("(");
      if (obj && (pos != std::string::npos) && obj->IsA()->GetMethodAllAny(item.opt.substr(0,pos).c_str())) {
         std::stringstream exec;
         exec << "((" << obj->ClassName() << " *) " << std::hex << std::showbase
                      << (size_t)obj << ")->" << item.opt << ";";
         Info("ProcessObjectOptions", "Obj %s Execute %s", obj->GetName(), exec.str().c_str());
         gROOT->ProcessLine(exec.str().c_str());
      } else {
         Error("ProcessObjectOptions", "Fail to execute %s for object %p %s", item.opt.c_str(), obj, obj ? obj->ClassName() : "---");
         objpad = nullptr;
      }
      return objpad;
   }

   bool modified = false;

   if (obj && lnk) {
      if (gDebug > 1)
         Info("DecodeAllRanges", "Set draw option \"%s\" for object %s %s", item.opt.c_str(),
               obj->ClassName(), obj->GetName());
      lnk->SetOption(item.opt.c_str());
      modified = true;
   }

   if (item.fcust.compare("frame") == 0) {
      if (obj && obj->InheritsFrom(TFrame::Class())) {
         TFrame *frame = static_cast<TFrame *>(obj);
         if (item.fopt.size() >= 4) {
            frame->SetX1(item.fopt[0]);
            frame->SetY1(item.fopt[1]);
            frame->SetX2(item.fopt[2]);
            frame->SetY2(item.fopt[3]);
            modified = true;
         }
      }
   } else if (item.fcust.compare("pave") == 0) {
      if (obj && obj->InheritsFrom(TPave::Class())) {
         TPave *pave = static_cast<TPave *>(obj);
         if ((item.fopt.size() >= 4) && objpad) {
            auto *save = gPad;
            gPad = objpad;

            // first time need to overcome init problem
            pave->ConvertNDCtoPad();

            pave->SetX1NDC(item.fopt[0]);
            pave->SetY1NDC(item.fopt[1]);
            pave->SetX2NDC(item.fopt[2]);
            pave->SetY2NDC(item.fopt[3]);

            // printf("Setting %s %s %f %f %f %f\n", pave->GetName(), pave->ClassName(), item.fopt[0], item.fopt[1], item.fopt[2], item.fopt[3]);
            modified = true;

            pave->ConvertNDCtoPad();
            gPad = save;
         }
      }
   }

   return modified ? objpad : nullptr;
}

