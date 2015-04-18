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

#ifndef __SIM_DIRECTIVE_GROUP_H__
#define __SIM_DIRECTIVE_GROUP_H__ 1

#include <glib.h>
#include <glib-object.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SIM_TYPE_DIRECTIVE_GROUP		(sim_directive_group_get_gtype ())
#define SIM_DIRECTIVE_GROUP(obj)		(G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_DIRECTIVE_GROUP, SimDirectiveGroup))
#define SIM_DIRECTIVE_GROUP_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_DIRECTIVE_GROUP, SimDirectiveGroupClass))
#define SIM_IS_DIRECTIVE_GROUP(obj)		(G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_DIRECTIVE_GROUP))
#define SIM_IS_DIRECTIVE_GROUP_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_DIRECTIVE_GROUP))
#define SIM_DIRECTIVE_GROUP_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), SIM_TYPE_DIRECTIVE_GROUP, SimDirectiveGroupClass))

G_BEGIN_DECLS

typedef struct _SimDirectiveGroup		SimDirectiveGroup;
typedef struct _SimDirectiveGroupClass		SimDirectiveGroupClass;
typedef struct _SimDirectiveGroupPrivate	SimDirectiveGroupPrivate;

struct _SimDirectiveGroup {
  GObject	parent;

  SimDirectiveGroupPrivate	*_priv;
};

struct _SimDirectiveGroupClass {
  GObjectClass	parent_class;
};

GType			sim_directive_group_get_gtype		(void);
SimDirectiveGroup*	sim_directive_group_new			(void);

gchar*			sim_directive_group_get_name		(SimDirectiveGroup	*dg);
void			sim_directive_group_set_name		(SimDirectiveGroup	*dg,
								 gchar			*name);
gboolean		sim_directive_group_get_sticky		(SimDirectiveGroup	*dg);
void			sim_directive_group_set_sticky		(SimDirectiveGroup	*dg,
								 gboolean		sticky);

void			sim_directive_group_append_id		(SimDirectiveGroup	*dg,
								 gint			id);
void			sim_directive_group_remove_id		(SimDirectiveGroup	*dg,
								 gint			id);
GList*			sim_directive_group_get_ids		(SimDirectiveGroup	*dg);

G_END_DECLS

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SIM_DIRECTIVE_GROUP_H__ */
