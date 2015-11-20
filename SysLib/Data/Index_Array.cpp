//*********************************************************
//	Index_Array.cpp - index array of data elements
//*********************************************************

#include "Index_Array.hpp"

#include <string.h>

//---------------------------------------------------------
//	Index_Array constructor
//---------------------------------------------------------

Index_Array::Index_Array (int data_size, int max_records, bool sort) :
	Data_Array ()
{
	index = NULL;
	root = 0;
	sorted = false;	

	Initialize (data_size, max_records, sort);
}

//---------------------------------------------------------
//	Initialize
//---------------------------------------------------------

bool Index_Array::Initialize (int data_size, int max_records, bool sort)
{
	Sorted (sort);

	return (Data_Array::Initialize (data_size, max_records));
}

//---------------------------------------------------------
//	Add
//---------------------------------------------------------

bool Index_Array::Add (void *data)
{
	if (!Data_Array::Add (data)) {
		return (false);
	}

	int record_number = Record_Index ();

	if (data == NULL) {
		data = Record (record_number);
	}

	//---- sorted data records ----

	if (sorted) {

		//---- add the first record ----

		if (record_number == 1) {
			root = 1;
			index [root] = record_number;
			return (true);
		}

		//---- add to the end of the list ----

		int low, high, sort;
		void *record;

		high = record_number - 1;
		record = Record (index [high]);
		sort = Sort_Function (data, record);

		if (sort >= 0) {
			root = record_number;
			index [root] = record_number;
			Record_Index (record_number);
			return (true);
		}

		//---- find the insert point ----

		low = 1;
		record = Record (index [low]);
		sort = Sort_Function (data, record);

		if (sort > 0) {

			//---- do a bisection search ----

			for (; low < high;) {
				root = (high + low) >> 1;
				record = Record (index [root]);
			
				sort = Sort_Function (data, record);

				if (sort == 0) {
					low = root;
					break;
				}
				if (sort < 0) {
					high = root;
				} else if (low < root) {
					low = root;
				} else {
					break;
				}
			}
			low++;
		}

		//---- insert the data ----

		root = low;
		size_t size = (size_t) (record_number - root) * sizeof (int);

		int *ptr = index + root;
		memmove (ptr+1, ptr, size);
		*ptr = record_number;

	} else {

		//---- create the ptree index ----

		int sort, *record_id;
		void *record;

		Index_Element *index_element = (Index_Element *) index;
		Index_Element *record_index = index_element + record_number;

		record_index->higher = record_index->lower = 0;

		if (record_number == 1) {
			root = record_number;
			return (true);
		}
		record_id = &root;

		while (*record_id > 0) {
			record = Record (*record_id);

			sort = Sort_Function (data, record);

			if (Split_Function (data, record)) {
				if (sort < 0) {
					record_index->higher = *record_id;
					record_index->lower = Cut_Lower (*record_id, data);
				} else {
					record_index->lower = *record_id;
					record_index->higher = Cut_Higher (*record_id, data);
				}
				break;
			} else {
				if (sort < 0) {
					record_id = &(index_element [*record_id].lower);
				} else {
					record_id = &(index_element [*record_id].higher);
				}
			}
		}
		*record_id = record_number;
	}
	Record_Index (record_number);
	return (true);
}

//---------------------------------------------------------
//	Get_Index
//---------------------------------------------------------

int  Index_Array::Get_Index (void *data)
{
	if (data && Num_Records ()) {
		if (sorted) {
			int low, high, sort, id;

			//---- check the low value ----

			low = root = 1;
			id = index [low];
			sort = Sort_Function (data, Record (id));

			if (sort == 0) return (id);
			if (sort < 0) return (root = 0);

			//---- check the high value ----

			high = root = Num_Records ();
			id = index [high];
			sort = Sort_Function (data, Record (id));

			if (sort == 0) return (id);
			if (sort > 0) return (root = 0);

			//---- do a bisection search ----

			for (; low < high;) {
				root = (high + low) >> 1;
				id = index [root];
			
				sort = Sort_Function (data, Record (id));

				if (sort == 0) return (id);
				if (sort < 0) {
					high = root;
				} else if (low < root) {
					low = root;
				} else {
					break;
				}
			}
			root = 0;

		} else {

			int sort, record_id;
			Index_Element *index_element = (Index_Element *) index;

			record_id = root;

			while (record_id > 0) {
				sort = Sort_Function (data, Record (record_id));

				if (sort == 0) break;

				if (sort > 0) {
					record_id = index_element [record_id].higher;
				} else {
					record_id = index_element [record_id].lower;
				}
			}
			return (record_id);
		}
	}
	return (0);
}

//---------------------------------------------------------
//	Get
//---------------------------------------------------------

void * Index_Array::Get (void *data)
{
	if (data && Num_Records ()) {
		if (sorted) {
			int low, high, sort;
			void *record;

			//---- check the low value ----

			low = root = 1;
			record = Record (index [low]);

			sort = Sort_Function (data, record);

			if (sort == 0) return (record);
			if (sort < 0) {
				root = 0;
				return (NULL);
			}

			//---- check the high value ----

			high = root = Num_Records ();
			record = Record (index [high]);

			sort = Sort_Function (data, record);

			if (sort == 0) return (record);
			if (sort > 0) {
				root = 0;
				return (NULL);
			}

			//---- do a bisection search ----

			for (; low < high;) {
				root = (high + low) >> 1;
				record = Record (index [root]);
			
				sort = Sort_Function (data, record);

				if (sort == 0) return (record);
				if (sort < 0) {
					high = root;
				} else if (low < root) {
					low = root;
				} else {
					break;
				}
			}
			root = 0;

		} else {

			int sort, record_id;
			void *record;
			Index_Element *index_element = (Index_Element *) index;

			record_id = root;

			while (record_id > 0) {
				record = Record (record_id);

				sort = Sort_Function (data, record);

				if (sort == 0) return (record);

				if (sort > 0) {
					record_id = index_element [record_id].higher;
				} else {
					record_id = index_element [record_id].lower;
				}
			}
		}
	}
	return (NULL);
}

//---------------------------------------------------------
//	Get_GE
//---------------------------------------------------------

void * Index_Array::Get_GE (void *data)
{
	if (data && Num_Records ()) {
		if (sorted) {
			int low, high, sort;
			void *record;

			//---- check the low value ----

			low = root = 1;
			record = Record (index [low]);

			sort = Sort_Function (data, record);

			if (sort <= 0) return (record);

			//---- check the high value ----

			high = root = Num_Records ();
			record = Record (index [high]);

			sort = Sort_Function (data, record);

			if (sort == 0) return (record);
			if (sort > 0) {
				root = 0;
				return (NULL);
			}

			//---- do a bisection search ----

			for (; low < high;) {
				root = (high + low) >> 1;
				record = Record (index [root]);
			
				sort = Sort_Function (data, record);

				if (sort == 0) return (record);
				if (sort < 0) {
					high = root;
				} else if (low < root) {
					low = root;
				} else {
					break;
				}
			}
			root = high;
			return (Record (index [root]));

		} else {

			int sort, record_id, higher;
			void *record;
			Index_Element *index_element = (Index_Element *) index;

			record_id = root;
			higher = 0;

			while (record_id > 0) {
				record = Record (record_id);

				sort = Sort_Function (data, record);

				if (sort == 0) return (record);

				if (sort > 0) {
					record_id = index_element [record_id].higher;
				} else {
					higher = record_id;
					record_id = index_element [record_id].lower;
				}
			}
			return (Record (higher));
		}
	}
	return (NULL);
}

//---------------------------------------------------------
//	Get_LE
//---------------------------------------------------------

void * Index_Array::Get_LE (void *data)
{
	if (data && Num_Records ()) {
		if (sorted) {
			int low, high, sort;
			void *record;

			//---- check the low value ----

			low = root = 1;
			record = Record (index [low]);

			sort = Sort_Function (data, record);

			if (sort == 0) return (record);
			if (sort < 0) {
				root = 0;
				return (NULL);
			}

			//---- check the high value ----

			high = root = Num_Records ();
			record = Record (index [high]);

			sort = Sort_Function (data, record);

			if (sort >= 0) return (record);

			//---- do a bisection search ----

			for (; low < high;) {
				root = (high + low) >> 1;
				record = Record (index [root]);
			
				sort = Sort_Function (data, record);

				if (sort == 0) return (record);
				if (sort < 0) {
					high = root;
				} else if (low < root) {
					low = root;
				} else {
					break;
				}
			}
			root = low;
			return (Record (index [root]));

		} else {

			int sort, record_id, lower;
			void *record;
			Index_Element *index_element = (Index_Element *) index;

			record_id = root;
			lower = 0;

			while (record_id > 0) {
				record = Record (record_id);

				sort = Sort_Function (data, record);

				if (sort == 0) return (record);

				if (sort > 0) {
					lower = record_id;
					record_id = index_element [record_id].higher;
				} else {
					record_id = index_element [record_id].lower;
				}
			}
			return (Record (lower));
		}
	}
	return (NULL);
}

//---------------------------------------------------------
//	First_Key
//---------------------------------------------------------

void * Index_Array::First_Key (void)
{
	if (sorted) {
		return (Check_Index (root = 1) ? Record (index [root]) : NULL);
	} else {
		int record_id;
		Index_Element *record_index;			
		Index_Element *index_element = (Index_Element *) index;

		record_id = root;

		while (record_id > 0) {
			record_index = index_element + record_id;

			if (record_index->lower == 0) break;

			record_id = record_index->lower;
		}
		return (Record (record_id));
	}
}

//---------------------------------------------------------
//	Next_Key
//---------------------------------------------------------

void * Index_Array::Next_Key (void)
{
	if (Record_Index () == 0) return (First_Key ());

	if (sorted) {
		return (Check_Index (++root) ? Record (index [root]) : NULL);
	} else {

		int record_id;
		Index_Element *record_index;
		Index_Element *index_element = (Index_Element *) index;

		record_id = Record_Index ();

		if (!Check_Index (record_id)) return (NULL);
			
		record_index = index_element + record_id;

		if (record_index->higher > 0) {
			record_id = record_index->higher;

			while (record_id > 0) {
				record_index = index_element + record_id;

				if (record_index->lower == 0) break;

				record_id = record_index->lower;
			}
		} else {
			int sort, higher;
			void *data, *record;

			data = Record (record_id);
			record_id = root;
			higher = 0;

			while (record_id > 0) {
				record = Record (record_id);

				sort = Sort_Function (data, record);

				if (sort == 0) break;

				if (sort > 0) {
					record_id = index_element [record_id].higher;
				} else {
					higher = record_id;
					record_id = index_element [record_id].lower;
				}
			}
			record_id = higher;
		}
		return (Record (record_id));
	}
}

//---------------------------------------------------------
//	Last_Key
//---------------------------------------------------------

void * Index_Array::Last_Key (void)
{
	if (sorted) {
		return (Check_Index (root = Num_Records ()) ? Record (index [root]) : NULL);
	} else {
		int record_id;
		Index_Element *record_index;			
		Index_Element *index_element = (Index_Element *) index;

		record_id = root;

		while (record_id > 0) {
			record_index = index_element + record_id;

			if (record_index->higher == 0) break;

			record_id = record_index->higher;
		}
		return (Record (record_id));
	}
}

//---------------------------------------------------------
//	Previous_Key
//---------------------------------------------------------

void * Index_Array::Previous_Key (void)
{
	if (sorted) {
		return (Check_Index (--root) ? Record (index [root]) : NULL);
	} else {

		int record_id;
		Index_Element *record_index;
		Index_Element *index_element = (Index_Element *) index;

		record_id = Record_Index ();

		if (!Check_Index (record_id)) return (NULL);
			
		record_index = index_element + record_id;

		if (record_index->lower > 0) {
			record_id = record_index->lower;

			while (record_id > 0) {
				record_index = index_element + record_id;

				if (record_index->higher == 0) break;

				record_id = record_index->higher;
			}
		} else {
			int sort, lower;
			void *data, *record;

			data = Record (record_id);
			record_id = root;
			lower = 0;

			while (record_id > 0) {
				record = Record (record_id);

				sort = Sort_Function (data, record);

				if (sort == 0) break;;

				if (sort > 0) {
					lower = record_id;
					record_id = index_element [record_id].higher;
				} else {
					record_id = index_element [record_id].lower;
				}
			}
			record_id = lower;
		}
		return (Record (record_id));
	}
}


//---------------------------------------------------------
//	Sort_Function
//---------------------------------------------------------

int Index_Array::Sort_Function (void *data1, void *data2)
{
	if (data1 == NULL || data2 == NULL) return (0);

	if (*((int *) data1) > *((int *) data2)) return (1);
	if (*((int *) data1) < *((int *) data2)) return (-1);
	return (0);
}

//---------------------------------------------------------
//	Split_Function
//---------------------------------------------------------

inline bool Index_Array::Split_Function (void *data1, void *data2)
{
	return (Key_Precedent (*((int *) data1), *((int *) data2)) > 0);
}

//---------------------------------------------------------
//	Key_Precedent
//---------------------------------------------------------

int Index_Array::Key_Precedent (int key1, int key2)
{
	int prec1, prec2;

	while (key1 && key2) {
		prec1 = (key1 - 1) ^ key1;
		prec2 = (key2 - 1) ^ key2;
		
		if (prec1 != prec2) {
			if (prec1 > prec2) return (1);
			return (-1);
		}
		key1 >>= 1;
		key2 >>= 1;
	}
	if (key1 > key2) return (1);
	if (key1 < key2) return (-1);
	return (0);
}

//---------------------------------------------------------
//	Cut_Lower
//---------------------------------------------------------

int Index_Array::Cut_Lower (int record_id, void *record)
{
	Index_Element *index_element = (Index_Element *) index;
	Index_Element *record_index = index_element + record_id;

	while (record_index->lower > 0) {
		if (Sort_Function (record, Record (record_index->lower)) >= 0) break;

		record_index = index_element + record_index->lower;
	}
	record_id = record_index->lower;

	if (record_id > 0) {
		record_index->lower = Cut_Higher (record_id, record);
	}
	return (record_id);
}

//---------------------------------------------------------
//	Cut_Higher
//---------------------------------------------------------

int Index_Array::Cut_Higher (int record_id, void *record) 
{
	Index_Element *index_element = (Index_Element *) index;
	Index_Element *record_index = index_element + record_id;

	while (record_index->higher > 0) {
		if (Sort_Function (record, Record (record_index->higher)) <= 0) break;

		record_index = index_element + record_index->higher;
	}
	record_id = record_index->higher;

	if (record_id > 0) {
		record_index->higher = Cut_Lower (record_id, record);
	}
	return (record_id);
}

//---------------------------------------------------------
//	Max_Records
//---------------------------------------------------------

bool Index_Array::Max_Records (int max_size)
{
	int max_record = Max_Records ();

	if (!Data_Array::Max_Records (max_size)) return (false);

	if (max_record == Max_Records ()) return (true);

	max_record = Max_Records ();

	if (max_record == 0) return (true);

	int index_size = (sorted) ? sizeof (int) : sizeof (Index_Element);

	size_t size = (size_t) (max_record + 1) * index_size;

	index = (int *) realloc (index, size);

	if (index == NULL) return (false);

	return (true);
}

//---------------------------------------------------------
//	Clear
//---------------------------------------------------------

void Index_Array::Clear (void)
{
	root = 0;
	if (index != NULL) {
		free (index);
		index = NULL;
	}
	Data_Array::Clear ();
}

//---------------------------------------------------------
//	Optimize
//---------------------------------------------------------

bool Index_Array::Optimize (bool compress)
{
	if (compress) {
		if (!Max_Records (Num_Records ())) return (false);
	}

	if (!sorted) {
		if (Max_Records () == 0) return (true);

		size_t size = (size_t) (Max_Records () + 1) * sizeof (int);

		int *new_index = (int *) malloc (size);

		if (new_index == NULL) return (false);

		int *index_ptr = new_index;

		for (void *record = First_Key (); record != NULL; record = Next_Key ()) {
			*(++index_ptr) = Record_Index ();
		}
		sorted = true;
		free (index);
		index = new_index;
	}
	return (true);
}

//---------------------------------------------------------
//	Sorted
//---------------------------------------------------------

bool Index_Array::Sorted (bool sort)
{
	if (sorted != sort) {
		int max_records = Max_Records ();

		if (Num_Records () != 0) Clear ();

		sorted = sort;

		return (Max_Records (max_records));
	}
	return (true);
}
