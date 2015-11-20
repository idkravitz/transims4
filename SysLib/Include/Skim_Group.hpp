//*********************************************************
//	Skim_Group.hpp - zone skim control parameters
//*********************************************************

#ifndef SKIM_GROUP_HPP
#define SKIM_GROUP_HPP

#include "Static_Service.hpp"
#include "Matrix_Data.hpp"
#include "Skim_File.hpp"
#include "Index_Array.hpp"
#include "Db_Base.hpp"
#include "Time_Equiv.hpp"

//---------------------------------------------------------
//	Skim_Group class definition
//---------------------------------------------------------

class Skim_Group : public Static_Service
{
public:
	Skim_Group (void);
	virtual ~Skim_Group (void)            { Clear (); }

	int  Group ()                         { return (group); }
	void Group (int value)                { group = value; }

	void Num_Zones (int value)            { zones = value; }
	void Memory_Factor (double value)     { mem_factor = value; }

	int  Dump_Field ()                    { return (dump_field); }

	bool Skim_Filename (char *filename);
	bool Skim_Format (char *format);

	bool Time_Period_File (char *filename);

	bool Read_Skim (bool equiv_report = false);

	bool Get_Record_A (int org, int des, int time);
	bool Get_Record_B (int org, int des, int time);

	void Clear (void);

	Db_Base * Skim_A (void)               { return (skim_a); }
	Db_Base * Skim_B (void)               { return (skim_b); }

private:
	int group, zones, key_a, key_b, dump_field;
	double mem_factor;
	Matrix_Key skim_key;
	Db_Base *skim_a, *skim_b;
	Index_Array *skim_data;
	Skim_File *skim_file;
	Time_Equiv *time_equiv;
};

//---------------------------------------------------------
//	Skim_Group_Array
//---------------------------------------------------------

class Skim_Group_Array : public Data_Array
{
public:
	Skim_Group_Array (int max_records = 0);
	virtual ~Skim_Group_Array (void);

	bool Add (Skim_Group *data = NULL)   { return (Data_Array::Add ((void *) data)); }

	Skim_Group * New_Record (bool clear = false, int number = 1)
	                                     { return ((Skim_Group *) Data_Array::New_Record (clear, number)); }

	Skim_Group * First (void)            { return ((Skim_Group *) Data_Array::First ()); }
	Skim_Group * Next (void)             { return ((Skim_Group *) Data_Array::Next ()); }

	Skim_Group * Last (void)             { return ((Skim_Group *) Data_Array::Last ()); }
	Skim_Group * Previous (void)         { return ((Skim_Group *) Data_Array::Previous ()); }

	Skim_Group * operator[] (int index)  { return ((Skim_Group *) Record (index)); }
};

#endif

