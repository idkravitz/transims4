//*********************************************************
//	Route_Leg_Data.hpp - transit route leg data classes
//*********************************************************

#ifndef ROUTE_LEG_DATA_HPP
#define ROUTE_LEG_DATA_HPP

#include "Travel_Leg_Data.hpp"
#include "Driver_Data.hpp"

//---------------------------------------------------------
//	Route_Leg_Data class definition
//---------------------------------------------------------

class Route_Leg_Data : public Driver_Data
{
public:
	Route_Leg_Data (int route = 0);
	virtual ~Route_Leg_Data (void)           { if (leg != NULL) delete leg; }

	bool Num_Travel_Legs (int value)         { return ((leg = new Travel_Leg_Array (value)) != NULL); }
	int  Num_Travel_Legs (void)              { return ((leg != NULL) ? leg->Num_Records () : 0); }

	Travel_Leg_Data * Travel_Leg (int index) { return ((leg != NULL) ? leg->Record (index) : NULL); }
	bool Travel_Leg (Travel_Leg_Data *value) { return ((leg != NULL) ? leg->Add (value) : false); }

private:
	Travel_Leg_Array *leg;	
};

//---------------------------------------------------------
//	Route_Leg_Array
//---------------------------------------------------------

class Route_Leg_Array : public Driver_Array
{
public:
	Route_Leg_Array (int max_records = 0);

	bool Add (Route_Leg_Data *data = NULL)    { return (Class_Array::Add (data)); }

	Route_Leg_Data * Record (int index)       { return ((Route_Leg_Data *) Class_Array::Record (index)); }
	Route_Leg_Data * Record (void)            { return ((Route_Leg_Data *) Class_Array::Record ()); }

	Route_Leg_Data * Get (int route)          { return ((Route_Leg_Data *) Class_Array::Get (route)); }

	Route_Leg_Data * First (void)             { return ((Route_Leg_Data *) Class_Array::First ()); }
	Route_Leg_Data * Next (void)              { return ((Route_Leg_Data *) Class_Array::Next ()); }

	Route_Leg_Data * operator[] (int index)   { return (Record (index)); }
};

#endif
