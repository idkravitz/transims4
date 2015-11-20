//*********************************************************
//	Zone_Location.hpp - zone location data class
//*********************************************************

#ifndef ZONE_LOCATION_HPP
#define ZONE_LOCATION_HPP

//#include "Index_Array.hpp"
#include "Class_Index.hpp"
#include "Class_Array.hpp"

//---------------------------------------------------------
//	Zone_Location class definition
//---------------------------------------------------------

class Zone_Location : public Class_Index
{
public:
	Zone_Location (int zone = 0);

	int   Zone (void)             { return (ID ()); }
	int   Location (void)         { return (location); }

	void  Zone (int value)        { ID (value); }
	void  Location (int value)    { location = value; }

private:
	int location;
};

////---------------------------------------------------------
////	Zone_Location structure
////---------------------------------------------------------
//
//typedef struct {
//	int   zone;
//	int   location;
//} Zone_Location;

//---------------------------------------------------------
//	Zone_Location_Array class definition
//---------------------------------------------------------

class Zone_Location_Array : public Class_Array
{
public:
	Zone_Location_Array (int max_records = 0);

	bool Add (Zone_Location *data = NULL)  { return (Class_Array::Add (data)); }
	
	Zone_Location * New_Record (bool clear = false, int number = 1)
	                                       { return ((Zone_Location *) Class_Array::New_Record (clear, number)); }

	Zone_Location * Get (int zone)         { return ((Zone_Location *) Class_Array::Get (zone)); }

	Zone_Location * Record (int index)     { return ((Zone_Location *) Class_Array::Record (index)); }
	Zone_Location * Record (void)          { return ((Zone_Location *) Class_Array::Record ()); }

	Zone_Location * First (void)           { return ((Zone_Location *) Class_Array::First ()); }
	Zone_Location * Next (void)            { return ((Zone_Location *) Class_Array::Next ()); }
	
	Zone_Location * First_Key (void)       { return ((Zone_Location *) Class_Array::First_Key ()); }
	Zone_Location * Next_Key (void)        { return ((Zone_Location *) Class_Array::Next_Key ()); }

	Zone_Location * operator[] (int index) { return (Record (index)); }
};

#endif
