/*
 * nmea.h
 *
 *      Author: Henrique
 */

#include "Cpu.h"
#include "lwmsgq.h"
#include "lwsem.h"
#include "mutex.h"

#ifndef NMEA_H_
#define NMEA_H_

typedef enum{
    IDLE = 0,
    RECEIVING,
    CHECKSUM,
    DONE
}NMEA_TUartParserState;

typedef struct{
    boolean active;
    _mqx_uint timeoutTicks;
    _mqx_uint mqxError;
    uchar_ptr strBuffer;
    uint16 strBufferSize;
    uint16 strBufferIndex;
    uchar buffer;
    NMEA_TUartParserState state;
    LWMSGQ_STRUCT_PTR outputQueuePtr;
    LWSEM_STRUCT_PTR uartSemPtr;
    MUTEX_STRUCT_PTR uartMutexPtr;
    void (*initFunction)(void);
    byte (*recvFunction)(uchar *);
}NMEA_TUartParserTaskData;

boolean NMEA_ChkValidate(char* pstr);
byte NMEA_ChkCalc(char* pstr, bool append);
void NMEA_UartParser(NMEA_TUartParserTaskData* tdata);

#endif /* NMEA_H_ */
