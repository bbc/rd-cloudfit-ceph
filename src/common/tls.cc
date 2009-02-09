#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "common/tls.h"

#include "config.h"

#undef dout
#undef derr
#define  dout(l)    if (l<=g_conf.debug_lockdep) *_dout << g_clock.now() << " " << pthread_self() << " tls: "
#define  derr(l)    if (l<=g_conf.debug_lockdep) *_derr << g_clock.now() << " " << pthread_self() << " tls: "


pthread_key_t   _tls_key = 0;

static int _initialized = 0;

static void _tls_destructor(void *value)
{
  free(value);
  pthread_setspecific(_tls_key, NULL);
}

void *tls_get_ptr()
{
  void *val;

  if (!_initialized) {
    derr(0) << "tls not initialized" << std::endl;
    return NULL;
  }

  val  = pthread_getspecific(_tls_key);

  if (!val) {
    int ret;
    val = malloc(sizeof(struct TlsData));
    ret = pthread_setspecific(_tls_key, val);

    if (ret)
      return NULL;
  }

  return val;
}

int tls_init()
{
  int ret;

  if (!_initialized) {
    ret = pthread_key_create(&_tls_key, _tls_destructor);
  }
 
  return ret; 
}

void tls_finalize()
{
  if (_initialized)
    pthread_key_delete(_tls_key);
}
