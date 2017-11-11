<?php
    $link = mysqli_connect("localhost", "root", "Harm1ess", "arduino");

    $sql = "INSERT INTO temp (value) VALUES ('".$_GET["value"]."')";    

    if (mysqli_query($link, $sql) === TRUE) {
      printf("success");
    }
    else echo("Error: " . mysqli_error($link));
?>
