/*
 * gps_parsing.c
 *
 *      Author: Henrique
 */

#include <mqx.h>
#include <stdlib.h>
#include <fio.h>
#include <stdlib.h>
#include <lwsem.h>
#include <string.h>

#include "gps.h"
#include "gps_prv.h"
#include "nmea.h"

extern LWSEM_STRUCT GPS_Data_LWSEM;
static ieee_single ToDecDegrees(char_ptr str){

    uint32 degree;
    ieee_single dec, aux;

    degree = atof(str)/100;
    dec = (atof(str) - (degree * 100))/60;
    aux = degree + dec;

    return aux;

}

static boolean ParseGSA(GPS_TData* data, char_ptr* tokens){
    enum {
        MODE1 = 0,
        MODE2,
        SATELLITE1,
        SATELLITE2,
        SATELLITE3,
        SATELLITE4,
        SATELLITE5,
        SATELLITE6,
        SATELLITE7,
        SATELLITE8,
        SATELLITE9,
        SATELLITE10,
        SATELLITE11,
        SATELLITE12,
        PDOP,
        HDOP,
        VDOP,
        CHECKSUMGSA
    };

    if(strlen(tokens[PDOP]) > 2){
        data->PDOP = (ieee_single)atof(tokens[PDOP]);
    }else{
        return FALSE;
    }

    return TRUE;
}

static boolean ParseRMC(GPS_TData* data, char_ptr* tokens){

    uint32 aux;

    enum {
        UTCTIME = 0,
        STATUS,
        LATITUDE,
        CARDINAL_NS,
        LONGITUDE,
        CARDINAL_WE,
        SPEED,
        COV,
        DATE,
        MAGNETIC,
        DECLINATION,
        MODE_INDICATOR,
        CHECKSUMRMC
    };

    if(tokens[STATUS][0] == 'A'){

        data->coordinate.latitude = ToDecDegrees(tokens[LATITUDE]);
        data->coordinate.longitude = ToDecDegrees(tokens[LONGITUDE]);
        data->coordinate.NorS = tokens[CARDINAL_NS][0];
        data->coordinate.WorE = tokens[CARDINAL_WE][0];

        aux = atol(tokens[UTCTIME]);
        data->date.hours = aux/10000;
        data->date.minutes = (aux%10000)/100;
        data->date.seconds = aux%100;

        aux = atol(tokens[DATE]);
        data->date.day = aux/10000;
        data->date.month = (aux%10000)/100;
        data->date.year = (aux%100) + 2000;

        data->COV = (ieee_single)atof(tokens[COV]);
	
    	return TRUE;
    }

    return FALSE;

}

static boolean ParseVTG(GPS_TData* data, char_ptr* tokens){

    enum {
        COURSE = 0,
        REFERENCE,
        COURSE2,
        REFERENCE2,
        SPEED_KNOTS,
        KNOTS,
        SPEED_KM,
        KM,
        MODE,
        CHECKSUMVTG
    };

    if(strlen(tokens[SPEED_KM]) > 2){
       // obtem a precisão
       data->speed = (ieee_single)atof(tokens[SPEED_KM]);
    }

    if(strlen(tokens[COURSE]) > 2){
        data->COV = (ieee_single)atof(tokens[COURSE]);
        return TRUE;
    }

    return FALSE;
}

struct {
    NMEAParsingFunction function;
    const char_ptr const key;
} static const FunctionTable[] = {
        {ParseGSA, "GPGSA"},
        {ParseRMC, "GPRMC"},
        {ParseVTG, "GPVTG"},
        {NULL, ""}
};

NMEAParsingFunction GPS_getFunctionFromTable(char_ptr key){

    byte i;

    for(i = 0; NULL != FunctionTable[i].function && strcmp(key, FunctionTable[i].key); i++);

    return FunctionTable[i].function;
}

void GPS_NMEAParser(GPS_TNMEAParserTaskData* tdata){

    char_ptr tokens[30];
    NMEAParsingFunction parsingFunction;

    while(tdata->active){

        if(fscanf(tdata->dev_fp, "$%s\n", tdata->stringBuffer) == 1){

            if(tdata->debug){
                printf("** %s\n", tdata->stringBuffer);
            }

            if(NMEA_ChkValidate(tdata->stringBuffer)){

                UTILS_Tokenizer(tokens, tdata->stringBuffer, ',');
                parsingFunction = GPS_getFunctionFromTable(tokens[0]);

                if(parsingFunction != NULL){
                    _lwsem_wait(&GPS_Data_LWSEM);
                    parsingFunction(tdata->gpsDataPtr, &tokens[1]);
                    _lwsem_post(&GPS_Data_LWSEM);
                }
            }
        }
    }
}

