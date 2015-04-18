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

#if !defined(__sim_xml_directive_h__)
#  define __sim_xml_directive_h__

#include "sim-action.h"
#include "sim-rule.h"
#include "sim-directive.h"
#include "sim-container.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define SIM_TYPE_XML_DIRECTIVE            (sim_xml_directive_get_type())
#define SIM_XML_DIRECTIVE(obj)            (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_XML_DIRECTIVE, SimXmlDirective))
#define SIM_XML_DIRECTIVE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_XML_DIRECTIVE, SimXmlDirectiveClass))
#define SIM_IS_XML_DIRECTIVE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_XML_DIRECTIVE))
#define SIM_IS_XML_DIRECTIVE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_XML_DIRECTIVE))

typedef struct _SimXmlDirective        SimXmlDirective;
typedef struct _SimXmlDirectiveClass   SimXmlDirectiveClass;
typedef struct _SimXmlDirectivePrivate SimXmlDirectivePrivate;

struct _SimXmlDirective {
	GObject object;
	SimXmlDirectivePrivate *_priv;
};

struct _SimXmlDirectiveClass {
	GObjectClass parent_class;

	/* signals */
	void (*changed) (SimXmlDirective * xmldb);
};

GType            sim_xml_directive_get_type (void);

SimXmlDirective* sim_xml_directive_new (void);
SimXmlDirective* sim_xml_directive_new_from_file (SimContainer *container,
						  const gchar *file);

void             sim_xml_directive_set_container (SimXmlDirective *xmldirect,
						  SimContainer    *container);

void             sim_xml_directive_changed (SimXmlDirective *xmldirect);
void             sim_xml_directive_reload (SimXmlDirective *xmldirect);
gboolean         sim_xml_directive_save (SimXmlDirective *xmldirect,
					 const gchar *file);
gchar*           sim_xml_directive_to_string (SimXmlDirective *xmldirect);

SimDirective*    sim_xml_directive_new_directive_from_node (SimXmlDirective *xmldirect,
							    xmlNodePtr       node);

SimAction*       sim_xml_directive_new_action_from_node (SimXmlDirective *xmldirect,
							 xmlNodePtr       node);

GNode*           sim_xml_directive_new_rule_from_node (SimXmlDirective  *xmldirect,
						       xmlNodePtr        node,
						       GNode            *root,
						       gint             level);

GList*           sim_xml_directive_get_directives (SimXmlDirective *xmldirect);

G_END_DECLS

#endif
