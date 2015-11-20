//*********************************************************
//	Dir_Path_Data.hpp - dir data class for path building
//*********************************************************

#ifndef DIR_PATH_DATA_HPP
#define DIR_PATH_DATA_HPP

#include "Dir_Data.hpp"

//---------------------------------------------------------
//	Dir_Path_Data class definition
//---------------------------------------------------------

class Dir_Path_Data : public Dir_Data
{
public:
	Dir_Path_Data (void);

	int  From_List (void)      { return (from_list); }
	int  To_List (void)        { return (to_list); }
	int  Length (void)         { return (length); }
	int  Bnode (void)          { return (bnode); }
	int  Type (void)           { return (type); }
	int  Use (void)            { return (use); }
	int  Aoffset (void)        { return (aoffset); }
	int  Boffset (void)        { return (boffset); }

	void From_List (int value) { from_list = value; }
	void To_List (int value)   { to_list = value; }
	void Length (int value)    { length = value; }
	void Bnode (int value)     { bnode = value; }
	void Type (int value)      { type = (short) value; }
	void Use (int value)       { use = (short) value; }
	void Aoffset (int value)   { aoffset = (short) value; }
	void Boffset (int value)   { boffset = (short) value; }

	//---- overloaded method ----
	
	int  Offset (void)         { return (Cost_List ()); }
	void Offset (int value)    { Cost_List (value); }

	int  Zone (void)           { return (Capacity ()); }
	void Zone (int value)      { Capacity (value); }

private:
	int   from_list;
	int   to_list;
	int   length;
	int   bnode;
	short type;
	short use;
	short aoffset;
	short boffset;
};

//---------------------------------------------------------
//	Dir_Path_Array class definition
//---------------------------------------------------------

class Dir_Path_Array : public Dir_Array
{
public:
	Dir_Path_Array (int max_records = 0);
	virtual ~Dir_Path_Array (void)            { Clear (); }

	bool Add (Dir_Path_Data *data = NULL)     { return (Dir_Array::Add (data)); }
	
	Dir_Path_Data * New_Record (bool clear = false, int number = 1)
	                                          { return ((Dir_Path_Data *) Dir_Array::New_Record (clear, number)); }

	Dir_Path_Data * Get (int ID)              { return ((Dir_Path_Data *) Dir_Array::Get (ID)); }
	Dir_Path_Data * Get_GE (int ID)           { return ((Dir_Path_Data *) Dir_Array::Get_GE (ID)); }
	Dir_Path_Data * Get_LE (int ID)           { return ((Dir_Path_Data *) Dir_Array::Get_LE (ID)); }

	Dir_Path_Data * Record (int index)        { return ((Dir_Path_Data *) Dir_Array::Record (index)); }
	Dir_Path_Data * Record (void)             { return ((Dir_Path_Data *) Dir_Array::Record ()); }

	Dir_Path_Data * First (void)              { return ((Dir_Path_Data *) Dir_Array::First ()); }
	Dir_Path_Data * Next (void)               { return ((Dir_Path_Data *) Dir_Array::Next ()); }
	
	Dir_Path_Data * Last (void)               { return ((Dir_Path_Data *) Dir_Array::Last ()); }
	Dir_Path_Data * Previous (void)           { return ((Dir_Path_Data *) Dir_Array::Previous ()); }
	
	Dir_Path_Data * First_Key (void)          { return ((Dir_Path_Data *) Dir_Array::First_Key ()); }
	Dir_Path_Data * Next_Key (void)           { return ((Dir_Path_Data *) Dir_Array::Next_Key ()); }

	Dir_Path_Data * operator[] (int index)    { return (Record (index)); }
};

#endif
