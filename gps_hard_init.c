/*
 * gps_hard_init.c
 *
 *      Author: Henrique
 */

#include <mqx.h>
#include <fio.h>
#include <bsp.h>
#include "gps_prv.h"

//PMTK (GlobalTec module chipset MT3339) configuration strings
static const char_ptr const CConfigStrings[] = {
//    "$PMTK104*37\n",                 // Cold Start
    "$PMTK102*31\n",                 // Warm Start
//    "$PMTK101*32\n",                 // Hot Start
    "$PMTK220,1000*1F\n",            // Update rate 1000 ms
    "$PMTK300,1000,0,0,0,0*1C\n",    // Fix Interval 1000 ms
    "$PMTK314,0,1,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0*29\n", // Set NMEA Outputs (GSA, RMC & VTG)
    "$PMTK251,115200*1F\n",          // Baudrate 115200 baud
    NULL
};

void GPS_HardInit(GPS_TNMEAParserTaskData* tdata){

    uint32 i, baud = BSP_GPS_SERIAL_BAUD_RATE;

    tdata->dev_fp = fopen(BSP_GPS_SERIAL_DEVICE, (char_ptr)BSP_GPS_SERIAL_FLAGS);

    if(NULL == tdata->dev_fp){
    	printf(":: Erro ao abrir o driver serial %s. Erro = %x\n\n", BSP_GPS_SERIAL_DEVICE, _task_get_error());
    	_task_block();
    }

    for(i=0; CConfigStrings[i] != NULL; i++){
    	fprintf(tdata->dev_fp, "%s", CConfigStrings[i]);
    	_time_delay_ticks(GPS_RESET_DELAY);
    }

    ioctl(tdata->dev_fp, IO_IOCTL_SERIAL_SET_BAUD, &baud);

}
