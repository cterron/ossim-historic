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

#include "sim-config.h"

enum
{
  DESTROY,
  LAST_SIGNAL
};

static gpointer parent_class = NULL;
static gint sim_config_signals[LAST_SIGNAL] = { 0 };

static void sim_config_scan (SimConfig    *config,
			     const gchar *filename);

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

      sim_config_ds_new (ds);

      list = list->next;
    }


  if (config->log.filename)
    g_free (config->log.filename);

  if (config->directive.filename)
    g_free (config->directive.filename);

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
  config->log.filename = NULL;

  config->datasources = NULL;

  config->directive.filename = NULL;

  config->scheduler.interval = 0;

  config->server.port = 0;
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
sim_config_new ()
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
sim_config_ds_new ()
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
