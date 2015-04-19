<?php

class Plugin_reference {

    var $plugin_id;
    var $plugin_sid;
    var $reference_id;
    var $reference_sid;

    function Plugin_reference ($plugin_id, $plugin_sid, $reference_id,
                               $reference_sid)
    {
        $this->plugin_id        = $plugin_id;
        $this->plugin_sid       = $plugin_sid;
        $this->reference_id     = $reference_id;
        $this->reference_sid    = $reference_sid;
    }

    function get_plugin_id()        { return $this->plugin_id;      }
    function get_plugin_sid()       { return $this->plugin_sid;     }
    function get_reference_id()     { return $this->reference_id;   }
    function get_reference_sid()    { return $this->reference_sid;  }

    function get_count($conn) 
    {
        $query = "SELECT count(*) FROM plugin_reference;";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            return $rs->fields["count(*)"];
        }
    }

    function get_list($conn, $args = "", $inf = 0, $sup = 25) 
    {
        $i = 0;

        $query = "SELECT * FROM plugin_reference $args";
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
            while (!$rs->EOF) {
                if (($i++ >= $inf) && ($inf++ <= $sup)) {
                    $list[] = new Plugin_reference ($rs->fields["plugin_id"], 
                                                    $rs->fields["plugin_sid"],
                                                    $rs->fields["reference_id"],
                                                    $rs->fields["reference_sid"]);
                } elseif ($inf > $sup) {
                    break;
                }
                $rs->MoveNext();
            }
        }
        return $list;
    }
}


?>
