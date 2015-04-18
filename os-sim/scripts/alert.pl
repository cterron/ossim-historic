#!/usr/bin/perl

use strict;

use ossim_conf;
use DBI;

$| = 1;

my $UPDATE_INTERVAL = $ossim_conf::ossim_data->{"UPDATE_INTERVAL"};
my $SLEEP = $UPDATE_INTERVAL * 15;
my $email = $ossim_conf::ossim_data->{"email_alert"};
my $mail = $ossim_conf::ossim_data->{"mail_path"};

#
# connect to dbi
# 
my $dsn = "dbi:mysql:" . $ossim_conf::ossim_data->{"ossim_base"}. 
          ":" . $ossim_conf::ossim_data->{"ossim_host"} . 
          ":" . $ossim_conf::ossim_data->{"ossim_port"};
my $dbh = DBI->connect($dsn, $ossim_conf::ossim_data->{"ossim_user"},
                       $ossim_conf::ossim_data->{"ossim_pass"})
            or die "Can't connect to DBI\n";

my $times;
while(1) {

    #
    # Host alert
    # 
    my $query = "select * from host_qualification, host 
        where host_ip = ip and alert = 1;";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    while (my $row = $sth->fetchrow_hashref) {
        if ($row->{attack} > $row->{threshold_a}) 
        {
            my $ip = $row->{ip};
            if ($times->{$ip} == 0) {
                $times->{$ip} = time();
            } elsif (time() > $times->{$ip} + ($row->{persistence} * 60)) {
                my $msg = "attack threshold for ip $ip exceded!\n";
                print $msg;
            
                #`echo "$msg" | $mail -s "alert from OSSIM" $email`;
                $times->{$ip} = 0;
            }
        }
        if ($row->{compromise} > $row->{threshold_c}) 
        {
            my $ip = $row->{ip};
            if ($times->{$ip} == 0) {
                $times->{$ip} = time();
            } elsif (time() > $times->{$ip} + ($row->{persistence} * 60)) {
                my $msg = "compromise threshold for ip $ip exceded!\n";
                print $msg;
            
                #`echo "$msg" | $mail -s "alert from OSSIM" $email`;
                $times->{$ip} = 0;
            }
        }
    }


    #
    # Net alert
    # 
    my $query = "select * from net_qualification, net 
        where net_name = name and alert = 1;";
    my $sth = $dbh->prepare($query);
    $sth->execute();
    while (my $row = $sth->fetchrow_hashref) {
        if ($row->{attack} > $row->{threshold_a}) 
        {
            my $net_name = $row->{net_name};
            if ($times->{$net_name} == 0) {
                $times->{$net_name} = time();
            } elsif (time() > $times->{$net_name} + ($row->{persistence} * 60)) {
                my $msg = "attack threshold for net $net_name exceded!\n";
                print $msg;
            
                `echo "$msg" | $mail -s "alert from OSSIM" $email`;
                $times->{$net_name} = 0;
            }
        }
        if ($row->{compromise} > $row->{threshold_c}) 
        {
            my $net_name = $row->{net_name};
            if ($times->{$net_name} == 0) {
                $times->{$net_name} = time();
            } elsif (time() > $times->{$net_name} + ($row->{persistence} * 60)) {
                my $msg = "compromise threshold for net $net_name exceded!\n";
                print $msg;
            
                `echo "$msg" | $mail -s "alert from OSSIM" $email`;
                $times->{$net_name} = 0;
            }
        }
    }

    sleep($SLEEP);
}

$dbh->disconnect;
exit 0;

