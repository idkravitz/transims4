//*********************************************************
//	Transfer_Data.hpp - transfers at transit stops
//*********************************************************

#ifndef TRANSFER_DATA_HPP
#define TRANSFER_DATA_HPP

#include "Stop_Data.hpp"

//---------------------------------------------------------
//	Transfer_Data class definition
//---------------------------------------------------------

class Transfer_Data : public Stop_Data
{
public:
	Transfer_Data (int id = 0);
	virtual ~Transfer_Data (void)  {}

	int  Walk_Board (void)        { return (walk_board); }
	int  Bus_Board (void)         { return (bus_board); }
	int  Rail_Board (void)        { return (rail_board); }
	int  Drive_Board (void)       { return (drive_board); }
	int  Walk_Alight (void)       { return (walk_alight); }
	int  Bus_Alight (void)        { return (bus_alight); }
	int  Rail_Alight (void)       { return (rail_alight); }
	int  Drive_Alight (void)      { return (drive_alight); }

	void Walk_Board (int value)   { walk_board = value; }
	void Bus_Board (int value)    { bus_board = value; }
	void Rail_Board (int value)   { rail_board = value; }
	void Drive_Board (int value)  { drive_board = value; }
	void Walk_Alight (int value)  { walk_alight = value; }
	void Bus_Alight (int value)   { bus_alight = value; }
	void Rail_Alight (int value)  { rail_alight = value; }
	void Drive_Alight (int value) { drive_alight = value; }

	void Add_Walk_Board (void)    { walk_board++; }
	void Add_Bus_Board (void)     { bus_board++; }
	void Add_Rail_Board (void)    { rail_board++; }
	void Add_Drive_Board (void)   { drive_board++; }
	void Add_Walk_Alight (void)   { walk_alight++; }
	void Add_Bus_Alight (void)    { bus_alight++; }
	void Add_Rail_Alight (void)   { rail_alight++; }
	void Add_Drive_Alight (void)  { drive_alight++; }

private:
	int  walk_board;
	int  bus_board;
	int  rail_board;
	int  drive_board;
	int  walk_alight;
	int  bus_alight;
	int  rail_alight;
	int  drive_alight;
};

//---------------------------------------------------------
//	Transfer_Array class definition
//---------------------------------------------------------

class Transfer_Array : public Stop_Array
{
public:
	Transfer_Array (int max_records = 0);

	bool Add (Transfer_Data *data = NULL)    { return (Stop_Array::Add (data)); }
	
	Transfer_Data * New_Record (bool clear = false, int number = 1)
	                                         { return ((Transfer_Data *) Stop_Array::New_Record (clear, number)); }

	Transfer_Data * Record (int index)       { return ((Transfer_Data *) Stop_Array::Record (index)); }
	Transfer_Data * Record (void)            { return ((Transfer_Data *) Stop_Array::Record ()); }

	Transfer_Data * Get (int id)             { return ((Transfer_Data *) Stop_Array::Get (id)); }
	Transfer_Data * Get_GE (int id)          { return ((Transfer_Data *) Stop_Array::Get_GE (id)); }
	Transfer_Data * Get_LE (int id)          { return ((Transfer_Data *) Stop_Array::Get_LE (id)); }

	Transfer_Data * First (void)             { return ((Transfer_Data *) Stop_Array::First ()); }
	Transfer_Data * Next (void)              { return ((Transfer_Data *) Stop_Array::Next ()); }
	
	Transfer_Data * Last (void)              { return ((Transfer_Data *) Stop_Array::Last ()); }
	Transfer_Data * Previous (void)          { return ((Transfer_Data *) Stop_Array::Previous ()); }

	Transfer_Data * First_Key (void)         { return ((Transfer_Data *) Stop_Array::First_Key ()); }
	Transfer_Data * Next_Key (void)          { return ((Transfer_Data *) Stop_Array::Next_Key ()); }

	Transfer_Data * operator[] (int index)   { return (Record (index)); }
};

#endif
