<?php

/*
 * Read config from /etc/ossim/framework/ossim.conf
 */
class ossim_conf {

    var $confs;

    function ossim_conf() {

        if (!$fd = fopen("/etc/ossim/framework/ossim.conf", "r")) {
            if (!$fd = fopen("/usr/local/etc/ossim/framework/ossim.conf", "r")) {
                echo "Can't open config file\n";
                exit();
            }
        }

        while (!feof($fd)) {
            $line = fgets($fd, 2048);
            if (strncmp($line, "#", 1)) {
                $conf = explode('=', $line);
                $type = $conf[0];
                if (isset($conf[1])) {
                    $value = $conf[1];
                    $this->confs["$type"] = trim($value);
                }
            }
        }

        fclose($fd);
    }

    function get_conf($type, $debug = TRUE) {

        if (isset($this->confs["$type"])) {
            return $this->confs["$type"];
        }

        else {
            require_once ('classes/Config.inc');
    
            $config = new Config();
            $value = $config->get_conf("$type");
            
            if ($value !== null)
                return $value;
        }

       /* 
        * $debug == FALSE
        * don't start output, session cache limiter problem 
        */ 
        if ($debug) {
            print "<b>Warning</b>: Error reading configuration: ";
            print "<b>$type</b> is not set<br/>\n";
        }
        return null;
    }

    function is_in_file($type) {
        return isset($this->confs["$type"]);
    }
}

?>
