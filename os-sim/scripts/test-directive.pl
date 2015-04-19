#!/usr/bin/perl

use Switch;
use IO::Socket;

my $host='127.0.0.1';
my $port='40001';
my $prot='tcp';


if(!$ARGV[0]) {
    print "Usage: $0 [num_directive]\n";
    exit();
}
  
my $num = $ARGV[0];

my $sock = new IO::Socket::INET (PeerAddr => $host, PeerPort => $port, Proto => $prot);
die "Could not create socket: $!\n" unless $sock;

my $sensor = '192.168.2.100';
my $src_ip;
my $dst_ip;
my $src_port;
my $dst_port;

sub directive1 {
    $src_ip = '192.168.1.10';
    $dst_ip = '192.168.1.11';
    $src_port = '1765';
    $dst_port = '139';

    my $alert_path_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1001" plugin_sid="113" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

    my $alert_path_1_1 = 'alert type="monitor" date="2004-06-29 21:35:04" plugin_id="2005" plugin_sid="248" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'" condition="ge" value="10"';
    my $alert_path_1_1_1 = 'alert type="monitor" date="2004-06-29 21:35:04" plugin_id="2005" plugin_sid="248" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'" condition="ge" value="300"';
    my $alert_path_1_1_2 = 'alert type="monitor" date="2004-06-29 21:35:04" plugin_id="2001" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$dst_ip.'" condition="ge" value="200"';

    my $alert_path_1_2 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1001" plugin_sid="125" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$dst_ip.'" src_port="'.$dst_port.'" dst_ip="'.$src_ip.'" dst_port="'.$src_port.'"';
    my $alert_path_1_2_1 = 'alert type="monitor" date="2004-06-29 21:35:04" plugin_id="2005" plugin_sid="248" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'" condition="ge" value="300"';
    my $alert_path_1_2_2 = 'alert type="monitor" date="2004-06-29 21:35:04" plugin_id="2001" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$dst_ip.'" condition="ge" value="200"';

    # PATH 1,1,1
    print $sock $alert_path_1 . "\n";
    print $sock $alert_path_1_1 . "\n";
    print $sock $alert_path_1_1_1 . "\n";

    sleep (2);

    # PATH 1,1,2
    print $sock $alert_path_1 . "\n";
    print $sock $alert_path_1_1 . "\n";
    print $sock $alert_path_1_1_2 . "\n";

    sleep (2);

    # PATH 1,2,1
    print $sock $alert_path_1 . "\n";
    print $sock $alert_path_1_2 . "\n";
    print $sock $alert_path_1_2_1 . "\n";

    sleep (2);

    # PATH 1,2,2
    print $sock $alert_path_1 . "\n";
    print $sock $alert_path_1_2 . "\n";
    print $sock $alert_path_1_2_2 . "\n";
}

sub directive2 {
    $src_ip = '192.168.1.10';
    $dst_ip = '192.168.1.11';
    $src_port = '1765';
    $dst_port = '139';

    my $alert_path_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="102" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

    my $alert_path_1_1 = 'alert type="monitor" date="2004-06-29 21:35:04" plugin_id="2005" plugin_sid="248" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'" condition="ge" value="10"';
    my $alert_path_1_1_1 = 'alert type="monitor" date="2004-06-29 21:35:04" plugin_id="2005" plugin_sid="248" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'" condition="ge" value="300"';
    my $alert_path_1_1_2 = 'alert type="monitor" date="2004-06-29 21:35:04" plugin_id="2001" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$dst_ip.'" condition="ge" value="200"';

    # PATH 1,1,1
    print $sock $alert_path_1 . "\n";
    print $sock $alert_path_1_1 . "\n";
    print $sock $alert_path_1_1_1 . "\n";

    sleep (2);

    # PATH 1,1,2
    print $sock $alert_path_1 . "\n";
    print $sock $alert_path_1_1 . "\n";
    print $sock $alert_path_1_1_2 . "\n";
}

sub directive3 {
    $src_ip = '192.168.1.10';
    $dst_ip = '192.168.1.11';
    $src_port = '1765';
    $dst_port = '139';

    my $alert_path_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1001" plugin_sid="1185" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

    my $alert_path_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1001" plugin_sid="1000001" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$dst_ip.'" src_port="'.$dst_port.'" dst_ip="'.$src_ip.'" dst_port="'.$src_port.'"';

    # PATH 1,1,1
    print $sock $alert_path_1 . "\n";
    print $sock $alert_path_1_1 . "\n";    
}

sub directive4 {
    my $i;
    my $n;
    my $m;

    $src_ip = '192.168.1.10';
    $dst_ip = '192.168.1.11';
    $src_port = '1765';
    $dst_port = '139';


    my $alert_path_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

    print $sock $alert_path_1 . "\n";

    my $alert_path_1_1;
    for ($i = 1; $i <= 15; $i++) {
	$alert_path_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="192.168.2.'.$i.'" dst_port="'.$dst_port.'"';

	print $sock $alert_path_1_1 . "\n";
    }

    sleep (5);

    my $alert_path_1_1_1;
    for ($i = 1; $i <= 300; $i++) {
	$n = int ($i / 254);
	$m = $i - ($n * 254) + 1;

	$alert_path_1_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="192.168.'.$n.'.'.$m.'" dst_port="'.$dst_port.'"';

	print $sock $alert_path_1_1_1 . "\n";
    }

    sleep (5);

    my $alert_path_1_1_1_1;
    for ($i = 1; $i <= 2000; $i++) {
	$n = int ($i / 254);
	$m = $i - ($n * 254) + 1;

	$alert_path_1_1_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="192.168.'.$n.'.'.$m.'" dst_port="'.$dst_port.'"';

	print $sock $alert_path_1_1_1_1 . "\n";
    }

    sleep (5);

    my $alert_path_1_1_1_1_1;
    for ($i = 1; $i <= 20000; $i++) {
	$n = int ($i / 254);
	$m = $i - ($n * 254) + 1;

	$alert_path_1_1_1_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="192.168.'.$n.'.'.$m.'" dst_port="'.$dst_port.'"';

	print $sock $alert_path_1_1_1_1_1 . "\n";
    }
}

sub directive5 {
    my $i;
    my $n;
    my $m;

    $src_ip = '192.168.1.10';
    $dst_ip = '192.168.1.11';
    $src_port = '1765';
    $dst_port = '137';


    my $alert_path_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

    print $sock $alert_path_1 . "\n";

    my $alert_path_1_1;
    for ($i = 1; $i <= 75; $i++) {
	$alert_path_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="192.168.2.'.$i.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

	print $sock $alert_path_1_1 . "\n";
    }

    sleep (5);

    my $alert_path_1_1_1;
    for ($i = 1; $i <= 1500; $i++) {
	$n = int ($i / 254);
	$m = $i - ($n * 254) + 1;

	$alert_path_1_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="192.168.'.$n.'.'.$m.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

	print $sock $alert_path_1_1_1 . "\n";
    }

    sleep (5);

    my $alert_path_1_1_1_1;
    for ($i = 1; $i <= 10000; $i++) {
	$n = int ($i / 254);
	$m = $i - ($n * 254) + 1;

	$alert_path_1_1_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="192.168.'.$n.'.'.$m.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

	print $sock $alert_path_1_1_1_1 . "\n";
    }

    sleep (5);

    my $alert_path_1_1_1_1_1;
    for ($i = 1; $i <= 50000; $i++) {
	$n = int ($i / 254);
	$m = $i - ($n * 254) + 1;

	$alert_path_1_1_1_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="192.168.'.$n.'.'.$m.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

	print $sock $alert_path_1_1_1_1_1 . "\n";
    }
}


sub directive6 {
    my $i;

    $src_ip = '192.168.1.10';
    $dst_ip = '192.168.1.11';
    $src_port = '1765';
    $dst_port = '137';

    my $alert_path_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1508" plugin_sid="29" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

    print $sock $alert_path_1 . "\n";

    my $alert_path_1_1;
    for ($i = 1; $i <= 3; $i++) {
	$alert_path_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1508" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

	print $sock $alert_path_1_1 . "\n";
    }
}

sub directive7 {
    my $i;

    $src_ip = '192.168.1.10';
    $dst_ip = '192.168.1.11';
    $src_port = '1765';
    $dst_port = '137';

    my $alert_path_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1508" plugin_sid="2" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

    print $sock $alert_path_1 . "\n";

    my $alert_path_1_1;
    for ($i = 1; $i <= 49; $i++) {
	$alert_path_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1508" plugin_sid="2" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

	print $sock $alert_path_1_1 . "\n";
    }
}

sub directive8 {
    my $i;

    $src_ip = '192.168.1.10';
    $dst_ip = '192.168.1.11';
    $src_port = '1765';
    $dst_port = '137';

    my $alert_path_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1508" plugin_sid="2" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

    print $sock $alert_path_1 . "\n";

    my $alert_path_1_1;
    for ($i = 1; $i <= 49; $i++) {
	$alert_path_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1508" plugin_sid="2" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

	print $sock $alert_path_1_1 . "\n";
    }
}

sub directive9 {
    my $i;
    
    $src_ip = '192.168.1.10';
    $dst_ip = '192.168.1.11';
    $src_port = '1765';
    $dst_port = '137';
    
    my $alert_path_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1505" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';
    my $alert_path_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1505" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$dst_ip.'" src_port="'.$src_port.'" dst_ip="'.$src_ip.'" dst_port="'.$dst_port.'"';
    
    print $sock $alert_path_1 . "\n";
    print $sock $alert_path_1_1 . "\n";
}

sub directive10 {
    my $i;

    $src_ip = '192.168.1.10';
    $dst_ip = '192.168.1.11';
    $src_port = '1765';
    $dst_port = '80';

    my $alert_path_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

    print $sock $alert_path_1 . "\n";

    my $alert_path_1_1;
    for ($i = 1; $i <= 75; $i++) {
	$alert_path_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

	print $sock $alert_path_1_1 . "\n";
    }

    sleep (5);

    my $alert_path_1_1_1;
    for ($i = 1; $i <= 1500; $i++) {
	$alert_path_1_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

	print $sock $alert_path_1_1_1 . "\n";
    }

    sleep (5);

    my $alert_path_1_1_1_1;
    for ($i = 1; $i <= 10000; $i++) {
	$alert_path_1_1_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

	print $sock $alert_path_1_1_1_1 . "\n";
    }

    sleep (5);

    my $alert_path_1_1_1_1_1;
    for ($i = 1; $i <= 100000; $i++) {
	$alert_path_1_1_1_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

	print $sock $alert_path_1_1_1_1_1 . "\n";
    }
}

sub directive11 {
    my $i;

    $src_ip = '192.168.1.10';
    $dst_ip = '192.168.1.11';
    $src_port = '1765';
    $dst_port = '1';


    my $alert_path_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

    print $sock $alert_path_1 . "\n";

    my $alert_path_1_1;
    for ($i = 2; $i <= 16; $i++) {
	$alert_path_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$i.'"';

	print $sock $alert_path_1_1 . "\n";
    }

    sleep (5);

    my $alert_path_1_1_1;
    for ($i = 17; $i <= 400; $i++) {

	$alert_path_1_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$i.'"';

	print $sock $alert_path_1_1_1 . "\n";
    }

    sleep (5);

    my $alert_path_1_1_1_1;
    for ($i = 401; $i <= 2000; $i++) {

	$alert_path_1_1_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$i.'"';

	print $sock $alert_path_1_1_1_1 . "\n";
    }

    sleep (5);

    my $alert_path_1_1_1_1_1;
    for ($i = 2001; $i <= 20000; $i++) {

	$alert_path_1_1_1_1_1 = 'alert type="detector" date="2004-06-30 10:35:49" plugin_id="1104" plugin_sid="1" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$i.'"';

	print $sock $alert_path_1_1_1_1_1 . "\n";
    }
}



sub directive12() {

    my $i, $j;

    $src_ip = '192.168.1.10';
    $dst_ip = '192.168.1.11';
    $src_port = '';
    $dst_port = '22';

    for ($j = 0; $j < 10; $j++) {

        for ($i = 1; $i <= 3; $i++) {

            my $alert_path = 'alert type="detector" date="2005-02-11 10:35:49" plugin_id="4002" plugin_sid="'.$i.'" sensor="'.$sensor.'" interface="eth0" protocol="TCP" src_ip="'.$src_ip.'" src_port="'.$src_port.'" dst_ip="'.$dst_ip.'" dst_port="'.$dst_port.'"';

            print $sock "$alert_path\n";
        }
    }
}


sub main {
    switch ($num) {
	case 1 {
	    directive1();
	}
	case 2  { 
	    directive2();
	}
	case 3  {
	    directive3();
	}
	case 4  {
	    directive4();
	}
	case 5  {
	    directive5();
	}
	case 6  {
	    directive6();
	}
	case 7  {
	    directive7();
	}
	case 8  {
	    directive8();
	}
	case 9  {
	    directive9();
	}
	case 10  {
	    directive10();
	}
	case 11  {
	    directive11();
	}
    case 12 {
        directive12();
    }
    }
    
    close($sock);
}

main();
