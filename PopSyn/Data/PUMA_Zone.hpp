//*********************************************************
//	PUMA_Zone.hpp - PUMA Zone Data classes
//*********************************************************

#ifndef PUMA_ZONE_HPP
#define PUMA_ZONE_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	PUMA_Zone class definition
//---------------------------------------------------------

class PUMA_Zone : public Class_Index
{
public:
	PUMA_Zone (int zone = 0);

	int      Zone (void)             { return (ID ()); }
	double * Data (void)             { return (data); }

	void     Zone (int value)        { ID (value); }
	void     Data (double *value)    { data [0] = value [0]; }

private:
	double data [1];
};

//---------------------------------------------------------
//	PUMA_Zone_Array class definition
//---------------------------------------------------------

class PUMA_Zone_Array : public Class_Array
{
public:
	PUMA_Zone_Array (int max_records = 0);

	bool Add (PUMA_Zone *data = NULL)    { return (Class_Array::Add (data)); }
	
	PUMA_Zone * New_Record (bool clear = false, int number = 1)
	                                     { return ((PUMA_Zone *) Class_Array::New_Record (clear, number)); }

	PUMA_Zone * Get (int puma)           { return ((PUMA_Zone *) Class_Array::Get (puma)); }
	
	PUMA_Zone * Record (int index)       { return ((PUMA_Zone *) Class_Array::Record (index)); }
	PUMA_Zone * Record (void)            { return ((PUMA_Zone *) Class_Array::Record ()); }

	PUMA_Zone * First (void)             { return ((PUMA_Zone *) Class_Array::First ()); }
	PUMA_Zone * Next (void)              { return ((PUMA_Zone *) Class_Array::Next ()); }

	PUMA_Zone * First_Key (void)         { return ((PUMA_Zone *) Class_Array::First_Key ()); }
	PUMA_Zone * Next_Key (void)          { return ((PUMA_Zone *) Class_Array::Next_Key ()); }

	PUMA_Zone * operator[] (int index)   { return (Record (index)); }

};
#endif
