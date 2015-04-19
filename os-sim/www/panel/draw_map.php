<?php
require_once 'classes/Security.inc';
require_once 'classes/Session.inc';
require_once 'classes/User_config.inc';
require_once 'ossim_db.inc';

Session::logcheck("MenuControlPanel", "ControlPanelExecutive");

$db = new ossim_db();
$conn = $db->connect();
$config = new User_config($conn);
$login = Session::get_session_user();

$window_id = GET('window_id') ? GET('window_id') : die("invalid window_id");

if (preg_match('#/window_panel.php#', @$_SERVER['HTTP_REFERER'])) {
    $status = 'edit';
} elseif (preg_match('#host.php$#', @$_SERVER['HTTP_REFERER'])) {
    $status = 'set_marker_pos';
} else {
    $status = 'view';
}

/*
 * Array
(
    [map_type] => virtual_earth
    [lon] => 0
    [lat] => 0
    [zoom] => 0
    [controls] => 0
    [max-zoom] => 0
    [min-zoom] => 15
)
 */
$opts = $config->get($login, $window_id, 'php', 'panel');

if (!$opts) {
    die(ossim_error(_("Map configuration not found, please configure a map using the Executive Panel")));
}

$zoom_js = "minZoomLevel: {$opts['max-zoom']}, maxZoomLevel: {$opts['min-zoom']}";

function get_icon($metric_a, $threshold_a, $metric_c, $threshold_c)
{
    $risk_a = round($metric_a/$threshold_a*100);
    $risk_c = round($metric_c/$threshold_c*100);
    $risk = $risk_a > $risk_c ? $risk_a : $risk_c;
    if ($risk > 500) {
        $icon = 'marker.png';
    } elseif ($risk > 300) {
        $icon = 'marker-gold.png';
    } elseif ($risk > 100) {
        $icon = 'marker-gold.png';
    } else {
        $icon = 'marker-green.png';
    }
    return $icon; 
}

$conf = $GLOBALS['CONF'];
$conf_threshold = $conf->get_conf('threshold');
////////////////////////////////////////////////////////////////
// Hosts
////////////////////////////////////////////////////////////////
$range = 'day';
$sql = "SELECT
            host.ip,
            host.hostname,
            host.descr,
            host.threshold_a,
            host.threshold_c,
            host.lon,
            host.lat,
            host_qualification.compromise,
            host_qualification.attack
        FROM
            host
        LEFT JOIN
            host_qualification
        ON
            host.ip = host_qualification.host_ip
        WHERE
            host.lon <> 0 AND
            host.lat <> 0";

if (!$rs = &$conn->Execute($sql)) {
    die($conn->ErrorMsg());
}
$hosts = array();
while (!$rs->EOF) {
    $ip = $rs->fields['ip'];
    
    // threshold inheritance
    // XXX TODO: missing the network inheritance provided by the metrics panel 
    $threshold_a = $rs->fields['threshold_a'] ? $rs->fields['threshold_a'] : $conf_threshold;
    $threshold_c = $rs->fields['threshold_c'] ? $rs->fields['threshold_c'] : $conf_threshold;
        
   $name = Host::ip2hostname($conn, $ip);
   $hosts[$ip] = array(
                        'name'        => $name,
                        'descr'       => $rs->fields['descr'],
                        'threshold_a' => $threshold_a,
                        'threshold_c' => $threshold_c,
                        'current_a'   => $rs->fields['compromise'] ? $rs->fields['compromise'] : 0,
                        'current_c'   => $rs->fields['attack'] ? $rs->fields['attack'] : 0,
                        'lon'         => $rs->fields['lon'],
                        'lat'         => $rs->fields['lat']
                    );

    $rs->MoveNext();
}
?>
<html>
<body>
    <style type="text/css">  
    #map {
            width: 512px;
            height: 450px;
            border: 1px solid gray;
        }
    </style>
    
    <script src="../js/prototype.js" type="text/javascript"></script>
    <? if ($opts['map_type'] == 'virtual_earth') { ?>
        <script src='http://dev.virtualearth.net/mapcontrol/v3/mapcontrol.js'></script>
    <? } ?>
    <script src="../js/OpenLayers/OpenLayers.js"></script>
    <script type="text/javascript">
        <!--
        var zoom = <?=$opts['zoom']?>;
        var lat = <?=$opts['lat']?>;
        var lon = <?=$opts['lon']?>;
        var map, markers;
        var position_lat, position_lon;

        function viewCenter()
        {
            var lonlat = map.getCenter();
            $('info-center-lat').innerHTML = lonlat.lon;
            $('info-center-lon').innerHTML = lonlat.lat;
        }

        function createMarker(lat, lon, icon)
        {
            var size = new OpenLayers.Size(21,25);
            var offset = new OpenLayers.Pixel(-(size.w/2), -size.h);
            var icon = new OpenLayers.Icon('../js/OpenLayers/img/'+icon, size, offset);

            var marker = new OpenLayers.Marker(new OpenLayers.LonLat(lon, lat),icon);
            /*
            marker.events.register('click', marker,
                function (evt) {
                    alert(this.icon.url);
                    Event.stop(evt);
                });
            */
            markers.addMarker(marker);
        }

        function acceptPosition()
        {
            opener.document.forms[0].lat.value = position_lat;
            opener.document.forms[0].lon.value = position_lon;
            window.close();
        }

        function init(){
            map = new OpenLayers.Map('map' <?=$opts['controls'] ? '' : ',{controls: [] }'?>);

            var options = {
                           resolutions: [1, 0.5, 0.3],
                           maxResolution: 'auto',
                           numZoomLevels: 4
                          };

            /*
            // http://www.nanog.org/mtg-0402/gif/map.jpg
            var graphic = new OpenLayers.Layer.Image(
                                'Custom Image',
                                'http://www.nanog.org/mtg-0402/gif/map.jpg',
                                new OpenLayers.Bounds(-180, -90, 90, 180),
                                new OpenLayers.Size(473, 624),
                                options);
            map.addLayer(graphic);
            //map.zoomToMaxExtent();

            //*/

         <? if ($opts['map_type'] == 'virtual_earth') { ?>
            velayer = new OpenLayers.Layer.VirtualEarth(
                                "VE",
                                {'type': VEMapStyle.Road, <?=$zoom_js?>});
            map.addLayer(velayer);
         <? } ?>

            /*
            var yahoo = new OpenLayers.Layer.Yahoo("Yahoo");
            map.addLayer(yahoo);
            //*/

            map.setCenter(new OpenLayers.LonLat(lat, lon), zoom);

            /*
            map.events.register("mousemove", map, function(e) {
                var lonlat = map.getLonLatFromViewPortPx(e.xy);
                $('info-lon').innerHTML = lonlat.lon;
                $('info-lat').innerHTML = lonlat.lat;
            });

            map.events.register("click", map, function(e) {
                var lonlat = map.getLonLatFromViewPortPx(e.xy);
                $('lon').value = lonlat.lon;
                $('lat').value = lonlat.lat;
            });
            map.events.register("zoomend", map, function(e) {
                $('info-zoom').innerHTML = map.getZoom();
            });
            map.events.register("move", map, function(e) {
                viewCenter();
                markers.redraw();
            });
            
            $('info-zoom').innerHTML = map.getZoom();
            */
         <? if ($status == 'edit') { ?>
            map.events.register("zoomend", map, function(e) {
                $('info-zoom').innerHTML = map.getZoom();
            });
            map.events.register("move", map, function(e) {
                viewCenter();
            });
        <? } ?>
            map.events.register("click", map, function(e) {
                var lonlat = map.getLonLatFromViewPortPx(e.xy);
                $('lon').innerHTML = lonlat.lon;
                position_lon = lonlat.lon;
                $('lat').innerHTML = lonlat.lat;
                position_lat = lonlat.lat;
            });
         <? if ($status != 'set_marker_pos') { ?>
            markers = new OpenLayers.Layer.Markers( "Markers" );
            map.addLayer(markers);
            <? foreach ($hosts as $ip => $data) {
                   $icon = get_icon($data['current_a'], $data['threshold_a'], $data['current_c'], $data['threshold_c']);    
            ?>
                   createMarker(<?=$data['lat']?>, <?=$data['lon']?>, '<?=$icon?>');
            <? } ?>
            markers.redraw();
         <? } ?>

        }
        // -->
    </script><div <?= $status != 'edit' ? 'style="display: none"' : ''?>>
    Zoom: <span id="info-zoom"></span><br>
    Center at: Lat: <span id="info-center-lat"></span> Lon: <span id="info-center-lon"></span><br></div>
  <? if ($status == 'set_marker_pos') { ?>
    <?=_("Click on the map to set the position of the host")?><br>
    Lat: <span id="lat"></span> Lon: <span id="lon"></span> <a href="#" onClick="javascript: acceptPosition();"><?=_("Accept")?></a><br>
  <? } ?>     
    <div id="errors"></div>
    <div id="map"></div>
    <script>init(); $('info-zoom').innerHTML = map.getZoom(); viewCenter();</script>
</body></html>