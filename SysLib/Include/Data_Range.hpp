//*********************************************************
//	Data_Range.hpp - data range class
//*********************************************************

#ifndef DATA_RANGE_HPP
#define DATA_RANGE_HPP

#include "Range_Data.hpp"
#include "Static_Service.hpp"
#include "System_Defines.hpp"

//---------------------------------------------------------
//	Data_Range Class definition
//---------------------------------------------------------

class Data_Range : public Range_Array, public Static_Service
{
public:
	Data_Range (int factor = 1, int minimum = 0, int maximum = MAX_INTEGER, int increment = 0);

	void Setup (int factor = 1, int minimum = 0, int maximum = MAX_INTEGER, int increment = 0);

	bool Add_Ranges (char *ranges);
	bool Add_Breaks (char *breaks);

private:
	int factor, minimum, maximum, increment;
};

#endif
