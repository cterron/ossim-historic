#!/usr/bin/perl

use strict;

use DBI;
use ossim_conf;

my $net_name = "";
if (!$ARGV[0]) {
    print("Usage: ./read-data.pl <net_name>\n");
    exit 1;
} else {
    $net_name = $ARGV[0];
}

my $dsn = "dbi:mysql:".$ossim_conf::ossim_data->{"ossim_base"}.":".$ossim_conf::ossim_data->{"ossim_host"}.":".$ossim_conf::ossim_data->{"ossim_port"};
my $dbh = DBI->connect($dsn, $ossim_conf::ossim_data->{"ossim_user"}, $ossim_conf::ossim_data->{"ossim_pass"})
    or die "Can't connect to DBI\n";

my $attack = 1;
my $compromise =1;

my $query = "SELECT * FROM net_qualification where net_name = '$net_name';";
my $sth = $dbh->prepare($query);
$sth->execute();
if ($sth->rows > 0) {
    my $row = $sth->fetchrow_hashref;
    $compromise = $row->{compromise}; 
    $attack = $row->{attack};
    
    print "$compromise\n$attack\n\n";
    print "Stats from $net_name\n\n";
} else {

if($compromise < 1){ $compromise = 1};
if($attack < 1){ $attack = 1};

    print "$compromise\n$attack\n\n";
    print "Stats from $net_name\n\n";
}

exit 0;

