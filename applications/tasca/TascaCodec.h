#ifndef TascaCodec_H
#define TascaCodec_H

#include "TObject.h"

class TascaCodec
{
public:
	enum  V785fields {
		// first shift offset, then mask bits:
		GEO = 		0x1F, 		GEO_OFF = 	27,
		CRATE = 	0xFF, 		CRATE_OFF = 16,
		CNT =		0x3F, 		CNT_OFF = 	 0,
		CHAN =		0x1F, 		CHAN_OFF = 	16,
		ADC =		0xFFF, 		ADC_OFF = 	 0,
		EVCNT = 	0xFFFFFF, 	EVCNT_OFF =  0,
		// compare masked value with mask:
		UNDER = 	0x00002000,
		OVER =		0x00001000,
		TYPE =		0x07000000,
		HEADER =	0x02000000,
		DATA =		0x00000000,
		EOB = 		0x04000000,
		UNVALID =	0x06000000
	};
	enum Detector {
		STOPX = 0, STOPX_SIZE = 18,
		STOPY = 1, STOPY_SIZE = 12,
		BACK  = 2, BACK_SIZE  =  8,
		VETO  = 3, VETO_SIZE  =  2
	};
	Detector detector;

	TascaCodec();
	TascaCodec(UInt_t v);
	virtual ~TascaCodec();

	// Setup matrix connecting channels with ADCs.
	// Called once.
	void setMap(Bool_t print);
	// Print map for a given detector and return number of ADC channels used
	UInt_t printMap(UInt_t detector);
	void printDetector(UInt_t detector, const char *name);
	void printDetector(UInt_t *adc, UInt_t n);
	// Decoding the V875
	UInt_t getAddress(){return (fiValue >> GEO_OFF) & GEO;}
	UInt_t getCrate(){return (fiValue >> CRATE_OFF) & CRATE;}
	UInt_t getCnt(){return (fiValue >> CNT_OFF) & CNT;}
	UInt_t getChan(){return (fiValue >> CHAN_OFF) & CHAN;}
	UInt_t getAdc(){return (fiValue >> ADC_OFF) & ADC;}
	UInt_t getCount(){return (fiValue >> EVCNT_OFF) & EVCNT;}
	Bool_t isHeader()  {return (fiValue & TYPE) == HEADER ;}
	Bool_t isData()    {return (fiValue & TYPE) == DATA;}
	Bool_t isEob()     {return (fiValue & TYPE) == EOB;}
	Bool_t isValid()   {return (fiValue & TYPE) != UNVALID;}
	Bool_t isUnder()   {return (fiValue & UNDER) == UNDER;}
	Bool_t isOver()    {return (fiValue & OVER) == OVER;}
    // Store 32 bit value of ADC which is used for the getter methods.
	void setValue(UInt_t v){fiValue=v;}
	// Fill table of multiplex indices from the four registers.
	// Called per event
	void setMpxIndex(UInt_t reg0, UInt_t reg1, UInt_t reg2, UInt_t reg3);
	// return index of stripe from ADC number
	UInt_t getIndex(UInt_t adc){return fiMap[adc][fiMpxIndex[adc]];}
	Bool_t isTof()     {return (fiReg0 & 0x40000000) == 0x40000000;}
	Bool_t isChopper() {return (fiReg0 & 0x80000000) == 0x80000000;}
	Bool_t isMacro()   {return (fiReg1 & 0x40000000) == 0x40000000;}
	Bool_t isMicro()   {return (fiReg1 & 0x80000000) == 0x80000000;}
	UInt_t getStopXAdc(UInt_t adc){return fiStopX[adc];}
	UInt_t getStopYAdc(UInt_t adc){return fiStopY[adc];}
	UInt_t getBackAdc(UInt_t adc) {return fiBack[adc];}
	UInt_t getVetoAdc(UInt_t adc) {return fiVeto[adc];}
	UInt_t getStopXnoAdc() {return STOPX_SIZE;}
	UInt_t getStopYnoAdc() {return STOPY_SIZE;}
	UInt_t getBacknoAdc()  {return BACK_SIZE;}
	UInt_t getVetonoAdc()  {return VETO_SIZE;}

private:
	UInt_t test,fiValue;
	UInt_t fiReg0, fiReg1, fiReg2, fiReg3;
	V785fields fields;
    UInt_t fiDetector[40]; // the detector number for ADC channels
    UInt_t fiMpxIndex[40]; // the multiplex indices from the registers
    UInt_t fiMap[40][8]; // returns stripe from ADC number and multiplex index
    UInt_t fiStopX[STOPX_SIZE]; // list of ADC channels
    UInt_t fiStopY[STOPY_SIZE]; // list of ADC channels
    UInt_t fiBack[BACK_SIZE]; // list of ADC channels
    UInt_t fiVeto[VETO_SIZE]; // list of ADC channels

	ClassDef(TascaCodec,1)
};
#endif
