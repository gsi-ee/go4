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


void TGo4CreateNewCondition::init()
{
   setAcceptDrops(FALSE);
   CondName->setText("new_cond");
   ClassnameCombo->setCurrentItem(0);
   ArraySizeSpin->setValue(1);
}

TGo4Condition* TGo4CreateNewCondition::MakeCondition()
{
   const char* cname = CondName->text().latin1();
   int arrsize = ArraySizeSpin->value();
   int ctype = ClassnameCombo->currentItem();

   if ((cname==0) || (*cname==0)) return 0;
   TGo4Condition* cond = 0;

   if (arrsize>1) {
       TGo4CondArray* arr = 0;
       if (ctype==2) {
          arr = new TGo4CondArray(cname, arrsize, "TGo4PolyCond");
          for (int n=0;n<arrsize;n++) {
             TGo4PolyCond* pcond = dynamic_cast<TGo4PolyCond*> (arr->At(n));
             Double_t xx[5] = {0, 100, 100,   0, 0};
             Double_t yy[5] = {0, 0,   100, 100, 0};
             for(int indx=0;indx<5;indx++) {
                xx[indx]+=n*20.;
                yy[indx]+=n*20.;
             }
             if (pcond!=0)
               pcond->SetValues(xx, yy, 5);
          }
       } else {
          arr = new TGo4CondArray(cname, arrsize, "TGo4WinCond");
          for (int n=0;n<arrsize;n++) {
            TGo4WinCond* wcond = dynamic_cast<TGo4WinCond*> (arr->At(n));
            if (wcond==0) continue;
            if (ctype==0)
               wcond->SetValues(0.,100.);
            else
               wcond->SetValues(0.,100.,0.,100.);
          }
       }
       cond = arr;
   } else

   switch (ctype) {
      case 0: {
         TGo4WinCond* wcond = new TGo4WinCond(cname, "1-D window condition");
         wcond->SetValues(0.,100.);
         cond = wcond;
         break;
      }
      case 1: {
         TGo4WinCond* wcond = new TGo4WinCond(cname, "2-D window condition");
         wcond->SetValues(0.,100.,0.,100.);
         cond = wcond;
         break;
      }
      case 2: {
         TGo4PolyCond* pcond = new TGo4PolyCond(cname, "Polygon condition");
         Double_t xx[5] = {0, 100, 100,   0, 0};
         Double_t yy[5] = {0, 0,   100, 100, 0};
         pcond->SetValues(xx, yy, 5);
         cond = pcond;
         break;
      }
   }

   if(cond!=0)
     cond->Enable();

   return cond;
}
