<?php

require_once ('ossim_db.inc');
require_once ('ossim_conf.inc');
require_once ('classes/Conf.inc');
require_once ('classes/Graph.inc');
require_once ('classes/Host.inc');
require_once ('classes/Host_qualification.inc');
require_once ('classes/Host_position.inc');
require_once ('classes/Link.inc');


function generate_main (){

$db = new ossim_db();
$ossim_conf = new ossim_conf();
$conn = $db->connect();
$base = $ossim_conf->get_conf("ossim_base");

if ($graph_id) {
    $file = fopen ("VGJ-applet/data/example$graph_id.html", "w");
    $id_list[] = $graph_id;
} else {
    $file = fopen ("VGJ/data/example.html", "w");
    $id_list = Graph::get_id_list($conn);
    if (count($id_list) == 0) exit();
}
foreach ($id_list as $id) {
//print "$id<BR>";
}
exit();

if(!$file){exit();}

$conf = Conf::get_conf($conn);

fwrite($file, "graph
[
    directed 1
");
$DEFAULT_threshold= $conf->get_threshold();
$var_l = 1; 
$var_x_pri = -100;
$var_y_pri = 0;
$var_z_pri = 0;
$dk_x = 0;
$dk_y = 0;
$dk_z = 0;
$x_values[] = 0;
$y_values[] = 0;
$z_values[] = 0;
$new_ids[] = 0;
$var_x = 0;
$var_y = 0;
$var_z = 0;
foreach ($id_list as $id) {
    $link_list = Graph::get_list($conn, "WHERE id = $id");
    $var_z += 200;
    $var_x_pri += 100;
    $var_z_pri = $var_z;
    if(!$link_list){
        $id++;
        rewind($file);
        fwrite($file,"cerrar[]");
        fclose($file);
    }
    foreach ($link_list as $link) {
        $ip = $link->get_ip();
        $got_name = 0;
        $colour = 0;
        $threshold = 0;
        $max_estad = 0;
        $temp_a = 0;
        $temp_c = 0;
       
        /* get host names */
        if($hostname = Host::ip2hostname($conn, $ip)){
            $threshold = Host::ipthresh_c($conn, $ip);
            $got_name = 1;
        }else{
            $hostname = $ip;
            $threshold = $DEFAULT_threshold;
        }

        $temp_a = Host_qualification::get_ip_attack($conn, $ip);
        $temp_c = Host_qualification::get_ip_compromise($conn, $ip);

        if($temp_a > $temp_c){
            $max_estad = $temp_a;
        } else {
            $max_estad = $temp_c;
        }
    }
    if($max_estad < ($threshold/2)){
        $colour = "green";
    } elseif ($max_estad < $threshold){
        $colour = "orange";
    } else {
        $colour = "red";
    }
        //echo "$hostname - $threshold - $max_estad - $colour<BR>";
    fwrite($file,"
node [
id $var_l
label \"$hostname\"
graphics [
    Image [
    ");
    $modded = 0;
    if($got_name == 1){
        $query = "SELECT * FROM host_position WHERE host_ip = '$ip';";
        $resultado = mysql_db_query($base, $query, $conn);
        if($row3 = mysql_fetch_array($resultado)){
            $var_x_pri = $row3[1];
            $var_y_pri = $row3[2];
            $var_z_pri = $row3[3];
            while(in_array($var_x_pri,$x_values)){
                $modded = 1;
                $var_x_pri +=100;
            }
            array_push($x_values,$var_x_pri);
            array_push($y_values,$var_y_pri);
            array_push($z_values,$var_z_pri);
            Host_position::update($conn, $ip, $var_x_pri, $var_y_pri, $var_z_pri);
        } else {
            $var_x_pri +=100;
            while(in_array($var_x_pri,$x_values)){
                $var_x_pri +=100;
            }
            $var_y_pri = rand(-80,80);
            array_push($new_ids,$var_l);
            Host_position::insert($conn, $ip, $var_x_pri, $var_y_pri, $var_z_pri);
        }
        if($colour == "green"){
            fwrite($file,"
        Type \"URL\"
        Location \"green-ball.gif\"
      ]
      ");
        } elseif ($colour == "orange"){
            fwrite($file,"
        Type \"URL\"
        Location \"orange-ball.gif\"
      ]
      ");
        } elseif ($colour == "red"){
            fwrite($file,"
        Type \"URL\"
        Location \"red-ball.gif\"
      ]
     ");
        } else {
            fwrite($file,"
        Type \"\"
        Location \"\"
      ]
     ");
        }
        fwrite($file,"
    center [
    x $var_x_pri
    y $var_y_pri
    z $var_z_pri
    ]
    ");
    } else {
        // No name
        $query = "SELECT * FROM host_position WHERE host_ip = '$ip';";
        $resultado2 = mysql_db_query($base, $query, $conn);
        if($row4 = mysql_fetch_array($resultado2)){
            // Esta en BBDD
            $var_x = $row4[1];
            $var_y = $row4[2];
            $var_z = $row4[3];
        } else {
            $var_x = $var_x_pri + rand(-25,25);
            if(rand(0,10) >= 5){
                $var_y = $var_y_pri + rand(150,600);
            } else {
                $var_y = $var_y_pri - rand(150,600);
            }
            $var_z = $var_z_pri;
            array_push($new_ids,$var_l);
            Host_position::insert($conn, $ip, $var_x, $var_y, $var_z);
        }
        if($colour == "green"){
            fwrite($file,"
        Type \"URL\"
        Location \"green-ball.gif\"
      ]
      ");
        } elseif ($colour == "orange"){
            fwrite($file,"
        Type \"URL\"
        Location \"orange-ball.gif\"
      ]
      ");
        } elseif ($colour == "red"){
            fwrite($file,"
        Type \"URL\"
        Location \"red-ball.gif\"
      ]
     ");
        } else {
            fwrite($file,"
        Type \"\"
        Location \"\"
      ]
     ");
        }
        fwrite($file,"
    center [
    x $var_x
    y $var_y
    z $var_z
    ]
    ");
    }
    fwrite($file,"
      width 35 
    height 35
    depth 35
    ]
    vgj [
    labelPosition \"below\"
    shape \"Oval\"
    ]
 ]
");
    $all[$hostname] = $var_l; 
    $var_l++;
}
    $link_ist = Graph::get_link_list($conn, $id);

    foreach($link_list as $link){
        $source = $link->get_source(); 
        $dest = $link->get_dest(); 
        $occurrences = $link->get_occurrences();
       
        /* get host names */
        if (!$hostnameSource = Host::ip2hostname($conn, $source)) {
            $hostnameSource = $source;
        }
        if (!$hostnameDest = Host::ip2hostname($conn, $dest)){
            $hostnameDest = $dest;
        }

        $id_source = $all[$hostnameSource];
        $id_dest = $all[$hostnameDest];
//label \"$occurrences\"
fwrite($file,"
edge [
");
if(in_array($id_source,$new_ids)){
fwrite($file,"
linestyle \"dashdot\"
");
} elseif($occurrences >= 10) {
fwrite($file,"
linestyle \"dashed\"
");
} else {
fwrite($file,"
linestyle \"solid\"
");
//echo "$hostnameSource -> $hostnameDest<BR>";
}


fwrite($file,"
label \"\"
source $id_source
target $id_dest
]
");
    }
//}
fwrite($file,"
]
");
fclose($file);
}


function generate ($graph_id){

$conn = $db->connect();

if ($graph_id) {
    $file = fopen ("VGJ-applet/data/example$graph_id.html", "w");
    $id_list[] = $graph_id;
} else {
    $file = fopen ("VGJ/data/example.html", "w");
    $query = "SELECT DISTINCT id FROM grafos;";
    $res = mysql_db_query($base, $query, $conn);
    while ($row = mysql_fetch_array($res)) {
        $id_list[] = $row["id"];
    }
    if (count($id_list) == 0) exit();
}

if(!$file){exit();}
fwrite($file, "graph
[
    directed 1
");
$res_thres= mysql_db_query($base, "SELECT * FROM conf", $conn);
if($row_thres = mysql_fetch_array($res_thres)){
$DEFAULT_threshold = $row_thres["threshold"];
}
$var_l = 1; 
$var_x_pri = -100;
$var_y_pri = 0;
$var_z_pri = 0;
$x_values[] = 0;
$y_values[] = 0;
$z_values[] = 0;
$new_ids[] = 0;
$var_x = 0;
$var_y = 0;
$var_z = 0;
foreach ($id_list as $id) {
    $query = "SELECT DISTINCT ip FROM grafos WHERE id = '$id';";
    $res = mysql_db_query($base, $query, $conn);
    $var_z += 200;
    $var_x_pri += 100;
    $var_z_pri = $var_z;
    if(!mysql_num_rows($res)){
    $id++;
    rewind($file);
    fwrite($file,"cerrar[]");
    fclose($file);
    }
    while ($row = mysql_fetch_array($res)) {
    
        $ip = $row["ip"];
        $got_name = 0;
        $colour = 0;
        $threshold = 0;
        $max_estad = 0;
       
        /* get host names */
        $query = "select * from ips where ip = '". $ip . "';";
        $res2 = mysql_db_query($base, $query, $conn);
        if ($row2 = mysql_fetch_array($res2)) {
            $hostname = $row2["nombre"];
            $threshold = $row2["threshold_c"];
            $got_name = 1;
        }else{
            $hostname = $ip;
            $threshold = $DEFAULT_threshold;
        }
        $query = "select * from estadisticas where ip = '$ip'";
        $res_estad = mysql_db_query($base, $query, $conn);
        if($row_estad = mysql_fetch_array($res_estad)){
            if($row_estad[1] > $row_estad[2]){
                $max_estad = $row_estad[1];
            } else {
                $max_estad = $row_estad[2];
            }
        }
        if($max_estad < ($threshold/2)){
        $colour = "green";
        } elseif ($max_estad < $threshold){
        $colour = "orange";
        } else {
        $colour = "red";
        }
        //echo "$hostname - $threshold - $max_estad - $colour<BR>";
fwrite($file,"
node [
id $var_l
label \"$hostname\"
graphics [
    Image [
    ");
    $modded = 0;
    if($got_name == 1){
    $query = "SELECT * FROM graficas WHERE nombre = '$ip';";
    $resultado = mysql_db_query($base, $query, $conn);
    if($row3 = mysql_fetch_array($resultado)){
    $var_x_pri = $row3[1];
    $var_y_pri = $row3[2];
    $var_z_pri = $row3[3];
    while(in_array($var_x_pri,$x_values)){
    $modded = 1;
    $var_x_pri +=100;
    }
    array_push($x_values,$var_x_pri);
    array_push($y_values,$var_y_pri);
    array_push($z_values,$var_z_pri);
    $query = "UPDATE graficas set x=$var_x_pri,y=$var_y_pri,z=$var_z_pri WHERE
    nombre = '$ip';";
    $res4 = mysql_db_query($base, $query, $conn);
    } else {
    $var_x_pri +=100;
    while(in_array($var_x_pri,$x_values)){
    $var_x_pri +=100;
    }
    $var_y_pri = rand(-30,30);
    array_push($new_ids,$var_l);
    $query = "INSERT INTO graficas values
    (\"$ip\",$var_x_pri,$var_y_pri,$var_z_pri);";
    mysql_db_query($base, $query, $conn);
    }
    if($colour == "green"){
    fwrite($file,"
        Type \"URL\"
        Location \"green-ball.gif\"
      ]
      ");
    } elseif ($colour == "orange"){
    fwrite($file,"
        Type \"URL\"
        Location \"orange-ball.gif\"
      ]
      ");
    } elseif ($colour == "red"){
    fwrite($file,"
        Type \"URL\"
        Location \"red-ball.gif\"
      ]
     ");
    } else {
    fwrite($file,"
        Type \"\"
        Location \"\"
      ]
     ");
     }
    fwrite($file,"
    center [
    x $var_x_pri
    y $var_y_pri
    z $var_z_pri
    ]
    ");} else {
    // No tiene nombre
    $query = "SELECT * FROM graficas WHERE nombre = '$ip';";
    $resultado2 = mysql_db_query($base, $query, $conn);
    if($row4 = mysql_fetch_array($resultado2)){
    // Esta en BBDD
    $var_x = $row4[1];
    $var_y = $row4[2];
    $var_z = $row4[3];
    } else {
    $var_x = $var_x_pri + rand(-25,25);
    if(rand(0,10) >= 5){
    $var_y = $var_y_pri + rand(100,550);
    } else {
    $var_y = $var_y_pri - rand(100,550);
    }
    $var_z = $var_z_pri;
    array_push($new_ids,$var_l);
    $query = "INSERT INTO graficas values (\"$ip\",$var_x,$var_y,$var_z);";
    mysql_db_query($base, $query, $conn);
    }
    if($colour == "green"){
    fwrite($file,"
        Type \"URL\"
        Location \"green-ball.gif\"
      ]
      ");
    } elseif ($colour == "orange"){
    fwrite($file,"
        Type \"URL\"
        Location \"orange-ball.gif\"
      ]
      ");
    } elseif ($colour == "red"){
    fwrite($file,"
        Type \"URL\"
        Location \"red-ball.gif\"
      ]
     ");
    } else {
    fwrite($file,"
        Type \"\"
        Location \"\"
      ]
     ");
     }
    fwrite($file,"
    center [
    x $var_x
    y $var_y
    z $var_z
    ]
    ");
    }
        fwrite($file,"
      width 35 
    height 35
    depth 35
    ]
    vgj [
    labelPosition \"below\"
    shape \"Oval\"
    ]
 ]
");
$all[$hostname] = $var_l; 
$var_l++;
    }
    $query = "SELECT DISTINCT e.origen, e.destino, e.ocurrencias 
        FROM enlaces e, grafos g where g.id = $id and 
        (g.ip = e.origen or g.ip = e.destino) order by e.ocurrencias desc;";
    $res = mysql_db_query($base, $query, $conn);

    while ($row = mysql_fetch_array($res)) {   
        $source = $row["origen"];
        $dest = $row["destino"];
        $occurrences = $row["ocurrencias"];
       
        /* get host names */
        $query = "select * from ips where ip = '". $source . "';";
        $res2 = mysql_db_query($base, $query, $conn);
        if ($row2 = mysql_fetch_array($res2)) {
            $hostnameSource = $row2["nombre"];
        }else{
            $hostnameSource = $source;
        }
        $query = "select * from ips where ip = '". $dest . "';";
        $res3 = mysql_db_query($base, $query, $conn);
        if ($row3 = mysql_fetch_array($res3)) {
            $hostnameDest = $row3["nombre"];
        }else{
            $hostnameDest = $dest;
        }
        $id_source = $all[$hostnameSource];
        $id_dest = $all[$hostnameDest];
//label \"$occurrences\"
fwrite($file,"
edge [
");
if(in_array($id_source,$new_ids)){
fwrite($file,"
linestyle \"dashdot\"
");
} elseif($occurrences >= 10) {
fwrite($file,"
linestyle \"dashed\"
");
} else {
fwrite($file,"
linestyle \"solid\"
");
//echo "$hostnameSource -> $hostnameDest<BR>";
}


fwrite($file,"
label \"\"
source $id_source
target $id_dest
]
");
    }
}
fwrite($file,"
]
");
fclose($file);
}
?>
