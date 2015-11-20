//*********************************************************
//	Select_Set.hpp - transit route selection set
//*********************************************************

#ifndef SELECT_SET_HPP
#define SELECT_SET_HPP

#include "Static_Service.hpp"
#include "Time_Range.hpp"
#include "Arcview_File.hpp"
#include "Data_Range.hpp"
#include "Offset_Data.hpp"
#include "Route_Legs.hpp"

//---------------------------------------------------------
//	Select_Set class definition
//---------------------------------------------------------

class Select_Set : public Static_Service
{
public:
	Select_Set (void);
	virtual ~Select_Set (void)             { Clear (); }

	int    Number ()                       { return (number); }
	int    Time_Method ()                  { return (time_method); }
	char * Select_Label ()                 { return (select_label); }
	int    Num_Periods ()                  { return ((time_periods) ? time_periods->Num_Ranges () : 0); }
	int    Time_Period (int second)        { return ((time_periods) ? time_periods->In_Index (second) : 0); }
	const char * Time_Format (int num)     { return ((time_periods) ? time_periods->Range_Format (num) : ""); }
	bool   Select_Modes (int mode)         { return ((select_modes) ? select_modes [mode] : true); }
	bool   Select_Routes (int route)       { return ((select_routes) ? select_routes->In_Range (route) : true); }

	Route_Legs * Get_Route_Legs (int index) { return (route_legs ? (*route_legs) [index] : NULL); }

	bool   Subarea_Flag ()                 { return (select_subarea != NULL); }
	bool   In_Subarea (double x, double y);

	void   Number (int value)              { number = value; }
	void   Time_Method (int value)         { time_method = value; }
	bool   Select_Label (char *text);
	bool   Time_Periods (char *range, char *increment);
	bool   Select_Modes (char *string);
	bool   Select_Routes (char *string);
	bool   Select_Subarea (char *filename);

	bool   Num_Route_Legs (int num)        { return (route_legs ? route_legs->Num_Records (num) : true); }
	bool   Add_Route_Legs (void)           { return (route_legs ? route_legs->Add () : true); }

	void   Copy (Select_Set *set_ptr);
	void   Replace (Select_Set *set_ptr);

	void   Reset (void);
	void   Clear (void);

protected:

	Time_Range * Time_Periods ()           { return (time_periods); }
	bool * Select_Modes ()                 { return (select_modes); }
	Data_Range * Select_Routes ()          { return (select_routes); }
	Arcview_File * Select_Subarea ()       { return (select_subarea); }
	Route_Legs_Array * Get_Route_Legs_Array ()   { return (route_legs); }

private:

	int  number;
	int  time_method;
	char *select_label;
	Time_Range *time_periods;
	bool *select_modes;
	Data_Range *select_routes;
	Arcview_File *select_subarea;
	Route_Legs_Array *route_legs;
};

//---------------------------------------------------------
//	Select_Set_Array
//---------------------------------------------------------

class Select_Set_Array : public Data_Array
{
public:
	Select_Set_Array (int max_records = 5);
	virtual ~Select_Set_Array (void);

	bool Add (Select_Set *data = NULL)    { return (Data_Array::Add ((void *) data)); }

	Select_Set * New_Record (bool clear = false, int number = 1)
										  { return ((Select_Set *) Data_Array::New_Record (clear, number)); }

	Select_Set * First (void)             { return ((Select_Set *) Data_Array::First ()); }
	Select_Set * Next (void)              { return ((Select_Set *) Data_Array::Next ()); }

	Select_Set * Last (void)              { return ((Select_Set *) Data_Array::Last ()); }
	Select_Set * Previous (void)          { return ((Select_Set *) Data_Array::Previous ()); }

	Select_Set * operator[] (int index)   { return (Select_Record (index)); }

private:
	Select_Set * Select_Record (int index);

	Select_Set * current_set;
};

#endif

