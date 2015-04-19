<?php

class Plugin_Custom_cloud extends Panel
{
    var $defaults = array(
        'cloud_db' => 'ossim',
	'cloud_table' => 'alarm',
	'cloud_column' => 'inet_ntoa(src_ip)',
	'cloud_link' => 'http://localhost/ossim/report/menu.php?host=_TAG_&section=metrics',
	'cloud_num' => 10,
	'cloud_tag_max_len' => 0,
	'cloud_resolv_ip' => 0
    );
    
    function getCategoryName()
    {
        return _("Custom Tag-Cloud");
    }
    
    function showSubCategoryHTML()
    {
        $html = '';
        $check_ossim = $check_snort = '';
        if ($this->get('cloud_db') == 'snort') {
            $check_snort = 'checked';
        } else {
            $check_ossim = 'checked';
        }

        $resolv_yes = $resolv_no = '';
        if ($this->get('cloud_resolv_ip') == '1') {
            $resolv_yes= 'checked';
        } else {
            $resolv_no= 'checked';
        }

        $html .= _("Database") . ':<br/>
            <input type="radio" name="cloud_db" value="ossim" '.$check_ossim.'>Ossim<br/>
            <input type="radio" name="cloud_db" value="snort" '.$check_snort.'>Snort
            <br/>
            <hr noshade>
        ';
        $html .= _("Cloud table");
        $html .= ': <input type ="text" name="cloud_table" value ="'.$this->get('cloud_table').'"><br/>';
        $html .= _("Cloud column");
        $html .= ': <input type ="text" name="cloud_column" value ="'.$this->get('cloud_column').'"><br/>';
        $html .= _("Cloud max num");
        $html .= ': <input type ="text" name="cloud_num" value ="'.$this->get('cloud_num').'"><br/>';
        $html .= _("Cloud link. Use _TAG_ for placeholder");
        $html .= ': <input type ="text" name="cloud_link" size="30" value ="'.$this->get('cloud_link').'"><br/>';
        $html .= _("Cloud tag max length, 0 means unlimited");
        $html .= ': <input type ="text" name="cloud_tag_max_len" value ="'.$this->get('cloud_tag_max_len').'"><br/>';
        $html .= "<hr noshade>";
        $html .= _("Resolve hostname on column?"). ':<br/>
            <input type="radio" name="cloud_resolv_ip" value="1" '.$resolv_yes.'>'._("Yes").'<br/>
            <input type="radio" name="cloud_resolv_ip" value="0" '.$resolv_no.'>'._("No").'
            <br/>
        ';

        return $html;
    }
    
    function showSettingsHTML()
    {
        return _("No extra settings needed for this category");
    }

    function showWindowContents()
    {
        require_once 'ossim_db.inc';

$table = $this->get('cloud_table');
$column = $this->get('cloud_column');
$cloud_num = intval($this->get('cloud_num'));
$dbname = $this->get('cloud_db');
$link = $this->get('cloud_link');
$max_len = $this->get('cloud_tag_max_len');
$resolv_hostname = $this->get('cloud_resolv_ip');

ossim_valid($table, OSS_DIGIT, OSS_LETTER, OSS_SCORE, 'illegal:' . _("table"));
ossim_valid($column, OSS_DIGIT, OSS_LETTER, OSS_SCORE, "\(\)", 'illegal:' . _("column"));

if (ossim_error()) {
    die(ossim_error());
}

$method = $dbname == 'snort' ? 'snort_connect' : 'connect';
$db = new ossim_db;
$conn = $db->$method();

// Extract top ocurrences
$sql = "SELECT $column AS tag, count(*) as quantity from $table group by $column order by quantity desc limit $cloud_num";

if (!$rs = $conn->Execute($sql)) {
echo "Error was: ".$conn->ErrorMsg()."\n\nQuery was: ".$sql;
exit();
}

if($resolv_hostname){
     require_once("classes/Host.inc");
}

while (!$rs->EOF) {
if($resolv_hostname){
$tag_names[$rs->fields[0]] = Host::ip2hostname($conn, $rs->fields[0], $is_sensor = false, $force_no_dns = true);
}
$tags[$rs->fields[0]] = $rs->fields[1];
$rs->MoveNext();
}

// reorder array alphabetically / numerically
ksort($tags);

$db->close($conn);

    // Default font sizes
    $min_font_size = 8;
    $max_font_size = 35;

    $minimum_count = min(array_values($tags));
    $maximum_count = max(array_values($tags));
    $spread = $maximum_count - $minimum_count;

    if($spread == 0) {
        $spread = 1;
    }

    $cloud_html = '';
    $cloud_tags = array(); // create an array to hold tag code
    foreach ($tags as $tag => $count) {
        $local_link = str_replace("_TAG_", $tag, $link);
        $local_name = $tag;
        if($resolv_hostname) $local_name = $tag_names[$tag];
        if($max_len > 0) $tag = substr($tag, 0, $max_len);
        $size = $min_font_size + ($count - $minimum_count) 
            * ($max_font_size - $min_font_size) / $spread;
        $cloud_tags[] = '<a style="font-size: '. floor($size) . 'px' 
            . '" class="tag_cloud" href="' . htmlspecialchars($local_link)
            . '" title="\'' . $tag . '\' returned a count of ' . $count . '">' 
            . htmlspecialchars(stripslashes($local_name)) . '</a>';
    }
    $cloud_html = join("\n", $cloud_tags) . "\n";
    return $cloud_html;

    }

}
    
?>
