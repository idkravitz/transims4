//*********************************************************
//	Skim_Group.hpp - zone skim control parameters
//*********************************************************

#ifndef SKIM_GROUP_HPP
#define SKIM_GROUP_HPP

#include "Static_Service.hpp"
#include "Matrix_Data.hpp"
#include "Skim_File.hpp"
#include "Pointer_Array.hpp"
#include "Db_Base.hpp"
#include "Time_Equiv.hpp"

//---------------------------------------------------------
//	Skim_Group class definition
//---------------------------------------------------------

class Skim_Group : public Static_Service
{
public:
	Skim_Group (int group = 0);

	int  Group ()                       { return (group); }
	void Group (int value)              { group = value; }
	
	void Num_Zones (int value)          { zones = value; }
	void Memory_Factor (double value)   { mem_factor = value; }

	bool Skim_Filename (char *filename, char *format = NULL);

	bool Time_Period_File (char *filename);

	bool Read_Skim (bool equiv_report = false);

	bool Get_Record (int org, int des, int time);

	Db_Base * Skim (void)               { return (&skim); }

private:
	int  group, last_key, zones;
	double mem_factor;
	bool time_flag;

	Matrix_Key skim_key;
	Db_Base skim;
	Index_Array skim_data;
	Skim_File skim_file;
	Time_Equiv time_equiv;
};

//---------------------------------------------------------
//	Skim_Group_Array
//---------------------------------------------------------

class Skim_Group_Array : public Data_Pointer_Array
{
public:
	Skim_Group_Array (int max_records = 0);

	void Delete_Pointers (void);

	bool Add (Skim_Group *data = NULL)   { return (Data_Pointer_Array::Add (data)); }

	Skim_Group * New_Record (bool clear = false, int number = 1)
	                                     { return ((Skim_Group *) Data_Pointer_Array::New_Record (clear, number)); }

	Skim_Group * First (void)            { return ((Skim_Group *) Data_Pointer_Array::First ()); }
	Skim_Group * Next (void)             { return ((Skim_Group *) Data_Pointer_Array::Next ()); }

	Skim_Group * Last (void)             { return ((Skim_Group *) Data_Pointer_Array::Last ()); }
	Skim_Group * Previous (void)         { return ((Skim_Group *) Data_Pointer_Array::Previous ()); }

	Skim_Group * operator[] (int index)  { return ((Skim_Group *) Record (index)); }
};

#endif

