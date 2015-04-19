<?php
require_once 'classes/Session.inc';
require_once 'classes/Security.inc';
Session::logcheck("MenuIncidents", "Osvdb");
require_once 'classes/Osvdb.inc';
require_once 'ossim_db.inc';

$db = new ossim_db();
$conn = $db->osvdb_connect();

$osvdb_id = GET("id");
$osvdb = Osvdb::get_osvdb($conn, $osvdb_id);
?>

<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
<table align="center" width="100%">
	<tr>
		<th><center><h2><?=$osvdb->get_title()?></h2></center></th>
	</tr>
</table>

<br>
<b>OSVDB ID:</b> <?=$osvdb->get_id()?>
<br><br>
<b>Disclosure Date:</b> <?=$osvdb->get_disclosure_date()?>
<br><br>
<b>Description:</b><br>
<?=$osvdb->get_description()?>
<br><br>
<b>Technical Description:</b><br>
<?=$osvdb->get_technical_description()?>
<br><br>
<?php
	$classifications_list = $osvdb->get_classifications();
	if (count($classifications_list)>0) {
		echo "<b>Vulnerability Classification:</b><br><ul>";
			foreach ($classifications_list as $classification) {
				echo "<li>" . $classification . "</li>";
			}
		echo "</ul><br>";
	}
?>
<b>Products:</b>
<br>
<ul>
        <?php
                $products_list = $osvdb->get_products();
                foreach ($products_list as $product) {
                        echo "<li>" . $product . "</li>";
                }
        ?>
</ul>
<br>
<b>Solution:</b><br>
<?=$osvdb->get_solution()?>
<br><br>
<b>Manual Testing Notes:</b><br>
<?=$osvdb->get_manual_test()?>
<br><br>
<b>External References:</b>
<br>
<ul>
        <?php
                $external_refs_list = $osvdb->get_external_refs();
                foreach ($external_refs_list as $external_ref) {
                        echo "<li>" . $external_ref . "</li>";
                }
        ?>
</ul>
<br>
</body>
</html>
