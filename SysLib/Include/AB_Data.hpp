//*********************************************************
//	AB_Data.hpp - network link A->B access key
//*********************************************************

#ifndef AB_DATA_HPP
#define AB_DATA_HPP

#include "Complex_Array.hpp"

#pragma pack (push, 4)

//---------------------------------------------------------
//	AB_Data struct definition
//---------------------------------------------------------

typedef struct {
	int anode;
	int bnode;
	int dir;
} AB_Data;

//---------------------------------------------------------
//	AB_Key class definition
//---------------------------------------------------------

class AB_Key : public Complex_Array
{
public:
	AB_Key (int max_records = 0);

	bool Add (AB_Data *data = NULL)      { return (Complex_Array::Add (data)); }
	
	AB_Data * Record (int index)         { return ((AB_Data *) Complex_Array::Record (index)); }
	AB_Data * Record (void)              { return ((AB_Data *) Complex_Array::Record ()); }

	AB_Data * Get (int key1, int key2)   { AB_Data ab; ab.anode = key1; ab.bnode = key2; return (Get (&ab)); }
	AB_Data * Get (AB_Data *data)        { return ((AB_Data *) Complex_Array::Get (data)); }

	AB_Data * First_Key (void)           { return ((AB_Data *) Complex_Array::First_Key ()); }
	AB_Data * Next_Key (void)            { return ((AB_Data *) Complex_Array::Next_Key ()); }

	AB_Data * operator[] (int index)     { return (Record (index)); }
};
#pragma pack (pop)
#endif
