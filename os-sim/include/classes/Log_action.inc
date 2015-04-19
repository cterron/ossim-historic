<?php
require_once ('classes/Session.inc');
require_once ('ossim_db.inc');


Class log_action {
    var $login;
    var $from;
    var $date;
    var $code;
    var $info;
    var $descr;
    var $priority;
    
    function log_action ($login, $from, $date, $code, $info, $descr, $priority)
    {
        $this->login       = $login; 
        $this->from        = htmlentities($from);
        $this->date        = $date;
        $this->code        = $code;
        $this->info        = $info;
        $this->descr       = $descr;
        $this->priority    = $priority;
    }
    
    function get_login()        { return $this->login; }
    function get_from()         { return $this->from; }
    function get_date()         { return $this->date; }
    function get_code()         { return $this->code; }
    function get_info()         { return $this->info; }
    function get_descr()        { return $this->descr; }
    function get_priority()     { return $this->priority; }


  
    function log($log_code, $extrainfo)
    {
       require_once('ossim_conf.inc');
       $conf = $GLOBALS["CONF"];
       $logact  = $conf->get_conf("user_action_log",FALSE);
       if (!$logact) {
            return; 
       }      

        $db = new ossim_db();
        $conn = $db->connect();
        
        if (!Log_action::log_action_to_db($conn, $log_code)) return; 
        
        $user_ip = Log_action::get_userip();
        $user = Session::get_session_user();

        $info = Log_action::get_loginfo($conn,$log_code);
        $info = Log_action::log_action_replace($info,$extrainfo);
               
        $sql = "INSERT INTO log_action (login, ipfrom, date, code, info) VALUES (?, ?, current_timestamp , ?, ?)";
        
        $params = array($user, $user_ip, $log_code, $info);
        if ($conn->Execute($sql, $params) === false)
        {
            print 'Error inserting log: '.$conn->ErrorMsg().'<BR>';
            return;
        }
    }
 
    function log_action_to_db($conn, $action_log)
    {
       $sql = "SELECT * FROM log_config where code = ?";
       $params = array($action_log);
        
       if (!$rs = &$conn->Execute($sql, $params)) {
            print $conn->ErrorMsg();
            return 0;
        } else {
         return($rs->fields['log']);
       }
    }
     
    function get_userip()
    {
        if (isset($_SERVER['HTTP_X_FORWARDED_FOR']) != "") {
  	        return $_SERVER['HTTP_X_FORWARDED_FOR'];
	    }

       return $_SERVER['REMOTE_ADDR']; 
    }


    function get_list($conn, $filter = "",  $order = "", $inf = 0, $sup = 25)
    {

        /* number of logs per page */
        $nlogs = $sup - $inf;


        $query = OssimQuery("SELECT log_action.*, log_config.priority, log_config.descr from log_action,
log_config where log_config.code = log_action.code $filter $order LIMIT $nlogs
OFFSET $inf");
        
        $list = array();
        
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
            return array();
        }
        
        while (!$rs->EOF) {
                    $list[] = new Log_action (
                    $rs->fields["login"],
                    $rs->fields["ipfrom"],
                    $rs->fields["date"],
                    $rs->fields["code"],
                    $rs->fields["info"],
                    $rs->fields["descr"],
                    $rs->fields["priority"]);
                    $rs->MoveNext();
        }
        return $list;
    }
 

    function get_count($conn, $cfilter)
    {
        $query = OssimQuery("SELECT count(*) AS count FROM log_action $cfilter
        ");
        
        if (!$rs = &$conn->Execute($query)) {
            print $conn->ErrorMsg();
        } else {
           return $rs->fields["count"];
        }
    }

    function log_action_replace($infostr,$arrplace)
    {
       if (count($arrplace)) { 
            $n = 1;
            foreach ($arrplace as $auxrplc) {
                $infostr = str_replace("%".$n."%",$auxrplc,$infostr);
                $n = $n + 1;
            }
       }
       return $infostr;
    }

    function get_loginfo($conn, $code)
    {
        $sql = "SELECT * from log_config WHERE code = ?";
        $params = array($code);

        if (!$rs = &$conn->Execute($sql, $params)) {
                print $conn->ErrorMsg();
        } else {
            return $rs->fields["descr"];
        }
    }
    
}    

?>
