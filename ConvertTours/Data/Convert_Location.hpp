//*********************************************************
//	Convert_Location.hpp - additional location data
//*********************************************************

#ifndef CONVERT_LOCATION_HPP
#define CONVERT_LOCATION_HPP

#include "Location_Data.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Convert_Location class definition
//---------------------------------------------------------

class Convert_Location : public Location_Data
{
public:
	Convert_Location (int id = 0);
	virtual ~Convert_Location (void);

	int    Num_Weight (void)            { return (num_weight); }
	void   Num_Weight (int num)         { num_weight = num; }
	bool   Initialize (void);

	int    Zone_List (void)             { return (zone_list); }
	int    Org_Parking (void)           { return (org_parking); }
	int    Des_Parking (void)           { return (des_parking); }
	double Weight (int num)             { return (Check_Index (num) ? weight [num-1] : 1.0); }

	void Zone_List (int value)          { zone_list = value; }
	void Org_Parking (int value)        { org_parking = value; }
	void Des_Parking (int value)        { des_parking = value; }
	void Weight (int num, double value) { if (Check_Index (num)) weight [num-1] = value; }

private:
	int    zone_list;
	int    org_parking;
	int    des_parking;
	double *weight;

	bool Check_Index (int num)          { return (weight != NULL && num > 0 && num <= num_weight); }

	static int num_weight;
};

//---------------------------------------------------------
//	Convert_Location_Array
//---------------------------------------------------------

class Convert_Location_Array : public Class_Array
{
public:
	Convert_Location_Array (int max_records = 0);

	bool Add (Convert_Location *data = NULL)    { return (Class_Array::Add (data)); }

	Convert_Location * New_Record (bool clear = false, int number = 1)
											    { return ((Convert_Location *) Class_Array::New_Record (clear, number)); }

	Convert_Location * Get (int id)             { return ((Convert_Location *) Class_Array::Get (id)); } 

	Convert_Location * First (void)             { return ((Convert_Location *) Class_Array::First ()); }
	Convert_Location * Next (void)              { return ((Convert_Location *) Class_Array::Next ()); }

	Convert_Location * operator[] (int index)   { return ((Convert_Location *) Record (index)); }
};

#endif

