//*********************************************************
//	Speed_Bin_Data.hpp - Speed Bin Count data
//*********************************************************

#ifndef SPEED_BIN_DATA_HPP
#define SPEED_BIN_DATA_HPP

#include "Complex_Array.hpp"

//---------------------------------------------------------
//	Speed_Bin_Data struct definition
//---------------------------------------------------------

typedef struct {
	int link_dir;
	int offset;
	int bin [1];
} Speed_Bin_Data;

//---------------------------------------------------------
//	Speed_Bin_Array class definition
//---------------------------------------------------------

class Speed_Bin_Array : public Complex_Array
{
public:
	Speed_Bin_Array (int max_speed = 0, int max_records = 0);

	bool Add (Speed_Bin_Data *data = NULL)         { return (Complex_Array::Add (data)); }

	Speed_Bin_Data * New_Record (bool clear = false, int number = 1)
		                                           { return ((Speed_Bin_Data *) Complex_Array::New_Record (clear, number)); }

	Speed_Bin_Data * Record (int index)            { return ((Speed_Bin_Data *) Complex_Array::Record (index)); }
	Speed_Bin_Data * Record (void)                 { return ((Speed_Bin_Data *) Complex_Array::Record ()); }

	Speed_Bin_Data * Get (Speed_Bin_Data *data)    { return ((Speed_Bin_Data *) Complex_Array::Get (data)); }
	Speed_Bin_Data * Get_GE (Speed_Bin_Data *data) { return ((Speed_Bin_Data *) Complex_Array::Get_GE (data)); }

	Speed_Bin_Data * First (void)                  { return ((Speed_Bin_Data *) Complex_Array::First ()); }
	Speed_Bin_Data * Next (void)                   { return ((Speed_Bin_Data *) Complex_Array::Next ()); }

	Speed_Bin_Data * First_Key (void)              { return ((Speed_Bin_Data *) Complex_Array::First_Key ()); }
	Speed_Bin_Data * Next_Key (void)               { return ((Speed_Bin_Data *) Complex_Array::Next_Key ()); }

	Speed_Bin_Data * operator[] (int index)        { return (Record (index)); }

	int  Num_Bins (void)                           { return (num_bins); }
	void Num_Bins (int value);

private:
	int num_bins;
};
#endif
