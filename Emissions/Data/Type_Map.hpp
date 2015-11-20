//*********************************************************
//	Type_Map.hpp - map for types and subtypes
//*********************************************************

#ifndef TYPE_MAP_HPP
#define TYPE_MAP_HPP

#include "Class_Array.hpp"

//---------------------------------------------------------
//	Type_Map class definition
//---------------------------------------------------------

class Type_Map : public Class_Index
{
public:
	Type_Map (int type = 0, int subtype = 0);

	int    Type (void)               { return (ID () >> 16); }
	int    SubType (void)            { return (ID () & 0xFFFF); }
	int    Map (void)                { return (map); }

	void   Type (int value)          { ID ((value << 16) + SubType ()); }
	void   SubType (int value)       { ID ((Type () << 16) + value); }
	void   Map (int value)           { map = value; }

private:
	int    map;
};

//---------------------------------------------------------
//	Type_Map_Array class definition
//---------------------------------------------------------

class Type_Map_Array : public Class_Array
{
public:
	Type_Map_Array (int max_records = 10);

	bool Add (Type_Map *data = NULL)      { return (Class_Array::Add (data)); }

	Type_Map * Record (int index)         { return ((Type_Map *) Class_Array::Record (index)); }
	Type_Map * Record (void)              { return ((Type_Map *) Class_Array::Record ()); }

	Type_Map * Get (int type, int subtype = 0) { 
                                             Type_Map map (type, subtype);
										     return ((Type_Map *) Class_Array::Get (&map)); 
	                                       }

	Type_Map * First (void)                { return ((Type_Map *) Class_Array::First ()); }
	Type_Map * Next (void)                 { return ((Type_Map *) Class_Array::Next ()); }

	Type_Map * operator[] (int index)      { return (Record (index)); }

	int  Map (int type, int subtype = 0)   { Type_Map *map = Get (type, subtype);
	                                         return ((map) ? map->Map () : 0); }
};

#endif
