//*********************************************************
//	Category_Data.hpp - category data classes
//*********************************************************

#ifndef CATEGORY_DATA_HPP
#define CATEGORY_DATA_HPP

#include "Class_Array.hpp"

#include "Value_Map.hpp"
#include "Label_Data.hpp"

//---------------------------------------------------------
//	Category_Data class definition
//---------------------------------------------------------

class Category_Data : public Class_Index
{
public:
	Category_Data (int number = 0, bool key_flag = false);

	int    Number (void)               { return (ID ()); }
	bool   Key_Flag (void)             { return (key_flag); }
	char * Name (void)                 { return (name); }
	int    Base_Index (void)           { return (base_index); }

	void   Number (int value)          { ID (value); }
	void   Key_Flag (bool flag)        { key_flag = flag; }
	void   Name (char *_name)          { str_cpy (name, sizeof (name), _name); }
	void   Base_Index (int value)      { base_index = value; }

	int    Add_Member (int value, char *string = NULL);

	int    Member (int value)          { return (label_data.Get_Index (value)); }
	int    Num_Members (void)          { return (label_data.Num_Records ()); }
	int    Max_Member (void)           { Label_Data *ptr = label_data.Last (); return ((ptr) ? ptr->Value () : 0); }

	char * First_Label (void)          { Label_Data *ptr = label_data.First (); return ((ptr) ? ptr->Label () : NULL); }
	char * Next_Label (void)           { Label_Data *ptr = label_data.Next (); return ((ptr) ? ptr->Label () : NULL); }

	Value_Map * First_Value (int value) { return (value_map.Get_GE (value, 0)); }
	Value_Map * Next_Value (void)       { return (value_map.Next_Key ()); }

	int    Max_Value (void);

	Value_Map * First_Member (int value, int group);
	Value_Map * Next_Member (int value, int group);

private:
	bool   key_flag;
	char   name [24];
	int    base_index;

	Value_Map_Array value_map;
	Label_Array label_data;
};

//---------------------------------------------------------
//	Category_Array class definition
//---------------------------------------------------------

class Category_Array : public Class_Array
{
public:
	Category_Array (int max_records = 10);

	bool Add (Category_Data *data = NULL)    { return (Class_Array::Add (data)); }

	Category_Data * Record (int index)       { return ((Category_Data *) Class_Array::Record (index)); }
	Category_Data * Record (void)            { return ((Category_Data *) Class_Array::Record ()); }
	
	Category_Data * Get (int value, bool flag = false);

	Category_Data * First (void)             { return ((Category_Data *) Class_Array::First ()); }
	Category_Data * Next (void)              { return ((Category_Data *) Class_Array::Next ()); }
	
	Category_Data * Last (void)              { return ((Category_Data *) Class_Array::Last ()); }
	Category_Data * Previous (void)          { return ((Category_Data *) Class_Array::Previous ()); }

	Category_Data * operator[] (int index)   { return (Record (index)); }
};

#endif
