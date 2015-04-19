<?php
    require_once ('classes/Session.inc');
    Session::logcheck("MenuCorrelation", "CorrelationDirectives");

    require_once ('ossim_conf.inc');
    require_once ('classes/Security.inc');
    
    $conf = $GLOBALS["CONF"];

    $XML_FILE = '/etc/ossim/server/directives.xml';
    $XSL_FILE = $conf->get_conf("base_dir") . '/directives/directivemenu.xsl';
    
    if (version_compare(PHP_VERSION,'5','>=') && extension_loaded('xsl')) {
        require_once('xslt-php4-to-php5.php');
    }

    if (GET('css_stylesheet')) {
        $css_stylesheet = GET('css_stylesheet');
    } else {
        $css_stylesheet = 'directives.css';
    }
   
    $array_params = array ('css_stylesheet' => $css_stylesheet);

    if (!function_exists('domxml_xslt_stylesheet_file')){
        require_once ("ossim_error.inc");
        $error = new OssimError();
        $error->display("PHP_DOMXML");
    }
    if (!is_file($XSL_FILE)) {
        die(_("Missing required XSL file")." '$XSL_FILE'");
    }
    if (!is_file($XML_FILE)) {
        die(_("Missing required XML file")." '$XML_FILE'");
    }
    $xslt = domxml_xslt_stylesheet_file($XSL_FILE);
    $xml = domxml_open_file($XML_FILE, DOMXML_LOAD_SUBSTITUTE_ENTITIES);
    $html = $xslt->process($xml, $array_params);
    echo $html->dump_mem(true);
?>
