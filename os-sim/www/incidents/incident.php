<?php
require_once 'classes/Session.inc';
require_once 'classes/Security.inc';
Session::logcheck("MenuIncidents", "IncidentsIncidents");
require_once 'ossim_db.inc';
require_once 'classes/Incident.inc';
require_once 'classes/Incident_ticket.inc';
require_once 'classes/Incident_tag.inc';


$id = GET('id');

ossim_valid($id, OSS_ALPHA, 'illegal:'._("Incident ID"));

if (ossim_error()) {
    die(ossim_error());
}

$db = new ossim_db();
$conn = $db->connect();

$incident_list = Incident::search($conn, array('incident_id' => $id));
if (count($incident_list) != 1) {
    die("Invalid incident ID or insufficient permission on incident");
}
$incident = $incident_list[0];
?>
<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
<style>
td {
    border-width: 0px;
}

</style>
</head>
<body>
<table align="center" width="100%">
  <tr>
    <th> <?= _("Ticket") ?> </th>
    <th width="550px"><?= _("Incident") ?></th>
    <th> <?= _("In Charge") ?> </th>
    <th> <?= _("Status") ?> </th>
    <th> <?= _("Priority") ?> </th>
    <th> <?= _("Action") ?> </th>
  </tr>
  <tr>
<?php

function format_user($user, $html = true, $show_email = false)
{
    if (is_a($user, 'Session')) {
        $login = $user->get_login();
        $name  = $user->get_name();
        $depto = $user->get_department();
        $company = $user->get_company();
        $mail  = $user->get_email();
    } elseif (is_array($user)) {
        $login = $user['login'];
        $name  = $user['name'];
        $depto = $user['department'];
        $company = $user['company'];
        $mail  = $user['email'];
    } else {
        return '';
    }
    $ret = $name;
    if ($depto && $company) $ret .= " / $depto / $company";
    if ($mail && $show_email) $ret = "$ret &lt;$mail&gt;"; 
    if ($login) $ret = "<label title=\"Login: $login\">$ret</label>";
    if ($mail) {
        $ret = '<a href="mailto:'.$mail.'">'.$ret.'</a>';
    } else {
        $ret = "$ret <font size=small color=red><i>(No email)</i></font>";
    }

    return $html ? $ret : strip_tags($ret);
}

    $name  = $incident->get_ticket();
    $title = $incident->get_title();
    $ref   = $incident->get_ref();
    $type  = $incident->get_type();
    $created = $incident->get_date();
    $life  = $incident->get_life_time();
    $updated = $incident->get_last_modification();
    $priority = $incident->get_priority();
    $incident_status    = $incident->get_status();
    $incident_in_charge = $incident->get_in_charge();
    $users = Session::get_list($conn);
    
    $incident_tags  = $incident->get_tags();
    $incident_tag  = new Incident_tag($conn);
    $taga = array();
    foreach ($incident_tags as $tag_id) {
        $taga[] = $incident_tag->get_html_tag($tag_id); 
    }
    $taghtm = count($taga) ? implode(' - ', $taga) : _("n/a");
?>

    <td><b><?=$name?></b></td>
    <td class="left">
        Name: <b><?=$title?> </b><br/>
        Class: <?=$ref?><br/>
        Type: <?=$type?><br/>
        Created: <?=$created?> (<?=$life?>)<br/>
        Last Update: <?=$updated?><br/>
        <hr/>
        Extra: <?=$taghtm?><br/>
        <hr/>
    <?php
        if ($ref == 'Alarm' or $ref == 'Event') {
            if ($ref == 'Alarm') {
                $alarm_list = $incident->get_alarms($conn);
            } else {
                $alarm_list = $incident->get_events($conn);
            }
            foreach ($alarm_list as $alarm_data) {
                echo 
                    "Source Ips: <b>" . 
                        $alarm_data->get_src_ips() . "</b> - " .
                    "Source Ports: <b>" . 
                        $alarm_data->get_src_ports() . "</b><br/>" .                    
                    "Dest Ips: <b>" . 
                        $alarm_data->get_dst_ips() . "</b> - " .
                    "Dest Ports: <b>" .
                        $alarm_data->get_dst_ports() . "</b>";
            }
        } elseif ($ref == 'Metric') {
            $metric_list = $incident->get_metrics($conn);
            foreach ($metric_list as $metric_data) {
                echo 
                    "Target: <b>" .
                        $metric_data->get_target() . "</b> - " .
                    "Metric Type: <b>" . 
                        $metric_data->get_metric_type() . "</b> - " .
                    "Metric Value: <b>" . 
                        $metric_data->get_metric_value() . "</b>";
            }
        } elseif ($ref == 'Anomaly') {
            $anom_list = $incident->get_anomalies($conn);
            foreach ($anom_list as $anom_data) {
                $anom_type = $anom_data->get_anom_type();
                $anom_ip = $anom_data->get_ip();        
                $anom_info_o = $anom_data->get_data_orig();        
                $anom_info   = $anom_data->get_data_new();
                if ($anom_type == 'mac') {
                    list($a_sen, $a_date_o, $a_mac_o, $a_vend_o) = explode(",", $anom_info_o);
                    list($a_sen, $a_date, $a_mac, $a_vend) = explode(",", $anom_info);
                    echo
                        "Host: <b>" . $anom_ip . "</b><br>" .
                        "Previous Mac: <b>". $a_mac_o . "(" . $a_vend_o . ")</b><br>" .
                        "New Mac: <b>". $a_mac . "(" . $a_vend . ")</b><br>";
                } elseif ($anom_type == 'service') {
                    list($a_sen, $a_date, $a_port, $a_prot_o, $a_ver_o) = explode(",", $anom_info_o);
                    list($a_sen, $a_date, $a_port, $a_prot, $a_ver) = explode(",", $anom_info);
                    echo
                        "Host: <b>" . $anom_ip . "</b><br>" .
                        "Port: <b>" . $a_port . "</b><br>" .
                        "Previous Protocol [Version]: <b>". $a_prot_o . " [" . $a_ver_o . "]</b><br>" .
                        "New Protocol [Version]: <b>". $a_prot . " [" . $a_ver . "]</b><br>";
                                  
                } elseif ($anom_type == 'os') {
                    list($a_sen, $a_date, $a_os_o) = explode(",", $anom_info_o);
                    list($a_sen, $a_date, $a_os) = explode(",", $anom_info);
                    echo
                        "Host: <b>" . $anom_ip . "</b><br>" .
                        "Previous OS: <b>". $a_os_o . "</b><br>" .
                        "New OS: <b>". $a_os . "</b><br>";
               
                }
                
            }
        }
    ?>
    </td>
    <!-- end incident data -->

    <td><?= $incident->get_in_charge_name($conn) ?></td>
    <td><? Incident::colorize_status($incident->get_status($conn)) ?></td>
    <td><?= Incident::get_priority_in_html($priority) ?></td>

    <td>
        <form action="#" method="get">
        <input type="button" name="submit_edit" value="<?=_("Edit")?>"
               style="width: 10em;"
               onClick="document.location = 'newincident.php?action=edit&ref=<?=$ref?>&incident_id=<?=$id?>';"
               /><br/>
        
        <input type="button" name="add_ticket" value="<?=_("New ticket")?>"
               style="width: 10em;" onclick="document.location = '#anchor';"/><br/>
           
        <input type="button" name="submit_delete" value="<?=_("Delete")?>"
               style="width: 10em; color: red;"
               onClick="document.location = 'manageincident.php?action=delincident&incident_id=<?=$id?>';"
               />
        </form>
    </td>
  </tr>
  <tr>
    <td colspan="6" style="text-align: left;"><hr/><b><?=_("Email changes to:")?></b><br/>
    <form action="manageincident.php?action=subscrip&incident_id=<?=$id?>" method="POST">
        <table width="100%" style="border-width: 0px;">
        <tr><td>&nbsp;</td>
        <td width="45%" style="text-align: left;">
        <?
        foreach ($incident->get_subscribed_users($conn, $id) as $u) {
            echo format_user($u, true, true) . '<br/>';
        }
        ?>
        </td><td style="text-align: right;" NOWRAP>
          <select name="login">
            <option value=""></option>
            <? foreach ($users as $u) { ?>
                <option value="<?=$u->get_login()?>"><?=format_user($u, false)?></option>
            <? } ?>
          </select>
          <input type="submit" name="subscribe" value="Subscribe">&nbsp;
          <input type="submit" name="unsubscribe" value="Unsubscribe">
        </td></tr></table>
    </form>
    </td>
  </tr>
</table>

<!-- end incident summary -->

<br>
<!-- incident ticket list-->
<?php
$tickets_list = $incident->get_tickets($conn);
for ($i = 0; $i < count($tickets_list); $i++) {
    $ticket = $tickets_list[$i];
    $ticket_id = $ticket->get_id();
    $date = $ticket->get_date();
    $life_time = Util::date_diff($date, $created);

    // Resolve users
    // XXX improve performance
    $creator = $ticket->get_user();
    $in_charge = $ticket->get_in_charge();
    $transferred = $ticket->get_transferred();    
    $creator = Session::get_list($conn, "WHERE login='$creator'");
    $creator = count($creator) == 1 ? $creator[0] : false;
    $in_charge = Session::get_list($conn, "WHERE login='$in_charge'");
    $in_charge = count($in_charge) == 1 ? $in_charge[0] : false;
    $transferred = Session::get_list($conn, "WHERE login='$transferred'");
    $transferred = count($transferred) == 1 ? $transferred[0] : false;

    $descrip = $ticket->get_description();
    $action  = $ticket->get_action();
    $status = $ticket->get_status();
    $prio = $ticket->get_priority();
    $prio_str = Incident::get_priority_string($prio);
    $prio_box = Incident::get_priority_in_html($prio);
    if ($attach = $ticket->get_attachment($conn)) {
        $file_id   = $attach->get_id();
        $file_name = $attach->get_name();
        $file_type = $attach->get_type();
    }
    
?>
    <table width="95%" border=1 cellspacing="0" align="center">
    <!-- ticket head -->
    <tr><td widht="90%" class="ticket" style="background: #ABB7C7;" nowrap>
        <b><?=format_user($creator)?></b> - <?=$date?>
        </td>
        <td style="background: #ABB7C7;">
            <?
            //
            // Allow the ticket creator and the admin delete the last ticket
            //
            if (($i == count($tickets_list) - 1) &&
                (Session::am_i_admin() ||
                 $creator == Session::get_session_user())
               )
            {
            ?>
            <input type="button" name="deleteticket"
                   value="<?=_("Delete Ticket")?>"
                   onclick="javascript: document.location = 'manageincident.php?action=delticket&ticket_id=<?=$ticket_id?>&incident_id=<?=$id?>'"
            >
            <? } ?>
        &nbsp;
        </td>
    </tr>
    <!-- end ticket head -->
    <tr>
        <!-- ticket contents -->
        <td style="width: 600px" valign="top">
            <table style="border-width: 0px;"><tr><td class="ticket_body" >
                
                <? if ($attach) { ?>
                    <b><?=_("Attachment")?>: </b>
                    <a href="attachment.php?id=<?=$file_id?>"><?=htm($file_name)?></a>
                    &nbsp;<i>(<?=$file_type?>)</i><br/>
                <? } ?>
                <b><?=_("Description")?></b><p class="ticket_body"><?=htm($descrip)?></p>
                <? if ($action) { ?>
                    <b><?=_("Action")?></b><p class="ticket_body"><?=htm($action)?></p>
                <? } ?>
            </td></tr></table>
        </td>
        <!-- end ticket contents -->
        <!-- ticket summary -->
        <td class="ticket" style="border-top-width: 0px; width: 230px" valign="top">
            <table style="border-width: 0px;">
            <tr><td class="ticket">
                <b><?=_("Status")?>: </b><?Incident::colorize_status($status)?>
            </td></tr>
            <tr valign="middle"><td>
                <table cellspacing="0" style="border-width: 0px;"><tr>
                    <td class="ticket"><b><?=_("Priority");?>: </b>
                    <td class="ticket"> <?=$prio_box?></td>
                    <td class="ticket"> - <?=$prio_str?></td>
                </tr></table>
            </td></tr>
            <? if (!$transferred) { ?>
            <tr><td class="ticket">
                <b><?=_("In charge")?>: </b><?=format_user($in_charge)?>
            </td></tr>
            <? } else { ?>
            <tr><td class="ticket">
                <b><?=_("Transferred To")?>: </b><?=format_user($transferred)?>
            </td></tr>
            <? } ?>
            <tr><td class="ticket" NOWRAP>
                <b><?=_("Since Creation")?>: </b><?=$life_time?>
            </td></tr>
            </table>
        </td>
        <!-- end ticket summary -->
    </table>
<? } ?>
<!-- end incident ticket list-->
<br>

<!-- form for new ticket -->
<script language="JavaScript" type="text/javascript">
    function chg_prio_str()
    {
        prio_num = document.newticket.priority;
        index = prio_num.selectedIndex;
        prio = prio_num.options[index].value;
        if (prio > 7) {
            document.newticket.prio_str.selectedIndex = 2;
        } else if (prio > 4) {
            document.newticket.prio_str.selectedIndex = 1;
        } else {
            document.newticket.prio_str.selectedIndex = 0;
        }
    }
    
    function chg_prio_num()
    {
        prio_str = document.newticket.prio_str;
        index = prio_str.selectedIndex;
        prio = prio_str.options[index].value;
        if (prio == 'High') {
            document.newticket.priority.selectedIndex = 7;
        } else if (prio == 'Medium') {
            document.newticket.priority.selectedIndex = 4;
        } else {
            document.newticket.priority.selectedIndex = 2;
        }
    }
        
</script>
  
<form name="newticket" method="POST"
      action="manageincident.php?action=newticket&incident_id=<?=$id?>"
      ENCTYPE="multipart/form-data">
<table align="center" width="1%" style="border-width: 0px" cellspacing="5">
<tr><td valign="top">

    <table style="text-align: left" id="anchor" align="left" width="1%" style="border-width: 1px">
    <tr>
        <th><?=_("Status")?></th>
        <td style="text-align: left">
          <select name="status">
            <option value="Open" <? if ($incident_status == 'Open') echo 'SELECTED'?>><?=_("Open")?></option>
            <option value="Closed" <? if ($incident_status == 'Closed') echo 'SELECTED'?>><?=_("Closed")?></option>
          </select>
        </td>
    </tr>
    <tr>
        <th><?=_("Priority")?></th>
        <td style="text-align: left">
          <select name="priority" onChange="chg_prio_str();">
            <? for ($i=1; $i<=10; $i++) { ?>
                <? $selected = $priority == $i ? 'SELECTED' : ''; ?>
                <option value="<?=$i?>" <?=$selected?>><?=$i?></option>
            <? } ?>
          </select>
          -&gt;
          <select name="prio_str" onChange="chg_prio_num();">
            <option value="Low"><?=_("Low")?></option>
            <option value="Medium"><?=_("Medium")?></option>
            <option value="High"><?=_("High")?></option>
         </td>
    </tr>
    <tr>
        <th><?=_("Transfer To")?></th>
        <td style="text-align: left">
          <select name="transferred">
            <option value=""></option>
            <? foreach ($users as $u) { ?>
                <? if ($u->get_login() == $incident_in_charge) continue; // Don't add current in charge?>
                <option value="<?=$u->get_login()?>"><?=format_user($u, false)?></option>
            <? } ?>
          </select>
        </td>
        <script>chg_prio_str();</script>
    </tr>
    <tr>
        <th><?=_("Attachment")?></th>
        <td style="text-align: left"><input type="file" name="attachment" /></td>
    </tr>
    <tr>
        <th ><?=_("Description")?></th>
        <td style="border-width: 0px;">
        <textarea name="description" rows="10" cols="80" WRAP=HARD></textarea>
    </td></tr>
    <tr>
        <th><?=_("Action")?></th>
        <td style="border-width: 0px;">
        <textarea name="action" rows="10" cols="80" WRAP=HARD></textarea>
    </td></tr>
    <tr>
        <td>&nbsp;</td>
        <td align="center" style="text-align: center">
        <input type="submit" name="add_ticket" value="<?=_("Add ticket")?>"/>
    </td></tr>
    </table>

</td>
<td valign="top">
<table style="text-align: left">
    <tr><th><?=_("Tags")?></th></tr>
    <? foreach ($incident_tag->get_list() as $t) { ?>
    <tr>
        <td style="text-align: left" NOWRAP>
            <? $checked = in_array($t['id'], $incident_tags) ? 'checked' : ''?>
            <input type="checkbox" name="tags[]" value="<?=$t['id']?>" <?=$checked?>>
            <label title="<?=$t['descr']?>"><?=$t['name']?></label><br/>
        </td>
    </tr>
    <? } ?>
</table>
</td>
</tr>
</table>
</form>
</body></html>
