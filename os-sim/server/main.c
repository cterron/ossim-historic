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

#include <libgda/libgda.h>
#include <ossim.h>

typedef struct {
  SimContainer   *container;
  SimDatabase    *database;
  SimConfig      *config;
} SimThreadData;

/*
 * sim_container_syslog:
 *
 *   arguments:
 *
 *   results:
 */

static gpointer
sim_thread_syslog (gpointer data)
{
  SimThreadData   *thr_data = (SimThreadData *) data;
  SimContainer    *container = thr_data->container;
  SimDatabase     *database = thr_data->database;
  SimConfig       *config = thr_data->config;
  SimSyslog       *syslog;
  gchar           *log_file;
 
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));
  g_return_if_fail (config);
  g_return_if_fail (SIM_IS_CONFIG (config));

  g_message ("sim_thread_syslog");

  log_file = sim_config_get_property_value (config, SIM_CONFIG_PROPERTY_TYPE_LOG_FILE);

  syslog = sim_syslog_new (container, log_file);
  g_assert (syslog != NULL);
  sim_syslog_run (syslog);

  return NULL;
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
  SimThreadData   *thr_data = (SimThreadData *) data;
  SimContainer    *container = thr_data->container;
  SimDatabase     *database = thr_data->database;
  SimConfig       *config = thr_data->config;
  SimScheduler    *scheduler;
 
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  g_message ("sim_thread_scheduler");

  scheduler = sim_scheduler_new (container, database, config);
  sim_scheduler_run (scheduler);

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
  SimThreadData   *thr_data = (SimThreadData *) data;
  SimContainer    *container = thr_data->container;
  SimDatabase     *database = thr_data->database;
  SimOrganizer    *organizer;
 
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  g_message ("sim_thread_organizer");

  organizer = sim_organizer_new (container, database);
  sim_organizer_run (organizer);

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
  SimThreadData   *thr_data = (SimThreadData *) data;
  SimContainer    *container = thr_data->container;
  SimDatabase     *database = thr_data->database;
  SimServer       *server;
 
  g_return_if_fail (container);
  g_return_if_fail (SIM_IS_CONTAINER (container));
  g_return_if_fail (database);
  g_return_if_fail (SIM_IS_DATABASE (database));

  g_message ("sim_thread_server");

  server = sim_server_new (container, database);
  sim_server_run (server);

  return NULL;
}

/*
 *
 *
 *
 */
int
main (int argc, char *argv[])
{
  GMainLoop     *loop;
  SimThreadData *data;
  SimConfig     *config; 
  SimContainer  *container;
  SimDatabase   *database;
  GThread       *thread;
  gchar         *datasource;
  gchar         *username;
  gchar         *password;

  /* Thread Init */
  if (!g_thread_supported ()) 
    {
      g_thread_init (NULL);
    }

  gda_init ("Ossim", "0.1", argc, argv);

  /* Config Init */
  config = sim_config_new (SIM_CONFIG_FILE);

  /* Database init */
  datasource = sim_config_get_property_value (config, SIM_CONFIG_PROPERTY_TYPE_DATABASE);
  username = sim_config_get_property_value (config, SIM_CONFIG_PROPERTY_TYPE_USERNAME);
  password = sim_config_get_property_value (config, SIM_CONFIG_PROPERTY_TYPE_PASSWORD);
  database = sim_database_new (datasource, username, password);

  /* Container init */
  container = sim_container_new ();
  sim_container_db_load_hosts (container, database);
  sim_container_db_load_nets (container, database);
  sim_container_db_load_signatures (container, database);
  sim_container_db_load_policies (container, database);
  sim_container_db_load_host_levels (container, database);
  sim_container_db_load_net_levels (container, database);
  sim_container_load_directives_from_file (container, "directives.xml");

  /* Thread Data Init */
  data = g_new0 (SimThreadData, 1);
  data->container = container;
  data->database = database;
  data->config = config;

  /* Syslog Thread */
  thread = g_thread_create (sim_thread_syslog, data, TRUE, NULL);
  g_return_if_fail (thread);

  /* Scheduler Thread */
  thread = g_thread_create (sim_thread_scheduler, data, TRUE, NULL);
  g_return_if_fail (thread);

  /* Organizer Thread */
  thread = g_thread_create (sim_thread_organizer, data, TRUE, NULL);
  g_return_if_fail (thread);

  /* Server Thread */
  thread = g_thread_create (sim_thread_server, data, TRUE, NULL);
  g_return_if_fail (thread);

  /* Main Loop */
  loop = g_main_loop_new (NULL, FALSE);
  g_main_loop_run (loop);

  return 0;
}
