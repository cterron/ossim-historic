<?php
/*
 * TODO:
 * - X Y Axis titles
 * - True Type Fonts
 * - Scale Labels
 * - Graph Sizes and Plot positioning
 * - Split classes in Custom_Graph and Custom_Graph_FromSQL
 * - Adapt Points Graph to new options
 */
require_once 'ossim_db.inc';

class Plugin_Custom_SQL extends Panel
{
    var $defaults = 
        array(
            'graph_db' => 'ossim',
            'graph_sql' => '',
            'graph_title' => '',
            'graph_type'  => 'pie',
            'graph_plotshadow' => 0,
            'graph_pie_theme' => 'sand',
            'graph_pie_3dangle' => 0,
            'graph_pie_explode' => 'none',
            'graph_pie_explode_pos' => 1,
            'graph_show_values' => 1,
            'graph_color' => '#000080',
            'graph_gradient' => '',
            'graph_y_min' => 0,
            'graph_y_max' => 0,
            'graph_x_min' => 0,
            'graph_x_max' => 0,
            'graph_y_top' => 0,
            'graph_y_bot' => 0,
            'graph_x_top' => 0,
            'graph_x_bot' => 0
        );
    
    function getCategoryName()
    {
        return _("Custom SQL graph");
    }
    
    function showSubCategoryHTML()
    {
        $html = '';
        $check_ossim = $check_snort = '';
        if ($this->get('graph_db') == 'snort') {
            $check_snort = 'checked';
        } else {
            $check_ossim = 'checked';
        }
        $html .= 'Database:
            <input type="radio" name="graph_db" value="ossim" '.$check_ossim.'>Ossim
            <input type="radio" name="graph_db" value="snort" '.$check_snort.'>Snort
            <br/>
        ';
        $html .= _("SQL code") . ':<br/>';
        $html .= '<textarea name="graph_sql" rows="17" cols="35" wrap="on">';
        $html .= $this->get('graph_sql');
        $html .= '</textarea>';
        return $html;
    }
    
    function showSettingsHTML()
    {
        $html = '';
        //
        // Graph Title
        //
        $html .= _("Graph Title");
        $html .= ': <input type="text" name="graph_title" value="'.$this->get('graph_title').'">';
        //
        // Graph types (pie, bars)
        //
        $html .= '<br/>'._("Graph Type").': <select name="graph_type">';
        $types = array('pie'    => _("Pie"),
                       'bars'   => _("Bars"));
        foreach ($types as $value => $label) {
            $checked = $this->get('graph_type') == $value ? 'selected' : '';
            $html .= "<option value='$value' $checked>$label</option>";
        }
        $html .= '</select><br/>';
        //
        // Shadow
        //
        $html .= _("Plot Shadow").": ";
        $opts = array(0 => _("No"), 1 => _("Yes"));
        foreach ($opts as $value => $label) {
            $check = $this->get('graph_plotshadow') == $value ? 'checked' : '';
            $html .= "<input type='radio' name='graph_plotshadow' value='$value' $check>$label ";
        }
        /********************************************************
                         PIE OPTIONS
        ********************************************************/
        $html .= "<hr/><b>"._("Pie Options")."</b><hr/>";
        //
        // Color Theme
        //
        $html .= _("Color Theme").': <select name="graph_pie_theme">';
        $themes = array(
            'sand'   => _("Sand"),
            'earth'  => _("Earth"),
            'pastel' => _("Pastel"),
            'water'  => _("Water")
        );
        foreach ($themes as $value => $label) {
            $selected = $this->get('graph_pie_theme') == $value ? 'selected' : '';
            $html .= "<option value='$value' $checked>$label</option>";
        }
        $html .= '</select><br/>';
        //
        // 3D Angle
        //
        $html .= _("3D Angle").': <input type="text" name="graph_pie_3dangle"'.
                ' value="'.$this->get('graph_pie_3dangle').'" size=2>';
        //
        // Explode slice
        //
        $html .= '<br/>'._("Explode Slice").": ";
        $opts = array(
            'none' => _("None"),
            'all'  => _("All"),
            'pos'  => 'Pos <input type="text" name="graph_pie_explode_pos" '.
                      'value="'.$this->get('graph_pie_explode_pos').'" size=2>'
        );
        foreach ($opts as $value => $label) {
            $check = $this->get('graph_pie_explode') == $value ? 'checked' : '';
            $html .= "<input type='radio' name='graph_pie_explode' value='$value' $check>$label ";
        }
        /********************************************************
                         BAR & POINTS OPTIONS
        ********************************************************/
        $html .= "<hr/><b>"._("Bar & Points Options")."</b><hr/>";
        //
        // Show values in graph
        //
        $html .= _("Show values").": ";
        $show_values = array(0 => _("No"), 1 => _("Yes"));
        foreach ($show_values as $key => $label) {
            $check = ($this->get('graph_show_values') == $key) ? 'checked' : '';
            $html.= " <input type='radio' name='graph_show_values' value='$key' $check>".$label;
        }
        $html .= "<br/>\n";
        //
        // Color settings
        //
        $color = $this->get('graph_color');
        $label = _("Color");
        $html .= <<<END
        <input type="hidden" id="graph_color" name="graph_color" value="$color" size=7>
        <table border=0><tr>
        <td>$label:&nbsp;</td>
        <td id="color_sample"
            style="border: 1px gray solid; width: 15px; height: 20px; font-size: 1px;
                   cursor: pointer; background: $color;"
            onClick="javscript: Control.ColorPalette.toggle('palette');">
        &nbsp;
        </td>
        </tr></table>
        <div id="palette" style="position:absolute; z-index: 100; display: none; padding: 0px"></div>
END;
        //
        // Gradient settings
        // (jpgraph-1.20.3/docs/html/729Usinggradientfillforbargraphs.html#7_2_9)
        // contant values defined at: jpgraph/src/jpgraph_gradient.php
        $gradients = array(
            0 => _("Plain"),
            1 => _("Middle Vertical"),
            2 => _("Middle Horizontal"),
            3 => _("Horizontal"),
            4 => _("Vertical"),
            5 => _("Wide Middle Vertical"),
            6 => _("Wide Middle Horizontal"),
            7 => _("Center"),
            8 => _("Reflection Left"),
            9 => _("Reflection Right"),
            10 => _("Raised")
        );
        $html .= _("Gradient").': <select name="graph_gradient">';
        foreach ($gradients as $value => $label) {
            $check = $this->get('graph_gradient') == $value ? 'selected' : '';
            $html .= "<option value='$value' $check>$label</option>"; 
        }
        $html .= '</select>';
        //
        // Min and Max values for X and Y axis
        //
        $html .= "<br/>"._("Axis Scale Values").":<br/>";
        $y_min = $this->get('graph_y_min') ? $this->get('graph_y_min') : 0;
        $y_max = $this->get('graph_y_max') ? $this->get('graph_y_max') : 0;
        $x_min = $this->get('graph_x_min') ? $this->get('graph_x_min') : 0;
        $x_max = $this->get('graph_x_max') ? $this->get('graph_x_max') : 0;
        $html .= "Y: <input type='text' name='graph_y_min' value='$y_min' size=3>min
                  <input type='text' name='graph_y_max' value='$y_max' size=3>max<br/>
                  X: <input type='text' name='graph_x_min' value='$x_min' size=3>min
                  <input type='text' name='graph_x_max' value='$x_max' size=3>max";
        //
        // Axis grace (jpgraph-1.20.3/docs/ref/LinearScale.html#_LINEARSCALE_SETGRACE)
        //
        $html .= "<br/>"._("Axis Scale Grace").":<br/>";
        $y_top = $this->get('graph_y_top') ? $this->get('graph_y_top') : 0;
        $y_bot = $this->get('graph_y_bot') ? $this->get('graph_y_bot') : 0;
        $x_top = $this->get('graph_x_top') ? $this->get('graph_x_top') : 0;
        $x_bot = $this->get('graph_x_bot') ? $this->get('graph_x_bot') : 0;
        $html .= "Y: <input type='text' name='graph_y_top' value='$y_top' size=3>%top
                  <input type='text' name='graph_y_bot' value='$y_bot' size=3>%bottom<br/>
                  X: <input type='text' name='graph_x_top' value='$x_top' size=3>%top
                  <input type='text' name='graph_x_bot' value='$x_bot' size=3>%bottom";
        return $html;
    }

    function showWindowContents()
    {
        $html = '';
        if (!$this->get('graph_sql')) {
            return _("Please configure options at the Sub-category tab");
        }
        if (!$this->get('graph_type')) {
            return _("Please configure options at the Settings tab");
        }

        // Return the image link
        $nocache = rand(100000, 99999999);
        $html .= '<img src="custom_graph.php?id='.$this->get('id', 'window_opts').'&nocache='.$nocache.'">';
        return $html;
    }
   
}
?>
