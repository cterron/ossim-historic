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


require_once 'av_init.php';

Session::logcheck('environment-menu', 'ToolsScan');

$conf       = $GLOBALS['CONF'];
$nmap_path  = $conf->get_conf('nmap_path');

$nmap_exists  = (file_exists($nmap_path)) ? 1 : 0;

$keytree = 'assets';

$scan_modes = array(
    'ping'   => _('Ping'),
    'fast'   => _('Fast Scan'),
    'normal' => _('Normal'),
    'full'   => _('Full Scan'),
    'custom' => _('Custom')
);

$time_templates = array(
    '-T0' => _('Paranoid'),
    '-T1' => _('Sneaky'),
    '-T2' => _('Polite'),
    '-T3' => _('Normal'),
    '-T4' => _('Aggressive'),
    '-T5' => _('Insane')
);


//Database connection
$db   = new ossim_db();
$conn = $db->connect();

/****************************************************
************ Default scan configuration *************
****************************************************/

$sensor            = 'local';
$scan_mode         = 'fast';
$ttemplate         = '-T3';
$scan_ports        = '1-65535';
$autodetected      = 1;
$rdns              = 1;
$disabled          = '';
$validation_errors = '';
$asset_type        = (GET('type')=='group') ? 'group' : ((GET('type')=='network') ? 'network' : 'asset');


$selected = Filter_list::get_total_selection($conn, $asset_type);

if ($selected > Filter_list::MAX_NMAP_ITEMS)
{
    $msg       = _('Asset scans can only be performed on %s assets at a time. Please select less assets and try again.');
    $limit_msg = sprintf($msg, Util::number_format_locale(Filter_list::MAX_NMAP_ITEMS));
}
else
{
    $rscan = new Remote_scan('', 'normal', 'local');
    $jobs  = $rscan->get_selected_assets($conn, $asset_type);
    $close = false;
    
    if (GET('action')=='scan')
    {
        $scan_mode       = GET('scan_mode');
        $timing_template = GET('timing_template');
        $custom_ports    = GET('custom_ports');
        $autodetect      = (GET('autodetect') == '1') ? 1 : 0;
        $rdns            = (GET('rdns') == '1') ? 1 : 0;
        $custom_ports    = str_replace(' ', '', $custom_ports);
        
        ossim_valid($scan_mode,       OSS_ALPHA, OSS_SCORE, OSS_NULLABLE,                 'illegal:' . _('Full scan'));
        ossim_valid($timing_template, OSS_ALPHA, OSS_PUNC, OSS_NULLABLE,                  'illegal:' . _('Timing_template'));
        ossim_valid($custom_ports,    OSS_DIGIT, OSS_SPACE, OSS_SCORE, OSS_NULLABLE, ',', 'illegal:' . _('Custom Ports'));
        
        if (ossim_error())
        {
            $validation_errors = ossim_get_error_clean();
        }
        else
        {
            // Run remote nmap scans
            $targets = array();
            foreach ($jobs as $sensor_id => $sdata)
            {
                if ($sdata["available"])// Sensor available
                {
                    foreach ($sdata["assets"] as $assets)
                    {
                        $targets[] = $assets['ip'];
                    }
                    $scan = new Remote_scan(implode(' ',$targets), $scan_mode, $sensor_id, Session::get_session_user(), $timing_template, $autodetect, $rdns, $custom_ports);
                    
                    $scan->do_scan(TRUE, TRUE);
                    
                    $last_error = $scan->get_last_error();
                    
                    unset($scan);
                    
                    if (is_array($last_error) && !empty($last_error['data']))
                    {
                        $validation_errors = _('Scan could not be completed.  The following errors occurred').":\n".$last_error['data'];
                        break;
                    }
                    else
                    {
                        $jobs[$sensor_id]['status'] = _("Running");
                        $close = true;
                    }
                }
            }
        }
    }
}

?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
    <title><?php echo _('OSSIM Framework');?></title>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
    <meta http-equiv="Pragma" content="no-cache"/>
    <script type="text/javascript" src="../js/jquery.min.js"></script>
    <script type="text/javascript" src="../js/notification.js"></script>
    <script type="text/javascript" src="../js/messages.php"></script>
    <script type="text/javascript" src="../js/jquery-ui.min.js"></script>
    <script type="text/javascript" src="../js/jquery.cookie.js"></script>
    <script type="text/javascript" src="../js/token.js"></script>
    <script type="text/javascript" src="../js/jquery.tipTip.js"></script>
    <script type="text/javascript" src="../js/utils.js"></script>
    <script type="text/javascript" src="../js/av_scan.js.php"></script>
    <script type="text/javascript" src="../js/fancybox/jquery.fancybox-1.3.4.pack.js"></script>

    <link rel="stylesheet" type="text/css" href="../style/av_common.css?t=<?php echo Util::get_css_id() ?>"/>
    <link rel="stylesheet" type="text/css" href="../style/environment/assets/asset_discovery.css"/>
    <link rel="stylesheet" type="text/css" href="../style/jquery-ui-1.7.custom.css"/>
    <link rel="stylesheet" type="text/css" href="../style/progress.css"/>
    <link rel="stylesheet" type="text/css" href="../style/tipTip.css"/>
    <link rel="stylesheet" type="text/css" href="../style/fancybox/jquery.fancybox-1.3.4.css"/>


    <script type='text/javascript'>

        function close_window()
        {
            if (typeof parent.GB_close == 'function')
            {
                parent.GB_close();
            }

            return false;
        }
        
        function hide_window(msg, type)
        {
            if (typeof parent.GB_hide == 'function')
            {
                top.frames['main'].notify(msg, type, true);
                parent.GB_hide();
            }

            return false;
        }

        $(document).ready(function()
        {
            $('#close_button').click(function(event)
            {
                event.preventDefault();
                close_window(false);
            });

            $("#assets_form").on( "keypress", function(e) 
            {
                if (e.which == 13 )
                {
                    return false;
                }
            });

            /****************************************************
             ********************* Tooltips *********************
             ****************************************************/

            if ($(".more_info").length >= 1)
            {
                $(".more_info").tipTip({maxWidth: "auto"});
            }

            bind_nmap_actions();
            
            <?php 
            if ($close) 
            { 
                $msg = sprintf(_('Asset scan in progress for %s assets'), count($targets));
                echo 'hide_window("'. $msg .'", "nf_success");';
            } 
            ?>

        });
    </script>

</head>

<body>

<!-- Asset form -->

<div id='c_info'>
    <?php
    if (!$nmap_exists)
    {
        $error = new Av_error();
        $error->set_message('NMAP_PATH');
        $error->display();
    }

    if ( !empty($validation_errors) )
    {
        $txt_error = "<div>"._('The following errors occurred').":</div>
                      <div style='padding: 10px;'>".$validation_errors."</div>";

        $config_nt = array(
            'content' => $txt_error,
            'options' => array (
                'type'          =>  'nf_error',
                'cancel_button' =>  FALSE
            ),
            'style' =>  'width: 80%; margin: 20px auto; text-align: left;'
        );

        $nt = new Notification('nt_1', $config_nt);
        $nt->show();
    }
    elseif (GET('action')=='scan')
    {
        $config_nt = array(
            'content' => '<div>'._('Asset Scan successfully launched in background').'</div>',
            'options' => array (
                'type'          =>  'nf_success',
                'cancel_button' =>  TRUE
            ),
            'style' =>  'width: 80%; margin: 20px auto; text-align: left;'
        );

        $nt = new Notification('nt_1', $config_nt);
        $nt->show();
    }
    ?>
</div>

<div id='c_asset_discovery'>

    <form name="assets_form" id="assets_form">
        <input type="hidden" name="action" value="scan">

        <table align="center" id='t_ad'>

            <tbody>
                <tr>
                    <th colspan="2"><?php echo _('Target selection') ?></th>
                </tr>

                <tr>
                    <td>
                        <span> <?php echo _('List of selected assets to scan:');?></span>
                    </td>
                </tr>

                <tr>
                    <td class='container nobborder'>
                        <?php
                            if ( !empty($jobs) )
                            {
                            ?>
                            
                            <table class="sensors">
                                <th><?php echo _('Assets') ?></th>
                                <th><?php echo _('Sensor') ?></th>
                                <th><?php echo _('Status') ?></th>
                            <?php
                                $available = FALSE;
                                foreach ($jobs as $sensor_id => $sdata)
                                {
                                    echo "<tr>\n";
                                    $first = $sdata["assets"][0];
                                    $last  = $sdata["assets"][count($sdata["assets"])-1];
                                    if (count($sdata["assets"])-1 == 0)
                                    {
                                        $asset = $first['ip'];
                                    }
                                    else
                                    {
                                        $asset = $first['ip'] . " ... " . $last['ip'];
                                    }
                                    echo "<td>".$asset."</td>\n";

                                    echo "<td>".$sdata['name']." [".$sdata['ip']."]</td>\n";

                                    if ($sdata["available"])
                                    {
                                        if ($sdata['status'] == _("Running"))
                                        {
                                            $icon      = "../pixmaps/running.gif";
                                        }
                                        else
                                        {
                                            $icon      = "../pixmaps/tick.png";
                                            $available = TRUE;
                                        }
                                    }
                                    else
                                    {
                                        $icon          = "../pixmaps/cross.png";
                                    }
                                    echo "<td><img src='$icon' class='more_info' title=\"".$sdata['status']."\" border=0></td>\n";
                                    echo "</tr>\n";
                                }
                                
                                if (!$available)
                                {
                                    $disabled = "disabled='disabled'";
                                }
                            ?>
                            </table>
                        <?php
                        }
                        else
                        {
                            if ($limit_msg)
                            {
                                $config_nt = array(
                            		'content' => $limit_msg,
                            		'options' => array (
                            			'type'          => 'nf_error',
                            			'cancel_button' => false
                            		),
                            		'style'   => 'width: 70%; margin: 5px auto 15px auto; text-align:center;'
                            	);
                            
                            	$nt = new Notification('nt_1', $config_nt);
                            	$nt->show();
                        	}
                        	
                            $disabled = "disabled='disabled'";
                        }
                        ?>
                    </td>
                </tr>

                <tr>
                    <th colspan="2"><?php echo _('Advanced Options')?></th>
                </tr>

                <!-- Full scan -->
                <tr>
                    <td colspan="2" style="padding:7px 0px 0px 10px">

                        <table id='t_adv_options'>
                            <!-- Full scan -->
                            <tr>
                                <td class='td_label'>
                                    <label for="scan_mode"><?php echo _('Scan type')?>:</label>
                                </td>
                                <td>
                                    <select id="scan_mode" name="scan_mode" class="nmap_select vfield">
                                        <?php
                                        foreach ($scan_modes as $sm_v => $sm_txt)
                                        {
                                            $selected = ($scan_mode == $sm_v) ? 'selected="selected"' : '';

                                            echo "<option value='$sm_v' $selected>$sm_txt</option>";
                                        }
                                        ?>
                                    </select>
                                </td>
                                <td style='padding-left: 20px;'>
                                    <span id="scan_mode_info"></span>
                                </td>
                            </tr>

                            <!-- Specific ports -->
                            <tr id='tr_cp'>
                                <td class='td_label'>
                                    <label for="custom_ports"><?php echo _('Specify Ports')?>:</label>
                                </td>
                                <td colspan="2">
                                    <?php
                                        $scan_ports = ($scan_ports == '') ? '1-65535' : $scan_ports;
                                    ?>
                                    <input class="greyfont vfield" type="text" id="custom_ports" name="custom_ports" value="<?php echo $scan_ports?>"/>
                                </td>
                            </tr>

                            <!-- Time template -->
                            <tr>
                                <td class='td_label'>
                                    <label for="timing_template"><?php echo _('Timing template')?>:</label>
                                </td>
                                <td>
                                    <select id="timing_template" name="timing_template" class="nmap_select vfield">
                                        <?php
                                        foreach ($time_templates as $ttv => $tt_txt)
                                        {
                                            $selected = ($ttemplate == $ttv) ? 'selected="selected"' : '';

                                            echo "<option value='$ttv' $selected>$tt_txt</option>";
                                        }
                                        ?>
                                    </select>
                                </td>
                                <td style='padding-left: 20px;'>
                                    <span id="timing_template_info"></span>
                                </td>
                            </tr>

                            <tr>
                                <td colspan="3">

                                    <?php $ad_checked = ($autodetected == 1) ? 'checked="checked"' : '';?>

                                    <input type="checkbox" id="autodetect" name="autodetect" class='vfield' <?php echo $ad_checked?> value="1"/>
                                    <label for="autodetect"><?php echo _('Autodetect services and Operating System')?></label>
                                </td>
                            </tr>
                            <tr>
                                <td colspan="3">

                                    <?php $rdns_checked = ($rdns == 1) ? 'checked="checked"' : '';?>

                                    <input type="checkbox" id="rdns" name="rdns" class='vfield' <?php echo $rdns_checked?> value="1"/>
                                    <label for="rdns"><?php echo _('Enable reverse DNS Resolution')?></label>
                                </td>
                            </tr>
                        </table>
                    </td>
                </tr>
            </tbody>
        </table>

        <p align="center">
            <?php
            if (!GET('action')=='scan' || !empty($validation_errors))
            { ?>
            <input type="button" class="av_b_secondary" id="close_button" value="<?php echo _('Cancel') ?>"/>
            <input type="submit" id="scan_button" <?php echo $disabled ?> value="<?php echo _('Start Scan') ?>"/>
            <?php 
            } 
            else
            {
            ?>
            <input type="button" class="av_b_secondary" id="close_button" value="<?php echo _('Close') ?>"/>
            <?php
            }
            ?>
        </p>

        <div id='scan_result'></div>

        <br/>

    </form>
</div>

<?php
//Close DB connection
$db->close();
?>
<!-- end of Asset form -->
</body>
</html>
