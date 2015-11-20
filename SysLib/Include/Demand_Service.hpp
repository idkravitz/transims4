//*********************************************************
//	Demand_Service.hpp - travel demand data class
//*********************************************************

#ifndef DEMAND_SERVICE_HPP
#define DEMAND_SERVICE_HPP

#include "Network_Service.hpp"
#include "Demand_Code.hpp"
#include "Static_Service.hpp"

#include "Household_Data.hpp"
#include "Population_Data.hpp"
#include "TTime_Data.hpp"
#include "Performance_Data.hpp"
#include "Connect_Time.hpp"
#include "Rider_Data.hpp"
#include "Veh_Type_Data.hpp"
#include "Vehicle_Data.hpp"
#include "Activity_Data.hpp"
#include "Trip_Data.hpp"
#include "Boundary_Data.hpp"

#define household_data		(*household_array)
#define population_data		(*population_array)
#define ttime_data			(*((TTime_Array *) dir_array))
#define performance_data	(*((Performance_Array *) dir_array))
#define connect_time		(*((Connect_Time_Array *) connect_array))
#define rider_data			(*((Rider_Array *) line_array))
#define veh_type_data		(*veh_type_array)
#define vehicle_data		(*vehicle_array)
#define activity_data		(*activity_array)
#define trip_data			(*trip_array)
#define boundary_data		(*((Boundary_Array *) boundary_array))

//---------------------------------------------------------
//	Demand_Service - travel demand class definition
//---------------------------------------------------------

class Demand_Service : public Network_Service, public Demand_Code, public Static_Scale
{
	friend class Ridership_Output;
	friend class Speed_Bin_Output;

public:
	enum Demand_File {HOUSEHOLD, POPULATION, LINK_DELAY, PERFORMANCE, RIDERSHIP, 
		VEHICLE_TYPE, VEHICLE, ACTIVITY, TRIP, BOUNDARY_SPEED,
		NEW_HOUSEHOLD, NEW_POPULATION, NEW_LINK_DELAY, NEW_PERFORMANCE, NEW_RIDERSHIP, 
		NEW_VEHICLE_TYPE, NEW_VEHICLE, NEW_ACTIVITY, NEW_TRIP, NEW_BOUNDARY_SPEED,
		END_DEMAND};

	Demand_Service (void);
	virtual ~Demand_Service (void);

	virtual void Execute (void);

	virtual bool Household_Processing (Db_File *file);
	virtual bool Population_Processing (Db_File *file);
	virtual bool Delay_Processing (Db_File *file);
	virtual bool Performance_Processing (Db_File *file);
	virtual bool Rider_Processing (Db_File *file);
	virtual bool Veh_Type_Processing (Db_File *file);
	virtual bool Vehicle_Processing (Db_File *file);
	virtual bool Activity_Processing (Db_File *file);
	virtual bool Trip_Processing (Db_File *file);
	virtual bool Boundary_Processing (Db_File *file);

protected:
	static char * SORT_VEHICLES;
	static char * DEMAND_FILE_FORMAT;
	static char * MAX_LINK_DELAY_ERRORS;

	void   Required_Demand_Files (Demand_File files []);
	void   Optional_Demand_Files (Demand_File files []);

	bool   Demand_File_Flag (Demand_File file)	{ return ((file < END_DEMAND) ? demand_file [file].flag : false); }
	void   Demand_File_False (Demand_File file)	{ if (file < END_DEMAND) { demand_file [file].flag = false; demand_file [file].option = false; }}
	void   Demand_File_True (Demand_File file)	{ if (file < END_DEMAND) demand_file [file].flag = true; }

	bool   Demand_Option_Flag (Demand_File file)	{ return ((file < END_DEMAND) ? demand_file [file].option : false); }
	void   Demand_Option_False (Demand_File file)	{ if (file < END_DEMAND) demand_file [file].option = false; }
	void   Demand_Option_True (Demand_File file)	{ if (file < END_DEMAND) demand_file [file].option = true; }

	Access_Type Demand_File_Access (Demand_File file)	{ return ((file < END_DEMAND) ? demand_file [file].access : READ); }
	void   Demand_File_Access (Demand_File file, Access_Type access)	{ if (file < END_DEMAND) demand_file [file].access = access; }

	Format_Type Demand_Format (void)			{ return ((Format_Type) Demand_Code::Default_Format ()); }
	void Demand_Format (Format_Type format)		{ Demand_Code::Default_Format (format); }

	bool   Demand_Data_Flag (Demand_File file)	{ return ((file < END_DEMAND) ? demand_file [file].data : false); }
	void   Demand_Data_True (Demand_File file)	{ if (file < END_DEMAND) demand_file [file].data = true; }

	Db_File * Demand_Db_File (Demand_File file)	{ return ((file < END_DEMAND) ? demand_file [file].file : NULL); }
	Db_Base * Demand_Db_Base (Demand_File file)	{ return ((Db_Base *) Demand_Db_File (file)); }
	
	bool Demand_Copy_Header (Demand_File from, Demand_File to);

	virtual void Program_Control (void);

	void Time_Increment (int increment);
	int  Time_Increment (void)					{ return ((dir_array) ? ttime_data.Period_Size () : 0); }
	virtual int  Time_Period (int tod)			{ return ((dir_array) ? ttime_data.Period (tod) : 0); }

	void Compress_Vehicles (bool compress = true)	{ compress_flag = compress; }
	void Sort_Vehicles (bool sort = true)		{ sort_flag = sort; }

	int  Max_Delay_Errors (void)				{ return (max_delay_errors); }
	void Max_Delay_Errors (int value)			{ max_delay_errors = value; }

	//---- data arrays ----

	Household_Array *household_array;
	Population_Array *population_array;
	Veh_Type_Array *veh_type_array;
	Vehicle_Array *vehicle_array;
	Activity_Array *activity_array;
	Trip_Array *trip_array;
	Boundary_Array *boundary_array;

private:
	typedef struct {
		char     *key;
		char     *format;
		Db_File  *file;
		bool      flag;
		bool      option;
		bool      data;
		Access_Type access;
	} File_Data;

	File_Data demand_file [END_DEMAND];

	bool sort_flag, compress_flag, control_flag;
	int max_delay_errors;

	//---- methods ----

	void Read_Household (void);
	void Read_Population (void);
	void Read_Delay (void);
	void Read_Performance (void);
	void Read_Ridership (void);
	void Read_Veh_Type (void);
	void Read_Vehicle (void);
	void Read_Activity (void);
	void Read_Trip (void);
	void Read_Boundary (void);
};
#endif
