<?php

function getDirFiles($dirPath)
{
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


