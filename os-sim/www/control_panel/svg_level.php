<?php
    require_once('classes/Session.inc');
//    Session::logcheck("MenuControlPanel", "ControlPanelMetrics");
//    $user = Session::get_session_user();

    header("Content-Type: image/svg+xml");
    
    $sec_level = $_GET["sl"];
    if ($sec_level >= 95) {
        $color = "excellent";
    } elseif ($sec_level >= 90) {
        $color = "good";
    } elseif ($sec_level >= 85) {
        $color = "moderate";
    } elseif ($sec_level >= 80) {
        $color = "low";
    } elseif ($sec_level >= 75) {
        $color = "bad";
    } else {
        $color = "emergency";
    }


// yscale set the height of the thermometer
    if (isset($_GET["scale"])) {
        $yscale = $_GET["scale"];
    } else {
        $yscale = 1;
    }
    $coordmax = $yscale * 100 + 5;
    $coordcur = $yscale * $sec_level + 5;

$blob = '<?xml version="1.0" ?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.0//EN"
 "http://www.w3.org/TR/2001/REC-SVG20010904/DTD/svg10.dtd">
<svg width="100%" height="100%" viewBox="0 0 130 '. (100*$yscale + 37) .'" preserveAspectRatio="xMinYMid meet">
<desc>
  Draw a thermometer to show the service level
</desc>
<defs>
   <style type="text/css"><![CDATA[
       .excellent {fill:green}
       .good {fill:#CCFF00}
       .moderate {fill:#FFFF00}
       .low {fill:orange}
       .bad {fill:#FF3300}
       .emergency {fill:red}
   ]]>
   </style>
   
   <filter id="halo">
       <feColorMatrix type="matrix"
           value="0 0 0 0 0
                  0 0 0 0 0
                  0 0 0 0.9 0
                  0 0 0 1   0" />
       <feGaussianBlur stdDeviation="1" result="colorBlur" />
       <feMerge>
           <feMergeNode in="colorBlur" />
           <feMergeNode in="SourceGraphic" />
       </feMerge>
   </filter>
  
   <filter id="3Dlight" width="150%" height="150%">
       <feGaussianBlur in="SourceAlpha" stdDeviation="1.5" result="shadow" />
       <feOffset in="shadow" dx="2" dy="2" result="shadow" />
       <feGaussianBlur in="SourceAlpha" stdDeviation="5" result="blur"/>
       <feOffset in="blur" dx="2" dy="2" result="offsetBlur"/>
       <feSpecularLighting in="blur" surfaceScale="5" specularConstant="1"
                           specularExponent="15" style="lighting-color:white"
                           result="specOut">
           <fePointLight x="-5000" y="-10000" z="10000"/>
       </feSpecularLighting>
       <feComposite in="specOut" in2="SourceAlpha" operator="in"
                    result="specOut" />
       <feComposite in="SourceGraphic" in2="specOut" operator="arithmetic"
                    k1="0" k2="1" k3="1" k4="0" result="litPaint" />
       <feMerge> 
           <feMergeNode in="shadow"/> 
           <feMergeNode in="litPaint"/> 
       </feMerge>
   </filter>
</defs>

<g style="font-size: 10pt; font-family: sans-serif;" transform="translate(13,5)">
    <text x="20" y="'.($yscale*100+10).'" style="text-anchor: end;">100%</text>
    <line x1="21" y1="'. ($yscale*100+5) .'" x2="25" y2="'. ($yscale*100+5) .'" style="stroke: black;" />
    <text x="20" y="'.($yscale*75+10).'" style="text-anchor: end;">75%</text>
    <line x1="21" y1="'.($yscale*75+5).'" x2="25" y2="'.($yscale*75+5).'" style="stroke: black;" />
    <text x="20" y="10" style="text-anchor: end;">0%</text>
    <line x1="21" y1="5" x2="25" y2="5" style="stroke: black;" />
    <text x="50" y="'.($yscale*50+15).'" class="'. $color .'"
          style="filter: url(#halo); text-anchor: begin;
                 font-size: 16pt;">'. (int)$sec_level .'%</text>
</g>

<g transform="translate(20,5)"  style="filter:url(#3Dlight);">
    <path class="'. $color .'"
          d="M25 '. $coordmax .' A 10 10 0 1 0 35 '. $coordmax .' L 35 ' 
          . $coordcur . ' A 5 5 0 1 0 25 '. $coordcur  .'Z"
          style="stroke: none;" />
    <path id="thermometer"
          d= "M25 '. $coordmax .' A 10 10 0 1 0 35 '. $coordmax .' L 35 5 A 5 5 0 1 0 25 5Z"
          style="stroke: black; fill: none;"/>

</g>

</svg>';

echo $blob;
?>
