//*********************************************************
//	Transfer_Data.cpp - transfers at transit stops
//*********************************************************

#include "Transfer_Data.hpp"

//---------------------------------------------------------
//	Transfer_Data constructor
//---------------------------------------------------------

Transfer_Data::Transfer_Data (int id) : Stop_Data (id)
{
	Walk_Board (0);
	Bus_Board (0);
	Rail_Board (0);
	Drive_Board (0);
	Walk_Alight (0);
	Bus_Alight (0);
	Rail_Alight (0);
	Drive_Alight (0);
}

//---------------------------------------------------------
//	Transfer_Array constructor
//---------------------------------------------------------

Transfer_Array::Transfer_Array (int max_records) : 
	Stop_Array (max_records)
{
	Data_Size (sizeof (Transfer_Data));
}
