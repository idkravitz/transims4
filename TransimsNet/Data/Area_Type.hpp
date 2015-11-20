//*********************************************************
//	Area_Type.hpp - area type data class
//*********************************************************

#ifndef AREA_TYPE_HPP
#define AREA_TYPE_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Area_Type class definition
//---------------------------------------------------------

class Area_Type : public Class_Index
{
public:
	Area_Type (int type = 0);

	int  Type (void)               { return (ID ()); }
	int  Primary (void)            { return (primary); }
	int  Secondary (void)          { return (secondary); }
	int  Signal_Type (void)        { return (signal_type); }
	int  Ring_Type (void)          { return (ring_type); }
	int  Split_Len (void)          { return (split_len); }    
	int  All_Stop (void)           { return (all_stop); }

	void Type (int value)          { ID (value); }
	void Primary (int value)       { primary = value; }
	void Secondary (int value)     { secondary = value; }
	void Signal_Type (int value)   { signal_type = value; }
	void Ring_Type (int value)     { ring_type = value; }
	void Split_Len (int value)     { split_len = value; }
	void All_Stop (int value)      { all_stop = value; }

private:
	int primary;
	int secondary;
	int signal_type;
	int ring_type;
	int split_len;
	int all_stop;
};

//---------------------------------------------------------
//	Area_Type_Array class definition
//---------------------------------------------------------

class Area_Type_Array : public Class_Array
{
public:
	Area_Type_Array (int max_records = 0);
	virtual ~Area_Type_Array (void)       { Clear (); }

	bool Add (Area_Type *data = NULL)     { return (Class_Array::Add (data)); }
	
	Area_Type * New_Record (bool clear = false, int number = 1)
	                                      { return ((Area_Type *) Class_Array::New_Record (clear, number)); }

	Area_Type * Get (int type)            { return ((Area_Type *) Class_Array::Get (type)); }
	Area_Type * Get_GE (int type)         { return ((Area_Type *) Class_Array::Get_GE (type)); }
	Area_Type * Get_LE (int type)         { return ((Area_Type *) Class_Array::Get_LE (type)); }
	
	Area_Type * Best (int type)           { return ((Area_Type *) Class_Array::Best (type)); }

	Area_Type * Record (int index)        { return ((Area_Type *) Class_Array::Record (index)); }
	Area_Type * Record (void)             { return ((Area_Type *) Class_Array::Record ()); }

	Area_Type * First (void)              { return ((Area_Type *) Class_Array::First ()); }
	Area_Type * Next (void)               { return ((Area_Type *) Class_Array::Next ()); }
	
	Area_Type * Last (void)               { return ((Area_Type *) Class_Array::Last ()); }
	Area_Type * Previous (void)           { return ((Area_Type *) Class_Array::Previous ()); }
	
	Area_Type * First_Key (void)          { return ((Area_Type *) Class_Array::First_Key ()); }
	Area_Type * Next_Key (void)           { return ((Area_Type *) Class_Array::Next_Key ()); }

	Area_Type * operator[] (int index)    { return (Record (index)); }
};

#endif
