//*********************************************************
//	Veh_Distribution.hpp - vehicle type distribution class
//*********************************************************

#ifndef VEH_DISTRIBUTION_HPP
#define VEH_DISTRIBUTION_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Veh_Distribution class definition
//---------------------------------------------------------

class Veh_Distribution
{
public:
	Veh_Distribution (void);

	int    Type (void)                        { return (type); }
	int    Sub_Type (void)                    { return (subtype); }
	double Share (void)                       { return (share); }

	void   Type (int value)                   { type = value; }
	void   Sub_Type (int value)               { subtype = value; }
	void   Share (double value)               { share = value; }

private:
	int    type;
	int    subtype;
	double share;
};

//---------------------------------------------------------
//	Veh_Distribution_Array class definition
//---------------------------------------------------------

class Veh_Distribution_Array : public Data_Array
{
public:
	Veh_Distribution_Array (int max_records = 10);

	bool Add (Veh_Distribution *data = NULL)  { return (Data_Array::Add (data)); }
	
	Veh_Distribution * New_Record (bool clear = false, int number = 1)
	                                          { return ((Veh_Distribution *) Data_Array::New_Record (clear, number)); }

	Veh_Distribution * Record (int index)     { return ((Veh_Distribution *) Data_Array::Record (index)); }
	Veh_Distribution * Record (void)          { return ((Veh_Distribution *) Data_Array::Record ()); }

	Veh_Distribution * First (void)           { return ((Veh_Distribution *) Data_Array::First ()); }
	Veh_Distribution * Next (void)            { return ((Veh_Distribution *) Data_Array::Next ()); }

	Veh_Distribution * Last (void)            { return ((Veh_Distribution *) Data_Array::Last ()); }
	Veh_Distribution * Previous (void)        { return ((Veh_Distribution *) Data_Array::Previous ()); }

	Veh_Distribution * operator[] (int index) { return (Record (index)); }
};

#endif
