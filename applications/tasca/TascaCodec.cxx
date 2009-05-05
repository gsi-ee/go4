using namespace std;
#include "Riostream.h"
#include <string>
#include "TascaCodec.h"
TascaCodec::TascaCodec() : TGo4Parameter(),
testmaxi(7),test(0),fiValue(0),fiReg0(0),fiReg1(0),fiReg2(0),fiReg3(0)
{}
TascaCodec::TascaCodec(const char* name) : TGo4Parameter(name),
testmaxi(7),test(0),fiValue(0),fiReg0(0),fiReg1(0),fiReg2(0),fiReg3(0)
{}
TascaCodec::~TascaCodec()
{}

void TascaCodec::setMpxIndex(UInt_t reg0, UInt_t reg1, UInt_t reg2, UInt_t reg3)
{
int s=0,i;
// bits are inverted!
	fiReg0=~reg0;
	fiReg1=~reg1;
	fiReg2=~reg2;
	fiReg3=~reg3;
	for(i=0;i<5;i++){
		fiMpxIndex[i]    = ((fiReg0>>s)&7) - test;
		fiMpxIndex[i+10] = ((fiReg1>>s)&7) - test;
		fiMpxIndex[i+20] = ((fiReg2>>s)&7) - test;
		fiMpxIndex[i+30] = ((fiReg3>>s)&7) - test;
		s += 3;
	}
	s++; // skip flag bit
	for(i=5;i<10;i++){
		fiMpxIndex[i]    = ((fiReg0>>s)&7) - test;
		fiMpxIndex[i+10] = ((fiReg1>>s)&7) - test;
		fiMpxIndex[i+20] = ((fiReg2>>s)&7) - test;
		fiMpxIndex[i+30] = ((fiReg3>>s)&7) - test;
		s += 3;
	}
	test++;
	if(test > testmaxi) test=0;
	if(testmaxi > 7)cout << "Testmaxi " << testmaxi << endl;
}
void TascaCodec::setMap(Bool_t print){
int i=0,si=0;
// fiStopX
for(i=0;i<6;i++) for(si=0;si<8;si++) {
	fiMap[i][si]   = 8 + i*16 + si*2;
	fiMap[i+6][si] = 9 + i*16 + si*2;
}
for(si=0;si<8;si++) {
	fiMap[12][si] = 104 + si*2;
	fiMap[13][si] = 120 + si*2;
	fiMap[14][si] = 105 + si*2;
	fiMap[15][si] = 121 + si*2;
}
for(i=0;i<16;i++) {
	fiDetector[i]=STOPX;
	fiStopX[i]=i;
}

for(si=0;si<4;si++) {
	fiMap[36][si]   =   0 + si*2;
	fiMap[36][si+4] = 136 + si*2;
	fiMap[37][si]   =   1 + si*2;
	fiMap[37][si+4] = 137 + si*2;
}
fiDetector[36]=STOPX;
fiDetector[37]=STOPX;
fiStopX[16]=36;
fiStopX[17]=37;
// fiBack
for(i=16;i<24;i++) {
	for(si=0;si<8;si++)	fiMap[i][si] = (i-16)*8 + si;
	fiDetector[i]=BACK;
	fiBack[i-16]=i;
	}
// fiStopY
for(i=24;i<27;i++) for(si=0;si<8;si++) {
	fiMap[i][si]   =  0 + (i-24)*16 + si*2;
	fiMap[i+3][si] =  1 + (i-24)*16 + si*2;
	fiMap[i+6][si] = 48 + (i-24)*16 + si*2;
	fiMap[i+9][si] = 49 + (i-24)*16 + si*2;
}
for(i=24;i<36;i++) {
	fiDetector[i]=STOPY;
	fiStopY[i-24]=i;
}
// fiVeto
for(si=0;si<4;si++) {
	fiMap[38][si]   = 0 + si*2;
	fiMap[38][si+4] = 8 + si*2;
	fiMap[39][si]   = 1 + si*2;
	fiMap[39][si+4] = 9 + si*2;
}
fiDetector[38]=VETO;
fiDetector[39]=VETO;
fiVeto[0]=38;
fiVeto[1]=39;

if(print){
//i=printMap(STOPX);
//i=printMap(STOPY);
//i=printMap(BACK);
//i=printMap(VETO);
printDetector(STOPX,"StopX");
printDetector(STOPY,"StopY");
printDetector(BACK,"Back");
printDetector(VETO,"Veto");
}}
void TascaCodec::printDetector(UInt_t detector, const char *name){
	printf("Detector %s (%d) ",name,detector);
	if(detector == STOPX) printDetector(fiStopX,STOPX_SIZE);
	if(detector == STOPY) printDetector(fiStopY,STOPY_SIZE);
	if(detector == BACK)  printDetector(fiBack,BACK_SIZE);
	if(detector == VETO)  printDetector(fiVeto,VETO_SIZE);
}
void TascaCodec::printDetector(UInt_t *adc, UInt_t n){
	UInt_t i=0,si=0;
	for(i=0;i<n;i++) {
			printf("\nadc %2d stripes ",*(adc+i));
			for(si=0;si<8;si++)	printf(" %3d",fiMap[*(adc+i)][si]);
	}
	printf("\n");
}

UInt_t TascaCodec::printMap(UInt_t detector){
int i=0,si=0,n=0;
printf("Detector %d ",detector);
for(i=0;i<40;i++) {
	if(fiDetector[i]==detector){
		n++;
		printf("\nadc %2d stripes ",i);
		for(si=0;si<8;si++)	printf(" %3d",fiMap[i][si]);
}}
printf("\nDetector %d uses %d ADC channels\n",detector,n);
return n;
}
//-----------------------------------------------------------
Bool_t TascaCodec::UpdateFrom(TGo4Parameter *pp){
  if(pp->InheritsFrom("TascaCodec")) {
    TascaCodec * from = (TascaCodec *) pp;
    testmaxi=from->testmaxi;
  }
  else
     cout << "Wrong parameter object: " << pp->ClassName() << endl;
  return kTRUE;
}

