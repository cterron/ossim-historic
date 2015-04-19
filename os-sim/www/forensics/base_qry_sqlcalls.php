<?php
/**
* Class and Function List:
* Function list:
* Classes list:
*/
/*******************************************************************************
** OSSIM Forensics Console
** Copyright (C) 2009 OSSIM/AlienVault
** Copyright (C) 2004 BASE Project Team
** Copyright (C) 2000 Carnegie Mellon University
**
** (see the file 'base_main.php' for license details)
**
** Built upon work by Roman Danyliw <rdd@cert.org>, <roman@danyliw.com>
** Built upon work by the BASE Project Team <kjohnson@secureideas.net>
*/
include ("geoip.inc");
$gi = geoip_open("/usr/share/geoip/GeoIP.dat", GEOIP_STANDARD);
global $colored_alerts, $debug_mode;
/* **************** Run the Query ************************************************** */
/* base_ag_main.php will include this file
*  - imported variables: $sql, $cnt_sql
*/
if ($printing_ag) {
    ProcessCriteria();
    $page = "base_ag_main.php";
    $tmp_page_get = "&amp;ag_action=view&amp;ag_id=$ag_id&amp;submit=x";
    $sql = $save_sql;
} else {
    $page = "base_qry_main.php";
    $cnt_sql = "SELECT COUNT(acid_event.cid) FROM acid_event " . $join_sql . $where_sql . $criteria_sql;
    $tmp_page_get = "";
}
/* Run the query to determine the number of rows (No LIMIT)*/
//$qs->GetNumResultRows($cnt_sql, $db);
$et->Mark("Counting Result size");
/* Setup the Query Results Table */
$qro = new QueryResultsOutput("$page" . $qs->SaveStateGET() . $tmp_page_get);
$qro->AddTitle(qroReturnSelectALLCheck());
//$qro->AddTitle("ID");
$qro->AddTitle(_SIGNATURE, "sig_a", " ", " ORDER BY sig_name ASC", "sig_d", " ", " ORDER BY sig_name DESC");
$qro->AddTitle(_TIMESTAMP, "time_a", " ", " ORDER BY timestamp ASC ", "time_d", " ", " ORDER BY timestamp DESC ");
$qro->AddTitle(_NBSOURCEADDR, "sip_a", " ", " ORDER BY ip_src ASC", "sip_d", " ", " ORDER BY ip_src DESC");
$qro->AddTitle(_NBDESTADDR, "dip_a", " ", " ORDER BY ip_dst ASC", "dip_d", " ", " ORDER BY ip_dst DESC");
//$qro->AddTitle("Asst", "oasset_d_a", " ", " ORDER BY ossim_asset_dst ASC", "oasset_d_d", " ", " ORDER BY ossim_asset_dst DESC");
$qro->AddTitle("Asst", "oasset_s_a", " ", " ORDER BY ossim_asset_src ASC", "oasset_s_d", " ", " ORDER BY ossim_asset_src DESC", "oasset_d_a", " ", " ORDER BY ossim_asset_dst ASC", "oasset_d_d", " ", " ORDER BY ossim_asset_dst DESC");
$qro->AddTitle("Prio", "oprio_a", " ", " ORDER BY ossim_priority ASC", "oprio_d", " ", " ORDER BY ossim_priority DESC");
$qro->AddTitle("Rel", "oreli_a", " ", " ORDER BY ossim_reliability ASC", "oreli_d", " ", " ORDER BY ossim_reliability DESC");
//$qro->AddTitle("Risk", "oriska_a", " ", " ORDER BY ossim_risk_a ASC", "oriska_d", " ", " ORDER BY ossim_risk_a DESC");
$qro->AddTitle("Risk", "oriska_a", " ", " ORDER BY ossim_risk_c ASC", "oriska_d", " ", " ORDER BY ossim_risk_c DESC", "oriskd_a", " ", " ORDER BY ossim_risk_a ASC", "oriskd_d", " ", " ORDER BY ossim_risk_a DESC");
$qro->AddTitle("L4-proto", "proto_a", " ", " ORDER BY ip_proto ASC", "proto_d", " ", " ORDER BY ip_proto DESC");
/* Apply sort criteria */
if ($qs->isCannedQuery()) $sort_sql = " ORDER BY timestamp DESC ";
else {
    $sort_sql = $qro->GetSortSQL($qs->GetCurrentSort() , $qs->GetCurrentCannedQuerySort());
    //  3/23/05 BDB   mods to make sort by work for Searches
    $sort_sql = "";
    if (!isset($sort_order)) {
        $sort_order = NULL;
    }
    if ($sort_order == "sip_a") {
        $sort_sql = " ORDER BY ip_src ASC";
        $criteria_sql = str_replace("1  AND ( timestamp", "ip_src >= 0 AND ( timestamp", $criteria_sql);
    } elseif ($sort_order == "sip_d") {
        $sort_sql = " ORDER BY ip_src DESC";
        $criteria_sql = preg_replace("/1  AND \( timestamp/", "ip_src >= 0 AND ( timestamp", $criteria_sql);
    } elseif ($sort_order == "dip_a") {
        $sort_sql = " ORDER BY ip_dst ASC";
        $criteria_sql = preg_replace("/1  AND \( timestamp/", "ip_dst >= 0 AND ( timestamp", $criteria_sql);
    } elseif ($sort_order == "dip_d") {
        $sort_sql = " ORDER BY ip_dst DESC";
        $criteria_sql = preg_replace("/1  AND \( timestamp/", "ip_dst >= 0 AND ( timestamp", $criteria_sql);
    } elseif ($sort_order == "sig_a") {
        $sort_sql = " ORDER BY sig_name ASC";
    } elseif ($sort_order == "sig_d") {
        $sort_sql = " ORDER BY sig_name DESC";
    } elseif ($sort_order == "time_a") {
        $sort_sql = " ORDER BY timestamp ASC";
    } elseif ($sort_order == "time_d") {
        $sort_sql = " ORDER BY timestamp DESC";
    } elseif ($sort_order == "oasset_d_a") {
        $sort_sql = " ORDER BY ossim_asset_dst ASC";
    } elseif ($sort_order == "oasset_d_d") {
        $sort_sql = " ORDER BY ossim_asset_dst DESC";
    } elseif ($sort_order == "oprio_a") {
        $sort_sql = " ORDER BY ossim_priority ASC";
    } elseif ($sort_order == "oprio_d") {
        $sort_sql = " ORDER BY ossim_priority DESC";
    } elseif ($sort_order == "oriska_a") {
        $sort_sql = " ORDER BY ossim_risk_c ASC";
    } elseif ($sort_order == "oriska_d") {
        $sort_sql = " ORDER BY ossim_risk_c DESC";
    } elseif ($sort_order == "oriskd_a") {
        $sort_sql = " ORDER BY ossim_risk_a ASC";
    } elseif ($sort_order == "oriskd_d") {
        $sort_sql = " ORDER BY ossim_risk_a DESC";
    } elseif ($sort_order == "oreli_a") {
        $sort_sql = " ORDER BY ossim_reliability ASC";
    } elseif ($sort_order == "oreli_d") {
        $sort_sql = " ORDER BY ossim_reliability DESC";
    } elseif ($sort_order == "proto_a") {
        $sort_sql = " ORDER BY ip_proto ASC";
        $criteria_sql = preg_replace("/1  AND \( timestamp/", "ip_proto > 0 AND ( timestamp", $criteria_sql);
    } elseif ($sort_order == "proto_d") {
        $sort_sql = " ORDER BY ip_proto DESC";
        $criteria_sql = preg_replace("/1  AND \( timestamp/", "ip_proto > 0 AND ( timestamp", $criteria_sql);
    }
    ExportHTTPVar("prev_sort_order", $sort_order);
}
// Choose the correct INDEX for select
if (preg_match("/^time/", $sort_order)) $sql.= " FORCE INDEX (timestamp)";
elseif (preg_match("/^sip/", $sort_order)) $sql.= " FORCE INDEX (ip_src)";
elseif (preg_match("/^dip/", $sort_order)) $sql.= " FORCE INDEX (ip_dst)";
elseif (preg_match("/^sig/", $sort_order)) $sql.= " FORCE INDEX (sig_name)";
elseif (preg_match("/^oasset/", $sort_order)) $sql.= " FORCE INDEX (acid_event_ossim_asset_dst)";
elseif (preg_match("/^oprio/", $sort_order)) $sql.= " FORCE INDEX (acid_event_ossim_priority)";
elseif (preg_match("/^oriska/", $sort_order)) $sql.= " FORCE INDEX (acid_event_ossim_risk_a)";
elseif (preg_match("/^oriskd/", $sort_order)) $sql.= " FORCE INDEX (acid_event_ossim_risk_c)";
elseif (preg_match("/^oreli/", $sort_order)) $sql.= " FORCE INDEX (acid_event_ossim_reliability)";
elseif (preg_match("/^proto/", $sort_order)) $sql.= " FORCE INDEX (ip_proto)";
// Make SQL string with criterias
if (!$printing_ag) $sql = $sql . $join_sql . $where_sql . $criteria_sql . $sort_sql;
if ($debug_mode > 0) {
    echo "<P>SUBMIT: $submit";
    echo "<P>sort_order: $sort_order";
    echo "<P>SQL (save_sql): $sql";
    echo "<P>SQL (sort_sql): $sort_sql";
}
/* Run the Query again for the actual data (with the LIMIT) */
//$result = ""; // $qs->ExecuteOutputQuery($sql, $db);
$result = $qs->ExecuteOutputQuery($sql, $db);
$et->Mark("Retrieve Query Data");
// Optimization UPDATE using SQL_CALC_FOUND_ROWS (2/02/2009 - Granada)
$qs->GetCalcFoundRows($cnt_sql, $db);
if ($debug_mode > 0) {
    $qs->PrintCannedQueryList();
    $qs->DumpState();
    echo "$sql<BR>";
}
if (!$printing_ag) {
    /* ***** Generate and print the criteria in human readable form */
    echo '<TABLE WIDTH="100%">
           <TR>
             <TD VALIGN=TOP>';
    if (!array_key_exists("minimal_view", $_GET)) {
        PrintCriteria($caller);
    }
    echo '</TD></tr><tr>
           <TD VALIGN=TOP>';
    if (!array_key_exists("minimal_view", $_GET)) {
        PrintFramedBoxHeader(_QSCSUMM, "#669999", "#FFFFFF");
        PrintGeneralStats($db, 1, $show_summary_stats, "$join_sql ", "$where_sql $criteria_sql");
    }
    PrintFramedBoxFooter();
    echo ' </TD>
           </TR>
          </TABLE>
		  <!-- END HEADER TABLE -->
		  
		  </div> </TD>
           </TR>
          </TABLE>';
}
/* Clear the old checked positions */
for ($i = 0; $i < $show_rows; $i++) {
    $action_lst[$i] = "";
    $action_chk_lst[$i] = "";
}
// time selection for graph x
$tr = ($_SESSION["time_range"] != "") ? $_SESSION["time_range"] : "all";
switch ($tr) {
    case "today":
        $interval = "hour(timestamp) as intervalo, 'h' as suf";
        $grpby = " GROUP BY intervalo,suf";
        break;

    case "day":
        $interval = "hour(timestamp) as intervalo, day(timestamp) as suf";
        $grpby = " GROUP BY intervalo,suf";
        break;

    case "week":
    case "weeks":
        $interval = "day(timestamp) as intervalo, monthname(timestamp) as suf";
        $grpby = " GROUP BY intervalo,suf ORDER BY suf,intervalo";
        break;

    case "month":
        $interval = "day(timestamp) as intervalo, monthname(timestamp) as suf";
        $grpby = " GROUP BY intervalo,suf ORDER BY suf,intervalo";
        break;

    default:
        $interval = "monthname(timestamp) as intervalo, year(timestamp) as suf";
        $grpby = " GROUP BY intervalo,suf ORDER BY suf,intervalo";
}
$sqlgraph = "SELECT COUNT(acid_event.cid) as num_events, $interval FROM acid_event " . $join_sql . $where_sql . $criteria_sql . $grpby;
//echo $sqlgraph."<br>";
/* Print the current view number and # of rows */
$qs->PrintResultCnt($sqlgraph, $tr);
// COLUMNS of Events Table (with ORDER links)
$qro->PrintHeader();
$i = 0;
while (($myrow = $result->baseFetchRow()) && ($i < $qs->GetDisplayRowCnt())) {
    $current_sip32 = $myrow[4];
    $current_sip = baseLong2IP($current_sip32);
    $current_dip32 = $myrow[5];
    $current_dip = baseLong2IP($current_dip32);
    $current_proto = $myrow[6];
    $current_sport = $current_dport = "";
    if ($myrow[14] != 0) $current_sport = ":" . $myrow[14];
    if ($myrow[15] != 0) $current_dport = ":" . $myrow[15];
    if ($debug_mode > 1) {
        SQLTraceLog("\n\n");
        SQLTraceLog(__FILE__ . ":" . __LINE__ . ":\n############## <calls to BuildSigByID> ##################");
    }
    $current_sig = BuildSigByID($myrow[2], $db);
    $current_sig_txt = BuildSigByID($myrow[2], $db, 2);
    if ($debug_mode > 1) {
        SQLTraceLog(__FILE__ . ":" . __LINE__ . ":\n################ </calls to BuildSigByID> ###############");
        SQLTraceLog("\n\n");
    }
    $current_otype = $myrow[7];
    $current_oprio = $myrow[8];
    $current_oreli = $myrow[9];
    $current_oasset_s = $myrow[10];
    $current_oasset_d = $myrow[11];
    $current_oriskc = $myrow[12];
    $current_oriska = $myrow[13];
    if ($portscan_payload_in_signature == 1) {
        /* fetch from payload portscan open port number */
        if (stristr($current_sig_txt, "(portscan) Open Port")) {
            $sql2 = "SELECT data_payload FROM data WHERE sid='" . $myrow[0] . "' AND cid='" . $myrow[1] . "'";
            $result2 = $db->baseExecute($sql2);
            $myrow_payload = $result2->baseFetchRow();
            $result2->baseFreeRows();
            $myrow_payload = PrintCleanHexPacketPayload($myrow_payload[0], 2);
            $current_sig = $current_sig . str_replace("Open Port", "", $myrow_payload);
        }
        /* fetch from payload portscan port range */
        else if (stristr($current_sig_txt, "(portscan) TCP Portscan") || stristr($current_sig_txt, "(portscan) UDP Portscan")) {
            $sql2 = "SELECT data_payload FROM data WHERE sid='" . $myrow[0] . "' AND cid='" . $myrow[1] . "'";
            $result2 = $db->baseExecute($sql2);
            $myrow_payload = $result2->baseFetchRow();
            $result2->baseFreeRows();
            $myrow_payload = PrintCleanHexPacketPayload($myrow_payload[0], 2);
            $current_sig = $current_sig . stristr(stristr($myrow_payload, "Port/Proto Range") , ": ");
        }
    }
    $current_sig = GetTagTriger($current_sig, $db, $myrow[0], $myrow[1]);
    // ********************** EVENTS TABLE **********************
    // <TR>
    qroPrintEntryHeader((($colored_alerts == 1) ? GetSignaturePriority($myrow[2], $db) : $i) , $colored_alerts);
    $tmp_rowid = "#" . (($qs->GetCurrentView() * $show_rows) + $i) . "-(" . $myrow[0] . "-" . $myrow[1] . ")";
    // <TD>
    // 1- Checkbox
    qroPrintEntry('<INPUT TYPE="checkbox" NAME="action_chk_lst[' . $i . ']" VALUE="' . $tmp_rowid . '">');
    echo '    <INPUT TYPE="hidden" NAME="action_lst[' . $i . ']" VALUE="' . $tmp_rowid . '">';
    // 2- ID
    
    /** Fix for bug #1116034 -- Input by Tim Rupp, original solution and code by Alejandro Flores **/
    //$temp = "<A HREF='base_qry_alert.php?submit=".rawurlencode($tmp_rowid)."&amp;sort_order=";
    //$temp .= ($qs->isCannedQuery()) ? $qs->getCurrentCannedQuerySort() : $qs->getCurrentSort();
    //$temp .= "'>".$tmp_rowid."</a>";
    //qroPrintEntry($temp);
    //$temp = "";
    // 3- Signature
    $tmpsig = explode("]", $current_sig);
    //$temp = $tmpsig[0]."] <A HREF='base_qry_alert.php?submit=".rawurlencode($tmp_rowid)."&amp;sort_order=";
    $temp = "<A HREF='base_qry_alert.php?submit=" . rawurlencode($tmp_rowid) . "&amp;sort_order=";
    $temp.= ($qs->isCannedQuery()) ? $qs->getCurrentCannedQuerySort() : $qs->getCurrentSort();
    $temp.= "'>" . $tmpsig[1] . "</a>";
    qroPrintEntry($temp, "left");
    $temp = "";
    // 4- Timestamp
    qroPrintEntry($myrow[3], "", "", "nowrap");
    $tmp_iplookup = 'base_qry_main.php?sig%5B0%5D=%3D' . '&amp;num_result_rows=-1' . '&amp;time%5B0%5D%5B0%5D=+&amp;time%5B0%5D%5B1%5D=+' . '&amp;submit=' . _QUERYDBP . '&amp;current_view=-1&amp;ip_addr_cnt=2';
    /* TCP or UDP show the associated port #
    if ( ($current_proto == TCP) || ($current_proto == UDP) )
    $result4 = $db->baseExecute("SELECT layer4_sport, layer4_dport FROM acid_event ".
    "WHERE sid='".$myrow[0]."' AND cid='".$myrow[1]."'");
    
    if ( ($current_proto == TCP) || ($current_proto == UDP) )
    {
    $myrow4 = $result4->baseFetchRow();
    
    if ( $myrow4[0] != "" )  $current_sport = ":".$myrow4[0];
    if ( $myrow4[1] != "" )  $current_dport = ":".$myrow4[1];
    }
    */
    // 5- Source IP Address
    if ($current_sip32 != "") {
        $country = strtolower(geoip_country_code_by_addr($gi, $current_sip));
        $country_name = geoip_country_name_by_addr($gi, $current_sip);
        if ($country) {
            $country_img = " <img src=\"/ossim/pixmaps/flags/" . $country . ".png\" alt=\"$country_name\" title=\"$country_name\">";
        } else {
            $country_img = "";
        }
        $ip_aux = ($sensors[$current_sip] != "") ? $sensors[$current_sip] : (($hosts[$current_sip] != "") ? $hosts[$current_sip] : $current_sip);
        qroPrintEntry('<A HREF="base_stat_ipaddr.php?ip=' . $current_sip . '&amp;netmask=32">' . $ip_aux . '</A><FONT SIZE="-1">' . $current_sport . '</FONT>' . $country_img, 'center', 'top', 'nowrap');
    } else {
        /* if no IP address was found check if this is a spp_portscan message
        * and try to extract a source IP
        * - contrib: Michael Bell <michael.bell@web.de>
        */
        if (stristr($current_sig_txt, "portscan")) {
            $line = split(" ", $current_sig_txt);
            foreach($line as $ps_element) {
                if (ereg("[0-9]*\.[0-9]*\.[0-9]*\.[0-9]", $ps_element)) {
                    $ps_element = ereg_replace(":", "", $ps_element);
                    qroPrintEntry("<A HREF=\"base_stat_ipaddr.php?ip=" . $ps_element . "&amp;netmask=32\">" . $ps_element . "</A>");
                }
            }
        } else qroPrintEntry('<A HREF="' . $BASE_urlpath . '/help/base_app_faq.php#1">' . _UNKNOWN . '</A>');
    }
    // 6- Destination IP Address
    if ($current_dip32 != "") {
        $country = strtolower(geoip_country_code_by_addr($gi, $current_dip));
        $country_name = geoip_country_name_by_addr($gi, $current_dip);
        if ($country) {
            $country_img = " <img src=\"/ossim/pixmaps/flags/" . $country . ".png\" alt=\"$country_name\" title=\"$country_name\">";
        } else {
            $country_img = "";
        }
        $ip_aux = ($sensors[$current_dip] != "") ? $sensors[$current_dip] : (($hosts[$current_dip] != "") ? $hosts[$current_dip] : $current_dip);
        qroPrintEntry('<A HREF="base_stat_ipaddr.php?ip=' . $current_dip . '&amp;netmask32">' . $ip_aux . '</A><FONT SIZE="-1">' . $current_dport . '</FONT>' . $country_img, 'center', 'top', 'nowrap');
    } else qroPrintEntry('<A HREF="' . $BASE_urlpath . '/help/base_app_faq.php#1">' . _UNKNOWN . '</A>');

    // 7- Asset
    qroPrintEntry("<img src=\"bar.php?value=" . $current_oasset_s . "&value2=" . $current_oasset_d . "&max=5\" border='0' align='absmiddle'>&nbsp;");
    $current_orisk = ($current_dip != "255.255.255.255") ? $current_oriska : $current_oriskc;
    
   /*if ($current_dip != "255.255.255.255") {
        qroPrintEntry("<img src=\"bar.php?value=" . $current_oasset_d . "&max=5\" border='0' align='absmiddle'>&nbsp;");
        $current_orisk = $current_oriska;
    } else {
        qroPrintEntry("<img src=\"bar.php?value=" . $current_oasset_s . "&max=5\" border='0' align='absmiddle'>&nbsp;");
        $current_orisk = $current_oriskc;
    }*/

    // 8- Priority
    qroPrintEntry("<img src=\"bar.php?value=" . $current_oprio . "&max=5\" border='0' align='absmiddle'>&nbsp;");
    //if ($current_oprio != "")
    //	qroPrintEntry($current_oprio);
    //else
    //	qroPrintEntry("--");

    // 10- Rel
    qroPrintEntry("<img src=\"bar.php?value=" . $current_oreli . "&max=9\" border='0' align='absmiddle'>&nbsp;");
    //if ($current_oreli != "")
    //	qroPrintEntry($current_oreli);
    //else
    //	qroPrintEntry("--");

    // 9- Risk
    qroPrintEntry("<img src=\"bar.php?value=" . $current_oriskc . "&value2=" . $current_oriska . "&max=9&range=1\" border='0' align='absmiddle'>&nbsp;");
    /*if ($current_otype == 2) {
        qroPrintEntry("<img src=\"bar.php?value=" . $current_orisk . "&max=9&range=1\" border='0' align='absmiddle'>&nbsp;");
    } else {
        qroPrintEntry("<img src=\"bar.php?value=" . $current_orisk . "&max=9&range=1\" border='0' align='absmiddle'>&nbsp;");
    }*/
    // 11- Protocol
    qroPrintEntry('<FONT>' . IPProto2str($current_proto) . '</FONT>');
    qroPrintEntryFooter();
    $i++;
    /*if ( ($current_proto == 6) || ($current_proto == 17) )
    {
    $result4->baseFreeRows();
    $myrow4[0] = $myrow4[1] = "";
    }*/
}
$result->baseFreeRows();
$qro->PrintFooter();
$qs->PrintBrowseButtons();
$qs->PrintAlertActionButtons();
$et->PrintForensicsTiming();
geoip_close($gi);
?>








