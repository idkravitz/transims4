//*********************************************************
//	Time_Equiv.hpp - Time Period Equivalence File
//*********************************************************

#ifndef TIME_EQUIV_HPP
#define TIME_EQUIV_HPP

#include "Time_Range.hpp"
#include "Pointer_Array.hpp"
#include "Db_File.hpp"

//---------------------------------------------------------
//	Time_Equiv Class definition
//---------------------------------------------------------

class Time_Equiv : public Data_Pointer_Array, public Db_File
{
public:
	Time_Equiv (void);
	
	virtual void Delete_Pointers (void);

	bool Read (bool report_flag);

	int  Period (int time);

	int  Num_Periods (void)				{ return (Data_Array::Num_Records ()); };
};

#endif
