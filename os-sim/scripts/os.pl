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
my $p0f = $ossim_conf::ossim_data->{"p0f_path"};

my $when;
my $host;
my $os;

open(P0F,"$p0f -t -q|");

while(<P0F>){
my $time = localtime;
#    if(/<(.*)>\s+(.*):\d+\s+-[\s+(.*)\s+\(.*|(.*)^(]/){
    if(/<(.*)>\s+(.*):\d+\s+-\s(.*)\s+$/){
        $when = $1;
        $host = $2;
        $os = $3;
        if($os =~ /(.*)\s$/){
        $os = $1;
        }
        if($os =~ /(.*)\s+\(.*/){
        $os = $1;
        }
        if($os =~ /(.*)\s\(or\s(.*)\)/){
        $os = $1 . "|" . $2;
        }
        my $query = "SELECT * FROM host_os WHERE ip = '$host';";
        my $sth = $dbh->prepare($query);
        $sth->execute();
        if (my $row = $sth->fetchrow_hashref) {
        my $prev_os = $row->{previous};
            if($os ne $prev_os && $row->{anom} == 0){
                $query = "UPDATE host_os SET anom = 1, os = '$os', os_time = '$time' WHERE ip = '$host';";
                my $sth = $dbh->prepare($query);
                $sth->execute();
            }
        } else {
            $query = "INSERT INTO host_os(ip, os, previous, anom, os_time) VALUES('$host', '$os', '$os', 0, '$time');";
            my $sth = $dbh->prepare($query);
            $sth->execute();
        }
    }
}

close(P0F);
$dbh->disconnect;
exit 0;

