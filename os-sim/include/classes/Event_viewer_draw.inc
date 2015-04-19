<?php
/*****************************************************************************
*
*    License:
*
*   Copyright (c) 2003-2006 ossim.net
*   Copyright (c) 2007-2009 AlienVault
*   All rights reserved.
*
*   This package is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; version 2 dated June, 1991.
*   You may not use, modify or distribute this program under any other version
*   of the GNU General Public License.
*
*   This package is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this package; if not, write to the Free Software
*   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
*   MA  02110-1301  USA
*
*
* On Debian GNU/Linux systems, the complete text of the GNU General
* Public License can be found in `/usr/share/common-licenses/GPL-2'.
*
* Otherwise you can read it here: http://www.gnu.org/licenses/gpl-2.0.txt
****************************************************************************/
/**
* Class and Function List:
* Function list:
* Classes list:
*/
$results_per_page = $this->results_per_page;
$host = GET('host');
$date_to = GET('date_to');
$date_from = GET('date_from');
$page_from = GET('page_from') ? GET('page_from') : 0;
$total_rows = GET('total_rows');
$display_by = GET('display_by') ? GET('display_by') : 'day';
$group_id = GET('group_id');
ossim_valid($host, OSS_IP_ADDR, OSS_NULLABLE, 'illegal:' . _("IP"));
ossim_valid($date_from, OSS_DIGIT, OSS_SCORE, OSS_NULLABLE, 'illegal:' . _("start date"));
ossim_valid($date_to, OSS_DIGIT, OSS_SCORE, OSS_NULLABLE, 'illegal:' . _("end date"));
ossim_valid($page_from, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("page from"));
ossim_valid($total_rows, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("num results"));
ossim_valid($display_by, OSS_LETTER, OSS_SCORE, OSS_NULLABLE, 'illegal:' . _("display by"));
ossim_valid($total_rows, OSS_DIGIT, OSS_NULLABLE, 'illegal:' . _("group id"));
if (ossim_error()) {
    die(ossim_error());
}
?>

<script language="JavaScript" type="text/javascript">

function toggle_events(from, num_results)
{
    var top = from + num_results - 1;
    for (i=from; i<=top; i++) {
        group_id='event-'+i;
        Element.toggle(group_id);
    }

    if (Element.getStyle('event-'+from, 'display') != 'none') {
        $('l-event-'+from).innerHTML = "-";
    } else {
        $('l-event-'+from).innerHTML = "+";

        // hide all detail block
        for (i=from; i<=top; i++) {
            $('detail-'+i).style.display = 'none';
            $('l-detail-'+i).innerHTML = "+";
        }
    }
    
    return false;
}

function toggle_details(detail_id)
{
    Element.toggle(detail_id);
    if (Element.getStyle(detail_id, 'display') != 'none')
        $('l-'+detail_id).innerHTML = "-";
    else
        $('l-'+detail_id).innerHTML = "+";
    return false;
}

</script>
  
<form method="GET">
<table align="center" width="70%">
    <tr><th colspan="2"><?php echo _("Filter") ?></th></tr>
    <tr>
        <th style="text-align: right" width="25%"><?php echo _("Host") ?>: </th>
        <td style="text-align: left">
        <input type="text" name="host" value="<?php echo $host ?>" size="15"/>
        </td>
    </tr><tr>
        <th style="text-align: right"><?php echo _("Date (YY-MM-DD)") ?>: </th>
        <td style="text-align: left; border-width: 0px">
            <?php echo _('from') ?> <input type="text" size=10 name="date_from" value="<?php echo $date_from ?>">&nbsp;
              <?php
Util::draw_js_calendar(array(
    'input_name' => 'document.forms[0].date_from',
    true
)) ?>
            <?php echo _('to') ?> <input type="text" size="10" name="date_to" value="<?php echo $date_to ?>">&nbsp;
              <?php
Util::draw_js_calendar(array(
    'input_name' => 'document.forms[0].date_to',
    true
)) ?>&nbsp;
  <!--          
  <select name="quick_date">
    <option value="value1"></option>
    <option value="value2">option2</option>
    <option value="value3">option3</option>
  </select>
  -->
        </td>
    </tr><tr>
        <th style="text-align: right"><?php echo _("Display by") ?>: </th>
        <td style="text-align: left; border-width: 0px">
            <input type="radio" name="display_by" value="day" <?php
if ($display_by == 'day') echo "checked " ?>/> <?php echo _("Date") ?>
            <input type="radio" name="display_by" value="sid_name" <?php
if ($display_by == 'sid_name') echo "checked " ?>/> <?php echo _("Type") ?>
            <input type="radio" name="display_by" value="ip_src" <?php
if ($display_by == 'ip_src') echo "checked " ?>/> <?php echo _("Source IP") ?>
            <input type="radio" name="display_by" value="ip_dst" <?php
if ($display_by == 'ip_dst') echo "checked " ?>/> <?php echo _("Dest. IP") ?>
        </td>
    </tr>
    <tr><th colspan="2"><input type="submit" value="<?php echo _("Go") ?>"></th>
</table>
</form>
<hr>
<?php
$options = array(
    'host' => $host,
    'date_from' => $date_from,
    'date_to' => $date_to,
    'results_per_page' => $results_per_page,
    'from' => $page_from,
    'total_rows' => $total_rows
);
$list = $this->get_list($options);
$link = $_SERVER['SCRIPT_NAME'] . "?host=$host&date_to=$date_to&date_from=$date_from" . "&total_rows={$this->total_rows}&display_by=$display_by&group_id=$group_id";
$pager = Util::pager_draw($page_from, $results_per_page, $this->total_rows, $link);
echo $pager;
?>

<table width="95%">
<tr>
<?php
// Table header
$num_cols = count($this->table);
for ($col = 1; $col <= $num_cols; $col++) {
    $width = isset($this->table[$col]['width']) ? 'width="' . $this->table[$col]['width'] . '%"' : '';
    echo "<th $width>" . $this->table[$col]['label'] . "</th>\n";
}
?>
</tr>
<?php
$list = $this->display_by($display_by, $list);
$i = 1;
foreach(array_keys($list) as $k) {
    // Display Groups
    $expand = '<a href="#" id="l-event-' . $i . '" onClick="return toggle_events(' . $i . ',' . count($list[$k]) . ')">+</a>';
    echo "<tr><th colspan='$num_cols' style='text-align: left; background-color: #EDE9E3'>$expand <b>$k (" . count($list[$k]) . ")</b></th></tr>\n";
    // Display columns
    foreach($list[$k] as $data) {
        $event_id = "event-" . $i;
        echo "\n<tr id='$event_id' style='display: none'>\n";
        for ($col = 1; $col <= $num_cols; $col++) {
            // Expand "button"
            if ($col == 1) {
                $detail_id = "detail-" . $i++;
                $expand = '<a href="#" id="l-' . $detail_id . '" onClick="return toggle_details(\'' . $detail_id . '\');">+</a> ';
            } else {
                $expand = '';
            }
            $align = isset($this->table[$col]['align']) ? $this->table[$col]['align'] : 'left';
            $wrap = isset($this->table[$col]['wrap']) && $this->table[$col]['wrap'] == false ? 'nowrap' : '';
            echo "<td style='text-align: $align' $width $wrap>$expand" . $this->parse_col_tpl($data, $col) . "</td>\n";
        }
        echo "</tr>\n";
        // Display payload
        echo '<tr id="' . $detail_id . '" style="display: none">' . "<td colspan='$num_cols'>" . $this->draw_event_info($data) . "</td></tr>\n";
    }
}
?>
</table>
<?php
echo "$pager\n"; ?>
