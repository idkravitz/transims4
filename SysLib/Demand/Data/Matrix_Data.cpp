//*********************************************************
//	Matrix_Data.cpp - matrix data classes
//*********************************************************

#include "Matrix_Data.hpp"

#include <stdio.h>

//---------------------------------------------------------
//	Matrix_Key constructor
//---------------------------------------------------------

Matrix_Key::Matrix_Key (int num_periods, int num_zones)
{
	Set_Key (num_periods, num_zones);
}

bool Matrix_Key::Set_Key (int num_periods, int num_zones)
{
	int i, mask, period_bits, int_bits;
	bool stat = true;

	int_bits = sizeof (int) * 8;

	mask = 0;

	for (i=1; i < int_bits; i++) {
		mask = (1 << i);

		if (num_periods < mask) break;
	}
	period_bits = i;
	period_mask = mask;

	if (num_zones == 0) {
		zone_bits = (int_bits - period_bits - 1) / 2;
	} else {
		for (i=1; i < int_bits; i++) {
			mask = (1 << i);

			if (num_zones < mask) break;
		}
		zone_bits = i;

		if (zone_bits * 2 + period_bits >= int_bits) {
			zone_bits = (int_bits - period_bits - 1) / 2;
			stat = false;
		}
	}
	zone_mask = (1 << zone_bits);
	return (stat);
}

int Matrix_Key::Key (int org, int des, int period) 
{
	if (org >= zone_mask || des >= zone_mask || period >= period_mask) {
		return (0);
	} else if (org < 1 || des < 1 || period < 1) {
		return (0);
	} else {
		return ((((period << zone_bits) + org) << zone_bits) + des);
	}
}

void Matrix_Key::Key (int key, int *org, int *des, int *period)
{
	*des = key % zone_mask;
	key >>= zone_bits;
	*org = key % zone_mask;
	*period = key >> zone_bits;
}

void Matrix_Key::Key (int key, int *org, int *des)
{
	*des = key % zone_mask;
	key >>= zone_bits;
	*org = key % zone_mask;
}

//---------------------------------------------------------
//	Matrix_Data constructor
//---------------------------------------------------------

Matrix_Data::Matrix_Data (int key) : Class_Index (key)
{
	Data (0);
}

//---------------------------------------------------------
//	Matrix_Array constructor
//---------------------------------------------------------

Matrix_Array::Matrix_Array (int num_periods, int num_zones, int max_records) : 
	Class_Array (sizeof (Matrix_Data), max_records, false), Matrix_Key (num_periods, num_zones)
{
}
