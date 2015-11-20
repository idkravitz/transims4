//*********************************************************
//	Attribute_Data.hpp - household attribute data class
//*********************************************************

#ifndef ATTRIBUTE_DATA_HPP
#define ATTRIBUTE_DATA_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Attribute_Data class definition
//---------------------------------------------------------

class Attribute_Data
{
public:
	Attribute_Data (int num_types = 0);
	virtual ~Attribute_Data (void)             { Clear (); }

	int    Num_Types (void)                    { return (num_types); }
	int    Num_Cells (void)                    { return (num_cells); }
	double Total (int type)                    { return (Check_Type (type) ? total [type-1] : 0); }
	double Target (int type)                   { return (Check_Type (type) ? target [type-1] : 0); }
	double Factor (int type)                   { return (Total (type)); }
	int    Index (int cell)                    { return (Check_Cell (cell) ? index [cell-1] : 0); }

	bool   Num_Types (int types);
	bool   Num_Cells (int cells);
	void   Total (int type, double value)      { if (Check_Type (type)) total [type-1] = value; }
	void   Target (int type, double value)     { if (Check_Type (type)) target [type-1] = value; }
	void   Factor (int type, double value)     { Total (type, value); }
	void   Index (int cell, int type)          { if (Check_Cell (cell)) index [cell-1] = type; }

	void   Add_Total (int type, double value)  { if (Check_Type (type)) total [type-1] += value; }
	void   Total_Cell (int cell, double value) { Add_Total (Index (cell), value); }
	double Cell_Factor (int cell)              { return (Factor (Index (cell))); }

	void   Normalize (void);
	void   Zero_Total (void);
	double Factor_Total (void);
	void   Clear (void);

private:
	void   Clear_Types ();
	void   Clear_Cells ();

	bool   Check_Type (int type)               { return (type > 0 && type <= num_types); }
	bool   Check_Cell (int cell)               { return (cell > 0 && cell <= num_cells); }

	int    num_types;
	int    num_cells;
	double *total;
	double *target;
	int    *index;
	int    *zone_field;
};

//---------------------------------------------------------
//	Attribute_Array class definition
//---------------------------------------------------------

class Attribute_Array : public Data_Array
{
public:
	Attribute_Array (int max_records = 10);
	virtual ~Attribute_Array (void)          { Clear (); }

	bool Add (Attribute_Data *data = NULL)   { return (Data_Array::Add (data)); }
	
	Attribute_Data * New_Record (bool clear = false, int number = 1)
	                                         { return ((Attribute_Data *) Data_Array::New_Record (clear, number)); }

	Attribute_Data * Record (int index)      { return ((Attribute_Data *) Data_Array::Record (index)); }
	Attribute_Data * Record (void)           { return ((Attribute_Data *) Data_Array::Record ()); }

	Attribute_Data * First (void)            { return ((Attribute_Data *) Data_Array::First ()); }
	Attribute_Data * Next (void)             { return ((Attribute_Data *) Data_Array::Next ()); }

	Attribute_Data * Last (void)             { return ((Attribute_Data *) Data_Array::Last ()); }
	Attribute_Data * Previous (void)         { return ((Attribute_Data *) Data_Array::Previous ()); }

	Attribute_Data * operator[] (int index)  { return (Record (index)); }

	void Clear (void);
};

#endif
