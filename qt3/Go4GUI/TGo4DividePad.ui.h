void TGo4DividePad::DivideCanvas()
{
  TGo4ViewPanel* panel = TGo4WorkSpace::Instance()->GetActivePanel();

  if (panel!=0)
     panel->Divide(SpinBoxX->value(), SpinBoxY->value());
}
