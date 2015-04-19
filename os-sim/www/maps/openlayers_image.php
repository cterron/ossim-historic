<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once 'classes/User_config.inc';
Session::logcheck("MenuConfiguration", "ConfigurationMaps");

// error == 4, means no user file selected for uploading
if (isset($_FILES['file']) && $_FILES['file']['error'] != 4) {
    if (is_uploaded_file($_FILES['file']['tmp_name'])) {
        // store the file in the DB
        $db = new ossim_db();
        $conn = $db->connect();
        $config = new User_config($conn);
        $login = Session::get_session_user();
        $config->set($login, 'maps_tmp_image', file_get_contents($_FILES['file']['tmp_name']));
        /*
         * Array
            (
                [0] => 987
                [1] => 1303
                [2] => 2
                [3] => width="987" height="1303"
                [bits] => 8
                [channels] => 3
                [mime] => image/jpeg
            )
         */
        $info = getimagesize($_FILES['file']['tmp_name']);
        
        $config->set($login, 'maps_tmp_image_width', $info[0]);
        $config->set($login, 'maps_tmp_image_height', $info[1]);
        $config->set($login, 'maps_tmp_image_type', $info['mime']); 
        
        header("Location: openlayers.php?layer=image"); exit;
    } else {
        echo ossim_error("An error occurred uploading the file, error code was: ".$_FILES['file']['error'].
                         ".<br>Check <a href='http://es2.php.net/manual/en/features.file-upload.errors.php'>here</a> for more info");
    }
}
?>
<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
<FORM ENCTYPE="multipart/form-data" ACTION="<?=$_SERVER['PHP_SELF']?>" METHOD=POST>
<table align="center" width="60%">
<tr>
<td style="border-width: 0px"><?=_("Choose image")?></td><td style="border-width: 0px"><input type="file" name="file"></td>
</tr>
</table><br>
<center><input type="submit" name="submit" value="<?=_("Send file")?>"></center>
</FORM>
</body></html>