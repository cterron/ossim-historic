/* Parse, sum, update levels */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mysql.h>
//#include <time.h>

#include "common.h"

extern POL_LINK policy;
extern HOST_LINK hosts;
extern ASSET_LINK assets;
extern NET_LINK nets;
extern NET_ASSET_LINK net_assets;
extern time_t global_time;
extern time_t *global_time_loc;
extern int update_interval;

static unsigned int
get_time (char *format)
{

  struct tm *today;
  int digits = 2;
  char date[digits + 1];
  time_t ltime;

  time (&ltime);
  today = localtime (&ltime);
  strftime (date, digits + 1, format, today);

  return atoi (date);
}

/* 
 * Return current hour in 24-hour format (00 - 23) 
 */
static unsigned int
get_hour ()
{

  return get_time ("%H");
}

/* 
 * Return the  day of the week as a decimal, 
 * range 1 to 7, Monday being 1 
 */
static unsigned int
get_day ()
{

  return get_time ("%u");
}

static int
is_attack_response (int plugin, int tplugin)
{

  FILE *fd;
  int sid;
  int is_attack_response = 0;

  if (plugin == 1)
    {
      if (NULL == (fd = fopen (ATTACK_RESPONSES_SIDS_FILE, "r")))
	{
	  printf ("Can't open file %s\n", ATTACK_RESPONSES_SIDS_FILE);
	  exit (-1);
	}
      while (!feof (fd))
	{
	  fscanf (fd, "%d", &sid);
	  if (sid == tplugin)
	    {
	      is_attack_response = 1;
	      break;
	    }
	}
      fclose (fd);
    }

  return is_attack_response;
}


static char *
get_signature (int sid)
{

  FILE *fd;
  int sf = 0;
  char *sig;

  sig = (char *) malloc (sizeof (char) * SIGNATURE_MAX_SIZE);

  if (NULL == (fd = fopen (SIDS_FILE, "r")))
    {
      printf ("Can't open file %s\n", SIDS_FILE);
      exit (-1);
    }

  while (!feof (fd))
    {
      fscanf (fd, "%d", &sf);
      fscanf (fd, "%64s", sig);
      if (sid == sf)
	break;
    }
  fclose (fd);


  return sig;
}


void
calculate (MYSQL * mysql, int plugin, int tplugin,
	   unsigned int priority_snort,
	   char protocol[5], char source_ip[16], char dest_ip[16],
	   int source_port, int dest_port)
{
  int is_atckrsp = 0;
  char *signature;
  int priority = 1;
  char port[10];
  int source_asset = 0, dest_asset = 0;
  int impactC = 0, impactA = 0;
  int sourceC = 0, destA = 0, destC = 0;
  char *row[3];
  int recovery = 3;
  char *query = "SELECT recovery FROM conf;";
  MYSQL_RES *result;
  MYSQL_ROW conf_row;
  unsigned int num_rows;




  /*
   * get current day and current hour
   * calculate date expresion to be able to compare dates
   * 
   * for example, Fri 21h = ((5 - 1) * 7) + 21 = 49
   *              Sat 14h = ((6 - 1) * 7) + 14 = 56
   */
  unsigned int hour = get_hour ();
  unsigned int day = get_day ();
  unsigned int date_expr = ((day - 1) * 7) + hour;

  if (mysql_query (mysql, query))
    {
      fprintf (stderr, "Failed to make query: %s\n%s\n",
	       query, mysql_error (mysql));
    }
  else
    {
      /* query succesded */
      if ((result = mysql_store_result (mysql)) &&
	  (num_rows = mysql_num_rows (result)))
	{
	  conf_row = mysql_fetch_row (result);
	  recovery = atoi (conf_row[0]);
	  mysql_free_result (result);
	}
    }

  if ((time (global_time_loc) - update_interval) >= global_time)
    {
      hosts = lower_hosts (hosts, recovery);
      nets = lower_nets (nets, recovery);
      save_nets (mysql, nets);
      global_time = time (global_time_loc);
    }

  /* 
   * is an attack-response?
   */
  is_atckrsp = is_attack_response (plugin, tplugin);

  /*
   * Get snort signature
   */
  if (plugin == GENERATOR_SPP_SPADE)
    {
      signature = (char *) malloc (sizeof (char) * 6);
      sprintf (signature, "spade");

    }
  else if (plugin == GENERATOR_FW1)
    {
      if (tplugin == FW1_ACCEPT_TYPE)
	{
	  signature =
	    (char *) malloc (sizeof (char) * strlen ("fw1-accept") + 1);
	  sprintf (signature, "fw1-accept");
	}
      else if (tplugin == FW1_DROP_TYPE)
	{
	  signature =
	    (char *) malloc (sizeof (char) * strlen ("fw1-drop") + 1);
	  sprintf (signature, "fw1-drop");
	}
      else if (tplugin == FW1_REJECT_TYPE)
	{
	  signature =
	    (char *) malloc (sizeof (char) * strlen ("fw1-reject") + 1);
	  sprintf (signature, "fw1-reject");
	}
      else
	{
	  printf ("Unexpected error getting signature\n");
	  exit (-1);
	}

    }
  else
    {
      signature = get_signature (tplugin);
    }

  sprintf (port, "%d", dest_port);
  if (get_priority
      (source_ip, dest_ip, protocol, port, signature, date_expr, policy,
       &priority))
    {
#ifdef VERBOSE
      printf ("priority at level 1: %d\n", priority);
#endif
    }

  /*
   * Get asset from ips
   */
  if ((source_asset = get_asset (assets, source_ip)))
    impactC = priority * priority_snort * source_asset;
  if ((dest_asset = get_asset (assets, dest_ip)))
    impactA = priority * priority_snort * dest_asset;


#ifdef VERBOSE
  printf ("\nPriority: %d\n", priority);
  printf ("Source asset: %d, Dest asset: %d\n", source_asset, dest_asset);
  printf ("C impact: %d, A impact: %d\n\n", impactC, impactA);
#endif


  /* C level */
  if (get_host_level (hosts, source_ip, 'c', &sourceC))
    {
      update_level (mysql, source_ip, "compromise", impactC);
      update_host_level (hosts, source_ip, 'c', impactC);
      update_nets_level (mysql, source_ip, 'c', impactC);
#ifdef VERBOSE
      printf ("compromise of ip %s is %d\n", source_ip, sourceC + impactC);
#endif
    }
  else
    {
      row[0] = (char *) malloc (strlen (source_ip) + 1);
      strncpy (row[0], source_ip, strlen (source_ip));

      row[1] = (char *) malloc (10);
      snprintf (row[1], 8, "%d", impactC);

      row[2] = (char *) malloc (2);
      strcpy (row[2], "1");

      hosts = add_host (row, hosts);
      free (row[0]);
      free (row[1]);
      free (row[2]);
      insert_level (mysql, source_ip, impactC, 1);
    }

  if (dest_ip);

  /* A level */
  if (get_host_level (hosts, dest_ip, 'a', &destA))
    {
      update_level (mysql, dest_ip, "attack", impactA);
      update_host_level (hosts, dest_ip, 'a', impactA);
      update_nets_level (mysql, dest_ip, 'a', impactA);
#ifdef VERBOSE
      printf ("attack of ip %s is %d\n", dest_ip, destA + impactA);
#endif
    }
  else
    {
      row[0] = (char *) malloc (strlen (dest_ip) + 1);
      strncpy (row[0], dest_ip, strlen (dest_ip));

      row[1] = (char *) malloc (2);
      strcpy (row[1], "1");

      row[2] = (char *) malloc (10);
      snprintf (row[2], 8, "%d", impactA);

      hosts = add_host (row, hosts);
      free (row[0]);
      free (row[1]);
      free (row[2]);

      insert_level (mysql, dest_ip, 1, impactA);
    }

  /* attack-responses */
  if (is_atckrsp)
    {
      if (get_host_level (hosts, dest_ip, 'c', &destC))
	{
#ifdef VERBOSE
	  printf ("ip %s compromise equals %d\n", dest_ip, destC);
#endif
	  update_level (mysql, dest_ip, "compromise", impactC);
	  update_host_level (hosts, dest_ip, 'c', impactC);
	  update_nets_level (mysql, dest_ip, 'c', impactC);
	}
    }
#ifdef VERBOSE
  printf ("\n");
#endif

  free (signature);

}
