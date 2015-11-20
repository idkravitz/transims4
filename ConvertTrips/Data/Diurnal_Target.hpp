//*********************************************************
//	Diurnal_Target.hpp - diurnal target data class
//*********************************************************

#ifndef DIURNAL_TARGET_HPP
#define DIURNAL_TARGET_HPP

#include "Diurnal_Data.hpp"

//---------------------------------------------------------
//	Diurnal_Target class definition
//---------------------------------------------------------

class Diurnal_Target : public Diurnal_Data
{
public:
	Diurnal_Target (void);
	virtual ~Diurnal_Target (void)            { Clear (); }

	double Target (int num = 1)               { return (Check (num) ? target [num-1] : 0.0); }
	int    Trips (int num = 1)                { return (Check (num) ? trips [num-1] : 0); }

	void   Target (int num, double value)     { if (Check (num)) target [num-1] = value; }
	void   Target (double value)              { if (Check (1)) *target = value; }
	void   Trips (int num, int value)         { if (Check (num)) trips [num-1] = value; }

	void   Add_Target (int num, double value) { if (Check (num)) target [num-1] += value; }
	void   Add_Target (double value)          { if (Check (1)) *target += value; }
	void   Add_Trips (int num, int value = 1) { if (Check (num)) trips [num-1] += value; }

	int    Num_Shares (void)                  { return (Diurnal_Data::Num_Shares ()); }
	bool   Num_Shares (int num);

	void   Clear (void);

private:
	double *target;
	int *trips;
};

//---------------------------------------------------------
//	Diurnal_Target_Array class definition
//---------------------------------------------------------

class Diurnal_Target_Array : public Diurnal_Array
{
public:
	Diurnal_Target_Array (int max_records = 0);

	bool Add (Diurnal_Target *data = NULL)  { return (Diurnal_Array::Add (data)); }
	
	Diurnal_Target * New_Record (bool clear = false, int number = 1)
	                                      { return ((Diurnal_Target *) Diurnal_Array::New_Record (clear, number)); }

	Diurnal_Target * Record (int index)     { return ((Diurnal_Target *) Diurnal_Array::Record (index)); }
	Diurnal_Target * Record (void)          { return ((Diurnal_Target *) Diurnal_Array::Record ()); }

	Diurnal_Target * First (void)           { return ((Diurnal_Target *) Diurnal_Array::First ()); }
	Diurnal_Target * Next (void)            { return ((Diurnal_Target *) Diurnal_Array::Next ()); }
	
	Diurnal_Target * Last (void)            { return ((Diurnal_Target *) Diurnal_Array::Last ()); }
	Diurnal_Target * Previous (void)        { return ((Diurnal_Target *) Diurnal_Array::Previous ()); }

	Diurnal_Target * operator[] (int index) { return (Record (index)); }

	virtual void Clear (void);
};
#endif

