//*********************************************************
//	Link_Event.hpp - Link Event Time Period data
//*********************************************************

#ifndef LINK_EVENT_HPP
#define LINK_EVENT_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Link_Event class definition
//---------------------------------------------------------

class Link_Event : public Class2_Index
{
public:
	Link_Event (int link = 0);

	int  Link (void)                   { return (Key1 ()); }
	int  Period (void)                 { return (Key2 ()); }
	int  Plan_Start (void)             { return (plan_start); }
	int  Started (void)                { return (started); }
	int  Start_Diff (void)             { return (start_diff); }
	int  Plan_End (void)               { return (plan_end); }
	int  Ended (void)                  { return (ended); }
	int  End_Diff (void)               { return (end_diff); }

	void Link (int value)              { Key1 (value); }
	void Period (int value)            { Key2 (value); }
	void Plan_Start (int value)        { plan_start = value; }
	void Started (int value)           { started = value; }
	void Start_Diff (int value)        { start_diff = value; }
	void Plan_End (int value)          { plan_end = value; }
	void Ended (int value)             { ended = value; }
	void End_Diff (int value)          { end_diff = value; }
	
	void Add_Plan_Start (void)         { plan_start++; }
	void Add_Start_Diff (int value)    { started++; start_diff += value; }
	void Add_Plan_End (void)           { plan_end++; }
	void Add_End_Diff (int value)      { ended++; end_diff += value; }

private:
	int  plan_start;
	int  started;
	int  start_diff;
	int  plan_end;
	int  ended;
	int  end_diff;
};

//---------------------------------------------------------
//	Link_Event_Array class definition
//---------------------------------------------------------

class Link_Event_Array : public Class2_Array
{
public:
	Link_Event_Array (int max_records = 0);

	bool Add (Link_Event *data = NULL)     { return (Class2_Array::Add (data)); }
	
	Link_Event * New_Record (bool clear = false, int number = 1)
	                                       { return ((Link_Event *) Class2_Array::New_Record (clear, number)); }

	Link_Event * Get (Link_Event *data)    { return ((Link_Event *) Class2_Array::Get (data)); }

	Link_Event * Record (int index)        { return ((Link_Event *) Class2_Array::Record (index)); }
	Link_Event * Record (void)             { return ((Link_Event *) Class2_Array::Record ()); }

	Link_Event * First (void)              { return ((Link_Event *) Class2_Array::First ()); }
	Link_Event * Next (void)               { return ((Link_Event *) Class2_Array::Next ()); }
	
	Link_Event * First_Key (void)          { return ((Link_Event *) Class2_Array::First_Key ()); }
	Link_Event * Next_Key (void)           { return ((Link_Event *) Class2_Array::Next_Key ()); }

	Link_Event * operator[] (int index)    { return (Record (index)); }
};

#endif
