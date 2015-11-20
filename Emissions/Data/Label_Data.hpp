//*********************************************************
//	Label_Data.hpp - category label data classes
//*********************************************************

#ifndef LABEL_DATA_HPP
#define LABEL_DATA_HPP

#include "Class_Array.hpp"
#include "Utility.hpp"

//---------------------------------------------------------
//	Label_Data class definition
//---------------------------------------------------------

class Label_Data : public Class_Index
{
public:
	Label_Data (int value = 0);

	int  Value (void)                  { return (ID ()); }
	char * Label (void)                { return (label); }

	void Value (int value)             { ID (value); }
	void Label (char * value)          { str_cpy (label, sizeof (label), value); }

private:
	char label [24];
};

//---------------------------------------------------------
//	Label_Array class definition
//---------------------------------------------------------

class Label_Array : public Class_Array
{
public:
	Label_Array (int max_records = 10);

	bool Add (Label_Data *data = NULL)    { return (Class_Array::Add (data)); }

	Label_Data * Record (int index)       { return ((Label_Data *) Class_Array::Record (index)); }
	Label_Data * Record (void)            { return ((Label_Data *) Class_Array::Record ()); }

	Label_Data * Get (int value)          { return ((Label_Data *) Class_Array::Get (value)); }

	Label_Data * First (void)             { return ((Label_Data *) Class_Array::First ()); }
	Label_Data * Next (void)              { return ((Label_Data *) Class_Array::Next ()); }

	Label_Data * Last (void)              { return ((Label_Data *) Class_Array::Last ()); }

	Label_Data * operator[] (int index)   { return (Record (index)); }
};

#endif
