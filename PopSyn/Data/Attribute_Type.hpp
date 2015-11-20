//*********************************************************
//	Attribute_Type.hpp - attribute type class
//*********************************************************

#ifndef ATTRIBUTE_TYPE_HPP
#define ATTRIBUTE_TYPE_HPP

#include "Data_Array.hpp"
#include "Data_Range.hpp"

//---------------------------------------------------------
//	Attribute_Type class definition
//---------------------------------------------------------

class Attribute_Type
{
public:
	Attribute_Type (int num_types = 0);
	virtual ~Attribute_Type (void)             { Clear (); }

	int    Num_Types (void)                    { return (num_types); }
	int    PUMS_Field (void)                   { return (pums_field); }
	int    PUMS_Type (int value)               { return ((pums_breaks) ? pums_breaks->In_Index (value) : 0); }
	int    Zone_Field (int type)               { return (Check_Type (type) ? zone_field [type-1] : 0); }
	int    Offset (void)                       { return (offset); }
	int    Type (void)                         { return (type); }
	char * Field_Name (void)                   { return (name); }

	bool   Num_Types (int types);
	void   PUMS_Field (int value)              { pums_field = value; }
	void   PUMS_Breaks (Data_Range *value)     { pums_breaks = value; }
	void   Zone_Field (int type, int value)    { if (Check_Type (type)) zone_field [type-1] = value; }
	void   Offset (int value)                  { offset = value; }
	void   Type (int value)                    { type = value; }
	void   Field_Name (char *name);

	void   Clear (void);

private:
	bool   Check_Type (int type)               { return (type > 0 && type <= num_types); }
	void   Clear_Types (void);

	int    num_types;
	int    pums_field;
	Data_Range *pums_breaks;
	int   *zone_field;
	int    offset;
	int    type;
	char  *name;
};

//---------------------------------------------------------
//	Attribute_Type_Array class definition
//---------------------------------------------------------

class Attribute_Type_Array : public Data_Array
{
public:
	Attribute_Type_Array (int max_records = 10);
	virtual ~Attribute_Type_Array (void)     { Clear (); }

	bool Add (Attribute_Type *data = NULL)   { return (Data_Array::Add (data)); }
	
	Attribute_Type * New_Record (bool clear = false, int number = 1)
	                                         { return ((Attribute_Type *) Data_Array::New_Record (clear, number)); }

	Attribute_Type * Record (int index)      { return ((Attribute_Type *) Data_Array::Record (index)); }
	Attribute_Type * Record (void)           { return ((Attribute_Type *) Data_Array::Record ()); }

	Attribute_Type * First (void)            { return ((Attribute_Type *) Data_Array::First ()); }
	Attribute_Type * Next (void)             { return ((Attribute_Type *) Data_Array::Next ()); }

	Attribute_Type * Last (void)             { return ((Attribute_Type *) Data_Array::Last ()); }
	Attribute_Type * Previous (void)         { return ((Attribute_Type *) Data_Array::Previous ()); }

	Attribute_Type * operator[] (int index)  { return (Record (index)); }

	void Clear (void);
};

#endif
