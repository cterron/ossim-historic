<?php

class WebIndicator {

    var $indicators = array();

    function update_display()
    {
        echo "<script language=\"JavaScript\" type=\"text/javascript\">\n";

        foreach($this->indicators as $key => $indics) {
            foreach($indics as $indic) {

                if (isset($indic["frame"])) {
                    echo "var webindic=parent." . $indic["frame"] . ".document.getElementById(\"" . $indic["id"] . "\");\n";
                } else {
                    echo "var webindic=document.getElementById(\"" . $indic["id"] . "\");\n";
                }

                echo "if (webindic) { webindic.style.display = ";

                if (WebIndicator::is_on($key)) {
                    echo "''";	
                } else {
                	echo "'none'";
                }
                echo "; }\n";  
            }
        }

        echo "</script>\n";
    }

    function set_on($name)
    {
        $_SESSION["_WebIndicator_" . $name] = true;
    }
    
    function set_off($name)
    {
        unset($_SESSION["_WebIndicator_" . $name]);
    }
    
    function is_on($name)
    {
        return isset($_SESSION["_WebIndicator_" . $name]);
    }
    
    

    function WebIndicator()
    {
    	
        // Define arrays of Web indicator
        // Each array can contain one or more sub-arrays that define uniq html element
        // Each sub-array contain the id of html element to show/hide 
        // and an optional frame name for this id
        // Switching on/off a web indicator switch on/off each of its html element

/*                      
        $this->indicators["Reload_policies"] = array (
            array (
                "id" => "ReloadPolicy1",
                ),
            array (
                "frame" => "top",
                "id" => "ReloadPolicy2",
                ),
            array (
                "frame" => "top",
                "id" => "ReloadPolicy3",
                ),
            );
*/

            
        $this->indicators["Reload_policies"] = array (
            array (
                "frame" => "top",
                "id" => "Reload_policies",
                ),
            );

        $this->indicators["Reload_nets"] = array (
            array (
                "frame" => "top",
                "id" => "Reload_nets",
                ),
            );

        $this->indicators["Reload_hosts"] = array (
            array (
                "frame" => "top",
                "id" => "Reload_hosts",
                ),
            );

        $this->indicators["Reload_sensors"] = array (
            array (
                "frame" => "top",
                "id" => "Reload_sensors",
                ),
            );

        $this->indicators["Reload_plugins"] = array (
            array (
                "frame" => "top",
                "id" => "Reload_plugins",
                ),
            );

        $this->indicators["Reload_directives"] = array (
            array (
                "frame" => "top",
                "id" => "Reload_directives",
                ),
            );

        $this->indicators["Reload_servers"] = array (
            array (
                "frame" => "top",
                "id" => "Reload_servers",
                ),
            );

        $this->indicators["ReloadPolicy"] = array (
            array (
                "frame" => "top",
                "id" => "ReloadPolicy",
                ),
            );
    }

}

$OssimWebIndicator = new WebIndicator();

?>
