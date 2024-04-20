// Copyright (c) 2020-2023 Cesanta Software Limited
// All rights reserved

#include <syslog.h>
#include "mongoose.h"
#include "net.h"
#include "appl.h"

static int s_sig_num;
static void signal_handler(int sig_num) {
  signal(sig_num, signal_handler);
  s_sig_num = sig_num;
}

static void mylog(char ch, void *param) {
  static char buf[256];
  static size_t len;
  buf[len++] = ch;
  if (ch == '\n' || len >= sizeof(buf)) {
    syslog(LOG_INFO, "%.*s", (int) len, buf); // Send logs
    len = 0;
  }
}

/*
 * System function daemon() replacement based on FreeBSD implementation.
 * Original source file CVS tag:
 * $FreeBSD: src/lib/libc/gen/daemon.c,v 1.3 2000/01/27 23:06:14 jasone Exp $
 */
static int become_daemon(nochdir, noclose)
int nochdir, noclose;
{
  int fd;
  switch (fork())
  {
  case -1:
    return (-1);
  case 0:
    break;
  default:
    _exit(0);
  }

  if (setsid() == -1)
    return (-1);

  if (!nochdir)
    (void)chdir("/");

  if (!noclose && (fd = open("/dev/null", O_RDWR, 0)) != -1)
  {
    (void)dup2(fd, STDIN_FILENO);
    (void)dup2(fd, STDOUT_FILENO);
    (void)dup2(fd, STDERR_FILENO);
    if (fd > 2)
      (void)close(fd);
  }
  return (0);
}

int main(void) {
  struct mg_mgr mgr;

  signal(SIGPIPE, SIG_IGN);
  signal(SIGINT, signal_handler);
  signal(SIGTERM, signal_handler);

	openlog(NULL, LOG_CONS | LOG_PID, 0);
	syslog(LOG_INFO, "%s ++ ", __func__);
  syslog(LOG_INFO, "Mac Size : char:%d short:%d int:%d int64_t:%d long:%d long long:%d float:%d double:%d", 
    sizeof(char), sizeof(short), sizeof(int), sizeof(int64_t), sizeof(long), sizeof(long long), sizeof(float), sizeof(double));

#if 1
  if( become_daemon(true, false) != 0){
    syslog(LOG_INFO, "%s, become_daemon Fail", __func__);
    return 0;
  }
#endif  

  //mg_log_set(MG_LL_DEBUG);  // Set debug log level
  mg_log_set(MG_LL_ERROR);  
  //mg_log_set_fn(mylog, NULL);
  mg_mgr_init(&mgr);

  appl_start();

  web_init(&mgr);
  while (1) { // s_sig_num == 0
    mg_mgr_poll(&mgr, 50);
  }

  mg_mgr_free(&mgr);
  MG_INFO(("Exiting on signal %d", s_sig_num));

  return 0;
}
