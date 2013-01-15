<?php
/**************************************************************************/
/*                                                                        */
/* Copyright (c) 2005, 2011 NoMachine, http://www.nomachine.com.           */
/*                                                                        */
/* NXBUILDER, NX protocol compression and NX extensions to this software  */
/* are copyright of NoMachine. Redistribution and use of the present      */
/* software is allowed according to terms specified in the file LICENSE   */
/* which comes in the source distribution.                                */
/*                                                                        */
/* Check http://www.nomachine.com/licensing.html for applicability.       */
/*                                                                        */
/* NX and NoMachine are trademarks of Medialogic S.p.A.                   */
/*                                                                        */
/* All rigths reserved.                                                   */
/*                                                                        */
/**************************************************************************/

session_start();
include ("HandleDB.php");
include ("conf.php");
include ("Messages.php");
?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>

<head>
<title> NX Builder</title>

<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
<link rel="stylesheet" type="text/css" href="style/default.css" media="all" >
<link rel="stylesheet" type="text/css" href="style/general.css" media="all" >
<script src="js/browser.js"  language=javaScript type="text/javascript"></script>
<script src="js/function.js"  language=javaScript type="text/javascript"></script>
<script src="js/panel.js"  language=javaScript type="text/javascript"></script>
<LINK REL="icon" HREF="<?=$sharedSkin?>/favicon.ico" TYPE="image/gif">
<script>
 function sendFormUser()
 {
  var form=document.getElementById('Login');
  form.action="ModifyUser.php";
  form.submit();
 }
 
</script>
</head>
<body onLoad="siteDownload()">

<center>


<? 
if( isset($_SESSION['userName']) && (!isset($_SESSION['error'])) )
{
 include("includes/Toolbar.php"); 
 $classLogin="logintableLogged";
}
else
{
 include("includes/Toolbardisable.php");
 $classLogin="logintable";
}

$noCheck="";
if(!file_exists("builder.cfg"))
{
 $noCheck="cfgNoFound";
 $_SESSION['NoCfg']=$Message[$noCheck];
}
else
{
 checkForUpdate();
}


?>

<noscript>
    <table align="center" width="60%" cellpadding="1" cellspacing="0" style="margin-top:15px;>       
     <tr>
       <td width="20" align="center"><img src="<?=$shared?>/info.png" border="0"></td><td class="t_2orange">Please enable javascript</td>
     </tr>
    </table>
</noscript>

<form name="Login" id="Login" ecntype="multipart/form-data" action="auth.php" method="post">
<table border="0" width="60%" height="250" align="center" cellspacing="0" cellpadding="0" class="<?=$classLogin?>">
<tr>
  <td  valign="top">
<? 
if((!isset($_SESSION['userName']) || empty($_SESSION['password'])) )
{
?>
   <table align="right" class="logintableinside">
    <tr>
     <td  colspan="2"><? echo date("F j, Y, g:i a"); ?></td><td></td>
    </tr>
<?
 if(empty($noCheck))
 {
?>
   
    <tr>
     <td height="15" colspan="2" style="border-top:1px solid <?=$borderGrey?>;">&nbsp;</td>
    </tr>
    <tr>
     <td  class="titleGrey" width="50%">Username:</td><td> <input type="text" name="userName" id="userName" style="width:150px;" class="textGrey"></td>
    </tr>
    <tr>
     <td class="titleGrey" width="50%">Password:</td><td> <input type="password" name="password" id="password" style="width:150px;" class="textGrey"></td>
    </tr>
    <tr>
     <td class="titleGrey" width="50%" >Login as a Guest Builder user</td><td><input type="checkbox" name="guestUser" style="margin:0px;" class="textGrey" onclick="setLoginFiels(this)"></td>
    </tr>
    <tr>
     <td height="15" colspan="2" align="center" style="border-bottom:1px solid <?=$borderGrey?>;">&nbsp;
     </td>
    </tr>
    <tr>
     <td colspan="2" align="center">
     
<?
if(isset($_SESSION['error']) && (empty($noCheck))  )
{
?>
   <tr>
    <td  colspan="2">
    <table align="center" width="100%" cellpadding="1" cellspacing="0" style="margin-top:15px;">       
     <tr>
       <td width="20"><img src="<?=$shared?>/info.png" border="0"></td><td class="t_2orange"><?=$_SESSION['error']?></td>
     </tr>
    </table>
    </td>
   </tr>
<?
}
?>
   <tr>
    <td colspan="2" align="center">
      <input name="login" style="margin-top:15px;" type="image" width="82" height="17" src="<?=$button?>/b_login.png" border="0" onmouseout="this.src='<?=$button?>/b_login.png'" onmouseover="this.src='<?=$button?>/b_loginOver.png'">
    </td>
  </tr>
     </td>
   </tr> 
<?
 }
 if(!empty($noCheck)) 
 {
?>
   <tr>
    <td  colspan="2">
    <table align="center" width="100%" cellpadding="1"  cellspacing="0" style="margin-top:15px;">       
     <tr>
       <td width="20"><img src="<?=$shared?>/info.png" border="0"></td><td class="t_2orange"><?=$_SESSION['NoCfg']?></td>
     </tr>
    </table>
    </td>
   </tr>
<?
 }      
?>   
   </table>
<?
}
else   
{   
?>
   <table align="right" cellpadding="0" cellspacing="0" border="0" class="logintableinside">
    <tr>
     <td colspan="2"><? echo date("F j, Y, g:i a"); ?></td>
    </tr>
    <tr>
     <td height="20">&nbsp;</td>
    </tr>
    <tr>
     <td height="30"  valign="middle" align="left" class="titleGrey"  width="80">Logged as: </td><td align="left"><strong> <?=$_SESSION['userName']?> </strong></td>
    </tr>
    <tr>
     <td  height="30" valign="middle" align="left" class="titleGrey" width="80">Profile: </td><td align="left"> <strong><?=ucfirst($_SESSION['identity'])?></strong></td>
    </tr>
    <tr>
     <td height="15" colspan="2" align="center" class="border_bottom">&nbsp;
     </td>
    </tr>
     <tr>
     <td colspan="2" >
       <table align="center" width="100%" cellpadding="1" cellspacing="0" style="margin-top:15px;">
       <tr>
        <td width="20"><img src="<?=$shared?>/info.png" border="0"></td><td class="t_2orange">User logged in as <?=ucfirst($_SESSION['identity'])?></td>
       </tr>
       </table>
     </td>
    </tr>
    <tr>
     <td colspan="2" align="center">
      <input type="hidden" name="selUser" value="<?=$_SESSION['idUser']?>">
     <?
     if($_SESSION['identity']=="administrator")
     {
     ?>
      <input type="image" src="<?=$button?>/b_yourAccount.png" width="82" height="17"  border="0" onmouseout="this.src='<?=$button?>/b_yourAccount.png'" onmouseover="this.src='<?=$button?>/b_yourAccountOver.png'"  name="chAccount" style="margin-top:15px;margin-right:35px;" onclick="sendFormUser();">
      <!-- <input name="chAccount" style="margin-top:15px;" onclick="sendFormUser();" alt="Your Account" type="image" width="56" height="17" src="<?=$button?>/b_login.png" border="0" onmouseout="this.src='<?=$button?>/b_login.png'" onmouseover="this.src='<?=$button?>/b_loginOver.png'"> -->
     <?
     }
     ?> 
      <input name="logOff" style="margin-top:15px;" alt="Log Off" type="image" width="82" height="17" src="<?=$button?>/b_logoff.png" border="0" onmouseout="this.src='<?=$button?>/b_logoff.png'" onmouseover="this.src='<?=$button?>/b_logoffOver.png'">
     </td>
   </tr>
   </table>

<?

}

?>

  </td>
</tr>
</table>
</form>

<? include("includes/Footer.php") ?>
