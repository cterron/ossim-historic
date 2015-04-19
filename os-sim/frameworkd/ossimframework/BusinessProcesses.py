import threading, time, sys

import Const, Util
from OssimDB import OssimDB
from OssimConf import OssimConf

_CONF  = OssimConf(Const.CONFIG_FILE)
_DB    = OssimDB()
_DB.connect(_CONF['ossim_host'],
            _CONF['ossim_base'],
            _CONF['ossim_user'],
            _CONF['ossim_pass'])

# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#                                                                       #
#             BPMember            Measure --------<> MeasureList        #
#                ^                   ^                                  #
#                |                   |               MemberTypes        #
#      +----+----+----+----+         |                                  #
#      |    |    |    |    |     MeasureDB         BusinessProcesses    #
#     Host  |   Net   |   File                                          #
#      HostGroup  NetGroup                                              #
#                                                                       #
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

class BPMember:

    def __init__(self, member):
        self.member = member
        # Child classes must declare the following variables:
        # - self.member_type: the type of the child member 
        #   For example: 'host', 'file'
        #   These values must be present in bp_asset_member_type db table
        # - self.measures: a list of measure objects.
        #   Every kind of member has its own measures

    def update_status(self):
        for m in self.measures:
            severity = m.get_severity()
            if severity >= Measure.MIN_SEVERITY and \
               severity <= Measure.MAX_SEVERITY:

                # clean the old member entry
                query = """
                    DELETE FROM bp_member_status 
                        WHERE member = '%s' and measure_type = '%s';
                """ % (self.member, m.measure_type)
                _DB.exec_query(query)

                # insert the new one
                query = """
                    INSERT INTO bp_member_status
                        (member, status_date, measure_type, severity)
                        VALUES('%s', now(), '%s', %d);
                """ % (self.member, m.measure_type, severity)
                print __name__, \
                    "Updating measure [%s] of member [%s:%s]" % \
                    (m.measure_type, self.member_type, self.member), \
                    "with severity [%s]" % (severity)
                _DB.exec_query(query)


class BPMemberHost(BPMember):

    def __init__(self, member):

        BPMember.__init__(self, member)

        # A host member has the following measures:
        # risk, vulnerability, incident and metric
        self.member_type = 'host'
        measure_list = MeasureList(self.member)
        self.measures = [
            measure_list['host_alarm'],
            measure_list['host_metric'],           # 1
            measure_list['host_vulnerability'],    # 2
            measure_list['host_incident'],
            measure_list['host_incident_alarm'],
            measure_list['host_incident_event'],
            measure_list['host_incident_metric'],
            measure_list['host_incident_anomaly'],
            measure_list['host_incident_vulns'],
            measure_list['host_availability'],
        ]

        # add net->metric and net->vulnerability alternative measures
        # to each network the host belongs to
        for net in self.get_member_nets():
            net_measure_list = MeasureList(net)
            self.measures[1].add_alternative_measure(
                net_measure_list['net_metric'])
            self.measures[2].add_alternative_measure(
                net_measure_list['net_vulnerability'])

        # add global->metric and global->vulnerability alternative measures
        self.measures[1].add_alternative_measure(
            measure_list['global_metric'])
        self.measures[2].add_alternative_measure(
            measure_list['global_vulnerability'])

    def get_member_nets(self):
        nets = []
        result = _DB.exec_query("""SELECT ips FROM net;""")
        if result != []:
            for r in result:
                net = r['ips']
                if Util.isIpInNet(host=self.member, net_list=[net]):
                    nets.append(net)
        return nets

class BPMemberNet(BPMember):

    def __init__(self, member):
        BPMember.__init__(self, member)
        self.member_type = 'net'
        measure_list = MeasureList(self.member)
        self.measures = [
            measure_list['net_metric'],           # 0
            measure_list['net_vulnerability'],    # 1
        ]

        # metric measure: get global->metric value if net->metric is empty
        self.measures[0].add_alternative_measure(
            measure_list['global_metric'])
        self.measures[1].add_alternative_measure(
            measure_list['global_vulnerability'])

# TODO: new members
class BPMemberHostGroup(BPMember):

    def __init__(self, member):
        BPMember.__init__(self, member)
        self.member_type = 'host_group'
        self.measures = []

class BPMemberNetGroup(BPMember):

    def __init__(self, member):
        BPMember.__init__(self, member)
        self.member_type = 'host_group'
        self.measures = []

class BPMemberFile(BPMember):

    def __init__(self, member):
        BPMember.__init__(self, member)
        self.member_type = 'file'
        self.measures = []


class MeasureList:

    def __init__(self, member):
        self.member = member
        self.measures = {
            ### host measures ###
            'host_alarm': \
                MeasureDB (
                    measure_type = 'host_alarm',
                    request = """
                SELECT risk AS host_alarm FROM alarm
                    WHERE dst_ip = inet_aton('%s') OR
                          src_ip = inet_aton('%s');
                    """ % (self.member, self.member),
                    severity_max = 10
                ),
            'host_metric': \
                MeasureDB (
                    measure_type = 'host_metric',
                    request = """
                SELECT compromise + attack AS host_metric
                    FROM host_qualification
                    WHERE host_ip = '%s';
                    """ % (self.member),
                    severity_max = int(_CONF["threshold"])*2
                ),
            'host_vulnerability': \
                MeasureDB (
                    measure_type = 'host_vulnerability',
                    request = """
                SELECT vulnerability AS host_vulnerability
                    FROM host_vulnerability WHERE ip = '%s';
                    """ % (self.member),
                    severity_max = 10
                ),
            'host_incident': \
                MeasureDB (
                    measure_type = 'host_incident',
                    request = """
                SELECT priority AS host_incident FROM incident
                    WHERE title LIKE '%%%s%%';
                    """ % (self.member),
                    severity_max = 10
                ),
            'host_incident_alarm': \
                MeasureDB (
                    measure_type = 'host_incident_alarm',
                    request = """
                SELECT incident.priority AS host_incident_alarm, incident.id 
                    FROM incident, incident_alarm
                    WHERE incident.id = incident_alarm.incident_id AND
                        (incident_alarm.src_ips LIKE "%%%s%%" OR 
                         incident_alarm.dst_ips LIKE "%%%s%%");
                    """ % (self.member, self.member),
                    severity_max = 10
                ),
            'host_incident_event': \
                MeasureDB (
                    measure_type = 'host_incident_event',
                    request = """
                SELECT incident.priority AS host_incident_event, incident.id 
                    FROM incident, incident_event
                    WHERE incident.id = incident_event.incident_id AND
                        (incident_event.src_ips LIKE "%%%s%%" OR 
                         incident_event.dst_ips LIKE "%%%s%%");
                    """ % (self.member, self.member),
                    severity_max = 10
                ),
            'host_incident_metric': \
                MeasureDB (
                    measure_type = 'host_incident_metric',
                    request = """
                SELECT incident.priority AS host_incident_metric, incident.id
                    FROM incident, incident_metric
                    WHERE incident.id = incident_metric.incident_id AND
                        incident_metric.target = "%s";
                    """ % (self.member),
                    severity_max = 10
                ),
            'host_incident_anomaly': \
                MeasureDB (
                    measure_type = 'host_incident_anomaly',
                    request = """
                SELECT incident.priority AS host_incident_anomaly, incident.id
                    FROM incident, incident_anomaly
                    WHERE incident.id = incident_anomaly.incident_id AND
                        incident_anomaly.ip = "%s";
                    """ % (self.member),
                    severity_max = 10
                ),
            'host_incident_vulns': \
                MeasureDB (
                    measure_type = 'host_incident_vulns',
                    request = """
                SELECT incident.priority AS host_incident_vulns, incident.id 
                    FROM incident, incident_vulns
                    WHERE incident.id = incident_vulns.incident_id AND 
                        incident_vulns.ip = "%s";
                    """ % (self.member),
                    severity_max = 10
                ),
            'host_availability': \
                MeasureDB (
                    measure_type = 'host_availability',
                    # nagios plugin_id: 1525
                    # nagios sids for host availability: 1-6
                    request = """
                SELECT userdata1 AS host_availability 
                    FROM event_tmp 
                    WHERE plugin_id='1525' AND src_ip=inet_aton("%s")
                    ORDER BY timestamp LIMIT 1;
                    """ % (self.member),
                    severity_max = 100,
                    translation = {
                        'host_availability: DOWN': 100,
                        'host_availability: UP': 0,
                        'service_availability: CRITICAL': 100,
                        'service_availability: UNREACHABLE': 60,
                        'service_availability: WARNING': 60,
                        'service_availability: UNKNOWN': 20,
                        'service_availability: OK': 0,
                    }
                ),
            ### net measures ###
            'net_metric': \
                MeasureDB (
                    measure_type = 'net_metric',
                    request = """
                        SELECT compromise + attack AS net_metric
                            FROM net_qualification
                            WHERE net_name = '%s';
                    """ % (self.member),
                    severity_max = int(_CONF["threshold"])*2
                ),
            'net_vulnerability': \
                MeasureDB (
                    measure_type = 'net_vulnerability',
                    request = """
                        SELECT vulnerability AS net_vulnerability
                            FROM net_vulnerability WHERE net = '%s';
                        """ % (self.member),
                    severity_max = 10,
                ),
            ### global measures ###
            'global_metric': \
                MeasureDB (
                    measure_type = 'global_metric',
                    request = """
                        SELECT (SUM(compromise)+SUM(attack))/count(*) 
                            AS global_metric FROM host_qualification;
                    """,
                    severity_max = int(_CONF["threshold"]*2)
                ),
            'global_vulnerability': \
                MeasureDB (
                    measure_type = 'global_vulnerability',
                    request = """
                        SELECT SUM(vulnerability)/count(*)
                            AS global_vulnerability FROM host_vulnerability;
                    """,
                    severity_max = 10
                ),
        }

    def __getitem__(self, item):
        return self.measures[item]

    def __setitem__(self, item, value):
        self.measures[item] = value

class Measure:

    MAX_SEVERITY = 10
    MIN_SEVERITY = 0

    def __init__(self, measure_type, request, severity_max, translation = {}):
        self.measure_type = measure_type
        self.request = request
        self.severity_max = severity_max
        self.translation = translation
        self.alternative_measures = []

    # you must redefine this method in child classes
    def get_measure(self):
        print __name__, self.request
        return None

    def get_severity(self):

        def _get_severity(measure):
            measure.measure_value = measure.get_measure()
            if measure.measure_value is not None:
                severity = measure.measure_value * Measure.MAX_SEVERITY \
                    / measure.severity_max
                if severity > Measure.MAX_SEVERITY:
                    severity = Measure.MAX_SEVERITY
                return severity
            return None

        # array with measures 
        # [ original (self) plus alternatives (self.alternative_measures) ]
        measures = self.alternative_measures
        measures.insert(0, self)

        for measure in measures:
            severity = _get_severity(measure)
            if severity is not None:
                return severity

        return Measure.MIN_SEVERITY

    # if a measure returns a 'None' severity,
    # try getting the value using alternative measures
    def add_alternative_measure(self, alt_measure):
        self.alternative_measures.append(alt_measure)



class MeasureDB(Measure):

    def get_measure(self):
        result = _DB.exec_query(self.request)
        if result != []:
            #
            # IMPORTANT: the result is indexed by measure_type,
            # so be careful building your queries in bp_meber_* classes.
            #
            # for example, given a measure of type 'metric', 
            # you need to build your query this way: 
            # """SELECT foobar AS metric"""
            #                     ^^^^^^
            if result[0].has_key(self.measure_type):
                if result[0][self.measure_type] is not None:
                    s = self.translation.get(result[0][self.measure_type],
                                             result[0][self.measure_type])
                    if type(s) is int: # severity must be integer
                        return s
        return None


class MemberTypes:

    def __init__(self):
        self.types = self.get_types()

    def get_types(self):
        types = []
        query = """SELECT distinct(type_name) FROM bp_asset_member_type;"""
        result = _DB.exec_query(query)
        for row in result:
            types.append(row['type_name'])
        return types        

    # this method is defined to allow the use
    # of the operators 'in' and 'not in'
    def __contains__(self, measure_type):
        return measure_type in self.types 


class BusinessProcesses(threading.Thread):

    def __init__(self, seconds_between_iterations=Const.SLEEP):
        self.member_types = MemberTypes()
        self.sleep = float(seconds_between_iterations)
        threading.Thread.__init__(self)

    def get_members(self):
        query = \
            """SELECT distinct(member), member_type FROM bp_asset_member;"""
        members = _DB.exec_query(query)
        return members

    def run(self):
        while 1:
            self.members = self.get_members()
            for m in self.members:
                member = None

                # check bp_asset_member_type table for supported member types
                if m['member_type'] not in self.member_types:
                    print __name__, "Unsupported member type (%s)" % \
                        (m['member_type'])
                    continue

                if m['member_type'] == 'host':
                    member = BPMemberHost(m['member'])
                elif m['member_type'] == 'net':
                    member = BPMemberNet(m['member'])

                if member:
                    member.update_status()

            time.sleep(self.sleep)


if __name__ == '__main__':

    bp = BusinessProcesses(seconds_between_iterations=10)
    bp.start()

    while 1:
        try:
            time.sleep(1)
        except KeyboardInterrupt:
            import os, signal
            pid = os.getpid()
            os.kill(pid, signal.SIGTERM)


# vim:ts=4 sts=4 tw=79 expandtab:
