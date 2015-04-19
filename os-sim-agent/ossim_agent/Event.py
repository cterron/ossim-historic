from Logger import Logger
logger = Logger.logger

class Event:

    EVENT_TYPE = 'event'
    EVENT_ATTRS = [
        "type",
        "date",
        "sensor",
        "interface",
        "plugin_id",
        "plugin_sid",
        "priority",
        "protocol",
        "src_ip",
        "src_port",
        "dst_ip",
        "dst_port",
        "username",
        "password",
        "filename",
        "userdata1",
        "userdata2",
        "userdata3",
        "userdata4",
        "userdata5",
        "userdata6",
        "userdata7",
        "userdata8",
        "userdata9",
        "occurrences",
        "log",
        "data",
        "snort_sid",    # snort specific
        "snort_cid",    # snort specific
    ]

    def __init__(self):
        self.event = {}
        self.event["event_type"] = self.EVENT_TYPE

    def __setitem__(self, key, value):

        if key in self.EVENT_ATTRS:
            self.event[key] = self.sanitize_value(value)

        elif key != 'event_type':
            logger.warning("Bad event attribute: %s" % (key))

    def __getitem__(self, key):
        return self.event.get(key, None)

    # event representation
    def __repr__(self):
        event = self.EVENT_TYPE
        for attr in self.EVENT_ATTRS:
            if self[attr]:
                event += ' %s="%s"' % (attr, self[attr])
        return event + "\n"

    # return the internal hash
    def dict(self):
        return self.event

    def sanitize_value(self, string):
        return str(string).strip().replace("\"", "\\\"").replace("'", "")


class EventOS(Event):
    EVENT_TYPE = 'host-os-event'
    EVENT_ATTRS = [
        "host",
        "os",
        "sensor",
        "interface",
        "date",
        "plugin_id",
        "plugin_sid",
        "occurrences",
        "log",
    ]

class EventMac(Event):
    EVENT_TYPE = 'host-mac-event'
    EVENT_ATTRS = [
        "host",
        "mac",
        "vendor",
        "sensor",
        "interface",
        "date",
        "plugin_id",
        "plugin_sid",
        "occurrences",
        "log",
    ]

class EventService(Event):
    EVENT_TYPE = 'host-service-event'
    EVENT_ATTRS = [
        "host",
        "sensor",
        "interface",
        "port",
        "protocol",
        "service",
        "application",
        "date",
        "plugin_id",
        "plugin_sid",
        "occurrences",
        "log",
    ]

class EventHids(Event):
    EVENT_TYPE = 'host-ids-event'
    EVENT_ATTRS = [
        "host",
        "hostname",
        "hids_event_type",
        "target",
        "what",
        "extra_data",
        "sensor",
        "date",
        "plugin_id",
        "plugin_sid",
        "username",
        "password",
        "filename",
        "userdata1",
        "userdata2",
        "userdata3",
        "userdata4",
        "userdata5",
        "userdata6",
        "userdata7",
        "userdata8",
        "userdata9",
        "occurrences",
        "log",
    ]



class WatchRule(Event):

    EVENT_TYPE = 'watch-rule'
    EVENT_ATTRS = [
        "plugin_id",
        "plugin_sid",
        "condition",
        "value",
        "port_from",
        "port_to",
        "interval",
        "from",
        "to",
        "absolute",
    ]
class Snort(Event):
    EVENT_TYPE = 'snort-event'
    EVENT_ATTRS = [
        "sensor",
        "interface",
        "gzipdata",
        "unziplen",
        "event_type",
        "plugin_id",
        "type",
        "occurrences"
    ]

# vim:ts=4 sts=4 tw=79 expandtab:
