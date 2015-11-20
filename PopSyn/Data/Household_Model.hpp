//*********************************************************
//	Household_Model.hpp - household model class
//*********************************************************

#ifndef HOUSEHOLD_MODEL_HPP
#define HOUSEHOLD_MODEL_HPP

#include "Pointer_Array.hpp"
#include "PUMA_Data.hpp"

//---------------------------------------------------------
//	Household_Model class definition
//---------------------------------------------------------

class Household_Model : public PUMA_Data_Array
{
public:
	Household_Model (int model = 0);
	virtual ~Household_Model (void)            { Clear (); }

	int    Model (void)                        { return (model); }
	int    Location_Field (void)               { return (location_field); }
	int    Total_Field (void)                  { return (total_field); }

	void   Model (int value)                   { model = value; }
	void   Location_Field (int value)          { location_field = value; }
	void   Total_Field (int value)             { total_field = value; }

private:
	int    model;
	int    location_field;
	int    total_field;
};

//---------------------------------------------------------
//	Household_Model_Array class definition
//---------------------------------------------------------

class Household_Model_Array : public Data_Pointer_Array
{
public:
	Household_Model_Array (int max_records = 5);
	virtual ~Household_Model_Array (void)     { Clear (); }

	virtual void Delete_Pointers (void);

	bool Add (Household_Model *data = NULL)   { return (Data_Pointer_Array::Add (data)); }
	
	Household_Model * Record (int index)      { return ((Household_Model *) Data_Pointer_Array::Record (index)); }
	Household_Model * Record (void)           { return ((Household_Model *) Data_Pointer_Array::Record ()); }

	Household_Model * First (void)            { return ((Household_Model *) Data_Pointer_Array::First ()); }
	Household_Model * Next (void)             { return ((Household_Model *) Data_Pointer_Array::Next ()); }

	Household_Model * Last (void)             { return ((Household_Model *) Data_Pointer_Array::Last ()); }
	Household_Model * Previous (void)         { return ((Household_Model *) Data_Pointer_Array::Previous ()); }

	Household_Model * operator[] (int index)  { return (Record (index)); }
};

#endif
