<?php
require_once 'classes/Session.inc';
Session::logcheck("MenuMonitors", "MonitorsAvailability");

apache_setenv('OSSIM_NAGIOS_ALLOWED', '1');
$url = str_replace('nagios', 'secured_nagios', $_SERVER['REQUEST_URI']);
virtual($url);
?>