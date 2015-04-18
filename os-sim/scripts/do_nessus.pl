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
my $nessus_rpt_path = $ossim_conf::ossim_data->{"nessus_rpt_path"};
my $nessus_tmp = $nessus_rpt_path . "/tmp/";

my $today_date =  strftime "%Y%m%d", gmtime;
my $today = $nessus_rpt_path .  strftime "%Y%m%d", gmtime;
$today .= "/";

unless(-d $nessus_rpt_path && -W $nessus_rpt_path){
die "Need write permission to $nessus_rpt_path and almost all of it's ubdirs\n";
}

unless(-e $today && -W $today){
print "Creating todays scan result dir\n";
mkdir $today;
chmod 0755, $today;
}

unless(-e $nessus_tmp && -W $nessus_tmp){
print "No temp dir, creating\n";
mkdir $nessus_tmp;
chmod 0755, $nessus_tmp;
}



my $temp_target="$nessus_tmp/targets.txt";

open (FILE, ">$temp_target") || die "Error open() $temp_target\n";

my $row;
my $host_ip;
my $query = "SELECT *, inet_ntoa(host_ip) as temporal from host_scan where plugin_id = 3001;";
my $sth = $dbh->prepare($query);
$sth->execute();
while($row = $sth->fetchrow_hashref){
$host_ip = $row->{temporal};
print FILE "$host_ip\n";
}

close(FILE);

open (RESULT, "$nessus -T nsr -q $nessus_host $nessus_port $nessus_user $nessus_pass $temp_target $nessus_tmp/temp_res.nsr|");
close(RESULT);


my $tempfile = "$nessus_tmp/temp_res." . $today_date . ".nsr";

# Sirve para algo declarar las variables en perl !!!!!?
my $host;
my %hv = ();
my $risk;
my $ip;
my $vulnerability;
my $rows;
my $row2;
my $row3;
my $net;
my $rv;
my $key;

`$nessus -T text -i $nessus_tmp/temp_res.nsr -o $nessus_tmp/temp_res.txt`;

open (JODER, "/bin/cat $nessus_tmp/temp_res.txt |");

while (<JODER>) {

    if (/^\+\s+(\d+\.\d+\.\d+\.\d+) :/) {
      $host = $1; 
      $hv{$host}=0
    }

#if (/^ \. [^(List)]/);

    if (/Risk factor : (.*)/) { 
        $risk=$1; 
	    $risk =~ s/ \(.*|if.*//g; 
	    $risk =~ s/ //g; 
        
	    if ($risk eq "Verylow/none") { $rv=1 }
	    if ($risk eq "Low") { $rv=2 }
	    if ($risk eq "Low/Medium") { $rv=3 }
	    if ($risk eq "Medium/Low") { $rv=4 }
	    if ($risk eq "Medium") { $rv=5 }
	    if ($risk eq "Medium/High") { $rv=6 }
	    if ($risk eq "High/Medium") { $rv=7 }
	    if ($risk eq "High") { $rv=8 }
	    if ($risk eq "Veryhigh") { $rv=9 }

        $hv{$host} += $rv;
    }
}

close(JODER);

foreach $key ( keys(%hv) ) {
    
    $ip = $key;
    $vulnerability = $hv{$key};

    #print $key, "-", $hv{$key}, "\n";
    
    # delete to update values
    $query = "DELETE FROM host_vulnerability WHERE ip = '$ip'";
    $sth = $dbh->prepare($query);
    $sth->execute();

    #
    # update host levels
    # 
    $query = "INSERT INTO host_vulnerability 
                VALUES ('$ip', '$vulnerability');";
    $sth = $dbh->prepare($query);
    $sth->execute();


    #
    # update net levels
    # 
    $vulnerability = 0;
    $rows = 0;

    $query = "SELECT * FROM net;";
    $sth = $dbh->prepare($query);
    $sth->execute();
    while ($row = $sth->fetchrow_hashref)
    {
        $net = $row->{name};

        $query = "SELECT * FROM net_host_reference WHERE net_name='$net'";
        $sth = $dbh->prepare($query);
        $sth->execute();
        while ($row2 = $sth->fetchrow_hashref) {
            $host_ip = $row2->{host_ip};
            $query = "SELECT * FROM host_vulnerability
                            WHERE ip = '$host_ip'";
            $sth = $dbh->prepare($query);
            $sth->execute();
            if ($row3 = $sth->fetchrow_hashref) {
                $vulnerability += $row3->{vulnerability};
                $rows += 1;
            }
        }
        if ($rows) {
            $query = "UPDATE net_vulnerability 
                SET vulnerability = $vulnerability
                WHERE net = '$net'";
            $sth = $dbh->prepare($query);
            $sth->execute();
        }
        $rows = 0;
    }
}

if(-e $tempfile){
print "Appending results\n";
`cat $nessus_tmp/temp_res.$today_date.nsr >> $nessus_tmp/temp_res.nsr`;
}
if(-e $today && -W $today){
print "Deleting todays scan result dir\n";
`rm -rf $today`;
}
`$nessus -T html_graph -i $nessus_tmp/temp_res.nsr -o $today`;

chmod 0755, $today;

open (TEMPORAL, "<$temp_target");

while(<TEMPORAL>){
$_ =~ s/\./_/g;
chop;
chmod 0755, "$today/$_";

}

close(TEMPORAL);

`rm $nessus_tmp/temp_res.$today_date.nsr`;
rename "$nessus_tmp/temp_res.nsr", "$nessus_tmp/temp_res." . $today_date . ".nsr";
unlink "$nessus_rpt_path/last"; 
`ln -s $today $nessus_rpt_path/last`;

my %refs = ();
my @temp_array;
my $otro_indice;

my $y_dale = $nessus_tmp . "/temp_res." . $today_date . ".nsr";

open(OTRO_TEMP, "<$y_dale") || die "Error open() $y_dale \n";


while(<OTRO_TEMP>){

if(/^([^\|]*)\|[^\|]*\|([^\|]*)\|.*/){
if($refs{$1} =~ /$2/){
;
} else {
$refs{$1} .= "$2|";
}
}

}

if(keys %refs){
print "Updating...\n";
foreach $key (keys %refs){
chop($refs{$key});
my $query = "DELETE FROM host_plugin_sid where host_ip = inet_aton(\"$key\") and plugin_id = 3001;";
my $sth = $dbh->prepare($query);
$sth->execute();
@temp_array = split(/\|/,$refs{$key});
foreach $otro_indice (0 .. $#temp_array) {
my $query = "INSERT INTO host_plugin_sid(host_ip, plugin_id, plugin_sid) values(inet_aton(\"$key\"), 3001, $temp_array[$otro_indice]);";
my $sth = $dbh->prepare($query);
$sth->execute();
}
}
}

close(OTRO_TEMP);

$dbh->disconnect;

print "Did you see the rm -rf within this code ? I hope you're scared by now.\n";

