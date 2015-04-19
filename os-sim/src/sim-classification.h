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

#ifndef __SIM_CLASSIFICATION_H__
#define __SIM_CLASSIFICATION_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <libgda/libgda.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_CLASSIFICATION                  (sim_classification_get_type ())
#define SIM_CLASSIFICATION(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_CLASSIFICATION, SimClassification))
#define SIM_CLASSIFICATION_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_CLASSIFICATION, SimClassificationClass))
#define SIM_IS_CLASSIFICATION(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_CLASSIFICATION))
#define SIM_IS_CLASSIFICATION_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_CLASSIFICATION))
#define SIM_CLASSIFICATION_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_CLASSIFICATION, SimClassificationClass))

G_BEGIN_DECLS

typedef struct _SimClassification          SimClassification;
typedef struct _SimClassificationClass     SimClassificationClass;
typedef struct _SimClassificationPrivate   SimClassificationPrivate;

struct _SimClassification {
  GObject parent;

  SimClassificationPrivate *_priv;
};

struct _SimClassificationClass {
  GObjectClass parent_class;
};

GType                 sim_classification_get_type                    (void);
SimClassification*    sim_classification_new                         (void);
SimClassification*    sim_classification_new_from_data               (gint                 id,
								      const gchar         *name,
								      const gchar         *description,
								      gint                 priority);
SimClassification*    sim_classification_new_from_dm                 (GdaDataModel        *dm,
								      gint                 row);
  
gint                  sim_classification_get_id                      (SimClassification   *classification);
void                  sim_classification_set_id                      (SimClassification   *classification,
								      gint                 id);
gchar*                sim_classification_get_name                    (SimClassification   *classification);
void                  sim_classification_set_name                    (SimClassification   *classification,
								      const gchar         *name);
gchar*                sim_classification_get_description             (SimClassification   *classification);
void                  sim_classification_set_description             (SimClassification   *classification,
								      const gchar         *description);
gint                  sim_classification_get_priority                (SimClassification   *classification);
void                  sim_classification_set_priority                (SimClassification   *classification,
								      gint                 priority);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_CLASSIFICATION_H__ */

// vim: set tabstop=2:

