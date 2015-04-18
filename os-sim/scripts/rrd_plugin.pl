#!/usr/bin/perl

# Script for the plugin rrd_threshold y rrd_anomaly
#
# 2004-02-11 Fabio Ospitia Trujillo <fot@ossim.net>


use DBI;
use ossim_conf;
use Socket;

sub byebye {
    print "$0: forking into background...\n";
    exit;
}

fork and byebye;

my $pidfile = "/var/run/rrd_plugin.pid";

sub die_clean {
    unlink $pidfile;
    exit;
}

open(PID, ">$pidfile") or die "Unable to open $pidfile\n";
print PID $$;
close(PID);

# Data Source 
my $ds_type = "mysql";
my $ds_name = $ossim_conf::ossim_data->{"ossim_base"};
my $ds_host = $ossim_conf::ossim_data->{"ossim_host"};
my $ds_port = $ossim_conf::ossim_data->{"ossim_port"};
my $ds_user = $ossim_conf::ossim_data->{"ossim_user"};
my $ds_pass = $ossim_conf::ossim_data->{"ossim_pass"};

# Interfaces (comma separated)
my $main_interface = $ossim_conf::ossim_data->{"ossim_interface"};
my $interfaces = "$main_interface";

# Anomaly
my $rrd_sleep = 300;
my $rrd_interval = 300;
my $rrd_range = "1H";
my $rrd_worm = 1;

# RRD Path Files
my $rrd_bin = $ossim_conf::ossim_data->{"rrdtool_path"} . "/rrdtool";
my $rrd_ntop = $ossim_conf::ossim_data->{"rrdpath_ntop"};
my $rrd_log = "/var/log/rrd_plugin.log";

# [threshold, priority, persistence]

my %rrd_worm_atts = 
    ("synPktsSent" => [4,5,1],
     "synPktsRcvd" => [3,5,1],
     "totContactedSentPeers" => [1,5,1],
     "totContactedRcvdPeers" => [1,5,1],
     "web_sessions" => [5,5,1],
     "mail_sessions" => [1,5,1],
     "nb_sessions" => [1,5,1]);

#Host to exclude
my @rrd_worm_hosts = ();

my $dsn = "dbi:" . $ds_type . ":" . $ds_name . ":" . $ds_host . ":" . $ds_port . ":";
my $conn;

sub rrd_worm_has_host {
    my ($host) = @_;

    foreach $var (@rrd_worm_hosts) {
	if ($host eq $var) {
	    return 1;
	}
    }
    return 0;
}

# Return the average
sub rrd_graph_average {
    my ($file, $what, $type) = @_;

    my @result= `$rrd_bin graph /dev/null -s N-$rrd_range -e N -X 2 DEF:obs=$file:$what:AVERAGE PRINT:obs:$type:%lf`;
 
    chop ($result[1]);

    return $result[1];
}

sub rrd_fetch_hwpredict_by_time {
    my ($file, $stime, $etime) = @_;

    my $result = `$rrd_bin fetch $file HWPREDICT -s $stime -e $etime | grep $etime`;

    my @tmp = split (" ", $result); 

    return $tmp[1];
}

sub rrd_fetch_devpredict_by_time {
    my ($file, $stime, $etime) = @_;

    my $result = `$rrd_bin fetch $file DEVPREDICT -s $stime -e $etime | grep $etime`;

    my @tmp = split (" ", $result); 

    return $tmp[1];
}

sub rrd_fetch_average_by_time {
    my ($file, $stime, $etime) = @_;

    my $result = `$rrd_bin fetch $file AVERAGE -s $stime -e $etime | grep $etime`;

    my @tmp = split (" ", $result); 

    return $tmp[1];
}

sub rrd_fetch_max_by_time {
    my ($file, $stime, $etime) = @_;

    my $result = `$rrd_bin fetch $file MAX -s $stime -e $etime | grep $etime`;

    my @tmp = split (" ", $result); 

    return $tmp[1];
}

# Return the last faliure interval
sub rrd_fetch_last_failure {
    my ($file, $range) = @_;
    my @result;

    my @failures = `$rrd_bin fetch $file FAILURES -s N-$rrd_range -e N`;

    my $empty = 0;
    my $var;
    foreach $var (@failures) {
	unless ($var =~ m/(^\d+):.*1\.0000000000e\+00.*/) {
	    $empty = 1;
	    next;
	}

	@result = () if ($empty);
	push (@result, $1);
	$empty = 0;
    }

    return @result;
}

# Return true if is anomaly
sub rrd_anomaly {
    my ($ip, $interface, $att, $priority, $file, $persistence) = @_;

    my @failure = rrd_fetch_last_failure ($file, $rrd_range);
    return 0 unless (@failure);

    my $curr_time = time ();
    my $last_time = int ($curr_time / $rrd_interval) * $rrd_interval;
    my $first_time = $last_time - ($persistence * $rrd_interval);

    my $first_failure = $failure[$#failure - $persistence];
    my $last_failure = $failure[$#failure];

    return 0 if (($last_failure != $last_time) || ($first_failure != $first_time));

    my $hwpredict = rrd_fetch_hwpredict_by_time ($file, $last_failure - 1, $last_failure);
    my $devpredict = rrd_fetch_devpredict_by_time ($file, $last_failure - 1, $last_failure);
    #my $average = rrd_fetch_average_by_time ($file, $last_failure - 1, $last_failure);
    my $max = rrd_fetch_max_by_time ($file, $last_failure - 1, $last_failure);

    # If average is by excess
    return 0 unless ($max > ($hwpredict + (2 * $devpredict)));

    print OUTPUT "rrd_anomaly: $curr_time $ip $interface $att $priority $last_failure\n";

    return 1;
}

# Return true if is threshold
sub rrd_threshold {
    my ($ip, $interface, $att, $priority, $file, $threshold) = @_;
    my $res = rrd_graph_average ($file, "counter", "MAX");

    return 0 unless ($res > $threshold);
    my $curr_time = time ();

    print OUTPUT "rrd_threshold: $curr_time $ip $interface $att $priority " . ($res - $threshold) . "\n";

    return 1;
}

sub rrd_config {
    my ($interface) = @_;

    my $query = "SELECT INET_NTOA(ip) AS ip, rrd_attrib, threshold, priority, persistence FROM rrd_config;";
    my $stm = $conn->prepare($query);
    $stm->execute();

    while (my $row = $stm->fetchrow_hashref) {
	my $hl = $row->{ip};
	my $att = $row->{rrd_attrib};
	my $threshold = $row->{threshold};
	my $priority = $row->{priority};
	my $persistence = $row->{persistence};

	if (ip == 0) {
	    my $file = $rrd_ntop . "/interfaces/" . $interface . "/" . $att . ".rrd";
	    next unless (-e $file);

	    rrd_threshold ("GLOBAL", $interface, $att, $priority, $file, $threshold);
	    rrd_anomaly ("GLOBAL", $interface, $att, $priority, $file, $persistence);
	} else {
	    my $ip = inet_aton ($hl);

	    my $dir = "$ip";
	    $dir =~ tr/\./\//;
	    my $file = $rrd_ntop . "/interfaces/" . $interface . "/hosts/". $dir . "/" . $atts . ".rrd";

	    next unless (-e $file);
	    
	    rrd_threshold ($ip, $interface, $att, $priority, $file, $threshold);
	    rrd_anomaly ($ip, $interface, $att, $priority, $file, $persistence);
	}
    }

    if ($rrd_worm == 1) {
	foreach $att (keys %rrd_worm_atts){
	    my $threshold = $rrd_worm_atts{$att}[0];
	    my $priority = $rrd_worm_atts{$att}[1];
	    my $persistence = $rrd_worm_atts{$att}[2];

	    my @result= `find $rrd_ntop/interfaces/$interface | grep $att`;
	    foreach $file (@result) {
		my @tmp = split ("/", $file);

		$failure[$#failure];

		my $ip = "$tmp[$#tmp - 4].$tmp[$#tmp - 3].$tmp[$#tmp - 2].$tmp[$#tmp - 1]";

		next if (rrd_worm_has_host ($ip));

		chomp($file);

		rrd_threshold ($ip, $interface, $att, $priority, $file, $threshold);
		rrd_anomaly ($ip, $interface, $att, $priority, $file, $persistence);
	    }
	}
    }
}

# The Main Function
sub rrd_main {

    while (1) {
	$conn = DBI->connect($dsn, $ds_user, $ds_pass) or die "Can't connect to Database\n";
	open (OUTPUT, ">>$rrd_log") or die "Can't open file log";
	my $interface;
	foreach $interface (split (",", $interfaces)) {
	    rrd_config ($interface);
	}
	close (OUTPUT);
	$conn->disconnect;
	sleep ($rrd_sleep);
    }
}

rrd_main ();
