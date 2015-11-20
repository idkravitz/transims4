//********************************************************* 
//	Plan_File.hpp - Plan File Input/Output
//*********************************************************

#ifndef PLAN_FILE_HPP
#define PLAN_FILE_HPP

#include "Static_Service.hpp"
#include "Ext_File.hpp"

#include "stdlib.h"

typedef struct {
	int   key1;
	int   key2;
	int   start_id;
	int   end_id;
	int   duration;
	int   stop_time;
	int   cost;
	int   gcf;
	char  trip;
	char  leg;	
	char  start_type;
	char  end_type;
	char  driver_flag;
	char  mode;
	short tokens;
	int   data [1];
} Plan_Data;

//---------------------------------------------------------
//	Plan_File Class definition
//---------------------------------------------------------

class  Plan_File : public Ext_File, public Static_Scale
{
public:
	Plan_File (Access_Type access = READ, Memory_Type memory = PRIVATE_MEMORY, Sort_Type sort = TRAVELER_SORT);
	Plan_File (char *filename, Access_Type access = READ, Memory_Type memory = PRIVATE_MEMORY, Sort_Type sort = TRAVELER_SORT);
	~Plan_File ();

	virtual bool Open (int ext_number);
	virtual bool Open (char *filename = NULL);

	//---- read/write functions ----

	bool Read (long offset = -1);
	bool Write (Plan_Data *plan = NULL);

	//---- size functions ----

	int  Num_Records (void)		{ return (num_record); };
	int  Num_Plans (void)		{ return (num_plan); };
	int  Num_Travelers (void)	{ return (num_traveler); };
	int  Num_Trips (void)		{ return (num_trip); };

	void Reset_Counters (int rec = 0, int plan = 0, int trv = 0, int trip = 0)	
					{ num_record = rec; num_plan = plan; num_traveler = trv; num_trip = trip; };

	//---- output data fields ----

	int Traveler (void)		{ return ((time_sort) ? plan->key2 : plan->key1); };
	int Household (void)	{ return (Traveler () / Traveler_Scale ()); }
	int Person (void)		{ return (Traveler () % Traveler_Scale ()); }
	int Trip (void)			{ return ((int) plan->trip); };
	int Leg (void)			{ return ((int) plan->leg); };
	int Time (void)			{ return ((time_sort) ? plan->key1 : plan->key2); };
	int Start_ID (void)		{ return (plan->start_id); };
	int Start_Type (void)	{ return ((int) plan->start_type); };
	int End_ID (void)		{ return (plan->end_id); };
	int End_Type (void)		{ return ((int) plan->end_type); };
	int Duration (void)		{ return (plan->duration); };
	int Stop_Time (void)	{ return (plan->stop_time); };
	int Cost (void)			{ return (plan->cost); };
	int GCF (void)			{ return (plan->gcf); };
	int Driver_Flag (void)	{ return ((int) plan->driver_flag); };
	int Mode (void)			{ return ((int) plan->mode); };
	int Tokens (void)		{ return ((int) plan->tokens); };

	int Vehicle (void);
	int Passsengers (void)			{ return ((Mode () == AUTO_MODE) ? plan->data [1] : 0); }
	int Route (void);
	int Mode_ID (void);
	int *Path (int *num_path);
	int *Data (int *max_token);

	//---- input data fields ----

	void Traveler (int data)		{ if (time_sort) plan->key2 = data; else plan->key1 = data; };
	void Household (int data)		{ Traveler (data * Traveler_Scale () + Person ()); }
	void Person (int data)			{ Traveler (Household () * Traveler_Scale () + data); }
	void Trip (int data)			{ plan->trip = (char) data; };
	void Leg (int data)				{ plan->leg = (char) data; };
	void Time (int data)			{ if (time_sort) plan->key1 = data; else plan->key2 = data; };
	void Start_ID (int data)		{ plan->start_id = data; };
	void Start_Type (int data)		{ plan->start_type = (char) data; };
	void End_ID (int data)			{ plan->end_id = data; };
	void End_Type (int data)		{ plan->end_type = (char) data; };
	void Duration (int data)		{ plan->duration = data; };
	void Stop_Time (int data)		{ plan->stop_time = data; };
	void Cost (int data)			{ plan->cost = data; };
	void GCF (int data)				{ plan->gcf = data; };
	void Driver_Flag (int data)		{ plan->driver_flag = (char) data; };
	void Mode (int data)			{ plan->mode = (char) data; };
	void Tokens (int data)			{ plan->tokens = (short) data; };

	void Vehicle (int data);
	void Passengers (int data)		{ if (Mode () == AUTO_MODE) plan->data [1] = data; };
	void Route (int data);
	void Mode_ID (int data);
	void Num_Path (int data);

	Plan_Data * Plan (void)			{ return (plan); };
	bool Plan (Plan_Data *plan);

	void Delete_Plan (Plan_Data *plan = NULL);

	bool Check_Size (int size);

	//--- type functions ----

	Memory_Type Plan_Memory (void)			{ return ((allocate_memory) ? ALLOCATE_MEMORY : PRIVATE_MEMORY); };
	Sort_Type   Plan_Sort (void)			{ return ((time_sort) ? TIME_SORT : TRAVELER_SORT); };

	void Plan_Memory (Memory_Type memory)	{ allocate_memory = (memory == ALLOCATE_MEMORY); };
	void Plan_Sort (Sort_Type sort)			{ time_sort = (sort == TIME_SORT); };
	
	bool Node_Based_Flag (void)             { return (node_based); }
	void Node_Based_Flag (bool flag)        { node_based = flag; }

private:
	bool Setup_Record (void);
	void Setup (Memory_Type memory, Sort_Type sort);
	
	bool Read_Plan_Def (void);
	bool Write_Plan_Def (void);

	bool allocate_memory, time_sort, node_based;
	int num_record, num_plan, num_traveler, num_trip, max_data;

	Plan_Data *plan;
};

#endif
