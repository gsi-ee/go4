
void TGo4HisDrawOptionsShort::init()
{
}

void TGo4HisDrawOptionsShort::SlotXlin()
{
   SetDrawOpt(3, 0, "");
}

void TGo4HisDrawOptionsShort::SlotXlog()
{
    SetDrawOpt(3, 1, "");
}

void TGo4HisDrawOptionsShort::SlotYlin()
{
    SetDrawOpt(4, 0, "");
}

void TGo4HisDrawOptionsShort::SlotYlog()
{
    SetDrawOpt(4, 1, "");
}

void TGo4HisDrawOptionsShort::SlotZlin()
{
    SetDrawOpt(5, 0, "");
}

void TGo4HisDrawOptionsShort::SlotZlog()
{
    SetDrawOpt(5, 1, "");
}

void TGo4HisDrawOptionsShort::SlotD1his()
{
   SetDrawOpt(0, 0, "");
}

void TGo4HisDrawOptionsShort::SlotD1line()
{
   SetDrawOpt(0, 0, "L");
}

void TGo4HisDrawOptionsShort::SlotD2scatter()
{
    SetDrawOpt(0, 0, "");
}

void TGo4HisDrawOptionsShort::SlotD2pixel()
{
    SetDrawOpt(0, 0, "COLZ");
}

void TGo4HisDrawOptionsShort::SlotD2surfcol()
{
   SetDrawOpt(0, 0, "SURF2");
}

void TGo4HisDrawOptionsShort::SlotD2lego()
{
    SetDrawOpt(0, 0, "LEGO2");
}

void TGo4HisDrawOptionsShort::SlotD2contfill()
{
    SetDrawOpt(0, 0, "CONT0Z");
}

void TGo4HisDrawOptionsShort::SlotD2contline()
{
    SetDrawOpt(0, 0, "CONT1Z");
}

void TGo4HisDrawOptionsShort::SlotLineColor()
{
   ChangeColor(0);
}

void TGo4HisDrawOptionsShort::SlotFillColor()
{
   ChangeColor(1);
}

void TGo4HisDrawOptionsShort::SlotMarkerColor()
{
   ChangeColor(2);
}

void TGo4HisDrawOptionsShort::ChangeColor(int kind)
{
   TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();
   if (panel==0) return;

   TPad* pad = panel->GetActivePad();
   TObject* obj = panel->GetSelectedObject(pad, 0);

   Color_t icol = 0;

   switch (kind) {
      case 0: {
        TAttLine* att = dynamic_cast<TAttLine*> (obj);
        if (att==0) return;
        icol = att->GetLineColor();
        break;
      }
      case 1: {
        TAttFill* att = dynamic_cast<TAttFill*> (obj);
        if (att==0) return;
        icol = att->GetFillColor();
        break;
      }
      case 2: {
        TAttMarker* att = dynamic_cast<TAttMarker*> (obj);
        if (att==0) return;
        icol = att->GetMarkerColor();
        break;
      }
   }

   TColor* rcol = gROOT->GetColor(icol);

   QColor qcol;

   if (rcol!=0)
      qcol.setRgb(rcol->GetRed(), rcol->GetGreen(), rcol->GetBlue());

   QColor c = QColorDialog::getColor(qcol);
   if (!c.isValid()) return;

   icol = TColor::GetColor(c.red(), c.green(), c.blue());

   switch (kind) {
      case 0: {
        TAttLine* att = dynamic_cast<TAttLine*> (obj);
        if (att!=0) att->SetLineColor(icol);
        break;
      }
      case 1: {
        TAttFill* att = dynamic_cast<TAttFill*> (obj);
        if (att!=0) att->SetFillColor(icol);
        break;
      }
      case 2: {
        TAttMarker* att = dynamic_cast<TAttMarker*> (obj);
        if (att!=0) att->SetMarkerColor(icol);
        break;
      }
   }

   panel->MarkPadModified(pad);

   panel->ShootRepaintTimer(pad);

   panel->ActivateInGedEditor(obj);
}

void TGo4HisDrawOptionsShort::SetDrawOpt(int kind, int value, char *dropt)
{
   TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();
   if (panel!=0) panel->ChangeDrawOption(kind, value,dropt);
}
