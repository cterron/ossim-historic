import threading, re, socket

from ActionMail import ActionMail
from ActionExec import ActionExec
from OssimConf import OssimConf
from OssimDB import OssimDB
import Const
import Util

class Action(threading.Thread):

    def __init__(self, request):

        self.__request = self.parseRequest(request)
        self.__responses = {}
        self.__conf = OssimConf(Const.CONFIG_FILE)
        self.__db = OssimDB()
        threading.Thread.__init__(self)


    # build a hash with the request info
    def parseRequest(self, request):

        #
        # request example:
        #
        # event date="2005-06-16 13:06:18" plugin_id="1505" plugin_sid="4"
        # risk="8" priority="4" reliability="10" event_id="297179"
        # backlog_id="13948" src_ip="192.168.1.10" src_port="1765"
        # dst_ip="192.168.1.11" dst_port="139" protocol="6"
        # sensor="192.168.6.64"
        #

        request_hash = {}

        try:
            request_hash['type'] = request.split()[0]
        except IndexError:
            request_hash['type'] = 'unknown'
            print __name__, \
                ": Sorry, unknown request type received:", request
            return {}

        result = re.findall('(\w+)="([^"]+)"', request)
        for i in result:
            request_hash[i[0]] = i[1]

        return request_hash

    
    # response-actions stores net names
    def getIpsByNet(self, netname):
        
        query = "SELECT ips FROM net WHERE name = '%s'" % (netname)
        net_info = self.__db.exec_query(query)

        ips = 'ANY'
        for net in net_info:
            ips = net['ips']

        return ips


    # get matched actions from db
    def getActions(self):

        actions = []

        #
        # ANY: for strings  :'ANY'
        #      for integers : 0
        #

        for response in self.__responses:

            # inocente hasta que se demuestre lo contrario
            match = 1

            # source host, dest host and sensor lists for each response
            host_source_list = []
            host_dest_list = []
            host_sensor_list = []
            for host in response['host']:
                if host['_type'] == 'source':
                    host_source_list.append(host['host'])
                elif host['_type'] == 'dest':
                    host_dest_list.append(host['host'])
                elif host['_type'] == 'sensor':
                    host_sensor_list.append(host['host'])

            # source net, dest net lists for each response
            net_source_list = []
            net_dest_list = []
            for net in response['net']:
                if net['_type'] == 'source':
                    net_source_list.append(self.getIpsByNet(net['net']))
                elif net['_type'] == 'dest':
                    net_dest_list.append(self.getIpsByNet(net['net']))

            # source port and dest port lists for each response
            port_source_list = []
            port_dest_list = []
            for port in response['port']:
                if port['_type'] == 'source':
                    port_source_list.append(port['port'])
                elif port['_type'] == 'dest':
                    port_dest_list.append(port['port'])

            # plugin list for each response
            plugin_list = []
            for plugin in response['plugin']:
                plugin_list.append(plugin['plugin_id'])

            # source hosts
            if self.__request['src_ip'] not in host_source_list and \
              (not Util.isIpInNet(self.__request['src_ip'],
                                  net_source_list)) and \
              'ANY' not in host_source_list and \
              'ANY' not in net_source_list:
                match = 0
                continue

            # dest hosts
            if self.__request['dst_ip'] not in host_dest_list and \
              (not Util.isIpInNet(self.__request['dst_ip'],
                                  net_dest_list)) and \
              'ANY' not in host_dest_list and \
              'ANY' not in net_dest_list:
                match = 0
                continue

            # source ports
            if int(self.__request['src_port']) not in port_source_list and \
              0 not in port_source_list:
                match = 0
                continue

            # dest ports
            if int(self.__request['dst_port']) not in port_dest_list and \
              0 not in port_dest_list:
                match = 0
                continue

            # plugins
            if int(self.__request['plugin_id']) not in plugin_list and \
              0 not in plugin_list:
                match = 0
                continue

            # sensors
            if self.__request['sensor'] not in host_sensor_list and \
              'ANY' not in host_sensor_list:
                match = 0
                continue


            if match:

                query = "SELECT action_id FROM response_action " +\
                        "WHERE response_id = %d" % (response["id"])
                action_info = self.__db.exec_query(query)

                for action in action_info:
                    action_id = action['action_id']
                    if actions.count(action_id) == 0:
                        actions.append(action_id)

        return actions


    # fill responses hash with all response db info
    def getResponses(self):

        responses = self.__db.exec_query("SELECT * FROM response")
        for response in responses:
            for item in ("host", "net", "plugin", "port", "action"):
                response[item] = self.__db.exec_query(
                    "SELECT * FROM response_%s WHERE response_id = %d" %\
                        (item, response["id"]))
            # ensure int datatype for plugin ids and ports
	    # XXX response["plugin"] and response["port"] are not string type
	    # they are arrays
#            for item in ("plugin", "port"):
#                response[item] = int(response[item])

        return responses


    def requestRepr(self, request):
        
        temp_str  = " Alert detail: \n"
        for key, value in request.iteritems():
            temp_str += " * %s: \t%s\n" % (key, value)
        return temp_str


    def doAction(self, action_id):

        replaces = {
                'DATE':         self.__request.get('date', ''),
                'PLUGIN_ID':    self.__request.get('plugin_id', ''),
                'PLUGIN_SID':   self.__request.get('plugin_sid', ''),
                'RISK':         self.__request.get('risk', ''),
                'PRIORITY':     self.__request.get('priority', ''),
                'RELIABILITY':  self.__request.get('reliability', ''),
                'SRC_IP':       self.__request.get('src_ip', ''),
                'DST_IP':       self.__request.get('dst_ip', ''),
                'PROTOCOL':     self.__request.get('protocol', ''),
                'SENSOR':       self.__request.get('sensor', ''),
                'PLUGIN_NAME':  self.__request.get('plugin_id', ''),
                'SID_NAME':     self.__request.get('plugin_sid', ''),
                'USERDATA1':    self.__request.get('userdata1', ''),
                'USERDATA2':    self.__request.get('userdata2', ''),
                'USERDATA3':    self.__request.get('userdata3', ''),
                'USERDATA4':    self.__request.get('userdata4', ''),
                'USERDATA5':    self.__request.get('userdata5', ''),
                'USERDATA6':    self.__request.get('userdata6', ''),
                'USERDATA7':    self.__request.get('userdata7', ''),
                'USERDATA8':    self.__request.get('userdata8', ''),
                'USERDATA9':    self.__request.get('userdata9', ''),
                'FILENAME':     self.__request.get('filename', ''),
                'USERNAME':     self.__request.get('username', ''),
                'PASSWORD':     self.__request.get('password', ''),
            }

        query = "SELECT * FROM plugin WHERE id = %d" % int(self.__request['plugin_id'])

        for plugin in self.__db.exec_query(query):
            # should only yield one result anyway
            replaces["PLUGIN_NAME"] = plugin['description']

        query = "SELECT * FROM plugin_sid WHERE plugin_id = %d AND sid = %d" %\
            (int(self.__request['plugin_id']), int(self.__request['plugin_sid']))
        for plugin_sid in self.__db.exec_query(query):
            # should only yield one result anyway
            replaces["SID_NAME"] = plugin_sid['name']

        query = "SELECT * FROM action WHERE id = %d" % (action_id)
        for action in self.__db.exec_query(query):

            print __name__, ": Response with action: ", action['descr']

            # email notification
            if action['action_type'] == 'email':

                query = "SELECT * FROM action_email WHERE action_id = %d" %\
                    (action_id)
                for action_email in self.__db.exec_query(query):
                    email_from = action_email['_from']
                    email_to = action_email['_to']
                    email_subject = action_email['subject']
                    email_message = action_email['message']

                    for replace in replaces:
                        if replaces[replace]:
                            email_from = email_from.replace(replace, replaces[replace])
                            email_to = email_to.replace(replace, replaces[replace])
                            email_subject= email_subject.replace(replace, replaces[replace])
                            email_message = email_message.replace(replace, replaces[replace])
                    
                    m = ActionMail()
                    m.sendmail(email_from,
                               [ email_to ],
                               email_subject,
                               email_message +\
                               "\n\n" + self.requestRepr(self.__request))
                    del(m)
                

            # execute external command
            elif action['action_type'] == 'exec':
                query = "SELECT * FROM action_exec WHERE action_id = %d" %\
                    (action_id)
                for action_exec in self.__db.exec_query(query):
                    action = action_exec['command']
                    for replace in replaces:
                        action = action.replace(replace, replaces[replace])
                    c = ActionExec()
                    c.execCommand(action)
                    del(c)


    # Notify every alarm if email_alert is set
    def mailNotify(self):

        email = self.__conf['email_alert']
        emails = self.__conf['email_sender']

        if emails is None or emails == "":
            emails = "ossim@localhost"

        if email is not None and email != "":

            m = ActionMail()
            m.sendmail( self.__conf['email_sender'] , [ self.__conf['email_alert'] ],
                       "Ossim Alert from server '%s'" % (socket.gethostname()),
                       self.requestRepr(self.__request))
            print __name__, ": Notification sent from %s to %s" % (emails, (self.__conf['email_alert']))


    def run(self):

        if self.__request != {}:

            if self.__request['type'] == "event":
                self.mailNotify()

            self.__db.connect(self.__conf['ossim_host'],
                              self.__conf['ossim_base'],
                              self.__conf['ossim_user'],
                              self.__conf['ossim_pass'])

            self.__responses = self.getResponses()
            actions = self.getActions()

            for action in actions:
                self.doAction(action)

            self.__db.close()

# vim:ts=4 sts=4 tw=79 expandtab:
