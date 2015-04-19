#!/usr/bin/perl

# Script 
#
# 2004-02-11 Fabio Ospitia Trujillo <fot@ossim.net>

use ossim_conf;
use DBI;
use POSIX;
use Compress::Zlib;

use strict;
use warnings;

my $backup_dir = $ossim_conf::ossim_data->{"backup_dir"};
my $backup_day = $ossim_conf::ossim_data->{"backup_day"};

# Data Source 
my $snort_type = $ossim_conf::ossim_data->{"snort_type"};
my $snort_name = $ossim_conf::ossim_data->{"snort_base"};
my $snort_host = $ossim_conf::ossim_data->{"snort_host"};
my $snort_port = $ossim_conf::ossim_data->{"snort_port"};
my $snort_user = $ossim_conf::ossim_data->{"snort_user"};
my $snort_pass = $ossim_conf::ossim_data->{"snort_pass"};

my $snort_dsn = "dbi:" . $snort_type . ":" . $snort_name . ":" . $snort_host . ":" . $snort_port . ":";

my $snort_conn = DBI->connect($snort_dsn, $snort_user, $snort_pass) or die "Can't connect to Database\n";

my $sec_1day = 60 * 60 * 24;
my $sec_curr = int (time () / $sec_1day) * $sec_1day;
my $sec_backup = $sec_curr -  ($sec_1day * $backup_day);

sub acid_event_insert_clause_from_row {
    my ($row) = @_;

    my $insert = "INSERT INTO acid_event (sid, cid";
    my $values = "VALUES ($row->[0], $row->[1]";

    $insert .= ", signature";
    $values .= ", $row->[2]";

    if ($row->[3]) {
	$insert .= ", sig_name";
	$values .= ", '$row->[3]'";
    }
    if ($row->[4]) {
	$insert .= ", sig_class_id";
	$values .= ", $row->[4]";
    }
    if ($row->[5]) {
	$insert .= ", sig_priority";
	$values .= ", $row->[5]";
    }

    $insert .= ", timestamp";
    $values .= ", '$row->[6]'";

    if ($row->[7]) {
	$insert .= ", ip_src";
	$values .= ", $row->[7]";
    }
    if ($row->[8]) {
	$insert .= ", ip_dst";
	$values .= ", $row->[8]";
    }
    if ($row->[9]) {
	$insert .= ", ip_proto";
	$values .= ", $row->[9]";
    }
    if ($row->[10]) {
	$insert .= ", layer4_sport";
	$values .= ", $row->[10]";
    }
    if ($row->[11]) {
	$insert .= ", layer4_dport";
	$values .= ", $row->[11]";
    }
    if ($row->[12]) {
	$insert .= ", ossim_type";
	$values .= ", $row->[12]";
    }
    if ($row->[13]) {
	$insert .= ", ossim_priority";
	$values .= ", $row->[13]";
    }
    if ($row->[14]) {
	$insert .= ", ossim_reliability";
	$values .= ", $row->[14]";
    }
    if ($row->[15]) {
	$insert .= ", ossim_asset_src";
	$values .= ", $row->[15]";
    }
    if ($row->[16]) {
	$insert .= ", ossim_asset_dst";
	$values .= ", $row->[16]";
    }
    if ($row->[17]) {
	$insert .= ", ossim_risk_c";
	$values .= ", $row->[17]";
    }
    if ($row->[18]) {
	$insert .= ", ossim_risk_a";
	$values .= ", $row->[18]";
    }

    $insert .= ") " . $values . ");";

    return $insert;
}

sub ossim_event_insert_clause_from_row {
    my ($row) = @_;

    my $insert = "INSERT INTO ossim_event (sid, cid";
    my $values = "VALUES ($row->[0], $row->[1]";

    if ($row->[2]) {
	$insert .= ", type";
	$values .= ", $row->[2]";
    }
    if ($row->[3]) {
	$insert .= ", priority";
	$values .= ", $row->[3]";
    }
    if ($row->[4]) {
	$insert .= ", reliability";
	$values .= ", $row->[4]";
    }
    if ($row->[5]) {
	$insert .= ", asset_src";
	$values .= ", $row->[5]";
    }
    if ($row->[6]) {
	$insert .= ", asset_dst";
	$values .= ", $row->[6]";
    }
    if ($row->[7]) {
	$insert .= ", risk_c";
	$values .= ", $row->[7]";
    }
    if ($row->[8]) {
	$insert .= ", risk_a";
	$values .= ", $row->[8]";
    }

    $insert .= ") " . $values . ");";

    return $insert;
}

sub data_insert_clause_from_row {
    my ($row) = @_;

    my $insert = "INSERT INTO data (sid, cid";
    my $values = "VALUES ($row->[0], $row->[1]";

    if ($row->[2]) {
	$insert .= ", data_payload";
	$values .= ", '$row->[2]'";
    }

    $insert .= ") " . $values . ");";

    return $insert;
}

sub opt_insert_clause_from_row {
    my ($row) = @_;

    my $insert = "INSERT INTO opt (sid, cid";
    my $values = "VALUES ($row->[0], $row->[1]";

    $insert .= ", optid";
    $values .= ", $row->[2]";

    $insert .= ", opt_proto";
    $values .= ", $row->[3]";

    $insert .= ", opt_code";
    $values .= ", $row->[4]";

    if ($row->[5]) {
	$insert .= ", opt_len";
	$values .= ", $row->[5]";
    }
    if ($row->[6]) {
	$insert .= ", opt_data";
	$values .= ", '$row->[6]'";
    }
    
    $insert .= ") " . $values . ");";
    
    return $insert;
}

sub tcphdr_insert_clause_from_row {
    my ($row) = @_;

    my $insert = "INSERT INTO tcphdr (sid, cid";
    my $values = "VALUES ($row->[0], $row->[1]";

    $insert .= ", tcp_sport";
    $values .= ", $row->[2]";

    $insert .= ", tcp_dport";
    $values .= ", $row->[3]";

    if ($row->[4]) {
	$insert .= ", tcp_seq";
	$values .= ", $row->[4]";
    }
    if ($row->[5]) {
	$insert .= ", tcp_ack";
	$values .= ", $row->[5]";
    }
    if ($row->[6]) {
	$insert .= ", tcp_off";
	$values .= ", $row->[6]";
    }
    if ($row->[7]) {
	$insert .= ", tcp_res";
	$values .= ", $row->[7]";
    }

    $insert .= ", tcp_flags";
    $values .= ", $row->[8]";

    if ($row->[9]) {
	$insert .= ", tcp_win";
	$values .= ", $row->[9]";
    }
    if ($row->[10]) {
	$insert .= ", tcp_csum";
	$values .= ", $row->[10]";
    }
    if ($row->[11]) {
	$insert .= ", tcp_urp";
	$values .= ", $row->[11]";
    }

    $insert .= ") " . $values . ");";

    return $insert;
}

sub udphdr_insert_clause_from_row {
    my ($row) = @_;

    my $insert = "INSERT INTO udphdr (sid, cid";
    my $values = "VALUES ($row->[0], $row->[1]";

    $insert .= ", udp_sport";
    $values .= ", $row->[2]";

    $insert .= ", udp_dport";
    $values .= ", $row->[3]";

    if ($row->[4]) {
	$insert .= ", udp_len";
	$values .= ", $row->[4]";
    }
    if ($row->[5]) {
	$insert .= ", udp_csum";
	$values .= ", $row->[5]";
    }

    $insert .= ") " . $values . ");";

    return $insert;
}

sub iphdr_insert_clause_from_row {
    my ($row) = @_;

    my $insert = "INSERT INTO iphdr (sid, cid";
    my $values = "VALUES ($row->[0], $row->[1]";

    $insert .= ", ip_src";
    $values .= ", $row->[2]";

    $insert .= ", ip_dst";
    $values .= ", $row->[3]";

    if ($row->[4]) {
	$insert .= ", ip_ver";
	$values .= ", $row->[4]";
    }
    if ($row->[5]) {
	$insert .= ", ip_hlen";
	$values .= ", $row->[5]";
    }
    if ($row->[6]) {
	$insert .= ", ip_tos";
	$values .= ", $row->[6]";
    }
    if ($row->[7]) {
	$insert .= ", ip_len";
	$values .= ", $row->[7]";
    }
    if ($row->[8]) {
	$insert .= ", ip_id";
	$values .= ", $row->[8]";
    }
    if ($row->[9]) {
	$insert .= ", ip_flags";
	$values .= ", $row->[9]";
    }
    if ($row->[10]) {
	$insert .= ", ip_off";
	$values .= ", $row->[10]";
    }
    if ($row->[11]) {
	$insert .= ", ip_ttl";
	$values .= ", $row->[11]";
    }

    $insert .= ", ip_proto";
    $values .= ", $row->[12]";

    if ($row->[13]) {
	$insert .= ", ip_csum";
	$values .= ", $row->[13]";
    }

    $insert .= ") " . $values . ");";

    return $insert;
}

sub icmphdr_insert_clause_from_row {
    my ($row) = @_;

    my $insert = "INSERT INTO icmphdr (sid, cid";
    my $values = "VALUES ($row->[0], $row->[1]";

    $insert .= ", icmp_type";
    $values .= ", $row->[2]";

    $insert .= ", icmp_code";
    $values .= ", $row->[3]";

    if ($row->[4]) {
	$insert .= ", icmp_csum";
	$values .= ", $row->[4]";
    }
    if ($row->[5]) {
	$insert .= ", icmp_id";
	$values .= ", $row->[5]";
    }
    if ($row->[6]) {
	$insert .= ", icmp_seq";
	$values .= ", $row->[6]";
    }

    $insert .= ") " . $values . ");";

    return $insert;
}

sub event_insert_clause_from_row {
    my ($row) = @_;

    my $insert = "INSERT INTO event (sid, cid, signature, timestamp) " .
	"VALUES ($row->[0], $row->[1], $row->[2], '$row->[3]');";

    return $insert;
}

sub delete_clause_from_pky {
    my ($table, $sid, $cid) = @_;

    my $delete = "DELETE FROM $table WHERE sid = $sid AND cid = $cid;";

    return $delete;
}

sub backup_acid_event {
    my ($conn, $where, $fd_insert, $fd_delete) = @_;

    my $query = "SELECT acid_event.sid, acid_event.cid, acid_event.signature, sig_name, sig_class_id, sig_priority, acid_event.timestamp, " .
	"ip_src, ip_dst, ip_proto, layer4_sport, layer4_dport, ossim_type, ossim_priority, " .
	"ossim_reliability, ossim_asset_src, ossim_asset_dst, ossim_risk_c, ossim_risk_a " . 
	"FROM event INNER JOIN acid_event ON (event.sid=acid_event.sid AND event.cid=acid_event.cid) " .
	$where;

    my $stm = $conn->prepare($query);
    $stm->execute();

    while (my $row = $stm->fetchrow_arrayref) {
	my $insert = acid_event_insert_clause_from_row ($row);
	my $delete = delete_clause_from_pky ("acid_event", $row->[0], $row->[1]);

	POSIX::write($fd_insert, "$insert\n", length($insert) + 1);
	POSIX::write($fd_delete, "$delete\n", length($delete) + 1);
    }
}

sub backup_ossim_event {
    my ($conn, $where, $fd_insert, $fd_delete) = @_;

    my $query = "SELECT ossim_event.sid, ossim_event.cid, type, priority, reliability, asset_src, asset_dst, risk_c, risk_a " .
	"FROM event INNER JOIN ossim_event ON (event.sid=ossim_event.sid AND event.cid=ossim_event.cid) " .
	$where;

    my $stm = $conn->prepare($query);
    $stm->execute();

    while (my $row = $stm->fetchrow_arrayref) {
	my $insert = ossim_event_insert_clause_from_row ($row);
	my $delete = delete_clause_from_pky ("ossim_event", $row->[0], $row->[1]);

	POSIX::write($fd_insert, "$insert\n", length($insert) + 1);
	POSIX::write($fd_delete, "$delete\n", length($delete) + 1);
    }
}

sub backup_data {
    my ($conn, $where, $fd_insert, $fd_delete) = @_;

    my $query = "SELECT data.sid, data.cid, data_payload " .
	"FROM event INNER JOIN data ON (event.sid=data.sid AND event.cid=data.cid) " .
	$where;

    my $stm = $conn->prepare($query);
    $stm->execute();

    while (my $row = $stm->fetchrow_arrayref) {
	my $insert = data_insert_clause_from_row ($row);
	my $delete = delete_clause_from_pky ("data", $row->[0], $row->[1]);

	POSIX::write($fd_insert, "$insert\n", length($insert) + 1);
	POSIX::write($fd_delete, "$delete\n", length($delete) + 1);
    }
}

sub backup_opt {
    my ($conn, $where, $fd_insert, $fd_delete) = @_;

    my $query = "SELECT opt.sid, opt.cid, optid, opt_proto, opt_code, opt_len, opt_data " .
	"FROM event INNER JOIN opt ON (event.sid=opt.sid AND event.cid=opt.cid) " .
	$where;

    my $stm = $conn->prepare($query);
    $stm->execute();

    while (my $row = $stm->fetchrow_arrayref) {
	my $insert = opt_insert_clause_from_row ($row);
	my $delete = delete_clause_from_pky ("opt", $row->[0], $row->[1]);

	POSIX::write($fd_insert, "$insert\n", length($insert) + 1);
	POSIX::write($fd_delete, "$delete\n", length($delete) + 1);
    }
};

sub backup_tcphdr {
    my ($conn, $where, $fd_insert, $fd_delete) = @_;

    my $query = "SELECT tcphdr.sid, tcphdr.cid, tcp_sport, tcp_dport, tcp_seq, tcp_ack, tcp_off, tcp_res, tcp_flags, tcp_win, tcp_csum, tcp_urp " .
	"FROM event INNER JOIN tcphdr ON (event.sid=tcphdr.sid AND event.cid=tcphdr.cid) " .
	$where;

    my $stm = $conn->prepare($query);
    $stm->execute();

    while (my $row = $stm->fetchrow_arrayref) {
	my $insert = tcphdr_insert_clause_from_row ($row);
	my $delete = delete_clause_from_pky ("tcphdr", $row->[0], $row->[1]);

	POSIX::write($fd_insert, "$insert\n", length($insert) + 1);
	POSIX::write($fd_delete, "$delete\n", length($delete) + 1);
    }
}

sub backup_udphdr {
    my ($conn, $where, $fd_insert, $fd_delete) = @_;

    my $query = "SELECT udphdr.sid, udphdr.cid, udp_sport, udp_dport, udp_len, udp_csum " .
	"FROM event INNER JOIN udphdr ON (event.sid=udphdr.sid AND event.cid=udphdr.cid) " .
	$where;

    my $stm = $conn->prepare($query);
    $stm->execute();

    while (my $row = $stm->fetchrow_arrayref) {
	my $insert = udphdr_insert_clause_from_row ($row);
	my $delete = delete_clause_from_pky ("udphdr", $row->[0], $row->[1]);

	POSIX::write($fd_insert, "$insert\n", length($insert) + 1);
	POSIX::write($fd_delete, "$delete\n", length($delete) + 1);
    }
}

sub backup_iphdr {
    my ($conn, $where, $fd_insert, $fd_delete) = @_;

    my $query = "SELECT iphdr.sid, iphdr.cid, ip_src, ip_dst, ip_ver, ip_hlen, ip_tos, ip_len, ip_id, ip_flags, ip_off, ip_ttl, ip_proto, ip_csum " .
	"FROM event INNER JOIN iphdr ON (event.sid=iphdr.sid AND event.cid=iphdr.cid) " .
	$where;

    my $stm = $conn->prepare($query);
    $stm->execute();

    while (my $row = $stm->fetchrow_arrayref) {
	my $insert = iphdr_insert_clause_from_row ($row);
	my $delete = delete_clause_from_pky ("iphdr", $row->[0], $row->[1]);

	POSIX::write($fd_insert, "$insert\n", length($insert) + 1);
	POSIX::write($fd_delete, "$delete\n", length($delete) + 1);
    }
}

sub backup_icmphdr {
    my ($conn, $where, $fd_insert, $fd_delete) = @_;

    my $query = "SELECT icmphdr.sid, icmphdr.cid, icmp_type, icmp_code, icmp_csum, icmp_id, icmp_seq " . 
	"FROM event INNER JOIN icmphdr ON (event.sid=icmphdr.sid AND event.cid=icmphdr.cid) " .
	$where;

    my $stm = $conn->prepare($query);
    $stm->execute();

    while (my $row = $stm->fetchrow_arrayref) {
	my $insert = icmphdr_insert_clause_from_row ($row);
	my $delete = delete_clause_from_pky ("icmphdr", $row->[0], $row->[1]);

	POSIX::write($fd_insert, "$insert\n", length($insert) + 1);
	POSIX::write($fd_delete, "$delete\n", length($delete) + 1);
    }
}

sub backup_event {
    my ($conn, $where, $fd_insert, $fd_delete) = @_;

    my $query = "SELECT sid, cid, signature, timestamp FROM event " . 
	$where;

    my $stm = $conn->prepare($query);
    $stm->execute();

    while (my $row = $stm->fetchrow_arrayref) {
	my $insert = event_insert_clause_from_row ($row);
	my $delete = delete_clause_from_pky ("event", $row->[0], $row->[1]);

	POSIX::write($fd_insert, "$insert\n", length($insert) + 1);
	POSIX::write($fd_delete, "$delete\n", length($delete) + 1);
    }
}

sub backup_gzip {
    my ($from, $to) = @_;

    open (INPUT, "$from") or die "Can't open file log $from";

    my $gz = gzopen("$to", "wb") or die "Can't open file log $to";
    while (<INPUT>) {
	$gz->gzwrite($_) or die "Error writing: %gzerrno\n";
    }
    $gz->gzclose;

    close (INPUT);
}

sub backup_execute_file {
    my ($conn, $file) = @_;

    open (INPUT, "$file") or die "Can't open file log $file";
    while (<INPUT>) {
	s/\;//;
	my $stm = $conn->prepare($_);
	$stm->execute();
    }
    close (INPUT);
}

sub backup_to_file {
    my $query = "SELECT min(timestamp) FROM event";
    my $stm = $snort_conn->prepare($query);
    $stm->execute();

    return unless (my $row = $stm->fetchrow_arrayref);

    unless ($row->[0] =~ m/^(\d+)-(\d+)-(\d+)*/) {
	return;
    }

    my $sec = POSIX::mktime( 0, 0, 0, $3, $2 - 1, $1 - 1900);
    while ($sec <= $sec_backup) {
	my $date = POSIX::strftime ("%Y-%m-%d", localtime($sec));
	my $file = POSIX::strftime ("%Y%m%d", localtime($sec));
	my $file_insert = "$backup_dir/insert-$file.sql";
	my $file_delete = "$backup_dir/delete-$file.sql";

	# If gzip file exist
	if (-e "$file_insert.gz") {
	    print ("File exist! $file_insert.gz\n");
	    $sec += $sec_1day;
	    next;
	}

	my $fd_insert = POSIX::open($file_insert, &POSIX::O_CREAT | &POSIX::O_WRONLY | &POSIX::O_TRUNC, 0640) or die "Can't open file log $file_insert";
	my $fd_delete = POSIX::open($file_delete, &POSIX::O_CREAT | &POSIX::O_WRONLY | &POSIX::O_TRUNC, 0640) or die "Can't open file log $file_delete";

	my $where = "WHERE event.timestamp >= '$date 00:00:00' AND event.timestamp <= '$date 23:59:59'";

	backup_event ($snort_conn, $where, $fd_insert, $fd_delete);
	backup_icmphdr ($snort_conn, $where, $fd_insert, $fd_delete);
	backup_iphdr ($snort_conn, $where, $fd_insert, $fd_delete);
	backup_udphdr ($snort_conn, $where, $fd_insert, $fd_delete);
	backup_tcphdr ($snort_conn, $where, $fd_insert, $fd_delete);
	backup_opt ($snort_conn, $where, $fd_insert, $fd_delete);
	backup_data ($snort_conn, $where, $fd_insert, $fd_delete);
	backup_ossim_event ($snort_conn, $where, $fd_insert, $fd_delete);
	backup_acid_event ($snort_conn, $where, $fd_insert, $fd_delete);

	POSIX::close($fd_insert);
	POSIX::close($fd_delete);

	# If file is empty
	if (-z "$file_insert") {
	    unlink ($file_insert);
	    unlink ($file_delete);
	    $sec += $sec_1day;
	    next;
	}

	# Gzip the files
	backup_gzip("$file_insert", "$file_insert.gz");
	backup_gzip("$file_delete", "$file_delete.gz");

	# Delete data
	backup_execute_file($snort_conn, $file_delete);

	# Remove files
	unlink ($file_insert);
	unlink ($file_delete);

	$sec += $sec_1day;
    }
}

sub backupdb_exit {
    $snort_conn->disconnect;
}

sub backupdb_main {
    backup_to_file ();

    backupdb_exit();
}

backupdb_main();
