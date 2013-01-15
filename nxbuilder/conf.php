<?php

$css="";
if(basename($_SERVER['PHP_SELF'])=="Configuration.php")
{
 $skinQuery=getColorTemplate();

 $skin=mysql_fetch_array($skinQuery);

 $css=$skin['name'];
}

if(empty($css))
{
 $css="defaultGuest";
}

// path without end / 

$shared="images/shared";
$serverShared="images/servers";
$sessionShared="images/sessions";

$button="images/buttons";
$sharedSkin="images/shared";
$toolbarSkin="images/toolbar";

?>
