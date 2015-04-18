#!/usr/bin/perl

# Script for ACID cache
#
# 2004-05-05 Fabio Ospitia Trujillo <fot@ossim.net>
# 2004-05-25 Bugfixes & improvements: DK <dk@ossim.net>
#
# NOTE: Need 'wget' program

use DBI;
use ossim_conf;
my $config_file = "/etc/ossim/framework/ossim.conf";

sub check_var($ $) 
{
    my $name = shift;
    my $var  = shift;

    if (!$var) {
        print "You must set '$name' variable at $config_file\n";
        exit;
    }
}

# Full path for the program 'wget'
my $wget_cmd = $ossim_conf::ossim_data->{"wget_path"};
# URL of ACID with user and passwd
my $acid_user = $ossim_conf::ossim_data->{"acid_user"};
my $acid_pass = $ossim_conf::ossim_data->{"acid_pass"};
my $acid_link = $ossim_conf::ossim_data->{"acid_link"};

check_var("wget_path", $wget_cmd);
check_var("acid_link", $acid_link);

my $acid_url = "";
if ($acid_link =~ m/(\w+:\/\/)(.*)/) {
    if ($acid_user eq "") {
        $acid_url = $acid_link;
    } else {
        $acid_url = $1 . $acid_user . ":" . $acid_pass . "@" . $2;
    }
} else {
    print "Error reading variable 'acid_link=$acid_link' from '$config_file'. ";
    print "Variable must be in 'http://...' format!\n";
    exit;
}

# ACID install directory
my $acid_path = $ossim_conf::ossim_data->{"acid_path"};
check_var("acid_path", $acid_path);

# Sleep for the while loop
my $acid_sleep = 60;
#

# Ugly but secure, where did I see this ?
my $pidfile = "/var/run/acid_cache.pid";
my $tmpfile = "/var/run/acid_wget.html";
my $logfile = "/var/run/acid_cache.log";

sub byebye {
    print "$0: forking into background...\n";
    exit;
}

fork and byebye;

sub die_clean {
    unlink $pidfile;
    exit;
}

open(PID, ">$pidfile") or die "Unable to open $pidfile\n";
print PID $$;
close(PID);

(-e $acid_path) or die "Unable to find $acid_path\n";

sub acid_replace {
    my ($fin, $fout) = @_;    

    my $old = "Queried DB on";
    my $new = "<FONT color=\"red\">Cached on</FONT>";

    open (OUTPUT, ">$fout") or die "Unable to open $fout\n";
    open (INPUT, "$fin") or die "Unable to open $fin\n";
    while (<INPUT>) {
	$_ =~ s/$old/$new/g;
	print OUTPUT $_;
    }
    close (INPUT);
    close (OUTPUT);
}

sub acid_main {
    my @res;

    while (1)
    {
	# Acid update Alerts
	@res = `$wget_cmd -O $tmpfile -o $logfile '$acid_url/acid_update_db.php'`;

	# Acid cache Unique Alerts
    if (-e $tmpfile){ unlink($tmpfile)};
	@res = `$wget_cmd -O $tmpfile -o $logfile '$acid_url/acid_stat_alerts.php?sort_order=occur_d'`;
	if (-e $tmpfile && -s $tmpfile){ 
    acid_replace ($tmpfile, "$acid_path/acid_stat_alerts.html");
    };

	# Acid cache by Source IP
    if (-e $tmpfile){ unlink($tmpfile)};
	@res = `$wget_cmd -O $tmpfile -o $logfile '$acid_url/acid_stat_uaddr.php?addr_type=1&sort_order=occur_d'`;
	if (-e $tmpfile && -s $tmpfile){ 
    acid_replace ($tmpfile, "$acid_path/acid_stat_uaddr1.html");
    };

	# Acid cache by Destination IP
    if (-e $tmpfile){ unlink($tmpfile)};
	@res = `$wget_cmd -O $tmpfile -o $logfile '$acid_url/acid_stat_uaddr.php?addr_type=2&sort_order=occur_d'`;
	if (-e $tmpfile && -s $tmpfile){ 
    acid_replace ($tmpfile, "$acid_path/acid_stat_uaddr2.html");
    };

	# Acid cache by Destination Port
    if (-e $tmpfile){ unlink($tmpfile)};
	@res = `$wget_cmd -O $tmpfile -o $logfile '$acid_url/acid_stat_ports.php?port_type=2&proto=-1&sort_order=occur_d'`;
	if (-e $tmpfile && -s $tmpfile){ 
    acid_replace ($tmpfile, "$acid_path/acid_stat_ports2.html");
    };
	sleep ($acid_sleep)
    }
}

acid_main ();
