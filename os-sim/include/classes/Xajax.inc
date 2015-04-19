<?php

require_once ('xajax/xajax.inc.php');

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
