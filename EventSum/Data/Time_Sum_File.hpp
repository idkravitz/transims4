//******************************************************** 
//	Time_Sum_File.hpp - Time Summary Input/Output
//********************************************************

#ifndef TIME_SUM_FILE_HPP
#define TIME_SUM_FILE_HPP

#include "Db_Header.hpp"

//---------------------------------------------------------
//	Time_Sum_File Class definition
//---------------------------------------------------------

class  Time_Sum_File : public Db_Header
{
public:

	Time_Sum_File (Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	Time_Sum_File (char *filename, Access_Type access = READ, Format_Type format = DEFAULT_FORMAT);
	virtual ~Time_Sum_File (void);

	char * From_Time (void)				{ Get_Field (from_time, svalue); return (svalue); }
	char * To_Time (void)				{ Get_Field (to_time, svalue); return (svalue); }
	int Trip_Start (void)				{ Get_Field (trip_start, &lvalue); return (lvalue); }
	int Started (void)					{ Get_Field (started, &lvalue); return (lvalue); }
	char * Start_Diff (void)			{ Get_Field (start_diff, svalue); return (svalue); }
	char * Start_Error (void)			{ Get_Field (start_abs, svalue); return (svalue); }
	int Trip_End (void)					{ Get_Field (trip_end, &lvalue); return (lvalue); }
	int Ended (void)					{ Get_Field (ended, &lvalue); return (lvalue); }
	char * End_Diff (void)				{ Get_Field (end_diff, svalue); return (svalue); }
	char * End_Error (void)				{ Get_Field (end_abs, svalue); return (svalue); }
	int Mid_Trip (void)					{ Get_Field (mid_trip, &lvalue); return (lvalue); }
	char * Travel_Time (void)			{ Get_Field (ttime, svalue); return (svalue); }
	char * TTime_Diff (void)			{ Get_Field (ttime_diff, svalue); return (svalue); }
	char * TTime_Error (void)			{ Get_Field (ttime_abs, svalue); return (svalue); }
	int  Sum_Trips (void)               { Get_Field (sum_trip, &lvalue); return (lvalue); }
	char * Sum_Error (void)             { Get_Field (sum_error, svalue); return (svalue); }
	char * Avg_Error (void)             { Get_Field (avg_error, svalue); return (svalue); }
	
	void From_Time (char *value)		{ Put_Field (from_time, value); }
	void To_Time (char *value)			{ Put_Field (to_time, value); }
	void Trip_Start (int value)			{ Put_Field (trip_start, value); }
	void Started (int value)			{ Put_Field (started, value); }
	void Start_Diff (char *value)		{ Put_Field (start_diff, value); }
	void Start_Error (char *value)		{ Put_Field (start_abs, value); }
	void Trip_End (int value)			{ Put_Field (trip_end, value); }
	void Ended (int value)				{ Put_Field (ended, value); }
	void End_Diff (char *value)			{ Put_Field (end_diff, value); }
	void End_Error (char *value)		{ Put_Field (end_abs, value); }
	void Mid_Trip (int value)			{ Put_Field (mid_trip, value); }
	void Travel_Time (char *value)		{ Put_Field (ttime, value); }
	void TTime_Diff (char *value)		{ Put_Field (ttime_diff, value); }
	void TTime_Error (char *value)		{ Put_Field (ttime_abs, value); }
	void Sum_Trips (int value)          { Put_Field (sum_trip, value); }
	void Sum_Error (char *value)        { Put_Field (sum_error, value); }
	void Avg_Error (char *value)        { Put_Field (avg_error, value); }

	virtual bool Create_Fields (void);

protected:
	virtual bool Set_Field_Numbers (void);

private:
	void Setup (void);

	int lvalue;

	int from_time, to_time, trip_start, started, start_diff, start_abs, trip_end, ended, end_diff, end_abs;
	int mid_trip, ttime, ttime_diff, ttime_abs, sum_trip, sum_error, avg_error;
};

#endif
