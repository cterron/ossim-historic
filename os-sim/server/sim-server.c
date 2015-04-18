/**
 *
 *
 */

#include "sim-server.h"
#include "sim-config.h"
#include "sim-database.h"
#include "sim-syslog.h"
#include "sim-organizer.h"
#include "sim-message.h"

#include <libgda/libgda.h>
#include <pthread.h>
#include <config.h>

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimServerPrivate {
  pthread_attr_t   attr;

  SimConfig       *config;
  SimDatabase     *database;

  GMainLoop       *loop;

  GQueue          *messages;

  GList           *policies;
  GList           *hosts;
  GList           *nets;
  GList           *assets;
  GList           *net_assets;
};

static void sim_server_syslog (SimServer *server);
static void sim_server_organizer (SimServer *server);

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void
sim_server_class_init (SimServerClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_server_instance_init (SimServer * server)
{
  server->_priv = g_new0 (SimServerPrivate, 1);

  server->_priv->database = NULL;
  server->_priv->config = NULL;
  
  server->_priv->loop = NULL;
  
  server->_priv->messages = g_queue_new ();
  
  server->_priv->policies = NULL;
  server->_priv->hosts = NULL;
  server->_priv->nets = NULL;
  server->_priv->assets = NULL;
  server->_priv->net_assets = NULL;
}

/* Public Methods */

GType
sim_server_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimServerClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_server_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimServer),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_server_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimServer", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

SimServer*
sim_server_new (void)
{
  SimServer *server = NULL;
  gint err;

  server = SIM_SERVER (g_object_new (SIM_TYPE_SERVER, NULL));

  err = pthread_attr_init (&server->_priv->attr);
  g_return_val_if_fail (err == 0, NULL);
                                                                                                                             
  err = pthread_attr_setstacksize (&server->_priv->attr, STACK_SIZE);
  g_return_val_if_fail (err == 0, NULL);
                                                                                                                             
  err = pthread_attr_setscope (&server->_priv->attr, PTHREAD_SCOPE_SYSTEM);
  g_return_val_if_fail (err == 0, NULL);

  return server;
}

/**
 * sim_server_run
 *
 *
 */
void
sim_server_run (SimServer *server)
{
  pthread_t thr;
  gint      err;
 
  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_message ("sim_server_run thread: %ld", pthread_self ());

  /* Database init */

  server->_priv->database = sim_database_new ();

  /* Syslog Thread */

  err = pthread_create(&thr, &server->_priv->attr, (void *(*)(void *)) sim_server_syslog, server);
  g_return_if_fail (err == 0);

  err = pthread_detach(thr);
  g_return_if_fail (err == 0);

  /* Organizer Thread */

  err = pthread_create(&thr, &server->_priv->attr, (void *(*)(void *)) sim_server_organizer, server);
  g_return_if_fail (err == 0);

  err = pthread_detach(thr);
  g_return_if_fail (err == 0);

  /* Main Loop */

  server->_priv->loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (server->_priv->loop);
}

/*****************************************
 * sim_server_syslog:
 *
 *   arguments:
 *
 *   results:
 *****************************************/

static void
sim_server_syslog (SimServer *server)
{
  SimSyslog *syslog;
 
  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_message ("sim_server_syslog thread: %ld", pthread_self ());

  syslog = sim_syslog_new ();
  sim_syslog_set_server(syslog, server);
  sim_syslog_run (syslog);
}

/*****************************************
 * sim_server_organizer:
 *
 *   arguments:
 *
 *   results:
 *****************************************/

static void
sim_server_organizer (SimServer *server)
{
  SimOrganizer *organizer;
 
  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_message ("sim_server_organizer thread: %ld", pthread_self ());

  organizer = sim_organizer_new ();
  sim_organizer_set_server(organizer, server);
  sim_organizer_run (organizer);
}

/**
 *
 *
 *
 */
void
sim_server_push_tail_messages (SimServer *server, GObject *message)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;

  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));
  g_return_if_fail (message != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (message));

  g_static_mutex_lock (&mutex);
  g_queue_push_tail (server->_priv->messages, message);
  g_static_mutex_unlock (&mutex);
}

/**
 *
 *
 *
 */
GObject*
sim_server_pop_head_messages (SimServer *server)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  GObject *msg;

  g_return_val_if_fail (server != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SERVER (server), NULL);

  g_static_mutex_lock (&mutex);
  msg = g_queue_pop_head (server->_priv->messages);
  g_static_mutex_unlock (&mutex);

  return msg;
}
/*
 *
 *
 *
 */
gint
sim_server_get_messages_num (SimServer *server)
{
  static GStaticMutex mutex = G_STATIC_MUTEX_INIT;
  gint num;

  g_return_val_if_fail (server != NULL, 0);
  g_return_val_if_fail (SIM_IS_SERVER (server), 0);

  g_static_mutex_lock (&mutex);
  num = server->_priv->messages->length;
  g_static_mutex_unlock (&mutex);

  return num;
}
