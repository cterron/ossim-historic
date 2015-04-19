<?php
require_once 'panel/Panel.php';

class Plugin_Config_Exchange extends Panel
{
    var $defaults = array(
        'import_text' => ''
    );
    
    function encode($options)
    {
        $text = $options['plugin_opts']['exported_plugin'].'::'."\n\r";
        $text .= chunk_split(base64_encode(serialize($options)), 35);
        return $text;
    }
    
    function decode($text)
    {
        list($plugin, $data) = explode('::', trim($text));
        $data = preg_replace("/\s*/s", '', $data);
        $data = unserialize(base64_decode($data));
        $data['exported_plugin'] = $plugin;
        return $data;
    }
    
    function save()
    {
        $text = $this->get('import_text');
        $data = $this->decode($text);
        $data['plugin'] = $data['plugin_opts']['exported_plugin'];
        return $data;
    }
    
    function getCategoryName()
    {
        return _("Config Import");
    }

    function showSubCategoryHTML()
    {
        $html = _("Import text") . ':<br/>';
        $html .= '<textarea name="import_text" rows="17" cols="36" wrap="off">';
        $html .= $this->get('import_text');
        $html .= '</textarea>';
        return $html;
    }
    
    function showSettingsHTML()
    {
        $opts = $this->get();
        $data = $this->decode($opts['plugin_opts']['import_text']);
        foreach ($data as $k => $v) {
            echo "[$k] = $v<br>\n";
        }
    }
    
    // No need to have this method because save() will transform the
    // data to the native plugin so next calls will use the
    // showWindowContents() of the native plugin
    function showWindowContents()
    {
        return "Error, this method shouldn't be called";
    }
}
?>
