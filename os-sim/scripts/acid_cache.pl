#!/usr/bin/perl

# Script for ACID cache
#
# 2004-05-05 Fabio Ospitia Trujillo <fot@ossim.net>
# 2004-05-25 Bugfixes & improvements: DK <dk@ossim.net>
# 2004-09-01 Session support, added vars: DK
#
# NOTE: Need 'wget' program

use DBI;
use ossim_conf;
my $config_file = "/etc/ossim/framework/ossim.conf";

sub check_var($ $ $) 
{
    my $name = shift;
    my $var  = shift;
    my $critical = shift;

    if (!$var) {
        print "You must set '$name' variable at $config_file\n";
        if($critical){exit;};
    }
}

# Full path for the program 'wget'
my $wget_cmd = $ossim_conf::ossim_data->{"wget_path"};
# URL of ACID with user and passwd
my $acid_user = $ossim_conf::ossim_data->{"acid_user"};
my $acid_pass = $ossim_conf::ossim_data->{"acid_pass"};
my $acid_link = $ossim_conf::ossim_data->{"acid_link"};
my $ossim_web_user = $ossim_conf::ossim_data->{"ossim_web_user"};
my $ossim_web_pass = $ossim_conf::ossim_data->{"ossim_web_pass"};
my $ossim_link = $ossim_conf::ossim_data->{"ossim_link"};

check_var("wget_path", $wget_cmd, 1);
check_var("acid_link", $acid_link, 1);
check_var("ossim_web_user", $ossim_web_user, 0);
check_var("ossim_web_pass", $ossim_web_pass, 0);
check_var("ossim_link", $ossim_link, 0);

my $acid_ip = "";
my $ossim_ip = "";

if($acid_link =~  m/(\w+:\/\/)(.*)/){
$acid_ip = $1;
$acid_link = $2;
}

# ossim ip is the important one and overwrites acid.
if($ossim_link =~  m/(\w+:\/\/)(.*)/){
$ossim_ip = $1;
$ossim_link = $2;
} else {
$ossim_ip = "127.0.0.1";
}

if($ossim_link == ""){ $ossim_link = "/ossim/";}
if($ossim_web_user == ""){ $ossim_web_user = "admin";}
if($ossim_web_pass == ""){ $ossim_web_pass = "admin";}
if($acid_link == ""){ $acid_link = "/acid/";}

# We benefit from two facts: You can pass multiple "?" as arguments and it
# doesn't matter if you pass and user and the server doesn't need one.

if ($acid_user eq "") {
        $acid_url1 = "http://$ossim_ip" . $ossim_link .  "/session/login.php?dest=" . $acid_link . "/acid_update_db.php&user=" . $ossim_web_user .  "&pass=" . $ossim_web_pass;
        $acid_url2 = "http://$ossim_ip" . $ossim_link .  "/session/login.php?dest=" .  $acid_link .  "/acid_stat_alerts.php?sort_order=occur_d&user=" . $ossim_web_user .  "&pass=" . $ossim_web_pass;
        $acid_url3 = "http://$ossim_ip" . $ossim_link .  "/session/login.php?dest=" .  $acid_link .  "/acid_stat_uaddr.php?addr_type=1&sort_order=occur_d&user=" . $ossim_web_user .  "&pass=" . $ossim_web_pass;
        $acid_url4 = "http://$ossim_ip" . $ossim_link .  "/session/login.php?dest=" .  $acid_link .  "/acid_stat_uaddr.php?addr_type=2&sort_order=occur_d&user=" . $ossim_web_user .  "&pass=" . $ossim_web_pass;
        $acid_url5 = "http://$ossim_ip" . $ossim_link .  "/session/login.php?dest=" .  $acid_link .  "/acid_stat_ports.php?port_type=2&proto=-1&sort_order=occur_d&user=" . $ossim_web_user .  "&pass=" . $ossim_web_pass;
} else {
        $acid_url1 = "http://". $acid_user . ":" . $acid_pass . "@" . $ossim_ip . $ossim_link .  "/session/login.php?dest=" . $acid_link . "/acid_update_db.php&user=" . $ossim_web_user .  "&pass=" . $ossim_web_pass;
        $acid_url2 = "http://". $acid_user . ":" . $acid_pass . "@" . $ossim_ip . $ossim_link .  "/session/login.php?dest=" . $acid_link .  "/acid_stat_alerts.php?sort_order=occur_d&user=" . $ossim_web_user .  "&pass=" . $ossim_web_pass;
        $acid_url3 = "http://". $acid_user . ":" . $acid_pass . "@" . $ossim_ip . $ossim_link .  "/session/login.php?dest=" . $acid_link .  "/acid_stat_uaddr.php?addr_type=1&sort_order=occur_d&user=" . $ossim_web_user .  "&pass=" . $ossim_web_pass;
        $acid_url4 = "http://". $acid_user . ":" . $acid_pass . "@" . $ossim_ip . $ossim_link .  "/session/login.php?dest=" . $acid_link .  "/acid_stat_uaddr.php?addr_type=2&sort_order=occur_d&user=" . $ossim_web_user .  "&pass=" . $ossim_web_pass;
        $acid_url5 = "http://". $acid_user . ":" . $acid_pass . "@" . $ossim_ip . $ossim_link .  "/session/login.php?dest=" . $acid_link .  "/acid_stat_ports.php?port_type=2&proto=-1&sort_order=occur_d&user=" . $ossim_web_user .  "&pass=" . $ossim_web_pass;
}

# ACID install directory
my $acid_path = $ossim_conf::ossim_data->{"acid_path"};
check_var("acid_path", $acid_path, 1);

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
	@res = `$wget_cmd -O $tmpfile -o $logfile '$acid_url1'`;

	# Acid cache Unique Alerts
    if (-e $tmpfile){ unlink($tmpfile)};
	@res = `$wget_cmd -O $tmpfile -o $logfile '$acid_url2'`;
	if (-e $tmpfile && -s $tmpfile){ 
    acid_replace ($tmpfile, "$acid_path/acid_stat_alerts.html");
    };

	# Acid cache by Source IP
    if (-e $tmpfile){ unlink($tmpfile)};
	@res = `$wget_cmd -O $tmpfile -o $logfile '$acid_url3'`;
	if (-e $tmpfile && -s $tmpfile){ 
    acid_replace ($tmpfile, "$acid_path/acid_stat_uaddr1.html");
    };

	# Acid cache by Destination IP
    if (-e $tmpfile){ unlink($tmpfile)};
	@res = `$wget_cmd -O $tmpfile -o $logfile '$acid_url4'`;
	if (-e $tmpfile && -s $tmpfile){ 
    acid_replace ($tmpfile, "$acid_path/acid_stat_uaddr2.html");
    };

	# Acid cache by Destination Port
    if (-e $tmpfile){ unlink($tmpfile)};
	@res = `$wget_cmd -O $tmpfile -o $logfile '$acid_url5'`;
	if (-e $tmpfile && -s $tmpfile){ 
    acid_replace ($tmpfile, "$acid_path/acid_stat_ports2.html");
    };
	sleep ($acid_sleep)
    }
}

acid_main ();
