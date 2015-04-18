/**
 *
 *
 */

#include "sim-policy.h"
#include "sim-server.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <config.h>
 
#define BUFFER_SIZE 1024

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimPolicyPrivate {
  SimServer   *server;

  gint policy_id;
  gchar source_ips[MAX_IPS];
  gchar dest_ips[MAX_IPS];
  gint priority;
  gint begin_hour;
  gint end_hour;
  gint begin_day;
  gint end_day;
  gchar port_list[MAX_PORTS];
  gchar sigs[MAX_SIGS];
  gchar sensors[MAX_SENSORS];
  gchar desc[256];
};

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void
sim_policy_class_init (SimPolicyClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_policy_instance_init (SimPolicy *policy)
{
  policy->_priv = g_new0 (SimPolicyPrivate, 1);

}

/* Public Methods */

GType
sim_policy_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimPolicyClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_policy_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimPolicy),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_policy_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimPolicy", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimPolicy *
sim_policy_new (void)
{
  SimPolicy *policy = NULL;

  policy = SIM_POLICY (g_object_new (SIM_TYPE_POLICY, NULL));

  return policy;
}
