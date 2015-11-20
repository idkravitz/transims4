//*********************************************************
//	Group_Weight.hpp - group period weight class
//*********************************************************

#ifndef GROUP_WEIGHT_HPP
#define GROUP_WEIGHT_HPP

#include "Class_Pointer.hpp"
#include "Class_Index.hpp"

//---------------------------------------------------------
//	Group_Weight class definition
//---------------------------------------------------------

class Group_Weight : public Class_Index
{
public:
	Group_Weight (int group = 0);
	virtual ~Group_Weight (void)      { Clear (); }

	int  Group (void)                 { return (ID ()); }
	void Group (int value)            { ID (value); }

	int  Num_Periods (void)           { return (weights.Num_Records ()); }

	double * First_Weight (void)      { return ((double *) weights.First ()); }
	double * Next_Weight (void)       { return ((double *) weights.Next ()); }

	double * Best_Weight (int period) { return ((double *) weights.Best (period)); }

	bool Add_Weight (double wt)       { return (weights.Add (&wt)); }

	void Clear (void)                 { weights.Clear (); }

private:
	Data_Array weights;
};

//---------------------------------------------------------
//	Group_Weight_Array class definition
//---------------------------------------------------------

class Group_Weight_Array : public Class_Pointer
{
public:
	Group_Weight_Array (int max_records = 0);
	virtual ~Group_Weight_Array (void)      { Clear (); }

	bool Add (Group_Weight *data)              { return (Class_Pointer::Add (data)); }
	
	Group_Weight * Get (int group)             { return ((Group_Weight *) Class_Pointer::Get (group)); }

	Group_Weight * Record (int index)          { return ((Group_Weight *) Class_Pointer::Record (index)); }
	Group_Weight * Record (void)               { return ((Group_Weight *) Class_Pointer::Record ()); }

	Group_Weight * First (void)                { return ((Group_Weight *) Class_Pointer::First ()); }
	Group_Weight * Next (void)                 { return ((Group_Weight *) Class_Pointer::Next ()); }
	
	Group_Weight * First_Key (void)            { return ((Group_Weight *) Class_Pointer::First_Key ()); }
	Group_Weight * Next_Key (void)             { return ((Group_Weight *) Class_Pointer::Next_Key ()); }

	Group_Weight * operator[] (int index)      { return (Record (index)); }

	virtual void Clear (void);
};

#endif
