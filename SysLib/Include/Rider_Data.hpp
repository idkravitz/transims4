//*********************************************************
//	Rider_Data.hpp - transit ridership data class
//*********************************************************

#ifndef RIDER_DATA_HPP
#define RIDER_DATA_HPP

#include "Line_Data.hpp"

#include "System_Defines.hpp"

//---------------------------------------------------------
//	Rider_Data class definition
//---------------------------------------------------------

class Rider_Data : public Line_Data
{
public:
	Rider_Data (void);
	virtual ~Rider_Data (void)                  { Rider_Data::Clear (); }

	virtual void Clear (void);

	int  Time (int run, int stop)               { return (Index (run, stop) ? rider [r].stop [s].time : 0); }
	int  Board (int run, int stop)              { return (Index (run, stop) ? rider [r].stop [s].on : 0); }
	int  Alight (int run, int stop)             { return (Index (run, stop) ? rider [r].stop [s].off : 0); }
	int  Load (int run)                         { return (Index (run) ? rider [r].load : 0); }
	int  On_Rider (int stop)                    { return (Index (1, stop) ? on_rider [s] : 0); }
	int  Off_Rider (int run, int stop)          { return (Index (run, stop) ? rider [r].stop [s].rider : 0); }

	void Time (int run, int stop, int time)     { if (Index (run, stop)) rider [r].stop [s].time = time; }
	void Board (int run, int stop, int num)     { if (Index (run, stop)) { rider [r].stop [s].on += num; rider [r].load += num; } }
	void Alight (int run, int stop, int num)    { if (Index (run, stop)) { rider [r].stop [s].off += num; rider [r].load -= num; } }
	void Load (int run, int value)              { if (Index (run)) rider [r].load = value; }
	void On_Rider (int stop, int id)            { if (Index (1, stop)) on_rider [s] = id; }
	void Off_Rider (int run, int stop, int id)  { if (Index (run, stop)) rider [r].stop [s].rider = id; }

	//---- overloaded methods ----

	int  Distance (int stop)                    { return (On_Rider (stop)); }
	void Distance (int stop, int value)         { On_Rider (stop, value); }

	int  Riders (int run, int stop)             { return (Off_Rider (run, stop)); }
	int  Max_Load (int run)                     { return (Load (run)); }

	void Summarize_Riders (void);
	
private:
	typedef struct {
		int time;
		int on;
		int off;
		int rider;
	} Run_Stop;

	typedef struct {
		Run_Stop *stop;
		int load;
	} Run_Rider;

	bool Check (void);
	bool Index (int run, int stop = 1)          { return (Check () && Line_Data::Index (run, stop)); }

	int *on_rider;
	Run_Rider *rider;
};

//---------------------------------------------------------
//	Rider_Array class definition
//---------------------------------------------------------

class Rider_Array : public Line_Array
{
public:
	Rider_Array (int max_records = 0);
	virtual ~Rider_Array (void);

	bool Add (Rider_Data *data = NULL)       { return (Line_Array::Add (data)); }
	
	Rider_Data * New_Record (bool clear = false, int number = 1)
	                                         { return ((Rider_Data *) Line_Array::New_Record (clear, number)); }

	Rider_Data * Get (int id)                { return ((Rider_Data *) Line_Array::Get (id)); }
	Rider_Data * Get_GE (int id)             { return ((Rider_Data *) Line_Array::Get_GE (id)); }
	Rider_Data * Get_LE (int id)             { return ((Rider_Data *) Line_Array::Get_LE (id)); }

	Rider_Data * Record (int index)          { return ((Rider_Data *) Line_Array::Record (index)); }
	Rider_Data * Record (void)               { return ((Rider_Data *) Line_Array::Record ()); }

	Rider_Data * First (void)                { return ((Rider_Data *) Line_Array::First ()); }
	Rider_Data * Next (void)                 { return ((Rider_Data *) Line_Array::Next ()); }
	
	Rider_Data * Last (void)                 { return ((Rider_Data *) Line_Array::Last ()); }
	Rider_Data * Previous (void)             { return ((Rider_Data *) Line_Array::Previous ()); }
	
	Rider_Data * First_Key (void)            { return ((Rider_Data *) Line_Array::First_Key ()); }
	Rider_Data * Next_Key (void)             { return ((Rider_Data *) Line_Array::Next_Key ()); }
	
	Rider_Data * Last_Key (void)             { return ((Rider_Data *) Line_Array::Last_Key ()); }
	Rider_Data * Previous_Key (void)         { return ((Rider_Data *) Line_Array::Previous_Key ()); }

	Rider_Data * operator[] (int index)      { return (Record (index)); }

	void Summarize_Riders (void);
};

#endif
