

def isIpInNet(host, net_list):

    if type(net_list) is not list:
        return False

    for net in net_list:

        if net == 'ANY':
            return True

        if net.count('/') != 1:
            print __name__, "Don't know what to do with malformed net (%s)" % (net)
            continue

        (base, mask) = net.split('/')
        b = base.split('.')
        h = host.split('.')

        if len(b) != 4 or len(h) != 4:
            continue

        val1 = int(b[0])*256*256*256 +\
               int(b[1])*256*256 +\
               int(b[2])*256 +\
               int(b[3])
        val2 = int(h[0])*256*256*256 +\
               int(h[1])*256*256 +\
               int(h[2])*256 +\
               int(h[3])

        if ((val1 >> (32 - int(mask))) == (val2 >> (32 - int(mask)))):
            return True

    return False

def getHostThreshold(conn,host,type):
    if type == "C":
        query = "SELECT threshold_c FROM host WHERE ip = '%s';" % (host)
        value = "threshold_c"
    else: 
        query = "SELECT threshold_a FROM host WHERE ip = '%s';" % (host)
        value = "threshold_a"
    result = conn.exec_query(query)
    if result:
        return result[0][value]
        # return this value
    else:
        net = getClosestNet(conn,host)
        threshold = getNetThreshold(conn,net,value)
        # return this value or a default global value
        return threshold

def getNetThreshold(conn,net,type):
    query = "SELECT %s FROM net WHERE name = '%s';" % (type,net)
    result = conn.exec_query(query)
    if result:
        return int(result[0][type])
    else:
        from OssimConf import OssimConf
        import Const
        conf = OssimConf (Const.CONFIG_FILE)
        return int(conf["threshold"])

def getNetAsset(conn,net):
    query = "SELECT asset FROM net WHERE name = '%s';" % (net)
    result = conn.exec_query(query)
    if result:
        return int(result[0]["asset"])
    else:
        import Const
        return Const.ASSET

def getHostAsset(conn,host):
    query = "SELECT asset FROM host WHERE hostname = '%s';" % (host)
    result = conn.exec_query(query)
    if result:
        return int(result[0]["asset"])
    else:
        return False

def getClosestNet(conn,host):

    net_list = []
    query = "SELECT name,ips FROM net;" 
    net_list = conn.exec_query(query)

    narrowest_mask = 0;
    narrowest_net = "";

    for net in net_list:
        if net["ips"].count('/') != 1:
            print __name__, "Don't know what to do with malformed net (%s)" % (net["ips"])
            continue

        (base, mask) = net["ips"].split('/')
        b = base.split('.')
        h = host.split('.')

        if len(b) != 4 or len(h) != 4:
            continue

        val1 = int(b[0])*256*256*256 +\
               int(b[1])*256*256 +\
               int(b[2])*256 +\
               int(b[3])
        val2 = int(h[0])*256*256*256 +\
               int(h[1])*256*256 +\
               int(h[2])*256 +\
               int(h[3])

        if ((val1 >> (32 - int(mask))) == (val2 >> (32 - int(mask)))):
            if int(mask) > int(narrowest_mask):
                narrowest_mask = mask
                narrowest_net = net["name"]
        if narrowest_mask > 0:
            return narrowest_net
    return False




# vim:ts=4 sts=4 tw=79 expandtab:
