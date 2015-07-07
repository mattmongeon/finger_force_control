#ifndef INCLUDED_BIOTAC_H
#define INCLUDED_BIOTAC_H


#pragma pack(push,1)
typedef struct
{
	unsigned short pac;
	unsigned short pdc;

	unsigned short tac;
	unsigned short tdc;

	unsigned short e1;
	unsigned short e2;
	unsigned short e3;
	unsigned short e4;
	unsigned short e5;
	unsigned short e6;
	unsigned short e7;
	unsigned short e8;
	unsigned short e9;
	unsigned short e10;
	unsigned short e11;
	unsigned short e12;
	unsigned short e13;
	unsigned short e14;
	unsigned short e15;
	unsigned short e16;
	unsigned short e17;
	unsigned short e18;
	unsigned short e19;
} biotac_data;


typedef struct
{
	unsigned long timestamp;
	biotac_data data;
	int load_cell;
} biotac_tune_data;
#pragma pack(pop)


void biotac_init();


void read_biotac(biotac_data* pData);



#endif  // INCLUDED_BIOTAC_H

