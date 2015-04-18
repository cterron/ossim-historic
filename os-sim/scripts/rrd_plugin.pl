#!/usr/bin/perl

# Script for the plugin rrd_threshold y rrd_anomaly
#
# 2004-02-11 Fabio Ospitia Trujillo <fot@ossim.net>


use DBI;
use ossim_conf;

# Data Source 
my $ds_type = "mysql";
my $ds_name = $ossim_conf::ossim_data->{"ossim_base"};
my $ds_host = $ossim_conf::ossim_data->{"ossim_host"};
my $ds_port = $ossim_conf::ossim_data->{"ossim_port"};
my $ds_user = $ossim_conf::ossim_data->{"ossim_user"};
my $ds_pass = $ossim_conf::ossim_data->{"ossim_pass"};

# Interfaces (comma separate)
my $interfaces = "eth0";

# Anomaly
my $rrd_sleep = 300;
my $rrd_interval = 300;
my $rrd_range = "1H";

# RRD Path Files
my $rrd_bin = $ossim_conf::ossim_data->{"rrdtool_path"} . "/rrdtool";
my $rrd_ntop = $ossim_conf::ossim_data->{"rrdpath_ntop"};
my $rrd_log = "/var/log/rrd_plugin.log";

# RRD Files
my %rrd_global_atts =
    ("active_host_senders_num" => ["activeHostSendersNum"],
     "arp_rarp_bytes"    => ["arpRarpBytes"],
     "broadcast_pkts"    => ["broadcastPkts"],
     "ethernet_bytes"    => ["ethernetBytes"],
     "ethernet_pkts"     => ["ethernetPkts"],
     "icmp_bytes"        => ["icmpBytes"],
     "igmp_bytes"        => ["igmpBytes"],
     "ip_bytes"          => ["ipBytes"],
     "ip_dhcp_bootp_bytes" => ["IP_DHCP-BOOTPBytes"],
     "ip_dns_bytes"      => ["IP_DNSBytes"],
     "ip_edonkey_bytes"  => ["IP_eDonkeyBytes"],
     "ip_ftp_bytes"      => ["IP_FTPBytes"],
     "ip_gnutella_bytes" => ["IP_GnutellaBytes"],
     "ip_http_bytes"     => ["IP_HTTPBytes"],
     "ip_kazaa_bytes"    => ["IP_KazaaBytes"],
     "ip_mail_bytes"     => ["IP_MailBytes"],
     "ip_messenger_bytes" => ["IP_MessengerBytes"],
     "ip_nbios_ip_bytes" => ["IP_NBios-IPBytes"],
     "ip_nfs_bytes"      => ["IP_NFSBytes"],
     "ip_nttp_bytes"     => ["IP_NNTPBytes"],
     "ip_snmp_bytes"     => ["IP_SNMPBytes"],
     "ip_ssh_bytes"      => ["IP_SSHBytes"],
     "ip_telnet_bytes"   => ["IP_TelnetBytes"],
     "ip_winmx_bytes"    => ["IP_WinMXBytes"],
     "ip_x11_bytes"      => ["IP_X11Bytes"],
     "ipx_bytes"         => ["ipxBytes"],
     "known_hosts_num"   => ["knownHostsNum"],
     "multicast_pkts"    => ["multicastPkts"],
     "ospf_bytes"        => ["ospfBytes"],
     "other_bytes"       => ["otherBytes"],
     "tcp_bytes"         => ["tcpBytes"],
     "udp_bytes"         => ["udpBytes"],
     "up_to_1024_pkts"   => ["upTo1024Pkts"],
     "up_to_128_pkts"    => ["upTo128Pkts"],
     "up_to_1518_pkts"   => ["upTo1518Pkts"],
     "up_to_512_pkts"    => ["upTo512Pkts"],
     "up_to_64_pkts"     => ["upTo64Pkts"]);

my %rrd_host_atts=
    ("pkt_sent" => ["pktSent"],
     "pkt_rcvd" => ["pktRcvd"],
     "bytes_sent" => ["bytesSent"],
     "bytes_rcvd" => ["bytesRcvd"],
     "tot_contacted_sent_peers" => ["totContactedSentPeers"],
     "tot_contacted_rcvd_peers" => ["totContactedRcvdPeers"],
     "ip_dns_sent_bytes" => ["IP_DNSSentBytes"],
     "ip_dns_rcvd_bytes" => ["IP_DNSRcvdBytes"],
     "ip_nbios_ip_sent_bytes" => ["IP_NBios-IPSentBytes"],
     "ip_nbios_ip_rcvd_bytes" => ["IP_NBios-IPRcvdBytes"],
     "ip_mail_sent_bytes" => ["IP_MailSentBytes"],
     "ip_mail_rcvd_bytes" => ["IP_MailRcvdBytes"]);

my $dsn = "dbi:" . $ds_type . ":" . $ds_name . ":" . $ds_host . ":" . $ds_port . ":";
my $conn;

# Return the average
sub rrd_graph_average {
    my ($file, $what, $type) = @_;

    my @result= `$rrd_bin graph /dev/null -s N-$rrd_range -e N -X 2 DEF:obs=$file:$what:AVERAGE PRINT:obs:$type:%lf`;
 
    chop ($result[1]);

    return $result[1];
}

sub rrd_fetch_hwpredict_by_time {
    my ($file, $stime, $etime) = @_;

    my $result = `$rrd_bin fetch $file HWPREDICT -s $stime -e $etime | grep $etime`;

    my @tmp = split (" ", $result); 

    return $tmp[1];
}

sub rrd_fetch_devpredict_by_time {
    my ($file, $stime, $etime) = @_;

    my $result = `$rrd_bin fetch $file DEVPREDICT -s $stime -e $etime | grep $etime`;

    my @tmp = split (" ", $result); 

    return $tmp[1];
}

sub rrd_fetch_average_by_time {
    my ($file, $stime, $etime) = @_;

    my $result = `$rrd_bin fetch $file AVERAGE -s $stime -e $etime | grep $etime`;

    my @tmp = split (" ", $result); 

    return $tmp[1];
}

# Return the last faliure interval
sub rrd_fetch_last_failure {
    my ($file, $range) = @_;
    my @result;

    my @failures = `$rrd_bin fetch $file FAILURES -s N-$rrd_range -e N`;

    my $empty = 0;
    my $var;
    foreach $var (@failures) {
	unless ($var =~ m/(^\d+):.*1\.0000000000e\+00.*/) {
	    $empty = 1;
	    next;
	}

	@result = () if ($empty);
	push (@result, $1);
	$empty = 0;
    }

    return @result;
}

# Return true if is anomaly
sub rrd_anomaly {
    my ($ip, $interface, $att, $priority, $file, $persistence) = @_;

    my @failure = rrd_fetch_last_failure ($file, $rrd_range);
    return 0 unless (@failure);

    my $curr_time = time ();
    my $last_time = int ($curr_time / $rrd_interval) * $rrd_interval;
    my $first_time = $last_time - ($persistence * $rrd_interval);

    my $first_failure = $failure[$#failure - $persistence];
    my $last_failure = $failure[$#failure];

    return 0 if (($last_failure != $last_time) || ($first_failure != $first_time));

    my $hwpredict = rrd_fetch_hwpredict_by_time ($file, $last_failure - 1, $last_failure);
    my $devpredict = rrd_fetch_devpredict_by_time ($file, $last_failure - 1, $last_failure);
    my $average = rrd_fetch_average_by_time ($file, $last_failure - 1, $last_failure);

    # If average is by excess
    return 0 unless ($average > ($hwpredict - (2 * $devpredict)));

    print OUTPUT "rrd_anomaly: $curr_time $ip $interface $att $priority $last_failure\n";

    return 1;
}

# Return true if is threshold
sub rrd_threshold {
    my ($ip, $interface, $att, $priority, $file, $threshold) = @_;
    my $res = rrd_graph_average ($file, "counter", "MAX");

    return 0 unless ($res > $threshold);
    my $curr_time = time ();

    print OUTPUT "rrd_threshold: $curr_time $ip $interface $att $priority " . ($res - $threshold) . "\n";

    return 1;
}

# Get Host attributes from DB
sub rrd_hosts {
    my ($interface) = @_; 

    my $query = "SELECT * FROM rrd_conf;";
    my $stm = $conn->prepare($query);
    $stm->execute();

    while (my $row = $stm->fetchrow_hashref) {
	my $ip = $row->{ip};
	my $val;
	foreach $val (keys %rrd_host_atts){
	    my $file = $rrd_ntop . "/interfaces/" . $interface . "/hosts/". $ip . "/" . $rrd_host_atts{$val}[0] . ".rrd";
	    
	    next unless (-e $file);
	    next unless ($row->{$val} =~ m/^(.*),(.*),(.*),(.*),(.*)$/);

	    my $threshold = $1;
	    my $priority = $2;
	    my $persistence = $5;
	    
	    rrd_threshold ($ip, $interface, $rrd_host_atts{$val}[0], $priority, $file, $threshold);
	    rrd_anomaly ($ip, $interface, $rrd_host_atts{$val}[0], $priority, $file, $persistence);
       }
    }
}

# Get Global Attributes from BD
sub rrd_global {
    my ($interface) = @_;

    my $query = "SELECT * FROM rrd_conf_global;";
    my $stm = $conn->prepare($query);
    $stm->execute();
    
    while (my $row = $stm->fetchrow_hashref) {
	my $val;
	foreach $val (keys %rrd_global_atts){
	    my $file = $rrd_ntop . "/interfaces/" . $interface . "/" . $rrd_global_atts{$val}[0] . ".rrd";
	    
	    next unless (-e $file);
	    next unless ($row->{$val} =~ m/^(.*),(.*),(.*),(.*),(.*)$/);

	    my $threshold = $1;
	    my $priority = $2;
	    my $persistence = $5;
	    
	    rrd_threshold ("GLOBAL", $interface, $rrd_global_atts{$val}[0], $priority, $file, $threshold);
	    rrd_anomaly ("GLOBAL", $interface, $rrd_global_atts{$val}[0], $priority, $file, $persistence);
       }
    }
}

# The Main Function
sub rrd_main {

    while (1) {
	$conn = DBI->connect($dsn, $ds_user, $ds_pass) or die "Can't connect to Database\n";
	open (OUTPUT, ">>$rrd_log") or die "Can't open file log";
	my $interface;
	foreach $interface (split (",", $interfaces)) {
	    rrd_global ($interface);
	    rrd_hosts ($interface);
	}
	close (OUTPUT);
	$conn->disconnect;
	sleep ($rrd_sleep);
    }
}

rrd_main ();
