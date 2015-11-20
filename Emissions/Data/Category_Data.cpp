//*********************************************************
//	Category_Data.cpp - emission rate category classes
//*********************************************************

#include "Category_Data.hpp"

#include "Utility.hpp"

#include <string.h>

//---------------------------------------------------------
//	Category_Data constructor
//---------------------------------------------------------

Category_Data::Category_Data (int number, bool flag) : Class_Index (number)
{
	Key_Flag (flag);
	name [0] = '\0';
	base_index = 1;
}

//---- Add Member ----

int Category_Data::Add_Member (int value, char *string)
{
	double weight;
	char *str_ptr, buffer [FIELD_BUFFER], *ptr, *buf;

	Value_Map map;
	Label_Data label;
	
	label.Value (value);

	if (string == NULL || (*string >= '0' && *string <= '9')) {
		str_fmt (buffer, sizeof (buffer), "%d", value);
		str_ptr = string;
	} else {
		str_ptr = Get_Token (string, buffer, sizeof (buffer));
	}
	label.Label (buffer);

	if (!label_data.Add (&label)) return (0);

	map.Group (0);
	map.Member (label_data.Num_Records ());

	if (str_ptr == NULL) {
		map.Value (value);
		map.Weight (1.0);

		if (!value_map.Add (&map)) return (0);

	} else {

		while (str_ptr != NULL) {
			str_ptr = Get_Token (str_ptr, buffer, sizeof (buffer));
			if (buffer [0] == '\0') break;

			buf = buffer;

			//---- check for a file group ----

			ptr = strchr (buf, '#');
			if (ptr == NULL) {
				map.Group (0);
			} else {
				map.Group (atoi (buf));
				buf = ptr + 1;
			}

			//---- check for a weighting factor ----

			ptr = strchr (buf, '*');
			value = atoi (buf);

			if (ptr == NULL) {
				weight = 1.0;
			} else {
				weight = atof (ptr + 1);
			}
			map.Value (value);
			map.Weight (weight);

			if (!value_map.Add (&map)) return (0);
		}
	}
	return (map.Member ());
}

//---- First_Member ----

Value_Map * Category_Data::First_Member (int value, int group)
{
	int member = Member (value);

	Value_Map *map_ptr;

	for (map_ptr = value_map.First (); map_ptr; map_ptr = value_map.Next ()) {
		if (map_ptr->Member () == member && (map_ptr->Group () == group || map_ptr->Group () == 0)) break;
	}
	return (map_ptr);
}

//---- Next_Member ----

Value_Map * Category_Data::Next_Member (int value, int group)
{
	int member = Member (value);

	Value_Map *map_ptr;

	for (map_ptr = value_map.Next (); map_ptr; map_ptr = value_map.Next ()) {
		if (map_ptr->Member () == member && (map_ptr->Group () == group || map_ptr->Group () == 0)) break;
	}
	return (map_ptr);		
}

//---- Max_Value ----

int Category_Data::Max_Value (void)
{
	int max_value = 0;

	Value_Map *map_ptr;

	for (map_ptr = value_map.First (); map_ptr; map_ptr = value_map.Next ()) {
		if (map_ptr->Value () > max_value) max_value = map_ptr->Value ();
	}
	return (max_value);		
}

//---------------------------------------------------------
//	Category_Array constructor
//---------------------------------------------------------

Category_Array::Category_Array (int max_records) : 
	Class_Array (sizeof (Category_Data), max_records)
{
}

Category_Data * Category_Array::Get (int value, bool flag)
{ 
	Category_Data *ptr = (Category_Data *) Class_Array::Get (value); 
	if (flag && ptr != NULL) {
		if (!ptr->Key_Flag ()) {
			ptr = NULL;
		}
	}
	return (ptr);
}
