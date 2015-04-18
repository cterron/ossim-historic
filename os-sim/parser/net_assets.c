/* Host->net_asset functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>
#include <time.h>

#include "common.h"

NET_ASSET_LINK
load_net_assets (MYSQL * mysql)
{
  MYSQL_RES *result;
  MYSQL_ROW row;
  unsigned int num_rows;
  NET_ASSET_LINK head = NULL;
  char *query = "select * from net;";

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
	      head = add_net_asset (row, head);
	    }
	  mysql_free_result (result);
	}
    }
  return head;
}

void
get_net_mask (char *net, struct in_addr *ip[MAX_HOSTS], int *mask, int *type)
{

  if (strstr (net, "-"))
    {
// parse net and set *ip[]=hosts, mask = 0;
    }
  if (strstr (net, ","))
    {
// parse net and set *ip[]=hosts, mask = 0;
    }
  if (strstr (net, "/"))
    {
// parse net and return  *ip[0] = host, mask = mask;
    }

}

NET_ASSET_LINK
add_net_asset (char *row[], NET_ASSET_LINK net_asset)
{
  NET_ASSET_LINK new = NULL;
  NET_ASSET_LINK tmp = NULL;
  NET_ASSET_LINK prev = NULL;

  new = (NET_ASSET_LINK) malloc (sizeof (OSSIM_NET_ASSET));
  if (!new)
    {
      printf ("Unable to malloc\n");
      exit (1);
    }

  strncpy (new->net_name, row[0], MAX_NET_NAME - 1);
//  new->ips = extract_ip_part(row[1]);
//  new->mask = extract_mask_part(row[1]);
  new->asset = atoi (row[2]);
  new->next = NULL;

  /* Let's do some real work */
  /* Learnt many years ago with "Teach yourself C in 21 days" */
  /* Order by ip */

  if (net_asset == NULL)
    {				/* Err.. first ? */
      net_asset = new;
      new->next = NULL;
    }
  else				/* No. */
    {
      if (new->net_name < net_asset->net_name)
	{
	  new->next = net_asset;
	  net_asset = new;
	}
      else
	{
	  tmp = net_asset->next;
	  prev = net_asset;
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
  return (net_asset);
}

int
get_net_asset (NET_ASSET_LINK net_asset, char *net_name)
{
  NET_ASSET_LINK curr;

  curr = net_asset;
  while (curr != NULL)
    {
      if (strstr (curr->net_name, net_name))
	{
	  return curr->asset;
	}
      curr = curr->next;
    }
  return MED_ASSET;
}


void
show_net_assets (NET_ASSET_LINK net_asset)
{
  NET_ASSET_LINK curr;

  curr = net_asset;
  while (curr != NULL)
    {
      printf ("Network: %s\n", curr->net_name);
      printf ("\tAsset: %u\n", curr->asset);
      curr = curr->next;
    }
}

void
free_net_assets (NET_ASSET_LINK net_asset)
{
  NET_ASSET_LINK curr, next;
  curr = net_asset;

  while (curr != NULL)
    {
      next = curr->next;
      free (curr);
      curr = next;
    }
}
