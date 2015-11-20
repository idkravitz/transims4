//*********************************************************
//	Connection.hpp - directional link connections
//*********************************************************

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include "Class_Index.hpp"
#include "Class_Array.hpp"
#include "Network_Code.hpp"

//---------------------------------------------------------
//	Connection class definition
//---------------------------------------------------------

class Connection : public Class_Index
{
public:
	Connection (int change = 0);

	int  Change (void)             { return (ID () - 360); }
	int  Dir (void)                { return (dir); }
	int  Facility (void)           { return (facility); }
	int  Index (void)              { return (index); }
	int  Type (void)               { return (type); }

	bool Is_Thru (void)            { return (type == Network_Code::THRU); }
	bool Is_UTurn (void)           { return (type == Network_Code::UTURN); }
	bool Is_Left (void)            { return (type == Network_Code::LEFT); }
	bool Is_Right (void)           { return (type == Network_Code::RIGHT); }
	bool Is_R_Merge (void)         { return (type == Network_Code::R_MERGE); }
	bool Is_R_Split (void)         { return (type == Network_Code::R_SPLIT); }
	bool Is_L_Merge (void)         { return (type == Network_Code::L_MERGE); }
	bool Is_L_Split (void)         { return (type == Network_Code::L_SPLIT); }
	bool Is_Other (void)           { return (type == Network_Code::NO_TYPE); }

	void Change (int value)        { ID (value + 360); }
	void Dir (int value)           { dir = value; }
	void Facility (int value)      { facility = value; }
	void Index (int value)         { index = value; }
	void Type (int value)          { type = value; };

	void Set_Thru (void)           { type = Network_Code::THRU; }
	void Set_UTurn (void)          { type = Network_Code::UTURN; }
	void Set_Left (void)           { type = Network_Code::LEFT; }
	void Set_Right (void)          { type = Network_Code::RIGHT; }
	void Set_R_Merge (void)        { type = Network_Code::R_MERGE; }
	void Set_R_Split (void)        { type = Network_Code::R_SPLIT; }
	void Set_L_Merge (void)        { type = Network_Code::L_MERGE; }
	void Set_L_Split (void)        { type = Network_Code::L_SPLIT; }
	void Set_Other (void)          { type = Network_Code::NO_TYPE; }

private:
	int dir;
	int facility;
	int index;
	int type;
};

//---------------------------------------------------------
//	Connection_Array class definition
//---------------------------------------------------------

class Connection_Array : public Class_Array
{
public:
	Connection_Array (int max_records = 0);
	virtual ~Connection_Array (void)       { Clear (); }

	Connection * Get_Thru (void);
	Connection * Get_Dir (int dir);

	bool Add (Connection *data = NULL)     { return (Class_Array::Add (data)); }
	
	Connection * New_Record (bool clear = false, int number = 1)
	                                       { return ((Connection *) Class_Array::New_Record (clear, number)); }

	Connection * Get (int change)          { return ((Connection *) Class_Array::Get (change)); }
	Connection * Get_GE (int change)       { return ((Connection *) Class_Array::Get_GE (change)); }
	Connection * Get_LE (int change)       { return ((Connection *) Class_Array::Get_LE (change)); }

	Connection * Record (int index)        { return ((Connection *) Class_Array::Record (index)); }
	Connection * Record (void)             { return ((Connection *) Class_Array::Record ()); }

	Connection * First (void)              { return ((Connection *) Class_Array::First ()); }
	Connection * Next (void)               { return ((Connection *) Class_Array::Next ()); }
	
	Connection * Last (void)               { return ((Connection *) Class_Array::Last ()); }
	Connection * Previous (void)           { return ((Connection *) Class_Array::Previous ()); }
	
	Connection * First_Key (void)          { return ((Connection *) Class_Array::First_Key ()); }
	Connection * Next_Key (void)           { return ((Connection *) Class_Array::Next_Key ()); }
	
	Connection * Last_Key (void)           { return ((Connection *) Class_Array::Last_Key ()); }
	Connection * Previous_Key (void)       { return ((Connection *) Class_Array::Previous_Key ()); }

	Connection * operator[] (int index)    { return (Record (index)); }
};

#endif
