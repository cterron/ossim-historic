<?php
/*

CREATE TABLE user_config (
	login VARCHAR(64)  NOT NULL REFERENCES users (login),
    category VARCHAR(64) NOT NULL DEFAULT 'main',
    name VARCHAR(64) NOT NULL,
    value TEXT,
    PRIMARY KEY (login, category, name)
);

Usage:

require_once 'classes/Session.inc';
require_once 'classes/User_config.inc';
require_once 'ossim_db.inc';

$db = new ossim_db();
$conn = $db->connect();

$config = new User_config($conn);
$login = Session::get_session_user();

// Get stored var as simple text (default)

$var = $config->get($login, 'var_name', 'simple');

// Save a var into the database as a serialized PHP var

$config->set($login, 'var_name', $myvar, 'php');

// Removes a config option

$config->del($login, 'var_name');

*/
class User_config
{
    var $conn;
    
    function User_config($conn)
    {
        $this->conn = $conn;
    }
    
    /*
     * @param $login string - The user's login name
     * @param $name string - What config entry name to retrieve
     * @param $type string - 'simple' for storing simple values (default) or
     *                       'php'  for storing PHP variables
     * @param $category - What's the category of the $name entry
     */
    function get($login, $name, $type = 'simple', $category = 'main')
    {
        $sql = "SELECT value
                FROM user_config
                WHERE login=? AND category=? AND name=?";
        $params = array($login, $category, $name);
        if (!$rs = $this->conn->Execute($sql, $params)) {
            die(ossim_error($this->conn->ErrorMsg()));
        }
        if ($rs->EOF) {
            return null;
        }
        $value = $rs->fields['value'];
        if ($type == 'simple') {
            return $value;
        } elseif ($type == 'php') {
            return unserialize($value);
        }
        die(ossim_error("Invalid config type: $type"));
    }
    
    function set($login, $name, $value, $type = 'simple', $category = 'main')
    {
        $sql = "REPLACE INTO user_config (login, category, name, value)
                VALUES (?, ?, ?, ?)";
        if ($type == 'simple') {
            $val = $value; 
        } elseif ($type == 'php') {
            $val = serialize($value);
        } else {
            die(ossim_error("Invalid config type: $type"));
        }
        $params = array($login, $category, $name, $val);
        if (!$this->conn->Execute($sql, $params)) {
            die(ossim_error($this->conn->ErrorMsg()));
        }
        return true;
    }
    
    function del($login, $name, $category='main')
    {
        $sql = "DELETE FROM user_config WHERE login=? AND name=? AND category=?";
        $params = array($login, $name, $category);
        if (!$this->conn->Execute($sql, $params)) {
            die(ossim_error($this->conn->ErrorMsg()));
        }
        return true;
    }
}
?>
