#!/usr/bin/perl

use strict;
use warnings;
use Sys::Syslog;

use DBI;
use ossim_conf;

$| = 1;

# Define also at www/control_panel/index.php so stats are displayed correctly.
#my $SLEEP=1800; 
my $SLEEP=900;  # 1/4 hour

my $interface = $ossim_conf::ossim_data->{"ossim_interface"};
my $rrdpath_ntop = $ossim_conf::ossim_data->{"rrdpath_ntop"};
my $rrdpath = $rrdpath_ntop . "/interfaces/" . $interface . "/";

my $dsn = 'dbi:mysql:'.$ossim_conf::ossim_data->{"ossim_base"}.':'.$ossim_conf::ossim_data->{"ossim_host"}.':'.  $ossim_conf::ossim_data->{"ossim_port"};
my $dbh = DBI->connect($dsn, $ossim_conf::ossim_data->{"ossim_user"}, $ossim_conf::ossim_data->{"ossim_pass"}) or 
    die "Can't connect to DBI\n";

my %rrd_values_global = ("active_host_senders_num" => ["activeHostSendersNum"],
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


sub is_over_threshold {
my ($rrd, $real_threshold, $persistence, $start, $end) = @_;
my $type = "ntop";
my $what = "MAX";
my $res;
my $execute = $ossim_conf::ossim_data->{"base_dir"} . "/scripts/get_rrd_value.pl";
my $time = localtime;

my $real_rrd = $rrd_values_global{$rrd}[0];

my $file = $rrdpath . $real_rrd . ".rrd";
if(stat($file)) {
    $res = `$execute $start $end $file $type $what`;
    if($res > $real_threshold){
    syslog('auth.info','RRD_anomaly: host: global what: %s ', $rrd);
    print "global: $rrd exceeds threshold by ",$res - $real_threshold,"\n"; 
    my $query = "SELECT * FROM rrd_anomalies_global where what = '$rrd' and acked = 0;";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    if (my $row = $sth->fetchrow_hashref) {
        my $count = $row->{count} + 1;
        $query = "UPDATE rrd_anomalies_global set count = $count, over = $res - $real_threshold where what = '$rrd' and acked = 0;";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    } else {
        $query = "INSERT INTO rrd_anomalies_global( what, count, anomaly_time, range, over, acked) VALUES('$rrd', 1, '$time', 'day', $res - $real_threshold, 0);";
        my $sth = $dbh->prepare($query);
        $sth->execute();
        }
   } # Over threshold
   else { # Reset without taking persistence into account.
    my $query = "SELECT * FROM rrd_anomalies_global where what = '$rrd' and acked = 0;";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    if (my $row = $sth->fetchrow_hashref) {
        my $count = $row->{count};
        if($count < $persistence) {
            $query = "DELETE from rrd_anomalies_global where what = '$rrd' and acked = 0;";
            my $sth = $dbh->prepare($query);
            $sth->execute();
            } # if count < persistence
            else {
            $count += 1;
            $query = "UPDATE rrd_anomalies_global set count = $count where what = '$rrd' and acked = 0;";
            my $sth = $dbh->prepare($query);
            $sth->execute();
            } # ifelse count < persistence
    } # If rows
   } # Else
} # stat file
} # sub



# What rrds to check
# 
while(1){
    my $query = "SELECT * FROM rrd_conf_global;";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    while (my $row = $sth->fetchrow_hashref) {
       my $val;
       my $element;

       foreach $val (keys %rrd_values_global){
            if($row->{$val} =~ m/^(.*),(.*),(.*),(.*),(.*)$/){ 
            is_over_threshold ($val,$1,$5, "N-1H", "N" );
            }
        }
    }
    sleep($SLEEP);
}

$dbh->disconnect;
exit 0;

