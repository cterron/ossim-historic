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
#include <sys/socket.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sim-util.h>
#include <gnet.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "sim-inet.h"

struct _SimPortProtocol {
  gint              port;
  SimProtocolType   protocol;
};

/*
 *
 *
 *
 */
SimProtocolType
sim_protocol_get_type_from_str (const gchar  *str)
{
  g_return_val_if_fail (str, SIM_PROTOCOL_TYPE_NONE);

  if (!g_ascii_strcasecmp (str, "ICMP"))
    return SIM_PROTOCOL_TYPE_ICMP;
  else if (!g_ascii_strcasecmp (str, "UDP"))
    return SIM_PROTOCOL_TYPE_UDP;
  else if (!g_ascii_strcasecmp (str, "TCP"))
    return SIM_PROTOCOL_TYPE_TCP;

  return SIM_PROTOCOL_TYPE_NONE;
}

/*
 *
 *
 *
 */
gchar*
sim_protocol_get_str_from_type (SimProtocolType type)
{
  switch (type)
    {
    case SIM_PROTOCOL_TYPE_ICMP:
      return g_strdup ("ICMP");
    case SIM_PROTOCOL_TYPE_UDP:
      return g_strdup ("UDP");
    case SIM_PROTOCOL_TYPE_TCP:
      return g_strdup ("TCP");
    default:
      return NULL;
    }
}

/*
 *
 *
 *
 */
SimConditionType
sim_condition_get_type_from_str (const gchar  *str)
{
  g_return_val_if_fail (str, SIM_CONDITION_TYPE_NONE);

  if (!g_ascii_strcasecmp (str, "eq"))
    return SIM_CONDITION_TYPE_EQ;
  else if (!g_ascii_strcasecmp (str, "ne"))
    return SIM_CONDITION_TYPE_NE;
  else if (!g_ascii_strcasecmp (str, "lt"))
    return SIM_CONDITION_TYPE_LT;
  else if (!g_ascii_strcasecmp (str, "le"))
    return SIM_CONDITION_TYPE_LE;
  else if (!g_ascii_strcasecmp (str, "gt"))
    return SIM_CONDITION_TYPE_GT;
  else if (!g_ascii_strcasecmp (str, "ge"))
    return SIM_CONDITION_TYPE_GE;

  return SIM_CONDITION_TYPE_NONE;
}

/*
 *
 *
 *
 */
gchar*
sim_condition_get_str_from_type (SimConditionType  type)
{
  switch (type)
    {
    case SIM_CONDITION_TYPE_EQ:
      return g_strdup ("eq");
    case SIM_CONDITION_TYPE_NE:
      return g_strdup ("ne");
    case SIM_CONDITION_TYPE_LT:
      return g_strdup ("lt");
    case SIM_CONDITION_TYPE_LE:
      return g_strdup ("le");
    case SIM_CONDITION_TYPE_GT:
      return g_strdup ("gt");
    case SIM_CONDITION_TYPE_GE:
      return g_strdup ("ge");
    default:
      return NULL;
    }
}

/*
 *
 *
 *
 */
SimPortProtocol*
sim_port_protocol_new (gint              port,
		       SimProtocolType   protocol)
{
  SimPortProtocol  *pp;

  g_return_val_if_fail (port >= 0, NULL);
  g_return_val_if_fail (protocol >= 0, NULL);

  pp = g_new0 (SimPortProtocol, 1);
  pp->port = port;
  pp->protocol = protocol;

  return pp;
}

/*
 *
 *
 *
 */
gboolean
sim_port_protocol_equal (SimPortProtocol  *pp1,
			 SimPortProtocol  *pp2)
{
  g_return_val_if_fail (pp1, FALSE);  
  g_return_val_if_fail (pp2, FALSE);  

  if (pp1->port == 0)
    return TRUE;    

  if ((pp1->port == pp2->port) && (pp1->protocol == pp2->protocol))
    return TRUE;

  return FALSE;
}

/*
 *
 *
 *
 */
SimRuleVarType
sim_get_rule_var_from_char (const gchar *var)
{
  g_return_val_if_fail (var != NULL, SIM_RULE_VAR_NONE);

  if (!strcmp (var, SIM_SRC_IP_CONST))
    return SIM_RULE_VAR_SRC_IA;
  else if (!strcmp (var, SIM_DST_IP_CONST))
    return SIM_RULE_VAR_DST_IA;
  else if (!strcmp (var, SIM_SRC_PORT_CONST))
    return SIM_RULE_VAR_SRC_PORT;
  else if (!strcmp (var, SIM_DST_PORT_CONST))
    return SIM_RULE_VAR_DST_PORT;
  else if (!strcmp (var, SIM_PROTOCOL_CONST))
    return SIM_RULE_VAR_PROTOCOL;
  else if (!strcmp (var, SIM_PLUGIN_SID_CONST))
    return SIM_RULE_VAR_PLUGIN_SID;

  return SIM_RULE_VAR_NONE;
}

/*
 *
 *
 *
 */
SimAlarmRiskType
sim_get_alarm_risk_from_char (const gchar *var)
{
  g_return_val_if_fail (var != NULL, SIM_ALARM_RISK_TYPE_NONE);

  if (!g_ascii_strcasecmp (var, "low"))
    return SIM_ALARM_RISK_TYPE_LOW;
  else if (!g_ascii_strcasecmp (var, "medium"))
    return SIM_ALARM_RISK_TYPE_MEDIUM;
  else if (!g_ascii_strcasecmp (var, "high"))
    return SIM_ALARM_RISK_TYPE_HIGH;
  else if (!g_ascii_strcasecmp (var, "all"))
    return SIM_ALARM_RISK_TYPE_ALL;
  else
    return SIM_ALARM_RISK_TYPE_NONE;
}

/*
 *
 *
 *
 */
SimAlarmRiskType
sim_get_alarm_risk_from_risk (gint risk)
{
  if ((risk >= 1) && risk <= 4)
    return SIM_ALARM_RISK_TYPE_LOW;
  else if (risk >= 5 && risk <= 7)
    return SIM_ALARM_RISK_TYPE_MEDIUM;
  else if (risk >= 8 && risk <= 10)
    return SIM_ALARM_RISK_TYPE_HIGH;
  else
    return SIM_ALARM_RISK_TYPE_NONE;
}

/*
 *
 *
 *
 */
GList*
sim_get_ias (const gchar *value)
{
  GInetAddr  *ia;
  GList      *list = NULL;

  g_return_val_if_fail (value != NULL, NULL);

  ia = gnet_inetaddr_new_nonblock (value, 0);

  list = g_list_append (list, ia);

  return list;
}

/*
 *
 *
 *
 */
GList*
sim_get_inets (const gchar *value)
{
  SimInet    *inet;
  GList      *list = NULL;
  gchar      *endptr;
  gchar      *slash;
  gint        from;
  gint        to;
  gint        i;

  g_return_val_if_fail (value != NULL, NULL);

  /* Look for a range */
  slash = strchr (value, '-');
  if (slash)
    {
      gchar **values0 = g_strsplit(value, ".", 0);
      if (values0[3])
	{
	  gchar **values1 = g_strsplit(values0[3], "-", 0);

	  from = strtol (values1[0], &endptr, 10);
	  to = strtol (values1[1], &endptr, 10);

	  for (i = 0; i <= (to - from); i++)
	    {
	      gchar *ip = g_strdup_printf ("%s.%s.%s.%d/32",
					   values0[0], values0[1],
					   values0[2], from + i);

	      inet = sim_inet_new (ip);
	      list = g_list_append (list, inet);

	      g_free (ip);
	    }

	  g_strfreev (values1);
	}

      g_strfreev (values0);
    }
  else
    {
      inet = sim_inet_new (value);
      list = g_list_append (list, inet);
    }

  return list;
}

/* function called by g_hash_table_foreach to add items to a GList */
static void
add_string_key_to_list (gpointer key, gpointer value, gpointer user_data)
{
        GList **list = (GList **) user_data;

        *list = g_list_append (*list, g_strdup (key));
}

/**
 * sim_string_hash_to_list
 */
GList *
sim_string_hash_to_list (GHashTable *hash_table)
{
	GList *list = NULL;

        g_return_val_if_fail (hash_table != NULL, NULL);

        g_hash_table_foreach (hash_table, (GHFunc) add_string_key_to_list, &list);
        return list;
}

/**
 * sim_file_load
 * @filename: path for the file to be loaded.
 *
 * Loads a file, specified by the given @uri, and returns the file
 * contents as a string.
 *
 * It is the caller's responsibility to free the returned value.
 *
 * Returns: the file contents as a newly-allocated string, or NULL
 * if there is an error.
 */
gchar *
sim_file_load (const gchar *filename)
{
  gchar *retval = NULL;
  gsize length = 0;
  GError *error = NULL;
  
  g_return_val_if_fail (filename != NULL, NULL);
  
  if (g_file_get_contents (filename, &retval, &length, &error))
    return retval;
  
  g_message ("Error while reading %s: %s", filename, error->message);
  g_error_free (error);
  
  return NULL;
}

/**
 * sim_file_save
 * @filename: path for the file to be saved.
 * @buffer: contents of the file.
 * @len: size of @buffer.
 *
 * Saves a chunk of data into a file.
 *
 * Returns: TRUE if successful, FALSE on error.
 */
gboolean
sim_file_save (const gchar *filename, const gchar *buffer, gint len)
{
  gint fd;
  gint res;
  
  g_return_val_if_fail (filename != NULL, FALSE);
  
  fd = open (filename, O_RDWR | O_CREAT, 0644);
  if (fd == -1) {
    g_message ("Could not create file %s", filename);
    return FALSE;
  }
  
  res = write (fd, (const void *) buffer, len);
  close (fd);
  
  return res == -1 ? FALSE : TRUE;
}

/**
 *
 *
 *
 *
 *
 */
gulong
sim_inetaddr_aton (GInetAddr     *ia)
{
  struct   in_addr in;
  gchar   *ip;
  gulong   val = -1;

  g_return_val_if_fail (ia, -1);

  if (!(ip = gnet_inetaddr_get_canonical_name (ia)))
    return -1;

  if (inet_aton (ip, &in)) val = in.s_addr;

  g_free (ip);

  return val;
}

/**
 *
 *
 *
 *
 *
 */
gulong
sim_inetaddr_ntohl (GInetAddr     *ia)
{
  struct   in_addr in;
  gchar   *ip;
  gulong   val = -1;

  g_return_val_if_fail (ia, -1);

  if (!(ip = gnet_inetaddr_get_canonical_name (ia)))
    return -1;

  if (inet_aton (ip, &in)) val = g_ntohl (in.s_addr);

  g_free (ip);

  return val;
}
