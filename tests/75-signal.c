#include "../thread.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

/* fonction thread pour envoyer SIG_USR1 a lui-meme */
static void * thfunc(void *dummy __attribute__((unused)))
{
  sleep(1);
  thread_yield();
  enum signal sig = SIG_USR1;
  send_signal_thread(sig, thread_self());
  thread_yield();
  sleep(1);
  return NULL;
}

/* fonction thread pour envoyer SIG_KILL au thread passe en parametre */
static void * signalToThread(void *t)
{ thread_t* thread = (thread_t *) t;
  sleep(1);
  thread_yield();
  enum signal sig = SIG_KILL;
  send_signal_thread(sig, thread);
  thread_yield();
  sleep(1);
  return NULL;
}

/* fonction thread ne faisant rien */
static void * doNothing(void *dummy __attribute__((unused))) // meme chose que thfunc sans l'envoi de signal
{
  sleep(1);
  thread_yield();
  thread_yield();
  sleep(1);
  return NULL;
}

/* fonction thread pour envoyer SIG_KILL puis SIG_USR1 au thread en parametre */
static void * signalsToThread(void *t)
{ thread_t* thread = (thread_t *) t;
  sleep(1);
  thread_yield();
  enum signal sig = SIG_KILL;
  send_signal_thread(sig, thread);
  sig = SIG_USR1;
  send_signal_thread(sig, thread);
  thread_yield();
  sleep(1);
  return NULL;
}

/* fonction thread pour tester le gestionnaire de signaux */
static void * threadfunc(void * arg)
{
    thread_yield();
    thread_exit(arg);
}

/* nouvelle fonction a executer sur le signal sig */
void handlerSig2(int sig){
    printf("je receptionne le signal %d à travers le nouveau gestionnaire \n",sig);
  }

int main()
{
  thread_t th1, th2;
  int err;

  // Test 1
  printf("Test 1 : envoi d'un signal 1 à lui même, puis envoi du signal 2 d'un autre thread vers celui-ci\n");
  err = thread_create(&th1, thfunc, NULL);
  assert(!err);
  err = thread_create(&th2, signalToThread, th1);
  assert(!err);
  err = thread_join(th2,NULL);
  assert(!err);
  err = thread_join(th1, NULL);
  assert(!err);

  // Test 2
  printf("Test 2 : envoi de 2 signaux à un thread avant un yield (gère plusieurs signaux en même temps)\n");
  err = thread_create(&th1, doNothing, NULL);
  assert(!err);
  err = thread_create(&th2, signalsToThread, th1);
  assert(!err);
  err = thread_join(th2,NULL);
  assert(!err);
  err = thread_join(th1, NULL);
  assert(!err);

  // Test 3
  printf("Test 3 : test du gestionnaire de signaux\n");
  err = thread_create(&th1, threadfunc, "thread1");
  assert(!err);
  send_signal_thread(SIG1,th1);
  err = thread_create(&th2, threadfunc, "thread2");
  assert(!err);
  signal_thread(SIG1,handlerSig2);
  send_signal_thread(SIG1,th2);

  err = thread_join(th2, NULL);
  assert(!err);
  err = thread_join(th1, NULL);
  assert(!err);

  return 0;
}
