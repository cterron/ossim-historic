/**
 *
 *
 */

#include <unistd.h>
#include <config.h>

#include "sim-scheduler.h"
#include "sim-server.h"
#include "sim-config.h"

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimSchedulerPrivate {
  SimServer   *server;

  gint         timer;
};

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void
sim_scheduler_class_init (SimSchedulerClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_scheduler_instance_init (SimScheduler *scheduler)
{
  scheduler->_priv = g_new0 (SimSchedulerPrivate, 1);

  scheduler->_priv->server = NULL;
  scheduler->_priv->timer = 30;
}

/* Public Methods */

GType
sim_scheduler_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimSchedulerClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_scheduler_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimScheduler),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_scheduler_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimScheduler", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimScheduler *
sim_scheduler_new (void)
{
  SimScheduler *scheduler = NULL;

  scheduler = SIM_SCHEDULER (g_object_new (SIM_TYPE_SCHEDULER, NULL));

  return scheduler;
}

/**
 *
 *
 *
 */
void
sim_scheduler_set_server (SimScheduler *scheduler,
		       SimServer *server)
{
  g_return_if_fail (scheduler != NULL);
  g_return_if_fail (SIM_IS_SCHEDULER (scheduler));
  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));

  scheduler->_priv->server = server;
}

/*
 *
 *
 *
 */
void
sim_scheduler_run (SimScheduler *scheduler)
{
  SimConfig  *config;
  GList  *hosts;
  GList  *nets;
  gchar   *interval;
  gint    recovery;
  gint    i;

  g_return_if_fail (scheduler != NULL);
  g_return_if_fail (SIM_IS_SCHEDULER (scheduler));
  g_return_if_fail (scheduler->_priv->server != NULL);
  g_return_if_fail (SIM_IS_SERVER (scheduler->_priv->server));

  config = (SimConfig *) sim_server_get_config (scheduler->_priv->server);

  interval = sim_config_get_property_value (config,
					    SIM_CONFIG_PROPERTY_TYPE_UPDATE_INTERVAL);

  if (interval)
    scheduler->_priv->timer = strtol (interval, (char **)NULL, 10) * 15;

  while (TRUE)
  {
    sim_server_db_load_config (scheduler->_priv->server);
    recovery = sim_server_get_recovery (scheduler->_priv->server);

    sim_server_set_hosts_recovery (scheduler->_priv->server, recovery);
    sim_server_set_nets_recovery (scheduler->_priv->server, recovery);

    sleep (scheduler->_priv->timer);
  }
}
