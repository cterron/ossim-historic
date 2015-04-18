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

#include "sim-alert.h"

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimAlertPrivate {
  GTime           start_time;
  GTime           last_time;

  gint            level;
  SimDirective   *directive;
  GNode          *rule_node;
};

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_alert_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_alert_impl_finalize (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_alert_class_init (SimAlertClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->dispose = sim_alert_impl_dispose;
  object_class->finalize = sim_alert_impl_finalize;
}

static void
sim_alert_instance_init (SimAlert *alert)
{
  alert->_priv = g_new0 (SimAlertPrivate, 1);

  alert->_priv->start_time = 0;
  alert->_priv->last_time = 0;
  alert->_priv->level = 0;
  alert->_priv->directive = NULL;
}

/* Public Methods */

GType
sim_alert_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimAlertClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_alert_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimAlert),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_alert_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimAlert", &type_info, 0);
  }

  return object_type;
}

/*
 *
 *
 *
 *
 */
SimAlert*
sim_alert_new (SimDirective  *directive,
	       GTime          time)
{
  SimAlert *alert = NULL;

  alert = SIM_ALERT (g_object_new (SIM_TYPE_ALERT, NULL));
  alert->_priv->start_time = time;
  alert->_priv->last_time = time;
  alert->_priv->directive = directive;
  alert->_priv->rule_node = sim_directive_get_rule_root (directive);

  return alert;
}
