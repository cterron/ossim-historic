# -*- coding: utf-8 -*-
#
#  License:
#
#  Copyright (c) 2015 AlienVault
#  All rights reserved.
#
#  This package is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; version 2 dated June, 1991.
#  You may not use, modify or distribute this program under any other version
#  of the GNU General Public License.
#
#  This package is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this package; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
#  MA  02110-1301  USA
#
#
#  On Debian GNU/Linux systems, the complete text of the GNU General
#  Public License can be found in `/usr/share/common-licenses/GPL-2'.
#
#  Otherwise you can read it here: http://www.gnu.org/licenses/gpl-2.0.txt
##

import os
import time
import ast
import yaml
import hashlib
import redis
from celery.utils.log import get_logger
from celery.task.control import inspect
from celerymethods import celeryconfig
from celerybeatredis.schedulers import PeriodicTask
from celery.task.control import revoke
from ansiblemethods.system.system import ansible_get_process_pid
logger = get_logger("celery")


class JobResult():
    def __init__(self, result, message, log_file="", error_id="0"):
        self.result = result
        self.message = message
        self.log_file = log_file
        self.error_id = error_id

    @property
    def serialize(self):
        return {'result': self.result, 'message': self.message,
                'log_file': self.log_file, 'error_id': self.error_id}


def exist_task_running(task_type, current_task_request, param_to_compare=None, argnum=0):
    """
    Check if there is any task of type 'task_type' running for the
    given system.

    If the param_to_compare is None, returns only if there is a task
    of type <task_type> running.

    In order to find a task running in a <param_to_compare>, you should
    specify which argument of the task represent the <param_to_compare>.

    For example:
    If the task was launched by running:
    alienvault_reconfigure("192.168.5.134") -> args[0] will be
    system ip, so you should specify argnum=0

    Args:
        task_type (str): The kind of task to look for (usually the method
                         name)
        current_task_request(): The current task request (current_task.request
                                from the caller)
        param_to_compare (str or None): Parameter to compare whithin the task,
                                        for example the system ip or
                                        the system id.
        argnum (int): The argument number where we can find the system ip
                      if needed.
    Returns:
        rt (True or False): True when a task matching the given criteria
                            is running, false otherwise.
    """
    try:
        # Get the current task_id
        ## alienvault_reconfigure.request.id
        current_task_id = current_task_request.id
        i = inspect()
        current_task_start_time = time.time()
        task_list = []
        # Retrieve the list of active tasks.
        active_taks = i.active()
        for node, tasks_list in active_taks.iteritems():
            for task in tasks_list:
                # Is this task of the given type?
                if task['id'] == current_task_id:
                    current_task_start_time = float(task['time_start'])
                if task['name'].find(task_type) > 0:
                    task_list.append(task)

        previous_task_running = False
        for task in task_list:
            # 1 - Is my own task?
            if task['id'] == current_task_id:
                continue
            task_start_time = task['time_start']
            # 2 - if not, Does the task started before the current one?
            started_before_the_current_one = (task_start_time != current_task_start_time) and \
                                             (task_start_time < current_task_start_time)
            if started_before_the_current_one and \
               param_to_compare is None:  # An existing task is running
                previous_task_running = True
                break

            # 3 - Does the task running in the same system?
            task_param_value = ast.literal_eval(task['args'])[argnum]
            if str(task_param_value) == str(param_to_compare) and \
               started_before_the_current_one:
                previous_task_running = True
                break

        if previous_task_running:
            info_msg = "A %s is running" % task_type + \
                       "....waiting [%s]" % current_task_id
            logger.info(info_msg)

    except Exception, e:
        logger.error("An error occurred %s" % (str(e)))
        return True
    return previous_task_running


def find_task_in_worker_list(celery_task_list, my_task):
    """
    Check if there is any task of type 'type' running or pending
    in a worker list.

    The celery_task_list corresponds to the list given from the call:
        i = inspect()
        i.active().values() or
        i.scheduled().values() or
        i.reserved().values()

    The task has the following format:
        {'task': <name of the celery task>,
         'process': <name of the process>,
         'param_value': <task condition>,
         'param_argnum': <position of the condition>}

    If the 'param_value' is None, it returns only if there is a task of type
    'task' found within the given list.

    In order to find a task running in a <param_value>, you should specify
    which argument from the task represents the <param_value>.

    For example:
    If the task was launched by running:
    alienvault_reconfigure("192.168.5.134") -> args[0] will be
    the system ip, so you should specify 'param_argnum':0

    Args:
        celery_task_list (list) : The list of task from a worker list
        my_task (dict)          : The task we want to look for.

    Returns:
        success (bool) : True if the task was found in the list,
                         False otherwise
        job_id (str)   : Job ID of the task

    """
    success = False
    job_id = 0

    for tasks_list in celery_task_list:
        for task in tasks_list:
            if task['name'].find(my_task['task']) > 0:
                if my_task['param_value'] is None:
                    success = True
                    job_id = task['id']
                    break
                else:
                    try:
                        task_param_value = ast.literal_eval(task['args'])[my_task['param_argnum']]
                    except IndexError:
                        task_param_value = ''

                    if str(task_param_value) == str(my_task['param_value']):
                        success = True
                        job_id = task['id']
                        break

    return success, job_id


def get_task_status(system_id, system_ip, task_list):
    """
    Check if there is any task within the 'task_list' running or pending
    for the given system.

    The format of the list of tasks to check is the following:
    {
        <Name of the task>: {'task': <name of the celery task>,
                             'process': <name of the process>,
                             'param_value': <task condition>,
                             'param_argnum': <position of the condition>}
    }

    Args:
        system_id (str) : The system_id where you want to check
                          if it's running
        system_ip (str) : The system_ip where you want to check
                          if it's running
        task_list (dict): The list of tasks to check.

    Returns:
        success (bool) : True if successful, False otherwise
        result (dict)  : Dic with the status and the job id for each task.

    """
    result = {}

    try:
        i = inspect()
        # Retrieve the list of active tasks.
        active = i.active()
        pending = i.scheduled()
        reserved = i.reserved()
        running_tasks = []
        pending_tasks = []
        if active is not None:
            running_tasks = active.values()
        if pending is not None:
            pending_tasks = pending.values()
        if reserved is not None:
            pending_tasks.extend(reserved.values())
        # Retrieve the list of pending tasks.
    except Exception, e:
        error_msg = "[celery.utils.get_task_status]: An error occurred: " + \
                    "%s" % (str(e))
        logger.error(error_msg)
        return False, {}

    # For each task we are going to check its status
    for name, my_task in task_list.iteritems():
        # Default status is not running
        result[name] = {"job_id": 0, "job_status": "not_running"}

        # Is the task in the list of active tasks?
        success, job_id = find_task_in_worker_list(running_tasks, my_task)
        if success:
            result[name]['job_status'] = "running"
            result[name]['job_id'] = job_id
            continue

        # Is the task in the list of pending tasks?
        success, job_id = find_task_in_worker_list(pending_tasks, my_task)
        if success:
            result[name]['job_status'] = "pending"
            result[name]['job_id'] = job_id
            continue

        # Is the task process running?
        success, pid = ansible_get_process_pid(system_ip, my_task['process'])
        if success:
            result[name]['job_status'] = "running" if pid > 0 else "not_running"
            result[name]['job_id'] = pid
            continue
        else:
            warning_msg = "Cannot retrieve the process pid %s: " % success + \
                          "%s" % pid
            logger.warning(warning_msg)
            return False, {}

    return True, result


def add_task_to_db(task, signature, prefix=''):
    """
    Adds a new scheduled task to the database, using a dictionary.
    """
    if type(task) != dict:
        return (False, "Configuration datatype is not a dictionary")

    try:
        task['name'] = prefix + task['name'] + ':' + signature
        ptask = PeriodicTask.from_dict(task)
        ptask.save()
    except Exception as e:
        error_msg = "Cannot insert new scheduled task " + \
                    "'%s': %s" % (str(task['task']), str(e))

        return (False, error_msg)

    return (True, '')


def restore_tasks_to_db():
    """
    Set the initial tasks in the database
    (i.e. Redis) from configuration files.
    """

    # Check first if everything is in place.
    default_tasks_file = getattr(celeryconfig,
                                 'CELERY_REDIS_SCHEDULER_DEFAULT_TASKS_FILE',
                                 None)
    custom_tasks_file = getattr(celeryconfig,
                                'CELERY_REDIS_SCHEDULER_CUSTOM_TASKS_FILE',
                                None)
    if not (default_tasks_file and os.path.isfile(default_tasks_file)):
        return (False, 'Default tasks file does not exist')
    if not (custom_tasks_file and os.path.isfile(custom_tasks_file)):
        return (False, 'Custom tasks file does not exist')

    # Purge old entries.
    redis_sched_url = getattr(celeryconfig,
                              'CELERY_REDIS_SCHEDULER_URL')
    redis_proto = redis.StrictRedis.from_url(redis_sched_url)
    redis_proto.flushdb()

    # Get the new ones from our files and add them to the database.
    prefix = getattr(celeryconfig, 'CELERY_REDIS_SCHEDULER_KEY_PREFIX')
    default_tasks = []
    default_task_prefix = prefix + ':default:'
    custom_tasks = []
    custom_task_prefix = prefix + ':custom:'
    try:
        with open(default_tasks_file, 'r') as f:
            content = yaml.load(f.read())
            default_tasks = content if content else []
        with open(custom_tasks_file, 'r') as f:
            content = yaml.load(f.read())
            custom_tasks = content if content else []
    except Exception, e:
        return (False, 'Cannot parse task YAML file: %s' % str(e))

    result, info = True, ''
    for task in default_tasks:
        signature = task.pop('signature')
        partial, msg = add_task_to_db(task,
                                      signature,
                                      prefix=default_task_prefix)
        result &= partial
        info = ';'.join([msg, info]) if msg else info
    for task in custom_tasks:
        signature = task.pop('signature')
        partial, msg = add_task_to_db(task,
                                      signature,
                                      prefix=custom_task_prefix)
        result &= partial
        info = ';'.join([msg, info]) if msg else info

    return (result, info)


def set_task_config(name, task, args, kwargs, interval=None, crontab=None, enabled=False, kind=''):
    """
    Set a celery task configuration, both in a file and the Redis db.
    """
    crs_def_tasks_file = getattr(celeryconfig,
                                 'CELERY_REDIS_SCHEDULER_DEFAULT_TASKS_FILE',
                                 None)
    crs_cus_tasks_file = getattr(celeryconfig,
                                 'CELERY_REDIS_SCHEDULER_CUSTOM_TASKS_FILE',
                                 None)
    crs_key_prefix = getattr(celeryconfig,
                             'CELERY_REDIS_SCHEDULER_KEY_PREFIX')

    conffiles = {'default': crs_def_tasks_file,
                 'custom': crs_cus_tasks_file}
    prefixes = {'default': crs_key_prefix + ':default:',
                'custom': crs_key_prefix + ':custom:'}

    if kind not in conffiles:
        error_msg = "Trying to configure a task that is " + \
                    "neither 'default', nor 'custom'"
        return (False, error_msg)
    if not (interval or crontab):
        error_msg = "Tasks need either an 'interval' or 'crontab' " + \
                    "time schedule configuration"
        return (False, error_msg)

    task = {'name': name,
            'task': task,
            'args': args,
            'kwargs': kwargs,
            'enabled': enabled}
    if interval:
        timer = task['interval'] = interval
    else:
        timer = task['crontab'] = crontab
    task['signature'] = hashlib.sha256(str(task['task']) +
                                       str(timer) +
                                       str(task['args']) +
                                       str(task['kwargs'])).hexdigest()

    with open(conffiles[kind], 'r') as f:
        file_content = yaml.load(f.read())

    if file_content:
        file_content = filter(lambda x: x['signature'] != task['signature'],
                              file_content)
    file_content.append(task)

    with open(conffiles[kind], 'w') as f:
        yaml.dump(file_content, f, default_flow_style=False)

    signature = task.pop('signature')
    return add_task_to_db(task,
                          signature,
                          prefix=prefixes[kind])


def get_task_config(task, args=None, kwargs=None, interval=None, crontab=None, kind=''):
    """
    Get a celery task configuration, both in a file and the Redis db.
    Return either a list of dicts, if only the task is given,
    or a single task, if all the arguments are filled.
    """
    crs_def_tasks_file = getattr(celeryconfig,
                                 'CELERY_REDIS_SCHEDULER_DEFAULT_TASKS_FILE',
                                 None)
    crs_cus_tasks_file = getattr(celeryconfig,
                                 'CELERY_REDIS_SCHEDULER_CUSTOM_TASKS_FILE',
                                 None)
    conffiles = {'default': crs_def_tasks_file,
                 'custom': crs_cus_tasks_file}

    if kind not in conffiles:
        error_msg = "Trying to find a task that is neither 'default'" + \
                    "nor 'custom'"
        return (False, error_msg)

    with open(conffiles[kind], 'r') as f:
        file_content = yaml.load(f.read())

    tasks = []
    if file_content:
        if not (args and kwargs and (interval or crontab)):
            tasks = filter(lambda x: x['task'] == task, file_content)
        else:
            timer = interval if interval else crontab
            signature = hashlib.sha256(str(task) +
                                       str(timer) +
                                       str(args) +
                                       str(kwargs)).hexdigest()
            tasks = filter(lambda x: x['signature'] == signature, file_content)
        if not tasks:
            return (False, "No tasks available")

    return (True, tasks)


def get_running_tasks(system_ip):

    try:
        with open("/tmp/log_system", "a") as f:
            f.write("Aqui llego 3\n")
        i = inspect()
        tasks = i.active()
    except Exception as e:
        error_msg = "[celery.utils.get_running_tasks]: " + \
                    "An error occurred: %s" % (str(e))
        logger.error(error_msg)
        return False, {}
    return (True, tasks)


def stop_running_task(task_id, force=True):
    """Terminates the given task
    Args:
        task_id(str): The task id you want to stop
        force(boolean): You want to force the stop
    """
    try:
        if force:
            revoke(task_id, terminate=True, signal='SIGKILL')
        else:
            revoke(task_id, terminate=True)
    except Exception as e:
        return False, str(e)
    return True, ""