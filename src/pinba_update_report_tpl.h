
void PINBA_UPDATE_REPORT_ADD_FUNC_D() /*pinba_update_report1_add(pinba_report *report, const pinba_stats_record *record)*/ /* {{{ */
{
	pinba_report *report = (pinba_report *)rep;
	PINBA_REPORT_DATA_STRUCT_D();
	/*struct pinba_report1_data *data;*/
	PINBA_REPORT_INDEX_D();

	pinba_timeradd(&report->time_total, &record->data.req_time, &report->time_total);
	pinba_timeradd(&report->ru_utime_total, &record->data.ru_utime, &report->ru_utime_total);
	pinba_timeradd(&report->ru_stime_total, &record->data.ru_stime, &report->ru_stime_total);
	report->kbytes_total += record->data.doc_size;
	report->memory_footprint += record->data.memory_footprint;

#if PINBA_REPORT_NO_INDEX()
	report->std.results_cnt++;
	PINBA_UPDATE_HISTOGRAM_ADD(report, report->std.histogram_data, record->data.req_time);
#else 
	{
		PINBA_INDEX_VARS_D();
		/*int index_len, dummy;*/

		PINBA_CREATE_INDEX_VALUE();

		data = (PINBA_REPORT_DATA_STRUCT() *)pinba_map_get(report->results, index);

		if (UNLIKELY(!data)) {
			/* no such value, insert */
			data = (PINBA_REPORT_DATA_STRUCT() *)calloc(1, sizeof(PINBA_REPORT_DATA_STRUCT()));

			data->histogram_data = pinba_lmap_create();
			PINBA_REPORT_ASSIGN_DATA();

			report->results = pinba_map_add(report->results, index, data);
			report->std.results_cnt++;
		}

		data->req_count++;
		pinba_timeradd(&data->req_time_total, &record->data.req_time, &data->req_time_total);
		pinba_timeradd(&data->ru_utime_total, &record->data.ru_utime, &data->ru_utime_total);
		pinba_timeradd(&data->ru_stime_total, &record->data.ru_stime, &data->ru_stime_total);
		data->kbytes_total += record->data.doc_size;
		data->memory_footprint += record->data.memory_footprint;
		PINBA_UPDATE_HISTOGRAM_ADD(report, data->histogram_data, record->data.req_time);
	}
#endif
}
/* }}} */

void PINBA_UPDATE_REPORT_DELETE_FUNC_D() /* pinba_update_report1_delete(pinba_report *report, const pinba_stats_record *record)*/ /* {{{ */
{
	pinba_report *report = (pinba_report *)rep;
	PINBA_REPORT_DATA_STRUCT_D();
	PINBA_REPORT_INDEX_D();

	if (report->std.results_cnt == 0) {
		return;
	}

	PINBA_REPORT_DELETE_CHECK(report, record);

	pinba_timersub(&report->time_total, &record->data.req_time, &report->time_total);
	pinba_timersub(&report->ru_utime_total, &record->data.ru_utime, &report->ru_utime_total);
	pinba_timersub(&report->ru_stime_total, &record->data.ru_stime, &report->ru_stime_total);
	report->kbytes_total -= record->data.doc_size;
	report->memory_footprint -= record->data.memory_footprint;

#if PINBA_REPORT_NO_INDEX()
	report->std.results_cnt--;
	PINBA_UPDATE_HISTOGRAM_DEL(report, report->std.histogram_data, record->data.req_time);
#else 
	{
		PINBA_INDEX_VARS_D();
		/*int index_len, dummy;*/

		PINBA_CREATE_INDEX_VALUE();

		data = (PINBA_REPORT_DATA_STRUCT() *)pinba_map_get(report->results, index);

		if (UNLIKELY(!data)) {
			/* no such value, mmm?? */
		} else {

			if (UNLIKELY(data->req_count == 1)) {
				pinba_lmap_destroy(data->histogram_data);
				free(data);
				pinba_map_delete(report->results, index);
				report->std.results_cnt--;
			} else {
				data->req_count--;
				pinba_timersub(&data->req_time_total, &record->data.req_time, &data->req_time_total);
				pinba_timersub(&data->ru_utime_total, &record->data.ru_utime, &data->ru_utime_total);
				pinba_timersub(&data->ru_stime_total, &record->data.ru_stime, &data->ru_stime_total);
				data->kbytes_total -= record->data.doc_size;
				data->memory_footprint -= record->data.memory_footprint;
				PINBA_UPDATE_HISTOGRAM_DEL(report, data->histogram_data, record->data.req_time);
			}
		}
	}
#endif
}
/* }}} */
