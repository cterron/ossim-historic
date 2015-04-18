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
 
#include "sim-classification.h"

enum
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimClassificationPrivate {
  gint             id;
  gchar           *name;
  gchar           *description;
  gint             priority;
};

static gpointer parent_class = NULL;
static gint sim_classification_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_classification_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_classification_impl_finalize (GObject  *gobject)
{
  SimClassification  *classification = SIM_CLASSIFICATION (gobject);

  if (classification->_priv->name)
    g_free (classification->_priv->name);
  if (classification->_priv->description)
    g_free (classification->_priv->description);

  g_free (classification->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_classification_class_init (SimClassificationClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_classification_impl_dispose;
  object_class->finalize = sim_classification_impl_finalize;
}

static void
sim_classification_instance_init (SimClassification *classification)
{
  classification->_priv = g_new0 (SimClassificationPrivate, 1);

  classification->_priv->id = 0;
  classification->_priv->name = NULL;
  classification->_priv->description = NULL;
  classification->_priv->priority = 0;
}

/* Public Methods */

GType
sim_classification_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimClassificationClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_classification_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimClassification),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_classification_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimClassification", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimClassification*
sim_classification_new (void)
{
  SimClassification *classification = NULL;
  
  classification = SIM_CLASSIFICATION (g_object_new (SIM_TYPE_CLASSIFICATION, NULL));
  
  return classification;
}

/*
 *
 *
 *
 *
 */
SimClassification*
sim_classification_new_from_data (gint           id,
				  const gchar   *name,
				  const gchar   *description,
				  gint           priority)
{
  SimClassification *classification = NULL;

  g_return_val_if_fail (id > 0, NULL);
  g_return_val_if_fail (name, NULL);

  classification = SIM_CLASSIFICATION (g_object_new (SIM_TYPE_CLASSIFICATION, NULL));
  classification->_priv->id = id;
  classification->_priv->name = g_strdup (name);
  if (description) classification->_priv->description = g_strdup (description);
  if (priority >= 0) classification->_priv->priority = priority;

  return classification;
}

/*
 *
 *
 *
 */
SimClassification*
sim_classification_new_from_dm (GdaDataModel  *dm,
				gint           row)
{
  SimClassification     *classification;
  GdaValue        *value;

  g_return_val_if_fail (dm, NULL);
  g_return_val_if_fail (GDA_IS_DATA_MODEL (dm), NULL);
  g_return_val_if_fail (row >= 0, NULL);
  
  classification = SIM_CLASSIFICATION (g_object_new (SIM_TYPE_CLASSIFICATION, NULL));
  
  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
  classification->_priv->id = gda_value_get_integer (value);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row);
  classification->_priv->name = gda_value_stringify (value);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 2, row);
  classification->_priv->description = gda_value_stringify (value);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 3, row);
  classification->_priv->priority = gda_value_get_integer (value);
  
  return classification;
}

/*
 *
 *
 *
 */
gint
sim_classification_get_id (SimClassification  *classification)
{
  g_return_val_if_fail (classification, 0);
  g_return_val_if_fail (SIM_IS_CLASSIFICATION (classification), 0);
  
  return classification->_priv->id;
}

/*
 *
 *
 *
 */
void
sim_classification_set_id (SimClassification  *classification,
			   gint                id)
{
  g_return_if_fail (classification);
  g_return_if_fail (SIM_IS_CLASSIFICATION (classification));
  
  classification->_priv->id = id;
}

/*
 *
 *
 *
 */
gchar*
sim_classification_get_name (SimClassification  *classification)
{
  g_return_val_if_fail (classification, NULL);
  g_return_val_if_fail (SIM_IS_CLASSIFICATION (classification), NULL);

  return classification->_priv->name;
}

/*
 *
 *
 *
 */
void
sim_classification_set_name (SimClassification  *classification,
			     const gchar        *name)
{
  g_return_if_fail (classification);
  g_return_if_fail (SIM_IS_CLASSIFICATION (classification));
  g_return_if_fail (name);
  
  if (classification->_priv->name)
    g_free (classification->_priv->name);

  classification->_priv->name = g_strdup (name);
}

/*
 *
 *
 *
 */
gchar*
sim_classification_get_description (SimClassification  *classification)
{
  g_return_val_if_fail (classification, NULL);
  g_return_val_if_fail (SIM_IS_CLASSIFICATION (classification), NULL);

  return classification->_priv->description;
}

/*
 *
 *
 *
 */
void
sim_classification_set_description (SimClassification  *classification,
				    const gchar        *description)
{
  g_return_if_fail (classification);
  g_return_if_fail (SIM_IS_CLASSIFICATION (classification));
  g_return_if_fail (description);
  
  if (classification->_priv->description)
    g_free (classification->_priv->description);

  classification->_priv->description = g_strdup (description);
}

/*
 *
 *
 *
 */
gint
sim_classification_get_priority (SimClassification  *classification)
{
  g_return_val_if_fail (classification, 0);
  g_return_val_if_fail (SIM_IS_CLASSIFICATION (classification), 0);
  
  return classification->_priv->priority;
}

/*
 *
 *
 *
 */
void
sim_classification_set_priority (SimClassification  *classification,
				 gint                priority)
{
  g_return_if_fail (classification);
  g_return_if_fail (SIM_IS_CLASSIFICATION (classification));
  
  classification->_priv->priority = priority;
}
