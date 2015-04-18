<?php


if ($id = $_GET["id"])
{

    require_once ('ossim_db.inc');
    require_once ('classes/Incident_file.inc');

    $db = new ossim_db();
    $conn = $db->connect();
    if ($files = Incident_file::get_list($conn, "WHERE id = $id"))
    {

        $type = $files[0]->get_type();
        $content = $files[0]->get_content();
    
        header("Content-type: $type");
        print $content;
    }

    $db->close($conn);
}

?>


