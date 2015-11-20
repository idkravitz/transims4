//*********************************************************
//	ActBalance.hpp - Activity Attraction Balancing
//*********************************************************

#ifndef ACTBALANCE_HPP
#define ACTBALANCE_HPP

#include "Demand_Service.hpp"
#include "Activity_File.hpp"
#include "Data_Range.hpp"
#include "Data_Array.hpp"
#include "Db_Header.hpp"
#include "Zone_Equiv.hpp"

#define MAX_PURPOSE		100

//---------------------------------------------------------
//	ActBalance - execution class definition
//---------------------------------------------------------

class ActBalance : public Demand_Service
{
public:

	ActBalance (void);
	virtual ~ActBalance (void);

	virtual void Execute (void);
	virtual void Page_Header (void);

protected:

	static char * ACTIVITY_PURPOSE_RANGE_x;
	static char * BALANCING_TARGET_FIELD_x;
	static char * BALANCING_FACTOR_FIELD_x;
	static char * BALANCING_TARGET_FILE;
	static char * BALANCING_TARGET_FORMAT;
	static char * NEW_BALANCING_FACTOR_FILE;
	static char * NEW_BALANCING_FACTOR_FORMAT;
	static char * ZONE_EQUIVALENCE_FILE;

	virtual void Program_Control (void);

private:
	enum ActBalance_Reports { ZONE_SUM = 1, DISTRICT_SUM, ZONE_EQUIV };

	int max_purpose, max_zone, total_act, total_used;
	int purpose_map [MAX_PURPOSE];

	Db_Header target_file, factor_file;

	typedef struct {
		int group;
		int target_field;
		int factor_field;
		Data_Range *purpose;
		double *target;
		double *current;
	} Factor_Group;

	Data_Array factor_group;
	Zone_Equiv zone_equiv;

	void Read_Targets (void);
	bool Read_Activity (int file_num);
	void Write_Factors (void);

	void Zone_Report (void);
	void Zone_Header (void);

	void District_Report (void);
	void District_Header (void);
};
#endif
