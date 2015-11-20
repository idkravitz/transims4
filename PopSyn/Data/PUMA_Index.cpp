//*********************************************************
//	PUMA_Index.cpp - PUMA index class
//*********************************************************

#include "PUMA_Index.hpp"

#include "Utility.hpp"

PUMA_Index PUMA_Index_Array::key;

//---------------------------------------------------------
//	PUMA_Index_Array constructor
//---------------------------------------------------------

PUMA_Index_Array::PUMA_Index_Array (int max_records) : 
	Index_Array (sizeof (PUMA_Index), max_records)
{
}

//---- key processing ----

PUMA_Index * PUMA_Index_Array::PUMA_Key (char *state, int puma)
{
	char buffer [FIELD_BUFFER];

	str_fmt (buffer, sizeof (buffer), "%2.2s%d", state, puma);

	return (PUMA_Key (buffer));
}

PUMA_Index * PUMA_Index_Array::PUMA_Key (char *state_puma)
{
	key.state = 0;
	for (int i=0; i < 2; i++) {
		key.state <<= 8;
		if (state_puma [i] >= 'a' && state_puma [i] <= 'z') {
			state_puma [i] += 'A' - 'a';
		}
		key.state += (int) (state_puma [i]);
	}
	if (state_puma [2] < '0' || state_puma [2] > '9') {
		return (NULL);
	}
	key.puma = atoi (state_puma+2);
	
	str_cpy (key.text, sizeof (key.text), state_puma, sizeof (key.text)-1);

	return (&key);
}
