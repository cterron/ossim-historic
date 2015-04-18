<?php

/*
 * Read config from /etc/ossim.conf
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
            $line = fgets($fd, 1024);
            if (strncmp($line, "#", 1)) {
                $conf = explode('=', $line);
                $type = $conf[0];
                if(isset($conf[1])){
                $value = $conf[1];
                }
                $this->confs["$type"] = trim($value);
            }
        }

        fclose($fd);
    }

    function get_conf($type) {
        return $this->confs["$type"];
    }
}

?>
