import string, sys, time

CONFIG = '/etc/ossim/agent/config.xml'
VERSION = 'OSSIM (Open Source Security Information Management) - Agent 0.9.4'

def debug(module, message, mark = "", color = ""):
    """print debug message"""
   
    msg = ""

    if color:
        if color == 'RED':      msg += "\033[01;31m"
        elif color == 'GREEN':  msg += "\033[01;32m"
        elif color == 'YELLOW': msg += "\033[01;33m"
        elif color == 'BLUE':   msg += "\033[01;34m"
        elif color == 'PURPLE': msg += "\033[01;35m"
        elif color == 'CYAN':   msg += "\033[01;36m"

    if mark:
        msg += " (%s) \033[00m" % (mark)
        
    msg += " %s:\t%s" % (module, message)
        
    print msg
    sys.stdout.flush()

    if VERBOSE:
        sys.__stdout__.write("%s\n" % (msg))
        sys.__stdout__.flush()
        

def debug_watchlog(message, mark = "", color = ""):

    msg = ""

    if color:
        if color == 'RED':      msg += "\033[01;31m"
        elif color == 'GREEN':  msg += "\033[01;32m"
        elif color == 'YELLOW': msg += "\033[01;33m"
        elif color == 'BLUE':   msg += "\033[01;34m"
        elif color == 'PURPLE': msg += "\033[01;35m"
        elif color == 'CYAN':   msg += "\033[01;36m"
            
    if mark:
        msg += " (%s) \033[00m" % (mark)
    
    date = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(time.time()))
   
    msg += date + ' : ' + message + '\n'
   
    watchlog.write(msg)
    watchlog.flush()


def normalizeWhitespace(text):
    "Remove redundant whitespace from a string"
    return string.join(string.split(text), ' ')


#
# pidof
#
# return the process ID of a running program
# if program is not running, return None
#
# TODO: pid read from ps command, any better solution?
#
def pidof(program):
    "find the process ID of a running program"

    import os
    import re

    ps = os.popen('ps axc')
    ps.readline() # skip initial line

    for line in ps:
        try:
            result = re.findall('(\S+)\s+\S+\s+\S+\s+\S+\s+(\S+)', line)
            pid    = result[0][0]
            name   = result[0][1]

            if name == program:
                debug_watchlog (line.strip(), 'PS', '')
                return pid

        # error reading ps
        except IndexError:
            return None

    # no process found
    return None


def pidof2(program):
    "find the process ID of a running program"

    import commands
    
    pid = string.split(commands.getoutput('pidof %s' % program), ' ')
    if pid[0] == '':
        return None
    else:
        return pid[0]

