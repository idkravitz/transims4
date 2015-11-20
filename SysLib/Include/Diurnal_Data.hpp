//*********************************************************
//	Diurnal_Data.hpp - diurnal distribution data classes
//*********************************************************

#ifndef DIURNAL_DATA_HPP
#define DIURNAL_DATA_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Diurnal_Data class definition
//---------------------------------------------------------

class Diurnal_Data
{
public:
	Diurnal_Data (void);
	virtual ~Diurnal_Data (void)              { Clear (); }

	int    Start_Time (void)                  { return (start); }
	int    End_Time (void)                    { return (end); }
	double Share (int num = 1)                { return (Check (num) ? share [num-1] : 0.0); }

	void   Start_Time (int value)             { start = value; }
	void   End_Time (int value)               { end = value; }
	void   Share (int num, double value)      { if (Check (num)) share [num-1] = value; }
	void   Share (double value)               { if (Check (1)) *share = value; }

	void   Add_Share (int num, double value)  { if (Check (num)) share [num-1] += value; }
	void   Add_Share (double value)           { if (Check (1)) *share += value; }

	int   Num_Shares (void)                   { return (nshare); }
	bool  Num_Shares (int num);

	void  Clear (void);

protected:
	bool   Check (int num)                    { return (num > 0 && num <= nshare); }

private:
	int start;
	int end;
	int nshare;
	double *share;
};

//---------------------------------------------------------
//	Diurnal_Array class definition
//---------------------------------------------------------

class Diurnal_Array : public Data_Array
{
public:
	Diurnal_Array (int max_records = 0);

	bool Add (Diurnal_Data *data = NULL)  { return (Data_Array::Add (data)); }
	
	Diurnal_Data * New_Record (bool clear = false, int number = 1)
	                                      { return ((Diurnal_Data *) Data_Array::New_Record (clear, number)); }

	Diurnal_Data * Record (int index)     { return ((Diurnal_Data *) Data_Array::Record (index)); }
	Diurnal_Data * Record (void)          { return ((Diurnal_Data *) Data_Array::Record ()); }

	Diurnal_Data * First (void)           { return ((Diurnal_Data *) Data_Array::First ()); }
	Diurnal_Data * Next (void)            { return ((Diurnal_Data *) Data_Array::Next ()); }
	
	Diurnal_Data * Last (void)            { return ((Diurnal_Data *) Data_Array::Last ()); }
	Diurnal_Data * Previous (void)        { return ((Diurnal_Data *) Data_Array::Previous ()); }

	Diurnal_Data * operator[] (int index) { return (Record (index)); }

	virtual void Clear (void);
};
#endif

