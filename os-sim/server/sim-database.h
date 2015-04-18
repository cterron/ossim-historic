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

#ifndef __SIM_DATABASE_H__
#define __SIM_DATABASE_H__ 1

#include <glib.h>
#include <glib-object.h>
#include <libgda/libgda.h>

#include "sim-enums.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_DATABASE                  (sim_database_get_type ())
#define SIM_DATABASE(obj)                  (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_DATABASE, SimDatabase))
#define SIM_DATABASE_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_DATABASE, SimDatabaseClass))
#define SIM_IS_DATABASE(obj)               (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_DATABASE))
#define SIM_IS_DATABASE_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_DATABASE))
#define SIM_DATABASE_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_DATABASE, SimDatabaseClass))

G_BEGIN_DECLS

typedef struct _SimDatabase        SimDatabase;
typedef struct _SimDatabaseClass   SimDatabaseClass;
typedef struct _SimDatabasePrivate SimDatabasePrivate;

struct _SimDatabase {
  GObject parent;

  SimDatabasePrivate *_priv;
};

struct _SimDatabaseClass {
  GObjectClass parent_class;
};

GType           sim_database_get_type                        (void);
SimDatabase*    sim_database_new                             (gchar *datasource,
							      gchar *username,
							      gchar *password);
gint            sim_database_execute_no_query                (SimDatabase *database,
							      gchar       *buffer);
GList*          sim_database_execute_command                 (SimDatabase *database,
							      gchar       *buffer);
GdaDataModel*   sim_database_execute_single_command          (SimDatabase *database,
							      gchar       *buffer);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_DATABASE_H__ */
