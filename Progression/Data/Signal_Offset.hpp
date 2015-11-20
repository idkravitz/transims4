//*********************************************************
//	Signal_Offset.hpp - progression signal offset class
//*********************************************************

#ifndef SIGNAL_OFFSET_HPP
#define SIGNAL_OFFSET_HPP

#include "Class_Pointer.hpp"
#include "Integer_Array.hpp"
#include "Green_Phase.hpp"

//---------------------------------------------------------
//	Signal_Offset class definition
//---------------------------------------------------------

class Signal_Offset : public Class_Index
{
public:
	Signal_Offset (int signal = 0);
	virtual ~Signal_Offset (void)             { Clear (); }

	int  Signal (void)                        { return (ID ()); }
	int  Period (void)                        { return (period); }
	int  Preset (void)                        { return (preset); }
	int  Input (void)                         { return (input); }
	int  Offset (void)                        { return (offset); }
	int  Cycle (void)                         { return (cycle); }

	void Signal (int value)                   { ID (value); }
	void Period (int value)                   { period = value; }
	void Preset (int value)                   { preset = value; }
	void Input (int value)                    { input = value; }
	void Offset (int value)                   { offset = value; }
	void Cycle (int value)                    { cycle = value; }

	int  First_Group (void)                   { return (group.First ()); }
	int  Next_Group (void)                    { return (group.Next ()); }
	bool Add_Group (int num)                  { return (group.Add (num)); }
	void Reset_Group (void)                   { return (group.Reset ()); }

	Green_Phase * First_Phase (void)          { return (phase.First ()); }
	Green_Phase * Next_Phase (void)           { return (phase.Next ()); }
	Green_Phase * Get_Phase (int in, int out) { return (phase.Get (in, out)); }
	bool Add_Phase (Green_Phase *ptr)         { return (phase.Add (ptr)); }
	
	void Clear (void)                         { group.Clear (); phase.Clear (); }

private:
	int period;
	int preset;
	int input;
	int offset;
	int cycle;
	Integer_List group;
	Green_Phase_Array phase;
};

//---------------------------------------------------------
//	Signal_Offset_Array class definition
//---------------------------------------------------------

class Signal_Offset_Array : public Class_Pointer
{
public:
	Signal_Offset_Array (int max_records = 0);
	virtual ~Signal_Offset_Array (void)     { Clear (); }

	bool Add (Signal_Offset *data)          { return (Class_Pointer::Add (data)); }

	Signal_Offset * Get (int signal)        { return ((Signal_Offset *) Class_Pointer::Get (signal)); }

	Signal_Offset * Record (int index)      { return ((Signal_Offset *) Class_Pointer::Record (index)); }
	Signal_Offset * Record (void)           { return ((Signal_Offset *) Class_Pointer::Record ()); }

	Signal_Offset * First (void)            { return ((Signal_Offset *) Class_Pointer::First ()); }
	Signal_Offset * Next (void)             { return ((Signal_Offset *) Class_Pointer::Next ()); }

	Signal_Offset * First_Key (void)        { return ((Signal_Offset *) Class_Pointer::First_Key ()); }
	Signal_Offset * Next_Key (void)         { return ((Signal_Offset *) Class_Pointer::Next_Key ()); }

	Signal_Offset * operator[] (int index)  { return (Record (index)); }

	virtual void Clear (void);
	virtual void Reset (void);
};

#endif
