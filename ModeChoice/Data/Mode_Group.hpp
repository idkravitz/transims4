//*********************************************************
//	Mode_Group.hpp - mode database parameters
//*********************************************************

#ifndef Mode_GROUP_HPP
#define Mode_GROUP_HPP

#include "Static_Service.hpp"
#include "Pointer_Array.hpp"
#include "Db_Base.hpp"

//---------------------------------------------------------
//	Mode_Group class definition
//---------------------------------------------------------

class Mode_Group : public Static_Service
{
public:
	Mode_Group (int group = 0);

	int   Group ()               { return (group); }
	void  Group (int value)      { group = value; }
	
	int   Mode ()                { return (mode); }
	void  Mode (int value)       { mode = value; }

	char *Label ()               { return (label); }
	void  Label (char *value); 

	bool  Add_Field (char *name, Db_Code::Field_Type type, int size, int decimal);
	
	Db_Base * Data (void)        { return (&data); }

	double Share (void);

private:
	int  group, mode, field;
	char label [20];
	Db_Base data;
};

//---------------------------------------------------------
//	Mode_Group_Array
//---------------------------------------------------------

class Mode_Group_Array : public Data_Pointer_Array
{
public:
	Mode_Group_Array (int max_records = 0);

	void Delete_Pointers (void);

	bool Add (Mode_Group *data = NULL)   { return (Data_Pointer_Array::Add (data)); }

	Mode_Group * New_Record (bool clear = false, int number = 1)
	                                     { return ((Mode_Group *) Data_Pointer_Array::New_Record (clear, number)); }

	Mode_Group * First (void)            { return ((Mode_Group *) Data_Pointer_Array::First ()); }
	Mode_Group * Next (void)             { return ((Mode_Group *) Data_Pointer_Array::Next ()); }

	Mode_Group * Last (void)             { return ((Mode_Group *) Data_Pointer_Array::Last ()); }
	Mode_Group * Previous (void)         { return ((Mode_Group *) Data_Pointer_Array::Previous ()); }

	Mode_Group * operator[] (int index)  { return ((Mode_Group *) Record (index)); }
};

#endif

