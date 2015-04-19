<?php
/*
 * TODO:
 * - Add options for Window contents update frecuency
 * - Export/import plugin for easily exchange config
 */

require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once 'ossim_db.inc';
require_once 'panel/Ajax_Panel.php';
Session::logcheck("MenuControlPanel", "ControlPanelExecutive");

if (Session::menu_perms("MenuControlPanel", "ControlPanelExecutiveEdit")) {
    $can_edit = GET('edit') ? true : false;
    $show_edit = true;
} else {
    $can_edit = $show_edit = false;
}

//
// Detect if that's an AJAX call
//
if (GET('interface') == 'ajax') {
    
    if (GET('ajax_method') == 'showWindowContents') {
        
        $ajax = &new Window_Panel_Ajax();
        $options = $ajax->loadConfig(GET('id'));
        if (count($options)) {
            $data['CONTENTS'] = $ajax->showWindowContents($options);
            $data['TITLE']    = $options['window_opts']['title'];
        } else { // New window
            $data['CONTENTS'] = '';
            $data['TITLE']    = _("New window");
        }
        if ($can_edit) {
            $data['CONFIG']   = '[<a href="window_panel.php?id='.GET('id').'" title="config">config</a>]';
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
</style>
  
</head>
<body>
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
<? } ?>
</td><td align="right"><small>
<? if ($show_edit && !GET('edit')) { ?>
[<a href="<?=$_SERVER['PHP_SELF']?>?edit=1">edit</a>]
<? } elseif ($show_edit) { ?>
[<a href="<?=$_SERVER['PHP_SELF']?>">no edit</a>]
<? } ?>
</small>
</td></tr>
</table>


<!-- displays saveConfig errors -->
<div id="container" style="margin: 0px; padding: 0px"></div>
<div id="loading" class="loading"></div>

<div id="placehere">
Ossim Panel Loading...
</div>

<!-- do nothing, Ajax.Updater in ajax_load() needs an element -->
<div id="null" style="display: none"></div>

<script>

var myResponders = {
    onCreate: function() {
        Element.show('loading');
        $('loading').innerHTML = 'Loading..';
    }
}
Ajax.Responders.register(myResponders);
var AjaxRequestCounter = 0;

function ajax_load(id)
{
    ajax_url = '<?=$_SERVER['PHP_SELF']?>?interface=ajax&ajax_method=showWindowContents&id='+id;
    AjaxRequestCounter++;
    new Ajax.Updater (
        'null',  // Element to refresh
        ajax_url, // URL
        {          // options
            method: 'get',
            asynchronous: true,
            parameters: '<?= $can_edit ? 'edit=1' : '' ?>',
            onComplete: function(req) {
                $('loading').innerHTML = 'Loading ('+AjaxRequestCounter+' pending)';
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
    ajax_url = '<?=$_SERVER['PHP_SELF']?>?interface=ajax&ajax_method=savePanelConfig';
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
    ajax_url = '<?=$_SERVER['PHP_SELF']?>?interface=ajax&ajax_method=moveWindow';
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

panel_load(<?=$rows?>, <?=$cols?>)
</script>

</body></html>
