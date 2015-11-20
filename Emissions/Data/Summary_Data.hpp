//*********************************************************
//	Summary_Data.hpp - emissions summary data classes
//*********************************************************

#ifndef SUMMARY_DATA_HPP
#define SUMMARY_DATA_HPP

#include "Class_Array.hpp"

//---------------------------------------------------------
//	Summary_Data class definition
//---------------------------------------------------------

class Summary_Data : public Class_Index
{
public:
	Summary_Data (int num_pollutants = 0);

	int    Area_Type (void)                       { return (ID () >> 24); }
	int    Facility_Type (void)                   { return ((ID () & 0x00FF0000) >> 16); }
	int    Road_Type (void)                       { return ((ID () & 0x0000FF00) >> 8); }
	int    Vehicle_Type (void)                    { return (ID () & 0x000000FF); }

	double Meters (void)                          { return (meters); }
	double Seconds (void)                         { return (seconds); }
	double Emissions (int type)                   { return (Check (type) ? emissions [type-1] : 0.0); }

	void   Area_Type (int value)                  { ID ((ID () & 0x00FFFFFF) + (value << 24)); }
	void   Facility_Type (int value)              { ID ((ID () & 0xFF00FFFF) + (value << 16)); }
	void   Road_Type (int value)                  { ID ((ID () & 0xFFFF00FF) + (value << 8)); }
	void   Vehicle_Type (int value)               { ID ((ID () & 0xFFFFFF00) + value); }    

	void   Meters (double value)                  { meters = value; }
	void   Seconds (double value)                 { seconds = value; }
	void   Emissions (int type, double value)     { if (Check (type)) emissions [type-1] = value; }

	void   Add_Meters (double value)              { meters += value; }
	void   Add_Seconds (double value)             { seconds += value; }
	void   Add_Emissions (int type, double value) { if (Check (type)) emissions [type-1] += value; }

	bool   Pollutants (int num);
	int    Pollutants (void)                      { return (pollutants); }

	void   Sum (Summary_Data *data);
	void   Zero (Summary_Data *data = NULL);
	void   Clear (void)                           { if (emissions) delete [] emissions; }

private:
	int pollutants;
	double meters, seconds, *emissions;

	bool Check (int num)                          { return (emissions && num > 0 && num <= pollutants); }
};

//---------------------------------------------------------
//	Summary_Array class definition
//---------------------------------------------------------

class Summary_Array : public Class_Array
{
public:
	Summary_Array (int num_pollutants = 0, int max_records = 0);
	virtual ~Summary_Array (void);

	bool Add (Summary_Data *data = NULL)    { return (Class_Array::Add (data)); }

	Summary_Data * Record (int index)       { return ((Summary_Data *) Class_Array::Record (index)); }
	Summary_Data * Record (void)            { return ((Summary_Data *) Class_Array::Record ()); }

	Summary_Data * Get (int value)          { return ((Summary_Data *) Class_Array::Get (value)); }
	Summary_Data * Get (Summary_Data *data) { return ((Summary_Data *) Class_Array::Get (data)); }

	Summary_Data * First (void)             { return ((Summary_Data *) Class_Array::First ()); }
	Summary_Data * Next (void)              { return ((Summary_Data *) Class_Array::Next ()); }
	
	Summary_Data * First_Key (void)         { return ((Summary_Data *) Class_Array::First_Key ()); }
	Summary_Data * Next_Key (void)          { return ((Summary_Data *) Class_Array::Next_Key ()); }

	Summary_Data * operator[] (int index)   { return (Record (index)); }

	int  Pollutants (void)                  { return (pollutants); }
	void Pollutants (int num)               { pollutants = num; }

private:
	int pollutants;
};

#endif
