#!/usr/bin/perl

use strict;

use DBI;
use ossim_conf;

my $dsn = "dbi:mysql:".$ossim_conf::ossim_data->{"ossim_base"}.":".$ossim_conf::ossim_data->{"ossim_host"}.":".$ossim_conf::ossim_data->{"ossim_port"};
my $dbh = DBI->connect($dsn, $ossim_conf::ossim_data->{"ossim_user"}, $ossim_conf::ossim_data->{"ossim_pass"})
    or die "Can't connect to DBI\n";


my $compromise = 1;
my $attack = 1;

my $query = "SELECT sum(compromise) FROM host_qualification;";
my $sth = $dbh->prepare($query);
$sth->execute();
if ($sth->rows > 0) {
    my $row = $sth->fetchrow_hashref;
    $compromise = $row->{"sum(compromise)"}; 
}

$query = "SELECT sum(attack) FROM host_qualification;";
$sth = $dbh->prepare($query);
$sth->execute();
if ($sth->rows > 0) {
    my $row = $sth->fetchrow_hashref;
    $attack = $row->{"sum(attack)"};
}

if($compromise < 1){ $compromise = 1};
if($attack < 1){ $attack = 1};



print "$compromise\n$attack\n0\n";
print "Stats from global\n\n";

exit 0;

