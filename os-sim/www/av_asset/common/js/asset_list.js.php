<?php
header('Content-type: text/javascript');

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
?>


function av_list()
{
    this.cfg = <?php echo Asset::get_path_url() ?>;
    
    this.ajax_url = this.cfg.common.controllers + 'asset_filter_ajax.php';
    
    this.selection_type = 'manual'; //manual or filter
    
    
    this.datatables_assets = {};
    
    this.asset_type = '';
    
    this.db = null;
    
    this.perms = {};
    
    
    /**************************************************************************/
    /***************************  FILTER FUNCTIONS  ***************************/
    /**************************************************************************/
    
    /*  Function to modifiy the filters  */
    this.set_filter_value = function(id, value, del_value, tag_label)
    {
        var __self = this
        var params = {};
        var data   = {};
    
        data["id"]       = id;
        data["filter"]   = value;
        data["delete"]   = ~~del_value;
        data["reload"]   = 1;
    
        params["action"] = "modify_filter";
        params["data"]   = data;
    
        var ctoken = Token.get_token("asset_filter_value");
    	$.ajax(
    	{
    		data: params,
    		type: "POST",
    		url: __self.ajax_url + "?token="+ctoken,
    		dataType: "json",
    		beforeSend: function()
    		{
        	   __self.disable_search_inputs();
        	   __self.show_search_loading();
    		},
    		success: function(data)
    		{
        		if (!data.error)
        		{
                    if (del_value)
                    {
                       __self.remove_tag(id, value);
                    }
                    else
                    {
                        if (typeof tag_label != 'undefined' && tag_label != '' )
                        {
                            __self.create_tag(tag_label, id, value);
                        }
                    }
                    
                    __self.reload_table();
        		}
        		else
        		{
        			__self.enable_search_inputs();
        			__self.hide_search_loading();
            		show_notification('asset_notif', data.msg, 'nf_error', 5000, true);
        		}
    		},
    		error: function(XMLHttpRequest, textStatus, errorThrown)
    		{
                //Checking expired session
        		var session = new Session(XMLHttpRequest, '');
                if (session.check_session_expired() == true)
                {
                    session.redirect();
                    return;
                }
                
                __self.enable_search_inputs();
	    		__self.hide_search_loading();
	    		
                show_notification('asset_notif', errorThrown, 'nf_error', 5000, true);
    		}
    	});
    }
    
    
    /*  Function to apply the filters and reload the datatables  */
    this.reload_assets_group = function(force)
    {
        var __self = this;
        
        if (typeof force == 'undefined')
        {
            force = false;
        }
    
        var data      = {};
    
        data["force"] = ~~force;
    
    	var ctoken = Token.get_token("asset_filter_value");
    	$.ajax(
    	{
    		data: {"action":"reload_group", "data": data },
    		type: "POST",
    		url: __self.ajax_url + "?token=" + ctoken,
    		dataType: "json",
    		beforeSend: function()
    		{
                __self.disable_search_inputs();
    		},
    		success: function(data)
    		{
                //if the datatables is defined we reload it
                __self.reload_table();
    		},
    		error: function(XMLHttpRequest, textStatus, errorThrown)
    		{
                //Checking expired session
        		var session = new Session(XMLHttpRequest, '');
                if (session.check_session_expired() == true)
                {
                    session.redirect();
                    return;
                }

                show_notification('asset_notif', errorThrown, 'nf_error', 5000, true);
    		},
    		complete: function()
    		{
        		__self.enable_search_inputs();
    		}
    	});
    }


    /*  Function to restore the search --> It deletes the filter_list object  */
    this.restart_search = function()
    {
        var __self = this;
        var params = {
            'type': this.asset_type
        }
        
        var ctoken = Token.get_token("asset_filter_value");
        $.ajax(
    	{
    		data: {"action":"restart_search", "data": params},
    		type: "POST",
    		url: __self.ajax_url + "?token=" + ctoken,
    		dataType: "json",
    		success: function(data)
    		{
        		try
        		{
                    if (data.error)
                    {
                        document.location.reload();
                    }
    
                    __self.remove_all_filters();
                    
                    __self.reload_table();

                }
                catch(Err)
                {
                    document.location.reload();
                }
    		},
    		error: function(XMLHttpRequest, textStatus, errorThrown)
    		{
                //Checking expired session
        		var session = new Session(XMLHttpRequest, '');
                if (session.check_session_expired() == true)
                {
                    session.redirect();
                    return;
                }
    
                show_notification('asset_notif', errorThrown, 'nf_error', 5000, true);
    		}
    	});
    }

    
    /*  Function to restore the filter_list object if we cancel the filters in the extra_filter Lightbox  */
    this.restore_filter_list = function()
    {
        var __self = this;
        var ctoken = Token.get_token("asset_filter_value");
        $.ajax(
    	{
    		data: {"action": "cancel_filter"},
    		type: "POST",
    		url: __self.ajax_url + "?token=" + ctoken,
    		dataType: "json",
            error: function(XMLHttpRequest, textStatus, errorThrown)
    		{
                //Checking expired session
        		var session = new Session(XMLHttpRequest, '');
                if (session.check_session_expired() == true)
                {
                    session.redirect();
                    return;
                }
    		}
    	});
    }
    
    
    this.reload_table = function()
    {
        this.datatables_assets.fnDraw();
    }
    
    
    this.modify_date_filter = function(that)
    {
        var filter = $(that).data('filter');
    
        var from   = $('#date_from_'+filter).val();
        var to     = $('#date_to_'+filter).val();
    
        var value  = 'range;' + from + ';' + to;
    
        this.set_filter_value(filter, value, 0);
    
        return false;
    }
    
    
    
    /**************************************************************************/
    /****************************  TRAY FUNCTIONS  ****************************/
    /**************************************************************************/
    
    this.load_tray = function(row)
    {
        var __self = this;
        var id     = $(row).attr('id');
        
        if (__self.datatables_assets.fnIsOpen(row))
        {
            $(row).next('tr').find('#tray_container').slideUp(300, function()
            {
                __self.datatables_assets.fnClose(row);
            });
        }
        else
        {
            var wrapper = $('<div></div>',
            {
                'id'   : 'tray_container',    
                'class': 'list_tray'
            }).css('visibility', 'hidden');
        
            $('<div></div>',
            {
                'class': 'tray_triangle clear_layer'
            }).appendTo(wrapper);
            
            $(wrapper).AV_asset_indicator(
            {
                'asset_type' : this.asset_type,
                'asset_id'   : id,
                'class'      : 'circle_tray',
                'perms'      : this.perms
            }).hide();
            
            __self.datatables_assets.fnOpen(row, wrapper, 'tray_details');
            
            wrapper.slideDown(300, function()
            {
                $(this).css('visibility', 'visible');
            });
        }
    }
    
    
        
    /**************************************************************************/
    /***************************  ACTION FUNCTIONS  ***************************/
    /**************************************************************************/
    
    /* Function to open export hosts page  */
    this.add_asset = function(){};
    
    /* Function to open export hosts page  */
    this.export_selection = function(){};
    
    /* Function to delete all hosts which match with filter criteria */
    this.delete_selection = function(){}; 
    
    
    /*  Function to open new host form lightbox  */
    this.add_note = function()
    {
        var __self = this;
        
        __self.save_selection().done(function()
        {
            var url   = __self.cfg.common.views + 'bk_add_note.php?type=' + __self.asset_type;
            var title = "<?php echo Util::js_entities(_('Add Note')) ?>";
            
            GB_show(title, url, '350', '500');
            
        });

    }
    
    
    /*  Function to open new host form lightbox  */
    this.asset_scan = function()
    {
        var __self = this;
        
        __self.save_selection((__self.asset_type == 'group')).done(function()
        {
            var url   = '/ossim/netscan/new_scan.php?type=' + __self.asset_type;
            var title = "<?php echo Util::js_entities(_('Asset Scan')) ?>";
            
            GB_show(title, url, '600', '720');
            
        });
    }
    
    
    /*  Function to open new host form lightbox  */
    this.vuln_scan = function()
    {
        var __self = this;
        
        __self.save_selection().done(function()
        {
            var url   = '/ossim/vulnmeter/new_scan.php?action=create_scan&type=' + __self.asset_type;
            var title = "<?php echo Util::js_entities(_('Vulnerability Scan')) ?>";
            
            GB_show(title, url, '600', '720');
            
        })
    }
    
    
    /*  Function to enable Availability Monitoring to the selected assets  */
    this.toggle_monitoring = function(action)
    {
        var __self = this;
        
        __self.save_selection().done(function()
        {
            var ctoken = Token.get_token("toggle_monitoring");
            $.ajax(
            {
                type: "POST",
                url: __self.cfg.common.controllers + "bk_toggle_monitoring.php",
                data: 'token=' + ctoken + '&asset_type=' + __self.asset_type + '&action=' + action,
                dataType: "json",
                success: function(data)
                {
                    if (data.status == 'OK')
                    {
                        show_notification('asset_notif', data.data, 'nf_success', 15000, true);
                        __self.reload_assets_group(true);
                    }
                    else if (data.status == 'warning')
                    {
                        show_notification('asset_notif', data.data, 'nf_warning', 15000, true);
                    }
                    
                },
                error: function(XMLHttpRequest, textStatus, errorThrown)
                {
                    //Checking expired session
                    var session = new Session(XMLHttpRequest, '');
                    if (session.check_session_expired() == true)
                    {
                        session.redirect();
                        return;
                    }
                    
                    var error = XMLHttpRequest.responseText;
                    show_notification('asset_notif', error, 'nf_error', 5000, true);
                }
            });
            
        });
    }


    /*  Function to open new host form lightbox  */
    this.show_tags = function(elem)
    {
        var __self = this;
        
        this.save_selection((__self.asset_type != 'asset')).done(function()
        {
            elem.show_dropdown();
        });
    }
    
    
    
    /**************************************************************************/
    /*************************  SELECTION FUNCTIONS  **************************/
    /**************************************************************************/
    
    this.manage_check_selection = function(input)
    {
        var __self = this;
        
        if($(input).prop('checked'))
        {
            __self.db.save_check($(input).val());
        }
        else
        {
            __self.db.remove_check($(input).val());
        }
        
        if (__self.selection_type == 'filter')
        {
            __self.db.clean_checked();
            $('.asset_check:checked').each(function()
            {
                __self.db.save_check($(this).val());
            });
        }
        
        __self.selection_type = 'manual';
        __self.manage_asset_selection();
    }
    
    
    this.update_asset_counter = function()
    {
        num = this.get_all_asset_filter();
        
        $('#num_assets').text(this.number(num));
    }
    
    
    this.manage_asset_selection = function()
    {
        var c_all   = $('.asset_check').length;
        var c_check = $('.asset_check:checked').length;
        var f_all   = this.get_all_asset_filter();
        
        $('[data-bind="chk-all-assets"]').prop('checked', (c_all > 0 && c_all == c_check));
        
        if (this.selection_type == 'manual' && c_all > 0 && c_all == c_check && f_all > c_all)
        {
            var elem = $('[data-bind="msg-selection"]')
            
            var text1 = "<?php echo _('You have selected ### assets on this page.') ?>".replace('###', c_all);
            var text2 = "<?php echo _('Select all ### assets.') ?>".replace('###', this.number(f_all));
            
            $('span', elem).text(text1);
            $('a', elem).text(text2);
            
            elem.show();
        }
        else
        {
            $('[data-bind="msg-selection"]').hide();
        }
                        
        this.update_asset_counter();        
        this.update_button_status();
    }
    
    
    this.check_all_manual = function()
    {
        var status = $('[data-bind="chk-all-assets"]').prop('checked');
        $('.asset_check').prop('checked', status).trigger('change');
    }
    
    
    this.check_all_filter = function()
    {
        $('[data-bind="msg-selection"]').hide();
        this.selection_type = 'filter';
        this.update_asset_counter();
    }
    
    
    this.get_all_asset_filter = function()
    {
        try
        {
            return this.datatables_assets.fnSettings()._iRecordsTotal;
        }
        catch(Err)
        {
            return 0;
        }
    }
    
    
    this.get_num_selected_assets = function()
    {
        if (this.selection_type == 'filter')
        {
            return this.get_all_asset_filter();
        }
        else
        {
            return $('.asset_check:checked').length
        }
    }
    
    
    this.save_selection = function(members)
    {
        members    = (typeof members != 'boolean') ? 0 : ~~members; 
        
        var all    = 1;
        var assets = [];
        
        if (this.selection_type == 'manual')
        {
            all = 0;
            
            $('.asset_check:checked').each(function(id, elem)
            {
                assets.push($(elem).val());
            })
        }
           
        var data =
        {
            "asset_type"  : this.asset_type,
            "assets"      : assets,
            "all"         : all,
            "save_members": members
        };

        var token = Token.get_token("save_selection");  
        return $.ajax(
        {
            type: "POST",
            url: this.cfg.common.controllers  + "save_selection.php",
            data: {"action": "save_list_selection", "token": token, "data": data},
            dataType: "json"
        }).fail(function(obj)
        {
            //Checking expired session
            var session = new Session(obj, '');
            if (session.check_session_expired() == true)
            {
                session.redirect();
                return;
            }
            
            show_notification('asset_notif', obj.responseText, 'nf_error', 15000, true);
        });
        
    } 
    
    
    this.save_members = function(id)
    {
        var __self = this;
        
        var data   =
        {
            "asset_id"   : id,
            "asset_type" : __self.asset_type,
            "member_type": 'asset',
            "all"        : 1
        };
                
        var token = Token.get_token("save_selection");
        return $.ajax(
        {
            type: "POST",
            url: this.cfg.common.controllers  + "save_selection.php",
            data: {"action": "save_member_selection", "token": token, "data": data},
            dataType: "json"
        }).fail(function(obj)
        {
            //Checking expired session
            var session = new Session(obj, '');
            if (session.check_session_expired() == true)
            {
                session.redirect();
                return;
            }
            
            show_notification('asset_notif', obj.responseText, 'nf_error', 15000, true);
        });
    }
    
    
    
    /**************************************************************************/
    /***************************  LINKS FUNCTIONS  ****************************/
    /**************************************************************************/
    
    /*  Function to open the host detail */
    this.load_asset_detail = function(id){};
    
    
    /*  Function to open extra filters lightbox  */
    this.show_more_filters = function()
    {
        var url   = this.cfg.common.views  + 'extra_filters.php';
        var title = "<?php echo Util::js_entities(_('More Filters')) ?>";
        
        GB_show(title, url, '600', '1100');
    }
    
    
    
    /**************************************************************************/
    /****************************  TAGS FUNCTIONS  ****************************/
    /**************************************************************************/

    /*  Function to create a tag filter  */
    this.create_tag = function(label, filter, value)
    {
        var tag_info  = filter + '###' + value;
        var tag_class = $.md5('label_'+tag_info) + ' filter_' + filter;
    
        $('#tags_filters').tagit('createTag', label, tag_class, tag_info);
    }
    
    
    /*  Function to delete a tag filter  */
    this.remove_tag = function(filter, value)
    {
        var tag_info  = filter + '###' + value;
        var tag_class = $.md5('label_'+tag_info);
    
        //Removing the tag
        $('#tags_filters li.'+tag_class).remove();
    
        //Deselecting the checkboxes
        $('#filter_'+filter).prop('checked', false);
    
        this.disable_range_selector(filter);
    
        //Deselecting the date radios
        $('#filter_'+filter + ' input').prop('checked', false);
    
        //Hidding date inputs
        $('#filter_'+filter + ' .asset_date_range').hide();
    
        //Removing the content from the dates selected
        $('#filter_'+filter + ' .date_filter').val('');
        $('#filter_'+filter + ' .date_filter').datepicker("option", {minDate: null, maxDate: null});
    }
    
    
    
    /**************************************************************************/
    /***************************  DRAW FUNCTIONS  *****************************/
    /**************************************************************************/
    
    this.draw_asset_list = function(){};
    
     
    this.update_button_status = function()
    {
        var num_hosts = this.get_num_selected_assets();
        
        if (num_hosts == 0)
        {
            $('#delete_selection').addClass('disabled');
            $('#label_selection').addClass('disabled');
            $('#edit_selection').addClass('disabled');
            $('[data-bind="export-selection"]').addClass('disabled');
            
            $('#button_action').addClass('av_b_disabled');
            $('[data-bind="dropdown-actions"]').removeAttr('id');
        }
        else
        {
            $('#delete_selection').removeClass('disabled');
            $('#label_selection').removeClass('disabled');
            $('#edit_selection').removeClass('disabled');
            $('[data-bind="export-selection"]').removeClass('disabled');
            
            $('#button_action').removeClass('av_b_disabled');
            $('[data-bind="dropdown-actions"]').attr('id', 'dropdown-actions');
        }
        
        if (!this.perms['delete'])
        {
            $('#delete_selection').addClass('disabled');

            $('#edit_selection').addClass('disabled');            
        }
        
        if (!this.perms['create'])
        {
            $('#button_add').addClass('av_b_disabled');
            
            $('[data-bind="dropdown-add"]').removeAttr('id');
        }
    }
    
    this.action_enabled = function(elem)
    {
        if ($(elem).hasClass('disabled') || $(elem).hasClass('av_b_disabled'))
        {
            return false;
        }
        
        return true;
    }
    
    this.show_search_loading = function()
    {
        $('.table_data tbody').prepend('<div class="dt_list_loading"><div/>');
        $('.dataTables_processing').css('visibility', 'visible');
        $('.table_data input').prop('disabled', true);
        $('.dataTables_length select').prop('disabled', true);
        $('.dt_footer').hide();
        
    }
    
    
    this.hide_search_loading = function()
    {
        $('.table_data .dt_list_loading').remove();
        $('.dataTables_processing').css('visibility', 'hidden');  
        $('.table_data input').prop('disabled', false);
        $('.dataTables_length select').prop('disabled', false);
        $('.dt_footer').show();
        
    }
    
    
    this.disable_search_inputs = function()
    {
        $('.input_search_filter').prop('disabled', true);
        $('.calendar input').datepicker('disable');
        $('.ui-slider').slider('disable');
        $('body').css('cursor', 'wait'); 
        $('[data-bind="more-filters"]').addClass('av_b_disabled');
    }
    
    
    this.enable_search_inputs = function()
    {
        $('.input_search_filter').prop("disabled", false);
        $('.calendar input').datepicker('enable');
    
        if ($('#filter_6').prop('checked'))
        {
            $('#asset_value_slider .ui-slider').slider('enable');
        }
    
        if ($('#filter_5').prop('checked'))
        {
            $('#vulns_slider .ui-slider').slider('enable');
        }
    
        $('[data-bind="more-filters"]').removeClass('av_b_disabled');
        
        $('body').css('cursor', 'default');
    }
    
    
    /*  Function to unmark all the filters  */
    this.remove_all_filters = function()
    {
        //Uncheck checkboxes and radio
        $('.input_search_filter').prop('checked', false);
    
        //Restart range selectors
        this.disable_range_selector('all');
        
        this.db.clean_checked();
        
        this.selection_type = 'manual';
        $('.asset_check').prop('checked', false).trigger('change');
    
        //Removing filter tags
        $("#tags_filters .tagit-choice").remove();
    
        //Hidding date inputs
        $('.asset_date_range').hide();
    
        //Empty the date picker
        $('.date_filter').val('');
        $('.date_filter').datepicker("option", {minDate: null, maxDate: null});
    }
    
    
    this.disable_range_selector = function(filter)
    {
        //Vulns Range
        if (filter == 5 || filter == 'all')
        {
            //Disabling Slider
            $('#vulns_slider .ui-slider').slider('disable');
            //Restoring default value
            $('#vulns_slider .ui-slider').slider('values', [0,4]);
            $('#vrangeA option:eq(0)').prop('selected', true);
            $('#vrangeB option:eq(4)').prop('selected', true);
        }
        //Asset value range
        else if (filter == 6 || filter == 'all')
        {
            //Disabling Slider
            $('#asset_value_slider .ui-slider').slider('disable');
            //Restoring default value
            $('#asset_value_slider .ui-slider').slider('values', [0,5]);
            $('#arangeA option:eq(0)').prop('selected', true);
            $('#arangeB option:eq(5)').prop('selected', true);
        }
    }
    
    
    /**************************************************************************/
    /***************************  EXTRA FUNCTIONS  ****************************/
    /**************************************************************************/
    
    /*  Validation Filter for Autocomplete  */
    this.is_ip_cidr = function(val)
    {
        var pattern = /^(([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])(\/([1-9]|[1-2][0-9]|3[0-2]))?$/ ;
    
        if (val.match(pattern))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    
    this.number = function(n)
    {
        if (typeof $.number == 'function')
        {
            return $.number(n);
        }
        else
        {
            return n;
        }
    }

}
