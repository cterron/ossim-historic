<?php

class Plugin_Rss extends Panel
{
    var $defaults = array(
        'rss_source_url' => 'http://secunia.com/information_partner/anonymous/o.rss',
        'rss_max_entries' => '5',
        'rss_max_title_len' => '0',
        'rss_max_desc_len' => '150',
        'rss_fade_out' => '0'
    );
    
    function getCategoryName()
    {
        return _("RSS Feed");
    }
    
    function showSubCategoryHTML()
    {
        $rss_fade_out_yes = $rss_fade_out_no  = '';

        if ($this->get('rss_fade_out') == '1') {
            $rss_fade_out_yes = 'checked';
        } else {            
            $rss_fade_out_no = 'checked';
        }
        
        $html = "";
        $html .= _("Feed url");
        $html .= ':<br/><input type ="text" size="60" name="rss_source_url" value ="'.$this->get('rss_source_url').'"><br/>';
        $html .= _("Max entries to show");
        $html .= ':<br/><input type ="text" size="5" name="rss_max_entries" value ="'.$this->get('rss_max_entries').'"><br/>';
        $html .= _("Max title length in chars");
        $html .= ':<br/><input type ="text" size="5" name="rss_max_title_len" value ="'.$this->get('rss_max_title_len').'"><br/>';
        $html .= _("Max description length in chars");
        $html .= ':<br/><input type ="text" size="5"name="rss_max_desc_len" value ="'.$this->get('rss_max_desc_len').'"><br/>';
        $html .= _("Fade older RSS entries using smaller text?"). ':<br/>
            <input type="radio" name="rss_fade_out" value="1" '.$rss_fade_out_yes.'>'._("Yes").'<br/>
            <input type="radio" name="rss_fade_out" value="0" '.$rss_fade_out_no.'>'._("No").'
            <br/>
        ';

        return $html;
    }
    
    function showSettingsHTML()
    {
        return _("No extra settings needed for this category");
    }

    function showWindowContents()
    {
        $included = @include_once("XML/RSS.php");
        
        if (!$included) {
            echo _("Could not found XML/RSS.php from the XML_RSS Pear package.") . "<br>";
            echo _("Try install it with the command: 'pear install -a xml_parser xml_rss' as root");
            exit;
        }

        if(ini_get("allow_url_fopen") != 1){
            echo _("You need 'allow_url_fopen' enabled on your php.ini for this to work");
            exit;
        }
        if(!method_exists("XML_RSS", "parse")){
           echo _("Pear XML_RSS needs to be enabled for this to work") . "<br/>";
           echo _("You should be able to enable it issuing the following commands:") . "<br/><br/>";
           echo "<b>pear install -a xml_rss</b><br/>";
           echo "<b>pear install -a xml_parser</b>";
           exit();
        }

        $html = "";
        $numitems = 0;

        $rss =&new XML_RSS($this->get('rss_source_url'));
        $rss->parse();

        $max_title_length = $this->get('rss_max_title_len');
        $max_desc_length = $this->get('rss_max_desc_len');

        $html .= "<dl>\n";
        $closing_small = $this->get('rss_fade_out') == 1 ? '' : "</small>";

        foreach ($rss->getItems() as $item) {
            if($numitems >= $this->get('rss_max_entries')){
                break;
            }
            
            if($max_title_length > 0){
                $title = substr($item['title'],0,$max_title_length);
                if(strlen($item['title']) > strlen($title)) $title .= "...";
            } else { 
                $title = $item['title'];
            }

            if($max_desc_length > 0){
                $desc = substr($item['description'],0,$max_desc_length);
                if(strlen($item['description']) > strlen($desc)) $desc .= "...";
            } else { 
                $desc = $item['description'];
            }
            $title = utf8_decode($title);
            $desc  = utf8_decode($desc);
            // intentionally left out closing <small>, needs testing on different browsers.
            $html .= "<li><a href=\"" . $item['link'] . "\" target=\"_blank\">" . $title . "</a> <small>" . $desc . $closing_small . "</li>\n";
            $numitems++;
        }
        
        if($this->get("rss_fade_out") == 1){
          for($i=0;$i<$numitems;$i++){
            $html .= "</small>";
          } 
        }

        $html .= "</dl>\n";

        return $html;
    }

}
    
?>
