/* Host->asset functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <time.h>

#include "common.h"

ASSET_LINK
load_assets (MYSQL * mysql)
{
  MYSQL_RES *result;
  MYSQL_ROW row;
  unsigned int num_rows;
  ASSET_LINK head = NULL;
  char *query = "select * from host;";

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
	      head = add_asset (mysql, row, head);
	    }
	  mysql_free_result (result);
	}
    }
  return head;
}

ASSET_LINK
add_asset (MYSQL * mysql, char *row[], ASSET_LINK asset)
{
  unsigned int num_rows;
  char query_asset[QUERY_MAX_SIZE];
  MYSQL_RES *result = NULL;
  MYSQL_ROW row_asset;
  ASSET_LINK new = NULL;
  ASSET_LINK tmp = NULL;
  ASSET_LINK prev = NULL;
  struct in_addr ip;

  ip.s_addr = inet_addr (row[0]);

  new = (ASSET_LINK) malloc (sizeof (OSSIM_ASSET));
  if (!new)
    {
      printf ("Unable to malloc\n");
      exit (1);
    }

  snprintf (query_asset, QUERY_MAX_SIZE,
	    "select * from host_sensor_reference where host_ip = '%s'",
	    row[0]);
  if (mysql_query (mysql, query_asset))
    {
      /* query failed */
      fprintf (stderr, "Failed to make query %s\n%s\n",
	       query_asset, mysql_error (mysql));

    }
  else
    {
      /* query succeeded */
      if ((result = mysql_store_result (mysql)) &&
	  (num_rows = mysql_num_rows (result)))
	{
	  if ((row_asset = mysql_fetch_row (result)))
	    {
	      strncpy (new->sensors, row_asset[1], MAX_SENSORS - 1);
	    }
	  mysql_free_result (result);
	}
    }


  new->ip = ip;
  new->asset = atoi (row[2]);
  new->next = NULL;

  /* Let's do some real work */
  /* Learnt many years ago with "Teach yourself C in 21 days" */
  /* Order by ip */

  if (asset == NULL)
    {				/* Err.. first ? */
      asset = new;
      new->next = NULL;
    }
  else				/* No. */
    {
      if (new->ip.s_addr < asset->ip.s_addr)
	{
	  new->next = asset;
	  asset = new;
	}
      else
	{
	  tmp = asset->next;
	  prev = asset;
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
  return (asset);
}

int
get_asset (ASSET_LINK asset, char *ip)
{
  ASSET_LINK curr;

  curr = asset;
  while (curr != NULL)
    {
      if (strstr (inet_ntoa (curr->ip), ip))
	{
	  return curr->asset;
	}
      curr = curr->next;
    }
  return MED_ASSET;
}


void
show_assets (ASSET_LINK asset)
{
  ASSET_LINK curr;

  curr = asset;
  while (curr != NULL)
    {
      printf ("IP: %s\n", inet_ntoa (curr->ip));
      printf ("\tAsset: %u\n", curr->asset);
      printf ("\tSensors: %s\n", curr->sensors);
      curr = curr->next;
    }
}

void
free_assets (ASSET_LINK asset)
{
  ASSET_LINK curr, next;
  curr = asset;

  while (curr != NULL)
    {
      next = curr->next;
      free (curr);
      curr = next;
    }
}
