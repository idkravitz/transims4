//*********************************************************
//	Magic_Move.cpp - Magic Movement
//*********************************************************

#include "Router.hpp"

//---------------------------------------------------------
//	Magic_Move
//---------------------------------------------------------

int Router::Magic_Move (int end_time, int type)
{
	trip_des.Cum (trip_org.Cum () + 1);
	trip_des.TOD (end_time);

	//---- check the time constraint ----

	if (trip_des.TOD () < trip_org.TOD ()) {
		if (max_tod < MAX_INTEGER) {
			trip_des.TOD (max_tod);
			if (trip_des.TOD () < trip_org.TOD ()) return (Set_Problem (TIME_PROBLEM));
		} else {
			trip_des.TOD (trip_org.TOD () + 1);
		}
	}

	//---- save the plan record ----

	plan_file.Leg (plan_file.Leg () + 1);
	plan_file.Time (Resolve (trip_org.TOD ()));
	plan_file.Start_ID (trip_org.ID ());
	plan_file.Start_Type (trip_org.Type ());
	plan_file.End_ID (trip_des.ID ());
	plan_file.End_Type (trip_des.Type ());
	plan_file.Duration (Resolve (trip_des.TOD () - trip_org.TOD ()));
	plan_file.Stop_Time (Resolve (trip_des.TOD ()));
	plan_file.Cost (trip_des.Cost () - trip_org.Cost ());
	plan_file.GCF (1);
	plan_file.Driver_Flag (0);
	plan_file.Mode (Plan_File::MAGIC_MODE);
	plan_file.Tokens (1);

	plan_file.Mode_ID (type);

	if (save_plans && !plan_file.Write ()) {
		Error ("Writing Plan File");
	}
	return (0);
}
