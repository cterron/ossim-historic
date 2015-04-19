<?php
/*
 * TODO:
 * - Add options for Window contents update frecuency
 * - Unify stuff used by both panel.php and window_panel.php
 * - Browser interoperatibility tests (currently only tested under Firefox) -
 * Better design
 */

require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once 'ossim_db.inc';
require_once 'panel/Ajax_Panel.php';
require_once 'classes/Util.inc';
Session::logcheck("MenuControlPanel", "ControlPanelExecutive");

$panel_id = GET('panel_id') ? GET('panel_id') : 1;

if (Session::menu_perms("MenuControlPanel", "ControlPanelExecutiveEdit")) {
    if(isset($_GET['edit']))
    {
        $show_edit = true;
        $_SESSION['ex_panel_can_edit']= $can_edit = GET('edit') ? true : false;
        $_SESSION['ex_panel_show_edit']=true;
    }else if(isset($_SESSION['ex_panel_can_edit'])&&isset($_SESSION['ex_panel_show_edit']))
    {
        $can_edit=$_SESSION['ex_panel_can_edit'];
        $show_edit=$_SESSION['ex_panel_show_edit'];
    }else
    {
        $can_edit = false;
        $show_edit = true;
    }
} else {
    $can_edit = $show_edit = false;
}

if (GET('edit_tabs') == 1) {
    $tabs = Window_Panel_Ajax::getPanelTabs();

    if (GET('submit')) {
        $tab_id = intval(GET('tab_id'));
        $tab_name = GET('tab_name');
        $tab_icon_url = GET('tab_icon_url');
        ossim_valid($tab_id, OSS_DIGIT, 'error: tab_id.');
        ossim_valid($tab_name, OSS_ALPHA, OSS_SCORE, OSS_SPACE, OSS_NULLABLE, 'error: Invalid name, alphanumeric, score, underscore and spaces allowed.');
        if (ossim_error()) {
            echo ossim_error();
        }

        if (is_array($tabs) && array_key_exists($tab_id, $tabs)) {
            unset($tabs[$tab_id]);
        } 

        if (GET('submit') != "delete") {
            if (!is_array($tabs)) {
                $tabs = array();
            }
            $tabs[$tab_id] = array(
                'tab_name' => $tab_name,
                'tab_icon_url' => htmlentities($tab_icon_url)
                 );
        }
        Window_Panel_Ajax::setPanelTabs($tabs);
    }
?>
<html>
<head>
<link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head><body>
<div style="text-align: right; width: 100%">[<a href="<?=$_SERVER['PHP_SELF']?>"><?=_("Return to executive panel"); ?></a>]</div>
<br>
<table align="center" width="90%">
<tr>
    <th><?=_("Tab Icon")?></th>
    <th><?=_("Tab Name")?></th>
    <th><?=_("Icon url")?></th>
</tr>
<?
    $last_tab_id = 0;
    if ($tabs != false) {
      foreach ($tabs as $tab_id => $tab_values) {
?>
<form action="<?=$_SERVER['PHP_SELF']?>" method="GET">
<tr><td>
<? if ($tabs[$tab_id]["tab_icon_url"]) { ?><img src="<?=$tabs[$tab_id]["tab_icon_url"]?>"><? } ?>&nbsp;
</td><td>
<input type="text" size="30" name="tab_name" value="<?= $tabs[$tab_id]["tab_name"]?>">
</td><td style="text-align: left">
<input type="text" size="50" name="tab_icon_url" value="<?= $tabs[$tab_id]["tab_icon_url"]?>">
<input type="submit" name="submit" value="update">&nbsp;<input type="submit" name="submit" value="delete">
<input type="hidden" name="edit_tabs" value="1">
<input type="hidden" name="panel_id" value="<?=$panel_id?>">
<input type="hidden" name="tab_id" value="<?= $tab_id ?>">
</form>
</tr>
<?php
    $last_tab_id = $tab_id;
      }
    }
?>
<form action="<?=$_SERVER['PHP_SELF']?>" method="GET">
<tr valign="middle"><td class="noborder">&nbsp;</td>
<td class="noborder">
<input type="text" size="30" name="tab_name" value="">
</td><td class="noborder" style="text-align: left">
<input type="text" size="50" name="tab_icon_url" value="">
<input type="hidden" name="tab_id" value="<?= $last_tab_id + 1;?>">
<input type="submit" name="submit" value="<?=_("insert new")?>">
<input type="hidden" name="edit_tabs" value="1">
<input type="hidden" name="panel_id" value="<?=$panel_id?>">
</form>
</tr></table><br>
* <i><?=_("You can choose only names, only icons or both")?></i>
</body></html>

<?php
   exit(); 
}

//
// Detect if that's an AJAX call
//
if (GET('interface') == 'ajax') {
    
    if (GET('ajax_method') == 'showWindowContents') {
        
        $ajax = &new Window_Panel_Ajax();
        $options = $ajax->loadConfig(GET('id'));
        $data['HELP_LABEL'] = _("help");

        if (count($options)) {
            // Add metric threshold indicator
            $indicator = "";
            if(isset($options['metric_opts']['enable_metrics']) && $options['metric_opts']['enable_metrics'] == 1 && isset($options['metric_opts']['metric_sql']) && strlen($options['metric_opts']['metric_sql']) > 0){

            $sql = $options['metric_opts']['metric_sql'];
            if (!preg_match('/^\s*\(?\s*SELECT\s/i', $sql) ||
             preg_match('/\sFOR\s+UPDATE/i', $sql) ||
             preg_match('/\sINTO\s+OUTFILE/i', $sql) ||
             preg_match('/\sLOCK\s+IN\s+SHARE\s+MODE/i', $sql))
            {
             die(_("SQL Query invalid due security reasons"));
            }
            $db = new ossim_db;
            $conn = $db->connect();
            if (!$rs = $conn->Execute($sql)) {
                echo "Error was: ".$conn->ErrorMsg()."\n\nQuery was: ".$sql;
                exit();
            }
            $metric_value = $rs->fields[0];
            $db->close($conn);
            $low_threshold = $options['metric_opts']['low_threshold'];
            $high_threshold = $options['metric_opts']['high_threshold'];

            // We need 5 states for the metrics: 
            /* 
                  * green
                -25 % 
                  * green-yellow
              - lower threshold
                  * green-yellow
                +25 %
                  * yellow
                -25 %
                  * yellow-red
              - upper threshold
                  * yellow-red
                +25 %
                  * red
            */

            $first_comp = $low_threshold - ($low_threshold / 4);
            $second_comp = $low_threshold + ($low_threshold / 4);
            $third_comp = $high_threshold - ($high_threshold / 4);
            $fourth_comp = $high_threshold + ($high_threshold / 4);
            
            if($metric_value <= $first_comp){
              $indicator = " <img src=\"../pixmaps/traffic_light1.gif\"/> ";
            } elseif($metric_value > $first_comp && $metric_value <= $second_comp){
              $indicator = " <img src=\"../pixmaps/traffic_light2.gif\"/> ";
            } elseif($metric_value > $second_comp && $metric_value <= $third_comp){
              $indicator = " <img src=\"../pixmaps/traffic_light3.gif\"/> ";
            } elseif($metric_value > $third_comp && $metric_value <= $fourth_comp){
              $indicator = " <img src=\"../pixmaps/traffic_light4.gif\"/> ";
            } elseif ($metric_value > $fourth_comp){
              $indicator = " <img src=\"../pixmaps/traffic_light5.gif\"/> ";
            } else {
              $indicator = " <img src=\"../pixmaps/traffic_light0.gif\"/> ";
            }


            }
            $data['CONTENTS'] = $ajax->showWindowContents($options);
            if (isset($options['window_opts']['title']))
                $data['TITLE'] = $options['window_opts']['title'] . $indicator;
            else
                $data['TITLE'] = "";
            if (isset($options['window_opts']['help']))
                $data['HELP_MSG'] =Util::string2js($options['window_opts']['help']);
            else
                $data['HELP_MSG'] = "";
        } else { // New window
            $data['CONTENTS'] = '';
            $data['TITLE']    = _("New window");
            $data['HELP_MSG'] = '';
        }
        if ($can_edit) {
            $data['CONFIG']   = '[<a href="window_panel.php?id='.GET('id').'&panel_id='.$panel_id.'" title="config">config</a>]';
        } else {
            $data['CONFIG'] = '';
        }
        $data['ID'] = GET('id');
        echo $ajax->parseTemplate('./window_tpl.htm', $data);
 
    } elseif (GET('ajax_method') == 'savePanelConfig' && $can_edit) {
        $ajax = &new Window_Panel_Ajax();
        $config['rows'] = GET('rows') ? GET('rows') : 3;
        $config['cols'] = GET('cols') ? GET('cols') : 2;
        $ajax->saveConfig('panel', $config);

    } elseif (GET('ajax_method') == 'moveWindow') {
        $ajax = &new Window_Panel_Ajax();
        $opts_from = $ajax->loadConfig(GET('from'));
        $opts_to   = $ajax->loadConfig(GET('to'));
        echo $ajax->saveConfig(GET('to'), $opts_from);
        echo $ajax->saveConfig(GET('from'), $opts_to);
        
    } else {
        echo "Not recognized AJAX method: '".GET('ajax_method')."'";
        printr($_GET);
    }
    exit;
// 
// Load Panel settings from config
//
} else {
    $ajax = &new Window_Panel_Ajax();
    $options = $ajax->loadConfig('panel');
    $rows = isset($options['rows']) ? $options['rows'] : 3;
    $cols = isset($options['cols']) ? $options['cols'] : 2;
}
?>
<html>
<head>
<script src="../js/prototype.js" type="text/javascript"></script>
<script src="../js/scriptaculous/scriptaculous.js" type="text/javascript"></script>
<script src="./panel.js" type="text/javascript"></script>
<script>
<!--
function wopen(url, name, w, h)
{
 // DK: Found this googling, thx :p
 // Fudge factors for window decoration space.
 // In my tests these work well on all platforms & browsers.
w += 32;
h += 96;
 var win = window.open(url,
  name, 
  'width=' + w + ', height=' + h + ', ' +
  'location=no, menubar=no, ' +
  'status=no, toolbar=no, scrollbars=yes, resizable=yes');
 win.resizeTo(w, h);
 win.focus();
}
// -->
</script>
<style type="text/css">
    body {
      background: white;
      color: black;
      font-family: tahoma,arial,verdana,helvetica,sans-serif;
      font-size:  8pt;
      margin: 1px;
      padding: 1px;
      margin-top: 1%;
      margin-left: 2%;
      margin-right: 2%;
      margin-bottom: 2%;
    }
  .panel-position {
    border: 0px solid #FFCFCF;
    margin: 1px;
    padding: 5px;
    /* background: #808080; */
    /* filter:alpha(opacity=50); -moz-opacity:.50; opacity:.50; */
    }
  .panel-active {
    background-color: #FFE59F;
    /* z-index: 1000; */
  }
  .placehere {
    position: relative;
    /* top: 5%; left: 5%; */
    border: 0px solid #bbb;
    margin: 0px; padding: 0px;
  }
  .loading {
      position: absolute;
      top: 1px;
      right: 1px;
      background-color: #AC0606;
      color: white;
  }
  .help {
      position: fixed;
      top: 5px;
      right: 5px;
      border: 1px;
      width: 300px;
      background-color: #F9F9F9;
      border: 1px dotted rgb(33,78,93);
      padding: 3px;
      z-index: 1001;
  }
.tag_cloud { padding: 3px; text-decoration: none; }
.tag_cloud:link  { color: #81d601; }
.tag_cloud:visited { color: #019c05; }
.tag_cloud:hover { color: #ffffff; background: #69da03; }
.tag_cloud:active { color: #ffffff; background: #ACFC65; }
</style>
  
</head>
<body>
<!-- Tabs if present -->
<?php
if (GET('fullscreen') != 1) {
    $tabs = Window_Panel_Ajax::getPanelTabs();
    $first = 1;
    if ($tabs) {
        print "<center>[ ";
        foreach ($tabs as $tab_id => $tab_name) {
            if (strlen($tabs[$tab_id]["tab_icon_url"]) > 0) {
                $image_string = '<img border="0" src="' . $tabs[$tab_id]["tab_icon_url"] . '">';
            } else {
                $image_string = "";
            }
            if ($first) {
                 $first = 0;
            } else {
                print " | ";
            }
?>
<a href="<?=$_SERVER['PHP_SELF'] ?>?panel_id=<?=$tab_id?>"><?= $tabs[$tab_id]["tab_name"] ?><?= $image_string; ?></a>
<?php
        }
        print "] </center>";
    } // foreach tab
?>
<!-- EDIT panel controls -->

<table width="100%" border=0 style="display: <? $can_edit || $show_edit ? 'inline' : 'none'?>; margin: 0px; padding: 0px;">
<tr><td align="left">
<? if ($can_edit) { ?>
<small>
    <?=_("Panel config")?>:
    <?=_("Geom")?>: <input id="rows" type="text" size="2" value="<?=$rows?>">x
    <input id="cols" type="text" size="2" value="<?=$cols?>">
    <a href="#" onClick="javascript:
        panel_save($('rows').value, $('cols').value);
        panel_load($('rows').value, $('cols').value);
        "><?=_("Apply")?></a>
</small>
<? 
} 
?>
</td><td align="right"><small>

<? if ($show_edit && !$can_edit ) { ?>
[<a href="<?=$_SERVER['PHP_SELF']?>?edit=1&panel_id=<?= $panel_id?>"><?php echo gettext("Edit"); ?></a>]
[<a href="<?=$_SERVER['PHP_SELF']?>?edit_tabs=1&panel_id=<?= $panel_id?>"><?php echo gettext("Edit Tabs"); ?></a>]
<? } elseif ($show_edit && $can_edit) { ?>
[<a href="<?=$_SERVER['PHP_SELF']?>?edit=0&panel_id=<?= $panel_id?>"><?php echo gettext("No Edit"); ?></a>]
[<a href="<?=$_SERVER['PHP_SELF']?>?edit_tabs=1&panel_id=<?= $panel_id?>"><?php echo gettext("Edit Tabs"); ?></a>]
<? } ?>
(<a href="<?=$_SERVER['PHP_SELF'] ?>?fullscreen=1&panel_id=<?=$panel_id?>" target="popup" onClick="wopen('<?=$_SERVER['PHP_SELF'] ?>?fullscreen=1&panel_id=<?=$panel_id?>', 'popup', 800, 600); return false;"><?=("Fullscreen")?></a>)
</small>
</td></tr>
</table>

<?// if not fullscreen
} else { 
// if in fullscreen mode show a big tab name and icon
    $tabs = Window_Panel_Ajax::getPanelTabs();
    if(strlen($tabs[$panel_id]["tab_icon_url"]) > 0){
        $image_string = "<img src=\"" . $tabs[$panel_id]["tab_icon_url"] . "\">";
    } else {
        $image_string = "";
    }
    print "<center><h2> [" .  $tabs[$panel_id]["tab_name"] . $image_string .  "] </h2></center>";
}
?>

<!-- displays saveConfig errors -->
<div id="container" style="margin: 0px; padding: 0px"></div>
<div id="loading" class="loading"></div>
<div id="help" class="help"></div>
<script>Element.hide('help');</script>

<div id="placehere">
Ossim Panel Loading...
</div>

<!-- do nothing, Ajax.Updater in ajax_load() needs an element -->
<div id="null" style="display: none"></div>

<script>

var myResponders = {
    onCreate: function() {
        Element.show('loading');
        $('loading').innerHTML = '<?php echo gettext("Loading"); ?>..';
    }
}
Ajax.Responders.register(myResponders);
var AjaxRequestCounter = 0;

function ajax_load(id)
{
    ajax_url = '<?=$_SERVER['PHP_SELF']?>?interface=ajax&panel_id=<?=$panel_id?>&ajax_method=showWindowContents&id='+id;
    AjaxRequestCounter++;
    new Ajax.Updater (
        'null',  // Element to refresh
        ajax_url, // URL
        {          // options
            method: 'get',
            asynchronous: true,
            parameters: '<?= $can_edit ? 'edit=1' : '' ?>',
            onComplete: function(req) {
                $('loading').innerHTML = '<?php echo gettext("Loading"); ?> ('+AjaxRequestCounter+' <?php echo gettext("pending"); ?>)';
                AjaxRequestCounter--;
                if (AjaxRequestCounter == 0) {
                    Element.hide('loading');
                }
                
                Control.Panel.setWindow(id, req.responseText);
            }
        }
    );
    return false;
}

function panel_save(rows, cols)
{
    ajax_url = '<?=$_SERVER['PHP_SELF']?>?interface=ajax&panel_id=<?=$panel_id?>&ajax_method=savePanelConfig';
    new Ajax.Updater (
        'container',  // Element to refresh
        ajax_url, // URL
        {          // options
            method: 'get',
            asynchronous: true,
            parameters: 'rows='+rows+'&cols='+cols+'<?= $can_edit ? '&edit=1' : '' ?>',
            onComplete: function(req) {
                $('container').innerHTML = req.responseText;
            }
        }
    );
    return false;
}

function on_move_window(fromEl, toEl)
{
    var fromPos = fromEl.id;
    var toPos   = toEl.id;
    ajax_url = '<?=$_SERVER['PHP_SELF']?>?interface=ajax&panel_id=<?=$panel_id?>&ajax_method=moveWindow';
    var myAjax = new Ajax.Updater (
        'container',  // Element to refresh
        ajax_url, // URL
        {          // options
            method: 'get',
            asynchronous: false,
            parameters: 'from='+fromPos+'&to='+toPos
        }
    );
    //
    // There is a bug in prototype when asynchronous = false, it doesn't
    // call  the "onComplete" function. This trick is a workarround.
    //
    $('container').innerHTML = myAjax.transport.responseText;
    ajax_load(fromPos);
    ajax_load(toPos);
    return false;
}

function panel_load(rows, cols)
{
    Control.Panel.setOptions(
        {
            rows: rows,
            cols: cols,
            posWidth: 520,
            posHeight: 300,
            posClass: 'panel-position',
            posHoverClass: 'panel-active',
            onWindowMove: on_move_window
        }
    );
    Control.Panel.drawGrid($('placehere'));
    for (i=1; i <= cols; i++) {
        for (j=1; j <= rows; j++) {
            var win_id = i+'x'+j;
            ajax_load(win_id);
        }
    }
}

panel_load(<?=$rows?>, <?=$cols?>);
Control.Tip.use = 'help';
</script>

</body></html>
