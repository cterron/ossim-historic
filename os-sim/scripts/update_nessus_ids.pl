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


open (PLUGINS, "$nessus -x -q -p $nessus_host $nessus_port $nessus_user $nessus_pass|");

my @plugin_rel_db = ();
my %plugin_rel_hash = ();
my %plugin_prio_hash = ();
my $index;
my $key;

while(<PLUGINS>){
if(/^([^\|]*)\|[^\|]*\|([^\|]*)\|.*\\n(.*)$/){
$plugin_rel_hash{$1} = $2;
my $risk_level = 2;
my $temp_risk = $3;
my $temp_plugin_id = $1;

    if ($temp_risk =~ /Risk factor : (.*)/) {
    my $risk=$1; 
    $risk =~ s/ \(.*|if.*//g; 
    $risk =~ s/ //g;        
    if ($risk eq "Verylow/none") { $risk_level = 1 }
    if ($risk eq "Low") { $risk_level = 1 }
    if ($risk eq "Low/Medium") { $risk_level = 2 }
    if ($risk eq "Medium/Low") { $risk_level = 2 }
    if ($risk eq "Medium") { $risk_level = 3 }
    if ($risk eq "Medium/High") { $risk_level = 3 }
    if ($risk eq "High/Medium") { $risk_level = 4 }
    if ($risk eq "High") { $risk_level = 4 }
    if ($risk eq "Veryhigh") { $risk_level = 5 }
    }

$plugin_prio_hash{$temp_plugin_id} = $risk_level; 
}
}

close(PLUGINS);
print "plugins fetched\n";

my $query = "SELECT * from plugin_sid where plugin_id = 3001;";

my $sth = $dbh->prepare($query);
$sth->execute();

my $row;

while($row = $sth->fetchrow_hashref){
if(exists($plugin_rel_hash{$row->{sid}})){
delete $plugin_rel_hash{$row->{sid}};
delete $plugin_prio_hash{$row->{sid}};
}
}

$query = "INSERT INTO plugin_sid(plugin_id, sid, category_id, class_id, reliability, priority, name) VALUES ";

if(keys %plugin_rel_hash){
print "Updating...\n";
foreach $key (keys %plugin_rel_hash){
print "$key:$plugin_rel_hash{$key}:$plugin_prio_hash{$key}\n";
#$plugin_rel_hash{$key} =~ s/'/''/; 
$plugin_rel_hash{$key} =~ s/'/\\'/gs;
$plugin_rel_hash{$key} =~ s/"/\\"/gs;
$query .= "(3001, $key, NULL, NULL, $plugin_prio_hash{$key}, 7, 'nessus: $plugin_rel_hash{$key}'),";
}

chop($query);
$query .= ";";

$sth = $dbh->prepare($query);
$sth->execute();
} else {
print "DB is up to date\n";
}
