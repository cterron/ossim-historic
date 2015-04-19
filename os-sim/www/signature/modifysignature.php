<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuPolicy", "PolicySignatures");
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1> <?php echo gettext("Modify signature group"); ?> </h1>

<?php

require_once ("classes/Security.inc");

$insert = POST('insert');
$name   = POST('name');
$nsigs  = POST('nsigs');
$descr  = POST('descr');

ossim_valid($insert, OSS_ALPHA, OSS_NULLABLE, 'illegal:'._("insert"));
ossim_valid($name, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE, 'illegal:'._("name"));
ossim_valid($nsigs, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("nsigs"));
ossim_valid($descr, OSS_SPACE, OSS_PUNC, OSS_ALPHA, OSS_NULLABLE, OSS_AT, 'illegal:'._("descr"));

if (ossim_error()) {
    die(ossim_error());
}

if (POST('insert')) {

    require_once 'ossim_db.inc';
    require_once 'classes/Signature_group.inc';
    $db = new ossim_db();
    $conn = $db->connect();

    for ($i = 1; $i <= $nsigs; $i++) {
        $mboxname = "mbox" . $i;
        if (POST("$mboxname")) {
            ossim_valid(POST("$mboxname"), OSS_ALPHA, OSS_NULLABLE, OSS_PUNC, 'illegal:'._("mboxname"));
            if (ossim_error()) { die(ossim_error()); }
            $signature_list[] = POST("$mboxname");
        }
    }
   
    Signature_group::update ($conn, $name, $signature_list, $descr);

    $db->close($conn);
}
?>
    <p>Signature succesfully updated</p>
    <p><a href="signature.php"> <?php echo gettext("Back"); ?> </a></p>

</body>
</html>

