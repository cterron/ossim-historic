import syslog

class ActionSyslog:

    # TODO: check security
    def __init__(self, prefix):
        syslog.openlog(prefix)

    def logSyslog(self, log_string):
        syslog.syslog(log_string)

if __name__ == "__main__":
    
    c = ActionSyslog("frameworkd")
    c.logSyslog("This is a frameworkd test log message")

# vim:ts=4 sts=4 tw=79 expandtab:
