/**
 * @summary     DataTables Plugins
 * @author      AlienVault
 *
 */

/*
Reload ajax with new url
*/
$.fn.dataTableExt.oApi.fnReloadAjax = function ( oSettings, sNewSource, fnCallback, bStandingRedraw )
{
    if ( typeof sNewSource != 'undefined' && sNewSource != null )
    {
        oSettings.sAjaxSource = sNewSource;
    }
    this.oApi._fnProcessingDisplay( oSettings, true );
    var that = this;
    var iStart = oSettings._iDisplayStart;

    oSettings.fnServerData( oSettings.sAjaxSource, [], function(json)
    {
        /* Clear the old information from the table */
        that.oApi._fnClearTable( oSettings );

        /* Got the data - add it to the table */
        var aData =  (oSettings.sAjaxDataProp !== "") ?
            that.oApi._fnGetObjectDataFn( oSettings.sAjaxDataProp )( json ) : json;

        for ( var i=0 ; i<json.aaData.length ; i++ )
        {
            that.oApi._fnAddData( oSettings, json.aaData[i] );
        }

        oSettings.aiDisplay = oSettings.aiDisplayMaster.slice();
        that.fnDraw();

        if ( typeof bStandingRedraw != 'undefined' && bStandingRedraw === true )
        {
            oSettings._iDisplayStart = iStart;
            that.fnDraw( false );
        }

        that.oApi._fnProcessingDisplay( oSettings, false );

        /* Callback user function - for event handlers etc */
        if ( typeof fnCallback == 'function' && fnCallback != null )
        {
            fnCallback( oSettings );
        }
    }, oSettings );
}


$.fn.dataTableExt.oApi.fnSetFilteringDelay = function ( oSettings, iDelay ) {
    var _that = this;

    if ( iDelay === undefined ) {
        iDelay = 250;
    }

    this.each( function ( i ) {
        $.fn.dataTableExt.iApiIndex = i;
        var
            $this = this,
            oTimerId = null,
            sPreviousSearch = null,
            anControl = $( 'input', _that.fnSettings().aanFeatures.f );

            anControl.unbind( 'keyup' ).bind( 'keyup', function() {
            var $$this = $this;

            if (sPreviousSearch === null || sPreviousSearch != anControl.val()) {
                window.clearTimeout(oTimerId);
                sPreviousSearch = anControl.val();
                oTimerId = window.setTimeout(function() {
                    $.fn.dataTableExt.iApiIndex = i;
                    _that.fnFilter( anControl.val() );
                }, iDelay);
            }
        });

        return this;
    } );
    return this;
};



/* Plugin for sorting by KB,MB,B and Bytes.
 * http://datatables.net/plug-ins/sorting extended to deal with:
 *    560 kb / quota;
 *    5.02 MB
 *    0 bytes / O b
 */


function get_unit(fs_data)
{
    var unit = 1;

    if (fs_data.match(/GB/i))
    {
        unit = 1024 * 1024 * 1024;
    }
    else if (fs_data.match(/MB/i))
    {
        unit = 1024 * 1024;
    }
    else if (fs_data.match(/KB/i))
    {
        unit = 1024;
    }

    return unit;
}


$.fn.dataTableExt.oSort['file-size-asc']  = function(a,b) {

    var x = parseFloat(a);

    if (isNaN(x))
    {
        x = -1;
    }

    var y = parseFloat(b);

    if (isNaN(y))
    {
        y = -1;
    }

    a = a.replace(/\s+?\/.*/,'');
    b = b.replace(/\s+?\/.*/,'');

    var x_unit = get_unit(a);
    var y_unit = get_unit(b);

    x = parseInt(parseFloat(x) * x_unit) || 0;
    y = parseInt(parseFloat(y) * y_unit) || 0;

    return ((x < y) ? -1 : ((x > y) ? 1 : 0));
};


$.fn.dataTableExt.oSort['file-size-desc']  = function(a,b) {

    var x = parseFloat(a);

    if (isNaN(x))
    {
        x = 1;
    }

    var y = parseFloat(b);

    if (isNaN(y))
    {
        y = 1;
    }

    a = a.replace(/\s+?\/.*/,'')
    b = b.replace(/\s+?\/.*/,'')

    var x_unit = get_unit(a);
    var y_unit = get_unit(b);

    x = parseInt(parseFloat(x) * x_unit) || 0;
    y = parseInt(parseFloat(y) * y_unit) || 0;

    return ((x < y) ? 1 : ((x > y) ?  -1 : 0));
};
