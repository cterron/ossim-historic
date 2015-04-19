<?php
require_once ('ossim_conf.inc');
require_once ('ossim_db.inc');



Class Log_config {
    var $code;
    var $log;
    var $descr;
    var $priority;
   
    function Log_config($code, $log, $descr, $priority)
    {
        $this->code        = $code; 
        $this->log         = $log;
        $this->descr       = $descr;
        $this->priority    = $priority;
    }
    
    function get_code()         { return $this->code; }
    function get_log()          { return $this->log; }
    function get_descr()        { return $this->descr; }
    function get_priority()     { return $this->priority; }



    function get_list($conn, $where = "", $order = "")
    {
	$query = OssimQuery("SELECT * FROM  log_config $order");

	if (!$rs = &$conn->Execute($query)) {
		print $conn->ErrorMsg();
		return array();
	}
	
	while (!$rs->EOF) {
			$list[] = new Log_config (
			$rs->fields["code"],
			$rs->fields["log"],
			$rs->fields["descr"],
			$rs->fields["priority"]);
			$rs->MoveNext();
	}
	return $list;
    }

    function update_log($conn, $code, $log)
    {
    	$sql = "UPDATE log_config SET log = ? WHERE code = ?";
        $params = array($log, $code);
        if (!$rs = &$conn->Execute($sql, $params)) {
                print $conn->ErrorMsg();
        }
                            
    }

  
    function update_log_conf($conn, $log_conflist)
    {
	foreach ($log_conflist as $log_conf) {
	$sql = "UPDATE log_config SET log= ? WHERE code=''";
    $params = array($log_conf);

	if ($this->conn->Execute($sql, $params) === false) {
            print 'error updating: '. $this->conn->ErrorMsg().'<BR>';
	    exit;
	 }
				

	}
    }


    
}    

?>
