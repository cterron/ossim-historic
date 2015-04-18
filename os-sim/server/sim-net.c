/**
 *
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>
#include <config.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "sim-database.h"
#include "sim-host.h"
#include "sim-net.h"

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimNetPrivate {
  gchar *name;
  glong a;
  glong c;
  GList *hosts;
};

static gpointer parent_class = NULL;
static gint sim_net_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void
sim_net_class_init (SimNetClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_net_instance_init (SimNet *net)
{
  net->_priv = g_new0 (SimNetPrivate, 1);

  net->_priv->name = NULL;
  net->_priv->c = 0;
  net->_priv->a = 0;
  net->_priv->hosts = NULL;
}

/* Public Methods */

GType
sim_net_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimNetClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_net_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimNet),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_net_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimNet", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimNet*
sim_net_new (gchar  *name,
	     gint    c,
	     gint    a)
{
  SimNet *net = NULL;

  g_return_val_if_fail (name != NULL, NULL);

  net = SIM_NET (g_object_new (SIM_TYPE_NET, NULL));
  net->_priv->name = name;
  net->_priv->c = c;
  net->_priv->a = a;

  return net;
}

/*
 *
 *
 *
 */
gchar*
sim_net_get_name (SimNet         *net)
{
  g_return_val_if_fail (net != NULL, NULL);
  g_return_val_if_fail (SIM_IS_NET (net), NULL);

  return net->_priv->name;
}

/*
 *
 *
 *
 */
void
sim_net_set_name (SimNet          *net,
		  gchar           *name)
{
  g_return_if_fail (net != NULL);
  g_return_if_fail (SIM_IS_NET (net));
  g_return_if_fail (name != NULL);

  net->_priv->name = name;
}

/*
 *
 *
 *
 */
gint
sim_net_get_c (SimNet  *net)
{
  g_return_val_if_fail (net != NULL, 0);
  g_return_val_if_fail (SIM_IS_NET (net), 0);

  return net->_priv->c;
}

/*
 *
 *
 *
 */
void
sim_net_set_c (SimNet  *net,
		gint      c)
{
  g_return_if_fail (net != NULL);
  g_return_if_fail (SIM_IS_NET (net));

  net->_priv->c = c;
}

/*
 *
 *
 *
 */
gint
sim_net_get_a (SimNet  *net)
{
  g_return_val_if_fail (net != NULL, 0);
  g_return_val_if_fail (SIM_IS_NET (net), 0);

  return net->_priv->a;
}

/*
 *
 *
 *
 */
void
sim_net_set_a (SimNet  *net,
	       gint      a)
{
  g_return_if_fail (net != NULL);
  g_return_if_fail (SIM_IS_NET (net));

  net->_priv->a = a;
}

/*
 *
 *
 *
 */
void
sim_net_add_host (SimNet          *net,
		  GObject         *host)
{
  g_return_if_fail (net != NULL);
  g_return_if_fail (SIM_IS_NET (net));
  g_return_if_fail (host != NULL);
  g_return_if_fail (SIM_IS_HOST (host));

  net->_priv->hosts = g_list_append (net->_priv->hosts, host);
}

/*
 *
 *
 *
 */
void
sim_net_add_host_ip (SimNet          *net,
		     gchar           *ip)
{
  g_return_if_fail (net != NULL);
  g_return_if_fail (SIM_IS_NET (net));

  net->_priv->hosts = g_list_append (net->_priv->hosts, ip);
}

/*
 *
 *
 *
 */
void
sim_net_remove_host (SimNet          *net,
		     GObject         *host)
{
  g_return_if_fail (net != NULL);
  g_return_if_fail (SIM_IS_NET (net));
  g_return_if_fail (host != NULL);
  g_return_if_fail (SIM_IS_HOST (host));

  net->_priv->hosts = g_list_remove (net->_priv->hosts, host);
}

/*
 *
 *
 *
 */
gboolean
sim_net_has_host (SimNet          *net,
		  GObject         *host)
{ 
  struct in_addr  hip;
  gchar          *ip;
  gint i;

  g_return_if_fail (net != NULL);
  g_return_if_fail (SIM_IS_NET (net));
  g_return_if_fail (host != NULL);
  g_return_if_fail (SIM_IS_HOST (host));

  for (i = 0; i < g_list_length (net->_priv->hosts); i++)
    {
      ip = (gchar *) g_list_nth_data (net->_priv->hosts, i);

      hip = sim_host_get_ip (SIM_HOST (host));

      if (!strcmp (inet_ntoa (hip), ip))
	{
	  return TRUE;
	}
    }

  return FALSE;
}

/*
 *
 *
 *
 */
GList*
sim_net_get_hosts (SimNet          *net)
{
  g_return_if_fail (net != NULL);
  g_return_if_fail (SIM_IS_NET (net));

  return net->_priv->hosts;
}

/*
 *
 *
 *
 */
void
sim_net_set_recovery (SimNet  *net,
		       gint      recovery)
{
  g_return_if_fail (net != NULL);
  g_return_if_fail (SIM_IS_NET (net));
  g_return_if_fail (recovery > 0);

  if (net->_priv->c > recovery)
    net->_priv->c -= recovery;
  else
    net->_priv->c = 0;

  if (net->_priv->a > recovery)
    net->_priv->a -= recovery;
  else
    net->_priv->a = 0;
}
