//*********************************************************
//	PUMA_Index.hpp - PUMA Index Data classes
//*********************************************************

#ifndef PUMA_INDEX_HPP
#define PUMA_INDEX_HPP

#include "Index_Array.hpp"

//---------------------------------------------------------
//	PUMA_Index structure
//---------------------------------------------------------

typedef struct {
	short puma;
	short state;
	char  text [16];
} PUMA_Index;

//---------------------------------------------------------
//	PUMA_Index_Array class definition
//---------------------------------------------------------

class PUMA_Index_Array : public Index_Array
{
public:
	PUMA_Index_Array (int max_records = 10);

	bool Add (PUMA_Index *data = NULL)   { return (Index_Array::Add (data)); }
	
	PUMA_Index * New_Record (bool clear = false, int number = 1)
	                                     { return ((PUMA_Index *) Index_Array::New_Record (clear, number)); }

	PUMA_Index * Get (PUMA_Index *data)  { return ((PUMA_Index *) Index_Array::Get (data)); }
	
	PUMA_Index * Record (int index)      { return ((PUMA_Index *) Index_Array::Record (index)); }
	PUMA_Index * Record (void)           { return ((PUMA_Index *) Index_Array::Record ()); }

	PUMA_Index * First (void)            { return ((PUMA_Index *) Index_Array::First ()); }
	PUMA_Index * Next (void)             { return ((PUMA_Index *) Index_Array::Next ()); }

	PUMA_Index * First_Key (void)        { return ((PUMA_Index *) Index_Array::First_Key ()); }
	PUMA_Index * Next_Key (void)         { return ((PUMA_Index *) Index_Array::Next_Key ()); }

	PUMA_Index * operator[] (int index)  { return (Record (index)); }

	PUMA_Index * PUMA_Key (char *state, int puma);
	PUMA_Index * PUMA_Key (char *state_puma);

private:
	static PUMA_Index key;
};
#endif
