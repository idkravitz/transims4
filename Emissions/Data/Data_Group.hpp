//*********************************************************
//	Data_Group.hpp - speed bin data group
//*********************************************************

#ifndef DATA_GROUP_HPP
#define DATA_GROUP_HPP

#include "Data_Array.hpp"
#include "Speed_Bin_File.hpp"

//---------------------------------------------------------
//	Data_Group class definition
//---------------------------------------------------------

class Data_Group
{
public:
	Data_Group (void);
	virtual ~Data_Group (void)               { Clear (); }

	int  Group (void)                        { return (group); }
	double Factor (void)                     { return (factor); }
	Speed_Bin_File * Speed_File (void)       { return (file); }

	void Group (int value)                   { group = value; }
	void Factor (double value)               { factor = value; }
	bool Open_File (char *name, char *format = NULL);

	void Clear (void);

private:
	int group;
	double factor;
	Speed_Bin_File *file;
};

//---------------------------------------------------------
//	Data_Group_Array
//---------------------------------------------------------

class Data_Group_Array : public Data_Array
{
public:
	Data_Group_Array (int max_records = 0);
	virtual ~Data_Group_Array (void);

	bool Add (Data_Group *data = NULL)   { return (Data_Array::Add ((void *) data)); }

	Data_Group * New_Record (bool clear = false, int number = 1)
										 { return ((Data_Group *) Data_Array::New_Record (clear, number)); }

	Data_Group * First (void)            { return ((Data_Group *) Data_Array::First ()); }
	Data_Group * Next (void)             { return ((Data_Group *) Data_Array::Next ()); }

	Data_Group * Last (void)             { return ((Data_Group *) Data_Array::Last ()); }
	Data_Group * Previous (void)         { return ((Data_Group *) Data_Array::Previous ()); }

	Data_Group * operator[] (int index)  { return ((Data_Group *) Record (index)); }
};

#endif

