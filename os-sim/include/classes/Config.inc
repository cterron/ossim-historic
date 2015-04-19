<?php

require_once ("ossim_db.inc");

class Config {

    var $confs = array();
    var $conn = null;

    function Config ()
    {
        $ossimdb = new ossim_db();
        $this->conn = $ossimdb->connect();

        $query = "SELECT * FROM config;";
        if (!$rs = &$this->conn->Execute($query)) {
            print $this->conn->ErrorMsg();
        } else {
            while (!$rs->EOF) 
            {
                $key   = $rs->fields["conf"];
                $value = $rs->fields["value"];
 
                $this->confs[$key] = $value;
                $rs->MoveNext();
            }
        }
    }
    
    function get_conf($key)
    {
        return $this->confs[$key];
    }

    function insert($key, $value)
    {
        $query = "INSERT INTO config (conf, value) VALUES ('$key', '$value')";

        if ($this->conn->Execute($query) === false) {
            print 'error updating: '. $this->conn->ErrorMsg().'<BR>';
            exit;
        }
    }
    
    function update($key, $value)
    {
        $query = "UPDATE config SET value = '$value' WHERE conf = '$key';";

        if ($this->conn->Execute($query) === false) {
            print 'error updating: '. $this->conn->ErrorMsg().'<BR>';
            exit;
        }
        if ($this->conn->Affected_Rows() == 0) {
            Config::insert($key, $value);
        }
    }


    /*  Reset default values */
    function reset()
    {
        $query = "DELETE FROM config";

	    if ($this->conn->Execute($query) === false) {
	        print 'error deleting: '. $this->conn->ErrorMsg().'<BR>';
	        exit;
	    }

        Config::common_reset();
	
        if (preg_match("/debian/i", $_SERVER["SERVER_SOFTWARE"]))
            Config::debian_reset();
        elseif (preg_match("/redhat|fedora/i", $_SERVER["SERVER_SOFTWARE"]))
            Config::fedora_reset();
        else
            Config::default_reset();
    }

    /* Common values */
    function common_reset()
    {
        /* Snort: Snort database and paths configuration */
        Config::insert('snort_path', '/etc/snort/');
        Config::insert('snort_rules_path', '/etc/snort/rules/');
        Config::insert('snort_type', 'mysql');
        Config::insert('snort_base', 'snort');
        Config::insert('snort_user', 'root');
        Config::insert('snort_pass', 'ossim');
        Config::insert('snort_host', 'localhost');
        Config::insert('snort_port', '3306');

        /* Server: Configure where the server is listening to */
        Config::insert('server_address', 'localhost');
        Config::insert('server_port', '40001');

        /* Metrics: Configure metric settings */
        Config::insert('recovery', '1');
        Config::insert('threshold', '300');

        /* PHP: PHP Configuration (graphs, acls, database api) */
        Config::insert('phpgacl_path', '/var/www/phpgacl/');
        Config::insert('fpdf_path', '/usr/share/fpdf/');
        Config::insert('use_resolv', '0');

        /* RRD: RRD Configuration (graphing) */
        Config::insert('graph_link', '/cgi-bin/draw_graph_combined.pl');
        Config::insert('rrdtool_lib_path', '/usr/lib/perl5/');
        
        /* Links: Links to other applications */
        Config::insert('ntop_link','http://localhost:3000');

        /* Backup: Backup configuration: backup database, directory, interval */
        Config::insert('backup_type','mysql');
        Config::insert('backup_base','snort_archive');
        Config::insert('backup_user','root');
        Config::insert('backup_pass','ossim');
        Config::insert('backup_host','localhost');
        Config::insert('backup_port','3306');
        Config::insert('backup_dir','/var/lib/ossim/backup');
        Config::insert('backup_day','5');
       
        /* Nessus: Nessus client configuration */
        Config::insert('nessus_user','ossim');
        Config::insert('nessus_pass','ossim');
        Config::insert('nessus_host','localhost');
        Config::insert('nessus_port','1241');
 
        /* ACID: Acid cache configuration */
        Config::insert('acid_user','ossim');
        Config::insert('acid_pass','ossim');
        Config::insert('ossim_web_user','admin');
        Config::insert('ossim_web_pass','admin');

        /* External applications: Path to other applications */
        Config::insert('nmap_path', '/usr/bin/nmap');
        Config::insert('p0f_path', '/usr/sbin/p0f');
        Config::insert('arpwatch_path', '/usr/sbin/arpwatch');
        Config::insert('mail_path', '/usr/bin/mail');
        Config::insert('touch_path', '/bin/tail');
        Config::insert('wget_path', '/usr/bin/wget');
    } 
    
    /* Debian specific values */
    function debian_reset() 
    {
        /* PHP: PHP Configuration (graphs, acls, database api) */
        Config::insert('jpgraph_path', '/usr/share/jpgraph/');
        Config::insert('adodb_path', '/usr/share/adodb/');

        /* RRD: RRD Configuration (graphing) */
        Config::insert('rrdtool_path', '/usr/bin/');
        Config::insert('mrtg_path', '/usr/bin/');
        Config::insert('mrtg_rrd_files_path','/opt/ossim/www/mrtg/');
        Config::insert('rrdpath_host','/opt/ossim/www/mrtg/host_qualification/');
        Config::insert('rrdpath_net','/opt/ossim/www/mrtg/net_qualification/');
        Config::insert('rrdpath_global','/opt/ossim/www/mrtg/global_qualification/');
        Config::insert('rrdpath_level','/opt/ossim/www/mrtg/level_qualification/');
        Config::insert('rrdpath_ntop','/var/lib/ntop/rrd');
        Config::insert('font_path','/opt/ossim/contrib/fonts/Vera.ttf');

        /* Links: Links to other applications */
        Config::insert('opennms_link','http://localhost:8180/opennms/');

        /* Nessus: Nessus client configuration */
        Config::insert('nessus_path','/usr/bin/nessus');
        Config::insert('nessus_rpt_path','/opt/ossim/www/vulnmeter/');

        /* ACID: Acid cache configuration */
        Config::insert('acid_link','/acidlab/');
        Config::insert('acid_path','/usr/share/acidlab/');
    }

    /* fedora/redhat specific values */
    function fedora_reset() {
        /* PHP: PHP Configuration (graphs, acls, database api) */
        Config::insert('jpgraph_path', '/usr/share/jpgraph/');
        Config::insert('adodb_path', '/var/www/adodb-411/');

        /* RRD: RRD Configuration (graphing) */
        Config::insert('rrdtool_path', '/usr/bin/');
        Config::insert('mrtg_path', '/usr/bin/');
        Config::insert('mrtg_rrd_files_path','/var/www/ossim/mrtg/');
        Config::insert('rrdpath_host','/var/www/ossim/mrtg/host_qualification/');
        Config::insert('rrdpath_net','/var/www/ossim/mrtg/net_qualification/');
        Config::insert('rrdpath_global','/var/www/ossim/mrtg/global_qualification/');
        Config::insert('rrdpath_level','/var/www/ossim/mrtg/level_qualification/');
        Config::insert('rrdpath_ntop','/usr/share/ntop/rrd/');
        Config::insert('font_path','/usr/share/ossim/fonts/Vera.ttf');

        /* Links: Links to other applications */
        Config::insert('opennms_link','http://localhost:8080/opennms/');

        /* Nessus: Nessus client configuration */
        Config::insert('nessus_path','/usr/local/bin/nessus/');
        Config::insert('nessus_rpt_path','/var/www/ossim/vulnmeter/');

        /* ACID: Acid cache configuration */
        Config::insert('acid_link','/acid/');
        Config::insert('acid_path','/var/www/acid/');
    }

    /* default values */
    function default_reset() {
        Config::debian_reset();
    }

}

?>
