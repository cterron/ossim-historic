#!/usr/bin/perl

use ossim_conf;
use DBI;
use POSIX;

use strict;
use warnings;

$| = 1;

my $dsn = "dbi:mysql:".$ossim_conf::ossim_data->{"ossim_base"}.":".$ossim_conf::ossim_data->{"ossim_host"}.":".$ossim_conf::ossim_data->{"ossim_port"};
my $dbh = DBI->connect($dsn, $ossim_conf::ossim_data->{"ossim_user"},
$ossim_conf::ossim_data->{"ossim_pass"})
    or die "Can't connect to DBI\n";


my $nessus = $ossim_conf::ossim_data->{"nessus_path"};
my $nessus_user = $ossim_conf::ossim_data->{"nessus_user"};
my $nessus_pass = $ossim_conf::ossim_data->{"nessus_pass"};
my $nessus_host = $ossim_conf::ossim_data->{"nessus_host"};
my $nessus_port = $ossim_conf::ossim_data->{"nessus_port"};


open (PLUGINS, "$nessus -q -p $nessus_host $nessus_port $nessus_user $nessus_pass|");

my @plugin_rel_db = ();
my %plugin_rel_hash = ();
my $index;
my $key;

while(<PLUGINS>){
if(/^([^\|]*)\|[^\|]*\|([^\|]*)\|.*/){
$plugin_rel_hash{$1} = $2;
}
}

close(PLUGINS);

my $query = "SELECT * from plugin_sid where plugin_id = 3001;";

my $sth = $dbh->prepare($query);
$sth->execute();

my $row;

while($row = $sth->fetchrow_hashref){
if(exists($plugin_rel_hash{$row->{sid}})){
delete $plugin_rel_hash{$row->{sid}};
}
}

$query = "INSERT INTO plugin_sid(plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES ";

if(keys %plugin_rel_hash){
print "Updating...\n";
foreach $key (keys %plugin_rel_hash){
print "$key:$plugin_rel_hash{$key}\n";
$plugin_rel_hash{$key} =~ s/'/''/; 
$query .= "(3001, $key, NULL, NULL, 2, 5, 'nessus: $plugin_rel_hash{$key}'),";
}

chop($query);
$query .= ";";

$sth = $dbh->prepare($query);
$sth->execute();
} else {
print "DB is up to date\n";
}
