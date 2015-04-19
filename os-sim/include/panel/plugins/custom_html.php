<?php

class Plugin_Custom_HTML extends Panel
{
    var $defaults = array(
        'customhtml' => ''
    );
    
    function getCategoryName()
    {
        return _("Custom HTML contents");
    }
    
    function showSubCategoryHTML()
    {
        $html = _("HTML code") . ':<br/>';
        $html .= '<textarea name="customhtml" rows="20" cols="35" wrap="on">';
        $html .= $this->get('customhtml');
        $html .= '</textarea>';
        return $html;
    }
    
    function showSettingsHTML()
    {
        return _("No extra settings needed for this category");
    }

    function showWindowContents()
    {
        $customhtml = $this->get('customhtml');
        $html = $customhtml;
        return $html;
    }
   
}
    
?>
