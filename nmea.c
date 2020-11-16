/*
 * nmea.c
 *
 *      Author: Henrique
 */

#include <mqx.h>
#include <fio.h>

#include "nmea.h"
#include "string.h"

byte NMEA_ChkCalc(char_ptr pstr, bool append){
    
    byte chk = 0x00;
    uint16 i;
    
    chk = pstr[1];
    
    for(i=2; i<strlen(pstr); i++){
        chk ^= pstr[i];
    }
    
    if(append){
    	snprintf(&pstr[strlen(pstr)], strlen(pstr), "*%02X\r\n", chk);
    }
    
    return chk;
    
}

boolean NMEA_ChkValidate(char_ptr pstr){
    
    byte i, acc = 0;
    uint32 checksum;
    
    if(strlen(pstr) < 4){
        return FALSE;
    }
    
    for(i=0; i < strlen(pstr) - 3; i++){
        acc ^= pstr[i];
    }
    
    sscanf(&pstr[strlen(pstr) - 2], "%x", &checksum);
    
    if(acc == (byte)checksum){
        return TRUE;
    }else{
        return FALSE;        
    }
    
}

void NMEA_UartParser(NMEA_TUartParserTaskData* tdata){
    
    tdata->initFunction();
    
    while(tdata->active){
        
        tdata->mqxError = _mutex_lock(tdata->uartMutexPtr);
        tdata->mqxError = _lwsem_wait_ticks(tdata->uartSemPtr, tdata->timeoutTicks);
        _mutex_unlock(tdata->uartMutexPtr);
                
        if(tdata->mqxError == MQX_INVALID_LWSEM){
            
            _sched_yield();
            
        }else if(tdata->mqxError == MQX_LWSEM_WAIT_TIMEOUT){
            
            tdata->state = IDLE;
            
        }else if(tdata->mqxError == MQX_OK){
        
            tdata->recvFunction(&tdata->buffer);
            
            if(tdata->strBufferIndex >= tdata->strBufferSize){
                tdata->strBufferIndex = 0;
                tdata->state = IDLE;
            }
                        
            switch(tdata->state){
            
                case IDLE:
                    
                    if(tdata->buffer == '$'){
                        tdata->strBufferIndex = 0;
                        tdata->state = RECEIVING;
                    }
                    
                    break;
                    
                case RECEIVING:
                    
                    switch(tdata->buffer){
                    
                        case '$':
                            tdata->strBufferIndex = 0;
                            break;
                            
                        case '\r':
                            tdata->state = CHECKSUM;
                            break;
                            
                        case '\n':
                            tdata->state = IDLE;                                
                            break;
                            
                        default:
                            
                            if(tdata->strBufferIndex < tdata->strBufferSize-1){
                                tdata->strBuffer[tdata->strBufferIndex++] = tdata->buffer;
                            }else{
                                tdata->state = IDLE;
                            }
                            
                            break;
                            
                    }
                    
                    break;
                    
                case CHECKSUM:
                    
                    if(tdata->buffer == '\n'){
                        tdata->strBuffer[tdata->strBufferIndex] = '\0';
                        
                        if(NMEA_ChkValidate(tdata->strBuffer)){
                            tdata->strBuffer[tdata->strBufferIndex - 3] = '\0';
                            _lwmsgq_send(tdata->outputQueuePtr, 
                                    (pointer)tdata->strBuffer, LWMSGQ_SEND_BLOCK_ON_FULL);
                            
                        }
                    }
                    
                    tdata->state = IDLE;
                    break;
                    
                default:
                    
                    tdata->state = IDLE;
                    break;
            }
            
        }else{
            
            tdata->state = IDLE;
            _sched_yield();
            
        }
        
    }
    
}
