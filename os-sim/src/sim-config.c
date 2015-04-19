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

#include <gnet.h>
#include <stdlib.h>

#include "sim-config.h"
#include "os-sim.h"
#include <config.h>
#include "sim-event.h"

extern SimMain  ossim; //needed to be able to access to ossim.dbossim directly in sim_config_set_data_role()

enum
{
  DESTROY,
  LAST_SIGNAL
};

static gpointer parent_class = NULL;
static gint sim_config_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_config_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_config_impl_finalize (GObject  *gobject)
{
  SimConfig  *config = (SimConfig *) gobject;
  GList      *list;

  list = config->datasources;
  while (list)
    {
      SimConfigDS *ds = (SimConfigDS *) list->data;
      sim_config_ds_free (ds);
      list = list->next;
    }

  list = config->notifies;
  while (list)
    {
      SimConfigNotify *notify = (SimConfigNotify *) list->data;
      sim_config_notify_free (notify);
      list = list->next;
    }

  if (config->log.filename)
    g_free (config->log.filename);

  if (config->directive.filename)
    g_free (config->directive.filename);

	if (config->server.name)
		g_free (config->server.name);

	if (config->server.ip)
		g_free (config->server.ip);

	if (config->server.role)
		g_free (config->server.role);
	
  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_config_class_init (SimConfigClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_config_impl_dispose;
  object_class->finalize = sim_config_impl_finalize;
}

static void
sim_config_instance_init (SimConfig *config)
{
  /*config->sensor.name = NULL;
  config->sensor.ip = NULL;
  config->sensor.interface = NULL;*/

  config->log.filename = NULL;

  config->datasources = NULL;
  config->notifies = NULL;
  config->rservers = NULL;

  config->notify_prog = NULL;

  config->directive.filename = NULL;

  config->scheduler.interval = 0;

  config->server.port = 0;
  config->server.name = NULL;
  config->server.ip = NULL;
	config->server.role = g_new0 (SimRole, 1);

  config->smtp.host = NULL;
  config->smtp.port = 0;

  config->framework.name = NULL;
  config->framework.host = NULL;
  config->framework.port = 0;

}

/* Public Methods */

GType
sim_config_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimConfigClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_config_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimConfig),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_config_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimConfig", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 */
SimConfig*
sim_config_new (void)
{
  SimConfig *config = NULL;

  config = SIM_CONFIG (g_object_new (SIM_TYPE_CONFIG, NULL));

  return config;
}

/*
 *
 *
 *
 */
SimConfigDS*
sim_config_ds_new (void)
{
  SimConfigDS *ds;

  ds = g_new0 (SimConfigDS, 1);
  ds->name = NULL;
  ds->provider = NULL;
  ds->dsn = NULL;

  return ds;
}

/*
 *
 *
 *
 */
void
sim_config_ds_free (SimConfigDS *ds)
{
  g_return_if_fail (ds);

  if (ds->name)
    g_free (ds->name);
  if (ds->provider)
    g_free (ds->provider);
  if (ds->dsn)
    g_free (ds->dsn);

  g_free (ds);
}

/*
 * This function doesn't returns anything, it stores directly the data into config parameter.
 */
void
sim_config_set_data_role (SimConfig		*config,
													SimCommand	*cmd)
{
	g_return_if_fail (config);
	g_return_if_fail (SIM_IS_CONFIG (config));
	g_return_if_fail (cmd);
	g_return_if_fail (SIM_IS_COMMAND (cmd));
	
	config->server.role->store = cmd->data.server_set_data_role.store;
	config->server.role->cross_correlate = cmd->data.server_set_data_role.cross_correlate;
	config->server.role->correlate = cmd->data.server_set_data_role.correlate;
	config->server.role->qualify = cmd->data.server_set_data_role.qualify;
	config->server.role->resend_event = cmd->data.server_set_data_role.resend_event;	
	config->server.role->resend_alarm = cmd->data.server_set_data_role.resend_alarm;	

	//Also store in DB the configuration
	gchar *store = g_strdup_printf ("%d", config->server.role->store);
	gchar *correlate = g_strdup_printf ("%d", config->server.role->correlate);
	gchar *cross_correlate = g_strdup_printf ("%d", config->server.role->cross_correlate);
	gchar *qualify = g_strdup_printf ("%d", config->server.role->qualify);
	gchar *resend_event = g_strdup_printf ("%d", config->server.role->resend_event);
	gchar *resend_alarm = g_strdup_printf ("%d", config->server.role->resend_alarm);
	
	gchar *query;
	query = g_strdup_printf ("UPDATE config SET value='%s' WHERE conf='server_store'", store);
  sim_database_execute_no_query (ossim.dbossim, query);
	g_free (query);

	query = g_strdup_printf ("UPDATE config SET value='%s' WHERE conf='server_correlate'", correlate);
  sim_database_execute_no_query (ossim.dbossim, query);
	g_free (query);
	
	query = g_strdup_printf ("UPDATE config SET value='%s' WHERE conf='server_cross_correlate'", cross_correlate);
  sim_database_execute_no_query (ossim.dbossim, query);
	g_free (query);
	
	query = g_strdup_printf ("UPDATE config SET value='%s' WHERE conf='server_qualify'", qualify);
  sim_database_execute_no_query (ossim.dbossim, query);
	g_free (query);
	
	query = g_strdup_printf ("UPDATE config SET value='%s' WHERE conf='server_resend_alarm'", resend_alarm);
  sim_database_execute_no_query (ossim.dbossim, query);
	g_free (query);

	query = g_strdup_printf ("UPDATE config SET value='%s' WHERE conf='server_resend_event'", resend_event);
  sim_database_execute_no_query (ossim.dbossim, query);
	g_free (query);
	
	g_free (store);
	g_free (correlate);
	g_free (cross_correlate);
	g_free (qualify);
	g_free (resend_alarm);
	g_free (resend_event);
}

/*
 *
 *
 *
 */
SimConfigDS*
sim_config_get_ds_by_name (SimConfig    *config,
			   const gchar  *name)
{
  GList  *list;

  g_return_val_if_fail (config, NULL);
  g_return_val_if_fail (SIM_IS_CONFIG (config), NULL);
  g_return_val_if_fail (name, NULL);

  list = config->datasources;
  while (list)
    {
      SimConfigDS *ds = (SimConfigDS *) list->data;

      if (!g_ascii_strcasecmp (ds->name, name))
	return ds;

      list = list->next;
    }

  return NULL;
}

/*
 *
 *
 *
 */
SimConfigNotify*
sim_config_notify_new (void)
{
  SimConfigNotify *notify;

  notify = g_new0 (SimConfigNotify, 1);
  notify->emails = NULL;
  notify->alarm_risks = NULL;

  return notify;
}

/*
 *
 *
 *
 */
void
sim_config_notify_free (SimConfigNotify *notify)
{
  GList *list;

  g_return_if_fail (notify);

  if (notify->emails)
    g_free (notify->emails);

  list = notify->alarm_risks;
  while (list)
    {
      gint *level = (gint *) list->data;
      g_free (level);
      list = list->next;
    }

  g_free (notify);
}

/*
 *
 *
 *
 */
SimConfigRServer*
sim_config_rserver_new (void)
{
  SimConfigRServer *rserver;

  rserver = g_new0 (SimConfigRServer, 1);
  rserver->name = NULL;
  rserver->ip = NULL;
  rserver->ia = NULL;
  rserver->port = 0;
	rserver->socket = NULL;
  rserver->iochannel = NULL;
  rserver->HA_role = HA_ROLE_NONE;
  rserver->is_HA_server = FALSE;

  return rserver;
}

/*
 *
 *
 *
 */
void
sim_config_rserver_free (SimConfigRServer *rserver)
{
  g_return_if_fail (rserver);

  if (rserver->name)
    g_free (rserver->name);
  if (rserver->ip)
    g_free (rserver->ip);
  if (rserver->ia)
    gnet_inetaddr_unref (rserver->ia);
	
  g_free (rserver);
}

// vim: set tabstop=2:

