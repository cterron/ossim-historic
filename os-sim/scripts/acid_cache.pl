#!/usr/bin/perl

# Script for ACID cache
#
# 2004-05-05 Fabio Ospitia Trujillo <fot@ossim.net>
#
# NOTE: Need 'wget' program

use DBI;
use ossim_conf;

# Full path for the program 'wget'
my $wget_cmd = "/usr/bin/wget";
# URL of ACID with user and passwd
my $acid_url = "http://ossim:ossim\@localhost/acid";
# ACID install directory
my $acid_dir = "/var/www/acid";
# Sleep for the while loop
my $acid_sleep = 60;
#
my $pidfile = "/var/run/acid_cahe.pid";
my $tmpfile = "/tmp/acid_wget.html";
my $logfile = "/tmp/acid_cache.log";

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

(-e $acid_dir) or die "Unable to find $acid_dir\n";

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
	@res = `$wget_cmd -O $tmpfile -o $logfile '$acid_url/acid_stat_alerts.php?sort_order=occur_d'`;
	acid_replace ($tmpfile, "$acid_dir/acid_stat_alerts.html");

	# Acid cache by Source IP
	@res = `$wget_cmd -O $tmpfile -o $logfile '$acid_url/acid_stat_uaddr.php?addr_type=1&sort_order=occur_d'`;
	acid_replace ($tmpfile, "$acid_dir/acid_stat_uaddr1.html");

	# Acid cache by Destination IP
	@res = `$wget_cmd -O $tmpfile -o $logfile '$acid_url/acid_stat_uaddr.php?addr_type=2&sort_order=occur_d'`;
	acid_replace ($tmpfile, "$acid_dir/acid_stat_uaddr2.html");

	# Acid cache by Destination Port
	@res = `$wget_cmd -O $tmpfile -o $logfile '$acid_url/acid_stat_ports.php?port_type=2&proto=-1&sort_order=occur_d'`;
	acid_replace ($tmpfile, "$acid_dir/acid_stat_ports2.html");

	sleep ($acid_sleep)
    }
}

acid_main ();
