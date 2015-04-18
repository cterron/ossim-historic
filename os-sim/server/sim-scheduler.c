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

#include <unistd.h>
#include <config.h>

#include "sim-scheduler.h"
#include "sim-container.h"
#include "sim-database.h"
#include "sim-config.h"
#include "sim-directive.h"

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct SimSchedulerTask {
  gint     id;
  gchar   *name;
  gint     timer;
};

struct _SimSchedulerPrivate {
  SimContainer   *container;
  SimDatabase    *database;
  SimConfig      *config;

  gint            timer;

  GList          *tasks;
};

static gpointer parent_class = NULL;
static gint sim_container_signals[LAST_SIGNAL] = { 0 };

static GTime       last = 0;
static GTime       timer = 0;


/* GType Functions */

static void 
sim_scheduler_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_scheduler_impl_finalize (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_scheduler_class_init (SimSchedulerClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_scheduler_impl_dispose;
  object_class->finalize = sim_scheduler_impl_finalize;
}

static void
sim_scheduler_instance_init (SimScheduler *scheduler)
{
  scheduler->_priv = g_new0 (SimSchedulerPrivate, 1);

  scheduler->_priv->container = NULL;
  scheduler->_priv->database = NULL;

  scheduler->_priv->timer = 30;

  scheduler->_priv->tasks = NULL;
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
SimScheduler*
sim_scheduler_new (SimContainer *container,
		   SimDatabase  *database,
		   SimConfig    *config)
{
  SimScheduler *scheduler = NULL;

  g_return_val_if_fail (container, NULL);
  g_return_val_if_fail (SIM_IS_CONTAINER (container), NULL);
  g_return_val_if_fail (database, NULL);
  g_return_val_if_fail (SIM_IS_DATABASE (database), NULL);

  scheduler = SIM_SCHEDULER (g_object_new (SIM_TYPE_SCHEDULER, NULL));
  scheduler->_priv->container = container;
  scheduler->_priv->database = database;
  scheduler->_priv->config = config;

  return scheduler;
}

/*
 *
 *
 *
 */
void
sim_scheduler_task_calculate (SimScheduler  *scheduler,
			      gpointer       data)
{
  SimContainer  *container;
  SimDatabase   *database;
  SimConfig     *config;
  gchar         *interval;
  gint           recovery;
  GTimeVal       curr_time;

  g_return_if_fail (scheduler != NULL);
  g_return_if_fail (SIM_IS_SCHEDULER (scheduler));

  container = scheduler->_priv->container;
  database = scheduler->_priv->database;
  config = scheduler->_priv->config;

  g_get_current_time (&curr_time);

  if (curr_time.tv_sec < (last + timer))
    return;

  last = curr_time.tv_sec;

  interval = sim_config_get_property_value (config, SIM_CONFIG_PROPERTY_TYPE_UPDATE_INTERVAL);

  if (interval)
    timer = strtol (interval, (char **)NULL, 10) * 15;
  else
    timer = 30;

  recovery = sim_container_db_get_recovery (container, database);
  sim_container_set_host_levels_recovery (container, database, recovery);
  sim_container_set_net_levels_recovery (container, database, recovery);
}

/*
 *
 *
 *
 */
void
sim_scheduler_task_correlation (SimScheduler  *scheduler,
				gpointer       data)
{
  SimContainer  *container;
  SimDatabase   *database;

  g_return_if_fail (scheduler != NULL);
  g_return_if_fail (SIM_IS_SCHEDULER (scheduler));

  container = scheduler->_priv->container;
  database = scheduler->_priv->database;

  sim_container_time_out_backlogs (container, database);
}

/*
 *
 *
 *
 */
void
sim_scheduler_run (SimScheduler *scheduler)
{
  GTimeVal      curr_time;

  g_return_if_fail (scheduler != NULL);
  g_return_if_fail (SIM_IS_SCHEDULER (scheduler));
  g_return_if_fail (scheduler->_priv->container != NULL);
  g_return_if_fail (SIM_IS_CONTAINER (scheduler->_priv->container));

  g_get_current_time (&curr_time);

  if (!last)
    last = curr_time.tv_sec;

  while (TRUE)
  {
    sim_scheduler_task_calculate (scheduler, NULL);
    sim_scheduler_task_correlation (scheduler, NULL);
  }
 
}
