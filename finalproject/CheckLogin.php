<?php 
session_start();
if ($_SESSION['account'] == "") {
    header("Location: Login.php");
    die();
}
?>
