import os, sys, time, re, socket

from Detector import Detector
from Event import Event, EventOS, EventMac, EventService, EventHids
from Logger import Logger
logger = Logger.logger

class RuleMatch:

    NEWLINE = "\\n"

    def __init__(self, name, rule, plugin):

        logger.debug("Adding rule (%s).." % (name))

        self.rule = rule
        self.name = name
        self.plugin = plugin

        # store {regexp: , pattern: , result: } hashes
        self.lines = []

        regexp = self.rule["regexp"]
        for r in regexp.split(RuleMatch.NEWLINE):
            try:
                self.lines.append({
                    "regexp": r,
                    "pattern": re.compile(r, re.IGNORECASE),
                    "result": None})
            except Exception, e:
                logger.error("Error reading rule [%s]: %s" % (self.name, e))

        self.nlines = regexp.count(RuleMatch.NEWLINE) + 1
        self.line_count = 1
        self.matched = False
        self.groups = {}


    def feed(self, line):

        self.matched = False
        self.groups = {}

        line_index = self.line_count - 1
        self.lines[line_index]["result"] = \
            self.lines[line_index]["pattern"].search(line)

        if self.line_count == self.nlines:
            if self.lines[line_index]["result"] is not None: # matched!
                self.matched = True
                self.line_count = 1

        else:
            if self.lines[line_index]["result"] is not None: # matched!
                self.line_count += 1
            else:
                self.line_count = 1


    def match(self):
        if self.matched:
            self.group()
        return self.matched


    # convert the list of pattern objects to a dictionary
    def group(self):

        self.groups = {}
        count = 1

        if self.matched:

            for line in self.lines:

                # group by index ()
                groups = line["result"].groups()
                for group in groups:
                    if group is None:
                        group = '' # convert to '' better than 'None'
                    self.groups.update({str(count): str(group)})
                    count += 1

                # group by name (?P<name-of-group>)
                self.groups.update(line["result"].groupdict())


    def generate_event(self, line = ''):

        if not self.rule.has_key('event_type'):
            logger.error("Event has no type, check plugin configuration!")
            return None

        if self.rule['event_type'] == Event.EVENT_TYPE:
            event = Event()
        elif self.rule['event_type'] == EventOS.EVENT_TYPE:
            event = EventOS()
        elif self.rule['event_type'] == EventMac.EVENT_TYPE:
            event = EventMac()
        elif self.rule['event_type'] == EventService.EVENT_TYPE:
            event = EventService()
        elif self.rule['event_type'] == EventHids.EVENT_TYPE:
            event = EventHids()
        else:
            logger.error("Bad event_type (%s) in rule (%s)" % \
                (self.rule["event_type"], self.name))
            return None

        for key, value in self.rule.iteritems():
            if key != "regexp":
                event[key] = self.plugin.get_replace_value(value, self.groups)

        if line:
            event['log'] = line

        return event


class ParserLog(Detector):

    def __init__(self, conf, plugin):
        self._conf = conf        # config.cfg info
        self._plugin = plugin    # plugins/X.cfg info
        self.rules = []          # list of RuleMatch objects
        Detector.__init__(self, conf, plugin)


    def process(self):

        location = self._plugin.get("config", "location")
        if self._plugin.has_option("config", "create_file"):
            create_file = self._plugin.getboolean("config", "create_file")
        else:
            create_file = False

        # first check if file exists
        if not os.path.exists(location) and create_file:
            if not os.path.exists(os.path.dirname(location)):
                logger.warning("Creating directory %s.." % \
                    (os.path.dirname(location)))
                os.mkdir(os.path.dirname(location), 0755)
            logger.warning("Can not read from file %s, no such file. " % \
                (location) + "Creating it..")
            fd = open(location, 'w')
            fd.close()

        # open file
        try:
            fd = open(location, 'r')
        except IOError, e:
            logger.error("Can not read from file %s: %s" % (location, e))
            sys.exit()

        # compile the list of regexp
        for key, item in self._plugin.rules().iteritems():
            self.rules.append(RuleMatch(key, item, self._plugin))

        # Move to the end of file
        fd.seek(0, 2)

        while 1:

            # is plugin enabled?
            if not self._plugin.getboolean("config", "enable"):

                # wait until plugin is enabled
                while not self._plugin.getboolean("config", "enable"):
                    time.sleep(1)

                # plugin is now enabled, skip events generated on
                # 'disable' state, so move to the end of file
                fd.seek(0, 2)

            where = fd.tell()
            line = fd.readline()

            if not line: # EOF reached
                time.sleep(1)
                fd.seek(where)
            else:

                # this could make a lot of noise...
                # logger.debug('Line read: %s' % (line))

                for rule in self.rules:
                    rule.feed(line)
                    if rule.match():
                        logger.info("Matched rule: [%s]" % (rule.name))
                        event = rule.generate_event(line)
                        if event is not None:
                            self.send_message(event)

                            # one rule matched, no need to check more
                            break

        fd.close()

# vim:ts=4 sts=4 tw=79 expandtab:
