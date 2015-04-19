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
#include "sim-database.h"

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
  else if (!g_ascii_strcasecmp (str, "Host_ARP_Event"))
    return SIM_PROTOCOL_TYPE_HOST_ARP_EVENT;
  else if (!g_ascii_strcasecmp (str, "Host_OS_Event"))
    return SIM_PROTOCOL_TYPE_HOST_OS_EVENT;
  else if (!g_ascii_strcasecmp (str, "Host_Service_Event"))
    return SIM_PROTOCOL_TYPE_HOST_SERVICE_EVENT;
  else if (!g_ascii_strcasecmp (str, "Host_IDS_Event"))
    return SIM_PROTOCOL_TYPE_HOST_IDS_EVENT;
  else if (!g_ascii_strcasecmp (str, "Information_Event"))
    return SIM_PROTOCOL_TYPE_INFORMATION_EVENT;
  else if (!g_ascii_strcasecmp (str, "OTHER"))
    return SIM_PROTOCOL_TYPE_OTHER;
 
  return SIM_PROTOCOL_TYPE_NONE;
}

/*
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
    case SIM_PROTOCOL_TYPE_HOST_ARP_EVENT:
      return g_strdup ("Host_ARP_Event");
    case SIM_PROTOCOL_TYPE_HOST_OS_EVENT:
      return g_strdup ("Host_OS_Event");
    case SIM_PROTOCOL_TYPE_HOST_SERVICE_EVENT:
      return g_strdup ("Host_Service_Event");
    case SIM_PROTOCOL_TYPE_HOST_IDS_EVENT:
      return g_strdup ("Host_IDS_Event");
    case SIM_PROTOCOL_TYPE_INFORMATION_EVENT:
      return g_strdup ("Information_Event");
    default:
      return g_strdup ("OTHER");
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
  g_return_val_if_fail (protocol >= -1, NULL);

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

//      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Policy port: %d , protocol: %d", pp1->port, pp1->protocol);
//      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "       port: %d , protocol: %d", pp2->port, pp2->protocol);
      
  if (pp1->port == 0)	//if the port defined in policy is "0", its like ANY and all the ports will match
    return TRUE;    

  if ((pp1->port == pp2->port) && (pp1->protocol == pp2->protocol))
    return TRUE;

  return FALSE;
}





/*
 *
 * FIXME:I think that this function is useless until we make a "sim_xml_directive_set_rule_*" function.  
 * This returns the var type of the n level in a rule from a directive
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
  else if (!strcmp (var, SIM_SENSOR_CONST))
    return SIM_RULE_VAR_SENSOR;
  else if (!strcmp (var, SIM_FILENAME_CONST))
    return SIM_RULE_VAR_FILENAME;
  else if (!strcmp (var, SIM_USERNAME_CONST))
    return SIM_RULE_VAR_USERNAME;
  else if (!strcmp (var, SIM_PASSWORD_CONST))
    return SIM_RULE_VAR_PASSWORD;
  else if (!strcmp (var, SIM_USERDATA1_CONST))
    return SIM_RULE_VAR_USERDATA1;
  else if (!strcmp (var, SIM_USERDATA2_CONST))
    return SIM_RULE_VAR_USERDATA2;
  else if (!strcmp (var, SIM_USERDATA3_CONST))
    return SIM_RULE_VAR_USERDATA3;
  else if (!strcmp (var, SIM_USERDATA4_CONST))
    return SIM_RULE_VAR_USERDATA4;
  else if (!strcmp (var, SIM_USERDATA5_CONST))
    return SIM_RULE_VAR_USERDATA5;
  else if (!strcmp (var, SIM_USERDATA6_CONST))
    return SIM_RULE_VAR_USERDATA6;
  else if (!strcmp (var, SIM_USERDATA7_CONST))
    return SIM_RULE_VAR_USERDATA7;
  else if (!strcmp (var, SIM_USERDATA8_CONST))
    return SIM_RULE_VAR_USERDATA8;
  else if (!strcmp (var, SIM_USERDATA9_CONST))
    return SIM_RULE_VAR_USERDATA9;
	
  return SIM_RULE_VAR_NONE;
}

/*
 * Used to get the variable type from properties in the directive
 */
/*
SimRuleVarType
sim_get_rule_var_from_property (const gchar *var)
{

  if (!strcmp (var, PROPERTY_FILENAME))
    return SIM_RULE_VAR_FILENAME;
  else if (!strcmp (var, PROPERTY_USERNAME))
    return SIM_RULE_VAR_USERNAME;
  else if (!strcmp (var, PROPERTY_PASSWORD))
    return SIM_RULE_VAR_PASSWORD;
  else if (!strcmp (var, PROPERTY_USERDATA1))
    return SIM_RULE_VAR_USERDATA1;
  else if (!strcmp (var, PROPERTY_USERDATA2))
    return SIM_RULE_VAR_USERDATA2;
  else if (!strcmp (var, PROPERTY_USERDATA3))
    return SIM_RULE_VAR_USERDATA3;
  else if (!strcmp (var, PROPERTY_USERDATA4))
    return SIM_RULE_VAR_USERDATA4;
  else if (!strcmp (var, PROPERTY_USERDATA5))
    return SIM_RULE_VAR_USERDATA5;
  else if (!strcmp (var, PROPERTY_USERDATA6))
    return SIM_RULE_VAR_USERDATA6;
  else if (!strcmp (var, PROPERTY_USERDATA7))
    return SIM_RULE_VAR_USERDATA7;
  else if (!strcmp (var, PROPERTY_USERDATA8))
    return SIM_RULE_VAR_USERDATA8;
  else if (!strcmp (var, PROPERTY_USERDATA9))
    return SIM_RULE_VAR_USERDATA9;
	
  return SIM_RULE_VAR_NONE;
}
*/

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
 * Given a string with network(s) or hosts, it returns a GList of SimInet objects (one network or host each object).
 * The format can be only: "192.168.1.1-40" or  "192.168.1.0/24" or "192.168.1.1".
 * This function doesn't accepts multiple hosts or nets.
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

  /* Look for a range: 192.168.0.1-20. This kind of network is stored in memory using hosts with 32 bit network mask */
  slash = strchr (value, '-');
  if (slash)
  {
    gchar **values0 = g_strsplit(value, ".", 0);
    if (values0[3])
  	{
	    gchar **values1 = g_strsplit(values0[3], "-", 0);

	  	from = strtol (values1[0], &endptr, 10);
		  to = strtol (values1[1], &endptr, 10);	

		  for (i = 0; i <= (to - from); i++)  //transform every IP into a host SimInet object and store into it
	    {
	      gchar *ip = g_strdup_printf ("%s.%s.%s.%d/32",
					   values0[0], values0[1],
					   values0[2], from + i);

	      inet = sim_inet_new (ip); 	//is this a host or a network? well, it's the same :)
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

/*
 *
 * Takes any string like "192.168.1.0-40,192.168.1.0/24,192.168.5.6", transform everything into SimInet objects
 * and put them into a GList. If the string has some "ANY", every other ip or network is removed. Then, inside the
 * GList wich is returned just will be one SimInet object wich contains "0.0.0.0".
 */
GList*
sim_get_SimInet_from_string (const gchar *value)
{
  SimInet    *inet;
  GList      *list = NULL;
  GList      *list_temp = NULL;
	gint i;

  g_return_val_if_fail (value != NULL, NULL);

  if ( g_strstr_len (value, strlen(value), SIM_IN_ADDR_ANY_CONST) ||
			 g_strstr_len (value, strlen(value), "any")) //if appears "ANY" anywhere in the string
  {
    inet = sim_inet_new(SIM_IN_ADDR_ANY_IP_STR);
    list = g_list_append(list, inet);
		return list;
  }

  if (strchr (value, ','))  		//multiple networks or hosts
  {
    gchar **values = g_strsplit (value, ",", 0);
    for (i = 0; values[i] != NULL; i++)
		{
			//g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_get_SimInet_from_string: values[%d] = %s", i, values[i]);
		  list_temp = sim_get_inets(values[i]);
			while (list_temp)
			{
				inet = (SimInet *) list_temp->data;
				list = g_list_append (list, inet);
				
				list_temp = list_temp->next;
			}
		}
		g_strfreev (values);
  }
  else 													//uh, just one network or one host.
	{
    list_temp = sim_get_inets (value);
    while (list_temp)
    {
      inet = (SimInet *) list_temp->data;
      list = g_list_append (list, inet);
			list_temp = list->next;
    }
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
 * Transforms a GInetAddr into an unsigned long.
 *
 *
 */
inline gulong
sim_inetaddr_ntohl (GInetAddr     *ia)
{
  struct   in_addr in;
  gchar   *ip;
  gulong   val = -1;

  g_return_val_if_fail (ia, -1);

  if (!(ip = gnet_inetaddr_get_canonical_name (ia)))
    return -1;

  if (inet_aton (ip, &in))
		val = g_ntohl (in.s_addr);

  g_free (ip);

  return val;
}

/*
 * Transforms a gchar * (i.e. 192.168.1.1) into an unsigned long
 */
inline gulong
sim_ipchar_2_ulong (gchar     *ip)
{
  struct   in_addr in;
  gulong   val = -1;

  if (inet_aton (ip, &in))
		val = g_ntohl (in.s_addr);

  return val;
}

/*
 * Check if all the characters in the given string are numbers, so we can transform
 * that string into a number if we want, or whatever.
 * The parameter may_be_float tell us if we have to check also if it's a
 * floating number, checking one "." in the string
 * may_be_float = 0 means no float.
 */
inline gboolean
sim_string_is_number (gchar *string, 
                      gboolean may_be_float)
{
	int n;
	gboolean ok = FALSE;
  int count = 0;

	if (!string)
		return FALSE;

	for (n=0; n < strlen(string); n++)
	{
	  if (g_ascii_isdigit (string[n]))
	    ok=TRUE;
	  else
    if (may_be_float)
    { 
      if ((string[n] == '.') && (count == 0))
      {
        count++;
        ok = TRUE;
      }			
    }
    else
	  {
	    ok = FALSE;
	    break;
	  }
	}
	return ok;
}

/*
 * Check if exists and remove all the appearances of the character from a string.
 * A pointer to the same string is returned to allow nesting (if needed).
 */
inline gchar *
sim_string_remove_char	(gchar *string,
													gchar c)
{
	int n;
	gboolean ok = FALSE;
  int count = 0;

	if (!string)
		return FALSE;

	gchar *s = string;
	
	while ((s = strchr (s, c)) != NULL)
		memmove (s, s+1, strlen (s));
	
	return string;
}

/*
 * Check if exists and substitute all the appearances of c_orig in the string,
 * with the character c_dest.
 * A pointer to the same string is returned.
 */
inline gchar *
sim_string_substitute_char	(gchar *string,
														gchar c_orig,
														gchar	c_dest)
{
	int n;
	gboolean ok = FALSE;
  int count = 0;

	if (!string)
		return FALSE;

	gchar *s = string;
	
	while ((s = strchr (s, c_orig)) != NULL)
		*s = c_dest;
	
	return string;
}


/*
 * Substitute for g_strv_length() as it's just supported in some environments
 */
guint 
sim_g_strv_length (gchar **str_array)
{
	  guint i = 0;
	  g_return_val_if_fail (str_array != NULL, 0);

	  while (str_array[i])
	    ++i;

	  return i;
}


/*
 * 
 * Used to debug wich is the value from a GdaValue to know the right function to call.
 * 
 */
void sim_gda_value_extract_type(GdaValue *value)
{
	GdaValueType lala;
	lala = gda_value_get_type(value);
  g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "sim_gda_value_extract_type");
						
	switch (lala)
	{
		case GDA_VALUE_TYPE_NULL:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_NULL");
						break;
		case GDA_VALUE_TYPE_BIGINT:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_BIGINT");
						break;
		case GDA_VALUE_TYPE_BIGUINT:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_BIGUINT");
						break;
		case GDA_VALUE_TYPE_BINARY:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_BINARY");
						break;
		case GDA_VALUE_TYPE_BLOB:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_BLOB");
						break;
		case GDA_VALUE_TYPE_BOOLEAN:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_BOOLEAN");
						break;
		case GDA_VALUE_TYPE_DATE:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_DATE");
						break;
		case GDA_VALUE_TYPE_DOUBLE:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_DOUBLE");
						break;
		case GDA_VALUE_TYPE_GEOMETRIC_POINT:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_GEOMETRIC_POINT");
						break;
		case GDA_VALUE_TYPE_GOBJECT:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_GOBJECT");
						break;
		case GDA_VALUE_TYPE_INTEGER:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_INTEGER");
						break;
		case GDA_VALUE_TYPE_LIST:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_LIST");
						break;
		case GDA_VALUE_TYPE_MONEY:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_MONEY");
						break;
		case GDA_VALUE_TYPE_NUMERIC:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_NUMERIC");
						break;
		case GDA_VALUE_TYPE_SINGLE:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_SINGLE");
						break;
		case GDA_VALUE_TYPE_SMALLINT:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_SMALLINT");
						break;
		case GDA_VALUE_TYPE_SMALLUINT:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_SMALLUINT");
						break;
		case GDA_VALUE_TYPE_STRING:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_STRING");
						break;
		case GDA_VALUE_TYPE_TIME:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_TIME");
						break;
		case GDA_VALUE_TYPE_TIMESTAMP:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_TIMESTAMP");
						break;
		case GDA_VALUE_TYPE_TINYINT:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_TINYINT");
						break;
		case GDA_VALUE_TYPE_TINYUINT:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_TINYUINT");
						break;
		case GDA_VALUE_TYPE_TYPE:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_TYPE");
						break;
		case GDA_VALUE_TYPE_UINTEGER:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_UINTEGER");
						break;
		case GDA_VALUE_TYPE_UNKNOWN:
			      g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "GDA_VALUE_TYPE_UNKNOWN");
						break;
		default:
						g_log (G_LOG_DOMAIN, G_LOG_LEVEL_DEBUG, "Error: GDA_VALUE_TYPE Desconocido");									
	}

}

/*
 * Arguments:
 * GList: list of gchar*
 * string: string to check.
 *
 * this function will take a glist and will check if the string is any of the strings inside the GList
 * Warning: Please, use this function just to check gchar's. Any other use will be very probably a segfault.
 */
gboolean
sim_cmp_list_gchar (GList *list, gchar *string)
{
	if (!string)
		return FALSE;

	gchar *cmp;
	while (list)
	{
		cmp = (gchar *) list->data;
		if (!strcmp (cmp, string))
			return TRUE;							//found!
		list = list->next;
	}
	return FALSE;
	
}


/*
 *
 * 
 *
 *dentro de hostmac:
 * sim_event_counter(event->time, SIM_COMMAND_SYMBOL_HOST_MAC_EVENT, event->sensor);
 */

