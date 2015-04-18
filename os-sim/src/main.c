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

#include <config.h>

#define _GNU_SOURCE
#include <getopt.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <libgda/libgda.h>
#include <os-sim.h>

#include "sim-scheduler.h"
#include "sim-organizer.h"
#include "sim-session.h"
#include "sim-server.h"
#include "sim-xml-config.h"

typedef struct {
  gchar          *config;
  gboolean        daemon;
  gint            debug;
} SimCmdArgs;

/* Globals Variables */
SimMain        ossim;

SimCmdArgs simCmdArgs;

/*
 *
 *
 *
 *
 */
static void
sim_log_handler (const gchar     *log_domain,
		 GLogLevelFlags   log_level,
		 const gchar     *message,
		 gpointer         data)
{
  gchar   *msg;

  g_return_if_fail (message);
  g_return_if_fail (ossim.log.fd);

  if (ossim.log.level < log_level)
    return;

  switch (log_level)
    {
      case G_LOG_LEVEL_ERROR:
	msg = g_strdup_printf ("%s-Error: %s\n", log_domain, message);
	write (ossim.log.fd, msg, strlen(msg));
	g_free (msg);
	break;
      case G_LOG_LEVEL_CRITICAL:
	msg = g_strdup_printf ("%s-Critical: %s\n", log_domain, message);
	write (ossim.log.fd, msg, strlen(msg));
	g_free (msg);
	break;
      case G_LOG_LEVEL_WARNING:
	msg = g_strdup_printf ("%s-Warning: %s\n", log_domain, message);
	write (ossim.log.fd, msg, strlen(msg));
	g_free (msg);
	break;
      case G_LOG_LEVEL_MESSAGE:
	msg = g_strdup_printf ("%s-Message: %s\n", log_domain, message);
	write (ossim.log.fd, msg, strlen(msg));
	g_free (msg);
	break;
      case G_LOG_LEVEL_INFO:
	msg = g_strdup_printf ("%s-Info: %s\n", log_domain, message);
	write (ossim.log.fd, msg, strlen(msg));
	g_free (msg);
	break;
      case G_LOG_LEVEL_DEBUG:
	msg = g_strdup_printf ("%s-Debug: %s\n", log_domain, message);
	write (ossim.log.fd, msg, strlen(msg));
	g_free (msg);
	break;
    }
}

/*
 * sim_container_scheduler:
 *
 *   arguments:
 *
 *   results:
 */
static gpointer
sim_thread_scheduler (gpointer data)
{
  g_message ("sim_thread_scheduler");

  ossim.scheduler = sim_scheduler_new (ossim.config);
  sim_scheduler_run (ossim.scheduler);

  return NULL;
}

/**
 * sim_container_organizer:
 *
 *   arguments:
 *
 *   results:
 */

static gpointer
sim_thread_organizer (gpointer data)
{
  g_message ("sim_thread_organizer");

  ossim.organizer = sim_organizer_new (ossim.config);
  sim_organizer_run (ossim.organizer);

  return NULL;
}

/**
 * sim_container_organizer:
 *
 *   arguments:
 *
 *   results:
 */

static gpointer
sim_thread_server (gpointer data)
{
  g_message ("sim_thread_server");

  ossim.server = sim_server_new (ossim.config);
  sim_server_run (ossim.server);

  return NULL;
}

/*
 *
 *
 *
 */
static void
options (int argc, char **argv)
{
  int c;
  int digit_optind = 0;

  /* Default Command Line Options */
  simCmdArgs.config = NULL;
  simCmdArgs.daemon = FALSE;
  simCmdArgs.debug = 4;

  while (TRUE)
    {
      int this_option_optind = optind ? optind : 1;
      int option_index = 0;
      static struct option options[] =
	{
	  {"config", 1, 0, 'c'},
	  {"daemon", 0, 0, 'd'},
	  {"debug", 0, 0, 'D'},
	  {0, 0, 0, 0}
	};
      
      c = getopt_long (argc, argv, "dc:D:", options, &option_index);
      if (c == -1)
	break;

      switch (c)
	{
	case 'c':
	  simCmdArgs.config = g_strdup (optarg);
	  break;

	case 'd':
	  simCmdArgs.daemon = TRUE;
	  break;
	  
	case 'D':
	  simCmdArgs.debug = strtol (optarg, (char **)NULL, 10);
	  break;
	  
	case '?':
	  break;
	  
	default:
	  g_print ("?\? getopt() return the caracter %c ?\?\n", c);
	}
    }

  if (optind < argc)
    {
      g_print ("Elements from ARGV are not option: ");
      while (optind < argc)
	g_print ("%s ", argv[optind++]);
      g_print ("\n");
    }

  if ((simCmdArgs.config) && !g_file_test (simCmdArgs.config, G_FILE_TEST_EXISTS))
    g_error ("Config XML File %s: Don't exists", simCmdArgs.config);
  
  if ((simCmdArgs.debug < 0) || (simCmdArgs.debug > 6))
    g_error ("Debug level %d: Is invalid", simCmdArgs.debug);

  if (simCmdArgs.daemon) 
    {
      if (fork ())
	exit (0);
      else
	;
    }
}

/*
 *
 *
 *
 */
sim_log_init (void)
{
  /* Init */
  ossim.log.filename = NULL;
  ossim.log.fd = 0;
  ossim.log.level = G_LOG_LEVEL_MESSAGE;

  /* File Logs */

  if (ossim.config->log.filename)
    {
      ossim.log.filename = g_strdup (ossim.config->log.filename);
    }
  else
    {
      /* Verify Directory */
      if (!g_file_test (OS_SIM_LOG_DIR, G_FILE_TEST_IS_DIR))
	g_error ("Log Directory %s: Is invalid", OS_SIM_LOG_DIR);
      
      ossim.log.filename = g_strdup_printf ("%s/%s", OS_SIM_LOG_DIR, SIM_LOG_FILE);
    }

  if ((ossim.log.fd = creat (ossim.log.filename, S_IWUSR|S_IRUSR|S_IRGRP|S_IROTH)) < 0)
    g_error ("Log File %s: Can't create", ossim.log.filename);
  
  switch (simCmdArgs.debug)
    {
    case 0:
      ossim.log.level = 0;
      break;
    case 1:
      ossim.log.level = G_LOG_LEVEL_ERROR;
      break;
    case 2:
      ossim.log.level = G_LOG_LEVEL_CRITICAL;
      break;
    case 3:
      ossim.log.level = G_LOG_LEVEL_WARNING;
      break;
    case 4:
      ossim.log.level = G_LOG_LEVEL_MESSAGE;
      break;
    case 5:
      ossim.log.level = G_LOG_LEVEL_INFO;
      break;
    case 6:
      ossim.log.level = G_LOG_LEVEL_DEBUG;
      break;
    }

  /* Log Handler */
  g_log_set_handler (NULL, G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL
		     | G_LOG_FLAG_RECURSION, sim_log_handler, NULL);

  g_log_set_handler ("GLib", G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL
		     | G_LOG_FLAG_RECURSION, sim_log_handler, NULL);

  g_log_set_handler (G_LOG_DOMAIN, G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL
		     | G_LOG_FLAG_RECURSION, sim_log_handler, NULL);
}

/*
 *
 *
 *
 */
sim_log_free (void)
{
  g_free (ossim.log.filename);
  close (ossim.log.fd);
}

/*
 *
 *
 *
 */
int
main (int argc, char *argv[])
{
  SimXmlConfig	*xmlconfig;
  GMainLoop	*loop;
  GThread	*thread;
  SimConfigDS	*ds;

  /* Global variable OSSIM Init */
  ossim.config = NULL;
  ossim.container = NULL;
  ossim.server = NULL;
  ossim.dbossim = NULL;
  ossim.dbsnort = NULL;

  /* Command Line Options */
  options (argc, argv);

  /* Thread Init */
  if (!g_thread_supported ())
    g_thread_init (NULL);

  /* GDA Init */
  gda_init (NULL, "0.9.6", argc, argv);

  /* Config Init */
  if (simCmdArgs.config)
    {
      if (!(xmlconfig = sim_xml_config_new_from_file (simCmdArgs.config)))
	g_error ("Config XML File %s is invalid", simCmdArgs.config);
      
      if (!(ossim.config = sim_xml_config_get_config (xmlconfig)))
	g_error ("Config is %s invalid", simCmdArgs.config);
    }
  else
    {
      if (!g_file_test (OS_SIM_GLOBAL_CONFIG_FILE, G_FILE_TEST_EXISTS))
	g_error ("Config XML File %s: Not Exists", OS_SIM_GLOBAL_CONFIG_FILE);
      
      if (!(xmlconfig = sim_xml_config_new_from_file (OS_SIM_GLOBAL_CONFIG_FILE)))
	g_error ("Config XML File %s is invalid", OS_SIM_GLOBAL_CONFIG_FILE);
      
      if (!(ossim.config = sim_xml_config_get_config (xmlconfig)))
	g_error ("Config %s is invalid", OS_SIM_GLOBAL_CONFIG_FILE);
    }

  /* Database Options */
  ds = sim_config_get_ds_by_name (ossim.config, SIM_DS_OSSIM);
  if (!ds)
    g_error ("OSSIM DB XML Config");
  ossim.dbossim = sim_database_new (ds);

  ds = sim_config_get_ds_by_name (ossim.config, SIM_DS_SNORT);
  if (!ds)
    g_error ("SNORT DB XML Config");
  ossim.dbsnort = sim_database_new (ds);

  ossim.mutex_directives = g_mutex_new ();
  ossim.mutex_backlogs = g_mutex_new ();

  /* Log Init */
  sim_log_init ();

  /* Container init */
  ossim.container = sim_container_new (ossim.config);

  /* Scheduler Thread */
  thread = g_thread_create (sim_thread_scheduler, NULL, TRUE, NULL);
  g_return_if_fail (thread);
  g_thread_set_priority (thread, G_THREAD_PRIORITY_NORMAL);

  /* Organizer Thread */
  thread = g_thread_create (sim_thread_organizer, NULL, TRUE, NULL);
  g_return_if_fail (thread);
  g_thread_set_priority (thread, G_THREAD_PRIORITY_NORMAL);

  /* Server Thread */
  thread = g_thread_create (sim_thread_server, NULL, TRUE, NULL);
  g_return_if_fail (thread);
  g_thread_set_priority (thread, G_THREAD_PRIORITY_NORMAL);

  /* Main Loop */
  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  /* Log Free */
  sim_log_free ();

  return 0;
}
