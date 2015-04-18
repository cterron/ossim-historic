#!/usr/bin/perl

use ossim_conf;
use DBI;
use POSIX;

use strict;
use warnings;

$| = 1;


my $dsn = "dbi:mysql:".$ossim_conf::ossim_data->{"ossim_base"}.":".$ossim_conf::ossim_data->{"ossim_host"}.":".$ossim_conf::ossim_data->{"ossim_port"};
my $dbh = DBI->connect($dsn, $ossim_conf::ossim_data->{"ossim_user"}, $ossim_conf::ossim_data->{"ossim_pass"}) 
    or die "Can't connect to DBI\n";

my $data_dir = $ossim_conf::ossim_data->{"data_dir"};

my $SLEEP = 600;

my $day_seconds = 86400;
my $month_seconds = 2592000;
my $year_seconds = 31536000;


while (1) {

#
# Net rrds
# 
while ((my $rrd_file = 
            glob("$ossim_conf::ossim_data->{mrtg_rrd_files_path}/net_qualification/*")))
{
    my $net_name = $rrd_file;
    $net_name =~ s/\.rrd$//;
    $net_name =~ s/\/.*\///;

    my $day_max_c=`$data_dir/scripts/get_rrd_value.pl N-1D N $rrd_file compromise MAX`;
    my $day_avg_c=`$data_dir/scripts/get_rrd_value.pl N-1D N $rrd_file compromise AVERAGE`;
    my $day_max_a=`$data_dir/scripts/get_rrd_value.pl N-1D N $rrd_file attack MAX`;
    my $day_avg_a=`$data_dir/scripts/get_rrd_value.pl N-1D N $rrd_file attack AVERAGE`;

    chop($day_max_c); chop($day_avg_c);
    chop($day_max_a); chop($day_avg_a);
    
    my $max_c_date= `$ossim_conf::ossim_data->{data_dir}/scripts/get_date.pl $net_name 1D compromise net`;
    $max_c_date = POSIX::strftime("%Y-%m-%d %H:%M:%S", localtime($max_c_date));
    my $max_a_date = `$ossim_conf::ossim_data->{data_dir}/scripts/get_date.pl $net_name 1D attack net`;
    $max_a_date = POSIX::strftime("%Y-%m-%d %H:%M:%S", localtime($max_a_date));

    my $query = "SELECT net_name FROM control_panel_net 
        WHERE net_name = '$net_name' AND time_range = 'day'";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    if ($sth->rows == 0) {
        $query = "INSERT INTO control_panel_net 
                    VALUES ('$net_name', 'day', '$day_max_c', 
                            '$day_max_a', '$max_c_date', '$max_a_date', '$day_avg_c', '$day_avg_a')";
        $sth = $dbh->prepare($query);
        $sth->execute();
    } elsif (($day_max_c > 0) || ($day_max_a > 0)) { 
        $query = "UPDATE control_panel_net 
            SET max_c = '$day_max_c', max_a = '$day_max_a',
                max_c_date = '$max_c_date', max_a_date = '$max_a_date',
                avg_c = '$day_avg_c', avg_a = '$day_avg_a' 
            WHERE net_name = '$net_name' AND time_range = 'day'";
        $sth = $dbh->prepare($query);
        $sth->execute();        
    }
    
    # clean up!
#    elsif ($day_max_c == 'nan' && $day_max_a == 'nan') {
#        $query = "DELETE FROM control_panel_net
#                    WHERE net_name = '$net_name' AND time_range = 'day'";
#        $sth = $dbh->prepare($query);
#        $sth->execute();
#    }

    my $month_max_c=`$data_dir/scripts/get_rrd_value.pl N-1M N $rrd_file compromise MAX`;
    my $month_avg_c=`$data_dir/scripts/get_rrd_value.pl N-1M N $rrd_file compromise AVERAGE`;
    my $month_max_a=`$data_dir/scripts/get_rrd_value.pl N-1M N $rrd_file attack MAX`;
    my $month_avg_a=`$data_dir/scripts/get_rrd_value.pl N-1M N $rrd_file attack AVERAGE`;

    chop($month_max_c); chop($month_avg_c);
    chop($month_max_a); chop($month_avg_a);

    $max_c_date= `$ossim_conf::ossim_data->{data_dir}/scripts/get_date.pl $net_name 1M compromise net`;
    $max_c_date = POSIX::strftime("%Y-%m-%d %H:%M:%S", localtime($max_c_date));
    $max_a_date = `$ossim_conf::ossim_data->{data_dir}/scripts/get_date.pl $net_name 1M attack net`;
    $max_a_date = POSIX::strftime("%Y-%m-%d %H:%M:%S", localtime($max_a_date));

    $query = "SELECT net_name FROM control_panel_net 
        WHERE net_name = '$net_name' AND time_range = 'month'";
    $sth = $dbh->prepare($query);
    $sth->execute();
    if ($sth->rows == 0) {
        $query = "INSERT INTO control_panel_net 
                    VALUES ('$net_name', 'month', '$month_max_c', 
                            '$month_max_a', '$max_c_date', '$max_a_date', '$month_avg_c', '$month_avg_a')";
        $sth = $dbh->prepare($query);
        $sth->execute();
    } elsif (($month_max_c > 0) || ($month_max_a > 0)) {
        $query = "UPDATE control_panel_net 
            SET max_c = '$month_max_c', max_a = '$month_max_a',
                max_c_date = '$max_c_date', max_a_date = '$max_a_date',
                avg_c = '$month_avg_c', avg_a = '$month_avg_a' 
            WHERE net_name = '$net_name' AND time_range = 'month'";
        $sth = $dbh->prepare($query);
        $sth->execute();        
    }
    
    # clean up!
#    elsif ($month_max_c == 'nan' && $month_max_a == 'nan') {
#        $query = "DELETE FROM control_panel_net
#                    WHERE net_name = '$net_name' AND time_range = 'month'";
#        $sth = $dbh->prepare($query);
#        $sth->execute();
#    }

    my $year_max_c=`$data_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file compromise MAX`;
    my $year_avg_c=`$data_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file compromise AVERAGE`;
    my $year_max_a=`$data_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file attack MAX`;
    my $year_avg_a=`$data_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file attack AVERAGE`;

    chop($year_max_c); chop($year_avg_c);
    chop($year_max_a); chop($year_avg_a);
    
    $max_c_date= `$ossim_conf::ossim_data->{data_dir}/scripts/get_date.pl $net_name 1Y compromise net`;
    $max_c_date = POSIX::strftime("%Y-%m-%d %H:%M:%S", localtime($max_c_date));
    $max_a_date = `$ossim_conf::ossim_data->{data_dir}/scripts/get_date.pl $net_name 1Y attack net`;
    $max_a_date = POSIX::strftime("%Y-%m-%d %H:%M:%S", localtime($max_a_date));


    $query = "SELECT net_name FROM control_panel_net 
        WHERE net_name = '$net_name' AND time_range = 'year'";
    $sth = $dbh->prepare($query);
    $sth->execute();
    if ($sth->rows == 0) {
        $query = "INSERT INTO control_panel_net 
                    VALUES ('$net_name', 'year', '$year_max_c', 
                            '$year_max_a', '$max_c_date', '$max_a_date', '$year_avg_c', '$year_avg_a')";
        $sth = $dbh->prepare($query);
        $sth->execute();
    } elsif (($year_max_c > 0) || ($year_max_a > 0)) {
        $query = "UPDATE control_panel_net 
            SET max_c = '$year_max_c', max_a = '$year_max_a',
                max_c_date = '$max_c_date', max_a_date = '$max_a_date',
                avg_c = '$year_avg_c', avg_a = '$year_avg_a' 
            WHERE net_name = '$net_name' AND time_range = 'year'";
        $sth = $dbh->prepare($query);
        $sth->execute();        
    }

    # clean up!
#    elsif ($year_max_c == 'nan' && $year_max_a == 'nan') {
#        $query = "DELETE FROM control_panel_net
#                    WHERE net_name = '$net_name' AND time_range = 'year'";
#        $sth = $dbh->prepare($query);
#        $sth->execute();
#    }
    
}


#
# Host rrds
# 
while ((my $rrd_file = 
            glob("$ossim_conf::ossim_data->{mrtg_rrd_files_path}/host_qualification/*")))
{
    my $host_ip = $rrd_file;
    $host_ip =~ s/\.rrd$//;
    $host_ip =~ s/\/.*\///;


    my $query;
    my $sth;
    my $stat_time = (stat($rrd_file))[9];

if($stat_time + $day_seconds >= time()){
    my $day_max_c=`$data_dir/scripts/get_rrd_value.pl N-1D N $rrd_file compromise MAX`;
    my $day_avg_c=`$data_dir/scripts/get_rrd_value.pl N-1D N $rrd_file compromise AVERAGE`;
    my $day_max_a=`$data_dir/scripts/get_rrd_value.pl N-1D N $rrd_file attack MAX`;
    my $day_avg_a=`$data_dir/scripts/get_rrd_value.pl N-1D N $rrd_file attack AVERAGE`;

    chop($day_max_c); chop($day_avg_c);
    chop($day_max_a); chop($day_avg_a);

    my $max_c_date= `$ossim_conf::ossim_data->{data_dir}/scripts/get_date.pl $host_ip 1D compromise host`;
    $max_c_date = POSIX::strftime("%Y-%m-%d %H:%M:%S", localtime($max_c_date));
    my $max_a_date = `$ossim_conf::ossim_data->{data_dir}/scripts/get_date.pl $host_ip 1D attack host`;
    $max_a_date = POSIX::strftime("%Y-%m-%d %H:%M:%S", localtime($max_a_date));

    my $query = "SELECT host_ip FROM control_panel_host 
        WHERE host_ip = '$host_ip' AND time_range = 'day'";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    if ($sth->rows == 0) {
        $query = "INSERT INTO control_panel_host 
                    VALUES ('$host_ip', 'day', '$day_max_c', 
                            '$day_max_a', '$max_c_date', '$max_a_date', '$day_avg_c', '$day_avg_a')";
        $sth = $dbh->prepare($query);
        $sth->execute();
    } elsif (($day_max_c > 0) || ($day_max_a > 0)) { 
        $query = "UPDATE control_panel_host 
            SET max_c = '$day_max_c', max_a = '$day_max_a',
                max_c_date = '$max_c_date', max_a_date = '$max_a_date',
                avg_c = '$day_avg_c', avg_a = '$day_avg_a' 
            WHERE host_ip = '$host_ip' AND time_range = 'day'";
        $sth = $dbh->prepare($query);
        $sth->execute();
    } 

} else {
    # clean up!
    $query = "DELETE FROM control_panel_host WHERE host_ip = '$host_ip' AND time_range = 'day'";
    $sth = $dbh->prepare($query);
    $sth->execute();
}


if($stat_time + $month_seconds >= time()){

    my $month_max_c=`$data_dir/scripts/get_rrd_value.pl N-1M N $rrd_file compromise MAX`;
    my $month_avg_c=`$data_dir/scripts/get_rrd_value.pl N-1M N $rrd_file compromise AVERAGE`;
    my $month_max_a=`$data_dir/scripts/get_rrd_value.pl N-1M N $rrd_file attack MAX`;
    my $month_avg_a=`$data_dir/scripts/get_rrd_value.pl N-1M N $rrd_file attack AVERAGE`;

    chop($month_max_c); chop($month_avg_c);
    chop($month_max_a); chop($month_avg_a);

    my $max_c_date= `$ossim_conf::ossim_data->{data_dir}/scripts/get_date.pl $host_ip 1M compromise host`;
    $max_c_date = POSIX::strftime("%Y-%m-%d %H:%M:%S", localtime($max_c_date));
    my $max_a_date = `$ossim_conf::ossim_data->{data_dir}/scripts/get_date.pl $host_ip 1M attack host`;
    $max_a_date = POSIX::strftime("%Y-%m-%d %H:%M:%S", localtime($max_a_date));

    $query = "SELECT host_ip FROM control_panel_host 
        WHERE host_ip = '$host_ip' AND time_range = 'month'";
    $sth = $dbh->prepare($query);
    $sth->execute();
    if ($sth->rows == 0) {
        $query = "INSERT INTO control_panel_host 
                    VALUES ('$host_ip', 'month', '$month_max_c', 
                            '$month_max_a', '$max_c_date', '$max_a_date', '$month_avg_c', '$month_avg_a')";
        $sth = $dbh->prepare($query);
        $sth->execute();
    } elsif (($month_max_c > 0) || ($month_max_a > 0)) {
        $query = "UPDATE control_panel_host 
            SET max_c = '$month_max_c', max_a = '$month_max_a',
                max_c_date = '$max_c_date', max_a_date = '$max_a_date',
                avg_c = '$month_avg_c', avg_a = '$month_avg_a' 
            WHERE host_ip = '$host_ip' AND time_range = 'month'";
        $sth = $dbh->prepare($query);
        $sth->execute();        
    }
} else {
    # clean up!
    $query = "DELETE FROM control_panel_host WHERE host_ip = '$host_ip' AND time_range = 'month'";
    $sth = $dbh->prepare($query);
    $sth->execute();
}
   

if($stat_time + $year_seconds >= time()){

    my $year_max_c=`$data_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file compromise MAX`;
    my $year_avg_c=`$data_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file compromise AVERAGE`;
    my $year_max_a=`$data_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file attack MAX`;
    my $year_avg_a=`$data_dir/scripts/get_rrd_value.pl N-1Y N $rrd_file attack AVERAGE`;

    chop($year_max_c); chop($year_avg_c);
    chop($year_max_a); chop($year_avg_a);
    
    my $max_c_date= `$ossim_conf::ossim_data->{data_dir}/scripts/get_date.pl $host_ip 1Y compromise host`;
    $max_c_date = POSIX::strftime("%Y-%m-%d %H:%M:%S", localtime($max_c_date));
    my $max_a_date = `$ossim_conf::ossim_data->{data_dir}/scripts/get_date.pl $host_ip 1Y attack host`;
    $max_a_date = POSIX::strftime("%Y-%m-%d %H:%M:%S", localtime($max_a_date));

    $query = "SELECT host_ip FROM control_panel_host 
        WHERE host_ip = '$host_ip' AND time_range = 'year'";
    $sth = $dbh->prepare($query);
    $sth->execute();
    if ($sth->rows == 0) {
        $query = "INSERT INTO control_panel_host 
                    VALUES ('$host_ip', 'year', '$year_max_c', 
                            '$year_max_a', '$max_c_date', '$max_a_date', '$year_avg_c', '$year_avg_a')";
        $sth = $dbh->prepare($query);
        $sth->execute();
    } elsif (($year_max_c > 0) || ($year_max_a > 0)) {
        $query = "UPDATE control_panel_host 
            SET max_c = '$year_max_c', max_a = '$year_max_a',
                max_c_date = '$max_c_date', max_a_date = '$max_a_date',
                avg_c = '$year_avg_c', avg_a = '$year_avg_a' 
            WHERE host_ip = '$host_ip' AND time_range = 'year'";
        $sth = $dbh->prepare($query);
        $sth->execute();        
    }
   
} else {
      # clean up!
      $query = "DELETE FROM control_panel_host WHERE host_ip = '$host_ip' AND time_range = 'year'";
      $sth = $dbh->prepare($query);
      $sth->execute();
}

}

sleep($SLEEP);
}

$dbh->disconnect;
exit 0;
