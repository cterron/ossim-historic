import os, sys, string, re
from ConfigParser import ConfigParser
from optparse import OptionParser
from Exceptions import AgentCritical
from Logger import Logger
logger = Logger.logger
import ParserUtil


class Conf(ConfigParser):

    DEFAULT_CONFIG_FILE = "/etc/ossim/agent/config.cfg"

    # same as ConfigParser.read() but also check
    # if configuration files exists
    def read(self, filenames):
        for filename in filenames:
            if not os.path.isfile(filename):
                AgentCritical("Configuration file (%s) does not exist!" % \
                    (filename))
        ConfigParser.read(self, filenames)

    # avoid confusions in configuration values
    def _strip_value(self, value):
        from string import strip
        return strip(strip(value, '"'), "'")

    # same as ConfigParser.items() but returns a hash instead of a list
    def hitems(self, section):
        hash = {}
        for item in self.items(section):
            hash[item[0]] = self._strip_value(item[1])
        return hash

    # same as ConfigParser.get() but stripping values with " and '
    def get(self, section, option):
        value = ConfigParser.get(self, section, option)
        value = self._strip_value(value)
        # value = string.lower(value)
        return value

    def getboolean(self, section, option):
        try:
            value = ConfigParser.getboolean(self, section, option)
        except ValueError: # not a boolean
            logger.warning("Value %s->%s is not a boolean" % (section, option))
            return False
        return value

    # print a representation of a config object,
    # very useful for debug purposes
    def __repr__(self):
        conf_str = ""
        for section in self.sections():
            conf_str += str(self.hitems(section))
        return conf_str


class Plugin(Conf):

    TRANSLATION_SECTION = 'translation'
    TRANSLATION_FUNCTION = 'translate'
    SECTIONS_NOT_RULES = ["config", "info", TRANSLATION_SECTION]

    def rules(self):
        rules = {}
        for section in self.sections():
            if section.lower() not in Plugin.SECTIONS_NOT_RULES :
                rules[section] = self.hitems(section)

        return rules

    def replace_aliases(self, aliases):

        # iter over all rules
        for rule in self.rules().iterkeys():

            regexp = self.get(rule, 'regexp')

            # look for \X values in regexp entry
            #
            # To match a literal backslash, one has to write '\\\\'
            # as the RE string, because the regular expression must be
            # "\\", and each backslash must be expressed as "\\" inside
            # a regular Python string literal
            #
            search = re.findall("\\\\\w\w+", regexp)

            if search != []:
                for string in search:

                    # replace \X with aliases' X entry
                    repl = string[1:]
                    if aliases.has_option("regexp", repl):
                        value = aliases.get("regexp", repl)
                        regexp = regexp.replace(string, value)
                        self.set(rule, "regexp", regexp)

    # replace config values matching {$X} with self.groups["X"]
    # and {f($X)} with f(self.groups["X"])
    def get_replace_value(self, value, groups):

        # variables are specified as {$v}
        # you can use two-anidated variables: {${$v}}
        for i in range(2):
            search = re.findall("\{\$[^\}\{]+\}", value)
            if search != []:
                for string in search:
                    var = string[2:-1]
                    if groups.has_key(var):
                        value = value.replace(string, str(groups[var]))

        # functions are specified as {f($v)}
        search = re.findall("(\{(\w+)\(\$([^\)]+)\)\})", value)
        if search != []:
            for string in search:
                (string_matched, func, var) = string
                if groups.has_key(var):

                    # special function translate() for translations
                    # translations are defined in the own plugin 
                    # with a [translation] entry
                    if func == Plugin.TRANSLATION_FUNCTION:
                        if self.has_section(Plugin.TRANSLATION_SECTION):
                            if self.has_option(Plugin.TRANSLATION_SECTION,
                                               groups[var]):
                                value = self.get(Plugin.TRANSLATION_SECTION,
                                                 groups[var])
                            else:
                                logger.warning("Unkown variable '%s', can not translate value" % (groups[var]))
                                value = groups[var]
                        else:
                            logger.warning("There is no translation section")
                            value = groups[var]

                    # call function 'func' with arg groups[var]
                    # functions are defined in ParserUtil.py file
                    elif hasattr(ParserUtil, func):
                        f = getattr(ParserUtil, func)
                        value = value.replace(string_matched, \
                                              str(f(groups[var])))

                    else:
                        logger.warning(
                            "Function '%s' is not implemented" % (func))
                        value = value.replace(string_matched, \
                                              str(groups[var]))

        return value


class Aliases(Conf):
    pass


class CommandLineOptions:

    def __init__(self):

        self.__options = None

        parser = OptionParser(
            usage = "%prog [-v] [-q] [-d] [-f] [-c config_file]",
            version = "OSSIM (Open Source Security Information Management) " + \
                      "- Agent ")

        parser.add_option("-v", "--verbose", dest="verbose",
                          action="count",
                          help="verbose mode, makes lot of noise")
        parser.add_option("-d", "--daemon", dest="daemon", action="store_true",
                          help="Run agent in daemon mode")
        parser.add_option("-f", "--force", dest="force", action="store_true",
                          help = "Force startup overriding pidfile")
        parser.add_option("-c", "--config", dest="config_file", action="store",
                          help = "read config from FILE", metavar="FILE")
        (self.__options, args) = parser.parse_args()

        if len(args) > 1:
            parser.error("incorrect number of arguments")

        if self.__options.verbose and self.__options.daemon:
            parser.error("incompatible options -v -d")


    def get_options(self):
        return self.__options


# create an array from a list of sids
# for example:
# "1,2,3-6,7" => [1, 2, 3, 4, 5, 6, 7]
def split_sids(string):

    list = list_tmp = []

    # split by ","
    list = string.split(',')

    # split by "-"
    for sid in list:
        a = sid.split('-')
        if len(a) == 2:
            list.remove(sid)
            for i in range(int(a[0]), int(a[1])+1):
                list_tmp.append(i)

    list.extend(list_tmp)
    return list


# vim:ts=4 sts=4 tw=79 expandtab:

