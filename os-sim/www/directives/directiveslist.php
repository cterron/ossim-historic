<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuCorrelation", "CorrelationDirectives");
?>

<?php
    $XML_FILE = '/etc/ossim/server/directives.xml';
    $XSL_FILE = 'directivemenu.xsl';
    
    if (version_compare(PHP_VERSION,'5','>=')&&extension_loaded('xsl'))
        require_once('xslt-php4-to-php5.php');

    if (isSet($_GET["css_stylesheet"])) {
        $css_stylesheet = $_GET["css_stylesheet"];
    } else {
        $css_stylesheet = 'directives.css';
    }
   
    $array_params = array ('css_stylesheet' => $css_stylesheet);
   
    $xslt = domxml_xslt_stylesheet_file($XSL_FILE);
    $xml = domxml_open_file($XML_FILE, DOMXML_LOAD_SUBSTITUTE_ENTITIES);
    $html = $xslt->process($xml, $array_params);
    echo $html->dump_mem(true);
?>
