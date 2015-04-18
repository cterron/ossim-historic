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

#include "sim-action.h"

enum
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimActionPrivate {
};

static gpointer parent_class = NULL;
static gint sim_server_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_action_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_action_impl_finalize (GObject  *gobject)
{
  SimAction *action = SIM_ACTION (gobject);

  g_free (action->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_action_class_init (SimActionClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_ref (G_TYPE_OBJECT);

  object_class->dispose = sim_action_impl_dispose;
  object_class->finalize = sim_action_impl_finalize;
}

static void
sim_action_instance_init (SimAction *action)
{
  action->_priv = g_new0 (SimActionPrivate, 1);

  action->type = SIM_ACTION_TYPE_NONE;
}

/* Public Methods */

GType
sim_action_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimActionClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_action_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimAction),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_action_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimAction", &type_info, 0);
  }

  return object_type;
}

/*
 *
 *
 *
 *
 */
SimAction*
sim_action_new (void)
{
  SimAction *action = NULL;

  action = SIM_ACTION (g_object_new (SIM_TYPE_ACTION, NULL));

  return action;
}

/*
 *
 *
 *
 *
 */
SimAction*
sim_action_clone (SimAction *action)
{
  SimAction *new_action;
  
  g_return_val_if_fail (action != NULL, NULL);
  g_return_val_if_fail (SIM_IS_ACTION (action), NULL);

  new_action = SIM_ACTION (g_object_new (SIM_TYPE_ACTION, NULL));
  new_action->type = action->type;

  return new_action;
}
