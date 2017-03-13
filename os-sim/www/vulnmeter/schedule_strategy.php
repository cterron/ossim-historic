<?php
require_once "schedule_strategy_main.php";
require_once "schedule_strategy_helper.php";

trait ScheduleTraitMain {
	public $nt_margin = 80;
	public $type = "single";
	public $action = "sched.php";
	public $colspan = 1;

	public function injectJS() {
		?>
	
		$("#vtree").dynatree(
		{
			initAjax: { url: "../tree.php?key=<?php echo (Session::is_pro()) ? 'assets|entitiesassets' : 'assets' ?>" },
			clickFolderMode: 2,
			onActivate: function(dtnode)
			{
				if(dtnode.data.url!='' && typeof(dtnode.data.url)!='undefined')
				{
					if (dtnode.data.key.match(/hostgroup_[a-f0-9]{32}/i) !== null) // asset group
					{
						Regexp = /([a-f0-9]{32})/i;
						match  = Regexp.exec(dtnode.data.key);
		
						value = match[1] + '#hostgroup';
						text  = dtnode.data.title;
		
						addto ("targets", text, value);
					}
					else
					{
						var Regexp     = /.*_(\w+)/;
						var match      = Regexp.exec(dtnode.data.key);
						var id         = "";
						var asset_name = "";
		
						id = match[1];
		
						Regexp = /^(.*)\s*\(/;
						match  = Regexp.exec(dtnode.data.val);
							
						asset_name = match[1];
							
						// Split for multiple IP/CIDR
						var keys = dtnode.data.val.split(",");
		
						for (var i = 0; i < keys.length; i++)
						{
							var item   = keys[i];
							var value  = "";
							var text   = "";
		
							if (item.match(/\d+\.\d+\.\d+\.\d+\/\d+/) !== null) // net
							{
								Regexp = /(\d+\.\d+\.\d+\.\d+\/\d+)/;
								match  = Regexp.exec(item);
									
								value = id + "#" + match[1];
								text  = asset_name + " (" + match[1] + ")";
							}
							else if (item.match(/\d+\.\d+\.\d+\.\d+/) !== null) // host
							{
								Regexp = /(\d+\.\d+\.\d+\.\d+)/;
								match  = Regexp.exec(item);
									
								value = id + "#" + match[1];
								text  = asset_name + " (" + match[1] + ")";
							}
		
							if(value != '' && text != '' && !exists_in_combo('targets', text, value, true))
							{
								addto ("targets", text, value);
							}
						}
					}
		
					simulation();
		
					dtnode.deactivate()
		
				}
			},
			onDeactivate: function(dtnode) {},
			onLazyRead: function(dtnode)
			{
				dtnode.appendAjax(
				{
					url: "../tree.php",
					data: {key: dtnode.data.key, page: dtnode.data.page}
				});
			}
		});
	        $("#delete_all").on( "click", function() {
	            $("#hosts_alive").attr('disabled', false);
	            
	            $("#scan_locally").attr('disabled', false);
	        
	            selectall('targets');
	            
	            deletefrom('targets');
	            
	            disable_button();
	            
	            $("#sresult").hide();
	            
	            $('#v_info').hide();
	            
	        });
	        
	        $("#delete_target").on( "click", function() {
	        
	            deletefrom('targets');
	            
	            // check targets to enable host_alive check
	            
	            var all_targets = getcombotext('targets');
	            
	            if (all_targets.length == 0)
	            {
	                $('#v_info').hide();
	            
	                $("#hosts_alive").attr('disabled', false);
	                    
	                $("#scan_locally").attr('disabled', false);
	            
	                disable_button();
	            
	                $("#sresult").hide();
	            }
	            else
	            {
	                var found = false;
	                
	                var i = 0;
	                
	                var num_targets = 0;
	                
	                while (i < all_targets.length && found == false)
	                {
	                    if (all_targets[i].match( _excluding_ip ))
	                    {
	                        found = true;
	                    }
	                    else
	                    {
	                        num_targets++;    
	                    }
	                    
	                    i++;
	                }
	                
	                if (found == false)
	                {
	                    $("#hosts_alive").attr('disabled', false);
	                    
	                    $("#scan_locally").attr('disabled', false);
	                }
	                
	                if ( num_targets > 0 )
	                {
	                    simulation();    
	                }
	                else
	                {
	                    disable_button();
	            
	                    $("#sresult").hide();
	                }
	            }
	        });
	
	        // Autocomplete assets
	        var assets = [ <?php echo $this->schedule->getAutocomplete(); ?> ];
	        
	        $("#searchBox").autocomplete(assets, {
	            minChars: 0,
	            width: 300,
	            max: 100,
	            matchContains: true,
	            autoFill: false,
	            selectFirst: false,
	            formatItem: function(row, i, max) {
	                return row.txt;
	            }
	            
	        }).result(function(event, item) {
	        
	        	var value = '';
	        	var text  = '';
	        
	            if (item.type == 'host_group' || item.type == 'net_group')
	            {
	                value = item.id + "#" + item.prefix;
	                text  = item.name;
	                
	                addto ("targets", text, value);
				}
	            else
	            {
	            	var keys  = item.ip.split(",");
	            	var ip    = '';
	
					for (var i = 0; i < keys.length; i++) 
					{
	                    ip   = keys[i].replace(/[\s\t\n\r ]+/g,"");
							
					    value  = item.id + "#" + ip;                   
					    text   = item.name + " (" +ip + ")";
						
						if(!exists_in_combo('targets', text, value, true))
						{
						    addto ("targets", text, value);
						}
					}
				}
				simulation();
	            $('#searchBox').val('');
	                            
	        });
	        $("#searchBox").click(function() {
	            $("#searchBox").removeClass('greyfont');
	            $("#searchBox").val('');
	            });
	        
	        $("#searchBox").blur(function() {
	            $("#searchBox").addClass('greyfont');
	            $("#searchBox").val('<?php echo _("Type here to search assets")?>');
	        });
	        
	        $('#searchBox').keydown(function(event) {
	            if (event.which == 13)
	            {
	               var target = $('#searchBox').val().replace(/^\s+|\s+$/g, '');
	               
	               targetRegex   = /\[.*\]/;
	               
	               if( target != '' && !target.match( targetRegex ) && !exists_in_combo('targets', target , target, true) ) // is a valid target?
	               {
	                    addto ("targets", target , target );
	                                            
	                    // force pre-scan
	
	                    if (target.match( _excluding_ip ) )
	                    {
	                        show_notification('v_info', '<?php echo _('We need to know all network IPs to exclude one of them, so the "Only hosts that are alive" option must be enabled.')?>' , 'nf_info', false, true, 'padding: 3px; width: 80%; margin: 12px auto 12px auto; text-align: center;');
	                    
	                        $("#hosts_alive").attr('checked', true);
	                        $("#hosts_alive").attr('disabled', true);
	                        $("#scan_locally").attr('disabled', false);
	                    }
	                    
	                    $("#searchBox").val("");
	                    
	                    simulation();
	                }
	           }
	        });
	        toggle_scan_locally(false);
	        simulation();
		<?php 
		}
		public function injectCSS() {
			?>
			.job_option {
				text-align:left;
				padding: 0px 0px 0px 70px;
			}
			.job_option-label {
				margin: 5px auto;
				padding: 5px auto;
			}
			.madvanced {
				text-align:left;
				padding: 0px 0px 4px 59px;
			}
			#user, #entity {
			width: 159px;
			}
			#user option:first-child, #entity option:first-child {
			text-align:center !important;
			}
			.bottom-buttons {
			margin:0px auto;
			text-align: center
			} 
			<?php
		}
		public function injectHTML() {
		?>
		<td class="noborder" valign="top">
			<table width="100%" class="transparent" cellspacing="0" cellpadding="0">
				<tr>
					<td class="nobborder" style="vertical-align: top;text-align:left;padding:10px 0px 0px 0px;">
						<table class="transparent" cellspacing="4">
							<tr>
								<td class="nobborder" style="text-align:left;">
									<input class="greyfont" type="text" id="searchBox" value="<?php echo _("Type here to search assets")?>" />
								</td>
							</tr>
							<tr>
								<td class="nobborder">
									<select id="targets" name="targets[]" multiple="multiple">
									<?php
									if ($selected_targets = $this->schedule->getTargets()) {
										foreach ($selected_targets as $t_id => $t_name) {
											echo "<option value='$t_id'>$t_name</option>";
										}
									}
									?>
									</select>
		                        </td>
		                    </tr>
		                    <tr>
		                        <td class="nobborder" style="text-align:right">
		                        	<input type="button" value=" [X] " id="delete_target" class="av_b_secondary small"/>
		                            <input type="button" style="margin-right:0px;"value="Delete all" id="delete_all" class="av_b_secondary small"/>
		                        </td>
		                    </tr>
		                </table>
		            </td>
		            <td class="nobborder" width="450px;" style="vertical-align: top;padding:0px 0px 0px 5px;">
		                <div id="vtree" style="text-align:left;width:100%;"></div>
		            </td>
		        </tr>
		    </table>
		</td>
		<?php
		}
	
}

trait ScheduleTraitModal {
	public $nt_margin = 90;
	public $type = "group";
	public $action = "new_scan.php";
	public $colspan = 2;

	public function injectJS() {}
	public function injectCSS() {
		?>
			.job_option {
				text-align:left;
				padding: 0px 0px 0px 30px;
			}
			.madvanced {
				text-align:left;
				padding: 0px 0px 4px 30px;
			}
			#user, #entity {
				width: 140px;
			}
			#close_button {
				margin-right: 10px;
			}
			.bottom-buttons {
				margin:0px auto 10px auto;
				text-align: center
			}
			<?php
		}
		public function injectHTML() {
			$selected_targets = $this->schedule->getTargets();
			?>
			<select id="targets" name="targets[]" multiple="multiple" style="display: none">
			<?php foreach ($selected_targets as $t_id => $t_name) {?>
				<option value='<?php echo $t_id ?>'><?php echo $t_name ?></option>
			<?php } ?>
			</select>

			<span id="thosts" class="hidden">
			<?php
			//This one is used to pass total hosts from BE to the FE, via common algorigm
			//It cannot be done on current realization in a proper way
			//because in main form data is passed via separate ajax request but not submit as it should be
			echo count($this->schedule->getPlainTargets());
			?>
			</span>
			<?php
		}	
}



class ScheduleStrategyEdit extends ScheduleStrategyHelper implements ScheduleStrategyInterface {
	use ScheduleTraitMain;
	public function init() {
		// read the configuration from database
		$query    = 'SELECT * FROM vuln_job_schedule WHERE id = ?';
		$params   = array($this->schedule->parameters["sched_id"]);
		$result   = $this->schedule->conn->execute($query, $params);
		$database = $result->fields;
		$this->loadData();
		//job name
		$this->schedule->parameters["SVRid"] = $database['email'];
		$this->schedule->scan_locally = intval($database['meth_Ucheck']);
		$this->schedule->setPrimarySettingsFromDB($database);
		$this->schedule->parameters["next_CHECK"] = Util::get_utc_unixtime($database['next_CHECK']);
		if ($database['schedule_type'] != 'O') {
			preg_match('/(\d{4})(\d{2})(\d{2})/', $database['begin'], $found);
			$this->schedule->parameters["biyear"]      = $found[1];
			$this->schedule->parameters["bimonth"]     = $found[2];
			$this->schedule->parameters["biday"]       = $found[3];
		};
		 // date to fill the form
		 $this->schedule->parameters["next_CHECK"] = gmdate('Y-m-j-G-i-w', $this->schedule->parameters["next_CHECK"] + 3600*$this->tz);

		 $data = explode("-",$this->schedule->parameters["next_CHECK"]);
		 $this->schedule->parameters["time_hour"]		= $data[3];
		 $this->schedule->parameters["time_min"]		= $data[4];
		 $this->schedule->parameters["dayofweek"]		= $data[5];
		 $this->schedule->parameters["time_interval"]	= 1;
		 $this->schedule->parameters["dayofmonth"]		= 1;
		 $this->schedule->parameters["nthweekday"]		= 1;
		 $this->schedule->parameters["not_resolve"]		= 1;
		

		 list($this->schedule->parameters["ROYEAR"],$this->schedule->parameters["ROMONTH"],$this->schedule->parameters["ROday"]) = $data;
		 list($this->schedule->parameters["biyear"],$this->schedule->parameters["bimonth"],$this->schedule->parameters["biday"]) = $data;
		 
		$this->schedule->parameters["time_interval"] = $database['time_interval'];
		$this->schedule->parameters["dayofmonth"] = $database['day_of_month'];
		$this->schedule->parameters["dayofweek"] = $database['day_of_week'];
		$this->schedule->parameters["schedule_type"] = $database['schedule_type'];
		$this->schedule->parameters["nthweekday"] = $database['day_of_month'];
		$this->schedule->load_targets();

	}
}




class ScheduleStrategyRerun extends ScheduleStrategyHelper implements ScheduleStrategyInterface {
	use ScheduleTraitMain;
	public function init() {
		$query    = 'SELECT * FROM vuln_jobs WHERE id = ?';
		$params   = array($this->schedule->parameters["job_id"]);
		$result   = $this->schedule->conn->execute($query, $params);
		$database = $result->fields;
		$this->schedule->parameters["SVRid"] = $database['notify'];
		$this->schedule->parameters["scan_locally"] = intval($database['authorized']);
		$this->schedule->setPrimarySettingsFromDB($database);
		$this->loadData();
		$this->schedule->load_targets();
	}
	
	public function persetDefaults() {
		$this->schedule->current_time_to_paramaters();
	}
}


class ScheduleStrategyDelete extends ScheduleStrategyHelper implements ScheduleStrategyInterface {
	use ScheduleTraitMain;
	public function execute() {
		$conn = $this->schedule->conn;
		$query = 'SELECT username, name, id, report_id FROM vuln_jobs WHERE id=?';
		$params = array($this->schedule->parameters["job_id"]);
		$result = $conn->execute($query, $params);
		$username   = $result->fields['username'];
		$job_name   = $result->fields['name'];
		$kill_id    = $result->fields['id'];
		$report_id  = $result->fields['report_id'];
		
			$can_i_delete = FALSE;
		
			if (Session::am_i_admin() || Session::get_session_user() == $username)
			{
				$can_i_delete = TRUE;
			}
			else if (Session::is_pro() && Acl::am_i_proadmin())
			{
				$user_vision = (!isset($_SESSION['_user_vision'])) ? Acl::get_user_vision($conn) : $_SESSION['_user_vision'];
		
				$my_entities_admin = array_keys($user_vision['entity_admin']);
		
				if (in_array($username, $my_entities_admin))
				{
					$can_i_delete = TRUE;
				}
			}
		
			if ($can_i_delete)
			{
				$query  = 'DELETE FROM vuln_jobs WHERE id=?';
				$params = array($kill_id);
				$result = $conn->execute($query, $params);
		
				$query  = 'DELETE FROM vuln_nessus_reports WHERE report_id=?';
				$params = array($report_id);
				$result = $conn->execute($query, $params);
		
				$query  = 'DELETE FROM vuln_nessus_report_stats WHERE report_id=?';
				$params = array($report_id);
				$result = $conn->execute($query, $params);
		
				$query  = 'DELETE FROM vuln_nessus_results WHERE report_id=?';
				$params = array($report_id);
				$result = $conn->execute($query, $params);
		
				$infolog = array($job_name);
				Log_action::log(65, $infolog);
			}
			$this->redirect();
	}
}



trait ScheduleCreate {
	public function init() {
		$this->loadData();
		$conf = $GLOBALS['CONF'];
		$this->schedule->parameters["scan_locally"] = $this->schedule->parameters["authorized"] = $conf->get_conf('nessus_pre_scan_locally');
		$this->schedule->parameters["hosts_alive"]  = 1;
	}
	public function persetDefaults() {
		$this->schedule->current_time_to_paramaters();
		if (!$this->schedule->parameters["sid"]) {
			foreach ($this->schedule->get_v_profiles() as $key => $value) {
				if (preg_match("/^Default\s-\s.*/",$value )) {
					$this->schedule->parameters["sid"] = $key;
				}
			}
		}
		$this->schedule->load_targets();
	}
}

class ScheduleStrategyCreate extends ScheduleStrategyHelper implements ScheduleStrategyInterface {
	use ScheduleTraitMain,ScheduleCreate;
}

class ScheduleStrategyCreateModal extends ScheduleStrategyHelper implements ScheduleStrategyInterface {
	use ScheduleTraitModal,ScheduleCreate;
	public function redirect() {
		return false;
	}
}

trait ScheduleSave {
	public function persetDefaults() {
                $conf = $GLOBALS['CONF'];
                $this->schedule->parameters["scan_locally"] = $this->schedule->parameters["authorized"] = $conf->get_conf('nessus_pre_scan_locally');
		$this->schedule->load_targets();
	}

	public function execute() {
		$this->schedule->validate();
		if($this->schedule->getErrors()) {
			$this->loadData();
		} else {
			// save the scan data
			$this->submit_scan();
			$this->redirect();
		}
	}
	
	public function submit_scan() {
		$btime_hour = $this->schedule->parameters["time_hour"];  // save local time
		$btime_min  = $this->schedule->parameters["time_min"];
			
		$bbiyear    = sprintf('%02d',$this->schedule->parameters["biyear"]);
		$bbimonth   = sprintf('%02d',$this->schedule->parameters["bimonth"]);
		$bbiday     = sprintf('%02d',$this->schedule->parameters["biday"]);
		$uyear = $umonth = $uday = null;
		$insert_time = $requested_run = gmdate('YmdHis');
		if ($this->schedule->parameters["schedule_type"] != "N") {
			$run_time   = sprintf('%02d%02d%02d',  $this->schedule->parameters['time_hour'], $this->schedule->parameters['time_min'], '00');
			if($this->schedule->parameters["schedule_type"] == 'O') {
				// date and time for run once
				$uyear  = empty($this->schedule->parameters["ROYEAR"]) ? gmdate('Y') : $this->schedule->parameters["ROYEAR"];
				$umonth = empty($this->schedule->parameters["ROMONTH"]) ? gmdate('m') : $this->schedule->parameters["ROMONTH"];
				$uday = empty($this->schedule->parameters["ROday"]) ? gmdate('d') : $this->schedule->parameters["ROday"];
				$requested_run = sprintf('%04d%02d%02d%06d', $uyear, $umonth, $uday, $run_time );
			} else {
				$begin_in_seconds   = Util::get_utc_unixtime("$bbiyear-$bbimonth-$bbiday {$this->schedule->parameters['time_hour']}:{$this->schedule->parameters['time_min']}:00") - 3600 * $this->tz;
				$current_in_seconds = gmdate('U');                // current datetime in UTC
				$ndays = dates::$daysMap;
				if($this->schedule->parameters["schedule_type"] == 'NW') {
					$array_time = array();
					if( $begin_in_seconds > $current_in_seconds ) {
						// if it is a future date
						$array_time = array('month'=> $bbimonth, 'day' => $bbiday, 'year' => $bbiyear);
					}
					$requested_run = $this->weekday_month(strtolower($ndays[$this->schedule->parameters['nthdayofweek']]), $this->schedule->parameters['nthweekday'], $btime_hour, $btime_min, $array_time);
					
					$this->schedule->parameters['dayofmonth'] = $this->schedule->parameters['nthweekday'];
					$this->schedule->parameters['dayofweek'] = $this->schedule->parameters['nthdayofweek'];
				} elseif ($this->schedule->parameters["schedule_type"] == 'W') {
					$timing = mktime ( 0, 0, 0, $bbiyear, $bbimonth, $bbiday);
					$wday  = date("w",$timing); // make week day for begin day
					if( $begin_in_seconds > $current_in_seconds  && $this->schedule->parameters["dayofweek"] == $wday) { // if it is a future date
						$next_day = "$bbiyear-$bbimonth-$bbiday";  // selected date by user
					} else {
						$next_day = gmdate("Y-m-d", strtotime("next ".$ndays[$this->schedule->parameters["dayofweek"]]." GMT",gmdate("U"))); // next week
					}
					$requested_run = "$next_day $btime_hour:$btime_min:00";
				} else {
					if( $begin_in_seconds > $current_in_seconds ) {
						$requested_run = $bbiyear.$bbimonth.$bbiday.$run_time;
					} else {
						$requested_run = $this->schedule->parameters["schedule_type"] == "D"
								? gmdate("Ymd", strtotime("+1 day GMT",gmdate("U"))).$run_time
								//else if month
						: gmdate("Y-m-", strtotime("next month GMT", gmdate("U"))). "{$this->schedule->parameters['dayofmonth']} $btime_hour:$btime_min:00";
					}
				}
			}
			list ($b_y,$b_m,$b_d,$b_h,$b_u,$b_s,$b_time) = Util::get_utc_from_date($this->schedule->conn,$requested_run, $this->tz);
			$requested_run = sprintf("%04d%02d%02d%02d%02d%02d", $b_y, $b_m, $b_d, $b_h, $b_u, "00");
		}

		ossim_clean_error();
		$queries = $this->schedule->load_ctx_from_targets($insert_time, $bbiyear, $bbimonth, $bbiday, $requested_run);
		$execute_errors = array();

		foreach ($queries as $id => $sql_data)
		{
			$rs = $this->schedule->conn->execute($sql_data['query'], $sql_data['params']);
			if ($rs === FALSE)
			{
				$execute_errors[] = $this->schedule->conn->ErrorMsg();
			}
		}
                if (!empty($execute_errors)) {
                    Av_exception::throw_error(Av_exception::DB_ERROR, implode("; ",$execute_errors));
                }
		if (empty($execute_errors) && $this->schedule->parameters["schedule_type"] != 'N')
		{
			// We have to update the vuln_job_assets

			if (intval($this->schedule->parameters["sched_id"]) == 0)
			{
				$query = ossim_query('SELECT LAST_INSERT_ID() as sched_id');
				$rs    = $this->schedule->conn->Execute($query);

				if (!$rs)
				{
					Av_exception::throw_error(Av_exception::DB_ERROR, $this->schedule->conn->ErrorMsg());
				}
				else
				{
					$sched_id = $rs->fields['sched_id'];
				}
			}

			Vulnerabilities::update_vuln_job_assets($this->schedule->conn, 'insert', $this->schedule->parameters["sched_id"], 0);
		}
		$this->saveSuccess();
	}
	
	private function saveSuccess() {
		$config_nt = array(
				'content' => '',
				'options' => array (
						'type'          => 'nf_success',
						'cancel_button' => FALSE),
				'style'   => 'width: 40%; margin: 20px auto; text-align: center;'
		);
		
		$config_nt['content'] = (empty($execute_errors)) ? _('Successfully Submitted Job') : _('Error creating scan job:') . implode('<br>', $execute_errors);
		
		$nt = new Notification('nt_1', $config_nt);
		$nt->show();
		$this->schedule->conn->close($this->schedule->conn);		
	}


	private function weekday_month($day, $nth, $h, $m, $start_date = array()) {
		list($current_year,$current_month,$current_day,$current_hour,$current_minute) = explode("-",date("Y-m-d-H-i"));
		if(!empty($start_date)) {
			$current_month = $start_date["month"];
			$current_day = $start_date["day"];
			$current_year = $start_date["year"];
		}
		$today  = mktime($current_hour, $current_minute, 0,  $current_month, $current_day, $current_year);
		//Last day of previous month
		$date = $this->calculate_day_in_week_in_month($current_month, $current_year, $nth, $day);
		//If date is less than current, we search in next month
		if ( $date < $today )
		{
			$current_month = date("m",strtotime("+1 month", $today));
			$date = $this->calculate_day_in_week_in_month($current_month, $current_year, $nth, $day);
		}

		return date('Y-m-d $h:$m:00', $date);
	}

	private function calculate_day_in_week_in_month($current_month, $current_year, $nth, $day) {
		$date   = strtotime("-1 day", mktime(0, 0, 0, $current_month, 1, $current_year));
		//Search date
		for ($i=0; $i<$nth; $i++){
			$date = strtotime("next $day", $date);
		}
		return $date;
	}
}


class ScheduleStrategySave extends ScheduleStrategyHelper implements ScheduleStrategyInterface {
	use ScheduleTraitMain,ScheduleSave;
}

class ScheduleStrategySaveModal extends ScheduleStrategyHelper implements ScheduleStrategyInterface {
	use ScheduleTraitModal, ScheduleSave;
	public function redirect() {
		if ($this->schedule->getErrors()) {
			$this->loadData();
			return;
		}
		$cnt = count($this->schedule->getPlainTargets());
		$message = $this->schedule->parameters["schedule_type"] == 'N'
				? sprintf(_('Vulnerability scan in progress for (%s) assets'), $cnt)
				: sprintf(_('Vulnerability scan has been scheduled on (%s) assets'), $cnt);
				?>
			<script>
		    	top.frames['main'].show_notification('asset_notif', "<?php echo Util::js_entities($message) ?>", 'nf_success', 15000, true);
		        parent.GB_hide();
		    </script>
		<?php 
		die();
	}
		
	public function saveSuccess() {
	}
}
