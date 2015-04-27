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


function Av_plugin_list(config)
{
    //Public variables
    this.edit_mode     = config.edit_mode;
    this.sensor_id     = config.sensor_id;
    this.maxrows       = config.maxrows || 10;
    this.page_modified = false;
    this.saved_plugins = {};
    this.dt_obj        = {};

    //Asset data
    var __asset_data   = config.asset_data || {};
    
    
    //Private variables
    


    //Copy of this
    var __self = this;


    /**************************************************************************/
    /***************************  DRAW FUNCTIONS  *****************************/
    /**************************************************************************/


    this.draw = function()
    {
        var dt_parameters  = __get_dt_parameters();
        var aaSorting      = dt_parameters.sort;
        var aoColumns      = dt_parameters.columns;
        var fnServerParams = dt_parameters.server_params;
        var iDisplayLength = dt_parameters.maxrows;
        
        __self.dt_obj = $('.table_data').dataTable( 
        {
            "bProcessing": true,
            "bServerSide": true,
            "bDeferRender": true,
            "sAjaxSource": "<?php echo AV_MAIN_PATH . "/av_asset/common/providers/dt_plugins.php" ?>",
            "iDisplayLength": iDisplayLength,
            "bLengthChange": false,
            "sPaginationType": "full_numbers",
            "bFilter": false,
            "aLengthMenu": [[10, 20, 50], [10, 20, 50]],
            "bJQueryUI": true,
            "aaSorting": aaSorting,
            "aoColumns": aoColumns,
            oLanguage : 
            {
                "sProcessing": "<?php echo _('Loading')?>...",
                "sLengthMenu": "Show _MENU_ entries",
                "sZeroRecords": "<?php echo _('No plugins found')?>",
                "sEmptyTable": "<?php echo _('No plugins found')?>",
                "sLoadingRecords": "<?php echo _('Loading') ?>...",
                "sInfo": "<?php echo _('Showing _START_ to _END_ of _TOTAL_ plugins')?>",
                "sInfoEmpty": "<?php echo _('Showing 0 to 0 of 0 entries')?>",
                "sInfoFiltered": "(<?php echo _('filtered from _MAX_ total entries')?>)",
                "sInfoPostFix": "",
                "sInfoThousands": ",",
                "sSearch": "<?php echo _('Search')?>",
                "sUrl": "",
                "oPaginate": {
                    "sFirst":    "<?php echo _('First') ?>",
                    "sPrevious": "<?php echo _('Previous') ?>",
                    "sNext":     "<?php echo _('Next') ?>",
                    "sLast":     "<?php echo _('Last') ?>"
                }
            },
            "fnRowCallback": function(nRow, aData, iDrawIndex, iDataIndex)
            {
                // Create table containers to load the Plugin Select Boxes inside
                if (__self.edit_mode)
                {
                    var _asset_id = aData['DT_RowId'];
    
                    $.each(aData['DT_RowData'], function(key, val)
                    {
                        // Create one select container for each plugin the each asset
                        for (var i = 0; i < val.length; i++)
                        {
                            var _table = '<table class="plugin_list plugin_select_container" id="select_' + _asset_id + '"';
                            
                            // Get the selects selections from saved_plugins or ajax response if not
                            var _vendor   = (typeof __self.saved_plugins[_asset_id] != 'undefined') ? __self.saved_plugins[_asset_id][i]['vendor']   : val[i].vendor;
                            var _model    = (typeof __self.saved_plugins[_asset_id] != 'undefined') ? __self.saved_plugins[_asset_id][i]['model']    : val[i].model;
                            var _version  = (typeof __self.saved_plugins[_asset_id] != 'undefined') ? __self.saved_plugins[_asset_id][i]['version']  : val[i].version;
                            var _mlist    = (typeof __self.saved_plugins[_asset_id] != 'undefined') ? __self.saved_plugins[_asset_id][i]['mlist']    : val[i].model_list;
                            var _vlist    = (typeof __self.saved_plugins[_asset_id] != 'undefined') ? __self.saved_plugins[_asset_id][i]['vlist']    : val[i].version_list;
                            
                            // Data attributes will be used in Select Object constructor
                            _table += ' id="' + _asset_id + '_' + i + '"';
                            _table += ' data-host="' + _asset_id + '"';
                            _table += ' data-vendor="' + _vendor + '"';
                            _table += ' data-model="' + _model + '"';
                            _table += ' data-version="' + _version + '"';
                            _table += " data-model-list='" + _mlist + "'";
                            _table += " data-version-list='" + _vlist + "'";
    
                            _table += '></table>';
                            
                            var container = $(_table).appendTo($("td:nth-child(2)", nRow));
                        }
                    });
                    
                    $("td:nth-child(2)", nRow).attr('colspan', 3);
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
                        if (__self.edit_mode && __self.page_modified)
                        {
                            __self.save_plugins();

                            __self.page_modified = false;
                        }
                        
                        if (__asset_data.asset_type == 'asset')
                        {
                            $('.dt_footer').hide();
                        }
                    },
                    "success": function (json) 
                    {

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
                        
                        
                        var error = '<?php echo _('Unable to load the plugins info for this asset') ?>';
                        show_notification('plugin_notif', error, 'nf_error', 5000, true);
                        
                        var json = $.parseJSON('{"sEcho": '+aoData[0].value+', "iTotalRecords": 0, "iTotalDisplayRecords": 0, "aaData": "" }');
                        
                        fnCallback( json );
                    },
                    "complete": function()
                    {
                        if (__self.edit_mode)
                        {
                            // Create Plugin Select Object from container data attributes
                            $('.plugin_list').each(function()
                            {    
                                $('#' + $(this).attr('id')).AVplugin_select(
                                {
                                    "vendor"       : $(this).attr('data-vendor'),
                                    "model"        : $(this).attr('data-model'),
                                    "version"      : $(this).attr('data-version'),
                                    "vendor_list"  : __vendor_list,
                                    "model_list"   : $.parseJSON($(this).attr('data-model-list')),
                                    "version_list" : $.parseJSON($(this).attr('data-version-list'))
                                });
                            });
                            
                            $('.select_plugin').on('change', function()
                            {
                                __self.page_modified = true;
                            });
                            
                            $('.dataTables_empty').attr('colspan', 4);
                        }
                    }
                });
            },
            "fnServerParams": function (aoData)
            {
                $.each(fnServerParams, function(index, value) {
                    aoData.push(value);
                });
                
                aoData.push( { "name": "edit_mode",  "value": __self.edit_mode } );
                aoData.push( { "name": "sensor_id",  "value": __self.sensor_id } );
            }
        });
    };



    /**************************************************************************/
    /****************************  STORAGE FUNCTIONS  **************************/
    /**************************************************************************/

    this.save_plugins = function()
    {
        if (__self.page_modified)
        {
            // *** Call to av_plugin_select.js method _get_selected_plugins() ***
            var _page_plugins = _get_selected_plugins();
            
            $.each(_page_plugins, function (_asset_id, _asset_plugins)
            {
                $.each(_asset_plugins, function (i, _plugin_data)
                {
                    var _mlist_arr = {};
                    var _vlist_arr = {};
                    
                    $("#select_" + _asset_id + " .model option").each(function (i, op)
                    {
                        if ($(op).val != '')
                        {
                            _mlist_arr[$(op).val()] = $(op).text();
                        }
                    });

                    $("#select_" + _asset_id + " .version option").each(function (i, op)
                    {
                        if ($(op).val != '')
                        {
                            _vlist_arr[$(op).val()] = $(op).text();
                        }
                    });

                    var _mlist = JSON.stringify(_mlist_arr);
                    var _vlist = JSON.stringify(_vlist_arr);
                    
                    _asset_plugins[i].mlist = _mlist;
                    _asset_plugins[i].vlist = _vlist;
                });
                
                __self.saved_plugins[_asset_id] = _asset_plugins;
            });
        }
    }
    
    
    
    /**************************************************************************/
    /****************************  HELPER FUNCTIONS  **************************/
    /**************************************************************************/

    function __get_dt_parameters()
    {
        var sort          = [];
        var columns       = [];
        var server_params = [];

        
        sort = [[1, "desc"]];


        if (__self.edit_mode == 1)
        {
            columns = [
                { "bSortable": false, "sClass" : "td_asset" },
                { "bSortable": false, "sClass" : "td_main" }
            ];
        }
        else
        {
            // Not working yet TODO
            columns = [
                
            ];
        }

        server_params = [
            {"name": "asset_id",  "value" : __asset_data.asset_id},
            {"name": "asset_type","value" : __asset_data.asset_type}
        ];


        var dt_parameters = {
            'sort'          : sort,
            'columns'       : columns,
            'server_params' : server_params,
            'maxrows'       : __self.maxrows
        }

        return dt_parameters;
    }
    
};
