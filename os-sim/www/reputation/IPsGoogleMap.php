<?php
/**
*
* License:
*
* Copyright (c) 2003-2006 ossim.net
* Copyright (c) 2007-2013 AlienVault
* All rights reserved.
*
* This package is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; version 2 dated June, 1991.
* You may not use, modify or distribute this program under any other version
* of the GNU General Public License.
*
* This package is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this package; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
* MA  02110-1301  USA
*
*
* On Debian GNU/Linux systems, the complete text of the GNU General
* Public License can be found in `/usr/share/common-licenses/GPL-2'.
*
* Otherwise you can read it here: http://www.gnu.org/licenses/gpl-2.0.txt
*
*/


ini_set('memory_limit', '1024M');
set_time_limit(300);

require_once 'av_init.php';
Session::logcheck("dashboard-menu", "IPReputation");


require_once 'classes/Reputation.inc';


$type = intval(GET('type'));
$act  = GET('act');

if (empty($act)) 
{
   $act = "All";
}

ossim_valid($act,   OSS_INPUT,OSS_NULLABLE,     'illegal: Action');

if (ossim_error()) 
{
    die(ossim_error());
}

$nodes      = array();
$Reputation = new Reputation();

$i = 0;
if ($Reputation->existReputation()) 
{

	list($ips,$cou,$order,$total) = $Reputation->get_data($type,$act);
	session_write_close();

	foreach ($ips as $activity => $ip_data) if ($activity==$act || $act=="All")
	{
		foreach ($ip_data as $ip => $latlng) 
		{
			if(preg_match("/-?\d+(\.\d+)?,-?\d+(\.\d+)?/",$latlng)) 
			{
				$tmp  = explode(",", $latlng);
				 
				$node = array(
				    'ip'  => "$ip [$activity]",
				    'lat' => $tmp[0],
				    'lng' => $tmp[1]
				);
				
				$nodes[$ip] = $node;
			} 
		}
	}
}
			
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
<head>
	<title><?php echo _("IP Reputation")?></title>
	
	
	<?php
    //CSS Files
    $_css_files = array(
        array('src' => 'av_common.css',     'def_path' => TRUE)
    );

    //JS Files
    $_js_files = array(
        array('src' => 'jquery.min.js',         'def_path' => TRUE),
        array('src' => 'utils.js',              'def_path' => TRUE),
        array('src' => 'notification.js',       'def_path' => TRUE),
        array('src' => 'av_map.js.php',         'def_path' => TRUE),
        array('src' => 'markerclusterer.js',    'def_path' => TRUE),
        array('src' => 'messages.php',          'def_path' => TRUE)
    );
    
    Util::print_include_files($_css_files, 'css');
    Util::print_include_files($_js_files, 'js');
    
    ?>
    	
	<script type="text/javascript">

		var otx_url = "<?php echo Reputation::getlabslink('XXXX') ?>";
		var points  = <?php echo json_encode($nodes) ?>;
		
		$(document).ready(function()
		{
    		av_map = new Av_map('map');
    		
            Av_map.is_map_available(function(conn)
            {
                if(conn)
                {   
        			av_map.set_zoom(3);
                    av_map.set_location(37.1833,-3.6141);
                    av_map.set_center_zoom(false);
                    
                    av_map.draw_map();
                    
                    var markers = [];
        			$.each(points, function(i, p)
        			{        	
        				var pos    = new google.maps.LatLng(p.lat, p.lng);
        				var marker = new google.maps.Marker(
        				{
        					position: pos,
        					title: p.ip
        				});
        				
        				google.maps.event.addListener(marker, 'click', function() 
        				{
                            try
                            {
                                var ip  = this.title.match(/\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}/);
                                var url = otx_url.replace('XXXX', ip)
        
                                var win = window.open(url, '_blank')
                                win.focus()
                            }
                            catch(Err){}  
                        });
                        
                        markers.push(marker);
        			});
        			
        			var mcOptions     = {gridSize: 80, maxZoom: 15};
        			var markerCluster = new MarkerClusterer(av_map.map, markers, mcOptions);
        			
                }
                else
                {
                    av_map.draw_warning();
                }
                
                if (typeof(parent.show_map)=='function') 
    			{
    				parent.show_map();
    			}
        			
            });
			
		});
	</script>
	
	<style type='text/css'>
    	body, html 
    	{
    		height:100%;
    		width:100%;
    		margin:0px;
    		padding:0px;
    	}
	</style>
</head>

<body>
	<div id="map" style="width: 100%; height: 100%"></div>
</body>

</html>
