#!/usr/bin/perl

use ossim_conf;
use DBI;

use strict;
use warnings;

$| = 1;


my $dsn = "dbi:mysql:".$ossim_conf::base.":".$ossim_conf::host.":".$ossim_conf::port;
my $dbh = DBI->connect($dsn, $ossim_conf::user, $ossim_conf::pass) 
    or die "Can't connect to DBI\n";

my $base_dir = $ossim_conf::base_dir;

my $SLEEP = 30;

while (1) {

#
# Host rrds
# 
while ((my $rrd_file = 
            glob("$ossim_conf::mrtg_rrd_files_path/host_qualification/*")))
{
    my $host_ip = $rrd_file;
    $host_ip =~ s/\.rrd$//;
    $host_ip =~ s/\/.*\///;

    my $day_max_c=`$base_dir/scripts/get_rrd_value.pl N-1D N $rrd_file compromise MAX`;
    my $day_min_c=`$base_dir/scripts/get_rrd_value.pl N-1D N $rrd_file compromise MIN`;
    my $day_avg_c=`$base_dir/scripts/get_rrd_value.pl N-1D N $rrd_file compromise AVERAGE`;
    my $day_max_a=`$base_dir/scripts/get_rrd_value.pl N-1D N $rrd_file attack MAX`;
    my $day_min_a=`$base_dir/scripts/get_rrd_value.pl N-1D N $rrd_file attack MIN`;
    my $day_avg_a=`$base_dir/scripts/get_rrd_value.pl N-1D N $rrd_file attack AVERAGE`;

    chop($day_max_c); chop($day_min_c); chop($day_avg_c);
    chop($day_max_a); chop($day_min_a); chop($day_avg_a);

    my $query = "SELECT host_ip FROM control_panel_host 
        WHERE host_ip = '$host_ip' AND time_range = 'day'";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    if ($sth->rows == 0) {
        $query = "INSERT INTO control_panel_host 
                    VALUES ('$host_ip', 'day', '$day_max_c', 
                            '$day_max_a', '$day_min_c', '$day_min_a', 
                            '$day_avg_c', '$day_avg_a')";
        $sth = $dbh->prepare($query);
        $sth->execute();
    } else {
        $query = "UPDATE control_panel_host 
            SET max_c = '$day_max_c', max_a = '$day_max_a',
                min_c = '$day_min_c', min_a = '$day_min_a',
                avg_c = '$day_avg_c', avg_a = '$day_avg_a' 
            WHERE host_ip = '$host_ip' AND time_range = 'day'";
        $sth = $dbh->prepare($query);
        $sth->execute();        
    }

    my $month_max_c=`$base_dir/scripts/get_rrd_value.pl N-1M N $rrd_file compromise MAX`;
    my $month_min_c=`$base_dir/scripts/get_rrd_value.pl N-1M N $rrd_file compromise MIN`;
    my $month_avg_c=`$base_dir/scripts/get_rrd_value.pl N-1M N $rrd_file compromise AVERAGE`;
    my $month_max_a=`$base_dir/scripts/get_rrd_value.pl N-1M N $rrd_file attack MAX`;
    my $month_min_a=`$base_dir/scripts/get_rrd_value.pl N-1M N $rrd_file attack MIN`;
    my $month_avg_a=`$base_dir/scripts/get_rrd_value.pl N-1M N $rrd_file attack AVERAGE`;

    chop($month_max_c); chop($month_min_c); chop($month_avg_c);
    chop($month_max_a); chop($month_min_a); chop($month_avg_a);

    $query = "SELECT host_ip FROM control_panel_host 
        WHERE host_ip = '$host_ip' AND time_range = 'month'";
    $sth = $dbh->prepare($query);
    $sth->execute();
    if ($sth->rows == 0) {
        $query = "INSERT INTO control_panel_host 
                    VALUES ('$host_ip', 'month', '$month_max_c', 
                            '$month_max_a', '$month_min_c', '$month_min_a', 
                            '$month_avg_c', '$month_avg_a')";
        $sth = $dbh->prepare($query);
        $sth->execute();
    } else {
        $query = "UPDATE control_panel_host 
            SET max_c = '$month_max_c', max_a = '$month_max_a',
                min_c = '$month_min_c', min_a = '$month_min_a',
                avg_c = '$month_avg_c', avg_a = '$month_avg_a' 
            WHERE host_ip = '$host_ip' AND time_range = 'month'";
        $sth = $dbh->prepare($query);
        $sth->execute();        
    }

    my $year_max_c=`$base_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file compromise MAX`;
    my $year_min_c=`$base_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file compromise MIN`;
    my $year_avg_c=`$base_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file compromise AVERAGE`;
    my $year_max_a=`$base_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file attack MAX`;
    my $year_min_a=`$base_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file attack MIN`;
    my $year_avg_a=`$base_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file attack AVERAGE`;

    chop($year_max_c); chop($year_min_c); chop($year_avg_c);
    chop($year_max_a); chop($year_min_a); chop($year_avg_a);

    $query = "SELECT host_ip FROM control_panel_host 
        WHERE host_ip = '$host_ip' AND time_range = 'year'";
    $sth = $dbh->prepare($query);
    $sth->execute();
    if ($sth->rows == 0) {
        $query = "INSERT INTO control_panel_host 
                    VALUES ('$host_ip', 'year', '$year_max_c', 
                            '$year_max_a', '$year_min_c', '$year_min_a', 
                            '$year_avg_c', '$year_avg_a')";
        $sth = $dbh->prepare($query);
        $sth->execute();
    } else {
        $query = "UPDATE control_panel_host 
            SET max_c = '$year_max_c', max_a = '$year_max_a',
                min_c = '$year_min_c', min_a = '$year_min_a',
                avg_c = '$year_avg_c', avg_a = '$year_avg_a' 
            WHERE host_ip = '$host_ip' AND time_range = 'year'";
        $sth = $dbh->prepare($query);
        $sth->execute();        
    }
}



#
# Net rrds
# 
while ((my $rrd_file = 
            glob("$ossim_conf::mrtg_rrd_files_path/net_qualification/*")))
{
    my $net_name = $rrd_file;
    $net_name =~ s/\.rrd$//;
    $net_name =~ s/\/.*\///;

    my $day_max_c=`$base_dir/scripts/get_rrd_value.pl N-1D N $rrd_file compromise MAX`;
    my $day_min_c=`$base_dir/scripts/get_rrd_value.pl N-1D N $rrd_file compromise MIN`;
    my $day_avg_c=`$base_dir/scripts/get_rrd_value.pl N-1D N $rrd_file compromise AVERAGE`;
    my $day_max_a=`$base_dir/scripts/get_rrd_value.pl N-1D N $rrd_file attack MAX`;
    my $day_min_a=`$base_dir/scripts/get_rrd_value.pl N-1D N $rrd_file attack MIN`;
    my $day_avg_a=`$base_dir/scripts/get_rrd_value.pl N-1D N $rrd_file attack AVERAGE`;

    chop($day_max_c); chop($day_min_c); chop($day_avg_c);
    chop($day_max_a); chop($day_min_a); chop($day_avg_a);

    my $query = "SELECT net_name FROM control_panel_net 
        WHERE net_name = '$net_name' AND time_range = 'day'";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    if ($sth->rows == 0) {
        $query = "INSERT INTO control_panel_net 
                    VALUES ('$net_name', 'day', '$day_max_c', 
                            '$day_max_a', '$day_min_c', '$day_min_a', 
                            '$day_avg_c', '$day_avg_a')";
        $sth = $dbh->prepare($query);
        $sth->execute();
    } else {
        $query = "UPDATE control_panel_net 
            SET max_c = '$day_max_c', max_a = '$day_max_a',
                min_c = '$day_min_c', min_a = '$day_min_a',
                avg_c = '$day_avg_c', avg_a = '$day_avg_a' 
            WHERE net_name = '$net_name' AND time_range = 'day'";
        $sth = $dbh->prepare($query);
        $sth->execute();        
    }

    my $month_max_c=`$base_dir/scripts/get_rrd_value.pl N-1M N $rrd_file compromise MAX`;
    my $month_min_c=`$base_dir/scripts/get_rrd_value.pl N-1M N $rrd_file compromise MIN`;
    my $month_avg_c=`$base_dir/scripts/get_rrd_value.pl N-1M N $rrd_file compromise AVERAGE`;
    my $month_max_a=`$base_dir/scripts/get_rrd_value.pl N-1M N $rrd_file attack MAX`;
    my $month_min_a=`$base_dir/scripts/get_rrd_value.pl N-1M N $rrd_file attack MIN`;
    my $month_avg_a=`$base_dir/scripts/get_rrd_value.pl N-1M N $rrd_file attack AVERAGE`;

    chop($month_max_c); chop($month_min_c); chop($month_avg_c);
    chop($month_max_a); chop($month_min_a); chop($month_avg_a);

    $query = "SELECT net_name FROM control_panel_net 
        WHERE net_name = '$net_name' AND time_range = 'month'";
    $sth = $dbh->prepare($query);
    $sth->execute();
    if ($sth->rows == 0) {
        $query = "INSERT INTO control_panel_net 
                    VALUES ('$net_name', 'month', '$month_max_c', 
                            '$month_max_a', '$month_min_c', '$month_min_a', 
                            '$month_avg_c', '$month_avg_a')";
        $sth = $dbh->prepare($query);
        $sth->execute();
    } else {
        $query = "UPDATE control_panel_net 
            SET max_c = '$month_max_c', max_a = '$month_max_a',
                min_c = '$month_min_c', min_a = '$month_min_a',
                avg_c = '$month_avg_c', avg_a = '$month_avg_a' 
            WHERE net_name = '$net_name' AND time_range = 'month'";
        $sth = $dbh->prepare($query);
        $sth->execute();        
    }

    my $year_max_c=`$base_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file compromise MAX`;
    my $year_min_c=`$base_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file compromise MIN`;
    my $year_avg_c=`$base_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file compromise AVERAGE`;
    my $year_max_a=`$base_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file attack MAX`;
    my $year_min_a=`$base_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file attack MIN`;
    my $year_avg_a=`$base_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file attack AVERAGE`;

    chop($year_max_c); chop($year_min_c); chop($year_avg_c);
    chop($year_max_a); chop($year_min_a); chop($year_avg_a);

    $query = "SELECT net_name FROM control_panel_net 
        WHERE net_name = '$net_name' AND time_range = 'year'";
    $sth = $dbh->prepare($query);
    $sth->execute();
    if ($sth->rows == 0) {
        $query = "INSERT INTO control_panel_net 
                    VALUES ('$net_name', 'year', '$year_max_c', 
                            '$year_max_a', '$year_min_c', '$year_min_a', 
                            '$year_avg_c', '$year_avg_a')";
        $sth = $dbh->prepare($query);
        $sth->execute();
    } else {
        $query = "UPDATE control_panel_net 
            SET max_c = '$year_max_c', max_a = '$year_max_a',
                min_c = '$year_min_c', min_a = '$year_min_a',
                avg_c = '$year_avg_c', avg_a = '$year_avg_a' 
            WHERE net_name = '$net_name' AND time_range = 'year'";
        $sth = $dbh->prepare($query);
        $sth->execute();        
    }
}

sleep($SLEEP);
}

$dbh->disconnect;
exit 0;

