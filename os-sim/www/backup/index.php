<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuTools", "ToolsBackup");

$user = Session::get_session_user();

require_once 'ossim_db.inc';

function str2date($str) {
	list($year, $month, $day) = sscanf($str, "%4s%2s%2s");
	return "$year-$month-$day";
}
function str2timestamp($str) {
	list($year, $month, $day, $hour, $min, $sec) = sscanf($str, "%4s%2s%2s%2s%2s%2s");
	return "$year-$month-$day $hour:$min:$sec";
}

function array2str ($arr) {
	$str = "";
    if(is_array($arr)){
	while (list($key, $value) = each ($arr)) {
		if ($str == "")
   		$str = $value;
   	else
   		$str .= "," . $value;
   }}
   return $str;
}

$conf = new ossim_conf();
$data_dir = $conf->get_conf("data_dir");
$backup_dir = $conf->get_conf("backup_dir");

if (file_exists ("/tmp/ossim-restoredb.pid")) {
	$isDisabled = 1;
} else {
	$isDisabled = 0;
}

$perform = $_POST["perform"];

if (!$isDisabled) {
	if ($perform == "insert") {
		$insert = $_POST["insert"];
		$files = array2str ($insert);
		
		if ($files != "") {
            $files = escapeshellcmd($files);
            $user = escapeshellcmd($user);
			$res = popen("$data_dir/scripts/restoredb.pl insert $files $user &", "r");
			$isDisabled = 1;
		}
	} elseif ($perform == "delete") {
		$delete = $_POST["delete"];
		$files = array2str ($delete);

		if ($files != "") {
            $files = escapeshellcmd($files);
            $user = escapeshellcmd($user);
			$res = popen("$data_dir/scripts/restoredb.pl delete $files $user &", "r");
			$isDisabled = 1;
		}
	}
	sleep(3);
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

	$date = str2date(substr($file, 7, 8));
	$query = "SELECT timestamp FROM acid_event WHERE timestamp > '$date 00:00:00' AND timestamp < '$date 23:59:59' LIMIT 1";
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
  				<th colspan="3">Backup Manager</th>
			</tr>
  			<tr>
  				<th>Dates to Restore</th>
  				<th></th>
  				<th>Dates in Database</th>
  			</tr>
  			<tr>
  				<td>
  					<select name="insert[]" size="10" multiple>
<?php if (count($insert)) {
for ($i=0; $i<count($insert); $i++) { ?>
						<option value=<?=$insert[$i]?>>&nbsp;&nbsp;<?=$insert[$i]?>&nbsp;&nbsp;</option>
<?php } 
} else { ?>
						<option size="100" disabled>&nbsp;&nbsp;--&nbsp;NONE&nbsp;--&nbsp;&nbsp;</option>
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
						<option size="100" disabled>&nbsp;&nbsp;--&nbsp;NONE&nbsp;--&nbsp;&nbsp;</option>
<?php } ?>
					</select>
				</td>
  			</tr>
  			<tr>
  				<td>
  					<button name="insertB" value="insertDo" type="submit" onclick="boton(this.form, 'insert')" <?= ($isDisabled) ? "disabled" : "" ?> >Insert</button>
  				</td>
  				<td></td>
  				<td>
  					<button name="deleteB" value="deleteDo" type="submit" onclick="boton(this.form, 'delete')"  <?= ($isDisabled) ? "disabled" : "" ?> >Delete</button>
  				</td>
  			</tr>
  		</table>
  		<input type="hidden" name="perform" value="">
  		</form>
  		<br>
		<table aling="center">
			<tr>
				<th colspan="5">Backup Events</th>
			</tr>
			<tr>
				<th>User</th>
				<th>Date</th>
				<th>Action</th>
				<th>Status</th>
				<th>Percent</th>
			</tr>
<?php
$db1 = new ossim_db();
$conn1 = $db1->connect();
$query = "SELECT * FROM restoredb_log ORDER BY id DESC LIMIT 10";
if (!$rs1 = $conn1->Execute($query)) {
	print 'error: '.$conn1->ErrorMsg().'<BR>';
	exit;
}
while (!$rs1->EOF) {	
?>
			<tr>
				<td><?= $rs1->fields["users"] ?></td>
				<td><?= str2timestamp($rs1->fields["date"]) ?></td>
				<td><?= $rs1->fields["data"] ?></td>
	<?php if ($rs1->fields["status"] == 1) { ?>
				<td><font color="orange"><b>Running</b></font></td>
	<?php } else { ?>
				<td><font color="green"><b>Done</b></font></td>
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
