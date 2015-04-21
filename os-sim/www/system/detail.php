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

// Check permissions
if (!Session::am_i_admin())
{
     $config_nt = array(
        'content' => _("You do not have permission to see this section"),
        'options' => array (
            'type'          => 'nf_error',
            'cancel_button' => FALSE
        ),
        'style' => 'width: 60%; margin: 30px auto; text-align:center;'
    );

    $nt = new Notification('nt_1', $config_nt);
    $nt->show();

    exit();
}


$cell_id = GET('id');
$ctime   = GET('date');

ossim_valid($cell_id,  OSS_ALPHA, OSS_DIGIT. OSS_SCORE,  'illegal:' . _('Message Id'));
ossim_valid($ctime,    OSS_DATETIME,                     'illegal:' . _('Creation Time'));

if (ossim_error())
{
   die(ossim_error());
}

list ($msg_id, $component_id) = explode('_', $cell_id);

$msg_id = intval($msg_id);

if (!valid_hex32($component_id, TRUE))
{
    echo ossim_error(_('Invalid canonical UUID'));
    exit();
}

// Call API
try
{
    $status = new System_status();
    $status->set_viewed($msg_id, $component_id);
    list($detail) = $status->get_message_detail($msg_id);
}
catch(Exception $e)
{
    echo ossim_error($e->getMessage());
    exit();
}
?>
<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
<head>
    <title> <?php echo gettext("OSSIM Framework"); ?> </title>
    <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
    <meta http-equiv="Pragma" content="no-cache"/>
    <link type="text/css" rel="stylesheet" href="../style/av_common.css?t=<?php echo Util::get_css_id() ?>"/>
    <script type="text/javascript" src="/ossim/js/jquery.min.js"></script>

    <script type="text/javascript">
        $(document).ready(function() {
            $('div.content a').on('click',function()
            {
                if ($(this).attr('href').match(/enable_plugin|add_system/))
                {
                    document.location.href = $(this).attr('href');
                }
                else
                {
                    top.frames['main'].go_to($(this).attr('href'));
                }

                return false;
            });

            $('button').on('click',function()
            {
                parent.GB_close()
            });
        });
    </script>
</head>

<body>
    <div class='content'>
        <span class="title"><?php echo $detail['desc'];?></span>
        <br/><br/>

        <span class="content"><?php echo nl2br($status->format_message($detail, array('creation_time' => $ctime)));?></span>

        <br/><br/><br/>
        <span class="sec_title"><?php echo _('Suggested Actions')?></span>

        <ul>
            <?php
            foreach ($detail['actions'] as $action)
            {
                echo "<li>".$status->format_action_link($action, $component_id)."</li>";
            }
            ?>
        </ul>
        <p><button type="button"><?php echo _('Close')?></button></p>
    </div>
</body>
</html>
