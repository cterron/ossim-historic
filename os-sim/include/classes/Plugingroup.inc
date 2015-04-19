<?php


class Plugingroup
{
    var $id;           // group id
    var $name;         // group name
    var $description;  // group description
    var $plugins;      // array of plugins/sids
    
    function Plugingroup($id, $name, $description, $plugins)
    {
        $this->id = $id;
        $this->name = $name;
        $this->description = $description;
        $this->plugins = $plugins;
    }
    
    function get_id()          { return $this->id; }
    function get_name()        { return $this->name; }
    function get_description() { return $this->description; }
    /*
     * @returns Ordered array of plugins, each one in the form of an assoc array:
     *           0 => array(
     *                  ['id']    =>   // plugin id
     *                  ['name']  =>   // plugin name
     *                  ['sids']  =>   // plugin sids string
     *                  ['descr'] =>   // plugin description
     *                ),
     */
    function get_plugins()     { return $this->plugins; }
    
    /*
     * @static
     */
    function get_list($conn, $where = '')
    {
        if ($where) {
            $where = "AND $where";
        }
        $sql = "SELECT
                    plugin_group_descr.group_id,
                    plugin_group_descr.name as group_name,
                    plugin_group_descr.descr as group_description,
                    plugin_group.plugin_id,
                    plugin_group.plugin_sid,
                    plugin.name as plugin_name,
                    plugin.description as plugin_description
                FROM
                    plugin_group_descr, plugin_group, plugin
                WHERE
                    plugin_group.plugin_id = plugin.id AND
                    plugin_group_descr.group_id = plugin_group.group_id
                    $where
                ORDER BY
                    plugin_group_descr.group_id, plugin.name";
        $list = array();
        if (!$rs = $conn->Execute($sql)) {
            die($conn->ErrorMsg());
        }
        $group = $plugins = array();
        $prev_id = null;
        while (!$rs->EOF) {
            // new group started
            if ($prev_id != $rs->fields['group_id']) {
                // first run, do not store yet
                if ($prev_id !== null) {
                    $list[] = new Plugingroup (
                                        $group['id'],
                                        $group['name'],
                                        $group['descr'],
                                        $plugins
                              );
                }
                // store the header info of the previous plugin group
                $group['id']    = $rs->fields['group_id'];
                $group['name']  = $rs->fields['group_name'];
                $group['descr'] = $rs->fields['group_description'];
                $plugins = array();
                $prev_id = $rs->fields['group_id'];
            }
            $plugins[$rs->fields['plugin_id']] = array(
                            'id'    => $rs->fields['plugin_id'],
                            'sids'  => $rs->fields['plugin_sid'],
                            'name'  => $rs->fields['plugin_name'],
                            'descr' => $rs->fields['plugin_description']);
            $rs->MoveNext();
        }
        // store the latest record if there where results
        if (count($group)) {
            $list[] = new Plugingroup (
                                    $group['id'],
                                    $group['name'],
                                    $group['descr'],
                                    $plugins
                          );
        }
        return $list;
        
    }
}
?>
