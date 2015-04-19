<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once 'ossim_db.inc';
require_once 'classes/Incident.inc';
require_once 'classes/Incident_tag.inc';
require_once 'classes/Incident_type.inc';
    
Session::logcheck("MenuIncidents", "IncidentsIncidents");

function order_img($subject)
{
    global $order_by, $order_mode;
    if ($order_by != $subject) return '';
    $img = $order_mode == 'DESC' ? 'abajo.gif' : 'arriba.gif';
    return '<img src="../pixmaps/top/'.$img.'" border=0>';
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

  <h1><?=_("Incidents")?></h1>

<?php

    $vars = array('order_by'   => OSS_LETTER . OSS_SCORE,
                  'order_mode' => OSS_LETTER,
                  'ref'        => OSS_LETTER,
                  'type'       => OSS_ALPHA . OSS_SPACE,
                  'title'      => OSS_ALPHA . OSS_SCORE . OSS_PUNC,
                  'related_to_user' => OSS_LETTER,
                  'with_text'  => OSS_ALPHA . OSS_PUNC . OSS_SCORE . OSS_SPACE,
                  'action'     => OSS_ALPHA . OSS_PUNC . OSS_SCORE . OSS_SPACE,
                  'attachment' => OSS_ALPHA . OSS_SPACE . OSS_PUNC,
                  'advanced_search' => OSS_DIGIT,
                  'priority'   => OSS_LETTER,
                  'in_charge'  => OSS_ALPHA . OSS_SCORE . OSS_PUNC . OSS_SPACE,
                  'status'     => OSS_LETTER,
                  'tag'        => OSS_DIGIT);
    
    foreach ($vars as $var => $validate) {
        $$var = GET("$var");
        if (!ossim_valid($$var, array($validate, OSS_NULLABLE))) {
            echo "Var '$var' not valid<br>";
            die(ossim_error());
        }
    }
    
    
    if (!$order_by) {
        $order_by = 'life_time';
        $order_mode = 'ASC';
    }

    // First time we visit this page, show by default only Open incidents
    // when GET() returns NULL, means that the param is not set
    if (GET('status') === null) $status = 'Open';

    $db = new ossim_db();
    $conn = $db->connect();

    $criteria = array(
            'ref' => $ref,
            'type' => $type,
            'title' => $title,
            'in_charge' => $in_charge,
            'with_text' => $with_text,
            'status' => $status,
            'priority_str' => $priority,
            'attach_name' => $attachment,
            'related_to_user' => $related_to_user,
            'tag' => $tag
        );
    $incident_tag = new Incident_tag($conn);
?>

  <!-- filter -->
  <form name="filter" method="GET" action="<?= $_SERVER["PHP_SELF"] ?>">
    <? if ($advanced_search) { ?>
        <input type="hidden" name="advanced_search" 
               value="1">
    <? } ?>
  <table align="center" width="100%">
    <tr>
      <th colspan="7">
<?php
        echo _("Filter");
        $change_to = _("change to ");
        if ($advanced_search) {
            $label  = _("Advanced");
            $change_to .= ' '._("Simple");
            echo " $label [<a href=\"" . 
                $_SERVER["PHP_SELF"] ."\"
                title=\"$change_to $\">$change_to</a>]";
        } else {
            $label  = _("Simple");
            $change_to .= ' '._("Advanced");            
            echo " $label [<a href=\"" . 
                $_SERVER["PHP_SELF"] ."?advanced_search=1\"
                title=\"$change_to $\">$change_to</a>]";
        }
?>
      </th>
    </tr>
    <tr><td colspan="7" style="border-width: 0px;">
      <table width="100%" align="center" style="border-width: 0px;">
          <td> <?php echo gettext("Class");  /* ref */  ?> </td>
          <td> <?php echo gettext("Type"); /* type */ ?> </td>
          <td> <?php echo gettext("Search text in all fields"); ?> </td>
          <td> <?php echo gettext("In charge"); ?> </td>
          <td> <?php echo gettext("Status"); ?> </td>
          <td> <?php echo gettext("Priority"); ?> </td>
          <td> <?php echo gettext("Action"); ?> </td>
        </tr>
        <tr>
          <td style="border-width: 0px;">
            <select name="ref" onChange="document.forms['filter'].submit()">
              <option value="">
                <?php echo gettext("ALL"); ?>
              </option>
              <option <? if ($ref == "Alarm") echo "selected" ?> value="Alarm">
    	        <?php echo gettext("Alarm"); ?>
              </option>
              <option <? if ($ref == "Event") echo "selected" ?> value="Event">
    	        <?php echo gettext("Event"); ?>
              </option>
              <option <? if ($ref == "Metric") echo "selected" ?> value="Metric">
    	        <?php echo gettext("Metric"); ?>
              </option>
              <option <? if ($ref == "Anomaly") echo "selected" ?> value="Anomaly">
    	        <?php echo gettext("Anomaly"); ?>
              </option>
              <option <? if ($ref == "Vulnerability") echo "selected" ?> value="Vulnerability">
                <?php echo gettext("Vulnerability"); ?>
              </option>
            </select>
          </td>
          <td style="border-width: 0px;">
            <select name="type" onChange="document.forms['filter'].submit()">
              <option value="" <? if (!$type) echo "selected" ?>>
                <?php echo gettext("ALL"); ?>
              </option>
              <? foreach (Incident_type::get_list($conn) as $itype) {
                  $id = $itype->get_id();
              ?>
                  <option <? if ($type == $id) echo "selected" ?> value="<?=$id?>">
                    <?= $id ?>
                  </option>
              <? } ?>
            </select>
          </td>
          <td style="border-width: 0px;">
            <input type="text" name="with_text" value="<?= $with_text ?>" /></td>
          <td style="border-width: 0px;">
            <input type="text" name="in_charge" value="<?= $in_charge ?>" /></td>
          <td style="border-width: 0px;">
            <select name="status" onChange="document.forms['filter'].submit()">
              <option value="">
                <?php echo gettext("ALL"); ?>
              </option>
              <option <? if ($status == "Open") echo "selected" ?>
                value="Open">
    	        <?php echo gettext("Open"); ?>
              </option>
              <option <? if ($status == "Closed") echo "selected" ?> value="Closed">
    	        <?php echo gettext("Closed"); ?>
              </option>
            </select>
          </td>
          <td style="border-width: 0px;">
            <select name="priority" onChange="document.forms['filter'].submit()">
              <option value="">
    	        <?php echo gettext("ALL"); ?>
              </option>
              <option <? if ($priority == "High") echo "selected" ?> value="High">
    	        <?php echo gettext("High"); ?>
              </option>
              <option <? if ($priority == "Medium") echo "selected" ?> value="Medium">
    	        <?php echo gettext("Medium"); ?>
              </option>
              <option <? if ($priority == "Low") echo "selected"  ?> value="Low">
    	        <?php echo gettext("Low"); ?>
              </option>
            </select>
          </td>
          <td nowrap style="border-width: 0px;">
            <input type="submit" name="filter" value="OK" />
          </td>
        </tr>
      </tr>
      </table>
    </td></tr>
<?php
    if ($advanced_search) {
?>
    <tr><td colspan="7" style="border-width: 0px;">
      <table width="100%" align="center" style="border-width: 0px;">
      <tr>
          <td><?=_("with User")?></td>
          <td><?=_("with Title")?></td>
          <td><?=_("with Attachment Name")?></td>
          <td><?=_("with Tag")?></td>
      </tr><tr>
          <td style="border-width: 0px;">
            <input type="text" name="related_to_user" value="<?=$related_to_user?>" /></td>
          <td style="border-width: 0px;">
            <input type="text" name="title" value="<?=$title?>" /></td>
          <td style="border-width: 0px;">
            <input type="text" name="attachment" value="<?=$attachment?>" /></td>
          <td style="border-width: 0px;">
          <select name="tag">
                <option value=""></option>
            <? foreach ($incident_tag->get_list() as $t) { ?>
                <? $selected = $tag == $t['id'] ? 'SELECTED' : ''; ?>
                <option value="<?=$t['id']?>" <?=$selected?>><?=$t['name']?></option>
            <? } ?>
          </select>
          </td>
      </tr>
      </table>
    </td></tr>
<?php
    }
?>
  </table>
  </form>
  <br/>
  <!-- end filter -->

  <table align="center" width="100%">
<?php
    $incident_list = Incident::search($conn, $criteria, $order_by, $order_mode);
    if (count($incident_list))
    {
        $filter = '';
        foreach ($criteria as $key => $value) {
            $filter .= "&$key=".urlencode($value);
        }
        
        if ($advanced_search) {
            $filter .= "&advanced_search=" . urlencode($advanced_search);
        }
        // Next time reverse the order of the column
        // XXX it reverses the order of all columns, should only
        //     reverse the order of the column previously sorted
        if ($order_mode) {
            $order_mode = $order_mode == 'DESC' ? 'ASC' : 'DESC';
            $filter .= "&order_mode=$order_mode";
        }
?>

    <tr>
      <th NOWRAP><a href="?order_by=id<?=$filter?>"><?=_("Ticket").order_img('id')?></a></th>
      <th NOWRAP><a href="?order_by=title<?=$filter?>"><?=_("Title").order_img('title')?></a></th>
      <th NOWRAP><a href="?order_by=priority<?=$filter?>"><?=_("Priority").order_img('priority')?></a></th>
      <th NOWRAP><a href="?order_by=date<?=$filter?>"><?=_("Created").order_img('date')?></a></th>
      <th NOWRAP><a href="?order_by=life_time<?=$filter?>"><?=_("Life Time").order_img('life_time')?></a></th>
      <th><?=_("In charge") ?></th>
      <th><?=_("Submitter") ?></th>
      <th><?=_("Type") ?></th>
      <th><?=_("Status") ?></th>
      <th><?=_("Extra") ?></th>
    </tr>

<?php
        $row = 0;
        foreach ($incident_list as $incident) 
        {
?>

    <tr <? if ($row++ % 2) echo 'bgcolor="#EFEFEF"'; ?> valign="center">
      <td>
        <a href="incident.php?id=<?= $incident->get_id() ?>">
        <?= $incident->get_ticket(); ?></a>
      </td>
      <td><b>
        <a href="incident.php?id=<?= $incident->get_id() ?>">

            <?= $incident->get_title(); ?></a></b>
<?php
if($incident->get_ref() == "Vulnerability"){
$vulnerability_list = $incident->get_vulnerabilities($conn);
// Only use first index, there shouldn't be more
if(!empty($vulnerability_list)){
echo " <font color=\"grey\" size=\"1\">(" . $vulnerability_list[0]->get_ip() . ":" . $vulnerability_list[0]->get_port() . ")</font>";
}
}
?>
      </td>
      <?php 
        $priority = $incident->get_priority();
      ?>
      <td><?=Incident::get_priority_in_html($priority)?></td>
      <td NOWRAP><?= $incident->get_date() ?></td>
      <td NOWRAP><?= $incident->get_life_time() ?></td>
      <td><?= $incident->get_in_charge_name($conn) ?></td>
      <td><?= $incident->get_submitter() ?></td>
      <td><?= $incident->get_type() ?></td>
      <td><? Incident::colorize_status($incident->get_status()) ?></td>
      <td>
        <?
        $rows = 0;
        foreach ($incident->get_tags() as $tag_id) {
            echo "<font color=\"grey\" size=\"1\">" . $incident_tag->get_html_tag($tag_id) . "</font><br/>\n";
            $rows++;
        }
        if (!$rows) echo "&nbsp;";
        ?>
      </td>
    </tr>

<?php
        } /* foreach */
    } /* incident_list */
    else {
        echo "<p align=\"center\">".gettext("No incidents")."</p>";
    }

    $db->close($conn);
?>
    <tr>
      <td colspan="8" align="center">

        <?php echo gettext("Insert new Incident"); ?> (
        <a href="newincident.php?ref=Alarm&title=New+Alarm+incident&priority=1&src_ips=&src_ports=&dst_ips=&dst_ports=">
	<?php echo gettext("Alarm"); ?> </a> | 
        <?php echo gettext("Anomaly"); ?> [ 
    <a  href="newincident.php?ref=Anomaly&title=New+Mac+Anomaly+Incident&priority=1&anom_type=mac">
	<?php echo gettext("Mac"); ?></a> ,
     <a href="newincident.php?ref=Anomaly&title=New+OS+Anomaly+Incident&priority=1&anom_type=os">
	<?php echo gettext("OS"); ?></a> ,
     <a href="newincident.php?ref=Anomaly&title=New+Service+Anomaly+Incident&priority=1&anom_type=service">
	<?php echo gettext("Services"); ?></a> ] 
    | 
        <a href="newincident.php?ref=Event&title=New+Event+incident&priority=1&src_ips=&src_ports=&dst_ips=&dst_ports=">
	<?php echo gettext("Event"); ?> </a> | 
        <a href="newincident.php?ref=Metric&title=New+Metric+incident&priority=1&target=&metric_type=&metric_value=0">
	<?php echo gettext("Metric"); ?> </a> |
        <a href="newincident.php?ref=Vulnerability&title=New+Vulnerability+incident&priority=1&ip=&port=&nessus_id=&risk=&description=">
    <?php echo gettext("Vulnerability"); ?> </a> )

    
      </td>
    </tr>
  </table>

</body>
</html>

