/* Host qualification functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <time.h>

#include "common.h"

HOST_LINK
load_hosts (MYSQL * mysql)
{
  MYSQL_RES *result;
  MYSQL_ROW row;
  unsigned int num_rows;
  HOST_LINK head = NULL;
  char *query = "select * from host_qualification;";

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
	      /* Start adding hosts to linked list */
	      head = add_host (row, head);
	    }
	  mysql_free_result (result);
	}
    }
  return head;
}

HOST_LINK
add_host (char *row[], HOST_LINK host)
{
  HOST_LINK new = NULL;
  HOST_LINK tmp = NULL;
  HOST_LINK prev = NULL;
  struct in_addr ip;

  ip.s_addr = inet_addr (row[0]);

  new = (HOST_LINK) malloc (sizeof (OSSIM_HOST));
  if (!new)
    {
      printf ("Unable to malloc\n");
      exit (1);
    }

  new->ip = ip;
  new->c = atol (row[1]);
  new->a = atol (row[2]);
  new->next = NULL;

  /* Let's do some real work */
  /* Learnt many years ago with "Teach yourself C in 21 days" */
  /* Order by ip */

  if (host == NULL)
    {				/* Err.. first ? */
      host = new;
      new->next = NULL;
    }
  else				/* No. */
    {
      if (new->ip.s_addr < host->ip.s_addr)
	{
	  new->next = host;
	  host = new;
	}
      else
	{
	  tmp = host->next;
	  prev = host;
	  if (tmp == NULL)
	    {
	      prev->next = new;
	    }
	  else
	    {
	      while ((tmp->next != NULL))
		{
		  if (new->ip.s_addr < tmp->ip.s_addr)
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
		  if (new->ip.s_addr < tmp->ip.s_addr)	/* Not yet */
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
  return (host);
}


void
show_hosts (HOST_LINK host)
{
  HOST_LINK curr;

  curr = host;
  while (curr != NULL)
    {
      printf ("IP: %s\n", inet_ntoa (curr->ip));
      printf ("\tC: %lu\n", curr->c);
      printf ("\tA: %lu\n", curr->a);
      curr = curr->next;
    }
}

HOST_LINK
lower_hosts (HOST_LINK host, int recovery)
{
  HOST_LINK curr;

  curr = host;
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
  return (host);
}


int
get_host_level (HOST_LINK host, char *ip, char what, int *value)
{
  HOST_LINK curr;

  curr = host;
  while (curr != NULL)
    {
      if (strstr (inet_ntoa (curr->ip), ip))
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
update_host_level (HOST_LINK host, char *ip, char what, int value)
{
  HOST_LINK curr;

  curr = host;
  while (curr != NULL)
    {
      if (strstr (inet_ntoa (curr->ip), ip))
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
free_hosts (HOST_LINK host)
{
  HOST_LINK curr, next;
  curr = host;

  while (curr != NULL)
    {
      next = curr->next;
      free (curr);
      curr = next;
    }
}
