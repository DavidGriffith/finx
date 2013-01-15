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
header("Cache-Control: no-store, no-cache, must-revalidate");  
header("Cache-Control: post-check=0, pre-check=0", false);
header("Pragma: no-cache");                          


include ("HandleDB.php");
checkPermission();

include ("conf.php");
include ("Messages.php");
include ("includes/Top.php");


$messageType="";
$error=false;

if(isset($_POST['delete_x']))
{
  $checkResult=deleteUser($_POST['selUser']);
  
  if($checkResult==0)
  {
   $messageType="ErrorDelete";
   $error=true;  
  }
  elseif($checkResult==-1)
  {
   $messageType="ErrorDeleteAdmin";
   $error=true;  
  }
  elseif($checkResult==1)
  {
   $messageType="ConfirmDelete";
   $error=true;  
  }
    
}  
  
  
$bgTable="#FFFAFF";
?>
<center>
<!--[if IE 6]>
<script type="text/javascript">
function calcWidth() {
document.getElementById('min_width').style.width = (document.body.clientWidth < 901 ? '900px' : '100%')
}
onresize = calcWidth;
</script>
<![endif]-->
<? include("includes/Toolbar.php");?> <div id="min_width">
<form name="userList" id="userList" method="post" action="<?=$_SERVER['PHP_SELF']?>">
<!--SectionIcon Start-->
<table border="0" cellspacing="0" cellpadding="0" class="header3" width="100%" style="min-width:900px;">
<tr>
 <td><img src="<?=$sharedSkin?>/icon-usersList.png" width="34" height="26" border="0"></td>
 <td class="txtHeader3" nowrap align="left">User List</td>
</tr>
</table>
<!--SectionIcon Stop-->

<table width="98%" cellspacing="0" cellpadding="0" boder="0" class="bgHedearServer" height="26" style="min-width:900px;margin-top:15px; border_right:0px; border-bottom:0px;">
<tr>
 <td height="26" class="border_bottom txtHeaderTable" width="113">&nbsp;Username</td>
 <td class="border_bottom txtHeaderTable" width="80">&nbsp;User Type</td>
 <td class="border_bottom txtHeaderTable" width="160">&nbsp;Server List</td>
 <td class="border_bottom txtHeaderTable" width="*">&nbsp;Session List</td>
 <td class="border_bottom txtHeaderTable" width="160">&nbsp;NX Login Mode</td>
</tr>
<!-- </table> -->



<?

$userList=userListAll();

//$numUser=mysql_num_rows($userList);

$countColor=1;

while($user=mysql_fetch_array($userList))
{
 
  $class="row2";
 
 if($countColor%2) $class="row1";

 $countColor++;
 $checked="";
 if($countColor==2) $checked=" checked"; 


?>

 <tr class="<?=$class?>">
  <td width="100" valign="middle" style="padding: 5px;" class="border_right border_bottom txtLabel">
   <table>
    <tr>
     <td>
      <input name="selUser" id="selUser" value="<?=$user['id']?>" type="radio" align="left" <?=$checked?> >
     </td>
     <td class="textBlack">
      <?=$user['userName']?>
     </td>
   </tr>  
   </table>
  </td>
  <td width="65" valign="middle" align="center" style="padding: 5px;" class="border_right border_bottom txtLabel">
  <?
  if ($user['identity']=="administrator") 
  {
   echo '<img src="'.$sharedSkin.'/administrator.png" border="0" >';
  }
  elseif($user['identity']=="user")
  {
   echo '<img src="'.$sharedSkin.'/user.png" border="0">';
  }  
  elseif($user['identity']=="guest")
  {
   echo '<img src="'.$sharedSkin.'/guest.png" border="0">'; 
  }
  ?>
  </td>
  <td width="150" valign="middle" style="width:240px; padding: 5px;" class="border_right border_bottom txtLabel">
  
   <select name="serverList<?=$user['id']?>" id="serverList" class="txtLabel"  onchange="javascript: sessionList('respSessions.php', this.value, <?=$user['id']?>)">
<?
$serverList = getServer();
$server = mysql_fetch_array($serverList);
$idFirst=$server['id'];
while($server)
{

?>
 <option value="<?=$server['id']?>"><?=$server['hostName']?></option>
<?
 $server = mysql_fetch_array($serverList);
}
?>
   </select>
  </td>
  <td width="*" valign="top" cellspacing="0" cellpadding="0" style="padding: 5px;" class="border_right border_bottom txtLabel">
  <div id="respose-<?=$user['id']?>">
   <table cellspacing="2" cellpadding="0" border="0">
   <tr>
   <?
   $countSession=0;
   if($user['identity']!="administrator")
   {
    $sessionList = sessionListUser($user['id'], $idFirst);
   }
   else
   {
    $sessionList = getSessionsList($idFirst);
   }
   
   $session=mysql_fetch_array($sessionList);
   if(!$session)
   {
   ?>
       <td align="center">
        <img src="images/shared/empty.png" border="0" width="62" height="50">
      </td>
   <? 
   
   }

   while($session)
   {
     $countSession++; 
     if($session['icon']=="") $session['icon']="empty.png"
   ?>
       <td align="left">
       
        <table width="63" border="0" cellpadding="1" cellspacing="0">
        <tr>
         <td valign="top" align="center"> <img src="<?=$sessionShared?>/<?=$session['icon']?>" width="62" height="50" title="<?=$session['sessionName']?>" ></td>
        </tr>
        <tr> 
         <td valign="top" align="center"> 
           <span class="titleBlueSmall" style="cursor: pointer;" title="<?=$session['sessionName']?>"><?=getSessionTitle($session['sessionName'],7)?></span>
         </td>
        </tr>
        </table>     
        
      </td>
   <?
    if($countSession==5)
    {
     echo "</tr><tr>";
     $countSession=0;
    }
    $session=mysql_fetch_array($sessionList);
   } 
   ?>
    </tr>
   </table>
   </div>
  </td>
  <td width="160" valign="middle" align="center"  class="border_bottom txtLabel">
  <?
   if($user['credentialsNX']=="2") { echo 'Use Web credentials to access NX'; } //<img align="center" src="'.$sharedSkin.'/check.png" >
   elseif($user['credentialsNX']=="1") { echo 'Login to NX server as a Guest User'; }
   elseif($user['credentialsNX']=="0") { echo 'Provide access credentials via NX Client'; }
  
  ?>
  </td>
 </tr>
<?
}
?>
</table>

<? if($messageType!="") { ?>
<div class="message">
<p class="t_2orange" style="margin:0px;padding:0px;line-height:27px;">
 <img src="<?=$sharedSkin?>/icon_alertWhite.png" align="middle">
 <?=$Message[$messageType]?>
</p></div>
<? } ?>


<table cellSpacing="0" cellPadding="0" width="98%" height="27" class="bgMiniBar">
<tr>
  <td width="100%"></td>
  <td width="100" align="center"><a href="AddUser.php"><img style="cursor: pointer; margin-right:10px;"  width="82" height="17" border="0" src="<?=$button?>/b_add.png" onmouseout="this.src='<?=$button?>/b_add.png'"  onmouseover="this.src='<?=$button?>/b_addOver.png'"></a></td> 
  <td width="100" align="center"><img width="82" height="17" border="0" src="<?=$button?>/b_modify.png" onclick="sendFormTo('userList','ModifyUser.php')" onmouseout="this.src='<?=$button?>/b_modify.png'"  onmouseover="this.src='<?=$button?>/b_modifyOver.png'" style="margin-right:10px; cursor: pointer;"></td>
  <td width="100" align="center"><input type="image" name="delete" id="delete"  width="82" height="17" border="0" src="<?=$button?>/b_delete.png"  onmouseout="this.src='<?=$button?>/b_delete.png'" onmouseover="this.src='<?=$button?>/b_deleteOver.png'" style="margin-right:10px;"></td>
</tr>
</table>
</form>
</div>
<? include("includes/Footer.php") ?>
