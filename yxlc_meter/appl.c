#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <linux/if.h>
#include <linux/serial.h>
#include <syslog.h>
#include <unistd.h>
#include "mongoose.h"
#include "can_frame.h"
#include "appl.h"
#include "modbus.h"
#include "MQTTClient.h"
#include "MQTTClientPersistence.h"

#define EXPORT_PATH "/sys/class/gpio/export"            // GPIO设备导出设备
#define DIR_OUT "out"
#define DIR_IN "in"

char* VERSION = "裕兴老厂dir noora";

struct appl_t APPL;
struct mg_mgr mgr_mqtt1; // thingsboard
struct mg_mgr mgr_mqtt2; // cloud
struct mg_mgr mgr_mqtt3; // gate and trans meter

int appl_cfg_save( void )
{
    long long chksum = 0;
    FILE* fp = NULL;
    int rc, i;
    fp = fopen("./cfg.bin","wb");
    if( fp == NULL){
        syslog(LOG_INFO,"%s, fopen ./cfg.bin Fail", __func__);
        return -1;
    }else{
        for( i = 0; i < sizeof(APPL.Set.buf - 8); i++ ){
            chksum += APPL.Set.buf[8 + i];
        }
        APPL.Set.s.chksum = chksum;
        rc = fwrite(APPL.Set.buf, sizeof(char), sizeof(APPL.Set.buf), fp);
        if( rc != sizeof(APPL.Set.buf)){
            syslog(LOG_INFO,"%s, fwrite ./cfg.bin Fail, rc:%d", __func__, rc);
            return -1;
        }else{
            return 0;
        }
    }
}

int appl_cfg_read( void )
{
    long long chksum = 0;
    FILE* fp = NULL;
    int rc, i;
    fp = fopen("./cfg.bin","rb");
    if( fp == NULL){
        syslog(LOG_INFO,"%s, fopen ./cfg.bin Fail", __func__);
        return -1;
    }else{
        rc = fread(APPL.Set.buf, sizeof(char), sizeof(APPL.Set.buf), fp);
        if( rc != sizeof(APPL.Set.buf)){
            syslog(LOG_INFO,"%s, fread ./cfg.bin Fail, rc:%d", __func__, rc);
            return -1;
        }else{
            for( i = 0; i < sizeof(APPL.Set.buf - 8); i++ ){
                chksum += APPL.Set.buf[8 + i];
            }
            if( chksum == APPL.Set.s.chksum ){
                return 0;
            }else{
                syslog(LOG_INFO,"%s, Chksum Fail, rc:%d", __func__, rc);
                return -1;
            }
        }
    }
}

void appl_cfg_set_err( void )
{
    APPL.Set.s.bErr = 1;
    strcpy(APPL.Set.s.szState,"故障");
}

void appl_cfg_reset_err( void )
{
    APPL.Set.s.bErr = 0;
    strcpy(APPL.Set.s.szState,"正常");
}

static char* appl_get_datetime_long( void )
{
    time_t timep;
    struct tm *tsp;
    static char buf[128];

    time(&timep);
    // tsp = gmtime(&timep);
    tsp = localtime(&timep);

    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", tsp->tm_year + 1900,
            tsp->tm_mon + 1,
            tsp->tm_mday,
            tsp->tm_hour,
            tsp->tm_min,
            (short)tsp->tm_sec);
    return buf;
}

static char* appl_get_datetime_short( void )
{
    static char buf[128];    
    time_t timep;
    struct tm *tsp;

    time(&timep);
    // tsp = gmtime(&timep);
    tsp = localtime(&timep);

    sprintf(buf, "%02d:%02d:%02d",
            tsp->tm_hour,
            tsp->tm_min,
            (short)tsp->tm_sec);
    return buf;
}

static void appl_get_datetime_num(int *y, int *m, int *d, int *h, int *min, int *s)
{
    time_t timep;
    struct tm *tsp;

    time(&timep);
    // tsp = gmtime(&timep);
    tsp = localtime(&timep);

    *y = 1900 + tsp->tm_year;
    *m = 1 + tsp->tm_mon;
    *d = tsp->tm_mday;
    *h = tsp->tm_hour;
    *min = tsp->tm_min;
    *s = tsp->tm_sec;
}

char* appl_get_dtstr(void)
{
    time_t timep;
    struct tm *tsp;
    static char buf[200];

    time(&timep);
    tsp = localtime(&timep);
    sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", tsp->tm_year + 1900,
            tsp->tm_mon + 1,
            tsp->tm_mday,
            tsp->tm_hour,
            tsp->tm_min,
            (short)tsp->tm_sec);
    return buf;
}

static void* thrd_485_1(void *param)
{
    char buf[128];
    modbus_t* ctx = NULL;
    struct timeval t;
    int rc;
    unsigned short data[256];
    unsigned short start;
    unsigned short nbr;
    int chidx = 1;
    int i;
    struct chan485_t* ch = &APPL.chan485[chidx];
    struct Dtsd1352_t* m = NULL;
    int64_t startts;

    syslog(LOG_INFO,"%s ENTER", __func__);

    appl_485_set_state(chidx, ST_485_INIT, ERR_485_NONE);
    while(1){
        startts = mg_millis();
        switch( ch->state){
        case ST_485_INIT:
            ctx = modbus_new_rtu(ch->szdev, ch->baud, 'N', 8, 1);
            if (ctx == NULL){
                MG_INFO(("%s, modbus rtu new fail", __func__));
                appl_485_set_state(chidx, ST_485_ERR, ERR_485_INIT_FAIL);
            }else if (modbus_connect(ctx) == -1){
                MG_INFO(("%s, modbus rtu connect fail", __func__));
                modbus_free(ctx);
                ctx = NULL;
                appl_485_set_state(chidx, ST_485_ERR, ERR_485_INIT_FAIL);
            }else{
                // t.tv_sec = 0;
                // t.tv_usec = 500000;  // 500ms
                // //modbus_set_response_timeout(ctx, &t);
                // modbus_set_response_timeout(ctx, 0, 500000);
                ch->reqcnt = 0;
                ch->failcnt = 0;
                appl_485_set_state(chidx, ST_485_RUN, ERR_485_NONE);
            }
        break;

        case ST_485_RUN:
            // Process Cmd
            if( ch->Cmd == CMD_485_RESET ){
                ch->Cmd = CMD_485_DONE;
                if (ctx != NULL){
                    modbus_close(ctx);
                    modbus_free(ctx);
                    ctx = NULL;
                }
                appl_485_set_state(chidx, ST_485_INIT, ERR_485_NONE);
                break;
            }
            for( i = 1; i <= 2; i++ )
            {
                m = &APPL.Dtsd1352[i];//根据现场进行改写
                if( mg_millis() - m->LastUpdate > 5000 ){
                    m->CommState = ST_COMM_ERR;
                    strcpy(m->szCommState,"故障");
                }else{
                    m->CommState = ST_COMM_NORM;
                    strcpy(m->szCommState,"正常");
                }
                // Comm with DTSD1352
                // PART 1
                appl_chan485_lock(chidx);
                usleep(100000);
                modbus_set_slave(ctx, m->Adr);
                start = 0x0000;
                nbr = 32;
                rc = modbus_read_registers( ctx, start, nbr, data);
                appl_chan485_unlock(chidx);
                ch->reqcnt += 1;
                if(rc != nbr){
                    ch->failcnt += 1;
                    m->CommFailTotalCnt += 1;
                    modbus_flush(ctx);
                }else{
                    m->com_active_e = ((uint32_t)(data[0x0000 - start] << 16 | data[0x0001 - start])) * m->PT * m->CT * 0.01;
                    m->pos_active_e = ((uint32_t)(data[0x000A - start] << 16 | data[0x000B - start])) * m->PT * m->CT * 0.01;
                    m->neg_active_e = ((uint32_t)(data[0x0014 - start] << 16 | data[0x0015 - start])) * m->PT * m->CT * 0.01;

                    // PART 2
                    appl_chan485_lock(chidx);
                    usleep(100000);
                    start = 0x0061;
                    nbr = 23;
                    rc = modbus_read_registers( ctx, start, nbr, data);
                    appl_chan485_unlock(chidx);
                    ch->reqcnt += 1;
                    if(rc != nbr){
                        ch->failcnt += 1;
                        m->CommFailTotalCnt += 1;
                        modbus_flush(ctx);
                    }else{
                        m->ua = data[0x0061 - start] * m->PT * 0.1;
                        m->ub = data[0x0062 - start] * m->PT * 0.1;
                        m->uc = data[0x0063 - start] * m->PT * 0.1;
                        m->ia = data[0x0064 - start] * m->CT * 0.01;
                        m->ib = data[0x0065 - start] * m->CT * 0.01;
                        m->ic = data[0x0066 - start] * m->CT * 0.01;
                        m->freq = data[0x0077 - start] * 0.01;

                        // PART 3
                        appl_chan485_lock(chidx);
                        usleep(100000);
                        start = 0x0078;
                        nbr = 32;
                        rc = modbus_read_registers( ctx, start, nbr, data);
                        appl_chan485_unlock(chidx);
                        ch->reqcnt += 1;
                        if(rc != nbr){
                            ch->failcnt += 1;
                            m->CommFailTotalCnt += 1;
                            modbus_flush(ctx);
                        }else{
                            m->PT = data[0x008D - start];
                            m->CT = data[0x008E - start];
                            
                            // PART 4
                            appl_chan485_lock(chidx);
                            usleep(100000);
                            start = 0x016A;
                            nbr = 60;
                            rc = modbus_read_registers( ctx, start, nbr, data);
                            appl_chan485_unlock(chidx);
                            ch->reqcnt += 1;
                            if(rc != nbr){
                                ch->failcnt += 1;
                                m->CommFailTotalCnt += 1;
                                modbus_flush(ctx);
                            }else{
                                m->com_active_p = ((int32_t)(data[0x016A - start] << 16 | data[0x016B - start])) * m->PT * m->CT * 0.001;
                                m->com_ractive_p = ((int32_t)(data[0x0172 - start] << 16 | data[0x0173 - start])) * m->PT * m->CT * 0.001;
                                m->pwr_factor = (short)data[0x017F - start] * 0.001;
                                m->pos_active_dem = data[0x0198 - start];
                                m->neg_active_dem = data[0x0199 - start];
                                
                                m->LastUpdate = mg_millis();
                                strcpy(m->szLastUpdate, appl_get_dtstr());
                            }
                        }
                    }
                }
            }            
        break;

        case ST_485_ERR:
            if(ch->Cmd == CMD_485_RESET){
                ch->Cmd = CMD_485_DONE;
                if (ctx != NULL){
                    modbus_close(ctx);
                    modbus_free(ctx);
                    ctx = NULL;
                }
                appl_485_set_state(chidx, ST_485_INIT, ERR_485_NONE);
            }else{
                usleep(300000);
            }
        break;

        default:
            // never reach here
        break;
        }
        usleep(100000);
        ch->loopcnt += 1;
        ch->looptime = mg_millis() - startts;
    }

    syslog(LOG_INFO, "%s EXIT", __func__);
}

static void* thrd_485_2(void *param)
{
    char buf[128];
    modbus_t* ctx = NULL;
    struct timeval t;
    int rc;
    unsigned short data[256];
    unsigned short start;
    unsigned short nbr;
    int chidx = 2;
    struct chan485_t* ch = &APPL.chan485[chidx];
    struct Dtsd1352_t* m = NULL;
    int step = 1;
    int i;
    int64_t startts;

    syslog(LOG_INFO, "%s ENTER", __func__);

    appl_485_set_state(chidx, ST_485_INIT, ERR_485_NONE);
    while(1){
        startts = mg_millis();
        switch( ch->state){
        case ST_485_INIT:
            ctx = modbus_new_rtu(ch->szdev, ch->baud, 'N', 8, 1);
            if (ctx == NULL){
                MG_INFO(("%s, modbus rtu new fail", __func__));
                appl_485_set_state(chidx, ST_485_ERR, ERR_485_INIT_FAIL);
            }else if (modbus_connect(ctx) == -1){
                MG_INFO(("%s, modbus rtu connect fail", __func__));
                modbus_free(ctx);
                ctx = NULL;
                appl_485_set_state(chidx, ST_485_ERR, ERR_485_INIT_FAIL);
            }else{
                t.tv_sec = 0;
                t.tv_usec = 500000;
                //modbus_set_response_timeout(ctx, &t);
                modbus_set_response_timeout(ctx, 0, 500000);
                ch->reqcnt = 0;
                ch->failcnt = 0;
                appl_485_set_state(chidx, ST_485_RUN, ERR_485_NONE);
            }
        break;

        case ST_485_RUN:
            // Process Cmd
            if( ch->Cmd == CMD_485_RESET ){
                ch->Cmd = CMD_485_DONE;
                if (ctx != NULL){
                    modbus_close(ctx);
                    modbus_free(ctx);
                    ctx = NULL;
                }
                appl_485_set_state(chidx, ST_485_INIT, ERR_485_NONE);
                break;
            }
            for( i = 3; i <= 4; i++ ){
                m = &APPL.Dtsd1352[i];
                if( mg_millis() - m->LastUpdate > 5000 ){
                    m->CommState = ST_COMM_ERR;
                    strcpy(m->szCommState,"故障");
                }else{
                    m->CommState = ST_COMM_NORM;
                    strcpy(m->szCommState,"正常");
                }
                // Comm with DTSD1352
                // PART 1
                appl_chan485_lock(chidx);
                usleep(50000);
                modbus_set_slave(ctx, m->Adr);
                start = 0x0000;
                nbr = 32;
                rc = modbus_read_registers( ctx, start, nbr, data);
                appl_chan485_unlock(chidx);
                ch->reqcnt += 1;
                if(rc != nbr){
                    ch->failcnt += 1;
                    m->CommFailTotalCnt += 1;
                    modbus_flush(ctx);
                }else{
                    m->com_active_e = ((int32_t)(data[0x0000 - start] << 16 | data[0x0001 - start])) * m->PT * m->CT * 0.01;
                    m->pos_active_e = ((int32_t)(data[0x000A - start] << 16 | data[0x000B - start])) * m->PT * m->CT * 0.01;
                    m->neg_active_e = ((int32_t)(data[0x0014 - start] << 16 | data[0x0015 - start])) * m->PT * m->CT * 0.01;

                    // PART 2
                    appl_chan485_lock(chidx);
                    usleep(50000);
                    start = 0x0061;
                    nbr = 23;
                    rc = modbus_read_registers( ctx, start, nbr, data);
                    appl_chan485_unlock(chidx);
                    ch->reqcnt += 1;
                    if(rc != nbr){
                        ch->failcnt += 1;
                        m->CommFailTotalCnt += 1;
                        modbus_flush(ctx);
                    }else{
                        m->ua = data[0x0061 - start] * m->PT * 0.1;
                        m->ub = data[0x0062 - start] * m->PT * 0.1;
                        m->uc = data[0x0063 - start] * m->PT * 0.1;
                        m->ia = data[0x0064 - start] * m->CT * 0.01;
                        m->ib = data[0x0065 - start] * m->CT * 0.01;
                        m->ic = data[0x0066 - start] * m->CT * 0.01;
                        m->freq = data[0x0077 - start] * 0.01;

                        // PART 3
                        appl_chan485_lock(chidx);
                        usleep(50000);
                        start = 0x0078;
                        nbr = 32;
                        rc = modbus_read_registers( ctx, start, nbr, data);
                        appl_chan485_unlock(chidx);
                        ch->reqcnt += 1;
                        if(rc != nbr){
                            ch->failcnt += 1;
                            m->CommFailTotalCnt += 1;
                            modbus_flush(ctx);
                        }else{
                            m->PT = data[0x008D - start];
                            m->CT = data[0x008E - start];
                            
                            // PART 4
                            appl_chan485_lock(chidx);
                            usleep(50000);
                            start = 0x016A;
                            nbr = 60;
                            rc = modbus_read_registers( ctx, start, nbr, data);
                            appl_chan485_unlock(chidx);
                            ch->reqcnt += 1;
                            if(rc != nbr){
                                ch->failcnt += 1;
                                m->CommFailTotalCnt += 1;
                                modbus_flush(ctx);
                            }else{
                                m->com_active_p = ((int32_t)(data[0x016A - start] << 16 | data[0x016B - start])) * m->PT * m->CT * 0.001;
                                m->com_ractive_p = ((int32_t)(data[0x0172 - start] << 16 | data[0x0173 - start])) * m->PT * m->CT * 0.001;
                                m->pwr_factor = (short)data[0x017F - start] * 0.001;
                                m->pos_active_dem = data[0x0198 - start];
                                m->neg_active_dem = data[0x0199 - start];
                                
                                m->LastUpdate = mg_millis();
                                strcpy(m->szLastUpdate, appl_get_dtstr());
                            }
                        }
                    }
                }
            }       

        break;

        case ST_485_ERR:
            if(ch->Cmd == CMD_485_RESET){
                ch->Cmd = CMD_485_DONE;
                if (ctx != NULL){
                    modbus_close(ctx);
                    modbus_free(ctx);
                    ctx = NULL;
                }
                appl_485_set_state(chidx, ST_485_INIT, ERR_485_NONE);
            }else{
                usleep(300000);
            }
        break;

        default:
            // never reach here
        break;
        }
        usleep(100000);
        ch->loopcnt += 1;
        ch->looptime = mg_millis() - startts;
    }
    syslog(LOG_INFO, "%s EXIT", __func__);
}


static void* thrd_485_4(void *param)
{
    char buf[128];
    modbus_t* ctx = NULL;
    struct timeval t;
    int rc;
    unsigned short data[256];
    unsigned short start;
    unsigned short nbr;
    int chidx = 4;
    int i;
    struct chan485_t* ch = &APPL.chan485[chidx];
    int64_t startts;
    struct Dtsd1352_t* m = NULL; 

    syslog(LOG_INFO,"%s ENTER", __func__);

    appl_485_set_state(chidx, ST_485_INIT, ERR_485_NONE);
    while(1){
        startts = mg_millis();
        switch( ch->state){
        case ST_485_INIT:
            ctx = modbus_new_rtu(ch->szdev, ch->baud, 'N', 8, 1);
            if (ctx == NULL){
                MG_INFO(("%s, modbus rtu new fail", __func__));
                appl_485_set_state(chidx, ST_485_ERR, ERR_485_INIT_FAIL);
            }else if (modbus_connect(ctx) == -1){
                MG_INFO(("%s, modbus rtu connect fail", __func__));
                modbus_free(ctx);
                ctx = NULL;
                appl_485_set_state(chidx, ST_485_ERR, ERR_485_INIT_FAIL);
            }else{
                t.tv_sec = 0;
                t.tv_usec = 500000;
                //modbus_set_response_timeout(ctx, &t);
                modbus_set_response_timeout(ctx, 0, 500000);
                ch->reqcnt = 0;
                ch->failcnt = 0;
                appl_485_set_state(chidx, ST_485_RUN, ERR_485_NONE);
            }
        break;

        // case ST_485_RUN:
        //     // Process Cmd
        //     if( ch->Cmd == CMD_485_RESET ){
        //         ch->Cmd = CMD_485_DONE;
        //         if (ctx != NULL){
        //             modbus_close(ctx);
        //             modbus_free(ctx);
        //             ctx = NULL;
        //         }
        //         appl_485_set_state(chidx, ST_485_INIT, ERR_485_NONE);
        //         break;
        //     }                                
        // break;
         case ST_485_RUN:
            // Process Cmd
            if( ch->Cmd == CMD_485_RESET )
            {
                ch->Cmd = CMD_485_DONE;
                if (ctx != NULL){
                    modbus_close(ctx);
                    modbus_free(ctx);
                    ctx = NULL;
                }
                appl_485_set_state(chidx, ST_485_INIT, ERR_485_NONE);
                break;
            }
            for(i=5;i<=5;i++)
            {
                m = &APPL.Dtsd1352[5];
                if( mg_millis() - m->LastUpdate > 5000 ){
                    m->CommState = ST_COMM_ERR;
                    strcpy(m->szCommState,"故障");
                }else{
                    m->CommState = ST_COMM_NORM;
                    strcpy(m->szCommState,"正常");
                }
                // Comm with DTSD1352
                // PART 1
                appl_chan485_lock(chidx);
                usleep(50000);
                modbus_set_slave(ctx, m->Adr);
                start = 0x0000;
                nbr = 32;
                rc = modbus_read_registers( ctx, start, nbr, data);
                appl_chan485_unlock(chidx);
                ch->reqcnt += 1;
                if(rc != nbr){
                    ch->failcnt += 1;
                    m->CommFailTotalCnt += 1;
                    modbus_flush(ctx);
                }else{
                    m->com_active_e = ((int32_t)(data[0x0000 - start] << 16 | data[0x0001 - start])) * m->PT * m->CT * 0.01;
                    m->pos_active_e = ((int32_t)(data[0x000A - start] << 16 | data[0x000B - start])) * m->PT * m->CT * 0.01;
                    m->neg_active_e = ((int32_t)(data[0x0014 - start] << 16 | data[0x0015 - start])) * m->PT * m->CT * 0.01;

                    // PART 2
                    appl_chan485_lock(chidx);
                    usleep(50000);
                    start = 0x0061;
                    nbr = 23;
                    rc = modbus_read_registers( ctx, start, nbr, data);
                    appl_chan485_unlock(chidx);
                    ch->reqcnt += 1;
                    if(rc != nbr){
                        ch->failcnt += 1;
                        m->CommFailTotalCnt += 1;
                        modbus_flush(ctx);
                    }else{
                        m->ua = data[0x0061 - start] * m->PT * 0.1;
                        m->ub = data[0x0062 - start] * m->PT * 0.1;
                        m->uc = data[0x0063 - start] * m->PT * 0.1;
                        m->ia = data[0x0064 - start] * m->CT * 0.01;
                        m->ib = data[0x0065 - start] * m->CT * 0.01;
                        m->ic = data[0x0066 - start] * m->CT * 0.01;
                        m->freq = data[0x0077 - start] * 0.01;

                        // PART 3
                        appl_chan485_lock(chidx);
                        usleep(50000);
                        start = 0x0078;
                        nbr = 32;
                        rc = modbus_read_registers( ctx, start, nbr, data);
                        appl_chan485_unlock(chidx);
                        ch->reqcnt += 1;
                        if(rc != nbr){
                            ch->failcnt += 1;
                            m->CommFailTotalCnt += 1;
                            modbus_flush(ctx);
                        }else{
                            m->PT = data[0x008D - start];
                            m->CT = data[0x008E - start];
                            
                            // PART 4
                            appl_chan485_lock(chidx);
                            usleep(50000);
                            start = 0x016A;
                            nbr = 60;
                            rc = modbus_read_registers( ctx, start, nbr, data);
                            appl_chan485_unlock(chidx);
                            ch->reqcnt += 1;
                            if(rc != nbr){
                                ch->failcnt += 1;
                                m->CommFailTotalCnt += 1;
                                modbus_flush(ctx);
                            }else{
                                m->com_active_p = ((int32_t)(data[0x016A - start] << 16 | data[0x016B - start])) * m->PT * m->CT * 0.001;
                                m->com_ractive_p = ((int32_t)(data[0x0172 - start] << 16 | data[0x0173 - start])) * m->PT * m->CT * 0.001;
                                m->pwr_factor = (short)data[0x017F - start] * 0.001;
                                m->pos_active_dem = data[0x0198 - start];
                                m->neg_active_dem = data[0x0199 - start];
                                
                                m->LastUpdate = mg_millis();
                                strcpy(m->szLastUpdate, appl_get_dtstr());
                            }
                        }
                    }
                }
            } 
            break; 


        case ST_485_ERR:
            if(ch->Cmd == CMD_485_RESET){
                ch->Cmd = CMD_485_DONE;
                if (ctx != NULL){
                    modbus_close(ctx);
                    modbus_free(ctx);
                    ctx = NULL;
                }
                appl_485_set_state(chidx, ST_485_INIT, ERR_485_NONE);
            }else{
                usleep(300000);
            }
        break;

        default:
            // never reach here
        break;
        }

        usleep(100000);
        ch->loopcnt += 1;
        ch->looptime = mg_millis() - startts;
    }
    syslog(LOG_INFO,"%s EXIT", __func__);
}

static void* thrd_485_3(void *param)
{
    char buf[128];
    modbus_t* ctx = NULL;
    struct timeval t;
    int rc;
    unsigned short data[256];
    unsigned short start;
    unsigned short nbr;
    int chidx = 3;
    int i;
    struct chan485_t* ch = &APPL.chan485[chidx];
    int64_t startts;
    struct Dtsd1352_t* m = NULL; 

    syslog(LOG_INFO,"%s ENTER", __func__);

    appl_485_set_state(chidx, ST_485_INIT, ERR_485_NONE);
    while(1){
        startts = mg_millis();
        switch( ch->state){
        case ST_485_INIT:
            ctx = modbus_new_rtu(ch->szdev, ch->baud, 'N', 8, 1);
            if (ctx == NULL){
                MG_INFO(("%s, modbus rtu new fail", __func__));
                appl_485_set_state(chidx, ST_485_ERR, ERR_485_INIT_FAIL);
            }else if (modbus_connect(ctx) == -1){
                MG_INFO(("%s, modbus rtu connect fail", __func__));
                modbus_free(ctx);
                ctx = NULL;
                appl_485_set_state(chidx, ST_485_ERR, ERR_485_INIT_FAIL);
            }else{
                t.tv_sec = 0;
                t.tv_usec = 500000;
                //modbus_set_response_timeout(ctx, &t);
                modbus_set_response_timeout(ctx, 0, 500000);
                ch->reqcnt = 0;
                ch->failcnt = 0;
                appl_485_set_state(chidx, ST_485_RUN, ERR_485_NONE);
            }
        break;

        case ST_485_RUN:
            // Process Cmd
            if( ch->Cmd == CMD_485_RESET ){
                ch->Cmd = CMD_485_DONE;
                if (ctx != NULL){
                    modbus_close(ctx);
                    modbus_free(ctx);
                    ctx = NULL;
                }
                appl_485_set_state(chidx, ST_485_INIT, ERR_485_NONE);
                break;
            }                                
        break;

        case ST_485_ERR:
            if(ch->Cmd == CMD_485_RESET){
                ch->Cmd = CMD_485_DONE;
                if (ctx != NULL){
                    modbus_close(ctx);
                    modbus_free(ctx);
                    ctx = NULL;
                }
                appl_485_set_state(chidx, ST_485_INIT, ERR_485_NONE);
            }else{
                usleep(300000);
            }
        break;

        default:
            // never reach here
        break;
        }

        usleep(100000);
        ch->loopcnt += 1;
        ch->looptime = mg_millis() - startts;
    }
    syslog(LOG_INFO,"%s EXIT", __func__);
}


void appl_chan485_lock(int idx)
{
    //pthread_mutex_lock(&APPL.chan485[idx].mutex);
}

void appl_chan485_unlock(int idx)
{
    //pthread_mutex_unlock(&APPL.chan485[idx].mutex);
}

void appl_485_set_state(int idx, int s, int e)
{
    struct chan485_t* c = &APPL.chan485[idx];
    c->state = s;
    switch( c->state){
        case ST_485_INIT:
            strcpy(c->szstate,"初始化");
        break;

        case ST_485_RUN:
            strcpy(c->szstate,"运行");
        break;

        case ST_485_ERR:
            strcpy(c->szstate,"故障");
            break;

        default:
            strcpy(c->szstate,"未知");
        break;
    }

    c->err = e;
    switch (e)
    {
    case ERR_485_NONE:
        strcpy(c->szerr,"无");
        break;
    case ERR_485_INIT_FAIL:
        strcpy(c->szerr,"初始化失败");
        break;
    
    default:
        strcpy(c->szerr,"未知");
        break;
    }
}

int appl_chan485_get_state(int idx)
{
    return APPL.chan485[idx].state;
}

void appl_can_set_state(int idx, int s, int e)
{
    struct chancan_t* c = &APPL.chancan[idx];
    c->State = s;
    switch( c->State){
        case ST_CAN_INIT:
            strcpy(c->szState,"初始化");
        break;

        case ST_CHANCAN_RUN:
            strcpy(c->szState,"运行");
        break;

        case ST_CHANCAN_ERR:
            strcpy(c->szState,"故障");
        break;        

        default:
            strcpy(c->szState,"未知");
        break;
    }

    c->Err = e;
    switch(e){
        case ERR_CAN_NONE:
        strcpy(c->szErr,"无");
        break;

        case ERR_CAN_INIT_FAIL:
        strcpy(c->szErr,"初始化失败");
        break;

        default:
        break;
    }
}

int appl_can_get_state(int idx)
{
    return APPL.chancan[idx].State;
}

void appl_485_set_485mode( void ) {
    int fd;
    struct serial_rs485 rs485conf = {0};
    int ret;
    
    // 1# 485
    fd = open("/dev/ttymxc1", O_RDWR | O_NOCTTY);
    if (fd <= 0) {
        syslog(LOG_INFO,"%s, Open ttymxc1 Fail",__func__);
    }else{
        /* get configure from device */
        ret = ioctl(fd, TIOCGRS485, &rs485conf);
        if(ret < 0) {
            // failed
            syslog(LOG_INFO,"%s, ioctl ttymxc1 Fail",__func__);
        }
        /* set enable rs485 mode in configure */
        /* Enable RS485 mode: */
        rs485conf.flags |= SER_RS485_ENABLED;
        /* Set logical level for RTS pin equal to 1 when sending: */
        rs485conf.flags |= SER_RS485_RTS_ON_SEND;
        /* Set logical level for RTS pin equal to 0 after sending: */
        rs485conf.flags &= ~SER_RS485_RTS_AFTER_SEND;
        /* Set this flag if you want to receive data even whilst sending data */
        rs485conf.flags &= ~SER_RS485_RX_DURING_TX;
        /* Set rts delay before send, if needed: */
        rs485conf.delay_rts_before_send = 0; // in miliseconds
        /* Set rts delay after send, if needed: */
        rs485conf.delay_rts_after_send = 0; // in miliseconds
        ret = ioctl(fd, TIOCSRS485, &rs485conf);
        if (ret < 0) {
            /* Error handling. See errno. */
            syslog(LOG_INFO,"%s, Set ttymxc1 485 Fail",__func__);
        }
        close(fd);
    }

    // 2# 485
    fd = open("/dev/ttymxc2", O_RDWR | O_NOCTTY);
    if (fd <= 0) {
        syslog(LOG_INFO,"%s, Open ttymxc2 Fail",__func__);
    }else{
        /* get configure from device */
        ret = ioctl(fd, TIOCGRS485, &rs485conf);
        if(ret < 0) {
            // failed
            syslog(LOG_INFO,"%s, ioctl ttymxc2 Fail",__func__);
        }
        /* set enable rs485 mode in configure */
        /* Enable RS485 mode: */
        rs485conf.flags |= SER_RS485_ENABLED;
        /* Set logical level for RTS pin equal to 1 when sending: */
        rs485conf.flags |= SER_RS485_RTS_ON_SEND;
        /* Set logical level for RTS pin equal to 0 after sending: */
        rs485conf.flags &= ~SER_RS485_RTS_AFTER_SEND;
        /* Set this flag if you want to receive data even whilst sending data */
        rs485conf.flags &= ~SER_RS485_RX_DURING_TX;
        /* Set rts delay before send, if needed: */
        rs485conf.delay_rts_before_send = 0; // in miliseconds
        /* Set rts delay after send, if needed: */
        rs485conf.delay_rts_after_send = 0; // in miliseconds
        ret = ioctl(fd, TIOCSRS485, &rs485conf);
        if (ret < 0) {
            /* Error handling. See errno. */
            syslog(LOG_INFO,"%s, Set ttymxc2 485 Fail",__func__);
        }
        close(fd);
    }    

    // 3# 485
    fd = open("/dev/ttymxc3", O_RDWR | O_NOCTTY);
    if (fd <= 0) {
        syslog(LOG_INFO,"%s, Open ttymxc3 Fail",__func__);
    }else{
        /* get configure from device */
        ret = ioctl(fd, TIOCGRS485, &rs485conf);
        if(ret < 0) {
            // failed
            syslog(LOG_INFO,"%s, ioctl ttymxc3 Fail",__func__);
        }
        /* set enable rs485 mode in configure */
        /* Enable RS485 mode: */
        rs485conf.flags |= SER_RS485_ENABLED;
        /* Set logical level for RTS pin equal to 1 when sending: */
        rs485conf.flags |= SER_RS485_RTS_ON_SEND;
        /* Set logical level for RTS pin equal to 0 after sending: */
        rs485conf.flags &= ~SER_RS485_RTS_AFTER_SEND;
        /* Set this flag if you want to receive data even whilst sending data */
        rs485conf.flags &= ~SER_RS485_RX_DURING_TX;
        /* Set rts delay before send, if needed: */
        rs485conf.delay_rts_before_send = 0; // in miliseconds
        /* Set rts delay after send, if needed: */
        rs485conf.delay_rts_after_send = 0; // in miliseconds
        ret = ioctl(fd, TIOCSRS485, &rs485conf);
        if (ret < 0) {
            /* Error handling. See errno. */
            syslog(LOG_INFO,"%s, Set ttymxc3 485 Fail",__func__);
        }
        close(fd);
    }    

    // 4# 485
    fd = open("/dev/ttymxc5", O_RDWR | O_NOCTTY);
    if (fd <= 0) {
        syslog(LOG_INFO,"%s, Open ttymxc5 Fail",__func__);
    }else{
        /* get configure from device */
        ret = ioctl(fd, TIOCGRS485, &rs485conf);
        if(ret < 0) {
            // failed
            syslog(LOG_INFO,"%s, ioctl ttymxc5 Fail",__func__);
        }
        /* set enable rs485 mode in configure */
        /* Enable RS485 mode: */
        rs485conf.flags |= SER_RS485_ENABLED;
        /* Set logical level for RTS pin equal to 1 when sending: */
        rs485conf.flags |= SER_RS485_RTS_ON_SEND;
        /* Set logical level for RTS pin equal to 0 after sending: */
        rs485conf.flags &= ~SER_RS485_RTS_AFTER_SEND;
        /* Set this flag if you want to receive data even whilst sending data */
        rs485conf.flags &= ~SER_RS485_RX_DURING_TX;
        /* Set rts delay before send, if needed: */
        rs485conf.delay_rts_before_send = 0; // in miliseconds
        /* Set rts delay after send, if needed: */
        rs485conf.delay_rts_after_send = 0; // in miliseconds
        ret = ioctl(fd, TIOCSRS485, &rs485conf);
        if (ret < 0) {
            /* Error handling. See errno. */
            syslog(LOG_INFO,"%s, Set ttymxc5 485 Fail",__func__);
        }
        close(fd);
    }    
}

static void fn_mqtt1(struct mg_connection *c, int ev, void *ev_data) {
//   struct chanmqtt_t* m = &APPL.chanmqtt[1];
//   if (ev == MG_EV_OPEN) {
//     MG_INFO(("%lu CREATED", c->id));
//     // c->is_hexdumping = 1;
//   } else if (ev == MG_EV_CONNECT) {
//     if (mg_url_is_ssl(m->szs_url)) {
//       struct mg_tls_opts opts = {.ca = mg_unpacked("/certs/ca.pem"),
//                                  .name = mg_url_host(m->szs_url)};
//       mg_tls_init(c, &opts);
//     }
//   } else if (ev == MG_EV_ERROR) {
//     // On error, log error message
//     MG_ERROR(("%lu ERROR %s", c->id, (char *) ev_data));
//   } else if (ev == MG_EV_MQTT_OPEN) {
//     m->bConnected = 1;
//     strcpy(m->szState,"正常");
//     // MQTT connect is successful
//     struct mg_str subt = mg_str(m->szs_sub_topic);
//     MG_INFO(("%lu CONNECTED to %s", c->id, m->szs_url));
//     struct mg_mqtt_opts sub_opts;
//     memset(&sub_opts, 0, sizeof(sub_opts));
//     sub_opts.topic = subt;
//     sub_opts.qos = m->s_qos;
//     mg_mqtt_sub(c, &sub_opts);
//     MG_INFO(("%lu SUBSCRIBED to %.*s", c->id, (int) subt.len, subt.ptr));

//   } else if (ev == MG_EV_MQTT_MSG) {
//     // When we get echo response, print it
//     struct mg_mqtt_message *mm = (struct mg_mqtt_message *) ev_data;
//     MG_INFO(("%lu RECEIVED %.*s <- %.*s", c->id, (int) mm->data.len,
//              mm->data.ptr, (int) mm->topic.len, mm->topic.ptr));
//   } else if (ev == MG_EV_CLOSE) {
//     MG_INFO(("%lu CLOSED", c->id));
//     m->s_conn = NULL;  // Mark that we're closed
//     m->bConnected = 0;
//     strcpy(m->szState,"故障");
//   }
}

static void fn_mqtt2(struct mg_connection *c, int ev, void *ev_data) {
  struct chanmqtt_t* m = &APPL.chanmqtt[2];
  if (ev == MG_EV_OPEN) {
    syslog(LOG_INFO,"%lu CREATED", c->id);
    // c->is_hexdumping = 1;
  } else if (ev == MG_EV_CONNECT) {
    if (mg_url_is_ssl(m->szs_url)) {
      struct mg_tls_opts opts = {.ca = mg_unpacked("/certs/ca.pem"),
                                 .name = mg_url_host(m->szs_url)};
      mg_tls_init(c, &opts);
    }
  } else if (ev == MG_EV_ERROR) {
    // On error, log error message
    syslog(LOG_INFO,"%lu ERROR %s", c->id, (char *) ev_data);
  } else if (ev == MG_EV_MQTT_OPEN) {
    m->bConnected = 1;
    strcpy(m->szState,"正常");
    m->TotalReconn++;
    // MQTT connect is successful
    struct mg_str subt = mg_str(m->szs_sub_topic);
    syslog(LOG_INFO,"%lu CONNECTED to %s", c->id, m->szs_url);
    struct mg_mqtt_opts sub_opts;
    memset(&sub_opts, 0, sizeof(sub_opts));
    sub_opts.topic = subt;
    sub_opts.qos = m->s_qos;
    mg_mqtt_sub(c, &sub_opts);
    syslog(LOG_INFO,"%lu SUBSCRIBED to %.*s", c->id, (int) subt.len, subt.ptr);
  } else if (ev == MG_EV_MQTT_MSG) {
    // When we get echo response, print it
    struct mg_mqtt_message *mm = (struct mg_mqtt_message *) ev_data;
    syslog(LOG_INFO,"%lu RECEIVED %.*s <- %.*s", c->id, (int) mm->data.len,
             mm->data.ptr, (int) mm->topic.len, mm->topic.ptr);
    m->TotalRecv++;
  } else if (ev == MG_EV_CLOSE) {
    syslog(LOG_INFO,"%lu CLOSED", c->id);
    m->s_conn = NULL;  // Mark that we're closed
    m->bConnected = 0;
    strcpy(m->szState,"故障");
  }
}

static void fn_mqtt3(struct mg_connection *c, int ev, void *ev_data) {
    struct chanmqtt_t* m = &APPL.chanmqtt[3];
    char buf[256];
    if (ev == MG_EV_OPEN) {
        syslog(LOG_INFO,"%s, %lu CREATED", __func__, c->id);
        // c->is_hexdumping = 1;
    } else if (ev == MG_EV_CONNECT) {
        if (mg_url_is_ssl(m->szs_url)) {
        struct mg_tls_opts opts = {.ca = mg_unpacked("/certs/ca.pem"),
                                    .name = mg_url_host(m->szs_url)};
        mg_tls_init(c, &opts);
        }
    } else if (ev == MG_EV_ERROR) {
        // On error, log error message
        syslog(LOG_INFO, "%s, %lu ERROR %s", __func__, c->id, (char *) ev_data);
    } else if (ev == MG_EV_MQTT_OPEN) {
        m->bConnected = 1;
        strcpy(m->szState,"正常");
        // MQTT connect is successful
        syslog(LOG_INFO,"%s, %lu CONNECTED to %s", __func__, c->id, m->szs_url);
        
        struct mg_str subt = mg_str(m->szs_sub_topic[0]);
        struct mg_mqtt_opts sub_opts;
        memset(&sub_opts, 0, sizeof(sub_opts));
        sub_opts.topic = subt;
        sub_opts.qos = m->s_qos;
        mg_mqtt_sub(c, &sub_opts);
        syslog(LOG_INFO,"%s, %lu SUBSCRIBED to %.*s", __func__, c->id, (int) subt.len, subt.ptr);

        // subt = mg_str(m->szs_sub_topic[1]);
        // memset(&sub_opts, 0, sizeof(sub_opts));
        // sub_opts.topic = subt;
        // sub_opts.qos = m->s_qos;
        // mg_mqtt_sub(c, &sub_opts);
        // syslog(LOG_INFO,"%s, %lu SUBSCRIBED to %.*s", __func__, c->id, (int) subt.len, subt.ptr);

  } else if (ev == MG_EV_MQTT_MSG) {
    // When we get echo response, print it
    struct mg_mqtt_message *mm = (struct mg_mqtt_message *) ev_data;
    //syslog(LOG_INFO,"%s, %lu RECEIVED %.*s <- %.*s", __func__, c->id, (int) mm->data.len,
    //         mm->data.ptr, (int) mm->topic.len, mm->topic.ptr);
    struct mg_str json = mg_str(mm->data.ptr);    
    double dval;
    bool ok;
    if( strcmp(mm->topic.ptr, m->szs_sub_topic[0]) == 0){ // gate meter
        syslog(LOG_INFO,"%s, Get Gate Meter Data", __func__);
        ok = mg_json_get_num(json,"$.m1_com_ap", &dval);
        if( ok ){
            // gm->com_active_p = dval;
            // gm->LastUpdate = mg_millis();
            // strcpy(gm->szLastUpdate, appl_get_datetime_long());
        }
        ok = mg_json_get_num(json,"$.m2_com_ap", &dval);
        if( ok ){
            // tm->com_active_p = dval;
            // tm->LastUpdate = mg_millis();
            // strcpy(tm->szLastUpdate, appl_get_datetime_long());
        }
    }
  } else if (ev == MG_EV_CLOSE) {
    syslog(LOG_INFO,"%s, %lu CLOSED", __func__, c->id);
    m->s_conn = NULL;  // Mark that we're closed
    m->bConnected = 0;
    strcpy(m->szState,"故障");
  }
}

static void* thrd_mqtt_1(void* param){
//     struct chanmqtt_t* m = &APPL.chanmqtt[1];
//     struct Dtsd1352_t* meter = NULL;
//     struct mg_mqtt_opts opts = {.user = mg_str(m->szusrname),
//                                 .clean = true,
//                                 .qos = m->s_qos,
//                                 .topic = mg_str(m->szs_pub_topic),
//                                 .version = 4,
//                                 .message = mg_str("bye")};
//     struct mg_mqtt_opts pub_opts;          
//     struct mg_str pubt = mg_str(m->szs_pub_topic);        
//     char msg[2048];
//     char buf[2048];      
//     int i;

//     mg_mgr_init(&mgr_mqtt1);    

//     MG_INFO(("%s ENTER, idx:1", __func__));

//     if (m->s_conn == NULL) m->s_conn = mg_mqtt_connect(&mgr_mqtt1, m->szs_url, &opts, fn_mqtt1, NULL);
//     while(1){
//         mg_mgr_poll(&mgr_mqtt1, 50);
//         if(mg_millis() - m->LastUpload > 5000){
//             m->LastUpload = mg_millis();
//             if(m->bConnected){
//                 for(i = 1; i <= 2; i++){
//                     meter = &APPL.Dtsd1352[i];
//                     if( meter->CommState == ST_COMM_NORM ){
//                         memset(&pub_opts, 0, sizeof(pub_opts));
//                         pub_opts.topic = pubt;
//                         sprintf(buf,
// "\"m%d_pf\":%.3f,\
// \"m%d_com_ap\":%.1f,\"m%d_com_ae\":%.1f,\"m%d_pos_ae\":%.1f,\"m%d_neg_ae\":%.1f, \
// \"m%d_ua\":%.1f,\"m%d_ub\":%.1f,\"m%d_uc\":%.1f, \
// \"m%d_ia\":%.1f,\"m%d_ib\":%.1f,\"m%d_ic\":%.1f",
// i, meter->pwr_factor,
// i, meter->com_active_p,i, meter->com_active_e,i, meter->pos_active_e,i, meter->neg_active_e,
// i, meter->ua,i, meter->ub,i, meter->uc,
// i, meter->ia,i, meter->ib,i, meter->ic);
//                         sprintf(msg,"{'ts':%lld,'values':{%s}}", (long long)time(NULL)*1000, buf);
//                         pub_opts.message = mg_str(msg);
//                         pub_opts.qos = m->s_qos, pub_opts.retain = false;
//                         mg_mqtt_pub(m->s_conn, &pub_opts);
//                     }
//                 }
//             }
//             if (m->s_conn == NULL) m->s_conn = mg_mqtt_connect(&mgr_mqtt1, m->szs_url, &opts, fn_mqtt1, NULL);
//         }
//     }

//     MG_INFO(("%s EXIT, idx:1", __func__));
}

static void* thrd_mqtt_2(void* param){
    int i;
    struct Dtsd1352_t* me = NULL;
    struct chanmqtt_t* m = &APPL.chanmqtt[2];
    struct Settings_t* set = &APPL.Set.s;
    struct mg_mqtt_opts opts = {.user = mg_str(m->szusrname),
                                .pass = mg_str(m->szpasswd),
                                .client_id = mg_str(m->szclientid),
                                .clean = true,
                                .qos = m->s_qos,
                                .topic = mg_str(m->szs_pub_topic),
                                .version = 4};
    struct mg_mqtt_opts pub_opts;          
    struct mg_str pubt = mg_str(m->szs_pub_topic);        
    char msg[2048];
    char buf[2048];   

    mg_mgr_init(&mgr_mqtt2);        

    syslog(LOG_INFO,"%s ENTER idx:2", __func__);

    if (m->s_conn == NULL) m->s_conn = mg_mqtt_connect(&mgr_mqtt2, m->szs_url, &opts, fn_mqtt2, NULL);
    while(1){
         mg_mgr_poll(&mgr_mqtt2, 20);
         // Process Cmd
         if( m->Cmd == CMD_MQTT_REGISTER ){
            m->Cmd = CMD_MQTT_DONE;
            // Register CtnMeter1
            sprintf(msg, "{\"project_id\":\"%s\",\"timestamp\":%lld,\"data\":[{\"device_id\":\"%s\",\"type\":12,\"idx\":1}]}", 
            set->szCloudUserName, (long long)time(NULL)*1000, "CtnMeter1");
            pub_opts.message = mg_str(msg);
            pub_opts.qos = m->s_qos, pub_opts.retain = false;
            pub_opts.topic = mg_str("register");
            mg_mqtt_pub(m->s_conn, &pub_opts);         
            // Register CtnMeter2
            sprintf(msg, "{\"project_id\":\"%s\",\"timestamp\":%lld,\"data\":[{\"device_id\":\"%s\",\"type\":12,\"idx\":1}]}", 
            set->szCloudUserName, (long long)time(NULL)*1000, "CtnMeter2");
            pub_opts.message = mg_str(msg);
            pub_opts.qos = m->s_qos, pub_opts.retain = false;
            pub_opts.topic = mg_str("register");
            mg_mqtt_pub(m->s_conn, &pub_opts);        
            // Register GateMeter
            sprintf(msg, "{\"project_id\":\"%s\",\"timestamp\":%lld,\"data\":[{\"device_id\":\"%s\",\"type\":17,\"idx\":1}]}", 
            set->szCloudUserName, (long long)time(NULL)*1000, "GateMeter");
            pub_opts.message = mg_str(msg);
            pub_opts.qos = m->s_qos, pub_opts.retain = false;
            pub_opts.topic = mg_str("register");
            mg_mqtt_pub(m->s_conn, &pub_opts);                                  
            // Register TransMeter1
            sprintf(msg, "{\"project_id\":\"%s\",\"timestamp\":%lld,\"data\":[{\"device_id\":\"%s\",\"type\":16,\"idx\":1}]}", 
            set->szCloudUserName, (long long)time(NULL)*1000, "TransMeter1");
            pub_opts.message = mg_str(msg);
            pub_opts.qos = m->s_qos, pub_opts.retain = false;
            pub_opts.topic = mg_str("register");
            mg_mqtt_pub(m->s_conn, &pub_opts);     
            // Register TransMeter2
            sprintf(msg, "{\"project_id\":\"%s\",\"timestamp\":%lld,\"data\":[{\"device_id\":\"%s\",\"type\":16,\"idx\":1}]}", 
            set->szCloudUserName, (long long)time(NULL)*1000, "TransMeter2");
            pub_opts.message = mg_str(msg);
            pub_opts.qos = m->s_qos, pub_opts.retain = false;
            pub_opts.topic = mg_str("register");
            mg_mqtt_pub(m->s_conn, &pub_opts);                                                                                                                                   
         }

        if(mg_millis() - m->LastFastUpload > set->UploadHighSpeed){
            m->LastFastUpload = mg_millis();
            if(m->bConnected){ 
                    // CtnMeter1
                    me = &APPL.Dtsd1352[1];
                    if(me->CommState == ST_COMM_NORM){
                        sprintf(msg, "{\"project_id\":\"%s\",\"timestamp\":%lld,\"data\":[{\"device_id\":\"%s\",\"type\":12,\"idx\":1,\
\"pos_ae\":%.1f,\"neg_ae\":%.1f,\"com_ap\":%.1f,\"com_rap\":%.1f,\
\"ua\":%.1f,\"ub\":%.1f,\"uc\":%.1f,\"ia\":%.1f,\"ib\":%.1f,\"ic\":%.1f,\
\"gf\":%.1f,\"pf\":%.1f,\"pos_adem\":%.1f}]}", 
                        set->szCloudUserName, (long long)time(NULL)*1000, "CtnMeter1",
                        me->pos_active_e,me->neg_active_e,me->com_active_p,me->com_ractive_p,
                        me->ua, me->ub, me->uc, me->ia, me->ib, me->ic,
                        me->freq, me->pwr_factor, me->pos_active_dem);
                        pub_opts.message = mg_str(msg);
                        pub_opts.qos = m->s_qos, pub_opts.retain = false;
                        pub_opts.topic = mg_str(m->szs_pub_topic[1]);
                        mg_mqtt_pub(m->s_conn, &pub_opts);   
                        m->TotalSend++;
                    }  
                    // CtnMeter2
                    me = &APPL.Dtsd1352[2];
                    if(me->CommState == ST_COMM_NORM){
                        sprintf(msg, "{\"project_id\":\"%s\",\"timestamp\":%lld,\"data\":[{\"device_id\":\"%s\",\"type\":12,\"idx\":1,\
\"pos_ae\":%.1f,\"neg_ae\":%.1f,\"com_ap\":%.1f,\"com_rap\":%.1f,\
\"ua\":%.1f,\"ub\":%.1f,\"uc\":%.1f,\"ia\":%.1f,\"ib\":%.1f,\"ic\":%.1f,\
\"gf\":%.1f,\"pf\":%.1f,\"pos_adem\":%.1f}]}", 
                        set->szCloudUserName, (long long)time(NULL)*1000, "CtnMeter2",
                        me->pos_active_e,me->neg_active_e,me->com_active_p,me->com_ractive_p,
                        me->ua, me->ub, me->uc, me->ia, me->ib, me->ic,
                        me->freq, me->pwr_factor, me->pos_active_dem);
                        pub_opts.message = mg_str(msg);
                        pub_opts.qos = m->s_qos, pub_opts.retain = false;
                        pub_opts.topic = mg_str(m->szs_pub_topic[2]);
                        mg_mqtt_pub(m->s_conn, &pub_opts);   
                        m->TotalSend++;
                    }                                      
                    // TransMeter1
                    me = &APPL.Dtsd1352[3];
                    if(me->CommState == ST_COMM_NORM){
                        sprintf(msg, "{\"project_id\":\"%s\",\"timestamp\":%lld,\"data\":[{\"device_id\":\"%s\",\"type\":16,\"idx\":1,\
\"pos_ae\":%.1f,\"neg_ae\":%.1f,\"com_ap\":%.1f,\"com_rap\":%.1f,\
\"ua\":%.1f,\"ub\":%.1f,\"uc\":%.1f,\"ia\":%.1f,\"ib\":%.1f,\"ic\":%.1f,\
\"gf\":%.1f,\"pf\":%.1f,\"pos_adem\":%.1f}]}", 
                        set->szCloudUserName, (long long)time(NULL)*1000, "TransMeter1",
                        me->pos_active_e,me->neg_active_e,me->com_active_p,me->com_ractive_p,
                        me->ua, me->ub, me->uc, me->ia, me->ib, me->ic,
                        me->freq, me->pwr_factor, me->pos_active_dem);
                        pub_opts.message = mg_str(msg);
                        pub_opts.qos = m->s_qos, pub_opts.retain = false;
                        pub_opts.topic = mg_str(m->szs_pub_topic[4]);
                        mg_mqtt_pub(m->s_conn, &pub_opts);   
                        m->TotalSend++;
                    }            
                    // TransMeter2
                    me = &APPL.Dtsd1352[4];
                    if(me->CommState == ST_COMM_NORM){
                        sprintf(msg, "{\"project_id\":\"%s\",\"timestamp\":%lld,\"data\":[{\"device_id\":\"%s\",\"type\":16,\"idx\":1,\
\"pos_ae\":%.1f,\"neg_ae\":%.1f,\"com_ap\":%.1f,\"com_rap\":%.1f,\
\"ua\":%.1f,\"ub\":%.1f,\"uc\":%.1f,\"ia\":%.1f,\"ib\":%.1f,\"ic\":%.1f,\
\"gf\":%.1f,\"pf\":%.1f,\"pos_adem\":%.1f}]}", 
                        set->szCloudUserName, (long long)time(NULL)*1000, "TransMeter2",
                        me->pos_active_e,me->neg_active_e,me->com_active_p,me->com_ractive_p,
                        me->ua, me->ub, me->uc, me->ia, me->ib, me->ic,
                        me->freq, me->pwr_factor, me->pos_active_dem);
                        pub_opts.message = mg_str(msg);
                        pub_opts.qos = m->s_qos, pub_opts.retain = false;
                        pub_opts.topic = mg_str(m->szs_pub_topic[5]);
                        mg_mqtt_pub(m->s_conn, &pub_opts);   
                        m->TotalSend++;
                    }                                                                               
                    // GateMeter
                    me = &APPL.Dtsd1352[5];
                    // me->com_active_e = APPL.Dtsd1352[3].com_active_e + APPL.Dtsd1352[4].com_active_e;
                    // me->com_active_p = APPL.Dtsd1352[3].com_active_p + APPL.Dtsd1352[4].com_active_p;
                    // me->com_ractive_p = APPL.Dtsd1352[3].com_ractive_p + APPL.Dtsd1352[4].com_ractive_p;
                    // me->neg_active_dem = APPL.Dtsd1352[3].neg_active_dem + APPL.Dtsd1352[4].neg_active_dem;
                    // me->neg_active_e = APPL.Dtsd1352[3].neg_active_e + APPL.Dtsd1352[4].neg_active_e;
                    // me->pos_active_dem = APPL.Dtsd1352[3].pos_active_dem + APPL.Dtsd1352[4].pos_active_dem;
                    // me->pos_active_e = APPL.Dtsd1352[3].pos_active_e + APPL.Dtsd1352[4].pos_active_e;
                    // if(APPL.Dtsd1352[3].CommState == ST_COMM_NORM && APPL.Dtsd1352[4].CommState == ST_COMM_NORM)
                    if(me->CommState == ST_COMM_NORM)
                    { 
                        strcpy(me->szCommState,"正常");
                        sprintf(msg, "{\"project_id\":\"%s\",\"timestamp\":%lld,\"data\":[{\"device_id\":\"%s\",\"type\":17,\"idx\":1,\
\"pos_ae\":%.1f,\"neg_ae\":%.1f,\"com_ap\":%.1f,\"com_rap\":%.1f,\
\"ua\":%.1f,\"ub\":%.1f,\"uc\":%.1f,\"ia\":%.1f,\"ib\":%.1f,\"ic\":%.1f,\
\"gf\":%.1f,\"pf\":%.1f,\"pos_adem\":%.1f}]}", 
                        set->szCloudUserName, (long long)time(NULL)*1000, "GateMeter",
                        me->pos_active_e,me->neg_active_e,me->com_active_p,me->com_ractive_p,
                        me->ua, me->ub, me->uc, me->ia, me->ib, me->ic,
                        me->freq, me->pwr_factor, me->pos_active_dem);
                        pub_opts.message = mg_str(msg);
                        pub_opts.qos = m->s_qos, pub_opts.retain = false;
                        pub_opts.topic = mg_str(m->szs_pub_topic[3]);
                        mg_mqtt_pub(m->s_conn, &pub_opts);   
                        m->TotalSend++;
                    }else{
                        me->CommState = ST_COMM_ERR;
                        strcpy(me->szCommState,"故障");
                    }                                                                                                                                                                       
        }
        }
        if (m->s_conn == NULL) m->s_conn = mg_mqtt_connect(&mgr_mqtt2, m->szs_url, &opts, fn_mqtt2, NULL);
        }

    syslog(LOG_INFO, "%s EXIT, idx:2", __func__);
}

// Get data from mosquitto server
static void* thrd_mqtt_3(void* param){
    struct chanmqtt_t* m = &APPL.chanmqtt[3];
    
    struct mg_mqtt_opts opts = {.user = mg_str(m->szusrname),
                                .clean = true,
                                .qos = m->s_qos,
                                .topic = mg_str(m->szs_pub_topic),
                                .version = 4,
                                .keepalive = 3,
                                .message = mg_str("bye")};
    struct mg_mqtt_opts pub_opts;          
    struct mg_str pubt = mg_str(m->szs_pub_topic);        
    char msg[2048]; 
    int64_t LastReconn = 0;
    int64_t LastCommCheck = 0;

    mg_mgr_init(&mgr_mqtt3);    

    syslog(LOG_INFO,"%s ENTER, idx:3", __func__);

    if (m->s_conn == NULL) m->s_conn = mg_mqtt_connect(&mgr_mqtt3, m->szs_url, &opts, fn_mqtt3, NULL);
    while(1){
        mg_mgr_poll(&mgr_mqtt3, 50);
        if(mg_millis() - LastReconn > 5000){ // 5s
            LastReconn = mg_millis();
            if (m->s_conn == NULL) m->s_conn = mg_mqtt_connect(&mgr_mqtt1, m->szs_url, &opts, fn_mqtt3, NULL);
        }
        if(mg_millis() - LastCommCheck > 1000){ // 1s
            LastCommCheck = mg_millis();
        }
    }

    syslog(LOG_INFO,"%s EXIT, idx:1", __func__);
}

static int mqtt4_connlost = 0;
static void fn_mqtt4_connlost(void *context, char *cause)
{
    syslog(LOG_INFO, "%s, mqtt connection lost, cause: %s\n", __func__, cause);
    mqtt4_connlost = 1;
}

static int fn_mqtt4_msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    struct chanmqtt_t* mq = &APPL.chanmqtt[4];
    int64_t CurrIntv;

    //syslog(LOG_INFO,"%s, Message arrived, topic:%s topic len:%d payload len:%d", __func__, topicName,topicLen, message->payloadlen);
    struct mg_str json = mg_str(message->payload);    
    double dval;
    bool ok;
    if( strcmp(topicName, mq->szs_sub_topic[0]) == 0){
        if(mq->LastRecv == 0){ // First Recv
            mq->TotalRecv = 0;
            mq->LastRecv = mg_millis();
            mq->TotalIntv = 0;
        }else{
            mq->TotalRecv++;
            CurrIntv = mg_millis() - mq->LastRecv;
            mq->LastRecv = mg_millis();
            if(CurrIntv > mq->MaxIntv){
                mq->MaxIntv = CurrIntv;
            }
            mq->TotalIntv += CurrIntv;
            mq->AvgIntv = mq->TotalIntv/mq->TotalRecv;
        }        
        ok = mg_json_get_num(json,"$.m1_com_ap", &dval);
        if( ok ){
            // cm->com_active_p = dval;
            // cm->LastUpdate = mg_millis();
            // strcpy(cm->szLastUpdate, appl_get_datetime_long());
            //syslog(LOG_INFO,"%s, Get Gate M1 Data", __func__);
        }
        ok = mg_json_get_num(json,"$.m2_com_ap", &dval);
        if( ok ){
            // pm->com_active_p = dval;
            // pm->LastUpdate = mg_millis();
            // strcpy(pm->szLastUpdate, appl_get_datetime_long());
            //syslog(LOG_INFO,"%s, Get Gate M2 Data", __func__);
        }
        ok = mg_json_get_num(json,"$.m3_com_ap", &dval);
        if( ok ){
            // tm->com_active_p = dval;
            // tm->LastUpdate = mg_millis();
            // strcpy(tm->szLastUpdate, appl_get_datetime_long());
            //syslog(LOG_INFO,"%s, Get Gate M2 Data", __func__);
        }        
        ok = mg_json_get_num(json,"$.m4_com_ap", &dval);
        if( ok ){
            //gm->com_active_p = dval;
            //gm->LastUpdate = mg_millis();
            //strcpy(gm->szLastUpdate, appl_get_datetime_long());
            //syslog(LOG_INFO,"%s, Get Gate M2 Data", __func__);
        }          
    }
}

static void mqtt4_connect( void ){
    int rc;
    struct chanmqtt_t* m = &APPL.chanmqtt[4];
    MQTTClient_deliveryToken token;
    MQTTClient_connectOptions conn_opts;
    MQTTClient cli;

    MQTTClient_connectOptions tmpconn_opts = MQTTClient_connectOptions_initializer5;
    conn_opts = tmpconn_opts;
    MQTTClient_createOptions createOpts = MQTTClient_createOptions_initializer;
    createOpts.MQTTVersion = MQTTVERSION_5;

    if ((rc = MQTTClient_createWithOptions(&cli, m->szs_url, m->szclientid, MQTTCLIENT_PERSISTENCE_NONE, NULL, &createOpts)) != MQTTCLIENT_SUCCESS){
        syslog(LOG_INFO,"%s, MQTTClient_createWithOptions fail, rc:%d msg:%s  %s    %s", __func__, rc, MQTTClient_strerror(rc));
    }
    conn_opts.keepAliveInterval = 8;
    conn_opts.cleansession = 0;
    conn_opts.username = m->szusrname;
    conn_opts.password = m->szpasswd;

    MQTTProperties props = MQTTProperties_initializer;
    MQTTProperties willProps = MQTTProperties_initializer;
    MQTTResponse response = MQTTResponse_initializer;
    MQTTClient_setCallbacks(cli, NULL, fn_mqtt4_connlost, fn_mqtt4_msgarrvd, NULL);
    response = MQTTClient_connect5(cli, &conn_opts, &props, &willProps);
    if (response.reasonCode != MQTTCLIENT_SUCCESS){
        syslog(LOG_INFO,"%s, MQTTClient_connect fail, rc:%d msg:%s", __func__, response.reasonCode, MQTTClient_strerror(response.reasonCode));
        mqtt4_connlost = 1;
    }else{
        syslog(LOG_INFO,"%s, Connect Ok",__func__);
        mqtt4_connlost = 0;
        response = MQTTClient_subscribe5(cli, m->szs_sub_topic[0], m->s_qos, NULL, NULL);
        if (response.reasonCode != MQTTCLIENT_SUCCESS && response.reasonCode != m->s_qos){
            syslog(LOG_INFO,"%s, MQTTClient_subscribe fail, rc: %d msg: %s", __func__, response.reasonCode, MQTTClient_strerror(response.reasonCode));
        }
    }
    MQTTResponse_free(response);
}

static void* thrd_mqtt4(void* param){
    int ReconnChk = 0;

    syslog(LOG_INFO,"%s, ++",__func__);
    mqtt4_connect();
    while(1){
        if(++ReconnChk > 10){
            ReconnChk = 0;
            if(mqtt4_connlost == 1){
                mqtt4_connect();
            }
        }
        
        sleep(1);
    }
    syslog(LOG_INFO,"%s, --",__func__);
}

void appl_snap_set_err( void )
{
    APPL.Snap.bErr = 1;
    strcpy(APPL.Snap.szState,"故障");
}

void appl_snap_reset_err( void )
{
    APPL.Snap.bErr = 0;
    strcpy(APPL.Snap.szState,"正常");
}

static int appl_snap_day_diff(int year_start, int month_start, int day_start, int year_end, int month_end, int day_end){
    int y2, m2, d2;
    int y1, m1, d1;

    m1 = (month_start + 9) % 12;
    y1 = year_start - m1 / 10;
    d1 = 365 * y1 + y1 / 4 - y1 / 100 + y1 / 400 + (m1 * 306 + 5) / 10 + (day_start - 1);

    m2 = (month_end + 9) % 12;
    y2 = year_end - m2 / 10;
    d2 = 365 * y2 + y2 / 4 - y2 / 100 + y2 / 400 + (m2 * 306 + 5) / 10 + (day_end - 1);

    return (d2 - d1);
}

int appl_snap_rmdir(const char *path) {
   DIR *d = opendir(path);
   size_t path_len = strlen(path);
   int r = -1;

   if (d) {
      struct dirent *p;

      r = 0;
      while (!r && (p=readdir(d))) {
          int r2 = -1;
          char *buf;
          size_t len;

          /* Skip the names "." and ".." as we don't want to recurse on them. */
          if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
             continue;

          len = path_len + strlen(p->d_name) + 2; 
          buf = malloc(len);

          if (buf) {
             struct stat statbuf;

             snprintf(buf, len, "%s/%s", path, p->d_name);
             if (!stat(buf, &statbuf)) {
                if (S_ISDIR(statbuf.st_mode))
                   r2 = appl_snap_rmdir(buf);
                else
                   r2 = unlink(buf);
             }
             free(buf);
          }
          r = r2;
      }
      closedir(d);
   }

   if (!r)
      r = rmdir(path);

   return r;
}

static void* thrd_snap(void* param){
    struct Snap_t* s = &APPL.Snap;
    struct Dtsd1352_t* me = NULL;
    char buf[128];
    char szfn[128];
    int y, m, d, h, min, ss; // current
    int yy, mm, dd;//dir
    int diff_day;
    int rc;
    DIR* dir;
    struct dirent *ptr;
    char szyy[8];
    char szmm[8];
    char szdd[8];
    int i;

    sleep(5);
    syslog(LOG_INFO, "%s, ++",__func__);

    while(1){
        usleep(300000);
        if(s->bErr){
            continue;
        }
        if (s->bStart == 0){
            appl_get_datetime_num(&y, &m, &d, &h, &min, &ss);
            sprintf(s->szcurrDatePath, "./snap/%04d-%02d-%02d", y, m, d);
            if( access(s->szcurrDatePath, NULL) !=0 ){ // directory does not exists
                //syslog(LOG_INFO,"%s, DatePath:%s does not exist",__func__, s->szcurrDatePath);
                if(mkdir(s->szcurrDatePath, 0755) < 0){
                    s->bErr = 1;
                    //syslog(LOG_INFO,"%s, mkdir fail",__func__);
                    continue;
                }
            }
               
            // Meter Snap Start
            for( i = 1; i <= 5; i++ ){
                sprintf(szfn, "%s/M%d_%04d-%02d-%02d %02d-%02d-%02d.csv",s->szcurrDatePath, i, y, m, d, h, min, ss);
                s->f[i] = fopen(szfn, "w+");
                if (s->f[i] == NULL){
                    s->bErr = 1;
                    continue;
                }else{
                    rc = fprintf(s->f[i],"ts,com_ap,pos_adem,neg_adem,com_ae,pos_ae,neg_ae,pf,pt,ct,ua,ub,uc,ia,ib,ic,freq,CommState\n");
                    if(rc < 0){
                        s->bErr = 1;
                        continue;
                    }
                }
            }        
          
            s->LastSnap = 0;
            s->bStart = 1;
        }else{
            if (mg_millis() - s->LastSnap > 5000){ /* snap every 5 seconds */
                s->LastSnap = mg_millis();
                appl_get_datetime_num(&y, &m, &d, &h, &min, &ss);
                sprintf(buf, "./snap/%04d-%02d-%02d", y, m, d);
                if (strcmp(buf, s->szcurrDatePath) != 0){ /* new date */
                    //syslog(LOG_INFO,"%s, New Date Detected : %s", __func__, buf);
                    if(s->fpcs != NULL){
                        fclose(s->fpcs);
                        s->fpcs = NULL;
                    }
                    // del outofdate dir
                    if ((dir = opendir("./snap")) == NULL){
                        s->bErr = 1;
                        continue;
                    }else{
                            while ((ptr = readdir(dir)) != NULL){
                                if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0){ /// current dir OR parrent dir
                                    continue;
                                }else if (ptr->d_type == 8){ /// file
                                }
                                else if (ptr->d_type == 10){ /// link file
                                    // printf("d_name:%s/%s\n",basePath,ptr->d_name);
                                }else if (ptr->d_type == 4){ /// dir
                                    if(strlen(ptr->d_name) == 10 && ptr->d_name[4] == '-' && ptr->d_name[7] == '-'){ // target dir
                                        //syslog(LOG_INFO,"%s, Target Dir:%s Detedted", __func__, ptr->d_name);
                                        strncpy(szyy, ptr->d_name, 4);
                                        strncpy(szmm, ptr->d_name + 5, 2);
                                        strncpy(szdd, ptr->d_name + 8, 2);
                                        yy = atoi(szyy);
                                        mm = atoi(szmm);
                                        dd = atoi(szdd);
                                        diff_day = appl_snap_day_diff(yy, mm, dd, y, m, d);
                                        if (diff_day > s->KeepDay){
                                            sprintf(buf, "./snap/%s",ptr->d_name);
                                            rc = appl_snap_rmdir(buf);
                                            if( rc < 0 ){
                                                syslog(LOG_INFO,"%s, Target Dir:%s Del Fail", __func__, ptr->d_name);
                                                s->bErr = 1;
                                                continue;
                                            }else{
                                                syslog(LOG_INFO,"%s, Target Dir:%s Del Ok", __func__, ptr->d_name);
                                            }
                                        }
                                    }
                                }
                            }
                            closedir(dir);
                    }
                    s->bStart = 0; /* start again at next loop */
                    continue;
                }else{
                s->LastSnap = mg_millis();
                strcpy(buf, appl_get_datetime_short());
        
                // Meter Snap
                for( i = 1; i <= 5; i++){
                    me = &APPL.Dtsd1352[i];
                if(s->f[i] != NULL && me->CommState == ST_COMM_NORM){
                    fprintf(s->f[i],"\
%s,%.1f,%.1f,%.1f,\
%.1f,%.1f,%.1f,\
%.3f,%d,%d,\
%.1f,%.1f,%.1f,\
%.1f,%.1f,%.1f,\
%.1f,%.d\n",
/*1*/buf, me->com_active_p,me->pos_active_dem,me->neg_active_dem,
/*2*/me->com_active_e,me->pos_active_e,me->neg_active_e,
/*3*/me->pwr_factor, me->PT, me->CT,
/*4*/me->ua, me->ub, me->uc,
/*5*/me->ia, me->ib, me->ic,
/*6*/me->freq, me->CommState);
                    fflush(s->f[i]);
                    }
                }                                                              
                }
            }
        }
    }
    syslog(LOG_INFO, "%s, --",__func__);
}

void appl_start( void )
{
    struct Settings_t* set = &APPL.Set.s;
    struct chan485_t* ch = NULL;
    struct chanmqtt_t* m = NULL;
    struct Snap_t* snap = &APPL.Snap;
    char buf[128];
    char buf2[128];
    int len;
    int i;
    pthread_t hthrd_485_1;
    pthread_t hthrd_485_2;
    pthread_t hthrd_485_3;
    pthread_t hthrd_485_4;
    pthread_t hthrd_can_1;
    pthread_t hthrd_can_2;
    pthread_t hthrd_dido;
    pthread_t hthrd_ctl;
    pthread_t hthrd_mqtt1;
    pthread_t hthrd_mqtt2;
    pthread_t hthrd_mqtt3;
    pthread_t hthrd_mqtt4;
    pthread_t hthrd_snap;

   appl_485_set_485mode();

    // Set
    if(appl_cfg_read() != 0){
        appl_cfg_set_err();
        syslog(LOG_INFO,"%s, appl_cfg_read fail", __func__);

        // Set Default
        strcpy(set->szCloudUrl,"119.45.116.112:18883");
        set->DataKeepDay = 700;
        set->UploadHighSpeed = 1500;
        set->UploadMediumSpeed = 60000;
        set->UploadSlowSpeed = 180000;
        // Load Default
        snap->KeepDay = 30;
    }else{
        appl_cfg_reset_err();
        // //1# MQTT thingsboard
        // m = &APPL.chanmqtt[1];
        // m->s_conn = NULL;
        // strcpy(m->szs_url,"124.222.45.156:1883");
        // strcpy(m->szs_pub_topic,"v1/devices/me/telemetry");
        // strcpy(m->szs_sub_topic,"v1/devices/me/ctl");
        // m->s_qos = 1;
        // //测试设备
        // //strcpy(m->szusrname,"gFCNk8oSxC6VlYXkhs3a");
        // strcpy(m->szusrname,"DZHbY2HAGeATfRCfhlW7");
        // m->bConnected = 0;

        //2# MQTT Cloud
        m = &APPL.chanmqtt[2];
        m->s_conn = NULL;
        strncpy(m->szs_url, set->szCloudUrl, sizeof(m->szs_url)-1);
        strncpy(m->szusrname,set->szCloudUserName, sizeof(m->szusrname) - 1);
        strncpy(m->szpasswd,set->szCloudPasswd, sizeof(m->szpasswd) - 1);
        strncpy(m->szclientid,set->szClientId, sizeof(m->szclientid) - 1);
        sprintf(m->szs_pub_topic[1],"sequential/%s/CtnMeter1",m->szusrname);
        sprintf(m->szs_pub_topic[2],"sequential/%s/CtnMeter2",m->szusrname);
        sprintf(m->szs_pub_topic[3],"sequential/%s/GateMeter",m->szusrname);
        sprintf(m->szs_pub_topic[4],"sequential/%s/TransMeter1",m->szusrname);
        sprintf(m->szs_pub_topic[5],"sequential/%s/TransMeter2",m->szusrname);
        strcpy(m->szs_sub_topic,"control");
        m->s_qos = 1;
        m->bConnected = 0;       
        pthread_create(&hthrd_mqtt2, NULL, thrd_mqtt_2, NULL);    
    }  
//电表地址，根据实际现场进行编写
    APPL.Dtsd1352[1].Adr = 1 ; // 1#储能电表
    APPL.Dtsd1352[2].Adr = 2; // 2#储能电表
    APPL.Dtsd1352[3].Adr = 3; // 1#变压器电表
    APPL.Dtsd1352[4].Adr = 4; // 2#变压器电表
    APPL.Dtsd1352[5].Adr = 5; // 关口表，实际不存在

    // CHAN 485 1
    ch = &APPL.chan485[1];
    strcpy(ch->szdev, "/dev/ttymxc1");
    ch->baud = 9600;
    ch->parity = 'N';
    strcpy(ch->szinfo, "1~2#储能柜电表");
    
    // CHAN 485 2
    ch = &APPL.chan485[2];
    strcpy(ch->szdev, "/dev/ttymxc2");
    ch->baud = 9600;
    ch->parity = 'N';
    strcpy(ch->szinfo, "1#~2#变压器电表");
    
    // CHAN 485 3
    ch = &APPL.chan485[3];
    strcpy(ch->szdev, "/dev/ttymxc3");
    ch->baud = 9600;
    ch->parity = 'N';
    strcpy(ch->szinfo, "无");    

    // CHAN 485 4
    ch = &APPL.chan485[4];
    strcpy(ch->szdev, "/dev/ttymxc5");
    ch->baud = 9600;
    ch->parity = 'N';
    strcpy(ch->szinfo, "关口电表");        

    pthread_create(&hthrd_485_1, NULL, thrd_485_1, NULL);
    pthread_create(&hthrd_485_2, NULL, thrd_485_2, NULL);
   // pthread_create(&hthrd_485_3, NULL, thrd_485_3, NULL);
    pthread_create(&hthrd_485_4, NULL, thrd_485_4, NULL);

    // Snap
    appl_snap_reset_err();
    snap->bStart = 0;
    snap->KeepDay = set->DataKeepDay;
    pthread_create(&hthrd_snap, NULL, thrd_snap, NULL);
}
