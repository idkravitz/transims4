//*********************************************************
//	Phasing_Data.hpp - network phasing plan data
//*********************************************************

#ifndef PHASING_DATA_HPP
#define PHASING_DATA_HPP

#include "Data_Array.hpp"

//---------------------------------------------------------
//	Phasing_Data class definition
//---------------------------------------------------------

class Phasing_Data
{
public:
	Phasing_Data (void);
	virtual ~Phasing_Data (void)       { Clear (); }

	int  Node (void)                   { return (node); }
	int  In_Link_Dir (void)            { return (in_link_dir); }
	int  In_Link (void)                { return ((in_link_dir >> 1)); }
	int  In_Dir (void)                 { return ((in_link_dir & 1)); } 
	int  Out_Link_Dir (void)           { return (out_link_dir); }
	int  Out_Link (void)               { return ((out_link_dir >> 1)); }
	int  Out_Dir (void)                { return ((out_link_dir & 1)); }
	int  Timing (void)                 { return (timing); }
	int  Phase (void)                  { return (phase); }
	int  Protection (void)             { return (protection); }
 	int  Detector (int num = 1); 

	void Node (int value)              { node = value; }
	void In_Link_Dir (int value)       { in_link_dir = value; }
	void In_Link (int value)           { in_link_dir = (value << 1) + In_Dir (); }
	void In_Dir (int value)            { in_link_dir = (In_Link () << 1) + value; }
	void Out_Link_Dir (int value)      { out_link_dir = value; }
	void Out_Link (int value)          { out_link_dir = (value << 1) + Out_Dir (); }
	void Out_Dir (int value)           { out_link_dir = (Out_Link () << 1) + value; }
	void Timing (int value)            { timing = value; }
	void Phase (int value)             { phase = (short) value; }
	void Protection (int value)        { protection = (char) value; }
	void Detector (int num, int value);

	bool Num_Detectors (int num);
	int  Num_Detectors (void)          { return (num_detectors); }

	void Clear (void);
	void Reset (void)                  { num_detectors = 0; }

	//---- overloaded functions ----

	int  Node_List (void)              { return (node); }
	void Node_List (int value)         { node = value; }

	int  Control_Index (void)          { return (in_link_dir); }
	void Control_Index (int value)     { in_link_dir = value; }

	int  Phase_List (void)             { return (out_link_dir); }
	void Phase_List (int value)        { out_link_dir = value; }

	int  Timing_Index (void)           { return (timing); }
	void Timing_Index (int value)      { timing = value; }

private:
	int   node;
	int   in_link_dir;
	int   out_link_dir;
	int   timing;
	short phase;
	char  protection;
	char  num_detectors;
	union {
		int  detector1;
		int *detector;
	};
};

//---------------------------------------------------------
//	Phasing_Array class definition
//---------------------------------------------------------

class Phasing_Array : public Data_Array
{
public:
	Phasing_Array (int max_records = 0);

	bool Add (Phasing_Data *data = NULL)    { return (Data_Array::Add (data)); }

	Phasing_Data * New_Record (bool clear = false, int number = 1)
	                                        { return ((Phasing_Data *) Data_Array::New_Record (clear, number)); }

	Phasing_Data * Record (int index)       { return ((Phasing_Data *) Data_Array::Record (index)); }
	Phasing_Data * Record (void)            { return ((Phasing_Data *) Data_Array::Record ()); }

	Phasing_Data * First (void)             { return ((Phasing_Data *) Data_Array::First ()); }
	Phasing_Data * Next (void)              { return ((Phasing_Data *) Data_Array::Next ()); }

	Phasing_Data * operator[] (int index)   { return (Record (index)); }
};

#endif
