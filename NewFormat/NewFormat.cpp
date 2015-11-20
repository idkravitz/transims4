//*********************************************************
//	NewFormat.cpp - Convert Version 3 Files to Version 4
//*********************************************************

#include "NewFormat.hpp"

char * NewFormat::VERSION3_DRIVER_PLANS		= "VERSION3_DRIVER_PLANS";
char * NewFormat::VERSION3_POPULATION_FILE	= "VERSION3_POPULATION_FILE";
char * NewFormat::VERSION3_ACTIVITY_FILE	= "VERSION3_ACTIVITY_FILE";
char * NewFormat::VERSION3_SURVEY_ACTIVITY	= "VERSION3_SURVEY_ACTIVITY";
char * NewFormat::TIME_OF_DAY_FORMAT		= "TIME_OF_DAY_FORMAT";

//---------------------------------------------------------
//	NewFormat constructor
//---------------------------------------------------------

NewFormat::NewFormat (void) : Demand_Service ()
{
	Program ("NewFormat");
	Version ("4.0.6");
	Title ("Convert Version 3 Files to Version 4");

	Network_File optional_network [] = {
		DIRECTORY, NODE, LINK, PARKING, ACTIVITY_LOCATION, NEW_TRANSIT_DRIVER, END_NETWORK
	};
	Demand_File optional_demand [] = {
		NEW_HOUSEHOLD, NEW_POPULATION, NEW_ACTIVITY, END_DEMAND
	};

	char *keys [] = {
		VERSION3_DRIVER_PLANS,
		VERSION3_POPULATION_FILE,
		VERSION3_ACTIVITY_FILE,
		VERSION3_SURVEY_ACTIVITY,
		TIME_OF_DAY_FORMAT,
		NULL
	};
	Optional_Network_Files (optional_network);
	Optional_Demand_Files (optional_demand);

	Key_List (keys);
	
	proj_service.Add_Keys ();

	person_number.Data_Size (sizeof (Person_Number));

	driver_flag = population_flag = survey_flag = activity_flag = xy_flag = loc_flag = false;
}

//---------------------------------------------------------
//	NewFormat destructor
//---------------------------------------------------------

NewFormat::~NewFormat (void)
{
}

//---------------------------------------------------------
//	main program
//---------------------------------------------------------

int main (int commands, char *control [])
{
	NewFormat *exe = new NewFormat ();

	return (exe->Start_Execution (commands, control));
}
