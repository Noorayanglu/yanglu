// Copyright (c) 2023 Cesanta Software Limited
// All rights reserved

#include <syslog.h>
#include "net.h"
#include "appl.h"

// Authenticated user.
// A user can be authenticated by:
//   - a name:pass pair, passed in a header Authorization: Basic .....
//   - an access_token, passed in a header Cookie: access_token=....
// When a user is shown a login screen, she enters a user:pass. If successful,
// a server responds with a http-only access_token cookie set.
struct user {
  const char *name, *pass, *access_token;
};

// Settings
struct settings {
  bool log_enabled;
  int log_level;
  long brightness;
  char *device_name;
};

static struct settings s_settings = {true, 1, 57, NULL};

static const char *s_json_header =
    "Content-Type: application/json\r\n"
    "Cache-Control: no-cache\r\n";
static uint64_t s_boot_timestamp = 0;  // Updated by SNTP

// This is for newlib and TLS (mbedTLS)
uint64_t mg_now(void) {
  return mg_millis() + s_boot_timestamp;
}

int ui_event_next(int no, struct ui_event *e) {
  if (no < 0 || no >= MAX_EVENTS_NO) return 0;

  srand((unsigned) no);
  e->type = (uint8_t) rand() % 4;
  e->prio = (uint8_t) rand() % 3;
  e->timestamp =
      (unsigned long) ((int64_t) mg_now() - 86400 * 1000 /* one day back */ +
                       no * 300 * 1000 /* 5 mins between alerts */ +
                       1000 * (rand() % 300) /* randomize event time */) /
      1000UL;

  mg_snprintf(e->text, MAX_EVENT_TEXT_SIZE, "event#%d", no);
  return no + 1;
}

// SNTP connection event handler. When we get a response from an SNTP server,
// adjust s_boot_timestamp. We'll get a valid time from that point on
static void sfn(struct mg_connection *c, int ev, void *ev_data) {
  uint64_t *expiration_time = (uint64_t *) c->data;
  if (ev == MG_EV_OPEN) {
    *expiration_time = mg_millis() + 3000;  // Store expiration time in 3s
  } else if (ev == MG_EV_SNTP_TIME) {
    uint64_t t = *(uint64_t *) ev_data;
    s_boot_timestamp = t - mg_millis();
    c->is_closing = 1;
  } else if (ev == MG_EV_POLL) {
    if (mg_millis() > *expiration_time) c->is_closing = 1;
  }
}

static void timer_sntp_fn(void *param) {  // SNTP timer function. Sync up time
  mg_sntp_connect(param, "udp://time.google.com:123", sfn, NULL);
}

// Parse HTTP requests, return authenticated user or NULL
static struct user *authenticate(struct mg_http_message *hm) {
  // In production, make passwords strong and tokens randomly generated
  // In this example, user list is kept in RAM. In production, it can
  // be backed by file, database, or some other method.
  static struct user users[] = {
      {"admin", "admin", "admin_token"},
      {"user1", "user1", "user1_token"},
      {"user2", "user2", "user2_token"},
      {NULL, NULL, NULL},
  };
  char user[64], pass[64];
  struct user *u, *result = NULL;
  mg_http_creds(hm, user, sizeof(user), pass, sizeof(pass));
  MG_VERBOSE(("user [%s] pass [%s]", user, pass));

  if (user[0] != '\0' && pass[0] != '\0') {
    // Both user and password is set, search by user/password
    for (u = users; result == NULL && u->name != NULL; u++)
      if (strcmp(user, u->name) == 0 && strcmp(pass, u->pass) == 0) result = u;
  } else if (user[0] == '\0') {
    // Only password is set, search by token
    for (u = users; result == NULL && u->name != NULL; u++)
      if (strcmp(pass, u->access_token) == 0) result = u;
  }
  return result;
}

static void handle_login(struct mg_connection *c, struct user *u) {
  char cookie[256];
  mg_snprintf(cookie, sizeof(cookie),
              "Set-Cookie: access_token=%s; Path=/; "
              "%sHttpOnly; SameSite=Lax; Max-Age=%d\r\n",
              u->access_token, c->is_tls ? "Secure; " : "", 3600 * 24);
  mg_http_reply(c, 200, cookie, "{%m:%m}", MG_ESC("user"), MG_ESC(u->name));
}

static void handle_logout(struct mg_connection *c) {
  char cookie[256];
  mg_snprintf(cookie, sizeof(cookie),
              "Set-Cookie: access_token=; Path=/; "
              "Expires=Thu, 01 Jan 1970 00:00:00 UTC; "
              "%sHttpOnly; Max-Age=0; \r\n",
              c->is_tls ? "Secure; " : "");
  mg_http_reply(c, 200, cookie, "true\n");
}

static void handle_debug(struct mg_connection *c, struct mg_http_message *hm) {
  int level = mg_json_get_long(hm->body, "$.level", MG_LL_DEBUG);
  mg_log_set(level);
  mg_http_reply(c, 200, "", "Debug level set to %d\n", level);
}

static size_t print_int_arr(void (*out)(char, void *), void *ptr, va_list *ap) {
  size_t i, len = 0, num = va_arg(*ap, size_t);  // Number of items in the array
  int *arr = va_arg(*ap, int *);              // Array ptr
  for (i = 0; i < num; i++) {
    len += mg_xprintf(out, ptr, "%s%d", i == 0 ? "" : ",", arr[i]);
  }
  return len;
}

static void handle_stats_get(struct mg_connection *c) {
  int points[] = {21, 22, 22, 19, 18, 20, 23, 23, 22, 22, 22, 23, 22};
  mg_http_reply(c, 200, s_json_header, "{%m:%d,%m:%d,%m:[%M]}\n",
                MG_ESC("temperature"), 21,  //
                MG_ESC("humidity"), 67,     //
                MG_ESC("points"), print_int_arr,
                sizeof(points) / sizeof(points[0]), points);
}

static size_t print_events(void (*out)(char, void *), void *ptr, va_list *ap) {
  size_t len = 0;
  struct ui_event ev;
  int pageno = va_arg(*ap, int);
  int no = (pageno - 1) * EVENTS_PER_PAGE;
  int end = no + EVENTS_PER_PAGE;

  while ((no = ui_event_next(no, &ev)) != 0 && no <= end) {
    len += mg_xprintf(out, ptr, "%s{%m:%lu,%m:%d,%m:%d,%m:%m}\n",  //
                      len == 0 ? "" : ",",                         //
                      MG_ESC("time"), ev.timestamp,                //
                      MG_ESC("type"), ev.type,                     //
                      MG_ESC("prio"), ev.prio,                     //
                      MG_ESC("text"), MG_ESC(ev.text));
  }

  return len;
}

static void handle_events_get(struct mg_connection *c,
                              struct mg_http_message *hm) {
  int pageno = mg_json_get_long(hm->body, "$.page", 1);
  mg_http_reply(c, 200, s_json_header, "{%m:[%M], %m:%d}\n", MG_ESC("arr"),
                print_events, pageno, MG_ESC("totalCount"), MAX_EVENTS_NO);
}

static void handle_settings_set(struct mg_connection *c, struct mg_str body) {
  struct Settings_t* set = &APPL.Set.s;
  struct chanmqtt_t* mqtt = &APPL.chanmqtt[2];
  int cmd;
  int param;
  bool ok = true;
  int rc;
  long val;
  int i;
  char buf[128];
  char* p;

  val = mg_json_get_long(body, "$.cmd", -1);
  if(val != -1){
      cmd = val;
      syslog(LOG_INFO,"%s, cmd:%d", __func__, cmd);
      switch (cmd)
      {
      case SETTINGS_CMD_SET_CLOUD_USERNAME:
        p = mg_json_get_str(body, "$.param");
        if( p != NULL){
          strcpy(set->szCloudUserName, p);
          free(p);
          if(appl_cfg_save() != 0){
            appl_cfg_set_err();
            ok = false;
            syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_CLOUD_USERNAME, appl_cfg_save Fail", __func__);
          }
        }else{
          ok = false;
          syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_CLOUD_USERNAME, mg_json_get_str Fail", __func__);
        }
      break;      

      case SETTINGS_CMD_SET_CLOUD_PASSWD:
        p = mg_json_get_str(body, "$.param");
        if( p != NULL){
          strcpy(set->szCloudPasswd, p);
          free(p);
          if(appl_cfg_save() != 0){
            appl_cfg_set_err();
            ok = false;
            syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_CLOUD_PASSWD, appl_cfg_save Fail", __func__);
          }
        }else{
          ok = false;
          syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_CLOUD_PASSWD, mg_json_get_str Fail", __func__);
        }
      break;           

      case SETTINGS_CMD_SET_CLOUD_URL:
        p = mg_json_get_str(body, "$.param");
        if( p != NULL){
          strcpy(set->szCloudUrl, p);
          free(p);
          if(appl_cfg_save() != 0){
            appl_cfg_set_err();
            ok = false;
            syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_CLOUD_URL, appl_cfg_save Fail", __func__);
          }
        }else{
          ok = false;
          syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_CLOUD_URL, mg_json_get_str Fail", __func__);
        }
      break;        

      case SETTINGS_CMD_SET_CLOUD_CLIENTID:
        p = mg_json_get_str(body, "$.param");
        if( p != NULL){
          strcpy(set->szClientId, p);
          free(p);
          if(appl_cfg_save() != 0){
            appl_cfg_set_err();
            ok = false;
            syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_CLOUD_CLIENTID, appl_cfg_save Fail", __func__);
          }
        }else{
          ok = false;
          syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_CLOUD_CLIENTID, mg_json_get_str Fail", __func__);
        }
      break;        

      case SETTINGS_CMD_SET_UPLOADHIHGSPEED:
        val = mg_json_get_long(body, "$.param", -1);
        if( val == -1 ){
          ok = false;
          syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_UPLOADHIHGSPEED, mg_json_get_long Fail", __func__);
        }else{
          set->UploadHighSpeed = val;
          if(appl_cfg_save() != 0){
            syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_UPLOADHIHGSPEED, appl_cfg_save Fail", __func__);
            appl_cfg_set_err();
            ok = false;
          }else{
            syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_UPLOADHIHGSPEED, set->UploadHighSpeed is set to %d", __func__, set->UploadHighSpeed);
          }
        }
        break;           

      case SETTINGS_CMD_SET_UPLOADMEDIUMSPEED:
        val = mg_json_get_long(body, "$.param", -1);
        if( val == -1 ){
          ok = false;
          syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_UPLOADMEDIUMSPEED, mg_json_get_long Fail", __func__);
        }else{
          set->UploadMediumSpeed = val;
          if(appl_cfg_save() != 0){
            syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_UPLOADMEDIUMSPEED, appl_cfg_save Fail", __func__);
            appl_cfg_set_err();
            ok = false;
          }else{
            syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_UPLOADMEDIUMSPEED, set->UploadMediumSpeed is set to %d", __func__, set->UploadMediumSpeed);
          }
        }
        break;            

      case SETTINGS_CMD_SET_UPLOADSLOWSPEED:
        val = mg_json_get_long(body, "$.param", -1);
        if( val == -1 ){
          ok = false;
          syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_UPLOADSLOWSPEED, mg_json_get_long Fail", __func__);
        }else{
          set->UploadSlowSpeed = val;
          if(appl_cfg_save() != 0){
            syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_UPLOADSLOWSPEED, appl_cfg_save Fail", __func__);
            appl_cfg_set_err();
            ok = false;
          }else{
            syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_UPLOADSLOWSPEED, set->UploadSlowSpeed is set to %d", __func__, set->UploadSlowSpeed);
          }
        }
        break;               

      case SETTINGS_CMD_REGISTER:
        mqtt->Cmd = CMD_MQTT_REGISTER;
        break;     

      case SETTINGS_CMD_SET_DATAKEEPDAY:
        val = mg_json_get_long(body, "$.param", -1);
        if( val == -1 ){
          ok = false;
          syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_DATAKEEPDAY, mg_json_get_long Fail", __func__);
        }else{
          set->DataKeepDay = val;
          if(appl_cfg_save() != 0){
            syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_DATAKEEPDAY, appl_cfg_save Fail", __func__);
            appl_cfg_set_err();
            ok = false;
          }else{
            syslog(LOG_INFO, "%s, SETTINGS_CMD_SET_DATAKEEPDAY, set->DataKeepDay is set to %d", __func__, set->DataKeepDay);
          }
        }
        break;     

      case SETTINGS_CMD_TEST:
        syslog(LOG_INFO,"%s, Get Test Cmd", __func__);
        rc = appl_snap_rmdir("./snap/test");
        syslog(LOG_INFO,"%s, rc:%d", __func__, rc);
        break;  
      
      default:
        break;
      }    
  }else{
    ok = false;
    syslog(LOG_INFO,"%s, Get Cmd Fail", __func__);
  }
  mg_http_reply(c, 200, s_json_header,
                "{%m:%s,%m:%m}",                          //
                MG_ESC("status"), ok ? "true" : "false",  //
                MG_ESC("message"), MG_ESC(ok ? "Success" : "Failed"));
}

static void handle_ctl_get(struct mg_connection *c) {
  struct Settings_t* set = &APPL.Set.s;
  struct Snap_t* snap = &APPL.Snap;

  mg_http_reply(c, 200, s_json_header, "{\
%m:%m,%m:%m,\
%m:%m}\n",
/*1*/MG_ESC("数据记录状态"), MG_ESC(snap->szState),MG_ESC("参数配置状态"), MG_ESC(set->szState),
/*2*/MG_ESC("软件版本"),MG_ESC(VERSION));
}

static void handle_settings_get(struct mg_connection *c) {
  struct Settings_t* set = &APPL.Set.s;
  mg_http_reply(c, 200, s_json_header, "{\
%m:%m,%m:%m,%m:%m,%m:%m,\
%m:%d,%m:%d,%m:%d,%m:%d}\n",  
  MG_ESC("Cloud用户名"),MG_ESC(set->szCloudUserName),MG_ESC("Cloud密码"),MG_ESC(set->szCloudPasswd),MG_ESC("Cloud Url"),MG_ESC(set->szCloudUrl),MG_ESC("Cloud Client ID"),MG_ESC(set->szClientId),
  MG_ESC("数据保留天数"),set->DataKeepDay,MG_ESC("上传高速"),set->UploadHighSpeed,MG_ESC("上传中速"),set->UploadMediumSpeed,MG_ESC("上传低速"),set->UploadSlowSpeed);
}



static void handle_comm_get(struct mg_connection *c) {
  struct chan485_t* c1 = &APPL.chan485[1];
  struct chan485_t* c2 = &APPL.chan485[2];
  struct chan485_t* c3 = &APPL.chan485[3];
  struct chan485_t* c4 = &APPL.chan485[4];
  struct chancan_t* c5 = &APPL.chancan[1];
  struct chancan_t* c6 = &APPL.chancan[2];
  struct chanmqtt_t* c7 = &APPL.chanmqtt[1];
  struct chanmqtt_t* c8 = &APPL.chanmqtt[2];
  mg_http_reply(c, 200, s_json_header, "{\
%m:{%m:%m,%m:%m,%m:%d,%m:%m,%m:%m,%m:%lld,%m:%lld,%m:%lld,%m:%lld},\
%m:{%m:%m,%m:%m,%m:%d,%m:%m,%m:%m,%m:%lld,%m:%lld,%m:%lld,%m:%lld},\
%m:{%m:%m,%m:%m,%m:%d,%m:%m,%m:%m,%m:%lld,%m:%lld,%m:%lld,%m:%lld},\
%m:{%m:%m,%m:%m,%m:%d,%m:%m,%m:%m,%m:%lld,%m:%lld,%m:%lld,%m:%lld},\
%m:{%m:%m,%m:%m,%m:%m,%m:%m,%m:%lld,%m:%lld,%m:%lld,%m:%lld},\
%m:{%m:%m,%m:%m,%m:%m,%m:%m,%m:%lld,%m:%lld,%m:%lld,%m:%lld},\
%m:{%m:%m,%m:%lld,%m:%lld,%m:%lld},\
%m:{%m:%m,%m:%lld,%m:%lld,%m:%lld}}\n",
    MG_ESC("1#485"),MG_ESC("状态"), MG_ESC(c1->szstate),MG_ESC("故障码"),MG_ESC(c1->szerr),MG_ESC("波特率"),c1->baud,MG_ESC("设备名"), MG_ESC(c1->szdev),
                MG_ESC("备注"), MG_ESC(c1->szinfo), MG_ESC("循环次数"),c1->loopcnt, MG_ESC("循环时间"),c1->looptime, MG_ESC("请求次数"),c1->reqcnt, MG_ESC("失败次数"),c1->failcnt,
    MG_ESC("2#485"),MG_ESC("状态"), MG_ESC(c2->szstate),MG_ESC("故障码"),MG_ESC(c2->szerr),MG_ESC("波特率"),c2->baud,MG_ESC("设备名"),MG_ESC(c2->szdev),
                MG_ESC("备注"),MG_ESC(c2->szinfo),MG_ESC("循环次数"),c2->loopcnt,MG_ESC("循环时间"),c2->looptime,MG_ESC("请求次数"),c2->reqcnt,MG_ESC("失败次数"),c2->failcnt,
    MG_ESC("3#485"),MG_ESC("状态"), MG_ESC(c3->szstate),MG_ESC("故障码"),MG_ESC(c3->szerr),MG_ESC("波特率"),c3->baud,MG_ESC("设备名"),MG_ESC(c3->szdev),
                MG_ESC("备注"),MG_ESC(c3->szinfo),MG_ESC("循环次数"),c3->loopcnt,MG_ESC("循环时间"),c3->looptime,MG_ESC("请求次数"),c3->reqcnt,MG_ESC("失败次数"),c3->failcnt,
    MG_ESC("4#485"),MG_ESC("状态"), MG_ESC(c4->szstate),MG_ESC("故障码"),MG_ESC(c4->szerr),MG_ESC("波特率"),c4->baud,MG_ESC("设备名"),MG_ESC(c4->szdev),
                MG_ESC("备注"),MG_ESC(c4->szinfo),MG_ESC("循环次数"),c4->loopcnt,MG_ESC("循环时间"),c4->looptime,MG_ESC("请求次数"),c4->reqcnt,MG_ESC("失败次数"),c4->failcnt,
    MG_ESC("1#CAN"),MG_ESC("状态"), MG_ESC(c5->szState),MG_ESC("故障码"),MG_ESC(c5->szErr),MG_ESC("设备名"),MG_ESC(c5->szdev),
                MG_ESC("备注"),MG_ESC(c5->szinfo),MG_ESC("循环次数"),c5->Loopcnt,MG_ESC("循环时间"),c5->LoopTime,MG_ESC("发送帧数"),c5->WrCnt,MG_ESC("接收帧数"),c5->RdCnt,   
    MG_ESC("2#CAN"),MG_ESC("状态"), MG_ESC(c6->szState),MG_ESC("故障码"),MG_ESC(c6->szErr),MG_ESC("设备名"),MG_ESC(c6->szdev),
                MG_ESC("备注"),MG_ESC(c6->szinfo),MG_ESC("循环次数"),c6->Loopcnt,MG_ESC("循环时间"),c6->LoopTime,MG_ESC("发送帧数"),c6->WrCnt,MG_ESC("接收帧数"),c6->RdCnt,
    MG_ESC("1#MQTT"),MG_ESC("状态"),MG_ESC(c7->szState),MG_ESC("发送计数"), c7->TotalSend,MG_ESC("接收计数"), c7->TotalRecv,MG_ESC("重连计数"), c7->TotalReconn,
    MG_ESC("2#MQTT"),MG_ESC("状态"),MG_ESC(c8->szState),MG_ESC("发送计数"), c8->TotalSend,MG_ESC("接收计数"),c8->TotalRecv,MG_ESC("重连计数"), c8->TotalReconn);
}

static void handle_comm_set(struct mg_connection *c, struct mg_str body) {
  struct chan485_t* ch = NULL;
  int idx = mg_json_get_long(body, "$.index", 0);
  int cmd = mg_json_get_long(body, "$.cmd", 0);
  printf("%s idx:%d cmd:%d\n", __func__, idx, cmd);
  if( idx >= 1 && idx <= 4 ){
    ch = &APPL.chan485[idx];
    ch->Cmd = cmd;
    mg_http_reply(c, 200, s_json_header, "true\n");
  }else{
    mg_http_reply(c, 403, s_json_header, "false\n");
  }
}

static void handle_meter_get(struct mg_connection *c) {
  struct Dtsd1352_t* m1 = &APPL.Dtsd1352[1]; // 1#储能电表
  struct Dtsd1352_t* m2 = &APPL.Dtsd1352[2]; // 2#储能电表
  struct Dtsd1352_t* m3 = &APPL.Dtsd1352[3]; // 1#变压器电表
  struct Dtsd1352_t* m4 = &APPL.Dtsd1352[4]; // 2#变压器电表
  struct Dtsd1352_t* m5 = &APPL.Dtsd1352[5]; // 关口电表
  mg_http_reply(c, 200, s_json_header, "{\
%m:{%m:%m,%m:%m,%m:%lld,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%d,%m:%d},\
%m:{%m:%m,%m:%m,%m:%lld,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%d,%m:%d},\
%m:{%m:%m,%m:%m,%m:%lld,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%d,%m:%d},\
%m:{%m:%m,%m:%m,%m:%lld,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%d,%m:%d},\
%m:{%m:%m,%m:%m,%m:%lld,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%f,%m:%d,%m:%d}}\n",
/*1*/MG_ESC("1#储能电表"),MG_ESC("通信状态"), MG_ESC(m1->szCommState),MG_ESC("数据更新时间"),MG_ESC(m1->szLastUpdate),MG_ESC("通信失败次数"),m1->CommFailTotalCnt,
     MG_ESC("组合有功功率"), m1->com_active_p, MG_ESC("正向有功总电能"),m1->pos_active_e, MG_ESC("反向有功总电能"),m1->neg_active_e, 
     MG_ESC("功率因数"),m1->pwr_factor, MG_ESC("正向有功需量"),m1->pos_active_dem,MG_ESC("反向有功需量"),m1->neg_active_dem,
     MG_ESC("A相电压"),m1->ua,MG_ESC("B相电压"),m1->ub,MG_ESC("C相电压"),m1->uc,MG_ESC("A相电流"),m1->ia,MG_ESC("B相电流"),m1->ib,MG_ESC("C相电流"),m1->ic,MG_ESC("频率"),m1->freq,MG_ESC("PT"),m1->PT,MG_ESC("CT"),m1->CT,
/*2*/MG_ESC("2#储能电表"),MG_ESC("通信状态"), MG_ESC(m2->szCommState),MG_ESC("数据更新时间"),MG_ESC(m2->szLastUpdate),MG_ESC("通信失败次数"),m2->CommFailTotalCnt,
     MG_ESC("组合有功功率"), m2->com_active_p, MG_ESC("正向有功总电能"),m2->pos_active_e, MG_ESC("反向有功总电能"),m2->neg_active_e, 
     MG_ESC("功率因数"),m2->pwr_factor, MG_ESC("正向有功需量"),m2->pos_active_dem,MG_ESC("反向有功需量"),m2->neg_active_dem,
     MG_ESC("A相电压"),m2->ua,MG_ESC("B相电压"),m2->ub,MG_ESC("C相电压"),m2->uc,MG_ESC("A相电流"),m2->ia,MG_ESC("B相电流"),m2->ib,MG_ESC("C相电流"),m2->ic,MG_ESC("频率"),m2->freq,MG_ESC("PT"),m2->PT,MG_ESC("CT"),m2->CT,     
/*3*/MG_ESC("1#变压器电表"),MG_ESC("通信状态"), MG_ESC(m3->szCommState),MG_ESC("数据更新时间"),MG_ESC(m3->szLastUpdate),MG_ESC("通信失败次数"),m3->CommFailTotalCnt,
     MG_ESC("组合有功功率"), m3->com_active_p, MG_ESC("正向有功总电能"),m3->pos_active_e, MG_ESC("反向有功总电能"),m3->neg_active_e, 
     MG_ESC("功率因数"),m3->pwr_factor, MG_ESC("正向有功需量"),m3->pos_active_dem,MG_ESC("反向有功需量"),m3->neg_active_dem,
     MG_ESC("A相电压"),m3->ua,MG_ESC("B相电压"),m3->ub,MG_ESC("C相电压"),m3->uc,MG_ESC("A相电流"),m3->ia,MG_ESC("B相电流"),m3->ib,MG_ESC("C相电流"),m3->ic,MG_ESC("频率"),m3->freq,MG_ESC("PT"),m3->PT,MG_ESC("CT"),m3->CT,     
/*4*/MG_ESC("2#变压器电表"),MG_ESC("通信状态"), MG_ESC(m4->szCommState),MG_ESC("数据更新时间"),MG_ESC(m4->szLastUpdate),MG_ESC("通信失败次数"),m4->CommFailTotalCnt,
     MG_ESC("组合有功功率"), m4->com_active_p, MG_ESC("正向有功总电能"),m4->pos_active_e, MG_ESC("反向有功总电能"),m4->neg_active_e, 
     MG_ESC("功率因数"),m4->pwr_factor, MG_ESC("正向有功需量"),m4->pos_active_dem,MG_ESC("反向有功需量"),m4->neg_active_dem,
     MG_ESC("A相电压"),m4->ua,MG_ESC("B相电压"),m4->ub,MG_ESC("C相电压"),m4->uc,MG_ESC("A相电流"),m4->ia,MG_ESC("B相电流"),m4->ib,MG_ESC("C相电流"),m4->ic,MG_ESC("频率"),m4->freq,MG_ESC("PT"),m4->PT,MG_ESC("CT"),m4->CT,     
/*5*/MG_ESC("1#光伏电表"),MG_ESC("通信状态"), MG_ESC(m5->szCommState),MG_ESC("数据更新时间"),MG_ESC(m5->szLastUpdate),MG_ESC("通信失败次数"),m5->CommFailTotalCnt,
     MG_ESC("组合有功功率"), m5->com_active_p, MG_ESC("正向有功总电能"),m5->pos_active_e, MG_ESC("反向有功总电能"),m5->neg_active_e, 
     MG_ESC("功率因数"),m5->pwr_factor, MG_ESC("正向有功需量"),m5->pos_active_dem,MG_ESC("反向有功需量"),m5->neg_active_dem,
     MG_ESC("A相电压"),m5->ua,MG_ESC("B相电压"),m5->ub,MG_ESC("C相电压"),m5->uc,MG_ESC("A相电流"),m5->ia,MG_ESC("B相电流"),m5->ib,MG_ESC("C相电流"),m5->ic,MG_ESC("频率"),m5->freq,MG_ESC("PT"),m5->PT,MG_ESC("CT"),m5->CT);
}

static void handle_firmware_upload(struct mg_connection *c,
                                   struct mg_http_message *hm) {
  char name[64], offset[20], total[20];
  struct mg_str data = hm->body;
  long ofs = -1, tot = -1;
  name[0] = offset[0] = '\0';
  mg_http_get_var(&hm->query, "name", name, sizeof(name));
  mg_http_get_var(&hm->query, "offset", offset, sizeof(offset));
  mg_http_get_var(&hm->query, "total", total, sizeof(total));
  MG_INFO(("File %s, offset %s, len %lu", name, offset, data.len));
  if ((ofs = mg_json_get_long(mg_str(offset), "$", -1)) < 0 ||
      (tot = mg_json_get_long(mg_str(total), "$", -1)) < 0) {
    mg_http_reply(c, 500, "", "offset and total not set\n");
  } else if (ofs == 0 && mg_ota_begin((size_t) tot) == false) {
    mg_http_reply(c, 500, "", "mg_ota_begin(%ld) failed\n", tot);
  } else if (data.len > 0 && mg_ota_write(data.ptr, data.len) == false) {
    mg_http_reply(c, 500, "", "mg_ota_write(%lu) @%ld failed\n", data.len, ofs);
    mg_ota_end();
  } else if (data.len == 0 && mg_ota_end() == false) {
    mg_http_reply(c, 500, "", "mg_ota_end() failed\n", tot);
  } else {
    mg_http_reply(c, 200, s_json_header, "true\n");
    if (data.len == 0) {
      // Successful mg_ota_end() called, schedule device reboot
      mg_timer_add(c->mgr, 500, 0, (void (*)(void *)) mg_device_reset, NULL);
    }
  }
}

static void handle_firmware_commit(struct mg_connection *c) {
  mg_http_reply(c, 200, s_json_header, "%s\n",
                mg_ota_commit() ? "true" : "false");
}

static void handle_firmware_rollback(struct mg_connection *c) {
  mg_http_reply(c, 200, s_json_header, "%s\n",
                mg_ota_rollback() ? "true" : "false");
}

static size_t print_status(void (*out)(char, void *), void *ptr, va_list *ap) {
  int fw = va_arg(*ap, int);
  return mg_xprintf(out, ptr, "{%m:%d,%m:%c%lx%c,%m:%u,%m:%u}\n",
                    MG_ESC("status"), mg_ota_status(fw), MG_ESC("crc32"), '"',
                    mg_ota_crc32(fw), '"', MG_ESC("size"), mg_ota_size(fw),
                    MG_ESC("timestamp"), mg_ota_timestamp(fw));
}

static void handle_firmware_status(struct mg_connection *c) {
  mg_http_reply(c, 200, s_json_header, "[%M,%M]\n", print_status,
                MG_FIRMWARE_CURRENT, print_status, MG_FIRMWARE_PREVIOUS);
}

static void handle_device_reset(struct mg_connection *c) {
  mg_http_reply(c, 200, s_json_header, "true\n");
  mg_timer_add(c->mgr, 500, 0, (void (*)(void *)) mg_device_reset, NULL);
}

static void handle_device_eraselast(struct mg_connection *c) {
  size_t ss = mg_flash_sector_size(), size = mg_flash_size();
  char *base = (char *) mg_flash_start(), *last = base + size - ss;
  if (mg_flash_bank() == 2) last -= size / 2;
  mg_flash_erase(last);
  mg_http_reply(c, 200, s_json_header, "true\n");
}

// HTTP request handler function
static void fn(struct mg_connection *c, int ev, void *ev_data) {
  if (ev == MG_EV_ACCEPT) {
    if (c->fn_data != NULL) {  // TLS listener!
      struct mg_tls_opts opts = {0};
      opts.cert = mg_unpacked("/certs/server_cert.pem");
      opts.key = mg_unpacked("/certs/server_key.pem");
      mg_tls_init(c, &opts);
    }
  } else if (ev == MG_EV_HTTP_MSG) {
    struct mg_http_message *hm = (struct mg_http_message *) ev_data;
    struct user *u = authenticate(hm);

    if ( 0 ) { // mg_http_match_uri(hm, "/api/#") && u == NULL
      mg_http_reply(c, 403, "", "Not Authorised\n");
    } else if (mg_http_match_uri(hm, "/api/login")) {
      handle_login(c, u);
    } else if (mg_http_match_uri(hm, "/api/logout")) {
      handle_logout(c);
    } else if (mg_http_match_uri(hm, "/api/debug")) {
      handle_debug(c, hm);
    } else if (mg_http_match_uri(hm, "/api/settings/get")) {
      handle_settings_get(c);
    } else if (mg_http_match_uri(hm, "/api/settings/set")) {
      handle_settings_set(c, hm->body);
    }else if (mg_http_match_uri(hm, "/api/stats/get")) {
      handle_stats_get(c);
    } else if (mg_http_match_uri(hm, "/api/events/get")) {
      handle_events_get(c, hm);
    } else if (mg_http_match_uri(hm, "/api/comm/get")) {
      handle_comm_get(c);
    } else if (mg_http_match_uri(hm, "/api/comm/set")) {
      handle_comm_set(c, hm->body);
    } else if (mg_http_match_uri(hm, "/api/meter/get")) {
      handle_meter_get(c);
    } else if (mg_http_match_uri(hm, "/api/ctl/get")) {
      handle_ctl_get(c);
    } else if (mg_http_match_uri(hm, "/api/firmware/upload")) {
      handle_firmware_upload(c, hm);
    } else if (mg_http_match_uri(hm, "/api/firmware/commit")) {
      handle_firmware_commit(c);
    } else if (mg_http_match_uri(hm, "/api/firmware/rollback")) {
      handle_firmware_rollback(c);
    } else if (mg_http_match_uri(hm, "/api/firmware/status")) {
      handle_firmware_status(c);
    } else if (mg_http_match_uri(hm, "/api/device/reset")) {
      handle_device_reset(c);
    } else if (mg_http_match_uri(hm, "/api/device/eraselast")) {
      handle_device_eraselast(c);
    } else {
      struct mg_http_serve_opts opts;
      memset(&opts, 0, sizeof(opts));
#if MG_ARCH == MG_ARCH_UNIX || MG_ARCH == MG_ARCH_WIN32
      opts.root_dir = "web_root";  // On workstations, use filesystem
#else
      opts.root_dir = "/web_root";  // On embedded, use packed files
      opts.fs = &mg_fs_packed;
#endif
      mg_http_serve_dir(c, ev_data, &opts);
    }
    MG_DEBUG(("%lu %.*s %.*s -> %.*s", c->id, (int) hm->method.len,
              hm->method.ptr, (int) hm->uri.len, hm->uri.ptr, (int) 3,
              &c->send.buf[9]));
  }
}

void web_init(struct mg_mgr *mgr) {
  s_settings.device_name = strdup("My Device");
  mg_http_listen(mgr, HTTP_URL, fn, NULL);
  mg_http_listen(mgr, HTTPS_URL, fn, (void *) 1);
  mg_timer_add(mgr, 3600 * 1000, MG_TIMER_RUN_NOW | MG_TIMER_REPEAT,
               timer_sntp_fn, mgr);
}
