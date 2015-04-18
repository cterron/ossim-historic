#!/usr/bin/perl

use ossim_conf;
use DBI;
use POSIX;

use strict;
use warnings;

my $debug = 0;

$| = 1;

my $dsn = "dbi:mysql:".$ossim_conf::ossim_data->{"ossim_base"}.":".$ossim_conf::ossim_data->{"ossim_host"}.":".$ossim_conf::ossim_data->{"ossim_port"};
my $dbh = DBI->connect($dsn, $ossim_conf::ossim_data->{"ossim_user"},
$ossim_conf::ossim_data->{"ossim_pass"})
    or die "Can't connect to DBI\n";



sub isIpInNet {

    my ($ip, $nets) = @_;

    my @net_list = split(",", $nets);
    foreach my $n (@net_list)
    {
        my ($net, $mask) = split("/", $n);

        my @a = split(/\./, $ip);
        my $val1 = $a[0]*256*256*256 + $a[1]*256*256 + $a[2]*256 + $a[3];

        @a = split(/\./, $net);
        my $val2 = $a[0]*256*256*256 + $a[1]*256*256 + $a[2]*256 + $a[3];

        if (($val1 >> (32 - $mask)) == ($val2 >> (32 - $mask))) {
            return 1;
        }
    }
    return 0;
}


sub get_nets {

    my $host_ip = shift;
    my @host_networks;

    my $query = "SELECT * FROM net;";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    if ($sth->rows > 0) {
        while (my $row = $sth->fetchrow_hashref) {
            my $net_ips = $row->{"ips"};
            my $net_name = $row->{"name"};
            if (isIpInNet($host_ip, $net_ips)) {
                push(@host_networks,$net_name);
            }
        }
    }
    return @host_networks;
}



my $nessus = $ossim_conf::ossim_data->{"nessus_path"};
my $nessus_user = $ossim_conf::ossim_data->{"nessus_user"};
my $nessus_pass = $ossim_conf::ossim_data->{"nessus_pass"};
my $nessus_host = $ossim_conf::ossim_data->{"nessus_host"};
my $nessus_port = $ossim_conf::ossim_data->{"nessus_port"};
my $nessus_rpt_path = $ossim_conf::ossim_data->{"nessus_rpt_path"};
my $nessus_distributed = $ossim_conf::ossim_data->{"nessus_distributed"};
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

# little security check
#if ($nessus_tmp =~ /vulnmeter/) {`rm -rf $nessus_tmp`};

unless(-e $nessus_tmp && -W $nessus_tmp){
print "No temp dir, creating\n";
mkdir $nessus_tmp;
chmod 0755, $nessus_tmp;
} 

my $row;
my $host_ip;
my $scan_networks = "";
my $query = "";
my $sth;
my $sth2;
my $sth3;
my $temp_target;


if($nessus_distributed) {
    if($debug){ print ("Entering distributed mode, distributed = $nessus_distributed\n")};
    $temp_target="$nessus_tmp/targets.txt";
    my $nessus_tmp_sensors = $nessus_tmp . "sensors/";
    unless(-e $nessus_tmp_sensors && -W $nessus_tmp_sensors){
	print "No sensors temp dir, creating\n";
	mkdir $nessus_tmp_sensors;
	chmod 0755, $nessus_tmp_sensors;
    } 
    $scan_networks = "";
    my @sensors;
    my @active_sensors;
    $query = "select * from sensor;";
    $sth = $dbh->prepare($query);
    $sth->execute();
    while($row = $sth->fetchrow_hashref){
	my $sensor_ip;
	$sensor_ip = $row->{ip};
	push(@sensors, $sensor_ip);
    }
    
    foreach my $sensor (@sensors){
	if($debug){ print ("Selecting networks/hosts for sensor $sensor\n")};
	`rm -f $nessus_tmp_sensors/$sensor.targets.txt; touch $nessus_tmp_sensors/$sensor.targets.txt`;
    }
    $scan_networks = "";
    $query = "select net.name,ips,sensor.ip as sensor_ip from net,net_scan,net_sensor_reference,sensor where net.name = net_scan.net_name and net_scan.plugin_id = 3001 AND net_sensor_reference.net_name = net_scan.net_name AND sensor.name = net_sensor_reference.sensor_name;";
    $sth = $dbh->prepare($query);
    $sth->execute();
    while($row = $sth->fetchrow_hashref){
	my $sensor2 = $row->{sensor_ip};
	my $scan_net = $row->{ips};
	if($debug){ print ("Adding $scan_net\n")};
	system("echo $scan_net >> $nessus_tmp_sensors/$sensor2.targets.txt");
	if($scan_networks ne ""){
	    $scan_networks = $scan_networks . "," . $scan_net;
	} else {
	    $scan_networks = $scan_net;
	}
    }
    $query = "SELECT sensor.ip, inet_ntoa(host_scan.host_ip) as temporal from host_scan,host,sensor,host_sensor_reference where plugin_id = 3001 AND host_sensor_reference.sensor_name = sensor.name AND host_sensor_reference.host_ip = inet_ntoa(host_scan.host_ip) AND host.ip = inet_ntoa(host_scan.host_ip);";
    $sth = $dbh->prepare($query);
    $sth->execute();
    while($row = $sth->fetchrow_hashref){
	$host_ip = $row->{temporal};
	my $sensor2 = $row->{ip};
	if(isIpInNet($host_ip, $scan_networks)){
	    if($debug){ print ("Host defined for nessus scan matching defined network: $host_ip\n")};
	    print "DUP: $host_ip\n";
	} else {
	if($debug){ print ("Adding $host_ip\n")};
	    system("echo $host_ip >> $nessus_tmp_sensors/$sensor2.targets.txt");
	}
    }
    my $sensor;
    my @dirents;
    opendir(DIRECTORY, $nessus_tmp_sensors) || die "can't opendir $nessus_tmp_sensors: $!";
    @dirents = grep { /^[^.]/ && -s "$nessus_tmp_sensors/$_" } readdir(DIRECTORY);
    foreach (@dirents){
	if(/(.*).target.*/){
	    my $temp_sensor = $1;
	    my $running = 0;
	    open (SANITY_CHECK, "$nessus -x -s -q $temp_sensor $nessus_port $nessus_user $nessus_pass|");
	    while(<SANITY_CHECK>){
		if(/Session ID(.*)Targets/){
		    $running = 1;
		    if($debug){ print ("Adding $temp_sensor to active sensors in distributed mode\n")};
		    push(@active_sensors, $temp_sensor);
		} 
	    }
	    if($running == 0){ print "$temp_sensor not running, see above error, disabling\n";}
	    close(SANITY_CHECK);
	}
    }
    print "-----------------\n";
    foreach (@active_sensors){
	my $child_pid = 0;
	$dbh->disconnect;
	$child_pid = fork && next;
	my $temp_active_sensor = $_;
	my $num_hosts = 0;
	open(NUM_HOSTS, "<$nessus_tmp_sensors/$temp_active_sensor.targets.txt");
	while(<NUM_HOSTS>){
	    if(/(.*)\/(.*)/){
		my $i;
		$num_hosts += (2 << (32-$2)-1) - 2; # ignore network & broadcast
	    } else {
		$num_hosts += 1;
	    }
	}
	print "$temp_active_sensor Up and running, starting scan against $num_hosts hosts\n";
	print "-----------------\n";
	`rm -f $nessus_tmp/$temp_active_sensor.STATUS`;
     if($debug){ print ("Going to scan\n")};
     if($debug){ system("cat $nessus_tmp_sensors/$temp_active_sensor.targets.txt")};

	open (RESULT, "$nessus -x -T nsr -q $temp_active_sensor $nessus_port $nessus_user $nessus_pass $nessus_tmp_sensors/$temp_active_sensor.targets.txt $nessus_tmp/$temp_active_sensor.temp_res.nsr|");
	close(RESULT);
	`touch $nessus_tmp/$temp_active_sensor.STATUS`;
`cat $nessus_tmp/$temp_active_sensor.temp_res.nsr >> $nessus_tmp/temp_res.nsr`;
	if(!$child_pid){
	    if($debug){ print ("$temp_active_sensor: I'm done\n")};
	    exit();
	}
    }
    my $theyre_done = 1;
    while($theyre_done){
	my $count_finished = 0;
	foreach(@active_sensors){
	    if(-e "$nessus_tmp/$_.STATUS"){
		$count_finished++;
	    }
	}
	if($count_finished == ($#active_sensors +1)){
	    $theyre_done = 0;
	}
	if($debug){ print ("Still waiting\n")};
	sleep(5);
    }
# Reopen dbh after fork...
    $dbh = DBI->connect($dsn, $ossim_conf::ossim_data->{"ossim_user"}, $ossim_conf::ossim_data->{"ossim_pass"});
    if($debug){ print ("Scan finished\n")};
} else { # non-distributed
    if($debug){ print ("Entering non-distributed mode, distributed = $nessus_distributed\n")};
    $temp_target="$nessus_tmp/targets.txt";
    
    open (FILE, ">$temp_target") || die "Error open() $temp_target\n";
    
    $scan_networks = "";
    if($debug){ print ("Obtaining networks\n")};
    $query = "select name,ips from net, net_scan where net.name = net_scan.net_name and net_scan.plugin_id = 3001;";
    $sth = $dbh->prepare($query);
    $sth->execute();
    while($row = $sth->fetchrow_hashref){
	my $scan_net = $row->{ips};
	if($scan_networks ne ""){
	    $scan_networks = $scan_networks . "," . $scan_net;
	} else {
	    $scan_networks = $scan_net;
	}
    if($debug){ print ("Adding $scan_net\n")};
    $query = "select name,ips from net, net_scan where net.name = net_scan.net_name and net_scan.plugin_id = 3001;";
	print FILE "$scan_net\n";
    }
    if($debug){ print ("Obtaining hosts\n")};
    $query = "SELECT *, inet_ntoa(host_ip) as temporal from host_scan where plugin_id = 3001;";
    $sth = $dbh->prepare($query);
    $sth->execute();
    while($row = $sth->fetchrow_hashref){
	$host_ip = $row->{temporal};
	if(isIpInNet($host_ip, $scan_networks)){
	    print "DUP: $host_ip. Please check your config.\n";
	} else {
    if($debug){ print ("Adding $host_ip\n")};
	    print FILE "$host_ip\n";
	}
    }
    print $scan_networks;
    
    close(FILE);
    
    if($debug){ print ("Starting non-distributed scan\n")};
    if($debug){ print ("Going to scan:\n")};
    if($debug){ system("cat $temp_target")};
    open (RESULT, "$nessus -x -T nsr -q $nessus_host $nessus_port $nessus_user $nessus_pass $temp_target $nessus_tmp/temp_res.nsr|");
    close(RESULT);
} # End distributed


if($debug){ print ("Converting and blah blah blah\n")};
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

open (VULNS, "/bin/cat $nessus_tmp/temp_res.txt |");

while (<VULNS>) {

    if (/^\+\s+(\d+\.\d+\.\d+\.\d+) :/) {
      $host = $1; 
      $hv{$host}=0;
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

close(VULNS);

# cleanup tables
$query = "UPDATE net_vulnerability SET vulnerability = 0;";
$sth = $dbh->prepare($query);
$sth->execute();


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

    my @vuln_networks = get_nets($ip);

    foreach $net (@vuln_networks){
    $query = "UPDATE net_vulnerability set vulnerability = vulnerability + $vulnerability where net = \"$net\";";
    $sth = $dbh->prepare($query);
    $sth->execute();
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

opendir(TEMPORAL, "$today");
my @temp_dirents = grep { /^[^.]/ && -d "$today/$_" } readdir(TEMPORAL);
foreach (@temp_dirents){
    chmod 0755, "$today/$_";
}

close(TEMPORAL);


if (-e "$nessus_tmp/temp_res.$today_date.nsr"){`rm $nessus_tmp/temp_res.$today_date.nsr`;}
rename "$nessus_tmp/temp_res.nsr", "$nessus_tmp/temp_res." . $today_date . ".nsr";
unlink "$nessus_rpt_path/last"; 
`ln -s $today $nessus_rpt_path/last`;

my %refs = ();
my @temp_array;
my $otro_indice;

my $y_dale = $nessus_tmp . "/temp_res." . $today_date . ".nsr";

open(TEMP_BIS, "<$y_dale") || die "Error open() $y_dale \n";


while(<TEMP_BIS>){
    
    if(/^([^\|]*)\|[^\|]*\|([^\|]*)\|.*/){
	if(!exists($refs{$1})){
	    $refs{$1} = "";
	}
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

close(TEMP_BIS);

$dbh->disconnect;

print "Did you see the rm -rf within this code ? I hope you're scared by now.\n";


