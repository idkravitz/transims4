//*********************************************************
//	Rate_Group.hpp - emission rate group
//*********************************************************

#ifndef RATE_GROUP_HPP
#define RATE_GROUP_HPP

#include "Data_Array.hpp"
#include "Db_Header.hpp"

//---------------------------------------------------------
//	Rate_Group class definition
//---------------------------------------------------------

class Rate_Group
{
public:
	Rate_Group (void);
	virtual ~Rate_Group (void)           { Clear (); }

	int Group (void)                     { return (group); }
	int Year_Field (void)                { return (year); }
	int Month_Field (void)               { return (month); }
	int Period_Field (void)              { return (period); }
	int Region_Field (void)              { return (region); }
	int Area_Type_Field (void)           { return (area_type); }
	int Facility_Field (void)            { return (facility); }
	int Road_Type_Field (void)           { return (road_type); }
	int Vehicle_Field (void)             { return (vehicle); }
	int Pollutant_Field (void)           { return (pollutant); }
	int Speed_Bin_Field (void)           { return (speed_bin); }
	int Rate_Field (void)                { return (rate); }
	int Rate_Units (void)                { return (units); }

	int    Year (void)                   { if (file) file->Get_Field (year, &lvalue); return (lvalue); }
	int    Month (void)                  { if (file) file->Get_Field (month, &lvalue); return (lvalue); }
	int    Period (void)                 { if (file) file->Get_Field (period, &lvalue); return (lvalue); }
	int    Region (void)                 { if (file) file->Get_Field (region, &lvalue); return (lvalue); }
	int    Area_Type (void)              { if (file) file->Get_Field (area_type, &lvalue); return (lvalue); }
	int    Facility (void)               { if (file) file->Get_Field (facility, &lvalue); return (lvalue); }
	int    Road_Type (void)              { if (file) file->Get_Field (road_type, &lvalue); return (lvalue); }
	int    Vehicle (void)                { if (file) file->Get_Field (vehicle, &lvalue); return (lvalue); }
	int    Pollutant (void)              { if (file) file->Get_Field (pollutant, &lvalue); return (lvalue); }
	int    Speed_Bin (void)              { if (file) file->Get_Field (speed_bin, &lvalue); return (lvalue); }
	double Rate (void)                   { if (file) file->Get_Field (rate, &dvalue); return (dvalue); }

	Db_Header * Rate_File ()             { return (file); }

	void Group (int value)               { group = value; }
	bool Year_Field (char *value)        { year = ((file) ? file->Field_Number (value) : 0); return (year != 0); }
	bool Month_Field (char *value)       { month = ((file) ? file->Field_Number (value) : 0); return (month != 0); }
	bool Period_Field (char *value)      { period = ((file) ? file->Field_Number (value) : 0); return (period != 0); }
	bool Region_Field (char *value)      { region = ((file) ? file->Field_Number (value) : 0); return (region != 0); }
	bool Area_Type_Field (char *value)   { area_type = ((file) ? file->Field_Number (value) : 0); return (area_type != 0); }
	bool Facility_Field (char *value)    { facility = ((file) ? file->Field_Number (value) : 0); return (facility != 0); }
	bool Road_Type_Field (char *value)   { road_type = ((file) ? file->Field_Number (value) : 0); return (road_type != 0); }
	bool Vehicle_Field (char *value)     { vehicle = ((file) ? file->Field_Number (value) : 0); return (vehicle != 0); }
	bool Pollutant_Field (char *value)   { pollutant = ((file) ? file->Field_Number (value) : 0); return (pollutant != 0); }
	bool Speed_Bin_Field (char *value)   { speed_bin = ((file) ? file->Field_Number (value) : 0); return (speed_bin != 0); }
	bool Rate_Field (char *value)        { rate = ((file) ? file->Field_Number (value) : 0); return (rate != 0); }
	void Rate_Units (int value)          { units = value; }

	void Year (int value)                { if (file) file->Put_Field (year, value); }
	void Month (int value)               { if (file) file->Put_Field (month, value); }
	void Period (int value)              { if (file) file->Put_Field (period, value); }
	void Region (int value)              { if (file) file->Put_Field (region, value); }
	void Area_Type (int value)           { if (file) file->Put_Field (area_type, value); }
	void Facility (int value)            { if (file) file->Put_Field (facility, value); }
	void Road_Type (int value)           { if (file) file->Put_Field (road_type, value); }
	void Vehicle (int value)             { if (file) file->Put_Field (vehicle, value); }
	void Pollutant (int value)           { if (file) file->Put_Field (pollutant, value); }
	void Speed_Bin (int value)           { if (file) file->Put_Field (speed_bin, value); }
	void Rate (double value)             { if (file) file->Put_Field (rate, value); }

	bool Open_File (char *name, char *format = NULL);

	void Clear (void);

private:
	int lvalue;
	double dvalue;

	int group, year, month, period, region, area_type, facility, road_type, vehicle, pollutant, speed_bin, rate, units;
	Db_Header *file;
};

//---------------------------------------------------------
//	Rate_Group_Array
//---------------------------------------------------------

class Rate_Group_Array : public Data_Array
{
public:
	Rate_Group_Array (int max_records = 0);
	virtual ~Rate_Group_Array (void);

	bool Add (Rate_Group *data = NULL)   { return (Data_Array::Add ((void *) data)); }

	Rate_Group * New_Record (bool clear = false, int number = 1)
										 { return ((Rate_Group *) Data_Array::New_Record (clear, number)); }

	Rate_Group * First (void)            { return ((Rate_Group *) Data_Array::First ()); }
	Rate_Group * Next (void)             { return ((Rate_Group *) Data_Array::Next ()); }

	Rate_Group * Last (void)             { return ((Rate_Group *) Data_Array::Last ()); }
	Rate_Group * Previous (void)         { return ((Rate_Group *) Data_Array::Previous ()); }

	Rate_Group * operator[] (int index)  { return ((Rate_Group *) Record (index)); }
};

#endif

