<?php


if (!$url = $_GET["url"]) {
    echo "url?";
    exit;
}


topdf($url);

function topdf($url) {
    header("Content-Type: application/pdf");
    flush();
    $command = "htmldoc --no-localfiles --no-compression -t pdf14 " .
        "--quiet --jpeg --webpage  '$url'";
    passthru($command);
}

?>
