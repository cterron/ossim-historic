/* Network qualification functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <time.h>

#include "common.h"

NET_LINK
load_nets (MYSQL * mysql)
{
  MYSQL_RES *result;
  MYSQL_ROW row;
  unsigned int num_rows;
  NET_LINK head = NULL;
  char *query = "select * from net_qualification;";

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
	      /* Start adding nets to linked list */
	      head = add_net (row, head);
	    }
	  mysql_free_result (result);
	}
    }
  return head;
}

NET_LINK
add_net (char *row[], NET_LINK net)
{
  NET_LINK new = NULL;
  NET_LINK tmp = NULL;
  NET_LINK prev = NULL;
  char net_name[MAX_NET_NAME];

  strncpy (net_name, row[0], MAX_NET_NAME - 1);

  new = (NET_LINK) malloc (sizeof (OSSIM_NET));
  if (!new)
    {
      printf ("Unable to malloc\n");
      exit (1);
    }

  strncpy (new->net_name, row[0], MAX_NET_NAME - 1);
  new->c = atol (row[1]);
  new->a = atol (row[2]);
  new->next = NULL;

  /* Let's do some real work */
  /* Learnt many years ago with "Teach yourself C in 21 days" */
  /* Order by net_name (sortof...) */

  if (net == NULL)
    {				/* Err.. first ? */
      net = new;
      new->next = NULL;
    }
  else				/* No. */
    {
      if (new->net_name < net->net_name)
	{
	  new->next = net;
	  net = new;
	}
      else
	{
	  tmp = net->next;
	  prev = net;
	  if (tmp == NULL)
	    {
	      prev->next = new;
	    }
	  else
	    {
	      while ((tmp->next != NULL))
		{
		  if (new->net_name < tmp->net_name)
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
		  if (new->net_name < tmp->net_name)	/* Not yet */
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
  return (net);
}


void
show_nets (NET_LINK net)
{
  NET_LINK curr;

  curr = net;
  while (curr != NULL)
    {
      printf ("NET: %s\n", curr->net_name);
      printf ("\tC: %lu\n", curr->c);
      printf ("\tA: %lu\n", curr->a);
      curr = curr->next;
    }
}

NET_LINK
lower_nets (NET_LINK net, int recovery)
{
  NET_LINK curr;

  curr = net;
  while (curr != NULL)
    {
      if (curr->c > recovery)
	{
	  curr->c -= recovery;
	}
      else
	{
	  curr->c = 0;
	}
      if (curr->a > recovery)
	{
	  curr->a -= recovery;
	}
      else
	{
	  curr->a = 0;
	}
      curr = curr->next;
    }
  return (net);
}


int
get_net_level (NET_LINK net, char *net_name, char what, int *value)
{
  NET_LINK curr;

  curr = net;
  while (curr != NULL)
    {
      if (strstr (curr->net_name, net_name))
	{
	  switch (what)
	    {
	    case 'c':
	      *value = curr->c;
	      break;
	    case 'a':
	      *value = curr->a;
	      break;
	    }
	  return 1;
	}
      curr = curr->next;
    }
  return 0;
}

int
update_net_level (NET_LINK net, char *net_name, char what, int value)
{
  NET_LINK curr;

  curr = net;
  while (curr != NULL)
    {
      if (strstr (curr->net_name, net_name))
	{
	  switch (what)
	    {
	    case 'c':
	      curr->c += value;
	      break;
	    case 'a':
	      curr->a += value;
	      break;
	    }
	  return 1;
	}
      curr = curr->next;
    }
  return 0;
}

void
free_nets (NET_LINK net)
{
  NET_LINK curr, next;
  curr = net;

  while (curr != NULL)
    {
      next = curr->next;
      free (curr);
      curr = next;
    }
}

void
save_nets (MYSQL * mysql, NET_LINK net)
{
  NET_LINK curr;
  char query[QUERY_MAX_SIZE];


  curr = net;
  while (curr != NULL)
    {

      snprintf (query, QUERY_MAX_SIZE - 1,
		"update net_qualification set attack = %lu, compromise = %lu where net_name = '%s';",
		curr->a, curr->c, curr->net_name);

      if (mysql_query (mysql, query))
	{
	  /* query failed */
	  fprintf (stderr, "Failed to make query %s\n%s\n",
		   query, mysql_error (mysql));

	}


      curr = curr->next;
    }
}
