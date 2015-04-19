

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


# vim:ts=4 sts=4 tw=79 expandtab:
