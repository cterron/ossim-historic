<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuTools", "ToolsBackup");

require_once 'classes/Util.inc';
require_once 'ossim_db.inc';
require_once ('classes/Backup.inc');

$conf = $GLOBALS["CONF"];
$data_dir = $conf->get_conf("data_dir");
$backup_dir = $conf->get_conf("backup_dir");

$isDisabled = Backup::running_restoredb();

$perform = $_POST["perform"];

if (!$isDisabled) {
	if ($perform == "insert") {
    		$insert = $_POST["insert"];
	        Backup::Insert($insert);
    }  elseif ($perform == "delete") {
	    	$delete = $_POST["delete"];
	    	Backup::Delete($delete);
	}
	unset($_POST["perform"]);
}

$db = new ossim_db();
$conn = $db->snort_connect();

$insert = Array();
$delete = Array();

$dir = dir($backup_dir);
while ($file = $dir->read()) {
   if($file == "." || $file == "..") {
   	continue;
   }
   if (is_dir($backup_dir.$file)) {
   	continue;
   }
   if (substr($file, 0, 7) != "insert-") {
		continue;
   }

	$date = Backup::str2date(substr($file, 7, 8));
	$query = OssimQuery("SELECT timestamp FROM acid_event WHERE timestamp >
    '$date 00:00:00' AND timestamp < '$date 23:59:59' LIMIT 1");
	if (!$rs = $conn->Execute($query)) {
  		print 'error: '.$conn->ErrorMsg().'<BR>';
  		exit;
  	}
  	if ($rs->EOF) {
		$insert[] = $date;
	} else {
		$delete[] = $date;
	}
}

$dir->close();
$db->close($conn);
?>
<html>
	<head>
		<title>Backup</title>
 		<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  		<meta http-equiv="Pragma" content="no-cache">
  		<link rel="stylesheet" type="text/css" href="../style/style.css"/>
  		<script language="javascript">
  			function boton (form, act) {
  				form.perform.value = act;
  				form.submit();
  			}
  		</script>
  	</head>
  	<body>
  		<center>
  		<form name="backup" action="<?php echo $_SERVER["PHP_SELF"]?>" method="post">
  	  	<table>
  			<tr>
  				<th colspan="3"><?php echo gettext("Backup Manager"); ?></th>
			</tr>
  			<tr>
  				<th><?php echo gettext("Dates to Restore"); ?></th>
  				<th></th>
  				<th><?php echo gettext("Dates in Database"); ?></th>
  			</tr>
  			<tr>
  				<td>
  					<select name="insert[]" size="10" multiple>
<?php if (count($insert)) {
for ($i=0; $i<count($insert); $i++) { ?>
						<option value=<?=$insert[$i]?>>&nbsp;&nbsp;<?=$insert[$i]?>&nbsp;&nbsp;</option>
<?php } 
} else { ?>
						<option size="100" disabled>&nbsp;&nbsp;--&nbsp;<?php echo gettext("NONE"); ?>&nbsp;--&nbsp;&nbsp;</option>
<?php } ?>
  					</select>
  				</td>
				<td></td>
				<td>
					<select name="delete[]" size="10" multiple>
<?php if (count($delete)) {
for ($i=0; $i<count($delete); $i++) { ?>
						<option size="100" value=<?=$delete[$i]?>>&nbsp;&nbsp;<?=$delete[$i]?>&nbsp;&nbsp;</option>
<?php } 
} else { ?>
						<option size="100" disabled>&nbsp;&nbsp;--&nbsp;<?php echo gettext("NONE"); ?>&nbsp;--&nbsp;&nbsp;</option>
<?php } ?>
					</select>
				</td>
  			</tr>
  			<tr>
  				<td>
  					<button name="insertB" value="insertDo" type="submit" onclick="boton(this.form, 'insert')" <?= ($isDisabled) ? "disabled" : "" ?> ><?php echo gettext("Insert"); ?></button>
  				</td>
  				<td></td>
  				<td>
  					<button name="deleteB" value="deleteDo" type="submit" onclick="boton(this.form, 'delete')"  <?= ($isDisabled) ? "disabled" : "" ?> ><?php echo gettext("Delete"); ?></button>
  				</td>
  			</tr>
  		</table>
  		<input type="hidden" name="perform" value="">
  		</form>
  		<br>
		<table aling="center">
			<tr>
				<th colspan="5"><?php echo gettext("Backup Events"); ?></th>
			</tr>
			<tr>
				<th><?php echo gettext("User"); ?></th>
				<th><?php echo gettext("Date"); ?></th>
				<th><?php echo gettext("Action"); ?></th>
				<th><?php echo gettext("Status"); ?></th>
				<th><?php echo gettext("Percent"); ?></th>
			</tr>
<?php
$db1 = new ossim_db();
$conn1 = $db1->connect();
$query = OssimQuery("SELECT * FROM restoredb_log ORDER BY id DESC LIMIT 10");
if (!$rs1 = $conn1->Execute($query)) {
	print 'error: '.$conn1->ErrorMsg().'<BR>';
	exit;
}
while (!$rs1->EOF) {	
?>
			<tr>
				<td><?= $rs1->fields["users"] ?></td>
				<td><?= Util::timestamp2date($rs1->fields["date"]) ?></td>
				<td><?= $rs1->fields["data"] ?></td>
	<?php if ($rs1->fields["status"] == 1) { ?>
				<td><font color="orange"><b><?php echo gettext("Running"); ?></b></font></td>
	<?php } else { ?>
				<td><font color="green"><b><?php echo gettext("Done"); ?></b></font></td>
	<?php } ?>
				<td><?= $rs1->fields["percent"] ?></td>
			</tr>
<?php 
	$rs1->MoveNext();
}
$db1->close($conn1);
?>
		</table>
		</center>
  	</body>
</html>
