//*********************************************************
//	Sum_Group.cpp - Summarize Group Statistics
//*********************************************************

#include "Validate.hpp"

#include <math.h>

//---------------------------------------------------------
//	Sum_Group
//---------------------------------------------------------

void Validate::Sum_Group (int group, Link_Data *link_ptr, int dir)
{
	int i, j, num;
	double volume, count, capacity, error, vc;
	bool flag;	

	TTime_Data *ttime_ptr;
	Group_Data *data, data_rec;

	if (group == 0) return;
	
	//---- check for counts ----

	ttime_ptr = ttime_data [dir];

	num = ttime_ptr->Periods ();

	if (num < 1) return;

	count = 0;

	for (i=1; i <= num; i++) {
		count += ttime_ptr->Count (i);
	}
	if (count == 0) return;

	//---- get the data record ----

	if (group < 0) {
		flag = true;
	} else {
		flag = false;

		data = (Group_Data *) group_data.Get (&group);

		if (data == NULL) {
			memset (&data_rec, '\0', sizeof (Group_Data));

			data_rec.index = group;
			if (!group_data.Add (&data_rec)) {
				Error ("Adding Group %d to Group Data", group);
			}
			data = (Group_Data *) group_data.Record ();
		}
	}

	//---- process each period ----

	for (i=1; i <= num; i++) {

		volume = ttime_ptr->Volume (i);
		count = ttime_ptr->Count (i);
		error = fabs (volume - count);

		if (flag) {
			data = NULL;

			for (j=0; volume_level [j] > 0; j++) {
				if (count < volume_level [j]) {
					group = j + 1;

					data = (Group_Data *) group_data.Get (&group);

					if (data == NULL) {
						memset (&data_rec, '\0', sizeof (Group_Data));

						data_rec.index = group;
						if (!group_data.Add (&data_rec)) {
							Error ("Adding Group %d to Group Data", group);
						}
						data = (Group_Data *) group_data.Record ();
					}
					break;
				}
			}
			if (data == NULL) continue;
		}
		
		capacity = ttime_ptr->Capacity () * hours;
		if (capacity > 0.0) {
			vc = volume / capacity;
			if (vc > 99.99) vc = 99.99;
		} else {
			vc = 0.0;
		}

		if (method) {
			double length = UnRound (link_ptr->Length ()) / 1000.0;

			volume *= length;
			count *= length;
			error *= length;
			capacity *= length;
		}

		//---- sum to category ----

		data->number++;

		data->volume += volume;
		data->volume_sq += volume * volume;

		data->count += count;
		data->count_sq += count * count;

		data->error += error;
		data->error_sq += error * error;
		
		data->count_volume += count * volume;

		data->capacity += capacity;
		if (vc > data->max_vc) data->max_vc = vc;

		//---- add to total as well ----

		total.number++;

		total.volume += volume;
		total.volume_sq += volume * volume;

		total.count += count;
		total.count_sq += count * count;

		total.error += error;
		total.error_sq += error * error;
		
		total.count_volume += count * volume;

		total.capacity += capacity;
		if (vc > total.max_vc) total.max_vc = vc;
	}
}
