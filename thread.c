#include "thread.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <valgrind/valgrind.h>
#include <signal.h>

static SIMPLEQ_HEAD(, my_thread) sleepq = SIMPLEQ_HEAD_INITIALIZER(sleepq);
static SIMPLEQ_HEAD(, my_thread) deadq = SIMPLEQ_HEAD_INITIALIZER(deadq);
static SIMPLEQ_HEAD(, my_thread) waiting_mutex = SIMPLEQ_HEAD_INITIALIZER(waiting_mutex);

my_thread *current_thread;
my_thread *main_thread;
/* un contexte qui va jouer le role de l'ordonnanceur
en organisant la transition entre les threads*/

// variables for preemption
#ifdef PREEMPTION
timer_t timerid;
struct itimerspec value;
struct sigevent sev;
#endif


struct sigaction new_action;

/* fonction permettant de verifier si le thread a recu un signal et executer le traitement necessaire*/
void handle_signal(struct my_thread *thread)
{
  int i;
  for (i = 0; i < NB_SIGNAUX; i++)
  {
    if (((thread->signal >> i) & 1) == 1)
    {
      signals[i].sa_handler(i);
    }
  }
  thread->signal = 0;
}

/* fonction permettant de remettre a 0 les flags signaux d'un thread */
void reset_signal(struct my_thread *thread)
{
  thread->signal = 0;
}

/*fonction speciale associée au contexte schedule */

void *scheduler(void)
{
  my_thread *t;
  t = thread_self();
  /* désallouer la pile du contexte courant si le thread a terminé*/
  if ((t->state == DEAD) && (t != main_thread))
  {
    SIMPLEQ_INSERT_TAIL(&deadq, t, next);
  }

  /* passer la main au premier thread dans la liste d'attente si ce n'est pas vide*/
  if (!SIMPLEQ_EMPTY(&sleepq))
  {
    current_thread = SIMPLEQ_FIRST(&sleepq);
    SIMPLEQ_REMOVE_HEAD(&sleepq, next);
    current_thread->uc.uc_link = NULL;
    current_thread->state = RUNNING;
    handle_signal(current_thread); // NOTE : pas utile ici ?
#ifdef PREEMPTION
    current_thread->execution_t = clock();
#endif
    swapcontext(&t->uc, &current_thread->uc);
  }

  else
  {
    setcontext(&main_thread->uc);
  }
}

void handler_sigusr2()
{
  if ((clock() - current_thread->execution_t) / CLOCKS_PER_SEC > 1)
  {
    thread_yield();
  }
}

__attribute__((constructor)) static void constr()
{
  /*initialisation des gestionnaires des signaux*/

  signals[SIG1].sa_handler = handler;
  signals[SIG2].sa_handler = handler;
  signals[SIG3].sa_handler = handler;

  SIMPLEQ_INIT(&sleepq);
  // recuperer le contexte du thread principal main
  main_thread = malloc(sizeof(my_thread));
  main_thread->state = RUNNING;
  reset_signal(main_thread);
  getcontext(&main_thread->uc);
  main_thread->uc.uc_link = NULL;
  current_thread = main_thread;

// Create timer
#ifdef PREEMPTION
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGUSR2;
  signal(SIGUSR2, handler_sigusr2);
  sev.sigev_value.sival_ptr = &timerid;
  if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1)
  {
    perror("timer_create");
    exit(EXIT_FAILURE);
  }

  //initialize timer
  value.it_value.tv_sec = 1;
  value.it_value.tv_nsec = 0;
  value.it_interval.tv_sec = 1;
  value.it_interval.tv_nsec = 0;
  if (timer_settime(timerid, 0, &value, NULL) == -1)
  {
    perror("timer_settime");
    exit(EXIT_FAILURE);
  }
#endif
}
/* recuperer l'identifiant du thread courant.
 */
thread_t thread_self(void)
{
  return current_thread;
}

/*utiliséé pour initialiser le contexte à la place de la fonction utilisateur.
elle permet de prendre en consideration le cas ou l'utilisateur utilise retourne
pour terminer la thread.
Elle prend en argument la fonction utilisateur et son argument*/
void *thread_fun(void *(*user_fn)(void *), void *arg)
{
  // sigaction(SIGALRM, &new_action, NULL);
  void *ret = (*user_fn)(arg);
  thread_exit(ret);
}

/* creer un nouveau thread qui va exécuter la fonction func avec l'argument funcarg.
 * renvoie 0 en cas de succès, -1 en cas d'erreur.
 */

int thread_create(thread_t *newthread, void *(*func)(void *), void *funcarg)
{

  my_thread *new_thread = malloc(sizeof(my_thread));
  my_thread *t;

  if (new_thread != NULL)
  {
    getcontext(&new_thread->uc);
    new_thread->uc.uc_stack.ss_size = 64 * 1024; /*la pile utilisée par ce contexte*/
    new_thread->uc.uc_stack.ss_sp = malloc(new_thread->uc.uc_stack.ss_size);
    new_thread->ret = NULL;
    new_thread->join = NULL;
    reset_signal(new_thread);
    new_thread->valgrind_stackid = VALGRIND_STACK_REGISTER(new_thread->uc.uc_stack.ss_sp, new_thread->uc.uc_stack.ss_sp + new_thread->uc.uc_stack.ss_size);
    makecontext(&new_thread->uc, (void (*)(void))thread_fun, 2, (void (*)(void))func, funcarg); /*modifie le contexte pointé par uc*/
    *newthread = (thread_t)new_thread;

    t = current_thread;
    t->state = WAITING;
    t->uc.uc_link = NULL;
    SIMPLEQ_INSERT_TAIL(&sleepq, t, next);

    new_thread->state = RUNNING;
    new_thread->uc.uc_link = NULL;
#ifdef PREEMPTION
    new_thread->execution_t = clock();
#endif
    current_thread = new_thread;
    swapcontext(&t->uc, &current_thread->uc);
    return 0;
  }
  return -1;
}

/* passer la main à un autre thread. */
int thread_yield(void)
{

  my_thread *t = thread_self();
  if (t != NULL)
  {
    handle_signal(t);
    t->state = WAITING;
    t->uc.uc_link = NULL;
    SIMPLEQ_INSERT_TAIL(&sleepq, t, next);
    scheduler();
    return 0;
  }

  return -1;
}

/* attendre la fin d'exécution d'un thread.
 * la valeur renvoyée par le thread est placée dans *retval.
 * si retval est NULL, la valeur de retour est ignorée.
 */
int thread_join(thread_t thread, void **retval)
{

  if (thread != NULL)
  {

    my_thread *t = NULL;
    t = (my_thread *)thread;

    /* tant que le thread en question n'a pas terminé on passe la main au  premire thread en attente
    jusqu'à trouver ce thread */
    if (t->state != DEAD)
    {
      t->join = thread_self();
      t->state = WAITING;
      scheduler();
    }

    if (retval != NULL)
    {
      *retval = t->ret;
    }
    return 0;
  }
  return -1;
}

/* terminer le thread courant en renvoyant la valeur de retour retval.
 * cette fonction ne retourne jamais.
 *
 * L'attribut noreturn aide le compilateur à optimiser le code de
 * l'application (élimination de code mort). Attention à ne pas mettre
 * cet attribut dans votre interface tant que votre thread_exit()
 * n'est pas correctement implémenté (il ne doit jamais retourner).
 */

void thread_exit(void *retval)
{
  my_thread *t = NULL;
  t = thread_self();
  if (t != NULL)
  {
    t->state = DEAD;
    t->ret = retval;

    if (t->join != NULL)
    {
      my_thread *join = NULL;
      join = t->join;
      SIMPLEQ_INSERT_TAIL(&sleepq, join, next);
    }
    if (t == main_thread)
    {
      scheduler();
      sigaction(SIGALRM, &new_action, NULL);
      exit(EXIT_SUCCESS);
    }
    scheduler();
  }
  sigaction(SIGALRM, &new_action, NULL);
  exit(EXIT_SUCCESS);
}

/* fonction d'initialisation d'un mutex */
int thread_mutex_init(thread_mutex_t *mutex)
{
  if (mutex == NULL)
  {
    return 1;
  }
  mutex->dummy = 0;
  return 0;
}

/* fonction de destruction d'un mutex */
int thread_mutex_destroy(thread_mutex_t *mutex)
{
  return 0;
}

/* fonction de verrouillage d'un mutex */
int thread_mutex_lock(thread_mutex_t *mutex)
{
  while (1)
  {
    if (mutex->dummy == 0)
    {
      mutex->dummy = 1;
      return 0;
    }
    else
    {
      //thread_yield();
      my_thread *t = NULL;
      t = thread_self();
      t->state = WAITING;
      SIMPLEQ_INSERT_TAIL(&waiting_mutex, t, next);
      scheduler();
    }
  }
}

/* fonction de deverrouillage d'un mutex */
int thread_mutex_unlock(thread_mutex_t *mutex)
{
  mutex->dummy = 0;
  my_thread *t = NULL;
  if (!SIMPLEQ_EMPTY(&waiting_mutex))
  {
    t = SIMPLEQ_FIRST(&waiting_mutex);
    SIMPLEQ_REMOVE_HEAD(&waiting_mutex, next);
    SIMPLEQ_INSERT_TAIL(&sleepq, t, next);
    return 0;
  }
}

/* Fonctions de gestion des signaux*/
/*exemple de handler de signale 1*/
void handler(int sig)
{
  printf("Je receptionne le signal %d\n", sig);
}

/*permet d'envoyer un signal à un thread donné*/
int send_signal_thread(int sig, thread_t t)
{
  if (t != NULL)
  {
    my_thread *th = NULL;
    th = (my_thread *)t;
    th->signal = th->signal | (1 << sig);
    return 1;
  }
  return 0;
}

/* permet de changer la gestionnaire d'un signal*/
void signal_thread(int sig, void (*handler)(int))
{
  signals[sig].sa_handler = handler;
}

__attribute__((destructor)) static void destr()
{

  my_thread *t;
  while (!SIMPLEQ_EMPTY(&deadq))
  {
    t = SIMPLEQ_FIRST(&deadq);
    SIMPLEQ_REMOVE_HEAD(&deadq, next);
    VALGRIND_STACK_DEREGISTER(t->valgrind_stackid);
    free(t->uc.uc_stack.ss_sp);
    free(t);
  }
  free(main_thread);
#ifdef PREEMPTION
  timer_delete(timerid);
#endif
};
