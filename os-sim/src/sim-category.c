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
 
#include "sim-category.h"

enum
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimCategoryPrivate {
  gint             id;
  gchar           *name;
};

static gpointer parent_class = NULL;
static gint sim_category_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_category_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_category_impl_finalize (GObject  *gobject)
{
  SimCategory  *category = SIM_CATEGORY (gobject);

  if (category->_priv->name)
    g_free (category->_priv->name);

  g_free (category->_priv);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_category_class_init (SimCategoryClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_category_impl_dispose;
  object_class->finalize = sim_category_impl_finalize;
}

static void
sim_category_instance_init (SimCategory *category)
{
  category->_priv = g_new0 (SimCategoryPrivate, 1);

  category->_priv->id = 0;
  category->_priv->name = NULL;
}

/* Public Methods */

GType
sim_category_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimCategoryClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_category_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimCategory),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_category_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimCategory", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 *
 */
SimCategory*
sim_category_new (void)
{
  SimCategory *category = NULL;
  
  category = SIM_CATEGORY (g_object_new (SIM_TYPE_CATEGORY, NULL));
  
  return category;
}

/*
 *
 *
 *
 *
 */
SimCategory*
sim_category_new_from_data (gint           id,
			    const gchar   *name)
{
  SimCategory *category = NULL;

  g_return_val_if_fail (id > 0, NULL);
  g_return_val_if_fail (name, NULL);

  category = SIM_CATEGORY (g_object_new (SIM_TYPE_CATEGORY, NULL));
  category->_priv->id = id;
  category->_priv->name = g_strdup (name);

  return category;
}

/*
 *
 *
 *
 */
SimCategory*
sim_category_new_from_dm (GdaDataModel  *dm,
			  gint           row)
{
  SimCategory     *category;
  GdaValue        *value;

  g_return_val_if_fail (dm, NULL);
  g_return_val_if_fail (GDA_IS_DATA_MODEL (dm), NULL);
  g_return_val_if_fail (row >= 0, NULL);

  category = SIM_CATEGORY (g_object_new (SIM_TYPE_CATEGORY, NULL));

  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
  category->_priv->id = gda_value_get_integer (value);

  value = (GdaValue *) gda_data_model_get_value_at (dm, 1, row);
  category->_priv->name = gda_value_stringify (value);

  return category;
}

/*
 *
 *
 *
 */
gint
sim_category_get_id (SimCategory  *category)
{
  g_return_val_if_fail (category, 0);
  g_return_val_if_fail (SIM_IS_CATEGORY (category), 0);
  
  return category->_priv->id;
}

/*
 *
 *
 *
 */
void
sim_category_set_id (SimCategory  *category,
		     gint     id)
{
  g_return_if_fail (category);
  g_return_if_fail (SIM_IS_CATEGORY (category));
  
  category->_priv->id = id;
}

/*
 *
 *
 *
 */
gchar*
sim_category_get_name (SimCategory  *category)
{
  g_return_val_if_fail (category, NULL);
  g_return_val_if_fail (SIM_IS_CATEGORY (category), NULL);

  return category->_priv->name;
}

/*
 *
 *
 *
 */
void
sim_category_set_name (SimCategory       *category,
		       const gchar       *name)
{
  g_return_if_fail (category);
  g_return_if_fail (SIM_IS_CATEGORY (category));
  g_return_if_fail (name);
  
  if (category->_priv->name)
    g_free (category->_priv->name);

  category->_priv->name = g_strdup (name);
}

