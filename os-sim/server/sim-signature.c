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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <config.h>

#include "sim-database.h"
#include "sim-signature.h"

enum 
{
  DESTROY,
  LAST_SIGNAL
};

struct _SimSignaturePrivate {
  gint          id;
  gchar        *name;
  GList        *children;
};

static gpointer parent_class = NULL;
static gint sim_signature_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void 
sim_signature_impl_dispose (GObject  *gobject)
{
  G_OBJECT_CLASS (parent_class)->dispose (gobject);
}

static void 
sim_signature_impl_finalize (GObject  *gobject)
{
  SimSignature *msg = SIM_SIGNATURE (gobject);

  g_free (msg->_priv->name);

  G_OBJECT_CLASS (parent_class)->finalize (gobject);
}

static void
sim_signature_class_init (SimSignatureClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);

  object_class->dispose = sim_signature_impl_dispose;
  object_class->finalize = sim_signature_impl_finalize;
}

static void
sim_signature_instance_init (SimSignature *signature)
{
  signature->_priv = g_new0 (SimSignaturePrivate, 1);

  signature->type = SIM_SIGNATURE_TYPE_NONE;
  
  signature->_priv->id = 0;
  signature->_priv->name = NULL;
}

/* Public Methods */

GType
sim_signature_get_type (void)
{
  static GType object_type = 0;
 
  if (!object_type)
  {
    static const GTypeInfo type_info = {
              sizeof (SimSignatureClass),
              NULL,
              NULL,
              (GClassInitFunc) sim_signature_class_init,
              NULL,
              NULL,                       /* class data */
              sizeof (SimSignature),
              0,                          /* number of pre-allocs */
              (GInstanceInitFunc) sim_signature_instance_init,
              NULL                        /* value table */
    };
    
    g_type_init ();
                                                                                                                             
    object_type = g_type_register_static (G_TYPE_OBJECT, "SimSignature", &type_info, 0);
  }
                                                                                                                             
  return object_type;
}

/*
 *
 *
 *
 */
SimSignature*
sim_signature_new (SimSignatureType   type,
		   gint               id,
		   gchar             *name)
{
  SimSignature *signature = NULL;

  signature = SIM_SIGNATURE (g_object_new (SIM_TYPE_SIGNATURE, NULL));
  signature->type = type;
  signature->_priv->id = id;
  signature->_priv->name = name;

  return signature;
}

/*
 *
 *
 *
 */
SimSignature*
sim_signature_new_from_dm (SimSignatureType  type,
			   GdaDataModel     *dm,
			   gint              row)
{
  SimSignature *signature = NULL;
  gchar        *name;
  GdaValue     *value;

  value = (GdaValue *) gda_data_model_get_value_at (dm, 0, row);
  name = gda_value_stringify (value);

  signature = SIM_SIGNATURE (g_object_new (SIM_TYPE_SIGNATURE, NULL));
  signature->type = type;
  signature->_priv->id = sim_signature_get_group_type_enum (name);
  signature->_priv->name = name;

  return signature;
}

/*
 *
 *
 *
 */
SimSignatureGroupType 
sim_signature_get_group_type_enum                   (gchar *group)
{
  SimSignatureGroupType type = SIM_SIGNATURE_GROUP_TYPE_NONE;
 
  if (!strcmp (group, "attack-responses"))
    type = SIM_SIGNATURE_GROUP_TYPE_ATTACK_RESPONSES;
  else if (!strcmp (group, "backdoor"))
    type = SIM_SIGNATURE_GROUP_TYPE_BACKDOOR;
  else if (!strcmp (group, "bad-traffic"))
    type = SIM_SIGNATURE_GROUP_TYPE_BAD_TRAFFIC;
  else if (!strcmp (group, "chat"))
    type = SIM_SIGNATURE_GROUP_TYPE_CHAT;
  else if (!strcmp (group, "ddos"))
    type = SIM_SIGNATURE_GROUP_TYPE_DDOS;
  else if (!strcmp (group, "deleted"))
    type = SIM_SIGNATURE_GROUP_TYPE_DELETED;
  else if (!strcmp (group, "dns"))
    type = SIM_SIGNATURE_GROUP_TYPE_DNS;
  else if (!strcmp (group, "dos"))
    type = SIM_SIGNATURE_GROUP_TYPE_DOS;
  else if (!strcmp (group, "experimental"))
    type = SIM_SIGNATURE_GROUP_TYPE_EXPERIMENTAL;
  else if (!strcmp (group, "exploit"))
    type = SIM_SIGNATURE_GROUP_TYPE_EXPLOIT;
  else if (!strcmp (group, "finger"))
    type = SIM_SIGNATURE_GROUP_TYPE_FINGER;
  else if (!strcmp (group, "ftp"))
    type = SIM_SIGNATURE_GROUP_TYPE_FTP;
  else if (!strcmp (group, "fw1-accept"))
    type = SIM_SIGNATURE_GROUP_TYPE_FW1_ACCEPT;
  else if (!strcmp (group, "fw1-drop"))
    type = SIM_SIGNATURE_GROUP_TYPE_FW1_DROP;
  else if (!strcmp (group, "fw1-reject"))
    type = SIM_SIGNATURE_GROUP_TYPE_FW1_REJECT;
  else if (!strcmp (group, "icmp"))
    type = SIM_SIGNATURE_GROUP_TYPE_ICMP;
  else if (!strcmp (group, "icmp-info"))
    type = SIM_SIGNATURE_GROUP_TYPE_ICMP_INFO;
  else if (!strcmp (group, "imap"))
    type = SIM_SIGNATURE_GROUP_TYPE_IMAP;
  else if (!strcmp (group, "info"))
    type = SIM_SIGNATURE_GROUP_TYPE_INFO;
  else if (!strcmp (group, "local"))
    type = SIM_SIGNATURE_GROUP_TYPE_LOCAL;
  else if (!strcmp (group, "misc"))
    type = SIM_SIGNATURE_GROUP_TYPE_MISC;
  else if (!strcmp (group, "multimedia"))
    type = SIM_SIGNATURE_GROUP_TYPE_MULTIMEDIA;
  else if (!strcmp (group, "mysql"))
    type = SIM_SIGNATURE_GROUP_TYPE_MYSQL;
  else if (!strcmp (group, "netbios"))
    type = SIM_SIGNATURE_GROUP_TYPE_NETBIOS;
  else if (!strcmp (group, "nntp"))
    type = SIM_SIGNATURE_GROUP_TYPE_NNTP;
  else if (!strcmp (group, "oracle"))
    type = SIM_SIGNATURE_GROUP_TYPE_ORACLE;
  else if (!strcmp (group, "other-ids"))
    type = SIM_SIGNATURE_GROUP_TYPE_OTHER_IDS;
  else if (!strcmp (group, "p2p"))
    type = SIM_SIGNATURE_GROUP_TYPE_P2P;
  else if (!strcmp (group, "policy"))
    type = SIM_SIGNATURE_GROUP_TYPE_POLICY;
  else if (!strcmp (group, "pop2"))
    type = SIM_SIGNATURE_GROUP_TYPE_POP2;
  else if (!strcmp (group, "pop3"))
    type = SIM_SIGNATURE_GROUP_TYPE_POP3;
  else if (!strcmp (group, "porn"))
    type = SIM_SIGNATURE_GROUP_TYPE_PORN;
  else if (!strcmp (group, "rpc"))
    type = SIM_SIGNATURE_GROUP_TYPE_RPC;
  else if (!strcmp (group, "rservices"))
    type = SIM_SIGNATURE_GROUP_TYPE_RSERVICES;
  else if (!strcmp (group, "scan"))
    type = SIM_SIGNATURE_GROUP_TYPE_SCAN;
  else if (!strcmp (group, "shellcode"))
    type = SIM_SIGNATURE_GROUP_TYPE_SHELLCODE;
  else if (!strcmp (group, "smtp"))
    type = SIM_SIGNATURE_GROUP_TYPE_SMTP;
  else if (!strcmp (group, "snmp"))
    type = SIM_SIGNATURE_GROUP_TYPE_SNMP;
  else if (!strcmp (group, "spade"))
    type = SIM_SIGNATURE_GROUP_TYPE_SPADE;
  else if (!strcmp (group, "sql"))
    type = SIM_SIGNATURE_GROUP_TYPE_SQL;
  else if (!strcmp (group, "telnet"))
    type = SIM_SIGNATURE_GROUP_TYPE_TELNET;
  else if (!strcmp (group, "tftp"))
    type = SIM_SIGNATURE_GROUP_TYPE_TFTP;
  else if (!strcmp (group, "virus"))
    type = SIM_SIGNATURE_GROUP_TYPE_VIRUS;
  else if (!strcmp (group, "web-attacks"))
    type = SIM_SIGNATURE_GROUP_TYPE_WEB_ATTACKS;
  else if (!strcmp (group, "web-cgi"))
    type = SIM_SIGNATURE_GROUP_TYPE_WEB_CGI;
  else if (!strcmp (group, "web-client"))
    type = SIM_SIGNATURE_GROUP_TYPE_WEB_CLIENT;
  else if (!strcmp (group, "web-coldfusion"))
    type = SIM_SIGNATURE_GROUP_TYPE_WEB_COLDFUSION;
  else if (!strcmp (group, "web-frontpage"))
    type = SIM_SIGNATURE_GROUP_TYPE_WEB_FRONTPAGE;
  else if (!strcmp (group, "web-iis"))
    type = SIM_SIGNATURE_GROUP_TYPE_WEB_IIS;
  else if (!strcmp (group, "web-misc"))
    type = SIM_SIGNATURE_GROUP_TYPE_WEB_MISC;
  else if (!strcmp (group, "web-php"))
    type = SIM_SIGNATURE_GROUP_TYPE_WEB_PHP;
  else if (!strcmp (group, "x11"))
    type = SIM_SIGNATURE_GROUP_TYPE_X11;

    return type;
}

/*
 *
 *
 *
 */
gint
sim_signature_get_id (SimSignature *signature)
{
  g_return_val_if_fail (signature != NULL, 0);
  g_return_val_if_fail (SIM_IS_SIGNATURE (signature), 0);

  return signature->_priv->id;
}

/*
 *
 *
 *
 */
void
sim_signature_set_id (SimSignature *signature,
		      gint          id)
{
  g_return_if_fail (signature != NULL);
  g_return_if_fail (SIM_IS_SIGNATURE (signature));

  signature->_priv->id = id;
}

/*
 *
 *
 *
 */
gchar*
sim_signature_get_name (SimSignature *signature)
{
  g_return_val_if_fail (signature, NULL);
  g_return_val_if_fail (SIM_IS_SIGNATURE (signature), NULL);

  return signature->_priv->name;
}

/*
 *
 *
 *
 */
void
sim_signature_set_name (SimSignature *signature,
			gchar        *name)
{
  g_return_if_fail (signature != NULL);
  g_return_if_fail (SIM_IS_SIGNATURE (signature));
  g_return_if_fail (name != NULL);

  signature->_priv->name = name;
}

/*
 *
 *
 *
 */
void
sim_signature_append_child (SimSignature      *signature,
			    SimSignature      *child)
{
  g_return_if_fail (signature != NULL);
  g_return_if_fail (SIM_IS_SIGNATURE (signature));
  g_return_if_fail (child != NULL);
  g_return_if_fail (SIM_IS_SIGNATURE (child));

  signature->_priv->children = g_list_append (signature->_priv->children, child);
}

/*
 *
 *
 *
 */
void
sim_signature_remove_child (SimSignature      *signature,
			    SimSignature      *child)
{
  g_return_if_fail (signature != NULL);
  g_return_if_fail (SIM_IS_SIGNATURE (signature));
  g_return_if_fail (child != NULL);
  g_return_if_fail (SIM_IS_SIGNATURE (child));

  signature->_priv->children = g_list_remove (signature->_priv->children, child);
}

/*
 *
 *
 *
 */
GList*
sim_signature_get_children (SimSignature      *signature)
{
  g_return_val_if_fail (signature != NULL, NULL);
  g_return_val_if_fail (SIM_IS_SIGNATURE (signature), NULL);

  return signature->_priv->children;
}

/*
 *
 *
 *
 */
gboolean
sim_signature_has_child_id (SimSignature      *signature,
			    gint               id)
{
  GList *list;

  g_return_val_if_fail (signature, FALSE);
  g_return_val_if_fail (SIM_IS_SIGNATURE (signature), FALSE);

  list = signature->_priv->children;
  while (list)
    {
      SimSignature *child = (SimSignature *) list->data;
      
      if (child->_priv->id == id)
	{
	  return TRUE;
	}

      list = list->next;
    }

  return FALSE;
}
