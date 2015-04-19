<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once 'classes/Xajax.inc';
require_once 'classes/Member_status.inc';
require_once 'classes/Util.inc';

Session::logcheck("MenuConfiguration", "ConfigurationMaps");

$map_id = GET('map_id') ? GET('map_id') : die("Invalid map_id");

$db = new ossim_db();
$conn = $db->connect();

$sql = "SELECT id, name, engine, center_x, center_y, zoom, engine_data3, engine_data4
        FROM map
        WHERE id = ?";
$map = $conn->GetRow($sql, array($map_id));
switch ($map['engine']) {
    case 'openlayers_op':    $layer = 'op';    break;
    case 'openlayers_ve':    $layer = 've';    break;
    case 'openlayers_image':
        $layer  = 'image';
        $width  = $map['engine_data3'];
        $height = $map['engine_data4'];
        break;
}

$status = new Member_status;
$sql = "SELECT id, type, ossim_element_key, x, y
        FROM map_element
        WHERE map_id=?";
if (!$rs = $conn->Execute($sql, array($map['id']))) {
    die(ossim_error($conn->ErrorMsg()));
}
$items = array();
while (!$rs->EOF) {
    /*
    $item = array(
        'name' => 
        'icon' =>
        'status_value' =>
        'status_text' =>
        'link' =>
        'description' =>
    */
    $item = $status->get($rs->fields['ossim_element_key'], $rs->fields['type']);
    $item['description'] = Util::string2js($item['description']);
    $item['x'] = $rs->fields['x'];
    $item['y'] = $rs->fields['y'];
    $item['popup'] = "<b>{$item['name']}</b><br>{$item['description']}";
    $items[] = $item;
    $rs->MoveNext();
}

?>
<html>
<head>
  <title> <?php echo gettext("OSSIM Framework"); ?> </title>
  <meta http-equiv="refresh" content="180">
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>

    <style type="text/css">  
    #map {
            width: 512px;
            height: 450px;
            border: 1px solid gray;
            align: center;
        }
    </style>
    
    <script src="../js/prototype.js" type="text/javascript"></script>
    <? if ($layer == 've') { ?>
        <script src='http://dev.virtualearth.net/mapcontrol/v3/mapcontrol.js'></script>
    <? } ?>
    <script src="../js/OpenLayers/OpenLayers.js"></script>
    <script type="text/javascript">
        <!--
        //OpenLayers.Popup.WIDTH = 200;
        //OpenLayers.Popup.HEIGHT = 200;
        //OpenLayers.Popup.COLOR = "white";
        //OpenLayers.Popup.OPACITY = 1;
        //OpenLayers.Popup.BORDER = "3px coral solid";

        var zoom = <?=$map['zoom']?>;
        var lon = <?=$map['center_x']?>;
        var lat = <?=$map['center_y']?>;
        var map, layer, markers;

        function viewCenter()
        {
            var lonlat = map.getCenter();
            $('lat').innerHTML = lonlat.lon;
            $('lon').innerHTML = lonlat.lat;
        }

        function createMarker(lon, lat, icon, content)
        {
            var lonlat = new OpenLayers.LonLat(lon, lat);
            var data = new Object();
            data.icon = new OpenLayers.Icon(icon);
            // rustic auto height popup size
            var str = content.split(/<br>/g);
            var height = str.length * 25;
            data.popupSize = new OpenLayers.Size(150, height);
            data.popupContentHTML = content;
            
            // 'feature' is an OpenLayers object capable of both:
            // create markers and set their popups
            var feature = new OpenLayers.Feature(markers, lonlat, data);
            
            var mypop = feature.createPopup();
            mypop.events.register("mouseout", mypop, popup_mouseout);
            // Store the popop as a private property so we can play with it later
            feature.mypop = mypop;
            
            var marker = feature.createMarker();
            marker.events.register("mouseover", feature, marker_mouseover);

            markers.addMarker(marker);
            return true;                                                             
        }
        
        function marker_mouseover(evt) {
            // 'this' is the object 'feature' attached to the registered event in createMarker
            map.addPopup(this.mypop, true); //true is exclusive, that's: hide the others popups
            Event.stop(evt);
        }
        
        function popup_mouseout(evt) {
            // 'this' is a popup object
            map.removePopup(this);
            Event.stop(evt);
        }
        
        function init()
        {
            map = new OpenLayers.Map('map');

            var options = {
                           resolutions: [1, 0.5, 0.3],
                           maxResolution: 'auto',
                           numZoomLevels: 4
                          };

         <? if ($layer == 'image') { ?>
            layer = new OpenLayers.Layer.Image(
                                'Custom Image',
                                './output_image_map.php?map_id=<?=$map_id?>',
                                new OpenLayers.Bounds(-180, -90, 90, 180),
                                new OpenLayers.Size(<?=$width?>, <?=$height?>),
                                options);
            //map.zoomToMaxExtent();
         <? } ?>

         <? if ($layer == 'op') { ?>
            layer = new OpenLayers.Layer.WMS( "OpenLayers WMS", 
                        "http://labs.metacarta.com/wms/vmap0", {layers: 'basic'} );
         <? } ?>
         
         <? if ($layer == 've') { ?>
            layer = new OpenLayers.Layer.VirtualEarth(
                                "VE",
                                {'type': VEMapStyle.Road});
         <? } ?>
            map.addLayer(layer);

            /*
            var yahoo = new OpenLayers.Layer.Yahoo("Yahoo");
            map.addLayer(yahoo);
            //*/

            map.setCenter(new OpenLayers.LonLat(lat, lon), zoom);

            map.events.register("zoomend", map, function(e) {
                markers.redraw();
            });
            map.events.register("move", map, function(e) {
                markers.redraw();
            });

            //var newl = new OpenLayers.Layer.Text( "text", { location:"../maps/openlayers_markers_text.php?map_id=<?=$map_id?>"} );
            //map.addLayer(newl);
            
            markers = new OpenLayers.Layer.Markers( "Markers" );
            map.addLayer(markers);
        <? foreach ($items as $i) { ?>
            createMarker(<?=$i['x']?>, <?=$i['y']?>, '<?=$i['icon']?>', '<?=$i['popup']?>');
        <? } ?>
        }
        
        // -->
    </script>
</head><body onLoad="javascript: init();">
<br>
<div id="map"></map>

</body></html>