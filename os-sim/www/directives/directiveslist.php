<?php
require_once ('classes/Session.inc');
Session::logcheck("MenuCorrelation", "CorrelationDirectives");
?>

<?php
    $XML_FILE = '/etc/ossim/server/directives.xml';
?>

<html>
<head>
  <title> Directives Editor </title>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" href="../style/style.css"/>
  <link rel="stylesheet" href="../style/directives.css"/>
</head>

<script language="JavaScript1.5" type="text/javascript">
<!--

function Menus(Objet)
{
	VarDIV=document.getElementById(Objet);
	if(VarDIV.className=="menucache") {
	    VarDIV.className="menuaffiche";
	} else {
	    VarDIV.className="menucache";
	}
}	
//-->
</SCRIPT>

<body>
<h1 align="center">Directives list</h1>
<?php

   /* create dom object from a XML file */
   if(!$dom = domxml_open_file($XML_FILE, DOMXML_LOAD_SUBSTITUTE_ENTITIES)) {
      echo "Error while parsing the document\n";
      exit;
   }
   
   /* Listing of directives */
   foreach ($dom->get_elements_by_tagname('directive') as $directive) {
      $id   = $directive->get_attribute('id');
      $name = $directive->get_attribute('name');
      switch ($id) {
         case 1:
?>
	     <a style="cursor:hand;" TITLE="To view or hide this type of directives click here." onclick="Menus('gen')">
	          <font style="font-size: 10pt;">Generic ossim</font></a>
	     <div id="gen" class="menuaffiche">
	     <table><tr><th>Id<th>Name
<?php	
            break;
	 case 3000: ?>
	     </table></div><br/>
	     <a style="cursor:hand;" TITLE="To view or hide this type of directives click here." onclick="Menus('attackcorr')">
	          <font style="size: 10pt;">Attack Correlation</font></a>
	     <div id="attackcorr" class="menucache">
	     <table><tr><th>Id<th>Name
<?php
             break;
	 case 6000: ?>
	     </table></div><br/>
	     <a style="cursor:hand;" TITLE="To view or hide this type of directives click here." onclick="Menus('virus')">
	         <font style="font-size: 10pt;">Virus and worms</font></a>
	     <div id="virus" class="menucache">
	     <table><tr><th>Id<th>Name
<?php
	     break;
	 case 9000: ?>
	     </table></div><br/>
	     <a style="cursor:hand;" TITLE="To view or hide this type of directives click here." onclick="Menus('wattackcorr')">
	         <font style="font-size: 10pt;">Web attack correlation</font></a>
	     <div id="wattackcorr" class="menucache">
	     <table><tr><th>Id<th>Name
<?php
             break;
	 case 12000: ?>
	      </table></div><br/>
	      <a style="cursor:hand;" TITLE="To view or hide this type of directives click here." onclick="Menus('dos')">
	          <font style="font-size: 10pt;">DoS</font></a>
	      <div id="dos" class="menucache">
	      <table><tr><th>Id<th>Name
<?php
             break;
	 case 15000:  ?>
	     </table></div><br/>
	     <a style="cursor:hand;" TITLE="To view or hide this type of directives click here." onclick="Menus('portscan')">
	         <font style="font-size: 10pt;">Portscan/Scan</font></a>
	     <div id="portscan" class="menucache">
	     <table><tr><th>Id<th>Name
<?php
             break;
	 case 18000: ?>
	     </table></div><br/>
	     <a style="cursor:hand;" TITLE="To view or hide this type of directives click here." onclick="Menus('anomalies')">
	         <font style="font-size: 10pt;">Behaviour anomalies</font></a>
	     <div id="anomalies" class="menucache">
	     <table><tr><th>Id<th>Name
<?php
             break;
	 case 21000: ?>
	     </table></div><br/>
	     <a style="cursor:hand;" TITLE="To view or hide this type of directives click here." onclick="Menus('abuse')">
	         <font style="font-size: 10pt;">Network abuse and error</font></a>
	     <div id="abuse" class="menucache">
	     <table><tr><th>Id<th>Name
<?php
             break;
         case 24000: ?>
             </table></div><br/>
	     <a style="cursor:hand;" TITLE="To view or hide this type of directives click here." onclick="Menus('trojans')">
	         <font style="font-size: 10pt;">Trojans</font></a>
             <div id="trojans" class="menucache">
	     <table><tr><th>Id<th>Name
<?php
             break;
	 case 27000: ?>
	     </table></div><br/>
	     <a style="cursor:hand;" TITLE="To view or hide this type of directives click here." onclick="Menus('misc')">
	         <font style="font-size: 10pt;">Miscellaneous</font></a>
	     <div id="misc" class="menucache">
	     <table><tr><th>Id<th>Name
<?php
             break;
	 case 500000: ?>
	     </table></div><br/>
	     <a style="cursor:hand;" TITLE="To view or hide this type of directives click here." onclick="Menus('usercont')">
	         <font style="font-size: 10pt;">User contributed</font></a>
	     <div id="usercont" class="menucache">
	     <table><tr><th>Id<th>Name
<?php
             break;
      } ?>

      <tr><td style="text-align: left;">
      <?php echo $id; ?>
      
      <td style="text-align: left;">
      <a href="directive.php?level=1&directive=<?php echo $id . "\" target=\"directives\">" . $name."</a></td></tr>";
   }
					      
?>
</table>
</div>
</body>
</html>
