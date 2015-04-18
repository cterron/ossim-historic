#!/usr/bin/perl

use strict;

#
# Load DB table structure
# 
my $user=`grep ^ossim_user /etc/ossim.conf | cut -d= -f2`; chop $user;
my $pass=`grep ^ossim_pass /etc/ossim.conf | cut -d= -f2`; chop $pass;
my $base=`grep ^ossim_base /etc/ossim.conf | cut -d= -f2`; chop $base;
my $ossim_host=`grep ^ossim_host /etc/ossim.conf | cut -d= -f2`; chop $ossim_host;
`mysql -u $user -p$pass -h$ossim_host  $base < ossim_tables.sql`;

my $snort_user=`grep ^snort_user /etc/ossim.conf | cut -d= -f2`; chop $snort_user;
my $snort_pass=`grep ^snort_pass /etc/ossim.conf | cut -d= -f2`; chop $snort_pass;
my $snort_base=`grep ^snort_base /etc/ossim.conf | cut -d= -f2`; chop $snort_base;
my $snort_archive_base=`grep ^snort_archive_base /etc/ossim.conf | cut -d= -f2`; chop $snort_archive_base;
my $snort_host=`grep ^snort_host /etc/ossim.conf | cut -d= -f2`; chop $snort_host;
`mysql -u $snort_user -p$snort_pass -h$snort_host  $snort_base < ossim_acid_schema_mysql.sql`;
`mysql -u $snort_user -p$snort_pass -h$snort_host  $snort_archive_base < ossim_acid_schema_mysql.sql`;



#
#  Fill protocol table from /etc/protocols 
# 
open (PROTOCOLS_FILE, "/etc/protocols") or die "Can't open file\n";
open (PROTOCOLS_SQL, ">protocols.sql") or die "Can't open file\n";
while ($_ = <PROTOCOLS_FILE>) 
{
    if (/^#/) {next();} # comments

    if (/([\w\-]+)\s*(\d+)\s+([\w\-]+)(.*\#\s?([\w\s\-]+))?/) 
    {
        my $protocol = $1;
        my $port_number = $2;
        my $alias = $3;
        my $descr = $5;
        $descr =~ s/\s*$//g;

        print PROTOCOLS_SQL 
            "INSERT INTO protocol VALUES " .
            "('$port_number', '$protocol', '$alias', '$descr');\n";
    }
}
close(PROTOCOLS_FILE);
close(PROTOCOLS_SQL);

#
#  Fill port table from /etc/protocols 
# 
open (SERVICES_FILE, "/etc/services") or die "Can't open services file\n";
open (SERVICES_SQL, ">services.sql") or die "Can't open file\n";
while ($_ = <SERVICES_FILE>) 
{
    if (/^#/) {next();} # comments
                                                                                
    if (/([\w\-]+)\s*(\d+)\/(\w+)(.*\#\s?([\w\s\-]+))?/)
    {
        my $port = $2;
        my $type = $3;
        my $service = $1;
        my $descr = $5;
        $descr =~ s/\s*$//g;
                                                                                
        print SERVICES_SQL 
            "INSERT INTO port VALUES " .
            "('$port', '$type', '$service', '$descr');\n";
    }
}
close(SERVICES_FILE);
close(SERVICES_SQL);


#
#  Insert snort rules into signature table
#
my $snort_rules_path=`grep "snort_rules_path" /etc/ossim.conf | cut -d= -f2`;
chop $snort_rules_path;

open (RULES_SQL, ">rules.sql") or die "Can't open file\n";

my $dirname;
while (($dirname = glob("$snort_rules_path/*.rules") )) {
    $dirname =~ s/\.rules$//;
    $dirname =~ s/\/.*\///;
    print RULES_SQL "INSERT INTO signature VALUES ('$dirname');\n";
}

#
# Not snort rules
# 
print RULES_SQL "INSERT INTO signature VALUES ('spade');\n";
print RULES_SQL "INSERT INTO signature VALUES ('fw1-accept');\n";
print RULES_SQL "INSERT INTO signature VALUES ('fw1-drop');\n";
print RULES_SQL "INSERT INTO signature VALUES ('fw1-reject');\n";

close(RULES_SQL);

`mysql -u $user -p$pass -h$ossim_host  $base < services.sql`;
`mysql -u $user -p$pass -h$ossim_host  $base < protocols.sql`;
`mysql -u $user -p$pass -h$ossim_host  $base < rules.sql`;
`rm -f services.sql protocols.sql rules.sql`;

`mysql -u $user -p$pass -h$ossim_host  $base < ossim_schema_mysql.sql`;
`mysql -u $user -p$pass -h$ossim_host  $base < ossim_data.sql`;

