#ifndef TascaCodec_H
#define TascaCodec_H

#include "TObject.h"

class TascaCodec
{
public:
	enum  V785fields {
		GEO = 		0xF8000000, GEO_OFF = 	27,
		CRATE = 	0x00FF0000, CRATE_OFF = 16,
		CNT =		0x0000003F, CNT_OFF = 	 0,
		CHAN =		0x001F0000, CHAN_OFF = 	16,
		ADC =		0x00000FFF, ADC_OFF = 	 0,
		COUNT = 	0x00FFFFFF, COUNT_OFF =  0,
		UNDER = 	0x00002000,
		OVER =		0x00001000,
		TYPE =		0x07000000, TYPE_OFF =	24,
		HEADER =	0x02000000,
		DATA =		0x0,
		EOB = 		0x04000000,
		UNVALID =	0x06000000
	};

	TascaCodec();
	TascaCodec(Int_t v);
	virtual ~TascaCodec();

	Int_t getAddress(){return (value & TascaCodec::GEO) >> TascaCodec::GEO_OFF;}
	Int_t getCrate(){return (value & TascaCodec::CRATE) >> TascaCodec::CRATE_OFF;}
	Int_t getCnt(){return (value & TascaCodec::CNT) >> TascaCodec::CNT_OFF;}
	Int_t getChan(){return (value & TascaCodec::CHAN) >> TascaCodec::CHAN_OFF;}
	Int_t getAdc(){return (value & TascaCodec::ADC) >> TascaCodec::ADC_OFF;}
	Int_t getCount(){return (value & TascaCodec::COUNT) >> TascaCodec::COUNT_OFF;}
	Int_t getType(){return (value & TascaCodec::TYPE) >> TascaCodec::TYPE_OFF;}
	void setValue(Int_t v){value=v;}
	Bool_t isHeader(){return (value & TascaCodec::TYPE) == TascaCodec::HEADER ;}
	Bool_t isData()  {return (value & TascaCodec::TYPE) == TascaCodec::DATA;}
	Bool_t isEob()   {return (value & TascaCodec::TYPE) == TascaCodec::EOB;}
	Bool_t isValid() {return (value & TascaCodec::TYPE) != TascaCodec::UNVALID;}
	Bool_t isUnder() {return (value & TascaCodec::UNDER) == TascaCodec::UNDER;}
	Bool_t isOver()  {return (value & TascaCodec::OVER) == TascaCodec::OVER;}

private:
	Int_t	value;
	TascaCodec::V785fields fields;

	ClassDef(TascaCodec,1)
};
#endif
