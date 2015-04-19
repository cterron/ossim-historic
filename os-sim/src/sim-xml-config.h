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

#if !defined(__sim_xml_config_h__)
#  define __sim_xml_config_h__


#include "sim-container.h"
#include "sim-config.h"

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define SIM_TYPE_XML_CONFIG            (sim_xml_config_get_type())
#define SIM_XML_CONFIG(obj)            (G_TYPE_CHECK_INSTANCE_CAST (obj, SIM_TYPE_XML_CONFIG, SimXmlConfig))
#define SIM_XML_CONFIG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST (klass, SIM_TYPE_XML_CONFIG, SimXmlConfigClass))
#define SIM_IS_XML_CONFIG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE (obj, SIM_TYPE_XML_CONFIG))
#define SIM_IS_XML_CONFIG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), SIM_TYPE_XML_CONFIG))

typedef struct _SimXmlConfig        SimXmlConfig;
typedef struct _SimXmlConfigClass   SimXmlConfigClass;
typedef struct _SimXmlConfigPrivate SimXmlConfigPrivate;

struct _SimXmlConfig {
	GObject object;

	SimXmlConfigPrivate *_priv;
};

struct _SimXmlConfigClass {
	GObjectClass parent_class;

	/* signals */
	void (*changed) (SimXmlConfig * xmldb);
};

GType            sim_xml_config_get_type (void);

SimXmlConfig*    sim_xml_config_new (void);
SimXmlConfig*    sim_xml_config_new_from_file (const gchar *file);

void             sim_xml_config_changed (SimXmlConfig *xmlconfig);
void             sim_xml_config_reload (SimXmlConfig *xmlconfig);
gboolean         sim_xml_config_save (SimXmlConfig *xmlconfig,
				      const gchar *file);
gchar*           sim_xml_config_to_string (SimXmlConfig *xmlconfig);

SimConfig*       sim_xml_config_new_config_from_node (SimXmlConfig *xmlconfig,
																								      xmlNodePtr       node);

SimConfig*       sim_xml_config_get_config (SimXmlConfig  *xmlconfig);

G_END_DECLS

#endif
// vim: set tabstop=2:
