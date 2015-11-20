//*********************************************************
//	Connect_Data.hpp - network lane connectivity data
//*********************************************************

#ifndef CONNECT_DATA_HPP
#define CONNECT_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Connect_Data class definition
//---------------------------------------------------------

class Connect_Data : public Class2_Index
{
public:
	Connect_Data (int in_link_dir = 0, int out_link_dir = 0);
	virtual ~Connect_Data (void)    {}

	int  In_Link_Dir (void)         { return (Key1 ()); }
	int  In_Link (void)             { return ((Key1 () >> 1)); }
	int  In_Dir (void)              { return ((Key1 () & 1)); } 
	int  Out_Link_Dir (void)        { return (Key2 ()); }
	int  Out_Link (void)            { return ((Key2 () >> 1)); }
	int  Out_Dir (void)             { return ((Key2 () & 1)); } 
	int  In_Lane_Low (void)         { return (in_lane_low); }
	int  In_Lane_High (void)        { return (in_lane_high); }
	int  Out_Lane_Low (void)        { return (out_lane_low); }
	int  Out_Lane_High (void)       { return (out_lane_high); }
	int  In_Thru_Low (void)         { return (in_thru_low); } 
	int  In_Thru_High (void)        { return (in_thru_high); }
	int  Out_Thru_Low (void)        { return (out_thru_low); }
	int  Out_Thru_High (void)       { return (out_thru_high); }
	int  Type (void)                { return (type); }
	int  Penalty (void)             { return (penalty); }
	int  Speed (void)               { return (speed); }
	int  Capacity (void)            { return (capacity); }

	void In_Link_Dir (int value)    { Key1 (value); }
	void In_Link (int value)        { Key1 ((value << 1) + In_Dir ()); }
	void In_Dir (int value)         { Key1 ((In_Link () << 1) + value); }
	void Out_Link_Dir (int value)   { Key2 (value); }
	void Out_Link (int value)       { Key2 ((value << 1) + Out_Dir ()); }
	void Out_Dir (int value)        { Key2 ((Out_Link () << 1) + value); }
	void In_Lane_Low (int value)    { in_lane_low = (char) value; }
	void In_Lane_High (int value)   { in_lane_high = (char) value; }
	void Out_Lane_Low (int value)   { out_lane_low = (char) value; }
	void Out_Lane_High (int value)  { out_lane_high = (char) value; }
	void In_Thru_Low (int value)    { in_thru_low = (char) value; }
	void In_Thru_High (int value)   { in_thru_high = (char) value; }
	void Out_Thru_Low (int value)   { out_thru_low = (char) value; }
	void Out_Thru_High (int value)  { out_thru_high = (char) value; }
	void Type (int value)           { type = (short) value; }
	void Penalty (int value)        { penalty = (short) value; }
	void Speed (int value)          { speed = (short) value; }
	void Capacity (int value)       { capacity = (short) value; }

private:
	char  in_lane_low;
	char  in_lane_high;
	char  out_lane_low;
	char  out_lane_high;
	char  in_thru_low;
	char  in_thru_high;
	char  out_thru_low;
	char  out_thru_high;
	short type;
	short penalty;
	short speed;			//---- rounded ----
	short capacity;
};

//---------------------------------------------------------
//	Connect_Array class definition
//---------------------------------------------------------

class Connect_Array : public Class2_Array
{
public:
	Connect_Array (int max_records = 0);

	bool Add (Connect_Data *data = NULL)       { return (Class2_Array::Add (data)); }
	
	Connect_Data * New_Record (bool clear = false, int number = 1)
	                                           { return ((Connect_Data *) Class2_Array::New_Record (clear, number)); }

	Connect_Data * Record (int index)          { return ((Connect_Data *) Class2_Array::Record (index)); }
	Connect_Data * Record (void)               { return ((Connect_Data *) Class2_Array::Record ()); }

	Connect_Data * Get (int key1, int key2)    { return ((Connect_Data *) Class2_Array::Get (key1, key2)); }
	Connect_Data * Get (Connect_Data *data)    { return ((Connect_Data *) Class2_Array::Get (data)); }

	Connect_Data * Get_GE (int key1, int key2) { return ((Connect_Data *) Class2_Array::Get_GE (key1, key2)); }
	Connect_Data * Get_GE (Connect_Data *data) { return ((Connect_Data *) Class2_Array::Get_GE (data)); }

	Connect_Data * Get_LE (int key1, int key2) { return ((Connect_Data *) Class2_Array::Get_LE (key1, key2)); }
	Connect_Data * Get_LE (Connect_Data *data) { return ((Connect_Data *) Class2_Array::Get_LE (data)); }

	Connect_Data * First (void)                { return ((Connect_Data *) Class2_Array::First ()); }
	Connect_Data * Next (void)                 { return ((Connect_Data *) Class2_Array::Next ()); }
	
	Connect_Data * Last (void)                 { return ((Connect_Data *) Class2_Array::Last ()); }
	Connect_Data * Previous (void)             { return ((Connect_Data *) Class2_Array::Previous ()); }

	Connect_Data * First_Key (void)            { return ((Connect_Data *) Class2_Array::First_Key ()); }
	Connect_Data * Next_Key (void)             { return ((Connect_Data *) Class2_Array::Next_Key ()); }

	Connect_Data * operator[] (int index)      { return (Record (index)); }
};

#endif
