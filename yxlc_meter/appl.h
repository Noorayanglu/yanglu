#ifndef __APPL_H__
#define __APPL_H__

enum CommState_t
{
    ST_COMM_ERR = 0,
    ST_COMM_NORM = 1,
};

enum chan485_state_t
{
    ST_485_INIT = 0,
    ST_485_RUN,
    ST_485_ERR,
};

enum chan485_err_t{
    ERR_485_NONE = 0,
    ERR_485_INIT_FAIL,
};

enum chan485_cmd_t
{
    CMD_485_DONE = 0,
    CMD_485_RESET = 1,
};

#define CHAN485_NBR 8
struct chan485_t{
    pthread_mutex_t mutex;
    char szinfo[128];
    int state;
    char szstate[32];
    int err;
    char szerr[64];
    int Cmd;
    int CmdParam;

    int baud;
    char szdev[32];
    char parity;
    int64_t reqcnt;
    int64_t failcnt;
    int64_t loopcnt;
    int64_t looptime;
};

enum chancan_state_t
{
    ST_CAN_INIT = 1,
    ST_CHANCAN_RUN,
    ST_CHANCAN_ERR,
};

enum chancan_err_t{
    ERR_CAN_NONE = 0,
    ERR_CAN_INIT_FAIL,
};

enum chancan_cmd_t
{
    CMD_CHANCAN_DONE = 0,
    CMD_CHANCAN_RESET = 1,
};

#define CHANCAN_NBR 3
struct chancan_t{
    int State;
    char szState[32];
    int Err;
    char szErr[128];
    int Cmd;
    int sock;
    char szdev[32];
    int64_t RdCnt;
    int64_t RdFailcnt;
    int64_t WrCnt;
    int64_t WrFailcnt;
    int64_t Loopcnt;
    int64_t LoopTime;
    char szinfo[128];

    pthread_mutex_t mutex;
};

#define CHAN_MQTT_NBR 4
struct chanmqtt_t{
    char szs_url[128];
    char szs_sub_topic[16][128]; 
    char szs_pub_topic[16][128];
    char szusrname[128];
    char szpasswd[128];
    char szclientid[128];
    int s_qos;                             // MQTT QoS
    struct mg_connection* s_conn;              // Client connection
    char szState[64];
    int bConnected;

    int64_t LastFastUpload;
    int64_t LastMediumUpload;
    int64_t LastSlowUpload;

    int Cmd;
    int CmdParam;

    int64_t TotalReconn;
    int64_t TotalSend;
    int64_t TotalRecv;

    int MaxIntv;
    int AvgIntv;
    int64_t TotalIntv;
    int64_t LastRecv;    
};

enum MqttCmd_t{
    CMD_MQTT_DONE = 0,
    CMD_MQTT_REGISTER = 1,
};

// !!! 注意，不要插入
struct Settings_t{
    long long chksum;
    int bErr;
    char szState[32];

    char szCloudUserName[128];
    char szCloudPasswd[128];
    char szCloudUrl[128];
    char szClientId[128];

    int DataKeepDay;

    int UploadHighSpeed; // ms
    int UploadMediumSpeed; //ms
    int UploadSlowSpeed; //ms
};

union UnSettings_t{
    struct Settings_t s;
    char buf[1024*64];
};

enum SettingsCmd_t{
    SETTINGS_CMD_SET_CLOUD_USERNAME = 101,
    SETTINGS_CMD_SET_CLOUD_PASSWD = 102,
    SETTINGS_CMD_SET_CLOUD_URL = 103,
    SETTINGS_CMD_REGISTER = 104,
    SETTINGS_CMD_SET_CLOUD_CLIENTID = 105,
    SETTINGS_CMD_SET_CLOUD_UPLOADINTV = 106,

    SETTINGS_CMD_SET_DATAKEEPDAY = 200,
    SETTINGS_CMD_SET_UPLOADHIHGSPEED = 201,
    SETTINGS_CMD_SET_UPLOADMEDIUMSPEED = 202,
    SETTINGS_CMD_SET_UPLOADSLOWSPEED = 203,    
    SETTINGS_CMD_TEST = 1000,
};

struct Snap_t{
    int enable;
    int timer;
    int intv;
    int bStart;
    int bErr;
    char szState[32];
    char szcurrDatePath[32];
    int datechk_timer;
    int KeepDay;
    FILE* fpcs;
    FILE* fbms;
    FILE* fac;
    FILE* fctl;
    FILE* fmisc;
    FILE* f[8];
    int64_t LastSnap;
};

#define DTSD1352_NBR 8
struct Dtsd1352_t{
    char szinfo[128];
    int Adr;

    double com_active_e;
    double com_active_p;
    double com_ractive_p;
    double pos_active_e;
    double pos_active_dem;
    double neg_active_dem;
    double neg_active_e;
    double pwr_factor;

    int PT;
    int CT;

    double ua;
    double ub;
    double uc;
    double ia;
    double ib;
    double ic;
    double freq;

    int64_t LastUpdate;
    char szLastUpdate[32];
    int CommState;
    char szCommState[32]; 
    int64_t CommFailTotalCnt;    
};

struct appl_t{
    struct chan485_t chan485[CHAN485_NBR + 1];
    struct chancan_t chancan[CHANCAN_NBR + 1];
    struct chanmqtt_t chanmqtt[CHAN_MQTT_NBR + 1];
    union  UnSettings_t Set;
    struct Dtsd1352_t Dtsd1352[DTSD1352_NBR + 1];
    struct Snap_t Snap;
};

extern char* VERSION;
extern char* CFG_FN;
extern struct appl_t APPL;
extern void appl_chan485_lock(int idx);
extern void appl_chan485_unlock(int idx);
#endif