//*********************************************************
//	Compare.cpp - Compare Plan Records
//*********************************************************

#include "PlanPrep.hpp"

#include "Network_Code.hpp"

//---------------------------------------------------------
//	Compare
//---------------------------------------------------------

void PlanPrep::Compare (Plan_Data *plan, Plan_Data *base)
{
	int i, j, tod, plan_time, base_time, diff, num1, num2, *node1, *node2, *a1, *a2, *b1, *b2, j1;
	bool found;

	if (plan->start_type == LOCATION_ID || plan->end_type == LOCATION_ID) return;
	if (base->start_type == LOCATION_ID || base->end_type == LOCATION_ID) return;

	tod = (plan->key2 + plan->stop_time) >> 1;

	if (tod >= MIDNIGHT) tod -= MIDNIGHT;

	tod = tod / increment;

	plan_time = plan->duration;
	base_time = base->duration;

	diff = abs (plan_time - base_time);

	if (base_time > 0) {
		diff = j = (diff * NUM_CHANGE_BINS + (base_time >> 1)) / base_time;
	} else {
		diff = j = 0;
	}

	if (j >= NUM_CHANGE_BINS) j = NUM_CHANGE_BINS - 1;

	time_changes [tod] [j]++;
	time_changes [TOTAL_TIME_PERIOD] [j]++;

	if (diff >= dump_time) {
		Print (2, "Travel Time Dump: Traveler=%d, Start Time=%d, Time1=%.0lf, Time2=%.0lf, Change=%.1lf%%", 
			plan->key1, plan->key2, base_time, plan_time, diff * 100.0 / NUM_CHANGE_BINS);
	}

	if (!path_flag) return;
	if (plan->mode != Plan_File::AUTO_MODE || plan->driver_flag == 0) return;
	if (base->mode != Plan_File::AUTO_MODE || plan->driver_flag == 0) return;

	num1 = base->tokens - 2; // - base->data [1];
	node1 = base->data + 2;

	if (num1 == 0) return;

	num2 = plan->tokens - 2; // - plan->data [1];
	node2 = plan->data + 2;

	diff = 0;

	j1 = 1;
	a1 = b1 = node1;

	for (i=1; i < num1; i++, a1++) {
		b1++;

		a2 = b2 = node2;
		found = false;

		for (j=j1; j < num2; j++, a2++) {
			b2++;
			if (*a1 == *a2 && *b1 == *b2) {
				node2 += j - j1 + 1;
				j1 = j + 1;
				found = true;
				break;
			}
		}
		if (!found) diff++;
	}
	diff = j = (diff * NUM_CHANGE_BINS + (num1 >> 1)) / num1;

	if (j >= NUM_CHANGE_BINS) j = NUM_CHANGE_BINS - 1;

	path_changes [tod] [j]++;
	path_changes [TOTAL_TIME_PERIOD] [j]++;

	if (diff >= dump_path) {
		Print (2, "Path Dump: Traveler=%d, Start Time=%d, Change=%.1lf%%", 
			plan->key1, plan->key2, diff * 100.0 / NUM_CHANGE_BINS);

		Print (1, "Path 1: ");
		num1 = base->tokens - 2; // - base->data [1];
		node1 = base->data + 2;

		for (i=0; i < num1; i++, node1++) {
			if (i && !(i % 10)) {
				Print (1, "\t");
			}
			Print (0, "%d ", *node1);
		}
		Print (1, "Path 2: ");
		num2 = plan->tokens - 2; // - plan->data [1];
		node2 = plan->data + 2;

		for (i=0; i < num2; i++, node2++) {
			if (i && !(i % 10)) {
				Print (1, "\t");
			}
			Print (0, "%d ", *node2);
		}
	}
}
