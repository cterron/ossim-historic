<?php

define ('CONF_FILE', "/etc/ossim/framework/ossim.conf");

if (!isset($GLOBALS["CONF"]))
    $GLOBALS["CONF"] = new ossim_conf();

class ossim_conf {

    var $conf;
    var $db_conf;

    function ossim_conf($read_db = True)
    {
        $this->conf = ossim_conf::get_conf_from_file(CONF_FILE);
        if ($read_db)
            $this->db_conf = ossim_conf::get_conf_from_db();
    }

    function get_conf_from_file($conffile)
    {
        if (!$fd = fopen($conffile, "r")) {
            echo "Can't open config file ($conffile)\n<br/>";
            exit();
        }

        $configuration = NULL;

        while (!feof($fd)) {
            $line = fgets($fd, 2048);
            $line = trim($line);
            if (strncmp($line, "#", 1)) {
                $conf = explode('=', $line);
                $type = trim($conf[0]);
                if (isset($conf[1])) {
                    $value = trim($conf[1]);
                    $configuration["$type"] = $value;
                }
            }
        }
        fclose($fd);

        return $configuration;
    }

    function get_conf_from_db()
    {
        require_once ("classes/Config.inc");
        $configuration = new Config();
        return $configuration->get_list();
    }

    function get_conf($type, $debug = true)
    {
        if (isset($this->conf[$type]))
            return $this->conf[$type];
        elseif (isset($this->db_conf[$type]))
            return $this->db_conf[$type];

       /*
        * $debug == FALSE
        * don't start output, session cache limiter problem
        */
        if ($debug) {
            print "<b>Warning</b>: Error reading configuration: ";
            print "<b>$type</b> is not set<br/>\n";
        }
        return NULL;
    }

    function is_in_file($type) {
        return isset($this->conf[$type]);
    }
}

?>
