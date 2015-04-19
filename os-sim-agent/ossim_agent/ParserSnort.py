import sys,os,struct,time,socket,string
import re
import stat
import zlib
from binascii import  hexlify 
from PacketUtils import UDPPacket,IPPacket,RawPacket,TCPPacket,getprotobynumber
from Utils import dumphexdata
from Logger import Logger
logger = Logger.logger
class EventSnort:
    """This class represent a Snort Event"""
    TYPEALARM = 1
    TYPELOG = 2
    def __init__(self,t,bytedata,endian,snortconf=None,pkt=None,linklayertype="ethernet"):
        (self.sig_generator,self.sig_id,self.sig_rev,self.classification, \
        self.priority,self.event_id,self.event_reference, \
        self.ref_tv_sec,self.ref_tv_usec) =  struct.unpack(endian+"IIIIIIIII",bytedata[:36])
        self.type = t
        self.snortconf = snortconf
        self._pkt = None
        if linklayertype == "ethernet":
            self.offsetip = 14
        elif linklayertype == "cookedlinux":
            self.offsetip = 16
        else:
            raise Exception,"Unknown link layer type"
        if t==EventSnort.TYPEALARM:
            # Event time
            (self.tv_sec,self.tv_usec) = struct.unpack(endian+"II",bytedata[36:44])
            # Event ip info. All the info in these bytes are in HOST network order
            (self.sip,self.dip, \
            self.sport,self.dport \
            ,self.protocol,self.flags) = struct.unpack(endian+"IIHHII",bytedata[44:64])
            self._pkt = None
            self._packet = None
        elif t==EventSnort.TYPELOG:
            self.logflags = struct.unpack(endian+"I",bytedata[36:40])
            (self.tv_sec, \
            self.tv_usec, \
            self.__packet_caplen, \
            self.__packet_realcaplen ) = struct.unpack(endian+"IIII",bytedata[40:56])
            self._pkt = pkt
            # Snort pseudopacket (from sfportscan)
            if pkt[0:12] == "MACDADMACDAD":
                self.offsetip = 14
            self._ethertype, = struct.unpack(">H",pkt[self.offsetip-2:self.offsetip])
            #print "Ethertype: %04x" % self._ethertype
            if self._ethertype==0x0800:
                self._packet = IPPacket(pkt[self.offsetip:])
                self.sip = self._packet.sip
                self.dip = self._packet.dip
                self.dport = self._packet.dport
                self.sport = self._packet.sport
                self.protocol = self._packet.protocol
                self.flags = 0
            else:
                dumphexdata(self._pkt)
                self.sip = self.dip = self.dport = self.sport = 0
                self.protocol = self.flags = 0
                self._packet = RawPacket(self._pkt)

        else:
            raise Exception, "Bad EventSnort type"    
    def __str__(self):
        st ="""type="detector" date="%s" """+\
        """snort_gid="%u" snort_sid="%u" snort_rev="%u" """+\
        """snort_classification="%u" snort_priority="%u" """
        st = st % (time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(self.tv_sec)),
        self.sig_generator,self.sig_id,self.sig_rev,
        self.classification,self.priority)
        if self._packet!=None:
            if  isinstance(self._packet,IPPacket):
                st=st+ """ packet_type="ip" """
            elif isinstance(self._packet,RawPacket):
                st=st+ """ packet_type="raw" """
            st= st + str(self._packet)
        return st
    def strgzip(self):
        #print self.__str__()
        data = zlib.compress(self.__str__())
        return (len(self.__str__()),hexlify(data))

        
        
    def getcaplen(self):
        return self.__packet_caplen
    def getrealcaplen(self):
        return self.__packet_realcaplen
    caplen = property(getcaplen)
    packetlen = property(getrealcaplen)
    def dump(self):
        def _searchgen(gen):
            if self.GENERATOR.has_key(gen):
                return self.GENERATOR[gen]
            else:
                return "UNKNOWN GENERATOR"
        eventtime = time.strftime("%d-%m-%Y %H:%M:%S",time.localtime(self.tv_sec))
        if  not  self.snortconf     is None:
            selfgen = "%s.%10u ID: %u G:%s SID:%s CLASS:%u PRI:%u" % (eventtime,self.tv_sec, \
                self.event_id, \
                _searchgen(self.sig_generator),self.snortconf.searchsid(self.sig_id,self.sig_generator), \
                self.classification,self.priority)
        else:
            selfgen = "%s.%10u ID: %u G:%s SID:%s CLASS:%u PRI:%u" % (eventtime,self.tv_sec, \
                self.event_id, \
                _searchgen(self.sig_generator),self.sig_id, \
                self.classification,self.priority)

            
        eventreftime = time.strftime("%d-%m-%Y %H:%M:%S",time.localtime(self.ref_tv_sec))
        eventref = " REF:%u REFTIME:%s.%10u" % (self.event_reference,eventreftime,self.ref_tv_usec)
        selfipinfo =  " PROTO:%s SRC:%s:%d -> DST:%s:%d F:%08x" % (getprotobynumber(self.protocol), \
            socket.inet_ntoa(struct.pack("L",socket.htonl((self.sip)))), \
            self.sport, \
            socket.inet_ntoa(struct.pack("L",socket.htonl((self.dip)))), \
            self.dport, \
            self.flags)
            
        if (self.event_reference!=self.event_id):
            selfrefevent = "\t%s\n" % (eventref)
        else:
            selfrefevent = ""
        if self.type == EventSnort.TYPEALARM:
            print "ALARM "+selfgen+selfipinfo+selfrefevent
        elif self.type == EventSnort.TYPELOG:
            print "ALARMLOG " +selfgen+selfipinfo+selfrefevent
        else:
            raise Exception,"Bad alarm type"

class ParserSnort(object):
    # 1 -> Alarm log
    # 2 -> alarm log packat
    MAGICLITTEENDIAN = 0x2dac5ceb
    MAGICBIGENDIAN = 0x5ceb2dac
    SIZEHDR = 8
    SIZERECORDHDR = 8
    SIZEALARMLOG = 48
    def __init__(self,linklayer="ethernet",snort=None):
        self.packettypes=(1,2)
        self.snortconf = snort
        self._fd = None
        self._currentname=""
        self._prefix = ""
        self._dir = ""
        self._hdrread = False
        self._evskip = False
        if linklayer=="ethernet":
            self.offsetip = 14
            self.linklayer = linklayer
        elif linklayer=="cookedlinux":
            self.offsetip = 16
            self.linklayer = linklayer
        else:
            raise Exception,"Unknown linklayer"
        self._logfiles = []
            

    def _filterfile(self,file):
        r = re.compile(self._prefix+r'\.\d+$')

        if os.path.isfile(os.path.join(self._dir,file)) and \
            r.match(file)<>None:
            return True
        else:
            return False
    def _checklogname(self):
        dircontents =filter(self._filterfile,os.listdir(self._dir))
        dircontents.sort()
        if len(dircontents)>0:
            return os.path.join(self._dir,dircontents[-1])
        else:
            return None
    def _readhdr(self,fd):
        """ Read and verify the header of the log file"""
        st = os.fstat(fd.fileno())
        self._filesize = st[stat.ST_SIZE]
        if st[stat.ST_SIZE]>=self.SIZEHDR:
            data = fd.read(self.SIZEHDR)
            if len(data)!=self.SIZEHDR:
                raise Exception,"I/O error on %s " % self._currentname
            (magic,flags) = struct.unpack("II",data)
            if magic == self.MAGICLITTEENDIAN:
                self._endian = '<'
            elif magic == self.MAGICBIGENDIAN:
                self._endian = '>'
            else:
                raise Exception,"Bad magic number on %s" % self._currentfile
            return True
        return False
    def _skipevents(self,fd):
        """ Skip events till the last one in the file since open it"""
        r = False
        # Only the header
        pos = fd.tell()
        if (pos+self.SIZEHDR)>self._filesize:
            return True
        while not r:
            pos = fd.tell()
            if (pos+self.SIZERECORDHDR)<=self._filesize:
                data = fd.read(self.SIZERECORDHDR)
                if len(data)!=self.SIZERECORDHDR:
                    raise Exception,"I/O error on file %s" % self._currentname
                (type,size) = struct.unpack("II",data)
                # Check for size
                if (pos+size)<=self._filesize:
                    if type == 1:
                        fd.seek(size,1)
                    elif type == 2:
                        data = fd.read(size)
                        if len(data)!=size:
                            raise Exception,"I/O error on file %s" %self._currentname
                        # Obtain de caplen 
                        (caplen,) = struct.unpack(self._endian+"I",data[48:52])
                        if (pos+caplen)<=self._filesize:
                            fd.seek(caplen,1)
                        else:
                            r = True
                            fd.seek(pos,0)
                    else:
                        raise Exception,"Bad type of record on file %s" % self._currentname
            else:
                r = True
                fd.seek(pos,0)

            
    def init_log_dir(self,directory,prefix):
        if self._fd<>None:
            self._fd.close()
            self._fd = None
        # Now
        self._prefix = prefix
        self._dir = directory
        dircontents = filter(self._filterfile,os.listdir(self._dir))
        dircontents.sort() 
        # At init, only the last file or None if there isn't any file
        if len(dircontents)>0:
            # get the timestamp
            # The snort timestamp is:
            # prefix.timestamp, where timestamp is the Unix ctime
            temp = dircontents[-1]
            self._timestamp = temp[temp.rindex('.')+1:]

            self._logdir = [os.path.join(self._dir,dircontents[-1])]
        else:
            self._logdir = []
        self._currentfile = ""
        self._skip = True # Skip all events the first time

    def _update_log_list(self):
        dircontents = filter(self._filterfile,os.listdir(self._dir))
        dircontents.sort()
        #print dircontents
        for f in dircontents:
            timestamp = f[f.rindex('.')+1:]
            f = os.path.join(self._dir,f)

            ## FIXME:
            ## self._timestamp is not declared if log directory
            ## is empty (init_log_dir->len(dircontents))
            ## need to be tested
            #
            #if not hasattr(self, "_timestamp"):
            #    self._logdir.append(f)
            #    self._timestamp = timestamp
            #elif timestamp>self._timestamp and f not in self._logdir:
            #    self._logdir.append(f)

            if timestamp>self._timestamp and f not in self._logdir:
                self._logdir.append(f)


    def _try_rotate(self):
        time.sleep(10)
        self._update_log_list()
        if len(self._logdir)>0:
            self._fd.close()
            self._fd = None
        #print "Called _try_rotate"
        #print self._logdir



    def get_snort_event(self,filtertype=2):
        while 1:
            # Select the file from the list
            if self._fd == None:
                if len(self._logdir)==0:
                    self._update_log_list()
                    time.sleep(10)
                    continue
                else:
                    self._currentfile = self._logdir[0]
                    del self._logdir[0]
                    self._timestamp = self._currentfile[self._currentfile.rindex('.')+1:]
                    self._fd =  open(self._currentfile,"r")
                    logger.debug("Open file %s " % self._currentfile)
                    self._hdrread = False
            # Read the header
            if not self._hdrread:
                if not self._readhdr(self._fd):
                    self._update_log_list()
                    if len(self._logdir)>0: 
                        #We have a newer file
                        self._fd.close()
                        self._fd = None
                    else:
                        time.sleep(10)
                    continue
                else:
                    self._hdrread = True
            # Skip events
            # With the first file, we must skip events and seek to the end of it
            if self._skip:
                self._skipevents(self._fd)
                self._skip = False
            # Now, procces the file
            pos = self._fd.tell()
            st = os.fstat(self._fd.fileno())
            if (pos+self.SIZERECORDHDR)<=st[stat.ST_SIZE]:
                data = self._fd.read(self.SIZERECORDHDR)
                if len(data)!=self.SIZERECORDHDR:
                    raise Exception,"I/O error on %s" % self._currentname
                (type,size) = struct.unpack(self._endian+"II",data)
                #print "Tipo:%u Size:%u" % (type,size)
                if type == 1:
                    if  pos+size<=st[stat.ST_SIZE]:
                        if filtertype == type:
                            data = self._fd.read(size)
                            if len(data)!=size:
                                raise Exception,"I/O error on %s" % self._currentname
                            ev = EventSnort(EventSnort.TYPEALARM,data,self._endian,None,None,linklayertype=self.linklayer)
                            return ev
                        else:
                            self._fd.seek(size,1)
                    else:
                        self._fd.seek(pos,0)
                        self._try_rotate()    

                elif type == 2:
                    if  pos+size<=st[stat.ST_SIZE]:
                        data = self._fd.read(size)
                        if len(data)!=size:
                            raise Exception,"I/O error on %s" % self._currentname
                        (caplen,)=struct.unpack(self._endian+"L",data[48:52])
                        if (pos+caplen)<=st[stat.ST_SIZE]:
                            pkt = self._fd.read(caplen)
                            if len(pkt)!=caplen:
                                raise Exception,"I/O error on %s" % self._currentname
                            if filtertype==type:
                                ev = EventSnort(EventSnort.TYPELOG,data,self._endian,None,pkt,linklayertype=self.linklayer)
                                return  ev
                        else:
                            self._fd.seek(pos,0)
                            self._try_rotate()
                    else:
                        self._fd.seek(pos,0)
                        self._try_rotate()



                else: 
                    raise Exception,"Bad type of record on file %s" % self._currentname
            else:
                # If there isn't any more data (we can read the header of the alarm)
                self._try_rotate()



    def monitor(self,directory,prefix,filtertype=2):
        if prefix<>self._prefix or directory<>self._dir:
            if self._fd<>None:
                self._fd.close()
            self._currentname=""
            self._prefix = prefix
            self._dir = directory
            self._hdrread = False
            self._evskip = False
        while 1:
            tempname = self._checklogname()
            if tempname == None:
                time.sleep(10)
                continue
            # Now, the rotate file logic
            if self._currentname<>tempname:
                if self._fd<>None:
                    self._fd.close()
                print "Tempname: %s" % tempname
                self._currentname = tempname
                self._fd = open(self._currentname,"r")
                print "File Descriptor: %u" % self._fd.fileno()
                self._hdrread = False
            if not self._hdrread:
                if not self._readhdr(self._fd):
                    time.sleep(10)
                    continue
                else:
                    self._skipevents(self._fd)
                self._hdrread = True
            # Wait for events
            pos = self._fd.tell()
            st = os.fstat(self._fd.fileno())
            if (pos+self.SIZERECORDHDR)<=st[stat.ST_SIZE]:
                data = self._fd.read(self.SIZERECORDHDR)
                if len(data)!=self.SIZERECORDHDR:
                    raise Exception,"I/O error on %s" % self._currentname
                (type,size) = struct.unpack(self._endian+"II",data)
                if type == 1 and pos+size<=st[stat.ST_SIZE]:
                    if filtertype == type:
                        data = self._fd.read(size)
                        if len(data)!=size:
                            raise Exception,"I/O error on %s" % self._currentname
                        ev = EventSnort(EventSnort.TYPEALARM,data,self._endian,None,None,linklayertype=self.linklayer)
                        return ev
                    else:
                        # Skip record
                        self._fd.seek(size,1)

                elif type == 2 and pos+size<=st[stat.ST_SIZE]:
                    data = self._fd.read(size)
                    if len(data)!=size:
                        raise Exception,"I/O error on %s" % self._currentname
                    (caplen,)=struct.unpack(self._endian+"L",data[48:52])
                    if (pos+caplen)<=st[stat.ST_SIZE]:
                        pkt = self._fd.read(caplen)
                        if len(pkt)!=caplen:
                            raise Exception,"I/O error on %s" % self._currentname
                        if filtertype==type:
                            ev = EventSnort(EventSnort.TYPELOG,data,self._endian,None,pkt,linklayertype=self.linklayer)
                            return  ev
                    else:
                        self._fd.seek(pos,0)

                elif (type!=1 and type!=2):
                    raise Exception,"Bad type of record on file %s" % self._currentname
            else:
                time.sleep(10)
            # end while


    def dumpfile(self,f,filtertype=2):
        """ dump a unified alarm / log record"""
        if not os.path.isfile(f):
            raise Exception,"Error: %s is not a file" % file
        fd = open(f,"r")
        self._currentname = f
        if self._readhdr(fd):
            while 1:
                data = fd.read(self.SIZERECORDHDR)
                if data=="":
                    fd.close()
                    print "End of file"
                    return
                if len(data)==self.SIZERECORDHDR:
                    (type,size) = struct.unpack(self._endian+"II",data)
                    if type == EventSnort.TYPEALARM:
                        data = fd.read(size)
                        if len(data)!=size:
                            raise Exception,"I/O error on %s" % self._currentname
                        if (filtertype == type):
                            ev = EventSnort(EventSnort.TYPEALARM,data,self._endian,None,None)
                            print ev
                    elif type == EventSnort.TYPELOG:
                        data = fd.read(size)
                        if len(data)!=size:
                            raise Exception,"I/O error on %s" % self._currentname
                        (caplen,)=struct.unpack(self._endian+"L",data[48:52])
                        pkt = fd.read(caplen)
                        if len(pkt)!=caplen:
                            raise Exception,"I/O error on %s" % self._currentname
                        if filtertype==type:
                            ev = EventSnort(EventSnort.TYPELOG,data,self._endian,None,pkt)
                            print ev
                    else:
                        raise Exception,"Bad type of record on file %s" % self._currentname

                
                

        else:
            print "Can't read the header of %s" % file

# vim:ts=4 sts=4 tw=79 expandtab:
