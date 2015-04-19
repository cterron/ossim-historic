<?php

require_once ('ossim_conf.inc');
$config = $GLOBALS["CONF"];

$xajax_php_path = $config->get_conf("xajax_php_path");
$xajax_included = @include_once "$xajax_php_path/xajax.inc.php";
if (!$xajax_included) {
    // xajax default php location, included in the source
    include_once 'xajax/xajax.inc.php';
}
$xajax_js_path = $config->get_conf("xajax_js_path");
$xajax_js = $xajax_js_path . DIRECTORY_SEPARATOR . 'xajax.js';
if (!is_file($xajax_js)) {
    // xajax default js location, included in the source
    $xajax_js_path  = "../js/";
}

DEFINE('XAJAX_JS', $xajax_js_path . DIRECTORY_SEPARATOR . 'xajax.js');

class xajaxTableResponse extends xajaxResponse {

    function addTableRow($tableName) {
        $this->addScript("xajax.\$('".$tableName."').insertRow(-1);");
    }
   
    function addTableCell($tableName, $cellContents, $rowNumber=null) {
        if ($rowNumber !== null || !is_numeric($rowNumber))
            $rowNumber = "tbl.rows.length-1";
        $this->addScript("var tbl=xajax.\$('".$tableName."');tbl.rows[".$rowNumber."].insertCell(-1).innerHTML='".addSlashes($cellContents)."';");
    }
}

function xajax_debug($var, &$resp)
{
    $html = printr($var, false, true);
    $resp->AddAppend("xajax_debug", "innerHTML", $html);
    return $resp;
}

?>
