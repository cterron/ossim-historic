/* Copyright (c) 2003 ossim.net
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *    from the author.
 *
 * 4. Products derived from this software may not be called "Os-sim" nor
 *    may "Os-sim" appear in their names without specific prior written
 *    permission from the author.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <config.h>
#include <time.h>

#include "sim-organizer.h"
#include "sim-host.h"
#include "sim-net.h"
#include "sim-signature.h"
#include "sim-message.h"
#include "sim-policy.h"
#include "sim-rule.h"
#include "sim-directive.h"
#include "sim-host-level.h"
#include "sim-net-level.h"

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimOrganizerPrivate {
  SimContainer  *container;
  SimDatabase   *database;
};

static gpointer parent_class = NULL;
static gint sim_container_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_organizer_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_organizer_impl_finalize (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_organizer_class_init (SimOrganizerClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_organizer_impl_dispose;
  object_class->finalize = sim_organizer_impl_finalize;
}

static void
sim_organizer_instance_init (SimOrganizer *organizer)
{
  organizer->_priv = g_new0 (SimOrganizerPrivate, 1);

  organizer->_priv->container = NULL;
  organizer->_priv->database = NULL;
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
SimOrganizer*
sim_organizer_new (SimContainer  *container,
		   SimDatabase  *database)
{
  SimOrganizer *organizer = NULL;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (database, NULL);
  g_return_val_if_fail (SIM_IS_DATABASE (database), NULL);

  organizer = SIM_ORGANIZER (g_object_new (SIM_TYPE_ORGANIZER, NULL));
  organizer->_priv->container = container;
  organizer->_priv->database = database;

  return organizer;
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
  
  g_return_if_fail (organizer != NULL);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (organizer->_priv->container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (organizer->_priv->container));

  while (TRUE) 
    {
      msg = SIM_MESSAGE (sim_container_pop_message (organizer->_priv->container));
      
      if (msg == NULL)
	{
	  continue;
	}

      if (msg->type == SIM_MESSAGE_TYPE_INVALID)
	{
	  g_object_unref (msg);
	  continue;
	}

      switch (msg->type)
	{
	case SIM_MESSAGE_TYPE_SNORT:
	  sim_organizer_calificate (organizer, msg);
	  sim_organizer_correlation (organizer, msg);
	  break;
	case SIM_MESSAGE_TYPE_LOGGER:
	  break;
	case SIM_MESSAGE_TYPE_RRD:
	  break;
	default:
	  break;
	}

      g_object_unref (msg);
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
  SimContainer    *container;
  SimDatabase     *database;
  SimHost         *host;
  SimNet          *net;
  SimSignature    *signature;
  SimPolicy       *policy;
  SimHostLevel    *host_level;
  SimNetLevel     *net_level;
  GList           *nets;

  GInetAddr       *src_ia;
  GInetAddr       *dst_ia;
  gint             dst_port;
  gint             priority;
  gint             plugin;
  gint             tplugin;
  SimProtocolType  protocol;
  SimPortProtocol *pp;

  gint             policy_priority = 1;

  gint             src_asset = 5;
  gint             dst_asset = 5;

  gint             impactC = 0;
  gint             impactA = 0;

  gint             date = 0;
  gint             i;
  struct tm       *loctime;
  time_t           curtime;

  g_return_if_fail (organizer != NULL);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (message != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (message));
  g_return_if_fail (organizer->_priv->container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (organizer->_priv->container));

  container = organizer->_priv->container;
  database = organizer->_priv->database;

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

  src_ia = sim_message_get_src_ia (message);
  dst_ia = sim_message_get_dst_ia (message);
  dst_port = sim_message_get_dst_port (message);
  plugin = sim_message_get_plugin (message);
  tplugin = sim_message_get_tplugin (message);
  priority = sim_message_get_priority (message);
  protocol = sim_message_get_protocol (message);

  switch (plugin)
    {
    case GENERATOR_SPP_SPADE:
      signature = sim_container_get_signature_group_by_id (container, SIM_SIGNATURE_GROUP_TYPE_SPADE);
      break;
    case GENERATOR_FW1:
      switch (tplugin)
	{
	case FW1_ACCEPT_TYPE:
	  signature = sim_container_get_signature_group_by_id (container, SIM_SIGNATURE_GROUP_TYPE_FW1_ACCEPT);
	  break;
	case FW1_REJECT_TYPE:
	  signature = sim_container_get_signature_group_by_id (container, SIM_SIGNATURE_GROUP_TYPE_FW1_REJECT);
	  break;
	case FW1_DROP_TYPE:
	  signature = sim_container_get_signature_group_by_id (container, SIM_SIGNATURE_GROUP_TYPE_FW1_DROP);
	  break;
	default:
	  break;
	}

      break;
    default:
      signature = sim_container_get_signature_group_by_sid (container, tplugin);
      break;
    }

  g_return_if_fail (signature);

  pp = sim_port_protocol_new (dst_port, protocol);

  policy = (SimPolicy *) 
    sim_container_get_policy_match (container,
				    date,
				    src_ia,
				    dst_ia,
				    pp,
				    sim_signature_get_name (signature));

  if (policy)
    policy_priority = sim_policy_get_priority (policy);

  /* Source Asset */
  host = (SimHost *) sim_container_get_host_by_ia (container, src_ia);
  if (host)
    src_asset = sim_host_get_asset (host);

  if (src_asset)
    impactC = policy_priority * priority * src_asset;
  
  /* Destination Asset */
  host = (SimHost *) sim_container_get_host_by_ia (container, dst_ia);
  if (host)
    dst_asset = sim_host_get_asset (host);

  if (dst_asset)
    impactA = policy_priority * priority * dst_asset;


  /* Updates Host Level C */
  host_level = sim_container_get_host_level_by_ia (container, src_ia);
  if (host_level)
    {
      sim_host_level_plus_c (host_level, impactC); /* Memory update */
      sim_container_db_update_host_level (container, database, host_level); /* DB update */
    }
  else
    {
      host_level = sim_host_level_new (src_ia, impactC, 1); /* Create new host*/
      sim_container_append_host_level (container, host_level); /* Memory addition */
      sim_container_db_insert_host_level (container, database, host_level); /* DB insert */
    }

  /* Update Net Levels C */
  nets = sim_container_get_nets_has_ia (container, src_ia);
  while (nets)
    {
      net = (SimNet *) nets->data;
      
      net_level = sim_container_get_net_level_by_name (container, sim_net_get_name (net));
      if (net_level)
	{
	  sim_net_level_plus_c (net_level, impactC); /* Memory update */
	  sim_container_db_update_net_level (container, database, net_level); /* DB update */
	}
      else
	{
	  net_level = sim_net_level_new (sim_net_get_name (net), impactC, 1);
	  sim_container_append_net_level (container, net_level); /* Memory addition */
	  sim_container_db_insert_net_level (container, database, net_level); /* DB insert */
	}

      nets = nets->next;
    }
  g_list_free (nets);

  /* Updates Host Level A */
  host_level = sim_container_get_host_level_by_ia (container, dst_ia);
  if (host_level)
    {
      sim_host_level_plus_a (host_level, impactA); /* Memory update */
      sim_container_db_update_host_level (container, database, host_level); /* DB update */
    }
  else
    {
      host_level = sim_host_level_new (dst_ia, 1, impactA); /* Create new host*/
      sim_container_append_host_level (container, host_level); /* Memory addition */
      sim_container_db_insert_host_level (container, database, host_level); /* DB insert */
    }

  /* Update Net Levels A */
  nets = sim_container_get_nets_has_ia (container, dst_ia);
  while (nets)
    {
      net = (SimNet *) nets->data;
      
      net_level = sim_container_get_net_level_by_name (container, sim_net_get_name (net));
      if (net_level)
	{
	  sim_net_level_plus_a (net_level, impactA); /* Memory update */
	  sim_container_db_update_net_level (container, database, net_level); /* DB update */
	}
      else
	{
	  net_level = sim_net_level_new (sim_net_get_name (net), 1, impactA);
	  sim_container_append_net_level (container, net_level); /* Memory addition */
	  sim_container_db_insert_net_level (container, database, net_level); /* DB insert */
	}
      
      nets = nets->next;
    }
  g_list_free (nets);

  /* Attack Responses */
  /* Updates Host Level C */
  if (sim_signature_get_id (signature) == SIM_SIGNATURE_GROUP_TYPE_ATTACK_RESPONSES)
    {
      host_level = sim_container_get_host_level_by_ia (container, dst_ia);
      if (host_level)
	{
	  sim_host_level_plus_c (host_level, impactC); /* Memory update */
	  sim_container_db_update_host_level (container, database, host_level); /* DB update */
	}
      else
	{
	  host_level = sim_host_level_new (dst_ia, impactC, 1); /* Create new host*/
	  sim_container_append_host_level (container, host_level); /* Memory addition */
	  sim_container_db_insert_host_level (container, database, host_level); /* DB insert */
	}

      /* Update Net Levels C */
      nets = sim_container_get_nets_has_ia (container, dst_ia);
      while (nets)
	{
	  net = (SimNet *) nets->data;
	  
	  net_level = sim_container_get_net_level_by_name (container, sim_net_get_name (net));
	  if (net_level)
	    {
	      sim_net_level_plus_c (net_level, impactC); /* Memory update */
	      sim_container_db_update_net_level (container, database, net_level); /* DB update */
	    }
	  else
	    {
	      net_level = sim_net_level_new (sim_net_get_name (net), impactC, 1);
	      sim_container_append_net_level (container, net_level); /* Memory addition */
	      sim_container_db_insert_net_level (container, database, net_level); /* DB insert */
	    }

	  nets = nets->next;
	}
    }
}

/*
 *
 *
 */
void
sim_organizer_correlation (SimOrganizer *organizer, 
			  SimMessage   *message)
{
  SimContainer  *container;
  SimDatabase   *database;
  GList         *directives;
  GList         *backlogs;

  g_return_if_fail (organizer != NULL);
  g_return_if_fail (SIM_IS_ORGANIZER (organizer));
  g_return_if_fail (message != NULL);
  g_return_if_fail (SIM_IS_MESSAGE (message));
  g_return_if_fail (organizer->_priv->container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (organizer->_priv->container));

  container = organizer->_priv->container;
  database = organizer->_priv->database;

  directives = sim_container_get_directives (container);
  backlogs = sim_container_get_backlogs (container);

  /* Match Directives */
  while (directives)
    {
      SimDirective *directive = (SimDirective *) directives->data;

      if (sim_directive_match_rule_root_by_message (directive, message))
	{
	  SimDirective *new_directive;
	  SimRule      *rule_root;
	  GNode        *node;
	  GNode        *children;

	  new_directive = sim_directive_clone (directive);
	  node = sim_directive_get_rule_root (new_directive);
	  rule_root = (SimRule *) node->data;
	  sim_rule_set_message_data (rule_root, message);

	  sim_container_db_insert_backlog (container, database, new_directive);

	  if (!G_NODE_IS_LEAF (node))
	    {
	      children = node->children;
	      while (children)
		{
		  sim_directive_set_rule_vars (new_directive, children);

		  children = children->next;
		}
	    }

	  sim_container_append_backlog (container, new_directive);
	}

      directives = directives->next;
    }

  /* Match Backlogs */
  while (backlogs)
    {
      SimDirective *directive = (SimDirective *) backlogs->data;

      if (sim_directive_match_rule_by_message (directive, message))
	{ 
	  sim_container_db_update_backlog (container, database, directive);

	  if (sim_directive_matched (directive))
	    {
	      g_message ("Directive Matched %s", sim_directive_get_name (directive));
	      sim_container_remove_backlog (container, directive);
	      g_object_unref (directive);
	    }
	}

      backlogs = backlogs->next;
    }

  g_list_free (directives);
  g_list_free (backlogs);
}
