<?php
require_once 'classes/Security.inc';

class Panel
{
    var $params = array();
    
    function Panel()
    {
        return;
    }
    
    function getCategoryName()
    {
        return _("Category Name not configured");
    }
    
    function setup($params)
    {
        if (!isset($params['plugin_opts'])) {
            echo "<b>Warning: old format detected, please configure again</b><br>";
            return;
        }
        $all_options = $params['plugin_opts'];
        $plugin_opts = array();
        foreach ($this->defaults as $var => $value) {
            if (isset($all_options[$var])) {
                $plugin_opts[$var] = strip($all_options[$var]);
            } else {
                $plugin_opts[$var] = $value;
            }
        }
        $this->params['plugin']      = $params['plugin'];
        $this->params['plugin_opts'] = $plugin_opts;
        $this->params['window_opts'] = $params['window_opts'];
        if (isset($params['metric_opts'])) {
        $this->params['metric_opts'] = $params['metric_opts'];
        } else {
        $this->params['metric_opts'] = array();
        }
    }
    
    // This method is called from $ajax->saveConfig(), in case the plugin
    // needs to modify data at save time (ex. the import plugin)
    function save()
    {
        return $this->get();
    }
    
    function get($param = null, $category = 'plugin_opts')
    {
        // if $param is null, return all params
        if ($param === null) {
    		return $this->params;
    	}
        if (isset($this->params[$category][$param])) {
            $ret = stripslashes($this->params[$category][$param]);
        } else {
            echo "Warning, not defined var '$param', shouldn't occur, please report<br>";
            $ret = null;
        }
        return $ret;
    }
}
?>
