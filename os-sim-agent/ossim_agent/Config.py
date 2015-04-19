import os, sys, string, re
from ConfigParser import ConfigParser
from optparse import OptionParser
from Exceptions import AgentCritical
from Logger import Logger
logger = Logger.logger
import ParserUtil


class Conf(ConfigParser):

    DEFAULT_CONFIG_FILE = "/etc/ossim/agent/config.cfg"

    # fill this table with the *mandatory* entries
    # that need to be present in config.cfg file
    _NEEDED_CONFIG_ENTRIES = {
        'daemon': [],
        'log': [],
        'plugin-defaults': ['sensor', 'interface'],
        'watchdog': ['enable', 'interval'],
        'output-server': ['enable', 'ip', 'port'],
        'plugins': [],
    }
    _EXIT_IF_MALFORMED_CONFIG = True

    # same as ConfigParser.read() but also check
    # if configuration files exists
    def read(self, filenames):
        for filename in filenames:
            if not os.path.isfile(filename):
                AgentCritical("Configuration file (%s) does not exist!" % \
                    (filename))
        ConfigParser.read(self, filenames)
        self.check_needed_config_entries()
    
    # check for needed entries in .cfg files
    # this function uses the variable _NEEDED_CONFIG_ENTRIES
    def check_needed_config_entries(self):
        for section, values in self._NEEDED_CONFIG_ENTRIES.iteritems():
            if not self.has_section(section):
                logger.critical (
                    "Needed section [%s] not found!" % (section))
                if self._EXIT_IF_MALFORMED_CONFIG:
                    sys.exit()
            for value in values:
                if not self.has_option(section, value):
                    logger.critical (
                        "Needed option [%s->%s] not found!" % (section, value))
                    if self._EXIT_IF_MALFORMED_CONFIG:
                        sys.exit()

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
        conf_str = '<sensor-config>\n'
        for section in self.sections():
            conf_str += '  <section name="%s">\n' % (section)
            for i in self.items(section):
                conf_str += '    <item name="%s" value="%s" />\n' % (i[0], i[1])
            conf_str += '  </section>\n'
        conf_str += '</sensor-config>'
        return conf_str

class Plugin(Conf):

    # fill this table with the *mandatory* entries
    # that need to be present in a plugin.cfg file
    _NEEDED_CONFIG_ENTRIES = {
        'config': ['type', 'source']
    }
    _EXIT_IF_MALFORMED_CONFIG = False

    TRANSLATION_SECTION = 'translation'
    TRANSLATION_FUNCTION = 'translate'
    TRANSLATION_DEFAULT = '_DEFAULT_'
    SECTIONS_NOT_RULES = ["config", "info", TRANSLATION_SECTION]

    def rules(self):
        rules = {}
        for section in self.sections():
            if section.lower() not in Plugin.SECTIONS_NOT_RULES :
                rules[section] = self.hitems(section)

        return rules


    # look for \_CFG(section,option) values in config parameters
    # and replace this with value found in global config file 
    def replace_config(self, conf):

        for section in self.sections():
            for option in self.options(section):
                regexp = self.get(section, option)
                search = re.findall("(\\\\_CFG\(([\w-]+),([\w-]+)\))", regexp)

                if search != []:
                    for string in search:
                        (all, arg1, arg2) = string
                        if conf.has_option(arg1, arg2):
                            value = conf.get(arg1, arg2)
                            regexp = regexp.replace(all, value)
                            self.set(section, option, regexp)


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


    # variables are specified as {$v}
    # you can use two-anidated variables: {${$v}}
    # this function is called from get_replace_value()
    def _replace_variables(self, value, groups):
        
        for i in range(2):
            search = re.findall("\{\$[^\}\{]+\}", value)
            if search != []:
                for string in search:
                    var = string[2:-1]
                    if groups.has_key(var):
                        value = value.replace(string, str(groups[var]))

        return value

    # special function translate() for translations
    # translations are defined in the own plugin with a [translation] entry
    # this function is called from get_replace_value()
    def _replace_translations(self, value, groups):

        regexp = "(\{(" + Plugin.TRANSLATION_FUNCTION + ")\(\$([^\)]+)\)\})"
        search = re.findall(regexp, value)
        if search != []:
            for string in search:
                (string_matched, func, var) = string
                if groups.has_key(var):
                    if self.has_section(Plugin.TRANSLATION_SECTION):
                        if self.has_option(Plugin.TRANSLATION_SECTION,
                                           groups[var]):
                            value = self.get(Plugin.TRANSLATION_SECTION,
                                             groups[var])
                        else:
                            logger.warning("Can not translate '%s' value" %\
                                (groups[var]))

                            # It's not possible to translate the value,
                            # revert to _DEFAULT_ if the entry is present
                            if self.has_option(Plugin.TRANSLATION_SECTION,
                                               Plugin.TRANSLATION_DEFAULT):
                                value = self.get(Plugin.TRANSLATION_SECTION,
                                                 Plugin.TRANSLATION_DEFAULT)
                            else:
                                value = groups[var]
                    else:
                        logger.warning("There is no translation section")
                        value = groups[var]

        return value

    # functions are specified as {f($v)}
    # this function is called from get_replace_value()
    def _replace_user_functions(self, value, groups):

        search = re.findall("(\{(\w+)\((\$[^\)]+)\)\})", value)
        if search != []:
            for string in search:
                (string_matched, func, variables) = string
                vars = split_variables(variables)

                # check that all variables have a replacement
                for var in vars:
                    if not groups.has_key(var):
                        logger.warning("Can not replace '%s'" % (value))
                        return value

                # call function 'func' with arg groups[var]
                # functions are defined in ParserUtil.py file
                if func != Plugin.TRANSLATION_FUNCTION and \
                   hasattr(ParserUtil, func):

                    # 'f' is the function to be called
                    f = getattr(ParserUtil, func)

                    # 'vars' are the list of arguments of the function
                    # 'args' are a custom representation of the list
                    #        to be used as f argument [ f(args) ]
                    args  = ""
                    for i in (range(len(vars))):
                        args += "groups[vars[%s]]," % (str(i))

                    # exec replacement
                    try:
                        exec "value = value.replace(string_matched," +\
                            "str(f(" + args + ")))"
                    except TypeError, e:
                        logger.error(e)

                else:
                    logger.warning(
                        "Function '%s' is not implemented" % (func))
                    value = value.replace(string_matched, \
                                          str(groups[var]))

        return value

    # replace config values matching {$X} with self.groups["X"]
    # and {f($X)} with f(self.groups["X"])
    def get_replace_value(self, value, groups):

        # 1) replace variables
        value = self._replace_variables(value, groups)

        # 2) replace translations
        value = self._replace_translations(value, groups)

        # 3) replace user functions
        value = self._replace_user_functions(value, groups)

        return value


class Aliases(Conf):
    _NEEDED_CONFIG_ENTRIES = {}


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


# create an array from a list of variables
# for example:
# "$1,$2, $3,   $5" => [1, 2, 3, 5]
def split_variables(string):
    return re.findall("(?:\$([^,\s]+))+", string)


# create an array from a list of sids
# for example:
# "1,2,3-6,7" => [1, 2, 3, 4, 5, 6, 7]
def split_sids(string, separator=','):

    list = list_tmp = []

    # split by 'separator'
    list = string.split(separator)

    # split by "-"
    for sid in list:
        a = sid.split('-')
        if len(a) == 2:
            list.remove(sid)
            for i in range(int(a[0]), int(a[1])+1):
                list_tmp.append(str(i))

    list.extend(list_tmp)
    return list


if __name__ == '__main__':
    conf = Conf()
    conf.read(['/etc/ossim/agent/config.cfg'])
    print conf


# vim:ts=4 sts=4 tw=79 expandtab:

