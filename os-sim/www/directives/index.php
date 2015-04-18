<?php
    $XML_FILE = '/etc/ossim/server/directives.xml';
?>

<html>
<head>
  <title> Directives Editor </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
</head>

<body>

  <h1 align="center">Directives Editor</h1>

<?php

require_once ('classes/Plugin.inc');
require_once ('classes/Plugin_sid.inc');
require_once ('ossim_db.inc');

$db = new ossim_db();
$conn = $db->connect();

function directives_table($dom, $directive_id)
{
?>
    <!-- main table: directives -->
    <table align="center">
      <tr><th colspan="2">Directives</th></tr>
      <tr>
        <th>Id</th>
        <th>Name</th>
      </tr>
<?php
    foreach ($dom->get_elements_by_tagname('directive') as $directive) {
        $id   = $directive->get_attribute('id');
        $name = $directive->get_attribute('name');
?>
      <tr>
        <td><?php echo $id ?></td>
        <td><a 
<?php if (!strcmp($id, $directive_id)) echo "class=\"selected\""; ?>
            href="<?php 
            echo $_SERVER["PHP_SELF"] ?>?directive=<?php 
            echo $id ?>"><?php echo $name ?></a></td>
      </tr>
<?php
    }
?>
    </table>
    <br/>
    <!-- end main table: directives -->
<?php
}



function rule_table_header($directive_id)
{
?>
    <!-- rule table -->
    <table align="center">
      <tr><th colspan="12">Rules (Directive <?php echo $directive_id ?>)</th></tr>
      <tr>
        <td></td>
        <th>Name</th>
        <th>Priority</th>
        <th>Reliability</th>
        <th>Time_out</th>
        <th>Occurrence</th>
        <th>From</th>
        <th>To</th>
        <th>Port_from</th>
        <th>Port_to</th>
        <th>Plugin ID</th>
        <th>Plugin SID</th>
      </tr>
<?php
}


function rule_table_foot() {
?>
    </table>
    <br/>
    <!-- end main table: directives -->
<?php
}


function rule_table($dom, $directive_id, $directive, $level, $ilevel)
{
    global $conn;

    if($directive->has_child_nodes()) {
        $rules = $directive->child_nodes();

        $branch = 0;
        foreach($rules as $rule) {
            if (($rule->type == XML_ELEMENT_NODE) && 
                ($rule->tagname() == 'rule'))
            {
?>
    <?php if ($level == 2) { ?>
      <tr bgcolor="#CCCCCC">
    <?php } elseif ($level == 3) { ?>
      <tr bgcolor="#999999">
    <?php } elseif ($level == 4) { ?>
      <tr bgcolor="#9999CC">
    <?php } elseif ($level == 5) { ?>
      <tr bgcolor="#6699CC">
    <?php } ?>
      
        <!-- expand -->
        <td class="left">
    <? if (($level == 1) && ($rule->has_child_nodes())) {
    ?>
            <a href="<?php echo $_server["php_self"] ?>?directive=<?php 
                echo $directive_id?>&level=<?php echo $ilevel + 1?>"><?php 
                echo "+" ?></a>
    <? } elseif ($rule->has_child_nodes()) { ?>
            <a href="<?php echo $_server["php_self"] ?>?directive=<?php 
                echo $directive_id?>&level=<?php echo $ilevel-$level+1?>"><?php 
                echo '-' ?></a>
    <? } ?>
        </td>
        <!-- end expand -->
        
        <td><?php echo $rule->get_attribute('name'); ?></td>
        <td><?php echo $rule->get_attribute('priority'); ?></td>
        <td><?php echo $rule->get_attribute('reliability'); ?></td>
        <td><?php echo $rule->get_attribute('time_out'); ?></td>
        <td><?php echo $rule->get_attribute('occurrence'); ?></td>
        <td><?php echo $rule->get_attribute('from'); ?></td>
        <td><?php echo $rule->get_attribute('to'); ?></td>
        <td><?php echo $rule->get_attribute('port_from'); ?></td>
        <td><?php echo $rule->get_attribute('port_to'); ?></td>
        <td>
<?php 
    $plugin_id = $rule->get_attribute('plugin_id'); 
    if ($plugin_list = Plugin::get_list($conn, "WHERE id = $plugin_id")) {
        $name = $plugin_list[0]->get_name();
        echo "<a href=\"../conf/pluginsid.php?id=$plugin_id&" . 
                "name=$name\">$name</a> ($plugin_id)";
    }
?>
        </td>
        <td>
<?php 
    $plugin_sid = $rule->get_attribute('plugin_sid'); 
    foreach (split(',', $plugin_sid) as $sid) {

        /* sid == ANY */
        if (!strcmp($sid, "ANY")) {
            echo "ANY";
        } 
        
        /* get name of plugin_sid */
        elseif ($plugin_list = Plugin_sid::get_list
                ($conn, "WHERE plugin_id = $plugin_id AND sid = $sid")) {
            $name = $plugin_list[0]->get_name();
            echo "<a title=\"$name\">$sid</a>&nbsp; ";
        }
    }
?>
        </td>
      </tr>
                
<?php
                if ($level > 1) {
                    if ($rule->has_child_nodes()) {
                        $rules = $rule->child_nodes();
                        foreach ($rules as $rule) {
                            rule_table($dom, $directive_id, $rule, 
                                       $level - 1, $ilevel);
                        }
                    } 
                }
                $branch++;
            }
        } /* foreach */
    }
}


    /* create dom object from a XML file */
    if(!$dom = domxml_open_file($XML_FILE)) {
        echo "Error while parsing the document\n";
        exit;
    }

    $directive_id = $_GET["directive"];
    directives_table($dom, $directive_id);

    if ($directive_id) {
    
        $doc = $dom->document_element();
        $doc = $doc->child_nodes();
        $directive = $doc[$directive_id * 2 -1];

        if (!$level = $_GET["level"])   $level = 1;
        $_SESSION["path"] = 0;

        rule_table_header($directive_id);
        rule_table($dom, $directive_id, $directive, $level, $level);
        rule_table_foot();
    }

$db->close($conn);

?>

</body>
</html>


