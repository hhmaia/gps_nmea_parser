/*
 * gps_prv.h
 *
 *      Author: Henrique
 */

#include <mqx.h>
#include <fio.h>
#include "gps.h"
#include "commons_include.h"

#ifndef GPS_PRV_H_
#define GPS_PRV_H_

#define GPS_MAX_NMEA_TOKENS                 20U
#define GPS_MESSAGE_SIZE                    200U  //must be multiple of sizeof(_mqx_max_type)
#define GPS_QUEUE_SIZE                      5U

//timing constants
#define GPS_MSG_PERIOD                      MS_TO_TICKS(1000)
#define GPS_SETUP_DELAY                     MS_TO_TICKS(50)     //50U/SYSTEM_TICK_MS    //ms
#define GPS_RESET_DELAY                     MS_TO_TICKS(1000)   //1000U/SYSTEM_TICK_MS //ms
#define GPS_TIMEOUT_UART_PARSER             MS_TO_TICKS(2000)   //2000U/SYSTEM_TICK_MS //ms

typedef boolean (*NMEAParsingFunction)(GPS_TData*, char_ptr*);

typedef struct {
    boolean active;
    boolean debug;
    _mqx_uint error;
    GPS_TData* gpsDataPtr;
    MQX_FILE_PTR dev_fp;
    char_ptr stringBuffer;
}GPS_TNMEAParserTaskData;

void GPS_HardInit(GPS_TNMEAParserTaskData* tdata);
void GPS_NMEAParser(GPS_TNMEAParserTaskData* tdata);

#endif /* GPS_PRV_H_ */
