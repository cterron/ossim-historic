<?php

function acidIP2long($IP_str)
{
   $tmp_long = ip2long($IP_str);
   if ( $tmp_long < 0 )
      $tmp_long = 4294967296 - abs($tmp_long);
                                                                                                                                                 
   return $tmp_long;
}

function acidLong2IP($long_IP)
{
   $tmp_IP = $long_IP;
   if ( $long_IP > 2147483647 )
   {
      $tmp_IP = 4294967296 -  $tmp_IP;
      $tmp_IP = $tmp_IP * (-1);
   }
                                                                                                                                                 
   $tmp_IP = long2ip($tmp_IP);
   return $tmp_IP;
}

function get_acid_info($ip, $acid_link, $acid_prefix) {
    return "$acid_link/".$acid_prefix."_qry_main.php?new=2&num_result_rows=-1&submit=Query+DB&current_view=-1&ip_addr_cnt=2&ip_addr%5B0%5D%5B0%5D=+&ip_addr%5B0%5D%5B1%5D=ip_src&ip_addr%5B0%5D%5B2%5D=%3D&ip_addr%5B0%5D%5B3%5D=$ip&ip_addr%5B0%5D%5B8%5D=+&ip_addr%5B0%5D%5B9%5D=OR&ip_addr%5B1%5D%5B0%5D=+&ip_addr%5B1%5D%5B1%5D=ip_dst&ip_addr%5B1%5D%5B2%5D=%3D&ip_addr%5B1%5D%5B3%5D=$ip&ip_addr%5B1%5D%5B8%5D=+&ip_addr%5B1%5D%5B9%5D=+&sort_order=time_d";
}

?>
