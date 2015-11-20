//*********************************************************
//	Phasing_Data.cpp - network phasing plan data classes
//*********************************************************

#include "Phasing_Data.hpp"

#include <string.h>

//---------------------------------------------------------
//	Phasing_Data constructor
//---------------------------------------------------------

Phasing_Data::Phasing_Data (void)
{
	Reset ();
	Node (0);
	In_Link_Dir (0);
	Out_Link_Dir (0);
	Timing (0);
	Phase (0);
	Protection (0);
	Detector (0);
}

void Phasing_Data::Clear (void)
{
	if (num_detectors > 1) {
		delete [] detector;
	}
	num_detectors = 0;
}

int Phasing_Data::Detector (int num)
{ 
	if (num > 0 && num <= num_detectors) {
		if (num_detectors == 1) {
			return (detector1);
		} else {
			return (detector [num-1]);
		}
	} else {
		return (0);
	}
}

void Phasing_Data::Detector (int num, int value)
{ 
	if (num > 0 && num <= num_detectors) {
		if (num_detectors == 1) {
			detector1 = value;
		} else {
			detector [num-1] = value;
		}
	}
}

bool Phasing_Data::Num_Detectors (int num)
{
	Clear ();

	if (num == 1) {
		num_detectors = 1;
	} else if (num > 1 && num < 128) {
		detector = new int [num];
		if (detector == NULL) return (false);

		memset (detector, '\0', num * sizeof (int));
		num_detectors = (char) num;
	}
	return (true);
}

//---------------------------------------------------------
//	Phasing_Array constructor
//---------------------------------------------------------

Phasing_Array::Phasing_Array (int max_records) : 
	Data_Array (sizeof (Phasing_Data), max_records)
{
}
