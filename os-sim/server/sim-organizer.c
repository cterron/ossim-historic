/**
 *
 *
 */


#include "sim-organizer.h"
#include "sim-server.h"
#include "sim-message.h"
#include "sim-signature.h"
#include "sim-policy.h"
#include "sim-host.h"
#include "sim-net.h"
#include "sim-host-asset.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <config.h>
#include <time.h>

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimOrganizerPrivate {
  SimServer   *server;

  gint         fd;
  GIOChannel  *io;
};

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void
sim_organizer_class_init (SimOrganizerClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_organizer_instance_init (SimOrganizer *organizer)
{
  organizer->_priv = g_new0 (SimOrganizerPrivate, 1);

  organizer->_priv->io = NULL;
}

/* Public Methods */

GType
sim_organizer_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimOrganizerClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_organizer_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimOrganizer),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_organizer_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimOrganizer", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimOrganizer *
sim_organizer_new (void)
{
  SimOrganizer *organizer = NULL;

  organizer = SIM_ORGANIZER (g_object_new (SIM_TYPE_ORGANIZER, NULL));

  return organizer;
}

/**
 *
 *
 *
 */
void
sim_organizer_set_server (SimOrganizer *organizer,
		       SimServer *server)
{
  g_return_if_fail (organizer != NULL);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (server != NULL);
  g_return_if_fail (SIM_IS_SERVER (server));

  organizer->_priv->server = server;
}

/*
 *
 *
 *
 */
void
sim_organizer_run (SimOrganizer *organizer)
{
  SimMessage *msg = NULL;

  GTimer *timer;
  gdouble sec;
  gulong  micro;

  g_return_if_fail (organizer != NULL);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (organizer->_priv->server != NULL);
  g_return_if_fail (SIM_IS_SERVER (organizer->_priv->server));

  timer = g_timer_new ();
  g_timer_start (timer);
  g_timer_stop (timer);
  sec = g_timer_elapsed (timer, &micro);

  while (TRUE) 
    {
      msg = SIM_MESSAGE (sim_server_pop_head_messages (organizer->_priv->server));
      
      if (msg == NULL)
	{
	  continue;
	}
      if (msg->type == SIM_MESSAGE_TYPE_INVALID)
	{
	  continue;
	}

      switch (msg->type)
	{
	case SIM_MESSAGE_TYPE_SNORT:
	  sim_organizer_calificate (organizer, msg);
	  break;
	case SIM_MESSAGE_TYPE_LOGGER:
	  break;
	case SIM_MESSAGE_TYPE_RRD:
	  break;
	default:
	  break;
	}
    }
}

/*
 *
 *
 */
void
sim_organizer_calificate (SimOrganizer *organizer, 
			  SimMessage   *message)
{
  SimServer     *server;
  SimPolicy     *policy;
  SimHost       *host;
  SimHostAsset  *host_asset;
  SimNet        *net;
  SimSignature  *signature;
  GList         *nets;
  gint           recovery_level = 1;
  gint           plugin;
  gint           tplugin;
  gint           priority;
  gchar         *source_ip;
  gchar         *destin_ip;
  gint           destin_port;
  SimProtocolType protocol;
  gint           policy_priority = 1;
  gint           source_asset = 5;
  gint           destin_asset = 5;
  gint           impactC = 0, impactA = 0;
  gint           date = 0;
  gint           i;
  struct in_addr source_in;
  struct in_addr destin_in;
  struct tm     *loctime;
  time_t         curtime;

  g_return_if_fail (organizer != NULL);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (message != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (message));
  g_return_if_fail (organizer->_priv->server != NULL);
  g_return_if_fail (SIM_IS_SERVER (organizer->_priv->server));

  server = organizer->_priv->server;

  /*
   * get current day and current hour
   * calculate date expresion to be able to compare dates
   *
   * for example, Fri 21h = ((5 - 1) * 7) + 21 = 49
   *              Sat 14h = ((6 - 1) * 7) + 14 = 56
   */
  curtime = time (NULL);
  loctime = localtime (&curtime);
  date = ((loctime->tm_wday - 1) * 7) + loctime->tm_hour;

  plugin = sim_message_get_plugin (message);
  tplugin = sim_message_get_tplugin (message);
  priority = sim_message_get_priority (message);
  protocol = sim_message_get_protocol (message);
  source_ip = sim_message_get_source_ip (message);
  destin_ip = sim_message_get_destination_ip (message);
  destin_port = sim_message_get_destination_port (message);

  switch (plugin)
    {
    case GENERATOR_SPP_SPADE:
      signature = (SimSignature *) 
	sim_server_get_sig_subgroup_from_type (server, SIM_SIGNATURE_SUBGROUP_TYPE_SPADE);
      break;
    case GENERATOR_FW1:
      switch (tplugin)
	{
	case FW1_ACCEPT_TYPE:
	  signature = (SimSignature *) 
	    sim_server_get_sig_subgroup_from_type (server, SIM_SIGNATURE_SUBGROUP_TYPE_FW1_ACCEPT);
	  break;
	case FW1_REJECT_TYPE:
	  signature = (SimSignature *) 
	    sim_server_get_sig_subgroup_from_type (server, SIM_SIGNATURE_SUBGROUP_TYPE_FW1_REJECT);
	  break;
	case FW1_DROP_TYPE:
	  signature = (SimSignature *) 
	    sim_server_get_sig_subgroup_from_type (server, SIM_SIGNATURE_SUBGROUP_TYPE_FW1_DROP);
	  break;
	default:
	  break;
	}

      break;
    default:
      signature = (SimSignature *) 
	sim_server_get_sig_subgroup_from_sid (server, tplugin);
      break;
    }

  g_return_if_fail (signature != NULL);

  policy = (SimPolicy *) 
    sim_server_get_policy_by_match (server,
				    date,
				    source_ip,
				    destin_ip,
				    destin_port,
				    protocol,
				    sim_signature_get_name (signature));

  if (policy)
    policy_priority = sim_policy_get_priority (policy);

  g_message ("Calculate: src_ip: %s dst_ip: %s, signature %s, policy_priority: %d",
	     source_ip, destin_ip, sim_signature_get_name (signature), policy_priority);

  inet_aton(source_ip, &source_in);
  inet_aton(destin_ip, &destin_in);

  /* Source Asset */
  host_asset = (SimHostAsset *) sim_server_get_host_asset_by_ip (server, source_in);
  if (host_asset)
      source_asset = sim_host_asset_get_asset (host_asset);

  if (source_asset)
    impactC = policy_priority * priority * source_asset;

  /* Destination Asset */
  host_asset = (SimHostAsset *) sim_server_get_host_asset_by_ip (server, destin_in);
  if (host_asset)
    destin_asset = sim_host_asset_get_asset (host_asset);
  
  if (destin_asset)
    impactA = policy_priority * priority * destin_asset;

  /* Updates C level */
  host = (SimHost *) sim_server_get_host_by_ip (server, source_in);
  if (host)
    {
      gint c = sim_host_get_c (host);

      sim_host_set_c (host, c + impactC); /* Memory update */
      sim_server_db_update_host (server, G_OBJECT (host)); /* DB update */

      /* Update nets */
      nets = sim_server_get_nets_by_host (server, G_OBJECT (host));
      for (i = 0; i < g_list_length(nets); i++)
	{
	  gint net_c;

	  net = (SimNet *) g_list_nth_data (nets, i);

	  net_c = sim_net_get_c (net);
	  sim_net_set_c (net, net_c + impactC); /* Memory update */
	  sim_server_db_update_net (server, G_OBJECT (net)); /* DB update */
	}
    }
  else
    {
      g_message ("Host New %s", source_ip);

      host = sim_host_new (); /* Create new host*/

      /* to improve this! */
      sim_host_set_ip (host, source_in);
      sim_host_set_c (host, impactC);
      sim_host_set_a (host, 1);
      
      sim_server_add_host (server, G_OBJECT (host)); /* Memory addition */
      sim_server_db_insert_host (server, G_OBJECT (host)); /* DB insert */

      /* Update nets */
      nets = sim_server_get_nets_by_host (server, G_OBJECT (host));
      for (i = 0; i < g_list_length(nets); i++)
	{
	  gint net_c;

	  net = (SimNet *) g_list_nth_data (nets, i);

	  net_c = sim_net_get_c (net);
	  sim_net_set_c (net, net_c + impactC); /* Memory update */
	  sim_server_db_update_net (server, G_OBJECT (net)); /* DB update */
	}
    }

  /* Updates A level */
  host = (SimHost *) sim_server_get_host_by_ip (server, destin_in);
  if (host)
    {
      gint a = sim_host_get_a (host);

      sim_host_set_a (host, a + impactA); /* Memory update */
      sim_server_db_update_host (server, G_OBJECT (host)); /* DB update */

      /* Update nets */
      nets = sim_server_get_nets_by_host (server, G_OBJECT (host));
      for (i = 0; i < g_list_length(nets); i++)
	{
	  gint net_a;

	  net = (SimNet *) g_list_nth_data (nets, i);

	  net_a = sim_net_get_a (net);
	  sim_net_set_a (net, net_a + impactA); /* Memory update */
	  sim_server_db_update_net (server, G_OBJECT (net)); /* DB update */
	}
    }
  else
    {
      host = sim_host_new (); /* Create new host*/

      /* to improve this! */
      sim_host_set_ip (host, destin_in);
      sim_host_set_c (host, 1);
      sim_host_set_a (host, impactA);

      sim_server_add_host (server, G_OBJECT (host)); /* Memory addition */
      sim_server_db_insert_host (server, G_OBJECT (host)); /* DB insert */

      /* Update nets */
      nets = sim_server_get_nets_by_host (server, G_OBJECT (host));
      for (i = 0; i < g_list_length(nets); i++)
	{
	  gint net_a;

	  net = (SimNet *) g_list_nth_data (nets, i);

	  net_a = sim_net_get_a (net);
	  sim_net_set_a (net, net_a + impactA); /* Memory update */
	  sim_server_db_update_net (server, G_OBJECT (net)); /* DB update */
	}
    }

  /* attack-responses */
  if (signature->subgroup == SIM_SIGNATURE_SUBGROUP_TYPE_ATTACK_RESPONSES)
    {
      host = (SimHost *) sim_server_get_host_by_ip (server, destin_in);
      if (host)
	{
	  gint c = sim_host_get_c (host);

	  sim_host_set_c (host, c + impactC); /* Memory update */
	  sim_server_db_update_host (server, G_OBJECT (host)); /* DB update */

	  /* Update nets */
	  nets = sim_server_get_nets_by_host (server, G_OBJECT (host));
	  for (i = 0; i < g_list_length(nets); i++)
	    {
	      gint net_c;

	      net = (SimNet *) g_list_nth_data (nets, i);

	      net_c = sim_net_get_c (net);
	      sim_net_set_c (net, sim_net_get_c (net) + impactC); /* Memory update */
	      sim_server_db_update_net (server, G_OBJECT (net)); /* DB update */
	    }
	}
    }
}
