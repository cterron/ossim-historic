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
include_once ("base_conf.php");
include ("vars_session.php");
include ("$BASE_path/includes/base_constants.inc.php");
include ("$BASE_path/includes/base_include.inc.php");
include_once ("$BASE_path/base_db_common.php");
include_once ("$BASE_path/base_qry_common.php");
include_once ("$BASE_path/base_stat_common.php");
($debug_time_mode >= 1) ? $et = new EventTiming($debug_time_mode) : '';
$cs = new CriteriaState("base_stat_alerts.php");
$submit = ImportHTTPVar("submit", VAR_ALPHA | VAR_SPACE, array(
    _SELECTED,
    _ALLONSCREEN,
    _ENTIREQUERY
));
$cs->ReadState();
// Check role out and redirect if needed -- Kevin
$roleneeded = 10000;
$BUser = new BaseUser();
if (($BUser->hasRole($roleneeded) == 0) && ($Use_Auth_System == 1)) base_header("Location: " . $BASE_urlpath . "/index.php");
$qs = new QueryState();
$qs->AddCannedQuery("most_frequent", $freq_num_alerts, _MOSTFREQALERTS, "occur_d");
$qs->AddCannedQuery("last_alerts", $last_num_ualerts, _LASTALERTS, "last_d");
$qs->MoveView($submit); /* increment the view if necessary */
$page_title = _ALERTTITLE;
if ($qs->isCannedQuery()) PrintBASESubHeader($page_title . ": " . $qs->GetCurrentCannedQueryDesc() , $page_title . ": " . $qs->GetCurrentCannedQueryDesc() , $cs->GetBackLink() , 1);
else PrintBASESubHeader($page_title, $page_title, $cs->GetBackLink() , 1);
/* Connect to the Alert database */
$db = NewBASEDBConnection($DBlib_path, $DBtype);
$db->baseDBConnect($db_connect_method, $alert_dbname, $alert_host, $alert_port, $alert_user, $alert_password);
if ($event_cache_auto_update == 1) UpdateAlertCache($db);
$criteria_clauses = ProcessCriteria();
if (!$printing_ag) {
    /* ***** Generate and print the criteria in human readable form */
    echo '<TABLE WIDTH="100%">
           <TR>
             <TD WIDTH="60%" VALIGN=TOP>';
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
		  
		  </div>  </TD>
           </TR>
          </TABLE>';
}
$from = " FROM acid_event  " . $criteria_clauses[0];
$where = ($criteria_clauses[1] != "") ? " WHERE " . $criteria_clauses[1] : " ";
// use accumulate tables only with timestamp criteria
$use_ac = (preg_match("/AND/", preg_replace("/AND \( timestamp/", "", $criteria_clauses[1]))) ? false : true;
if (preg_match("/^(.*)AND\s+\(\s+timestamp\s+[^']+'([^']+)'\s+\)\s+AND\s+\(\s+timestamp\s+[^']+'([^']+)'\s+\)(.*)$/", $where, $matches)) {
    if ($matches[2] != $matches[3]) {
        $where = $matches[1] . " AND timestamp BETWEEN('" . $matches[2] . "') AND ('" . $matches[3] . "') " . $matches[4];
    } else {
        $where = $matches[1] . " AND timestamp >= '" . $matches[2] . "' " . $matches[4];
    }
}
$qs->AddValidAction("ag_by_id");
$qs->AddValidAction("ag_by_name");
$qs->AddValidAction("add_new_ag");
$qs->AddValidAction("del_alert");
$qs->AddValidAction("email_alert");
$qs->AddValidAction("email_alert2");
$qs->AddValidAction("csv_alert");
$qs->AddValidAction("archive_alert");
$qs->AddValidAction("archive_alert2");
$qs->AddValidActionOp(_SELECTED);
$qs->AddValidActionOp(_ALLONSCREEN);
$qs->SetActionSQL($from . $where);
($debug_time_mode >= 1) ? $et->Mark("Initialization") : '';
$qs->RunAction($submit, PAGE_STAT_ALERTS, $db);
($debug_time_mode >= 1) ? $et->Mark("Alert Action") : '';
/* Get total number of events */
/* mstone 20050309 this is expensive -- don't do it if we're avoiding count() */
/*if ($avoid_counts != 1 && !$use_ac) {
$event_cnt = EventCnt($db);
if($event_cnt == 0){
$event_cnt = 1;
}
}*/
/* create SQL to get Unique Alerts */
$cnt_sql = "SELECT count(DISTINCT signature) " . $from . $where;
/* Run the query to determine the number of rows (No LIMIT)*/
if (!$use_ac) $qs->GetNumResultRows($cnt_sql, $db);
($debug_time_mode >= 1) ? $et->Mark("Counting Result size") : '';
/* Setup the Query Results Table */
$qro = new QueryResultsOutput("base_stat_alerts_graph.php?caller=" . $caller);
$qro->AddTitle(" ");
$qro->AddTitle(_SIGNATURE, "sig_a", " ", " ORDER BY sig_name ASC", "sig_d", " ", " ORDER BY sig_name DESC");
$qro->AddTitle(_TOTAL . "&nbsp;#", "occur_a", " ", " ORDER BY sig_cnt ASC", "occur_d", " ", " ORDER BY sig_cnt DESC");
$qro->AddTitle(_SENSOR . "&nbsp;#");
$qro->AddTitle(_("Src. Addr.") , "saddr_a", ", count(DISTINCT ip_src) AS saddr_cnt ", " ORDER BY saddr_cnt ASC", "saddr_d", ", count(DISTINCT ip_src) AS saddr_cnt ", " ORDER BY saddr_cnt DESC");
$qro->AddTitle(_("Dst. Addr.") , "daddr_a", ", count(DISTINCT ip_dst) AS daddr_cnt ", " ORDER BY daddr_cnt ASC", "daddr_d", ", count(DISTINCT ip_dst) AS daddr_cnt ", " ORDER BY daddr_cnt DESC");
/*$qro->AddTitle(_FIRST,
"first_a", ", min(timestamp) AS first_timestamp ",
" ORDER BY first_timestamp ASC",
"first_d", ", min(timestamp) AS first_timestamp ",
" ORDER BY first_timestamp DESC");

if ( $show_previous_alert == 1 )
$qro->AddTitle("Previous");

$qro->AddTitle(_LAST,
"last_a", ", max(timestamp) AS last_timestamp ",
" ORDER BY last_timestamp ASC",
"last_d", ", max(timestamp) AS last_timestamp ",
" ORDER BY last_timestamp DESC");
*/
$qro->AddTitle(_("Graph"));
$sort_sql = $qro->GetSortSQL($qs->GetCurrentSort() , $qs->GetCurrentCannedQuerySort());
/* mstone 20050309 add sig_name to GROUP BY & query so it can be used in postgres ORDER BY */
/* mstone 20050405 add sid & ip counts */
$sql = "SELECT DISTINCT signature, count(signature) as sig_cnt, " . "min(timestamp), max(timestamp), sig_name, count(DISTINCT(acid_event.sid)), count(DISTINCT(ip_src)), count(DISTINCT(ip_dst)), sig_class_id " . $sort_sql[0] . $from . $where . " GROUP BY signature, sig_name, sig_class_id " . $sort_sql[1];
//echo $sql."<br>";
// time selection for graph x
$tr = ($_SESSION["time_range"] != "") ? $_SESSION["time_range"] : "all";
list($x, $y, $xticks, $xlabels) = range_graphic($tr);
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
        $interval = ($use_ac) ? "day(day) as intervalo, monthname(day) as suf" : "day(timestamp) as intervalo, monthname(timestamp) as suf";
        $grpby = "GROUP BY intervalo,suf ORDER BY suf,intervalo";
        break;

    case "month":
        $interval = ($use_ac) ? "day(day) as intervalo, monthname(day) as suf" : "day(timestamp) as intervalo, monthname(timestamp) as suf";
        $grpby = "GROUP BY intervalo,suf ORDER BY suf,intervalo";
        break;

    default:
        $interval = ($use_ac) ? "monthname(day) as intervalo, year(day) as suf" : "monthname(timestamp) as intervalo, year(timestamp) as suf";
        $grpby = "GROUP BY intervalo,suf ORDER BY suf,intervalo";
}
$sqlgraph = "SELECT count(signature) as sig_cnt, $interval FROM acid_event $where AND signature=SIGNATURE AND sig_name='SIGNAME' AND sig_class_id=SIGCLASSID $grpby";
// use accumulate tables only with timestamp criteria
if ($use_ac) {
    $where = $more = $sqla = $sqlb = $sqlc = "";
    if (preg_match("/timestamp/", $criteria_clauses[1])) {
        $where = "WHERE " . str_replace("timestamp", "day", $criteria_clauses[1]);
        $sqla = " and ac_alerts_signature.day=ac_alerts_sid.day";
        $sqlb = " and ac_alerts_signature.day=ac_alerts_ipsrc.day";
        $sqlc = " and ac_alerts_signature.day=ac_alerts_ipdst.day";
    }
    $orderby = str_replace("acid_event.", "", $sort_sql[1]);
    $sql = "SELECT SQL_CALC_FOUND_ROWS DISTINCT signature, sum(sig_cnt) as sig_cnt,
      min(ac_alerts_signature.first_timestamp) as first_timestamp,  max(ac_alerts_signature.last_timestamp) as last_timestamp,
      sig_name,
      (select count(distinct(sid)) from ac_alerts_sid where ac_alerts_signature.signature=ac_alerts_sid.signature $sqla) as sig_cnt,
      (select count(distinct(ip_src)) from ac_alerts_ipsrc where ac_alerts_signature.signature=ac_alerts_ipsrc.signature $sqlb) as saddr_cnt,
      (select count(distinct(ip_dst)) from ac_alerts_ipdst where ac_alerts_signature.signature=ac_alerts_ipdst.signature $sqlc) as daddr_cnt,
      sig_class_id
      FROM ac_alerts_signature FORCE INDEX(primary) $where GROUP BY signature, sig_name, sig_class_id $orderby";
    $event_cnt = EventCnt($db, "", "", "SELECT sum(sig_cnt) FROM ac_alerts_signature FORCE INDEX(primary) $where");
    $where = "AND " . str_replace("timestamp", "day", $criteria_clauses[1]);
    if ($tr != "today" && $tr != "day") {
        // we dont have hour interval in ac_* tables
        $sqlgraph = "SELECT sum(sig_cnt) as sig_cnt, $interval FROM ac_alerts_signature WHERE signature=SIGNATURE AND sig_name='SIGNAME' AND sig_class_id=SIGCLASSID $grpby";
    }
} else {
    $event_cnt = EventCnt($db, "", $where);
    if ($event_cnt == 0) $event_cnt = 1;
}
//echo $sql."<br>".$sqlgraph."<br>".$interval." ".$tr;
/* Run the Query again for the actual data (with the LIMIT) */
$result = $qs->ExecuteOutputQuery($sql, $db);
if ($use_ac) $qs->GetCalcFoundRows($cnt_sql, $db);
($debug_time_mode >= 1) ? $et->Mark("Retrieve Query Data") : '';
if ($debug_mode == 1) {
    $qs->PrintCannedQueryList();
    $qs->DumpState();
    echo "$sql<BR>";
}
/* Print the current view number and # of rows */
$qs->PrintResultCnt();
echo '
  <script src="js/jquery.js" language="javascript" type="text/javascript"></script>
  <script src="js/jquery.flot.pack.js" language="javascript" type="text/javascript"></script>
  ';
if (isset($_SERVER['HTTP_USER_AGENT']) && (strpos($_SERVER['HTTP_USER_AGENT'], 'MSIE') !== false)) echo '<script src="js/excanvas.pack.js" language="javascript" type="text/javascript" ></script>';
echo '<FORM METHOD="post" NAME="PacketForm" ACTION="base_stat_alerts.php">';
$qro->PrintHeader();
$i = 0;
// The below is due to changes in the queries...
// We need to verify that it works all the time -- Kevin
$and = (strpos($where, "WHERE") != 0) ? " AND " : " WHERE ";
while (($myrow = $result->baseFetchRow()) && ($i < $qs->GetDisplayRowCnt())) {
    $sig_id = $myrow[0];
    /* get Total Occurrence */
    $total_occurances = $myrow[1];
    /* Get other data */
    $sig_name = $myrow[4];
    $num_sensors = $myrow[5];
    $num_src_ip = $myrow[6];
    $num_dst_ip = $myrow[7];
    $class_id = $myrow[8];
    /* First and Last timestamp of this signature */
    $start_time = $myrow[2];
    $stop_time = $myrow[3];
    /* mstone 20050406 only do this if we're going to provide links to the first/last or if we're going to show the previous event time */
    if ($show_first_last_links == 1 || $show_previous_alert == 1) {
        $temp = "SELECT timestamp, acid_event.sid, acid_event.cid " . $from . $where . $and . "signature='" . $sig_id . "'
               ORDER BY timestamp DESC";
        $result2 = $db->baseExecute($temp, 0, 2);
        $last = $result2->baseFetchRow();
        $last_num = $total_occurances - 1;
        /* Getting the previous timestamp of this signature
        * (I.E. The occurances before Last Timestamp)
        */
        if ($show_previous_alert == 1) {
            if ($total_occurances == 1) {
                $prev = $last;
                $prev_time = $prev[0];
                $prev_num = 0;
            } else {
                $prev = $result2->baseFetchRow();
                $prev_time = $prev[0];
                $prev_num = $total_occurances - 2;
                $result2->baseFreeRows();
            }
        }
    }
    if ($show_first_last_links == 1) {
        /* Doing the same as above for the first entry that we are searching for.
        * The reason for doing this is because some older DB's such as ones using ODBC
        * probably don't support the move() function. Therefore, for the older DB's
        * to get the first entry from the $temp variable above, we would need to
        * continue to call MoveNext() for each and every entry for that signature. For
        * signatures with a large amount of alerts(i.e. >1000), this could cause a severe
        * performance hit for those users.
        */
        $temp = "SELECT timestamp, acid_event.sid, acid_event.cid " . $from . $where . $and . "signature='" . $sig_id . "'
               ORDER BY timestamp ASC";
        $result2 = $db->baseExecute($temp, 0, 1);
        $first = $result2->baseFetchRow();
        $first_num = 0;
        $result2->baseFreeRows();
    }
    /* Print out (Colored Version) -- Alejandro */
    qroPrintEntryHeader((($colored_alerts == 1) ? GetSignaturePriority($sig_id, $db) : $i) , $colored_alerts, 0, 'height="42"');
    $tmp_rowid = rawurlencode($sig_id);
    echo '  <TD nowrap>&nbsp;&nbsp;
                 <INPUT TYPE="checkbox" NAME="action_chk_lst[' . $i . ']" VALUE="' . $tmp_rowid . '">
                 &nbsp;&nbsp;
             </TD>';
    echo '      <INPUT TYPE="hidden" NAME="action_lst[' . $i . ']" VALUE="' . $tmp_rowid . '">';
    $signame = explode("]", BuildSigByID($sig_id, $db));
    qroPrintEntry(trim($signame[1]) , "left", "middle");
    //qroPrintEntry(BuildSigByID($sig_id, $db),"left","middle");
    qroPrintEntry('<FONT>' . '<A HREF="base_qry_main.php?new=1amp;&amp;sig%5B0%5D=%3D&amp;sig%5B1%5D=' . (rawurlencode($sig_id)) . '&amp;sig_type=1' . '&amp;submit=' . _QUERYDBP . '&amp;num_result_rows=-1">' . $total_occurances . '</A>' .
    /* mstone 20050309 lose this if we're not showing stats */
    (($avoid_counts != 1) ? ('(' . (round($total_occurances / $event_cnt * 100)) . '%)') : ('')) . '</FONT>', 'center', 'middle', 'nowrap');
    qroPrintEntry('<A HREF="base_stat_sensor.php?sig%5B0%5D=%3D&amp;sig%5B1%5D=' . rawurlencode($sig_id) . '&amp;sig_type=1">' . $num_sensors . '</A>', 'center', 'middle');
    if ($db->baseGetDBversion() >= 100) $addr_link = '&amp;sig_type=1&amp;sig%5B0%5D=%3D&amp;sig%5B1%5D=' . rawurlencode($sig_id);
    else $addr_link = '&amp;sig%5B0%5D=%3D&amp;sig%5B1%5D=' . rawurlencode($sig_id);
    qroPrintEntry('<FONT>' . BuildUniqueAddressLink(1, $addr_link) . $num_src_ip . '</A></FONT>', 'center', 'middle', 'nowrap');
    qroPrintEntry('<FONT>' . BuildUniqueAddressLink(2, $addr_link) . $num_dst_ip . '</A></FONT>', 'center', 'middle', 'nowrap');
    // GRAPH
    $graph = '<div id="plotarea' . $i . '" class="plot"></div>';
    $sqlgr = str_replace("SIGCLASSID", $class_id, $sqlgraph);
    $sqlgr = str_replace("SIGNAME", str_replace("'", "\'", $sig_name) , $sqlgr);
    $sqlgr = str_replace("SIGNATURE", $sig_id, $sqlgr);
    //echo $sqlgr."<br>";
    $rgraph = $qs->ExecuteOutputQuery($sqlgr, $db);
    $yy = $y;
    while ($rowgr = $rgraph->baseFetchRow()) {
        $label = trim($rowgr[1] . " " . $rowgr[2]);
        if (isset($yy[$label]) && $yy[$label] == 0) $yy[$label] = $rowgr[0];
    }
    /*$x= array(1,2,3,4,5,6,7);
    $y= array(1,0,1,0,1,0,1);
    $xticks= array(1,2,3,4,5,6,7);
    $xlabels= array("","","","","","","");*/
    $plot = plot_graphic("plotarea" . $i, 45, 400, $x, $yy, $xticks, $xlabels);
    qroPrintEntry($graph . $plot, 'center', 'middle');
    qroPrintEntryFooter();
    $i++;
    $prev_time = null;
}
$result->baseFreeRows();
$qro->PrintFooter();
$qs->PrintBrowseButtons();
$qs->PrintAlertActionButtons();
$qs->SaveState();
echo "\n</FORM>\n";
PrintBASESubFooter();
if ($debug_time_mode >= 1) {
    $et->Mark("Get Query Elements");
    $et->PrintTiming();
}
echo "</body>\r\n</html>";
?>
