<?php

require_once ('ossim_conf.inc');

class About {

    function About() {
        $this->version = "0.9.9";
        $this->date = "2008/02/19";
        $this->conf = $GLOBALS["CONF"];
        $this->logo = $this->conf->get_conf("ossim_link")."/pixmaps/logo3.jpg";
    }

    function get_version() {
        return $this->version;
    }
    
    function get_date() {
        return $this->date;
    }
    
    function get_logo() {
        return $this->logo;
    }
}

?>
