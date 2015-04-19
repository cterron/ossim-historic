<?php

$results_per_page = $this->results_per_page;

$host = GET('host');
$date_to = GET('date_to');
$date_from = GET('date_from');
$page_from = GET('page_from') ? GET('page_from') : 0;
$total_rows = GET('total_rows');
$display_by = GET('display_by') ?  GET('display_by') : 'day';
$group_id = GET('group_id');

ossim_valid($host, OSS_IP_ADDR, OSS_NULLABLE, 'illegal:'._("IP"));
ossim_valid($date_from, OSS_DIGIT, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("start date"));
ossim_valid($date_to, OSS_DIGIT, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("end date"));
ossim_valid($page_from, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("page from"));
ossim_valid($total_rows, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("num results"));
ossim_valid($display_by, OSS_LETTER, OSS_SCORE, OSS_NULLABLE, 'illegal:'._("display by"));
ossim_valid($total_rows, OSS_DIGIT, OSS_NULLABLE, 'illegal:'._("group id"));

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
    return false;
}

</script>
  
<body>
<form method="GET">
<table align="center" width="70%">
    <tr><th colspan="2"><?=_("Filter")?></th></tr>
    <tr>
        <th style="text-align: right" width="25%"><?=_("Host")?>: </th>
        <td style="text-align: left">
        <input type="text" name="host" value="<?=$host?>" size="15"/>
        </td>
    </tr><tr>
        <th style="text-align: right"><?=_("Date (YY-MM-DD)")?>: </th>
        <td style="text-align: left; border-width: 0px">
            <?=_('from')?> <input type="text" size=10 name="date_from" value="<?=$date_from?>">&nbsp;
              <? Util::draw_js_calendar(array('input_name' => 'document.forms[0].date_from', true))?>
            <?=_('to')?> <input type="text" size="10" name="date_to" value="<?=$date_to?>">&nbsp;
              <? Util::draw_js_calendar(array('input_name' => 'document.forms[0].date_to', true))?>&nbsp;
  <!--          
  <select name="quick_date">
    <option value="value1"></option>
    <option value="value2">option2</option>
    <option value="value3">option3</option>
  </select>
  -->
        </td>
    </tr><tr>
        <th style="text-align: right"><?=_("Display by")?>: </th>
        <td style="text-align: left; border-width: 0px">
            <input type="radio" name="display_by" value="day" <? if ($display_by == 'day') echo "checked" ?>/> <?=_("Date")?>
            <input type="radio" name="display_by" value="sid_name" <? if ($display_by == 'sid_name') echo "checked" ?>/> <?=_("Type")?>
            <input type="radio" name="display_by" value="ip_src" <? if ($display_by == 'ip_src') echo "checked" ?>/> <?=_("Source IP")?>
            <input type="radio" name="display_by" value="ip_dst" <? if ($display_by == 'ip_dst') echo "checked" ?>/> <?=_("Dest. IP")?>
        </td>
    </tr>
    <tr><th colspan="2"><input type="submit" value="<?=_("Go")?>"></th>
</table>
</form>
<hr>
<?

$options = array(
    'host' => $host,
    'date_from' => $date_from,
    'date_to' => $date_to,
    'results_per_page' => $results_per_page,
    'from' => $page_from,
    'total_rows' => $total_rows
);

$list = $this->get_list($options);

$link = $_SERVER['PHP_SELF'] . "?host=$host&date_to=$date_to&date_from=$date_from".
        "&total_rows={$this->total_rows}&display_by=$display_by&group_id=$group_id";

echo Util::pager_draw($page_from, $results_per_page, $this->total_rows, $link);

?>

<table width="95%">
<tr>
<?
// Table header
$num_cols = count($this->table);
for ($col=1; $col<=$num_cols; $col++) {
    $width = isset($this->table[$col]['width']) ? 'width="'.$this->table[$col]['width'].'%"' : '';
    echo "<th $width>".$this->table[$col]['label']."</th>\n";
}
?>
</tr>
<?
$list = $this->display_by($display_by, $list);
$i = 1;
foreach (array_keys($list) as $k) {
    // Display Groups
    $expand = '<a href="#" onClick="return toggle_events('.$i.','.count($list[$k]).')">+</a>';
    echo "<tr><th colspan='$num_cols' style='text-align: left; background-color: #EDE9E3'>$expand <b>$k (".count($list[$k]).")</b></th></tr>\n";
    // Display columns
    foreach ($list[$k] as $data) {
        $event_id = "event-".$i;
        echo "\n<tr id='$event_id' style='display: none'>\n";
        
        for ($col=1; $col<=$num_cols; $col++) {
            // Expand "button"
            if ($col == 1) {
                $detail_id = "detail-".$i++;
                $expand = '<a href="#" onClick="Element.toggle(\''.$detail_id.'\'); return false;">+</a> ';
            } else {
                $expand = '';
            }
            $align = isset($this->table[$col]['align']) ? $this->table[$col]['align'] : 'left';
            $wrap = isset($this->table[$col]['wrap']) && $this->table[$col]['wrap'] == false ? 'nowrap' : '';
            echo "<td style='text-align: $align' $width $wrap>$expand".$this->parse_col_tpl($data, $col)."</td>\n";
        }
        echo "</tr>\n";
        // Display payload
        echo '<tr id="'.$detail_id.'" style="display: none">'.
             "<td colspan='$num_cols'>".$this->draw_event_info($data).'</td></tr>';
    }
}
?>
</table>
<? echo Util::pager_draw($page_from, $results_per_page, $this->total_rows, $link); ?>

