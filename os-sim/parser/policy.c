/* Policy handling */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <time.h>

#include "common.h"

POL_LINK
load_policy (MYSQL * mysql)
{
  MYSQL_RES *result;
  MYSQL_ROW row;
  unsigned int num_rows;
  POL_LINK head = NULL;
  char *query = "select * from policy;";

  if (mysql_query (mysql, query))
    {
      /* query failed */
      fprintf (stderr, "Failed to make query %s\n%s\n",
	       query, mysql_error (mysql));

    }
  else
    {
      /* query succeeded */
      if ((result = mysql_store_result (mysql)) &&
	  (num_rows = mysql_num_rows (result)))
	{
	  while ((row = mysql_fetch_row (result)))
	    {
	      /* Start adding policies to linked list */
	      head = add_policy (mysql, row, head);
	    }
	  mysql_free_result (result);
	}
    }
  return head;
}

POL_LINK
add_policy (MYSQL * mysql, char *row[], POL_LINK pol)
{
  POL_LINK new = NULL;
  POL_LINK tmp = NULL;
  POL_LINK prev = NULL;
  int policy_id;
  unsigned int num_rows;
  char query_pol[QUERY_MAX_SIZE];
  char query_temp[QUERY_MAX_SIZE];
  char *next = NULL;
  MYSQL_RES *result = NULL;
  MYSQL_RES *result2 = NULL;
  MYSQL_ROW row_pol;
  MYSQL_ROW row_temp;

  policy_id = atoi (row[0]);

  new = (POL_LINK) malloc (sizeof (POLICY));
  if (!new)
    {
      printf ("Unable to malloc\n");
      exit (1);
    }

  new->policy_id = policy_id;
  new->priority = atoi (row[1]);
  strncpy (new->desc, row[2], MAX_DESC);
  new->next = NULL;

/* Get hosts, source */

  snprintf (query_pol, QUERY_MAX_SIZE,
	    "select * from policy_host_reference  where policy_id = %d and direction = 'source'",
	    policy_id);
  if (mysql_query (mysql, query_pol))
    {
      /* query failed */
      fprintf (stderr, "Failed to make query %s\n%s\n",
	       query_pol, mysql_error (mysql));

    }
  else
    {
      /* query succeeded */
      if ((result = mysql_store_result (mysql)) &&
	  (num_rows = mysql_num_rows (result)))
	{
	  next = new->source_ips;
	  while ((row_pol = mysql_fetch_row (result)))
	    {
	      if (strlen (new->source_ips) > MAX_IPS - 30)
		{
		  printf ("Too many ips, we've got a problem\n");
		  exit (1);
		}
	      strcat (next, "|");
	      strcat (next, row_pol[1]);
	      strcat (next, "|");
	      next += sizeof (row_pol[1]) + 1;
	    }
	  mysql_free_result (result);
	}
    }


/* Get networks, source */

  snprintf (query_pol, QUERY_MAX_SIZE,
	    "select * from policy_net_reference  where policy_id = %d and direction = 'source'",
	    policy_id);
  if (mysql_query (mysql, query_pol))
    {
      /* query failed */
      fprintf (stderr, "Failed to make query %s\n%s\n",
	       query_pol, mysql_error (mysql));

    }
  else
    {
      /* query succeeded */
      if ((result = mysql_store_result (mysql)) &&
	  (num_rows = mysql_num_rows (result)))
	{
	  if (next == NULL)
	    next = new->source_ips;
	  while ((row_pol = mysql_fetch_row (result)))
	    {
	      snprintf (query_temp, QUERY_MAX_SIZE,
			"select * from net_host_reference where net_name = '%s'",
			row_pol[1]);
	      if (mysql_query (mysql, query_temp))
		{
		  /* query failed */
		  fprintf (stderr, "Failed to make query %s\n%s\n",
			   query_temp, mysql_error (mysql));
		}
	      else
		{
		  /* query succeeded */
		  if ((result2 = mysql_store_result (mysql)) &&
		      (num_rows = mysql_num_rows (result2)))
		    {
		      while ((row_temp = mysql_fetch_row (result2)))
			{
			  if (strlen (new->source_ips) > MAX_IPS - 30)
			    {
			      printf ("Too many ips, we've got a problem\n");
			      exit (1);
			    }
			  strcat (next, "|");
			  strcat (next, row_temp[1]);
			  strcat (next, "|");
			  next += sizeof (row_temp[1]) + 1;
			}

		    }
		  mysql_free_result (result2);
		}
	    }
	  mysql_free_result (result);
	}
    }


/* Get hosts, dest*/

  snprintf (query_pol, QUERY_MAX_SIZE,
	    "select * from policy_host_reference  where policy_id = %d and direction = 'dest'",
	    policy_id);
  if (mysql_query (mysql, query_pol))
    {
      /* query failed */
      fprintf (stderr, "Failed to make query %s\n%s\n",
	       query_pol, mysql_error (mysql));

    }
  else
    {
      /* query succeeded */
      if ((result = mysql_store_result (mysql)) &&
	  (num_rows = mysql_num_rows (result)))
	{
	  next = new->dest_ips;
	  while ((row_pol = mysql_fetch_row (result)))
	    {
	      if (strlen (new->dest_ips) > MAX_IPS - 30)
		{
		  printf ("Too many ips, we've got a problem\n");
		  exit (1);
		}
	      strcat (next, "|");
	      strcat (next, row_pol[1]);
	      strcat (next, "|");
	      next += sizeof (row_pol[1]) + 1;
	    }
	  mysql_free_result (result);
	}
    }

/* Get networks, dest */

  snprintf (query_pol, QUERY_MAX_SIZE,
	    "select * from policy_net_reference  where policy_id = %d and direction = 'dest'",
	    policy_id);
  if (mysql_query (mysql, query_pol))
    {
      /* query failed */
      fprintf (stderr, "Failed to make query %s\n%s\n",
	       query_pol, mysql_error (mysql));

    }
  else
    {
      /* query succeeded */
      if ((result = mysql_store_result (mysql)) &&
	  (num_rows = mysql_num_rows (result)))
	{
	  if (next == NULL)
	    next = new->dest_ips;
	  while ((row_pol = mysql_fetch_row (result)))
	    {
	      snprintf (query_temp, QUERY_MAX_SIZE,
			"select * from net_host_reference where net_name = '%s'",
			row_pol[1]);
	      if (mysql_query (mysql, query_temp))
		{
		  /* query failed */
		  fprintf (stderr, "Failed to make query %s\n%s\n",
			   query_temp, mysql_error (mysql));
		}
	      else
		{
		  /* query succeeded */
		  if ((result2 = mysql_store_result (mysql)) &&
		      (num_rows = mysql_num_rows (result2)))
		    {
		      while ((row_temp = mysql_fetch_row (result2)))
			{
			  if (strlen (new->dest_ips) > MAX_IPS - 30)
			    {
			      printf ("Too many ips, we've got a problem\n");
			      exit (1);
			    }
			  strcat (next, "|");
			  strcat (next, row_temp[1]);
			  strcat (next, "|");
			  next += sizeof (row_temp[1]) + 1;
			}

		    }
		  mysql_free_result (result2);
		}
	    }
	  mysql_free_result (result);
	}
    }

/* Get ports */

  snprintf (query_pol, QUERY_MAX_SIZE,
	    "select * from policy_port_reference  where policy_id = %d",
	    policy_id);
  if (mysql_query (mysql, query_pol))
    {
      /* query failed */
      fprintf (stderr, "Failed to make query %s\n%s\n",
	       query_pol, mysql_error (mysql));

    }
  else
    {
      /* query succeeded */
      if ((result = mysql_store_result (mysql)) &&
	  (num_rows = mysql_num_rows (result)))
	{
	  next = new->port_list;
	  while ((row_pol = mysql_fetch_row (result)))
	    {
	      snprintf (query_temp, QUERY_MAX_SIZE,
			"select * from port_group_reference where port_group_name = '%s'",
			row_pol[1]);
	      if (mysql_query (mysql, query_temp))
		{
		  /* query failed */
		  fprintf (stderr, "Failed to make query %s\n%s\n",
			   query_temp, mysql_error (mysql));
		}
	      else
		{
		  /* query succeeded */
		  if ((result2 = mysql_store_result (mysql)) &&
		      (num_rows = mysql_num_rows (result2)))
		    {
		      while ((row_temp = mysql_fetch_row (result2)))
			{
			  if (strlen (new->port_list) > MAX_PORTS - 15)
			    {
			      printf
				("Too many ports, we've got a problem\n");
			      exit (1);
			    }
			  /* port_number/proto */
			  strcat (next, "|");
			  strcat (next, row_temp[1]);
			  strcat (next, "/");
			  strcat (next, row_temp[2]);
			  strcat (next, "|");
			  next += sizeof (row_temp[1]) + 1;
			}

		    }
		  mysql_free_result (result2);
		}
	    }
	  mysql_free_result (result);
	}
    }

  snprintf (query_pol, QUERY_MAX_SIZE,
	    "select * from policy_sig_reference where policy_id = %d",
	    policy_id);
  if (mysql_query (mysql, query_pol))
    {
      /* query failed */
      fprintf (stderr, "Failed to make query %s\n%s\n",
	       query_pol, mysql_error (mysql));

    }
  else
    {
      /* query succeeded */
      if ((result = mysql_store_result (mysql)) &&
	  (num_rows = mysql_num_rows (result)))
	{
	  next = new->sigs;
	  while ((row_pol = mysql_fetch_row (result)))
	    {
	      snprintf (query_temp, QUERY_MAX_SIZE,
			"select * from signature_group_reference where sig_group_name = '%s'",
			row_pol[1]);
	      if (mysql_query (mysql, query_temp))
		{
		  /* query failed */
		  fprintf (stderr, "Failed to make query %s\n%s\n",
			   query_temp, mysql_error (mysql));
		}
	      else
		{
		  /* query succeeded */
		  if ((result2 = mysql_store_result (mysql)) &&
		      (num_rows = mysql_num_rows (result2)))
		    {
		      while ((row_temp = mysql_fetch_row (result2)))
			{
			  if (strlen (new->sigs) > MAX_SIGS - 50)
			    {
			      printf ("Too many sigs, we've got a problem\n");
			      exit (1);
			    }
			  strcat (next, "|");
			  strcat (next, row_temp[1]);
			  strcat (next, "|");
			  next += sizeof (row_temp[1]) + 1;
			}

		    }
		  mysql_free_result (result2);
		}
	    }
	  mysql_free_result (result);
	}
    }



/* Get sensor */
  snprintf (query_pol, QUERY_MAX_SIZE,
	    "select * from policy_sensor_reference where policy_id = %d",
	    policy_id);
  if (mysql_query (mysql, query_pol))
    {
      /* query failed */
      fprintf (stderr, "Failed to make query %s\n%s\n",
	       query_pol, mysql_error (mysql));

    }
  else
    {
      /* query succeeded */
      if ((result = mysql_store_result (mysql)) &&
	  (num_rows = mysql_num_rows (result)))
	{
	  next = new->sensors;
	  while ((row_pol = mysql_fetch_row (result)))
	    {
	      if (strlen (new->sensors) > MAX_SENSORS - 30)
		{
		  printf ("Too many sensors, we've got a problem\n");
		  exit (1);
		}
	      strcat (next, "|");
	      strcat (next, row_pol[1]);
	      strcat (next, "|");
	      next += sizeof (row_pol[1]) + 1;
	    }
	  mysql_free_result (result);
	}
    }

/* Get timeframe */
  snprintf (query_pol, QUERY_MAX_SIZE,
	    "select * from policy_time where policy_id = %d", policy_id);
  if (mysql_query (mysql, query_pol))
    {
      /* query failed */
      fprintf (stderr, "Failed to make query %s\n%s\n",
	       query_pol, mysql_error (mysql));

    }
  else
    {
      /* query succeeded */
      if ((result = mysql_store_result (mysql)) &&
	  (num_rows = mysql_num_rows (result)))
	{
	  if ((row_pol = mysql_fetch_row (result)))
	    {
	      new->begin_hour = atoi (row_pol[1]);
	      new->end_hour = atoi (row_pol[2]);
	      new->begin_day = atoi (row_pol[3]);
	      new->end_day = atoi (row_pol[4]);
	    }
	  mysql_free_result (result);
	}
    }


  /* Let's do some real work */
  /* Learnt many years ago with "Teach yourself C in 21 days" */
  /* Order by policy_id */

  if (pol == NULL)
    {				/* Err.. first ? */
      pol = new;
      new->next = NULL;
    }
  else				/* No. */
    {
      if (new->policy_id < pol->policy_id)
	{
	  new->next = pol;
	  pol = new;
	}
      else
	{
	  tmp = pol->next;
	  prev = pol;
	  if (tmp == NULL)
	    {
	      prev->next = new;
	    }
	  else
	    {
	      while ((tmp->next != NULL))
		{
		  if (new->policy_id < tmp->policy_id)
		    {
		      new->next = tmp;
		      if (new->next != prev->next)
			{
			  printf ("Error");
			  exit (0);
			}
		      prev->next = new;
		      break;
		    }
		  else
		    {
		      tmp = tmp->next;
		      prev = prev->next;
		    }
		}
	      if (tmp->next == NULL)	/* End ? */
		{
		  if (new->policy_id < tmp->policy_id)	/* Not yet */
		    {
		      new->next = tmp;
		      prev->next = new;
		    }
		  else		/* End */
		    {
		      tmp->next = new;
		      new->next = NULL;
		    }
		}
	    }
	}
    }
  return (pol);
}

void
show_policy (POL_LINK pol)
{
  POL_LINK curr;

  curr = pol;
  while (curr != NULL)
    {
      printf ("Policy ID = %d\n---------------------\n", curr->policy_id);
      printf ("Priority = %d\n---------------------\n", curr->priority);
      printf ("Source_ips:%s\n", curr->source_ips);
      printf ("Dest_ips:%s\n", curr->dest_ips);
      printf ("Timeframe:%d-%d,%d-%d\n", curr->begin_day, curr->begin_hour,
	      curr->end_day, curr->end_hour);
      printf ("Ports:%s\n", curr->port_list);
      printf ("Sigs:%s\n", curr->sigs);
      printf ("Sensors:%s\n", curr->sensors);
      printf ("Desc:%s\n\n", curr->desc);
      curr = curr->next;
    }
}

void
free_policy (POL_LINK pol)
{
  POL_LINK curr, next;
  curr = pol;

  while (curr != NULL)
    {
      next = curr->next;
      free (curr);
      curr = next;
    }
}

int
get_priority (char *source_ip, char *dest_ip, char *protocol, char *dest_port,
	      char *signature, int date_expr, POL_LINK pol, int *priority)
{

  POL_LINK curr;
  char fixed_source[100];
  char fixed_dest[100];
  char fixed_port[100];
  char fixed_sig[100];
  char *tcp = "tcp";
  char *udp = "udp";
  char *icmp = "icmp";
  int start;
  int end;

  if (strstr (protocol, "TCP"))
    protocol = tcp;
  if (strstr (protocol, "UDP"))
    protocol = udp;
  if (strstr (protocol, "ICMP"))
    protocol = icmp;

  snprintf (fixed_source, 100, "|%s|", source_ip);
  snprintf (fixed_dest, 100, "|%s|", dest_ip);
  snprintf (fixed_port, 100, "|%s/%s|", dest_port, protocol);
  snprintf (fixed_sig, 100, "|%s|", signature);

  curr = pol;
  while (curr != NULL)
    {
      start = ((curr->begin_day - 1) * 7 + curr->begin_hour);
      end = ((curr->end_day - 1) * 7 + curr->end_hour);
      if ((strstr (curr->source_ips, fixed_source)
	   || strstr (curr->source_ips, "|any|"))
	  && (strstr (curr->dest_ips, fixed_dest)
	      || strstr (curr->dest_ips, "|any|")) && ((start < date_expr)
						       && (end > date_expr))
	  && (strstr (curr->port_list, fixed_port))
	  && (strstr (curr->sigs, fixed_sig)))
	{
#ifdef VERBOSE
	  printf ("\nPriority level 1!:%d\n", curr->priority);
#endif
	  *priority = curr->priority;
	  return 1;
	}
      curr = curr->next;
    }

  return 0;

}
