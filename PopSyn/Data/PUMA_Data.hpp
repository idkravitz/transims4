//*********************************************************
//	PUMA_Data.hpp - PUMA Data classes
//*********************************************************

#ifndef PUMA_DATA_HPP
#define PUMA_DATA_HPP

#include "PUMA_Zone.hpp"
#include "Attribute_Type.hpp"
#include "Household_Index.hpp"
#include "Class_Index.hpp"
#include "Class_Pointer.hpp"

//---------------------------------------------------------
//	PUMA_Data class definition
//---------------------------------------------------------

class PUMA_Data : public Class_Index
{
public:
	PUMA_Data (int puma = 0, int num_data = 1);
	virtual ~PUMA_Data (void)     { Clear (); }	

	int      PUMA (void)          { return (ID ()); }
	double * Data (void)          { return (data); }

	void     PUMA (int value)     { ID (value); }
	//void     Data (double *value) { data [0] = value [0]; }

	PUMA_Zone * Zone (void)       { return (puma_zone.Record ()); }
	PUMA_Zone * First (void)      { return (puma_zone.First ()); }
	PUMA_Zone * Next (void)       { return (puma_zone.Next ()); }
	
	PUMA_Zone * New_Zone (int zone);

	int   Num_Zones (void)        { return (puma_zone.Num_Records ()); }
	void  Num_Data (int value);
	void  Clear (void);

	Household_Index * First_HH (int cell)  { return (hh_index.Get_GE (cell, 0)); }
	Household_Index * Next_HH (void)       { return (hh_index.Next_Key ()); }

	bool  New_Household (int cell, int index);

private:
	PUMA_Zone_Array  puma_zone;
	Household_Index_Array  hh_index;

	double *data;
};

//---------------------------------------------------------
//	PUMA_Data_Array class definition
//---------------------------------------------------------

class PUMA_Data_Array : public Class_Pointer
{
public:
	PUMA_Data_Array (int max_records = 5);
	virtual ~PUMA_Data_Array (void)     { Clear (); }

	bool Add (PUMA_Data *data)          { return (Class_Pointer::Add (data)); }

	PUMA_Data * Get (int puma)          { return ((PUMA_Data *) Class_Pointer::Get (puma)); }
	
	PUMA_Data * Record (int index)      { return ((PUMA_Data *) Class_Pointer::Record (index)); }
	PUMA_Data * Record (void)           { return ((PUMA_Data *) Class_Pointer::Record ()); }

	PUMA_Data * First (void)            { return ((PUMA_Data *) Class_Pointer::First ()); }
	PUMA_Data * Next (void)             { return ((PUMA_Data *) Class_Pointer::Next ()); }

	PUMA_Data * First_Key (void)        { return ((PUMA_Data *) Class_Pointer::First_Key ()); }
	PUMA_Data * Next_Key (void)         { return ((PUMA_Data *) Class_Pointer::Next_Key ()); }

	PUMA_Data * operator[] (int index)  { return (Record (index)); }

	virtual void Delete_Pointers (void);

	void   Clear (void);

	int    Num_Cells (void)              { return (num_cells); }
	bool   Num_Cells (int cells);

	int    Num_Attributes (void)         { return (attribute.Num_Records ()); }
	int    Num_Data (void)               { return (num_data); }

	bool   Set_Record (void);

	PUMA_Data * New_PUMA (int puma);

	void   Set_PUMA_Data (PUMA_Data *);
	void   Set_PUMA_Zone (PUMA_Zone *);

	PUMA_Data * Get_PUMA_Data (void)     { return (puma_data); }
	PUMA_Zone * Get_PUMA_Zone (void)     { return (puma_zone); }

	bool   Add_Share (int index, double value);
	void   Sum_Targets (void);
	void   Distribute_Households (void);

	Attribute_Type * New_Attribute (void);
	Attribute_Type * Attribute (int num) { return (attribute [num]); }

	double PUMA_Total (void)             { return ((data_ptr) ? *data_ptr : 0.0); }
	void   PUMA_Total (double value)     { if (data_ptr) *data_ptr = value; }

	double Zone_Total (void)             { return ((zone_ptr) ? *zone_ptr : 0.0); }
	void   Zone_Total (double value)     { if (zone_ptr) *zone_ptr = value; }

	double PUMA_Share (int cell);
	void   PUMA_Share (int cell, double value);

	double Zone_Share (int cell);
	void   Zone_Share (int cell, double value);

	double PUMA_Target (int offset);
	void   PUMA_Target (int offset, double value);

	double Zone_Target (int offset);
	void   Zone_Target (int offset, double value);

protected:
	Attribute_Type_Array  attribute;

private:
	int num_cells, num_data;
	double *data_ptr, *zone_ptr;
	PUMA_Data *puma_data;
	PUMA_Zone *puma_zone;
};
#endif
