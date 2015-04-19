<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once 'panel/Ajax_Panel.php';
require_once 'classes/Util.inc';

Session::logcheck("MenuControlPanel", "ControlPanelExecutiveEdit");

/*
 * Brief architecture overview:
 * 
 * There is an Accordion dynamic ajax panel for controlling the window
 * options. Each category inside the 'Category' tab is internally an
 * independant plugin.
 * 
 * Once the user selects a category (or plugin) and click on 'Subcategory',
 * we do an ajax call and retrieve the contents of that tab from the plugin
 * method: showSubCategoryHTML. The same for the 'Settings' tab.
 * 
 * Internals:
 * 
 * 1� The windows_panel.php code displays the HTML
 * 2� When the user clicks a tab of the accordion(1):
 *      - The event is detected and the JS on_show_tab() function is called
 *      - on_show_tab(), serializes(2) the params filled in the form and
 *        build an Ajax GET request(3) to window_panel.php?interface=ajax&ajax_method=FOO
 *      - window_panel.php detects that the call is made from JS (Ajax),
 *        loads all the plugins it found in the directory $plugins_dir and
 *        creates the object Window_Panel_Ajax.
 *      - Window_Panel_Ajax then calls the method FOO of the plugin selected
 *        by the user in the 'Category' tab. This returns HTML code.
 *      - Window_Panel_Ajax send that HTML back to the Ajax request
 *      - The HTML contents of the clicked tab is replaced with the contents
 *        of the Ajax request.
 * 
 * (1) This widget is created using with Rico: new Rico.Accordion()
 * (2) Using Prototype: Form.serialize() function
 * (3) Using Prototype: new Ajax.Updater() function
 * 
 */

$id = GET('id');

//
// Detect if that's an AJAX call
//
if (GET('interface') == 'ajax') {
    
    if (!$id) {
        die("Invalid ID");
    }
    
    $ajax = &new Window_Panel_Ajax();

    
    $opts['window_opts']['id']    = $id;

    // complete missing options set by the user, with the previous defined ones
    // in case of edited window
    $options = $ajax->loadConfig($id);
    // new windows should load this
    if (count($options) || GET('plugin')) {
        $opts['plugin'] = GET('plugin') ? GET('plugin') : $options['plugin'];
        $title = isset($options['window_opts']['title']) ? $options['window_opts']['title'] : '';
        $help  = isset($options['window_opts']['help'])  ? $options['window_opts']['help'] : '';
        $opts['window_opts']['title'] = GET('window_title') ? GET('window_title') : $title;
        $opts['window_opts']['help']  = GET('window_help') !== null ? GET('window_help') : $help;

        $enable_metrics = isset($options['metric_opts']['enable_metrics'])  ? $options['metric_opts']['enable_metrics'] : '';
        $opts['metric_opts']['enable_metrics']  = GET('enable_metrics') !== null ? GET('enable_metrics') : $enable_metrics;
        $metric_sql = isset($options['metric_opts']['metric_sql'])  ? $options['metric_opts']['metric_sql'] : '';
        $opts['metric_opts']['metric_sql']  = GET('metric_sql') !== null ? GET('metric_sql') : $metric_sql;
        $low_threshold = isset($options['metric_opts']['low_threshold'])  ? $options['metric_opts']['low_threshold'] : '';
        $high_threshold = isset($options['metric_opts']['high_threshold'])  ? $options['metric_opts']['high_threshold'] : '';
        $opts['metric_opts']['low_threshold']  = GET('low_threshold') !== null ? intval(GET('low_threshold')) : $low_threshold;
        $opts['metric_opts']['high_threshold']  = GET('high_threshold') !== null ? intval(GET('high_threshold')) : $high_threshold;

    }
    if (!isset($options['plugin_opts'])) {
        $options['plugin_opts'] = array();
    }
    foreach ($options['plugin_opts'] as $key => $value) {
        $opts['plugin_opts'][$key] = $value;
    }
    foreach ($_GET as $key => $value) {
	// one: strip breaks the array variables...
	if(is_string($value))
        	$opts['plugin_opts'][$key] = strip($value);
	else
        	$opts['plugin_opts'][$key] = $value;
    }
    $method = GET('ajax_method');

    if ($method == 'saveConfig') {
        echo $ajax->saveConfig($id, $opts);

    } elseif ($method == 'showWindowContents') {
        // XXX This should save the options as temp.. so the user
        //     will be able to revert the settings
        $opts = $ajax->loadConfig($id);

        // Add metric threshold indicator
        $indicator = "";
        if(isset($opts['metric_opts']['enable_metrics']) && $opts['metric_opts']['enable_metrics'] == 1 && isset($opts['metric_opts']['metric_sql']) && strlen($opts['metric_opts']['metric_sql']) > 0){

        $db = new ossim_db;
        $conn = $db->connect();
        $sql = $opts['metric_opts']['metric_sql'];
        if (!$rs = $conn->Execute($sql)) {
            echo "Error was: ".$conn->ErrorMsg()."\n\nQuery was: ".$sql;
            exit();
        }
        $metric_value = $rs->fields[0];
        $db->close($conn);
            $low_threshold = $opts['metric_opts']['low_threshold'];
            $high_threshold = $opts['metric_opts']['high_threshold'];

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

        $data['CONTENTS'] = $ajax->showWindowContents($opts);
        $data['TITLE']    = $opts['window_opts']['title'] . $indicator;
        $data['HELP_LABEL'] = _("help");
        $data['HELP_MSG'] = Util::string2js($opts['window_opts']['help']);
        $data['CONFIG']   = '';
        $data['ID']       = $id;
        echo $ajax->parseTemplate('./window_tpl.htm', $data);
	
    } elseif ($method == 'showExportText') {
        $opts = $ajax->loadConfig($id);
        if ($opts['plugin'] != 'plugin_config_exchange') {
            $opts['plugin_opts']['exported_plugin'] = $opts['plugin'];
            $opts['plugin'] = 'plugin_config_exchange';
            $plugin = $ajax->getPlugin($opts);
            $data['CONTENTS'] = nl2br($plugin->encode(($opts)));
        // In case user hit the export button and the plugin is
        // already import (avoid encode twice)
        } else {
            $data['CONTENTS'] = nl2br($opts['plugin_opts']['import_text']);
        }
        
        $data['TITLE']    = _("Exported text");
        $data['HELP_LABEL'] = _("help");
        $data['HELP_MSG'] = '';
        $data['CONFIG']   = '';
        $data['ID']       = $id;
        echo $ajax->parseTemplate('./window_tpl.htm', $data);
    
    } else {
        // security check: only allow calling valid methods of Window_Panel_Ajax
        $allowed = array('showCategoriesHTML', 'showSubCategoryHTML', 'showMetricsHTML',
                         'showSettingsHTML');
        if (!in_array($method, $allowed)) {
            die("Invalid method: '$method'");
        }
        echo $ajax->$method($opts);
    }
    exit;
}
?>
<html>
<head>
    <script src="../js/prototype.js" type="text/javascript"></script>
    <script src="./panel.js" type="text/javascript"></script>
    <script src="../js/rico/rico.js" type="text/javascript" ></script>
    <style>
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
    input, textarea {
        background: #e7e9ed;
        color: #7b7b7b;
        font-size: 0.9em;
        padding: 1px 2px 1px 1px;
        border: 1px solid #d5d7db;
    }
    input:hover, textarea:hover{
        border: 1px solid #7b7b7b;
    }
    input:focus, textarea:focus{
        background: #FFFFFF;
        border: 1px solid #000000;
        color: #7b7b7b;
        padding: 1px 2px 1px 1px;
    }
    .accordionTabTitleBar {
        cursor: pointer;
    }
    .accordionTabTitleBar {
        font-size           : 12px;
        padding             : 4px 6px 4px 6px;
        border-style        : solid none solid none;
        border-top-color    : #BDC7E7;
        border-bottom-color : #182052;
        border-width        : 1px 0px 1px 0px;
    }

    .accordionTabTitleBarHover {
        font-size        : 11px;
        background-color : #1f669b;
        color            : #000000;
    }

    .accordionTabContentBox {
       font-size        : 11px;
       border           : 1px solid #1f669b;
       border-top-width : 0px;
       padding          : 0px 8px 0px 8px;
    }
   .loading {
      position: absolute;
      top: 1px;
      right: 1px;
      background-color: #AC0606;
      color: white;
  }
  .help {
      position: absolute;
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
<div id="loading" class="loading">Loading..</div>
<div id="help" class="help"></div>
<script>Element.hide('help');</script>

<form id="panel" method="POST">
<input type="hidden" name="panel_id" value="<?= GET('panel_id'); ?>">
<table width="100%" align="center">
<tr>
<td width="40%">
<div style="margin-top:6px; border-top-width:1px; border-top-style:solid;"
    id="accordionExample">

    <div id="panel1">
      <div id="panel1Header" class="accordionTabTitleBar">
        <?=_("Category")?>
      </div>
      <div id="panel1Content"  class="accordionTabContentBox">
      </div>
    </div>

    <div id="panel2">
      <div id="panel2Header" class="accordionTabTitleBar">
        <?=_("Sub-category")?>
      </div>
      <div id="panel2Content"  class="accordionTabContentBox">
      </div>
    </div>

    <div id="panel3">
      <div id="panel3Header" class="accordionTabTitleBar">
        <?=_("Settings")?>
      </div>
      <div id="panel3Content"  class="accordionTabContentBox">
      </div>
    </div>

    <div id="panel4">
      <div id="panel4Header" class="accordionTabTitleBar">
        <?=_("Metrics")?>
      </div>
      <div id="panel4Content"  class="accordionTabContentBox">
      </div>
    </div>


</div>
<br>
<center>
    <input type="button" value="<?=_("Accept config")?>"
           onClick="javascript: ajax_save('<?=$id?>'); document.location = 'panel.php?panel_id=<?=GET("panel_id")?>';">
    &nbsp;
    <input type="button" name="export" value="<?=_("Export Config")?> -&gt;"
           onClick="javascript: ajax_save('<?=$id?>'); ajax_show(false, 'export');">
    &nbsp;
    <input type="button" name="update" value="<?=_("Update Output")?> -&gt;"
           onClick="javascript: ajax_save('<?=$id?>'); ajax_show(false, 'output');">
</center>
</td>
<td>
<center><h3><?=_("Config for window position").": $id"?></h3>
<div id="debug" style="width: 520px; text-align: left;"></div>
<div id="output" style="border-width:0px; height: 400px; width: 520px; text-align: left;"></div>
</center>
<br>
<center>
    
</center>
</td></tr>
</table>
</form>
<script>
/*
@param object tab: The DIV header object, ex: panel1Header
                    - panel1Header: Category
                    - panel2Header: Subcategory
                    - panel3Header: Settings
                    - panel3Header: Panel Metrics
*/
function ajax_show(tab, window)
{
    var id;
    if (window) {
        id = window;
    } else {
        id = tab.titleBar.id;
    }
    var method;
    var refresh;

    if (id == 'panel1Header') {
        method = 'showCategoriesHTML';
        refresh = 'panel1Content';
    }
    if (id == 'panel2Header') {
        method = 'showSubCategoryHTML';
        refresh = 'panel2Content';
    }
    if (id == 'panel3Header') {
        method = 'showSettingsHTML';
        refresh = 'panel3Content';
    }
    if (id == 'panel4Header') {
        method = 'showMetricsHTML';
        refresh = 'panel4Content';
    }
    if (id == 'output') {
        method = 'showWindowContents';
        refresh = 'output';
    }
    if (id == 'export') {
        method = 'showExportText';
        refresh = 'output';
    }
    ajax_url = '<?=$_SERVER['PHP_SELF']?>?interface=ajax&ajax_method='+method+'&id=<?=$id?>&panel_id=<?=GET('panel_id')?>';
    Element.show('loading');
    // Ajax functionallity from Prototype v.1.x
    new Ajax.Updater (
        refresh,  // Element to refresh
        ajax_url, // URL
        {          // options
            method: 'get',
            asynchronous: true,
            parameters: Form.serialize('panel'),
            evalScripts: true,
            onComplete: function(req) {
                //$('debug').innerHTML = req.responseText;
                Element.hide('loading');
                $('refresh').innerHTML = req.responseText;
                
            }
        }
    );
    return false;
}

function ajax_save(id)
{
    ajax_url = '<?=$_SERVER['PHP_SELF']?>?interface=ajax&panel_id=<?=GET('panel_id')?>&ajax_method=saveConfig&id='+id;
    var debug = 'debug';
    var myAjax = new Ajax.Updater (
        debug,  // Element to refresh
        ajax_url, // URL
        {          // options
            method: 'get',
            asynchronous: false,
            parameters: Form.serialize('panel')
        }
    );
    //
    // There is a bug in prototype when asynchronous = false, it doesn't
    // call  the "onComplete" function. This trick is a workarround.
    //
    $('debug').innerHTML = myAjax.transport.responseText;
    return false;
}

var myaccordion = new Rico.Accordion( 'accordionExample',
                    {
                        panelHeight: 400,
                        // When user click on a tab, call on_show_tab()
                        onShowTab: function(tab) {
                            ajax_show(tab);
                        }
                    }
);

// Launch Ajax call for getting the contents of the first tab
ajax_show(myaccordion.accordionTabs[0]);

function colorSelected(color)
{
    $('graph_color').value = color;
    Element.setStyle('color_sample', {background: color});
    Element.hide('palette');
}
Control.ColorPalette.registerOnColorClick = colorSelected;
Control.Tip.use = 'help';
</script>


</body></html>
