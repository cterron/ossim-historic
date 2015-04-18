<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>Insert new port group</h1>

<form method="post" action="newsingleport.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th>Port number</th>
    <td class="left"><input type="text" name="port" size="5"></td>
  </tr>
  <tr>
    <th>Protocol</th>
    <td class="left">
      <select name="protocol">
        <option value="udp">UDP</option>
        <option value="tcp">TCP</option>
      </select>
    </td>
  </tr>
  <tr>
    <th>Service</th>
    <td class="left"><input type="text" name="service"></td>
  </tr>
  <tr>
    <th>Description</th>
    <td class="left">
      <textarea name="descr" rows="2" cols="20"></textarea>
    </td>
  </tr>
  <tr>
    <td colspan="2" align="center">
      <input type="submit" value="OK">
      <input type="reset" value="reset">
    </td>
  </tr>
</table>
</form>

</body>
</html>

