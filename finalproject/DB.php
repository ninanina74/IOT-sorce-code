<?php 
$GLOBALS['host'] = "localhost";
$GLOBALS['user'] = "aiotfb";
$GLOBALS['password'] = "aiotfbadmin";
$GLOBALS['db'] = "AIoTFinalProject";

function query($sql)
{
    try {
        $conn = mysqli_connect($GLOBALS['host'], $GLOBALS['user'], $GLOBALS['password'], $GLOBALS['db']);    
        mysqli_query($conn, "set names utf8");
        $result = mysqli_query($conn, $sql);

        $finalResult = [];
        
        while($row = mysqli_fetch_assoc($result))
        {
            $finalResult[] = $row;
        }      

        return $finalResult;
    } catch (Exception $e) {
        return $e->getMessage();
    } finally {
        mysqli_close($conn);
    }
}

function insertOrUpdate($sql)
{
    try {
        $conn = mysqli_connect($GLOBALS['host'], $GLOBALS['user'], $GLOBALS['password'], $GLOBALS['db']);    
        mysqli_query($conn, "set names utf8");
        $conn->query($sql);
    } catch (Exception $e) {
        return $e->getMessage();
    } finally {
        mysqli_close($conn);
    }
}
?>
