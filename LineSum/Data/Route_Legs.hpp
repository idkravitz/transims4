//*********************************************************
//	Route_Legs.hpp - transit leg distances and subarea flag
//*********************************************************

#ifndef ROUTE_LEGS_HPP
#define ROUTE_LEGS_HPP

#include "Data_Array.hpp"
#include "Matrix_File.hpp"
#include "Diurnal_File.hpp"
#include "Factor_File.hpp"
#include "Db_File.hpp"
#include "Time_Range.hpp"
#include "Time_Equiv.hpp"
#include "User_Program.hpp"

//---------------------------------------------------------
//	Route_Legs class definition
//---------------------------------------------------------

class Route_Legs
{
public:
	Route_Legs (void);
	virtual ~Route_Legs (void)          { Clear (); }

	bool Subarea_Flag ()                { return (subarea_flag); }
	int  Num_Legs ()                    { return (num_legs); }
	int  Percent (int leg)              { return (Check_Leg (leg) ? percent [leg-1] : 0); }
	int  Distance (int leg)             { return (Check_Leg (leg) ? distance [leg-1] : 0); } 

	void Subarea_Flag (bool flag)       { subarea_flag = flag; }
	bool Num_Legs (int legs);
	void Percent (int leg, int value)   { if (Check_Leg (leg)) percent [leg-1] = value; }
	void Distance (int leg, int value)  { if (Check_Leg (leg)) distance [leg-1] = value; }

	void Clear (void);

private:
	
	bool Check_Leg (int leg)            { return (leg > 0 && leg <= num_legs); }

	bool subarea_flag;
	int  num_legs;
	int *percent;
	int *distance;
};

//---------------------------------------------------------
//	Route_Legs_Array
//---------------------------------------------------------

class Route_Legs_Array : public Data_Array
{
public:
	Route_Legs_Array (int max_records = 0);
	virtual ~Route_Legs_Array (void);

	bool Add (Route_Legs *data = NULL)   { return (Data_Array::Add ((void *) data)); }

	Route_Legs * New_Record (bool clear = false, int number = 1)
										 { return ((Route_Legs *) Data_Array::New_Record (clear, number)); }

	Route_Legs * First (void)            { return ((Route_Legs *) Data_Array::First ()); }
	Route_Legs * Next (void)             { return ((Route_Legs *) Data_Array::Next ()); }

	Route_Legs * Last (void)             { return ((Route_Legs *) Data_Array::Last ()); }
	Route_Legs * Previous (void)         { return ((Route_Legs *) Data_Array::Previous ()); }

	Route_Legs * operator[] (int index)  { return ((Route_Legs *) Record (index)); }
};

#endif

