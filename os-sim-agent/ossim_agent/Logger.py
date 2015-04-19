#
# Static class for logging purposes
#
# Use from other classes:
#
#   from Logger import Logger
#   logger = Logger.logger
#
#   logger.debug("Some debug")
#   logger.info("Some info")
#   logger.error("Error")
#
# More info at http://docs.python.org/lib/module-logging.html
#

import string, logging, os, sys

class Logger:

    logger = logging.getLogger('agent')
    logger.setLevel(logging.INFO)

    __formatter = logging.Formatter('%(asctime)s %(module)s [%(levelname)s]: %(message)s')

    __streamhandler = logging.StreamHandler()
    __streamhandler.setFormatter(__formatter)
    logger.addHandler(__streamhandler)


    # log to file (file should be log->file in configuration)
    def add_file_handler(file):

        dir = file.rstrip(os.path.basename(file))
        if not os.path.isdir(dir):
            try:
                os.mkdir(dir, 0755)
            except OSError, e:
                print "Logger: Error adding file handler,", \
                    "can not create log directory (%s): %s" % (dir, e)
                return

        try:
            handler = logging.FileHandler(file)
        except IOError, e:
            print "Logger: Error adding file handler: %s" % (e)
            return

        handler.setFormatter(Logger.__formatter)
        Logger.logger.addHandler(handler)

    add_file_handler = staticmethod(add_file_handler)


    # Removes the stream handler
    # (Useful when agent starts in daemon mode)
    def remove_console_output():
        Logger.logger.removeHandler(Logger.__streamhandler)
    remove_console_output = staticmethod(remove_console_output)


    # show DEBUG messages or not
    def set_verbose(verbose = 'info'):
        if verbose.lower() == 'debug':
            Logger.logger.setLevel(logging.DEBUG)
        elif verbose.lower() == 'info':
            Logger.logger.setLevel(logging.INFO)
        elif verbose.lower() == 'warning':
            Logger.logger.setLevel(logging.WARNING)
        elif verbose.lower() == 'error':
            Logger.logger.setLevel(logging.ERROR)
        elif verbose.lower() == 'critical':
            Logger.logger.setLevel(logging.CRITICAL)
        else:
            Logger.logger.setLevel(logging.INFO)

    set_verbose = staticmethod(set_verbose)


if __name__ == "__main__":

    logger = Logger.logger
    logger.debug("Some debug text")
    logger.info("Some info text")
    logger.critical("Oppps, error")

    # now logs to file
    Logger.add_file_handler('/tmp/ossim.log')
    logger.critical("log to file")


# vim:ts=4 sts=4 tw=79 expandtab:
