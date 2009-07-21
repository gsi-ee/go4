//---------------------------------------------
// Go4 Comp analysis 
// Author: Hans G. Essel 
//         H.Essel@gsi.de 
// GSI, Experiment Electronics, Data Processing
//---------------------------------------------

#ifndef CompCodec_H
#define CompCodec_H

#include "TObject.h"
#include "TGo4Parameter.h"

class CompCodec : public TGo4Parameter
{
public:
	enum {
		// first shift offset, then mask bits:
		// SIS3302
		SMAX_RAW =	1024,	SMAX_E =		512, SCHANNELS = 8,
		SCARD =		0xFF,	SCARD_OFF = 	 24,
		SCHAN =		0xFF,	SCHAN_OFF = 	 16,
		SINDEX =	0x7,	SINDEX_OFF = 	 16,
		SDATA =		0xFFFF,	SDATAL_OFF =  	  0, SDATAH_OFF = 16,
		// V785
		VGEO = 		0x1F, 		VGEO_OFF = 	27,
		VCRATE = 	0xFF, 		VCRATE_OFF =16,
		VCNT =		0x3F, 		VCNT_OFF = 	 0,
		VCHAN =		0x1F, 		VCHAN_OFF = 16,
		VADC =		0xFFF, 		VADC_OFF = 	 0,
		VEVCNT = 	0xFFFFFF, 	VEVCNT_OFF = 0,
		// compare masked value with mask:
		VUNDER = 	0x00002000,
		VOVER =		0x00001000,
		VTYPE =		0x07000000,
		VHEADER =	0x02000000,
		VDATA =		0x00000000,
		VEOB = 		0x04000000,
		VUNVALID =	0x06000000,

		STOPX = 0, STOPX_SIZE = 18,
		STOPY = 1, STOPY_SIZE = 12,
		BACK  = 2, BACK_SIZE  =  8,
		VETO  = 3, VETO_SIZE  =  2,
		GAMMA = 4
	};

	CompCodec();
	CompCodec(const char* name);
	virtual ~CompCodec();
    Bool_t  UpdateFrom(TGo4Parameter *);

	// Setup matrix connecting channels with ADCs.
	// Called once.
	void setMap(Bool_t print);
	// Print map for a given detector and return number of ADC channels used
	UInt_t printMap(UInt_t detector);
	void printDetector(UInt_t detector, const char *name);
	void printDetector(UInt_t *adc, UInt_t n);
	// Decoding the V875
	inline UInt_t getAddress(){return (fiValue >> VGEO_OFF) & VGEO;}
	inline UInt_t getCrate(){return (fiValue >> VCRATE_OFF) & VCRATE;}
	inline UInt_t getCnt(){return (fiValue >> VCNT_OFF) & VCNT;}
	inline UInt_t getChan(){return (fiValue >> VCHAN_OFF) & VCHAN;}
	inline UInt_t getAdc(){return (fiValue >> VADC_OFF) & VADC;}
	inline UInt_t getCount(){return (fiValue >> VEVCNT_OFF) & VEVCNT;}
	inline Bool_t isHeader()  {return (fiValue & VTYPE) == VHEADER ;}
	inline Bool_t isData()    {return (fiValue & VTYPE) == VDATA;}
	inline Bool_t isEob()     {return (fiValue & VTYPE) == VEOB;}
	inline Bool_t isValid()   {return (fiValue & VTYPE) != VUNVALID;}
	inline Bool_t isUnder()   {return (fiValue & VUNDER) == VUNDER;}
	inline Bool_t isOver()    {return (fiValue & VOVER) == VOVER;}
    // Store 32 bit value of ADC which is used for the getter methods.
	inline void setValue(UInt_t v){fiValue=v;}
	// Fill table of multiplex indices from the four registers.
	// Called per event
	void setMpxIndex(UInt_t reg0, UInt_t reg1, UInt_t reg2, UInt_t reg3);
	inline UInt_t * getMpxIndex(){return fiMpxIndex;}
	// return index of stripe from ADC number
	inline UInt_t getIndex(UInt_t adc){return fiMap[adc][fiMpxIndex[adc]];}
	// bits in fiReg are inverted
	inline Bool_t isTof()     {return (fiReg0 & 0x00008000) != 0x00008000;} // bit 15
	inline Bool_t isChopper() {return (fiReg0 & 0x80000000) != 0x80000000;} // bit 31
	inline Bool_t isMacro()   {return (fiReg1 & 0x00008000) != 0x00008000;}
	inline Bool_t isMicro()   {return (fiReg1 & 0x80000000) != 0x80000000;}
	inline UInt_t getStopXAdc(UInt_t adc){return fiStopX[adc];}
	inline UInt_t getStopYAdc(UInt_t adc){return fiStopY[adc];}
	inline UInt_t getBackAdc(UInt_t adc) {return fiBack[adc];}
	inline UInt_t getVetoAdc(UInt_t adc) {return fiVeto[adc];}
	inline UInt_t getStopXnoAdc() {return STOPX_SIZE;}
	inline UInt_t getStopYnoAdc() {return STOPY_SIZE;}
	inline UInt_t getBacknoAdc()  {return BACK_SIZE;}
	inline UInt_t getVetonoAdc()  {return VETO_SIZE;}
	void Cleanup()
	{
	   	memset((void*) &fiDetector[0],0, sizeof(fiDetector));
	   	memset((void*) &fiMpxIndex[0],0, sizeof(fiMpxIndex));
	   	memset((void*) &fiMap[0][0],0, sizeof(fiMap));
	   	memset((void*) &fiStopX[0],0, sizeof(fiStopX));
	   	memset((void*) &fiStopY[0],0, sizeof(fiStopY));
	   	memset((void*) &fiBack[0],0, sizeof(fiBack));
	   	memset((void*) &fiVeto[0],0, sizeof(fiVeto));
	}

private:
	UInt_t testmaxi;//! Must be 0
	UInt_t fiReg0; //! Register 0
	UInt_t fiReg1;//! Register 1
	UInt_t fiReg2; //! Register 2
	UInt_t fiReg3;//! Register 3
    UInt_t fiDetector[40]; //! the detector number for ADC channels
    UInt_t fiMpxIndex[40]; //! the multiplex indices from the registers
    UInt_t fiMap[40][8]; //! returns stripe from ADC number and multiplex index
    UInt_t fiStopX[STOPX_SIZE]; //! list of ADC channels
    UInt_t fiStopY[STOPY_SIZE]; //! list of ADC channels
    UInt_t fiBack[BACK_SIZE]; //! list of ADC channels
    UInt_t fiVeto[VETO_SIZE]; //! list of ADC channels
	UInt_t test;//! Transient
	UInt_t fiValue;//! Transient

	ClassDef(CompCodec,1)
};
#endif
