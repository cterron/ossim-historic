/* Parser fw-1 rules */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"


int
getportbyservice (char *service)
{

  FILE *fd;
  char *fw1_service;
  int port = 0;

  fw1_service = (char *) malloc (sizeof (char) * 128);

  if (NULL == (fd = fopen (SERVICES_FILE, "r")))
    {
      printf ("Can't open file %s\n", SERVICES_FILE);
      exit (-1);
    }

  while (!feof (fd))
    {
      fscanf (fd, "%128s", fw1_service);
      if (!strcmp (fw1_service, service))
	{
	  fscanf (fd, "%d", &port);
	}
    }

  free (fw1_service);
  fclose (fd);

  return port;
}


int
insert_fw1_alert (unsigned long int source_ip,
		  unsigned int source_port,
		  unsigned long int dest_ip,
		  unsigned int dest_port,
		  char *service,
		  char *protocol,
		  char *action, char *sensor_fw, unsigned int rule)
{
  MYSQL mysql;
  char *query_snort;
  char *query_acid;
  unsigned long int sensor_id;
  unsigned long int sig_id;
  unsigned long int sig_class_id;
  char *new_sig_name;

  query_snort = (char *) malloc (sizeof (char) * QUERY_MAX_SIZE);
  query_acid = (char *) malloc (sizeof (char) * QUERY_MAX_SIZE);
  new_sig_name = (char *) malloc (sizeof (char) * QUERY_MAX_SIZE);

  /* establish a connection to a MySQL database engine */
  mysql_init (&mysql);
  mysql_options (&mysql, MYSQL_READ_DEFAULT_GROUP, "parser_syslog");
  if (!mysql_real_connect (&mysql,
			   get_conf ("snort_host"),
			   get_conf ("snort_user"),
			   get_conf ("snort_pass"),
			   get_conf ("snort_base"), 0, NULL, 0))
    {
      fprintf (stderr, "Failed to connect to database: Error: %s\n",
	       mysql_error (&mysql));
    }

  /* get sensor id */
  get_sensor_id (&mysql, sensor_fw, &sensor_id);

  /* get sig_class_id from sig_class table */
  get_sig_class_id (&mysql, &sig_class_id);

  /* get sig_id from signature table */
  get_sig_id (&mysql, action, &sig_id);

  /* build sig_name */
  snprintf (new_sig_name, QUERY_MAX_SIZE,
	    "FireWall-1: (%s) %lu-%d -> %lu-%d, %s-%s, rule: %u",
	    action, source_ip, source_port, dest_ip, dest_port,
	    service, protocol, rule);

  /* insert alert */
  insert_event (&mysql, sensor_id, sig_id, sig_class_id, new_sig_name,
		source_ip, dest_ip, source_port, dest_port, protocol);

  free (query_snort);
  free (query_acid);
  free (new_sig_name);

  mysql_close (&mysql);

  return 1;
}
