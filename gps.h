/*
 * gps.h
 *
 *      Author: Henrique
 */

#include <mqx.h>
#include <fio.h>
#include <lwsem.h>

#ifndef GPS_HEADER_H_
#define GPS_HEADER_H_

typedef struct{
    uint_8 hours;
    uint_8 minutes;
    uint_8 seconds;
    uint_8 reserved;
    uint_8 day;
    uint_8 month;
    uint_16 year;
}GPS_TTimeDate;

typedef struct{
    ieee_single latitude;
    ieee_single longitude;
    uchar NorS;
    uchar WorE;
    uint_16 reserved;
}GPS_TCoordinate;

typedef struct{
    ieee_single PDOP;
    ieee_single speed;
    ieee_single COV;
    GPS_TTimeDate date;
    GPS_TCoordinate coordinate;
    boolean GPS_Status;
}GPS_TData;

#define GPS_SIGNED_LAT(cord_ptr)   ( (cord_ptr)->NorS == 'N' ? (cord_ptr)->latitude : -(cord_ptr)->latitude )
#define GPS_SIGNED_LON(cord_ptr)   ( (cord_ptr)->WorE == 'E' ? (cord_ptr)->longitude : -(cord_ptr)->longitude )

void GPS_TaskNMEAParser(uint32_t task_init_data);
void GPS_GetData(GPS_TData* data);
void GPS_SetDebug(boolean state);

#endif /* GPS_HEADER_H_ */
