//*********************************************************
//	Class_Index.hpp - ID index base class
//*********************************************************

#ifndef CLASS_INDEX_HPP
#define CLASS_INDEX_HPP

//---------------------------------------------------------
//	Class_Index class definition
//---------------------------------------------------------

class Class_Index
{
public:
	Class_Index (int id = 0)    { ID (id); }
	virtual ~Class_Index (void) {}

	int  ID (void)              { return (id); }
	void ID (int value)         { id = value; }

private:
	int  id;
};

//---------------------------------------------------------
//	Class2_Index class definition
//---------------------------------------------------------

class Class2_Index
{
public:
	Class2_Index (int key1 = 0, int key2 = 0)	
		{ Key1 (key1); Key2 (key2); }
	virtual ~Class2_Index (void) {}

	int  Key1 (void)             { return (key1); }
	int  Key2 (void)             { return (key2); }

	void Key1 (int value)        { key1 = value; }
	void Key2 (int value)        { key2 = value; }

private:
	int  key1;
	int  key2;
};

#endif
