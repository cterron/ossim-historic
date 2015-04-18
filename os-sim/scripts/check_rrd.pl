#!/usr/bin/perl

use strict;
use warnings;
use Sys::Syslog;

use DBI;
use ossim_conf;

$| = 1;

my $SLEEP=1800;

my $interface = $ossim_conf::ossim_data->{"ossim_interface"};
my $rrdpath_ntop = $ossim_conf::ossim_data->{"rrdpath_ntop"};
my $rrdpath = $rrdpath_ntop . "/interfaces/" . $interface . "/hosts/";
my $rrdpath_global = $rrdpath_ntop . "/interfaces/" . $interface . "/";

my $dsn = 'dbi:mysql:'.$ossim_conf::ossim_data->{"ossim_base"}.':'.$ossim_conf::ossim_data->{"ossim_host"}.':'.  $ossim_conf::ossim_data->{"ossim_port"};
my $dbh = DBI->connect($dsn, $ossim_conf::ossim_data->{"ossim_user"}, $ossim_conf::ossim_data->{"ossim_pass"}) or 
    die "Can't connect to DBI\n";

my %rrd_values= 
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
    "ip_mail_rcvd_bytes" => ["IP_MailRcvdBytes"],
    "mrtg_a" => ["pktSent"],
    "mrtg_c" => ["pktSent"]); 

sub is_over_threshold {
my ($real_ip, $rrd, $real_threshold, $start, $end) = @_;
my $type = "ntop";
my $what = "MAX";
my $res;
my $execute = $ossim_conf::ossim_data->{"base_dir"} . "/scripts/get_rrd_value.pl";
my $time = localtime;

my $real_rrd = $rrd_values{$rrd}[0];

my $file = $rrdpath . $real_ip . "/" . $real_rrd . ".rrd";
if(stat($file)) {
    $res = `$execute $start $end $file $type $what`;
    if($res > $real_threshold){
    syslog('auth.info','RRD_anomaly: host: %s what: %s ', $real_ip, $rrd);
    print "$real_ip: $rrd exceeds threshold by ",$res - $real_threshold,"\n"; 
    my $query = "SELECT * FROM rrd_anomalies where ip = '$real_ip' and what = '$rrd' and acked = 0;";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    if (my $row = $sth->fetchrow_hashref) {
        my $count = $row->{count} + 1;
        $query = "UPDATE rrd_anomalies set count = $count, over = $res - $real_threshold where ip = '$real_ip' and what = '$rrd' and acked = 0;";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    } else {
        $query = "INSERT INTO rrd_anomalies(ip, what, count, anomaly_time, range, over, acked) VALUES('$real_ip', '$rrd', 1, '$time', 'day', $res - $real_threshold, 0);";
        my $sth = $dbh->prepare($query);
        $sth->execute();
        }
   }
}
}



# What ips to check
# 
while(1){
    my $query = "SELECT * FROM rrd_conf;";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    while (my $row = $sth->fetchrow_hashref) {
       my $ip = $row->{ip};
       my $val;
       my $element;

       foreach $val (keys %rrd_values){
            if($row->{$val} =~ m/^(.*),(.*),(.*),(.*)$/){ 
            is_over_threshold ($ip, $val,$1, "N-1H", "N" );
            }
        }
    }
    sleep($SLEEP);
}

$dbh->disconnect;
exit 0;

