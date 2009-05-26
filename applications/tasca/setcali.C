#include <fstream.h>
#include <Riostream.h>
#include "RVersion.h"
void setcali()
{
Bool_t on = kFALSE; // set value here
TascaCalibration *fCalibration = (TascaCalibration *) go4->GetObject("Calibration","Go4");
if(fCalibration!=0)  fCalibration->enableCalibration(on);
}
