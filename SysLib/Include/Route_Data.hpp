//*********************************************************
//	Route_Data.hpp - transit route header and nodes data
//*********************************************************

#ifndef ROUTE_DATA_HPP
#define ROUTE_DATA_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Route_Data class definition
//---------------------------------------------------------

class Route_Data : public Class_Index
{
public:
	Route_Data (int id = 0);
	virtual ~Route_Data (void)                   { Clear (); }

	int  Route (void)                           { return (ID ()); }
	int  Mode (void)                            { return (mode); }
	int  Min_Time (void)                        { return (min_time); }
	int  Periods (void)                         { return (periods); }
	int  Headway (int num)                      { return (Period_Index (num) ? period [p].headway : 0); }
	int  Offset (int num)                       { return (Period_Index (num) ? period [p].offset : 0); }
	int  TTime (int num)                        { return (Period_Index (num) ? period [p].ttime : 0); }
	int  Nodes (void)                           { return (nodes); }
	int  Node (int num)                         { return (Node_Index (num) ? node [n].node : 0); }
	int  Dwell (int num)                        { return (Node_Index (num) ? node [n].dwell : 0); }
	int  Time (int num)                         { return (Node_Index (num) ? node [n].time : 0); }
	int  Speed (int num)                        { return (Node_Index (num) ? node [n].speed : 0); }

	void Route (int value)                      { ID (value); }
	void Mode (int value)                       { mode = value; }
	void Min_Time (int value)                   { min_time = value; }
	bool Periods (int value);
	void Headway (int num, int value)           { if (Period_Index (num)) period [p].headway = value; }
	void Offset (int num, int value)            { if (Period_Index (num)) period [p].offset = value; }
	void TTime (int num, int value)             { if (Period_Index (num)) period [p].ttime = value; }
	bool Nodes (int value);
	void Node (int num, int value)              { if (Node_Index (num)) node [n].node = value; }
	void Dwell (int num, int value)             { if (Node_Index (num)) node [n].dwell = value; }
	void Time (int num, int value)              { if (Node_Index (num)) node [n].time = value; }
	void Speed (int num, int value)             { if (Node_Index (num)) node [n].speed = value; }


	void   Null_Name (void)                     { name = NULL; }
	const char * Name (void)                    { return ((name != NULL) ? name : ""); }
	bool   Name (char *value);
	
	void   Null_Notes (void)                    { notes = NULL; }
	const char * Notes (void)                   { return ((notes != NULL) ? notes : ""); }
	bool   Notes (char *value);

	void Clear (void);
	void Reset (void)                           { nodes = 0; periods = 0; }

protected:

	bool Node_Index (int num)                   { n = num-1; return (n >= 0 && n < nodes); }
	bool Period_Index (int num)                 { p = num-1; return (p >= 0 && p < periods); }

	static int  n, p;

private:
	typedef struct {
		int headway;		//---- seconds ----
		int offset;			//---- seconds ----
		int ttime;			//---- seconds ----
	} Period_Data;

	typedef struct {
		int node;
		int dwell;			//---- seconds ----
		int time;			//---- seconds ----
		int speed;			//---- rounded mps ----
	} Node_Data;

	int mode;
	int min_time;			//---- seconds ----
	int periods;
	int nodes;

	Period_Data *period;
	Node_Data *node;
	
	char * name;
	char * notes;
};

//---------------------------------------------------------
//	Route_Array class definition
//---------------------------------------------------------

class Route_Array : public Class_Array
{
public:
	Route_Array (int max_records = 0);
	virtual ~Route_Array (void);

	bool Add (Route_Data *data = NULL)    { return (Class_Array::Add (data)); }
	
	Route_Data * New_Record (bool clear = false, int number = 1)
	                                      { return ((Route_Data *) Class_Array::New_Record (clear, number)); }

	Route_Data * Record (int index)       { return ((Route_Data *) Class_Array::Record (index)); }
	Route_Data * Record (void)            { return ((Route_Data *) Class_Array::Record ()); }

	Route_Data * Get (int id)             { return ((Route_Data *) Class_Array::Get (id)); }
	Route_Data * Get_GE (int id)          { return ((Route_Data *) Class_Array::Get_GE (id)); }
	Route_Data * Get_LE (int id)          { return ((Route_Data *) Class_Array::Get_LE (id)); }

	Route_Data * First (void)             { return ((Route_Data *) Class_Array::First ()); }
	Route_Data * Next (void)              { return ((Route_Data *) Class_Array::Next ()); }
	
	Route_Data * Last (void)              { return ((Route_Data *) Class_Array::Last ()); }
	Route_Data * Previous (void)          { return ((Route_Data *) Class_Array::Previous ()); }

	Route_Data * First_Key (void)         { return ((Route_Data *) Class_Array::First_Key ()); }
	Route_Data * Next_Key (void)          { return ((Route_Data *) Class_Array::Next_Key ()); }

	Route_Data * Last_Key (void)          { return ((Route_Data *) Class_Array::Last_Key ()); }
	Route_Data * Previous_Key (void)      { return ((Route_Data *) Class_Array::Previous_Key ()); }

	Route_Data * operator[] (int index)   { return (Record (index)); }

	int  Num_Periods (void)               { return (periods); }
	int  Num_Nodes (void)                 { return (nodes); }  

	void Num_Periods (int value)          { periods = value; }
	void Num_Nodes (int value)            { nodes = value; }

private:
	int periods;
	int nodes;
};

#endif
