//*********************************************************
//	Zone_Location.hpp - Process Zone Location Map Files
//*********************************************************

#ifndef ZONE_LOCATION_HPP
#define ZONE_LOCATION_HPP

#include "Integer_Array.hpp"
#include "Index_Array.hpp"
#include "Db_File.hpp"

//---------------------------------------------------------
//	Zone_Location_Map Class definition
//---------------------------------------------------------

class Zone_Location_Map : public Index_Array, public Db_File
{
public:
	Zone_Location_Map (void);
	virtual ~Zone_Location_Map (void);

	bool Read (bool report_flag);

	Integer_List * Locations (int zone);

	int  Num_Zones (void)				{ return (Index_Array::Num_Records ()); };
	int  Max_Zone (void)				{ return (max_zone); }

	int  First_Zone (void);
	int  Next_Zone (void);

private:
	int max_zone;

	typedef struct {
		int  zone;
		Integer_List *list;
	} Group_Data;
};

#endif
