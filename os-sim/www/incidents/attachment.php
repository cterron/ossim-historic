<?php
require_once 'classes/Session.inc';
Session::logcheck("MenuIncidents", "IncidentsIncidents");

require_once ('ossim_db.inc');
require_once ('classes/Incident_file.inc');
require_once ('classes/Incident_file.inc');
require_once ('classes/Security.inc');

$id = POST('id');
    
ossim_valid($id, OSS_NULLABLE, OSS_ALPHA, OSS_SCORE, 'illegal:'._("id"));
    
if (ossim_error()) {
    die(ossim_error());
}

if (!empty($id))
{
    $db = new ossim_db();
    $conn = $db->connect();
    if ($files = Incident_file::get_list($conn, "WHERE id = $id"))
    {
        $type = $files[0]->get_type();
        $fname = $files[0]->get_name();
    
        header("Content-type: $type");
        header('Content-Disposition: attachment; filename="'.$fname.'"');
        print $files[0]->get_content();
    }

    $db->close($conn);
}

?>


