//*********************************************************
//	Speed_Bin_Data.cpp - speed bin data class
//*********************************************************

#include "Speed_Bin_Data.hpp"

//---------------------------------------------------------
//	Speed_Bin_Array constructor
//---------------------------------------------------------

Speed_Bin_Array::Speed_Bin_Array (int nbins, int max_records) :
	Complex_Array (sizeof (Speed_Bin_Data), 2, false, max_records, false)
{
	num_bins = 0;

	Num_Bins (nbins);
}

void Speed_Bin_Array::Num_Bins (int nbins)
{
	if (nbins <= 0) {
		if (num_bins != 0) {
			Data_Size (sizeof (Speed_Bin_Data));
		}
		num_bins = 0;
	} else {
		Data_Size (sizeof (Speed_Bin_Data) + (nbins-1) * sizeof (int));
		num_bins = nbins;
	}
}

