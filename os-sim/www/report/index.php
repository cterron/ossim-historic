<html>
<head>
<title> OSSIM </title>
</head>

<frameset cols="18%,82%" border="0" frameborder="0">
<frame src="menu.php?host=<?php echo $_GET["host"] ?>">

<?php 
    /* inventory */
    if (!strcmp($_GET["section"], 'inventory')) {
        echo "<frame src=\"inventory.php?host=" . $_GET["host"] . "\" name=\"report\">";
    }
    
    /* metrics */
    elseif (!strcmp($_GET["section"], 'metrics')) {
        echo "<frame src=\"metrics.php?host=" . $_GET["host"] . "\" name=\"report\">";
    }

    /* default */
    else {
        echo "<frame src=\"inventory.php?host=" . $_GET["host"] . "\" name=\"report\">";
    }
?>

<frame src="inventory.php?host=<?php echo $_GET["host"] ?>" name="report">
<body>
</body>
</html>

