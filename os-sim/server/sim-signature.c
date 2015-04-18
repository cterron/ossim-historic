/**
 *
 *
 *
 *
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
  gchar        *description;
  GList        *parents;
  GList        *children;
};

static gpointer parent_class = NULL;
static gint sim_signature_signals[LAST_SIGNAL] = { 0 };

/* GType Functions */

static void
sim_signature_class_init (SimSignatureClass * class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  parent_class = g_type_class_peek_parent (class);
}

static void
sim_signature_instance_init (SimSignature *signature)
{
  signature->_priv = g_new0 (SimSignaturePrivate, 1);

  signature->type = SIM_SIGNATURE_TYPE_NONE;
  signature->subgroup = SIM_SIGNATURE_SUBGROUP_TYPE_NONE;
  
  signature->_priv->id = 0;
  signature->_priv->name = NULL;
  signature->_priv->description = NULL;
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
SimSignature *
sim_signature_new (void)
{
  SimSignature *signature = NULL;

  signature = SIM_SIGNATURE (g_object_new (SIM_TYPE_SIGNATURE, NULL));

  return signature;
}

SimSignatureSubgroupType 
sim_signature_get_subgroup_type_enum                   (gchar *subgroup)
{
  SimSignatureSubgroupType type = SIM_SIGNATURE_SUBGROUP_TYPE_NONE;
 
  if (!strcmp (subgroup, "attack-responses"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_ATTACK_RESPONSES;
  else if (!strcmp (subgroup, "backdoor"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_BACKDOOR;
  else if (!strcmp (subgroup, "bad-traffic"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_BAD_TRAFFIC;
  else if (!strcmp (subgroup, "chat"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_CHAT;
  else if (!strcmp (subgroup, "ddos"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_DDOS;
  else if (!strcmp (subgroup, "deleted"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_DELETED;
  else if (!strcmp (subgroup, "dns"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_DNS;
  else if (!strcmp (subgroup, "dos"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_DOS;
  else if (!strcmp (subgroup, "experimental"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_EXPERIMENTAL;
  else if (!strcmp (subgroup, "exploit"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_EXPLOIT;
  else if (!strcmp (subgroup, "finger"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_FINGER;
  else if (!strcmp (subgroup, "ftp"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_FTP;
  else if (!strcmp (subgroup, "fw1-accept"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_FW1_ACCEPT;
  else if (!strcmp (subgroup, "fw1-drop"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_FW1_DROP;
  else if (!strcmp (subgroup, "fw1-reject"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_FW1_REJECT;
  else if (!strcmp (subgroup, "icmp"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_ICMP;
  else if (!strcmp (subgroup, "icmp-info"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_ICMP_INFO;
  else if (!strcmp (subgroup, "imap"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_IMAP;
  else if (!strcmp (subgroup, "info"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_INFO;
  else if (!strcmp (subgroup, "local"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_LOCAL;
  else if (!strcmp (subgroup, "misc"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_MISC;
  else if (!strcmp (subgroup, "multimedia"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_MULTIMEDIA;
  else if (!strcmp (subgroup, "mysql"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_MYSQL;
  else if (!strcmp (subgroup, "netbios"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_NETBIOS;
  else if (!strcmp (subgroup, "nntp"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_NNTP;
  else if (!strcmp (subgroup, "oracle"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_ORACLE;
  else if (!strcmp (subgroup, "other-ids"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_OTHER_IDS;
  else if (!strcmp (subgroup, "p2p"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_P2P;
  else if (!strcmp (subgroup, "policy"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_POLICY;
  else if (!strcmp (subgroup, "pop2"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_POP2;
  else if (!strcmp (subgroup, "pop3"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_POP3;
  else if (!strcmp (subgroup, "porn"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_PORN;
  else if (!strcmp (subgroup, "rpc"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_RPC;
  else if (!strcmp (subgroup, "rservices"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_RSERVICES;
  else if (!strcmp (subgroup, "scan"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_SCAN;
  else if (!strcmp (subgroup, "shellcode"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_SHELLCODE;
  else if (!strcmp (subgroup, "smtp"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_SMTP;
  else if (!strcmp (subgroup, "snmp"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_SNMP;
  else if (!strcmp (subgroup, "spade"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_SPADE;
  else if (!strcmp (subgroup, "sql"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_SQL;
  else if (!strcmp (subgroup, "telnet"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_TELNET;
  else if (!strcmp (subgroup, "tftp"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_TFTP;
  else if (!strcmp (subgroup, "virus"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_VIRUS;
  else if (!strcmp (subgroup, "web-attacks"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_WEB_ATTACKS;
  else if (!strcmp (subgroup, "web-cgi"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_WEB_CGI;
  else if (!strcmp (subgroup, "web-client"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_WEB_CLIENT;
  else if (!strcmp (subgroup, "web-coldfusion"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_WEB_COLDFUSION;
  else if (!strcmp (subgroup, "web-frontpage"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_WEB_FRONTPAGE;
  else if (!strcmp (subgroup, "web-iis"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_WEB_IIS;
  else if (!strcmp (subgroup, "web-misc"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_WEB_MISC;
  else if (!strcmp (subgroup, "web-php"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_WEB_PHP;
  else if (!strcmp (subgroup, "x11"))
    type = SIM_SIGNATURE_SUBGROUP_TYPE_X11;

    return type;
}

/*
 *
 *
 *
 */
GList*
sim_signature_load_from_db (GObject      *database,
			    GNode        *root)
{
  SimSignature *sig_root = NULL;
  SimSignature *sig_group = NULL;
  SimSignature *sig_subgroup = NULL;
  GdaDataModel *dm0;
  GdaDataModel *dm1;
  GdaValue     *value0;
  GdaValue     *value1;
  GNode        *group;
  GNode        *subgroup;
  GNode        *node_sig;
  GList        *groups = NULL;
  GList        *subgroups = NULL;
  GList        *list0;
  GList        *list1;
  GList        *node0;
  GList        *node1;
  gchar        *query2;
  gchar        *str;
  gint          row_id0;
  gint          row_id1;
  gint          i;
  FILE         *fd;

  gchar *query0 = "select * from signature";
  gchar *query1 = "select * from signature_group";

  g_return_if_fail (database != NULL);
  g_return_if_fail (SIM_IS_DATABASE (database));

  /* Root Signature */
  sig_root = sim_signature_new ();
  sig_root->type = SIM_SIGNATURE_TYPE_NONE;

  root->data = sig_root;

  /* List of Signatures Subgroups */
  list0 = sim_database_execute_command (SIM_DATABASE (database), query0);
  if (list0 != NULL)
    {
      for (node0 = g_list_first (list0); node0 != NULL; node0 = g_list_next (node0))
	{
	  dm0 = (GdaDataModel *) node0->data;
	  if (dm0 == NULL)
	    {
	      g_message ("SIGNATURE SUBGROUPS DATA MODEL ERROR");
	    }
	  else
	    {
	      for (row_id0 = 0; row_id0 < gda_data_model_get_n_rows (dm0); row_id0++)
		{
		  sig_subgroup = sim_signature_new ();
		  sig_subgroup->type = SIM_SIGNATURE_TYPE_SUBGROUP;

		  /* Name */
		  value0 = (GdaValue *) gda_data_model_get_value_at (dm0, 0, row_id0);
		  sig_subgroup->_priv->name = gda_value_stringify (value0);

		  sig_subgroup->subgroup = sim_signature_get_subgroup_type_enum (sig_subgroup->_priv->name);

		  subgroup = g_node_new (sig_subgroup);
		  subgroups = g_list_append (subgroups, subgroup);

		  if ((fd = fopen ("sids", "r")) == NULL)
		    {
		      printf ("Can't open file %s\n", "sids");
		      exit (-1);
		    }
		  
		  while (!feof (fd))
		    {
		      gchar str[BUFFER_SIZE], *ret, **split;
		      ret = fgets (str, BUFFER_SIZE, fd);

		      split = g_strsplit (str, ";", 0);

		      if (!strcmp (sig_subgroup->_priv->name, split[0]))
			{
			  SimSignature *signature;

			  signature = sim_signature_new ();
			  signature->type = SIM_SIGNATURE_TYPE_SIGNATURE;
			  signature->_priv->id = strtol(split[1], (char **)NULL, 10);
			  signature->_priv->name = g_strdup (split[2]);
			  
			  node_sig = g_node_new (signature);
			  g_node_insert (subgroup, -1, node_sig);
			}

		      g_strfreev (split);
		    }
		  fclose (fd);
		}
	      g_object_unref(dm0);
	    }
	}
    }
  else
    {
      g_message ("SIGNATURE SUBGROUPS LIST ERROR");
    }

  /* List of Signatures Goups */
  list0 = sim_database_execute_command (SIM_DATABASE (database), query1);
  if (list0 != NULL)
    {
      for (node0 = g_list_first (list0); node0 != NULL; node0 = g_list_next (node0))
	{
	  dm0 = (GdaDataModel *) node0->data;
	  if (dm0 == NULL)
	    {
	      g_message ("SIGNATURE DATA MODEL ERROR");
	    }
	  else
	    {
	      for (row_id0 = 0; row_id0 < gda_data_model_get_n_rows (dm0); row_id0++)
		{
		  sig_group = sim_signature_new ();
		  sig_group->type = SIM_SIGNATURE_TYPE_GROUP;

		  /* Name */
		  value0 = (GdaValue *) gda_data_model_get_value_at (dm0, 0, row_id0);
		  sig_group->_priv->name = gda_value_stringify (value0);

		  /* Description */
		  value0 = (GdaValue *) gda_data_model_get_value_at (dm0, 1, row_id0);
		  sig_group->_priv->description = gda_value_stringify (value0);

		  group = g_node_new (sig_group);

		  query2 = g_strdup_printf ("select * from signature_group_reference where sig_group_name = '%s'", 
						   sig_group->_priv->name);		  
		  list1 = sim_database_execute_command (SIM_DATABASE (database), query2);
		  if (list1 != NULL)
		    {
		      for (node1 = g_list_first (list1); node1 != NULL; node1 = g_list_next (node1))
			{
			  dm1 = (GdaDataModel *) node1->data;
			  if (dm1 == NULL)
			    {
				  g_message ("POLICIES DATA MODEL ERROR 1");
			    }
			  else
			    {
			      for (row_id1 = 0; row_id1 < gda_data_model_get_n_rows (dm1); row_id1++)
				{
				  /* Subgroup */
				  value1 = (GdaValue *) gda_data_model_get_value_at (dm1, 1, row_id1);
				  str = gda_value_stringify (value1);

				  /* Search subgroup */
				  for (i = 0; i < g_list_length(subgroups); i++) 
				    {
				      subgroup = (GNode *) g_list_nth_data (subgroups, i);
				      sig_subgroup = (SimSignature *) subgroup->data;

				      if (!strcmp (sig_subgroup->_priv->name, str))
					{
					  g_node_insert (group, -1, g_node_copy (subgroup));
					  break;
					}
				    }
				  g_free (str);
				}
			      
			      g_object_unref(dm1);
			    }
			}
		    }
		  g_free (query2);

		  groups = g_list_append (groups, group);
		  g_node_insert (root, -1, group);
		}

	      g_object_unref(dm0);
	    }
	}
    }
  else
    {
      g_message ("SIGNATURE LIST ERROR");
    }

  return groups;
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
  g_return_val_if_fail (signature != NULL, NULL);
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
