/*
 * gps.c
 *
 *      Author: Henrique
 */

#include <mqx.h>
#include <fio.h>
#include <lwsem.h>
#include "gps.h"
#include "gps_prv.h"

LWSEM_STRUCT GPS_Data_LWSEM;

static GPS_TData GPS_Data = { .PDOP = 999.9 };
static char gps_input_stringBuffer[GPS_MESSAGE_SIZE];
static GPS_TNMEAParserTaskData gps_task_data = {
    .active = TRUE,
    .debug = FALSE,
    .gpsDataPtr = &GPS_Data,
    .stringBuffer = gps_input_stringBuffer
};

void GPS_TaskNMEAParser(uint32_t task_init_data){
    _lwsem_create(&GPS_Data_LWSEM, 1);
    GPS_HardInit(&gps_task_data);
    GPS_NMEAParser(&gps_task_data);
}

void GPS_GetData(GPS_TData* data){
    _lwsem_wait(&GPS_Data_LWSEM);
    *data = GPS_Data;
    _lwsem_post(&GPS_Data_LWSEM);
}

void GPS_SetDebug(boolean state){
    gps_task_data.debug = state;
}

