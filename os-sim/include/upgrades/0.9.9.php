<?php
require_once 'classes/Upgrade_base.inc';

class upgrade_099 extends upgrade_base
{
    function end_upgrade()
    {
        //
        // Reload ACLS
        //
        $this->reload_acls();
        return true;
    }
}
?>
