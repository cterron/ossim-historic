<html>
<head>
  <title>OSSIM Framework</title>
  <meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1"/>
  <META HTTP-EQUIV="Pragma" CONTENT="no-cache">
  <link rel="stylesheet" type="text/css" href="../style/style.css"/>
</head>
<body>
                                                                                
  <h1>OSSIM Framework</h1>
  <h2>Insert new host</h2>

<form method="post" action="newhost.php">
<table align="center">
  <input type="hidden" name="insert" value="insert">
  <tr>
    <th>Hostname</th>
    <td class="left"><input type="text" name="hostname"></td>
  </tr>
  <tr>
    <th>IP</th>
    <td class="left"><input type="text" value="<?php echo $ip ?>" name="ip"></td>
  </tr>
  <tr>
    <th>Asset</th>
    <td class="left">
      <select name="asset">
   <!-- <option value="0">0</option> -->
        <option value="1">1</option>
        <option value="2">2</option>
        <option value="3">3</option>
        <option value="4">4</option>
        <option value="5">5</option>
        <option value="6">6</option>
        <option value="7">7</option>
        <option value="8">8</option>
        <option value="9">9</option>
        <option value="10">10</option>
      </select>
    </td>
  </tr>
  <tr>
    <th>Threshold C</th>
    <td class="left"><input type="text" name="threshold_c" size="4"></td>
  </tr>
  <tr>
    <th>Threshold A</th>
    <td class="left"><input type="text" name="threshold_a" size="4"></td>
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

