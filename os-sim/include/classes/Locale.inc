<?php
require_once 'ossim_conf.inc';

$DEFAULT_LANGUAGE = "en_GB";
$DEFAULT_LOCALE_DIR = "/usr/share/locale/";

$conf = new ossim_conf();

if (!$language = $conf->get_conf("language", FALSE))
    $language = $DEFAULT_LANGUAGE;

if (!$locale_dir = $conf->get_conf("locale_dir", FALSE))
    $locale_dir = $DEFAULT_LOCALE_DIR;

// I18N support information here

// try out some locale options:
// 1) For debian works @euro or .UTF8
// 2) Mac OS X needs putenv()
list($lang, $country) = explode("_", $language);
$envlang = setlocale(LC_ALL, $language, "{$language}@euro", $lang, "{$language}.UTF8");
if ($envlang) {
    putenv("LANG=$envlang");
}

// Set the text domain as 'ossim'
// Localization files must be called ossim.po & ossim.mo
$domain = 'ossim';
bindtextdomain($domain, $locale_dir); 
textdomain($domain);
?>
