<?php
$url=$_SERVER['REQUEST_URI'];
header("Refresh: 5; URL=$url");  // Refresh the webpage every 5 seconds
?>
<html>
<head>
    <title>Temperature</title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
</head>
    <body><div align="center">
<?php
 
    $con=mysqli_connect("localhost","root","Harm1ess","arduino");
    $result = mysqli_query($con,'SELECT time,value FROM temp ORDER BY time DESC LIMIT 1');
    $ctemp = mysqli_fetch_array($result);
    $time = strtotime($ctemp['time']);
    $view = date("l, F d @  g:i A", $time);
    echo $view;
    echo "<h1>" . $ctemp['value'] . " *F</h1>";
    mysqli_close($con);
?>
        
    <table border="1" cellspacing="0" cellpadding="6">
      <tr>
        <td>Month</td>
        <td>Day</td>
        <td>High</td>
        <td>Low</td>
     </tr>
      
<?php
 
    $con=mysqli_connect("localhost","root","Harm1ess","arduino");
    $result = mysqli_query($con,'SELECT MONTH(time), DAY(time), MAX(value), MIN(value) FROM `temp` GROUP BY MONTH(time) DESC, DAY(time) DESC');
    while($row = mysqli_fetch_array($result))
    {      
        echo "<tr><td>";
        echo $row['MONTH(time)'];
        echo "</td><td>";
        echo $row['DAY(time)'];
        echo "</td><td>";
        echo $row['MAX(value)'];
        echo "</td><td>";
        echo $row['MIN(value)'];
        echo "</td></tr>";
    }
    mysqli_close($con);
?>
    </table>
    </div>
    </body>
</html>
