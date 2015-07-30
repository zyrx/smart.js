#include "smartjs.h"
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

size_t sj_get_free_heap_size() {
  /* TODO(alashkin): What kind of free memory we want to see? */
  return sysconf(_SC_AVPHYS_PAGES) * sysconf(_SC_PAGESIZE);
}

void sj_wdt_feed() {
  /* Currently do nothing. For compatibility only. */
}

void sj_system_restart() {
  /* TODO(alashkin): Do we really want to restart OS here? */
}

size_t sj_get_fs_memory_usage() {
  /* TODO(alashkin): What kind of fs memory we want to see? */
  return 0;
}

void sj_usleep(int usecs) {
  usleep(usecs);
}

void posix_timer_callback(int sig, siginfo_t *si, void *uc) {
  sj_call_function(v7, si->si_value.sival_ptr);
}

void sj_set_timeout(int msecs, v7_val_t *cb) {
  struct sigevent te;
  struct itimerspec its;
  struct sigaction sa;
  timer_t timer_id;

  sa.sa_flags = SA_SIGINFO;
  sa.sa_sigaction = posix_timer_callback;
  sigemptyset(&sa.sa_mask);

  sigaction(SIGRTMIN, &sa, NULL);

  te.sigev_notify = SIGEV_SIGNAL;
  te.sigev_signo = SIGRTMIN;
  te.sigev_value.sival_ptr = cb;

  timer_create(CLOCK_REALTIME, &te, &timer_id);

  its.it_interval.tv_sec = 0;
  its.it_interval.tv_nsec = 0;
  its.it_value.tv_sec = msecs / 1000;
  its.it_value.tv_nsec = (msecs % 1000) * 1000000;

  timer_settime(timer_id, 0, &its, NULL);
}