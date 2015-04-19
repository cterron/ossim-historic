<?php

function getDirFiles($dirPath)
{
    $filesArr = array();
    if ($handle = @opendir($dirPath))
    {
        while (false !== ($file = readdir($handle))) {
             if ($file != "." && $file != "..") {
                 $filesArr[] = trim($file);
             }
        }
                 
        closedir($handle);
     }
     return $filesArr;
}
?>


