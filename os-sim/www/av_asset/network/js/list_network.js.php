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

//Double click issue variables
var click_delay  = 300, n_clicks = 0, click_timer = null;

/**************************************************************************/
/****************************  EVENT HANDLERS  ****************************/
/**************************************************************************/

function load_search_handlers()
{
    __asset_list.draw_asset_list();
    

    $('.table_data').on('click', 'tr.asset_tr', function()
    {
        $(this).disableTextSelect();
        
        n_clicks++;  //count clicks
    
        var row = this;
        
        if(n_clicks === 1) //Single click event
        {
            click_timer = setTimeout(function() 
            {
                $(this).enableTextSelect();
                
                n_clicks = 0;             //reset counter
                
                __asset_list.load_tray(row);
    
            }, click_delay);
        } 
        else //Double click event
        {
            clearTimeout(click_timer);  //prevent single-click action
            n_clicks = 0;               //reset counter
            
            var id  = $(row).attr('id');
            
            //This function is defined in group_list.js or net_list.js
            __asset_list.load_asset_detail(id);
        }
        
    }).on('dblclick', '.table_data tr', function(e)
    {
        e.preventDefault();
    });
    

    
    /**********  Add Actions  **********/
    
    $('[data-bind="add-asset"]').on('click', function()
    {
        __asset_list.add_asset()
    });
    
    
    $('[data-bind="import-csv"]').on('click', function()
    {
        __asset_list.import_csv()
    });
        
    
    /**********  Checkboxes Actions  **********/
    
    $('[data-bind="chk-all-assets"]').on('change', function()
    {
        __asset_list.check_all_manual()
    });
    
    
    $('[data-bind="chk-all-filter"]').on('click', function()
    {
        __asset_list.check_all_filter()
    });


    /**********  Asset Actions  **********/
    
    $('[data-bind="export-selection"]').on('click', function()
    {
        if (!__asset_list.action_enabled(this))
        {
             return false;
        }
        
        __asset_list.export_selection();
    });


    /*  Adding click event on delete_all_hosts icon  */
    $('#delete_selection').on('click', function()
    {
        if (!__asset_list.action_enabled(this))
        {
             return false;
        }
        
        var n_assets = __asset_list.get_num_selected_assets();
        var msg = "<?php echo Util::js_entities(_('Are you sure you want to permanently delete ### networks?'))?>".replace('###', n_assets);
        
        var keys = {"yes": "<?php echo Util::js_entities(_('Yes')) ?>","no": "<?php echo Util::js_entities(_('No')) ?>"};
        
        av_confirm(msg, keys).done(function()
        {
            __asset_list.delete_selection();
        });

    });

    /* LABELS */

    // Labels dropdown

    var $label_selection = $('#label_selection');

    var o = {
        'load_tags_url'        : '<?php echo AV_MAIN_PATH?>/tags/providers/get_dropdown_tags.php',
        'manage_components_url': '<?php echo AV_MAIN_PATH?>/tags/controllers/tag_components_actions.php',
        'allow_edit'           : <?php echo intval(Session::am_i_admin()) ?>,
        'tag_type'             : 'asset',
        'select_from_filter'   : true,
        'show_tray_triangle'   : true,
        'on_save'              : function (status, data)
        {
            if (status == 'OK')
            {
                show_notification('asset_notif', data['components_added_msg'], 'nf_success', 5000, true);
            }
            else
            {
                show_notification('asset_notif', data, 'nf_error', 5000, true);
            }

        },
        'on_delete'            : function (status, data)
        {
            if (status == 'OK')
            {
                show_notification('asset_notif', data['components_deleted_msg'], 'nf_success', 5000, true)
            }
            else
            {
                show_notification('asset_notif', data, 'nf_error', 5000, true);
            }
        }
    };

    $label_selection.av_dropdown_tag(o);

    $label_selection.off('click').on('click', function()
    {
        if (!__asset_list.action_enabled(this))
        {
             return false;
        }
        
        __asset_list.show_tags($label_selection)
    });

    
    $('[data-bind="asset-scan"]').on('click', function()
    {
        __asset_list.asset_scan()
    });
    
    $('[data-bind="vuln_scan"]').on('click', function()
    {
        __asset_list.vuln_scan()
    });
    
    $('[data-bind="add-note"]').on('click', function()
    {
        __asset_list.add_note()
    });
    


    /**********  Tags Functions **********/

    $('#tags_filters').tagit(
    {
        onlyAllowDelete: true,
        beforeTagRemoved: function(event, ui)
        {
            return false;
        }
    });

    $(document).on('click', '#tags_filters .ui-icon-close', function(e)
    {
        e.preventDefault();
        e.stopImmediatePropagation();

        var info   = $(this).parents('li.tagit-choice').data('info').split('###');

        var type   = info[0];
        var value  = info[1];

        __asset_list.set_filter_value(type, value, 1);

        return false;
    });
    
    
    
    /**********  Filters Functions **********/
    
    //Lightbox for More Filters
    $('[data-bind="more-filters"]').on('click', function()
    {
        if (__asset_list.action_enabled(this))
        {
            __asset_list.show_more_filters();
        }
    });

    
    //Restart Filters
    $('[data-bind="restart-search"]').on('click', function()
    {
        __asset_list.restart_search();
    });


    $('[data-bind="search-asset"]').on('keyup', function(e)
    {
        if(e.keyCode == 13)
        {
            var value = $(this).val();

            if (value == '')
            {
                return false;
            }

            var label = '';

            if (__asset_list.is_ip_cidr(value))
            {
                var label = "<?php echo Util::js_entities(_('Network CIDR:')) ?> " + value;
                __asset_list.set_filter_value(24, value, 0, label);
            }
            else
            {
                var label = "<?php echo Util::js_entities(_('Network Name:')) ?> " + value;
                __asset_list.set_filter_value(23, value, 0, label);
            }

            $("#search_filter").val('');

            return false;
        }

    }).placeholder();

    /* Tiptip */

    $('.tiptip').tipTip({attribute: 'data-title'});

    /* ALARMS & EVENTS FILTERS */

    $('.value_filter').on('change', function()
    {

        var del   = $(this).prop('checked') ? 0 : 1;
        var id    = $(this).data('id');
        var label = '';

        if (id == 3)
        {
            label = "<?php echo Util::js_entities(_('Has Alarms')) ?>";
        }
        else if (id == 4)
        {
            label = "<?php echo Util::js_entities(_('Has Events')) ?>";
        }

        __asset_list.set_filter_value(id, id, del, label);

    });
    
    
    /* ASSET VALUE FILTER */

    //Slider
    $('#arangeA, #arangeB').selectToUISlider(
    {
        tooltip: false,
        labelSrc: 'text',
        labels: 5,
        sliderOptions:
        {
            stop: function(event, ui)
            {
                var val1  = $('#arangeA').val();
                var val2  = $('#arangeB').val();

                var value = val1 + ';' + val2;

                var label = "<?php echo Util::js_entities(_('Asset Value:')) ?> " + val1 + ' - ' + val2;

                $('#tags_filters li.filter_6').remove();

                __asset_list.set_filter_value(6, value, 0, label);
            }
        }
    });


    //Checkbox to enable/disable slider
    $('#filter_6').on('change', function()
    {
        if ($(this).prop('checked'))
        {
            var v1    = $('#arangeA').val();
            var v2    = $('#arangeB').val();

            var value = v1 + ';' + v2;

            var label = "<?php echo Util::js_entities(_('Asset Value:')) ?> " + v1 + ' - ' + v2;

            $('#asset_value_slider .ui-slider').slider('enable');

            __asset_list.set_filter_value(6, value, 0, label);
        }
        else
        {
            //Removing tag
            $('#tags_filters li.filter_6').remove();

            //Setting filter value in object
            __asset_list.set_filter_value(6, '', 1);
        }
    });


    /* VULNERABILITIES FILTER */

    //Slider
    $('#vrangeA, #vrangeB').selectToUISlider(
    {
        tooltip: false,
        labelSrc: 'text',
        sliderOptions:
        {
            stop: function( event, ui )
            {
                var val1  = $('#vrangeB').val();
                var val2  = $('#vrangeA').val();
                var text1 = '';
                var text2 = '';

                var value = val1 + ';' + val2;

                $('#tags_filters li.filter_5').remove();

                text1 = $('#vrangeA option:selected').text();
                text2 = $('#vrangeB option:selected').text();

                var label = "<?php echo Util::js_entities(_('Vulnerabilities:')) ?> " + text1 + ' - ' + text2;

                __asset_list.set_filter_value(5, value, 0, label);
            }
        }
    });


    //Checkbox to enable/disable slider
    $('#filter_5').on('change', function()
    {
        if ($(this).prop('checked'))
        {
            var v1    = $('#vrangeB').val();
            var v2    = $('#vrangeA').val();
            var t1    = '';
            var t2    = '';

            var value = v1 + ';' + v2;

            $('#vulns_slider .ui-slider').slider('enable');

            t1 = $('#vrangeA option:selected').text();
            t2 = $('#vrangeB option:selected').text();

            var label = "<?php echo Util::js_entities(_('Vulnerabilities:')) ?> " + t1 + ' - ' + t2;

            __asset_list.set_filter_value(5, value, 0, label);

        }
        else
        {
            //Removing tag
            $('#tags_filters li.filter_5').remove();

            //Setting filter value in object
            __asset_list.set_filter_value(5, '', 1);
        }
    });


    /* DATE FILTERS */

    $('.asset_date_input input[type=radio]').on('change', function()
    {
        var scope  = $(this).parents(".asset_date_input");
        var filter = $(scope).data('filter');
        var type   = $(this).val();
        var label  = '';
        var l_txt  = $(this).next('span').text();

        var value  = '';
        var from   = '';
        var to     = '';
        var del    = 0;

        if (type == 'range')
        {
            $('.asset_date_range', scope).show();

            from  = $('#date_from_'+ filter).val('');
            to    = $('#date_to_'+ filter).val('');

            value = type + ';' + from + ';' + to;

        }
        else
        {
            $('.asset_date_range', scope).hide();

            $('.calendar input', scope).val('');
        }

        value = type;

        if (filter == 1)
        {
            label = "<?php echo Util::js_entities(_('Assets Added:')) ?> " + l_txt;
        }
        else if (filter == 2)
        {
            label = "<?php echo Util::js_entities(_('Last Updated:')) ?> " + l_txt;
        }

        $('#tags_filters li.filter_'+filter).remove();

        __asset_list.set_filter_value(filter, value, del, label);

    });
    
    
    /*  CALENDAR PLUGIN  */
    $('.date_filter').datepicker(
    {
        showOn: "both",
        dateFormat: "yy-mm-dd",
        buttonImage: "/ossim/pixmaps/calendar.png",
        onSelect: function(date, ui)
        {
            var that   = ui.input;

            __asset_list.modify_date_filter(that);

        },
        onClose: function(selectedDate, ui)
        {
            var dir    = ui.id.match(/date_from_\d/);
            var filter = $(ui.input).data('filter');

            if (dir)
            {
                var dp = '#date_to_' + filter;

                $(dp).datepicker( "option", "minDate", selectedDate);
            }
            else
            {
                var dp = '#date_from_' + filter;

                $(dp).datepicker( "option", "maxDate", selectedDate);
            }
        }
    });


    $('.date_filter').on('keyup', function(e)
    {
        if (e.which == 13)
        {
            __asset_list.modify_date_filter(this);
        }
    });
    
    /* Line to prevent the autocomplete in the browser */ 
    
    $('input').attr('autocomplete','off');
}


/**********  LIGHTBOX EVENTS  **********/

function GB_onclose(url)
{
    var cond_restore = url.match(/extra_filters/);
    var cond_force   = url.match(/edition_type=bulk/);
    
    //If we cancel the extra filter Lightbox, we restore the filter object
    if (cond_restore)
    {
        __asset_list.restore_filter_list();
    }
    //If we close the bulk edition form, then we force the reload
    else if (cond_force)
    {
        __asset_list.reload_assets_group(true);
    }
}


function GB_onhide(url, params)
{
    var cond_new    = url.match(/net_form\.php$/);
    var cond_reload = url.match(/extra_filters/);
    var cond_force  = url.match(/(asset_form|import_all_nets|net_form\.php)/);
    var cond_notes  = url.match(/bk_add_note/);
    
    if (cond_new)
    {
        var id = params['id'];
        __asset_list.load_asset_detail(id);
    }
    if (cond_force)
    {
         __asset_list.reload_assets_group(true); 
    }
    else if (cond_reload)
    {
        __asset_list.reload_assets_group();
    }
    else if (cond_notes)
    {   
        var msg = "<?php echo Util::js_entities(_('Your note has been added to the assets in the selected networks.')) ?> ";
        
        show_notification('asset_notif', msg, 'nf_success', 15000, true);
    }
}




function av_network_list(perms)
{
    this.asset_type = 'network';
    this.db = new av_session_db('db_' + this.asset_type);
    this.perms = perms;

    
    /**************************************************************************/
    /***************************  DRAW FUNCTIONS  *****************************/
    /**************************************************************************/
    
    this.draw_asset_list = function()
    {
        var __self = this;
        
        __self.disable_search_inputs();
        
        __self.datatables_assets = $('.table_data').dataTable(
        {
            "bProcessing": true,
            "bServerSide": true,
            "bDeferRender": true,
            "sAjaxSource": __self.cfg.network.providers + "load_nets_result.php",
            "iDisplayLength": 20,
            "bLengthChange": true,
            "sPaginationType": "full_numbers",
            "bFilter": false,
            "aLengthMenu": [10, 20, 50],
            "bJQueryUI": true,
            "aaSorting": [[ 0, "desc" ]],
            "aoColumns": [
                { "bSortable": false, "sClass": "center", "sWidth": "30px"},
                { "bSortable": true,  "sClass": "left", "sWidth": "200px"},
                { "bSortable": false, "sClass": "left", "sWidth": "200px"},
                { "bSortable": false, "sClass": "left"},
                { "bSortable": false, "sClass": "center"},
                { "bSortable": false, "sClass": "center"},
                { "bSortable": false, "sClass": "center"},
                { "bSortable": false, "sClass": "center"},
                { "bSortable": false, "sClass": "center td_nowrap", "sWidth": "50px"}
            ],
            oLanguage :
            {
                "sProcessing": "&nbsp;<?php echo _('Loading Networks') ?> <img src='/ossim/pixmaps/loading3.gif' align='absmiddle'/>",
                "sLengthMenu": "&nbsp;_MENU_ <?php echo _('Networks') ?>",
                "sZeroRecords": "&nbsp;<?php echo _('No matching records found') ?>",
                "sEmptyTable": "&nbsp;<?php echo _('No networks found in the system') ?>",
                "sLoadingRecords": "&nbsp;<?php echo _('Loading') ?>...",
                "sInfo": "&nbsp;<?php echo _('Showing _START_ to _END_ of _TOTAL_ networks') ?>",
                "sInfoEmpty": "&nbsp;<?php echo _('Showing 0 to 0 of 0 networks') ?>",
                "sInfoFiltered": "(<?php echo _('filtered from _MAX_ total networks') ?>)",
                "sInfoPostFix": "",
                "sInfoThousands": ",",
                "sSearch": "<?php echo _('Search') ?>:",
                "sUrl": "",
                "oPaginate":
                {
                    "sFirst":    "",
                    "sPrevious": "&lt; <?php echo _('Previous') ?>",
                    "sNext":     "<?php echo _('Next') ?> &gt;",
                    "sLast":     ""
                }
            },
            "fnRowCallback": function(nRow, aData, iDrawIndex, iDataIndex)
            {
                var asset_id = aData['DT_RowId'];
    
                var input = $('<input>',
                {
                    'type'  : 'checkbox',
                    'value'  : asset_id ,
                    'class'  : 'asset_check',
                    'change' : function()
                    {
                        __self.manage_check_selection(this)
                    },
                    'click'  : function(e)
                    {
                        //To avoid to open the tray bar when clicking on the checkbox.
                        e.stopPropagation();
                    }
                }).appendTo($("td:nth-child(1)", nRow))
    
                if (__self.db.is_checked(asset_id) || __self.selection_type == 'filter')
                {
                    input.prop('checked', true)
                }
                
                $('<img></img>',
                {
                    "class"   : "load_edition",
                    "src"     : "/ossim/pixmaps/edit.png",
                    'click'  : function(e)
                    {
                        //To avoid to open the tray bar when clicking on the checkbox.
                        e.stopPropagation();
                        
                        __self.edit_network(asset_id);
                    }
                }).appendTo($("td:nth-child(9)", nRow));
                
                $('<img></img>',
                {
                    "class" : "detail_img",
                    "src"   : "/ossim/pixmaps/show_details.png",
                    "click" : function(e)
                    {
                        e.preventDefault();
                        e.stopPropagation();
                        
                        __self.load_asset_detail(asset_id);
                    }
                }).appendTo($("td:nth-child(9)", nRow));
                
                $(nRow).addClass('asset_tr');                
            },
             "fnPreDrawCallback": function ()
            {
                if (typeof $.fn.select2 == 'function')
                {
                    $('.dataTables_length select').select2(
                    {
                        hideSearchBox: true
                    });
                }
            },
            "fnServerData": function ( sSource, aoData, fnCallback, oSettings )
            {
                oSettings.jqXHR = $.ajax(
                {
                    "dataType": 'json',
                    "type": "POST",
                    "url": sSource,
                    "data": aoData,
                    "beforeSend": function()
                    {
                        __self.show_search_loading();
                    },
                    "success": function (json)
                    {
                        //DataTables Stuffs
                        $(oSettings.oInstance).trigger('xhr', oSettings);
                        fnCallback(json);
                    },
                    "error": function(data)
                    {
                        //Check expired session
                        var session = new Session(data, '');
    
                        if (session.check_session_expired() == true)
                        {
                            session.redirect();
                            return;
                        }
    
                        //DataTables Stuffs
                        var json = $.parseJSON('{"sEcho": '+aoData[0].value+', "iTotalRecords": 0, "iTotalDisplayRecords": 0, "aaData": "" }');
                        fnCallback(json);
                    },
                    "complete": function()
                    {
                        __self.hide_search_loading();
                        __self.enable_search_inputs();
                        __self.manage_asset_selection();
                    }
                });
            }
        });
    }
    

    
    /**************************************************************************/
    /*****************************  ADD FUNCTIONS  ****************************/
    /**************************************************************************/
    
    this.add_asset = function()
    {       
        var url   = this.cfg.network.views  + 'net_form.php';
        var title = "<?php echo Util::js_entities(_('New Network')) ?>";
        
        GB_show(title, url, '700', '720');
    }
        
    
    /*  Function to open import from csv lightbox  */
    this.import_csv = function()
    {
        var url   = this.cfg.network.views  + 'import_all_nets.php';
        var title = "<?php echo Util::js_entities(_('Import Networks from CSV')) ?>";
            
        GB_show(title, url, '700', '900');
    }
    
    
    
    /**************************************************************************/
    /***************************  ACTION FUNCTIONS  ***************************/
    /**************************************************************************/
    
    /* Function to open export hosts page  */
    this.export_selection = function()
    {
        var __self = this;
        
        __self.save_selection().done(function()
        {
            document.location.href = __self.cfg.network.views  + 'export_all_nets.php';
            
        });
    }    

    
    /* Function to delete all hosts which match with filter criteria */
    this.delete_selection = function()
    {
        var __self = this;
        
        __self.save_selection().done(function()
        {
            //AJAX data
        
            var h_data = 
            {
                "token" : Token.get_token("delete_network_bulk")
            };
        
            $.ajax(
            {
                type: "POST",
                url: __self.cfg.network.controllers  + "bk_delete.php",
                data: h_data,
                dataType: "json",
                beforeSend: function()
                {
                    $('#asset_notif').empty();
                    var _msg = '<?php echo Util::js_entities(_("Deleting Networks... Please Wait")) ?>';
        			show_loading_box('main_container', _msg , '');
                },
                success: function(data)
                {
        			hide_loading_box();
        
                    __self.restart_search();
                    
                    /* 
                    To Do
                    show_notification('asset_notif', data.data, 'nf_success', 15000, true);
                    */
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
                    
                    hide_loading_box();
                    
                    var _msg  = XMLHttpRequest.responseText;
                    
                    var _type = (_msg.match(/policy/)) ? 'nf_warning' : 'nf_error';
        
                    show_notification('asset_notif', _msg, _type, 15000, true);
        
                    __self.reload_table();
                }
            });
        });
    }
    
    /*  Function to open a lightbox to edit the network  */
    this.edit_network = function(asset_id)
    {
        var url   = this.cfg.network.views + "net_form.php?id=" + asset_id;
        var title = "<?php echo _('Edit Network') ?>";

        GB_show(title, url, '70%', '750');

        return false;
    }

    /**************************************************************************/
    /***************************  LINKS FUNCTIONS  ****************************/
    /**************************************************************************/
    
    /*  Function to open the network detail */
    this.load_asset_detail = function(id)
    {
        if (typeof id == 'undefined' || id == '')
        {
            return false;
        }
        
        var url = this.cfg.network.detail + '?asset_id='+urlencode(id);
        
        try
        {
            url = top.av_menu.get_menu_url(url, 'environment', 'assets', 'networks');
    	    top.av_menu.load_content(url);
        }
        catch(Err)
        {
            document.location.href = url

        }
    }

}

av_network_list.prototype = new av_list;
