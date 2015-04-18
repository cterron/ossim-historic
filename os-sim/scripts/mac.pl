#!/usr/bin/perl

use strict;
use warnings;
use Sys::Syslog;

use DBI;
use ossim_conf;

$| = 1;

my $dsn = 'dbi:mysql:'.$ossim_conf::ossim_data->{"ossim_base"}.':'.$ossim_conf::ossim_data->{"ossim_host"}.':'.  $ossim_conf::ossim_data->{"ossim_port"};
my $dbh = DBI->connect($dsn, $ossim_conf::ossim_data->{"ossim_user"}, $ossim_conf::ossim_data->{"ossim_pass"}) or 
    die "Can't connect to DBI\n";

my $arpwatch = $ossim_conf::ossim_data->{"arpwatch_path"};
my $interface = $ossim_conf::ossim_data->{"ossim_interface"};
my $my_touch = $ossim_conf::ossim_data->{"touch_path"};
my $has_ip = $ossim_conf::ossim_data->{"has_ip"};

my $when;
my $host;
my $mac;
my $temp_mac;
my $temp_previous;

`/bin/rm -f /var/log/arp.dat; $my_touch /var/log/arp.dat`;
if($has_ip){
open(ARPWATCH,"$arpwatch -d -i $interface -f /var/log/arp.dat 2>&1|");
} else {
print "No IPv4 Address asigned, make sure you patched arpwatch with included
arpwatch patch\n";
open(ARPWATCH,"$arpwatch -w -d -i $interface -f /var/log/arp.dat 2>&1|");
}

while(<ARPWATCH>){
my $time = localtime;
    if(/\s+ip\saddress:\s(.*)/){
        $host = $1;
        if(<ARPWATCH> =~ m/\s+(ethernet|mac)\saddress:\s(.*)/) {
            $mac = $2;
        }
        $mac .= "|";
        if(<ARPWATCH> =~ m/\s+(ethernet|mac)\svendor:\s(.*)/) {
        $mac .= "$2";
        }
        my $query = "SELECT * FROM host_mac WHERE ip = '$host';";
        my $sth = $dbh->prepare($query);
        $sth->execute();
        if (my $row = $sth->fetchrow_hashref) {
            my $prev_mac = $row->{previous};
            if($prev_mac =~ m/(.*)|.*/){
                $temp_previous = $1;
            }
            if($mac =~ m/(.*)|.*/){
                $temp_mac = $1;
            }
                if($temp_mac ne $temp_previous && $row->{anom} == 0){
                     $query = "UPDATE host_mac SET anom = 1, mac = '$mac', mac_time = '$time' WHERE ip = '$host';";
                     my $sth = $dbh->prepare($query);
                     $sth->execute();
                }
        } else {
            $query = "INSERT INTO host_mac(ip, mac, previous, anom, mac_time) VALUES('$host', '$mac', '$mac', 0, '$time');";
            $sth = $dbh->prepare($query);
            $sth->execute();
        }
    } elsif(/.*ethernet\smismatch\s(.*)\s(.*)\s\((.*)\).*/){
     $host = $1;
     my $prev_mac = $2;
     $mac = $3;
     my $query = "UPDATE host_mac SET anom = 1, mac = '$mac', previous = '$prev_mac', time = '$time' WHERE ip = '$host';";
     my $sth = $dbh->prepare($query);
     $sth->execute();
    }
}

close(ARPWATCH);
$dbh->disconnect;
exit 0;

