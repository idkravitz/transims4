//*********************************************************
//	Dir_Connect.hpp - directional link connections
//*********************************************************

#ifndef DIR_CONNECT_HPP
#define DIR_CONNECT_HPP

#include "Dir_List.hpp"
#include "Connection.hpp"

//---------------------------------------------------------
//	Dir_Connect class definition
//---------------------------------------------------------

class Dir_Connect : public Dir_List
{
public:
	Dir_Connect (int link_dir = 0);
	virtual ~Dir_Connect (void)          { Clear (); }

	int Type (void)                   { return (type); }
	int Length (void)                 { return (length); }
	int Offset (void)                 { return (offset); }
	int Setback (void)                { return (setback); }
	int Left_Turn (void)              { return (left_turn); }
	int Right_Turn (void)             { return (right_turn); }
	int Left_Merge (void)             { return (left_merge); }
	int Right_Merge (void)            { return (right_merge); }

	void Type (int value)             { type = (short) value; }
	void Length (int value)           { length = value; }
	void Offset (int value)           { offset = value; }
	void Setback (int value)          { setback = (short) value; }
	void Left_Turn (int value)        { left_turn = (char) value; }
	void Right_Turn (int value)       { right_turn = (char) value; }
	void Left_Merge (int value)       { left_merge = (char) value; }
	void Right_Merge (int value)      { right_merge = (char) value; }

	int Num_From (void)               { return ((from) ? from->Num_Records () : 0); }
	int Num_To (void)                 { return ((to) ? to->Num_Records () : 0); }

	bool From_Connection (Connection *ptr);
	bool To_Connection (Connection *ptr);

	Connection * From_First (void)    { return ((from) ? from->First_Key () : NULL); }
	Connection * From_Next (void)     { return ((from) ? from->Next_Key () : NULL); }

	Connection * To_First (void)      { return ((to) ? to->First_Key () : NULL); }
	Connection * To_Next (void)       { return ((to) ? to->Next_Key () : NULL); }
	
	Connection * From_Last (void)     { return ((from) ? from->Last_Key () : NULL); }
	Connection * From_Previous (void) { return ((from) ? from->Previous_Key () : NULL); }

	Connection * To_Last (void)       { return ((to) ? to->Last_Key () : NULL); }
	Connection * To_Previous (void)   { return ((to) ? to->Previous_Key () : NULL); }

	Connection * To_Thru (void)       { return ((to) ? to->Get_Thru () : NULL); }
	Connection * From_Thru (void)     { return ((from) ? from->Get_Thru () : NULL); }

	Connection * To_Dir (int dir)     { return ((to) ? to->Get_Dir (dir) : NULL); }
	Connection * From_Dir (int dir)   { return ((from) ? from->Get_Dir (dir) : NULL); }

	void Clear (void);
	void Set_Dir (void);

	//---- overlayed method ----

	int  Zone (void)                  { return (setback); }
	void Zone (int value)             { setback = (short) value; }

private:
	short type;
	short setback;
	int   length;
	int   offset;
	char  left_turn;
	char  right_turn;
	char  left_merge;
	char  right_merge;

	Connection_Array *from;
	Connection_Array *to;
};

//---------------------------------------------------------
//	Dir_Connect_Array class definition
//---------------------------------------------------------

class Dir_Connect_Array : public Dir_Array
{
public:
	Dir_Connect_Array (int max_records = 0);
	virtual ~Dir_Connect_Array (void);

	bool Add (Dir_Connect *data = NULL)    { return (Dir_Array::Add (data)); }
	
	Dir_Connect * New_Record (bool clear = false, int number = 1)
	                                       { return ((Dir_Connect *) Dir_Array::New_Record (clear, number)); }

	Dir_Connect * Record (int index)       { return ((Dir_Connect *) Dir_Array::Record (index)); }
	Dir_Connect * Record (void)            { return ((Dir_Connect *) Dir_Array::Record ()); }

	Dir_Connect * Get (int link)           { return ((Dir_Connect *) Dir_Array::Get (link)); }
	Dir_Connect * Get_GE (int link)        { return ((Dir_Connect *) Dir_Array::Get_GE (link)); }
	Dir_Connect * Get_LE (int link)        { return ((Dir_Connect *) Dir_Array::Get_LE (link)); }

	Dir_Connect * First (void)             { return ((Dir_Connect *) Dir_Array::First ()); }
	Dir_Connect * Next (void)              { return ((Dir_Connect *) Dir_Array::Next ()); }
	
	Dir_Connect * Last (void)              { return ((Dir_Connect *) Dir_Array::Last ()); }
	Dir_Connect * Previous (void)          { return ((Dir_Connect *) Dir_Array::Previous ()); }

	Dir_Connect * First_Key (void)         { return ((Dir_Connect *) Dir_Array::First_Key ()); }
	Dir_Connect * Next_Key (void)          { return ((Dir_Connect *) Dir_Array::Next_Key ()); }

	Dir_Connect * operator[] (int index)   { return (Record (index)); }
};

#endif
