//*********************************************************
//	Zone_Equiv.hpp - Process Zone Equivalence Files
//*********************************************************

#ifndef ZONE_EQUIV_HPP
#define ZONE_EQUIV_HPP

#include "Index_Array.hpp"
#include "Db_File.hpp"

//---------------------------------------------------------
//	Zone_Equiv Class definition
//---------------------------------------------------------

class Zone_Equiv : public Index_Array, public Db_File
{
public:
	Zone_Equiv (void);
	virtual ~Zone_Equiv (void);

	bool Read (bool report_flag);

	char * Group_Label (int group);
	int Zone_Group (int zone);

	int  Num_Groups (void)			{ return (Index_Array::Num_Records ()); };
	int  Num_Warning (void)			{ return (num_warning); };
	int  Num_Missing (void)			{ return (num_missing); };

private:
	typedef struct {
		int  group;
		char *label;
	} Group_Data;

	typedef struct {
		int zone;
		int group;
	} Zone_Map;

	int num_zone, num_warning, num_missing;

	Index_Array zone_group;
};

#endif
