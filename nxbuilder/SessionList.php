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


include_once ("HandleDB.php");
include_once ("conf.php");
checkPermission();
$messageType="";

$error=false;

if(isset($_POST['delete_x']))
{
 $error=true;
 if(deleteSession($_POST['sessID']))
 {
  $messageType="SessionDeleted";
 }
 else
 {
  $messageType="ErrorSessionDelete";
 }
}

if(isset($_POST['connect_x']) &&  $_POST['sessID']!="")
{


$fileName=getFileName($_POST['sessID']);

if(ereg("MSIE ([0-9].[0-9]{1,2})", $_SERVER["HTTP_USER_AGENT"])) {

header("Content-Type: application/nxs");
header("Content-Disposition: inline; filename=$fileName");
header("Expires: 0");
header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
header("Pragma: private");

} else {

header("Content-Type: application/nxs");
header("Content-Disposition: attachment; filename=$fileName");
header("Expires: 0");
header("Pragma: no-cache");

}

include ("CreateSession.php");

$choose=explode(",",$_POST['sessID']);

$k="resolution-".$choose[1];
$f="link-".$choose[1];
 
CreateSessionFile3_0($_POST['sessID'], $_POST[$k], $_POST[$f]); 
exit();



}


include ("Messages.php");
include ("includes/Top.php");

if(isset($_POST['operation']) && ($_POST['operation']=="delete") )
{
 deleteSession($_POST['sessID']);
}

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
<? include("includes/Toolbar.php");?>
<div id="min_width">
<form action="<?=$_SERVER['PHP_SELF']?>" method="post" id="SessionForm" name="SessionForm" style="margin: 0px; padding: 0px;">
<!--SectionIcon Start-->
<table border="0" cellspacing="0" cellpadding="0" class="header3" width="100%" style="min-width:900px;">
<tr>
 <td><img src="<?=$sharedSkin?>/icon-sessionList.png" width="34" height="26" border="0"></td>
 <td class="txtHeader3" nowrap align="left">Session List</td>
</tr>
</table>
<!--SectionIcon Stop-->

<?
 $serverList1=getServer();
 $numberSessionAvailable1=mysql_num_rows($serverList1);
  
 
 $server1=mysql_fetch_array($serverList1);
 $sessionList1 = getSessionsListAll();
 $numSess1=mysql_num_rows($sessionList1);
if($numSess1==0)
{
 $error=1;
 $messageType="NoSessionAvailable";
} 
 

?>

<table border="0" width="98%" cellspacing="0" cellpadding="0" class="bgHedearServer" style="min-width:900px; margin:0px;margin-top:15px;">
<tr height="26">
 <td width="30%" class="txtHeaderTable">&nbsp;Session</td>
 <td width="20%" class="txtHeaderTable">&nbsp;Desktop</td>
 <td width="25%" class="txtHeaderTable">&nbsp;Application</td>
 <td width="285" class="txtHeaderTable">&nbsp;Server</td>
 </tr>
</table>

 
<table width="98%" cellspacing="0" cellpadding="0" border="0" style="min-width:900px;">


<?



$countColor=0;

$checkFirs=1;

 $serverList=getServer();
 @$numberSessionAvailable=mysql_num_rows($serverList);

while($server=mysql_fetch_array($serverList))
{
 


 $sessionList = getSessionsList($server['id']);
 @$numSess=mysql_num_rows($sessionList);
  
 if($numSess>0)
 { 

 $class="row2";
 
 if($countColor%2)
 {
   $class="row1";
 }
 

 

 $countColor++;

?>
<tr>
 <td  class="<?=$class?>" valign="top">

<?
 
 
 $session = mysql_fetch_array($sessionList);

 

 
 while($session)
 {

  $check=" ";
  if(($countColor==1)&&($checkFirs==1))
  {
   $check=" checked";
   $checkFirs=0;
  }
  
 $adsl="";
 $modem="";
 $isdn="";
 $wan="";
 $lan="";
 if($server['defaultLink'] == "ADSL") $adsl="selected";
 elseif($server['defaultLink'] == "MODEM") $modem="selected";
 elseif($server['defaultLink'] == "ISDN") $isdn="selected";
 elseif($server['defaultLink'] == "WAN") $wan="selected";
 elseif($server['defaultLink'] == "LAN") $lan="selected";
   
 if($server['displayLink'] == "1")  $disabledLink = "none";  
 if($server['displayLink'] == "0")  $disabledLink = "block";

?>
 
  <table class="<?=$class?>" width="100%"  cellspacing="0" border="0" style="border-left:0px; border-right:0px;">
  <tr> 
   <td width="20" height="25" valign="middle" > <input name="sessID" id="sessID" type="radio" align="left" value="<?=$session['id']?>,<?=$server['id']?>" <?=$check?>></td> 
   <td width="280"  height="25" align="left" class="txtValue" valign="middle">
      <?=$session['sessionName']?>
   </td>
   <td width="200" height="25" align="left" valign="middle" class="txtValue">
   <?
    if($session['sessionType']!="custom")
     echo $session['sessionType'];
    else
     echo "-" 
   ?>
   </td>
   <td align="left" height="25" valign="middle" class="txtValue">
    
     <?
      if($session['sessionType']=="custom")
       echo $session['sessionType'];
      else
       echo "-" 
     ?>   
    
   </td>      
  </tr>
  </table> 
<?
  @$session = mysql_fetch_array($sessionList);
 } 
?>
    <div style="height: 20px; width: 97%;">&nbsp;</div>
 </td>
 <td  class="<?=$class?>" valign="middle" align="center" width="285"  style="border-left:0px;">
   <table  width="255" cellspacing="0" cellpadding="5" border="0" align="center">
   <tr>
     <td colspan="4" class="textBlack">
       <b>NX Server:</b> <?=$server['serverName']?> 
     </td>
   </tr>
    <tr>
     <td width="122">
       <?
        if(trim($server['icon'])=="")
        {       
       ?>
       <img width="122" height="93" src="<?=$serverShared?>/imgEmpty.png" style="margin:5px; margin-left:0px;">
       <?
        }
        else
        {
       ?>
       <img width="122" height="93" src="<?=$serverShared?>/<?=$server['icon']?>" style="margin:5px; margin-left:0px;">
       <?
        }
       ?>        
     </td>
     <td valign="bottom" align="left" style="padding-bottom:10px;">
     <div style="display:<?=$disabledLink?>;">
      <span class="textBlack">Link &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>
      <select name="link-<?=$server['id']?>" class="textBlack">
       <option value="ADSL" <?=$adsl?> >ADSL</option>
       <option value="MODEM" <?=$modem?> >MODEM&nbsp;&nbsp;&nbsp;&nbsp;</option>
       <option value="ISDN" <?=$isdn?> >ISDN</option>
       <option value="WAN" <?=$wan?> >WAN</option>
       <option value="LAN" <?=$lan?> >LAN</option>
      </select><br />
      </div><br />
      <span class="textBlack">Display&nbsp;&nbsp;</span>
      <select name="resolution-<?=$server['id']?>" class="textBlack" style="width:85px;">
       <option value="800x600">800x600</option>
       <option value="1024x768">1024x768</option>
       <option value="availablearea">Available area</option>
       <option value="fullscreen">Fullscreen</option>
       <? if(stristr($_SERVER['HTTP_USER_AGENT'], "Windows")) {?>  
       <option value="available_area_mm">Available area, Multi-head</option>
       <option value="fullscreen_mm">Fullscreen, Multi-head </option> 
       <?
        }
       ?>      
      </select>
     </td>
    </tr>
   </table>
 </td>
</tr>
<?
 }//end if($numSess>0)
}//end while($server=mysql_fetch_array($serverList))
?>


</table> 

<? if($messageType!="") { ?>
<div class="message">
<p class="t_2orange" style="margin:0px;padding:0px;line-height:27px;">
 <img src="<?=$sharedSkin?>/icon_alertWhite.png" align="middle">
 <?=$Message[$messageType]?>
</p></div>
<? } ?>

<?
 if($numSess1>0)
 {
?>
<!--<div  style="width:98%; height: 27px; min-width:900px; border: 1px solid #919B9D; background-color: #ECE9D8;margin-top:20px;"> </div>-->
<table cellSpacing="0" align="center" cellPadding="0" width="98%" height="27" class="bgMiniBar">
<tr>
  <td width="100%"></td>
  <td align="center" valign="top"><input type="image" name="connect" id="connect" style="cursor: pointer; margin-right:10px; margin-top: 5px;" width="82" height="17" border="0" src="<?=$button?>/b_connect.png"  onmouseout="this.src='<?=$button?>/b_connect.png'"  onmouseover="this.src='<?=$button?>/b_connectOver.png'" ></td>
   <td width="100" align="center"><a href="AddSession.php"><img style="cursor: pointer; margin-top:3px; margin-right:10px;"  width="82" height="17" border="0" src="<?=$button?>/b_add.png" onmouseout="this.src='<?=$button?>/b_add.png'"  onmouseover="this.src='<?=$button?>/b_addOver.png'"></a></td>
  <td align="center" valign="top"><input type="image" name="modify" id="modify" style="cursor: pointer;margin-right:10px; margin-top: 5px;" onclick="sendFormTo('SessionForm', 'ModifySession.php');" width="82" height="17" border="0" src="<?=$button?>/b_modify.png"  onmouseout="this.src='<?=$button?>/b_modify.png'"  onmouseover="this.src='<?=$button?>/b_modifyOver.png'" ></td>
  <td align="center" valign="top" ><input type="image" name="delete" id="delete" style="cursor: pointer;margin-right:10px;margin-top: 5px;"  width="82" height="17" border="0" src="<?=$button?>/b_delete.png"  onmouseout="this.src='<?=$button?>/b_delete.png'" onmouseover="this.src='<?=$button?>/b_deleteOver.png'" ></td>
</tr>
</table>
<?
 }
 elseif($numSess1<1)
 {
?>
<table cellSpacing="0" align="center" cellPadding="0" width="98%" height="27" class="bgMiniBar">
<tr>
  <td width="100%"></td>
  <td align="center" valign="top"><img name="connect" id="connect" style="margin-right:10px; margin-top: 5px;" width="82" height="17" border="0" src="<?=$button?>/b_connectDsb.png"></td>
  <?
  $serverQuery=getServer();
  @$numServer=mysql_num_rows($serverQuery);
  
  if($numServer>0)
  {
  ?>
   <td width="100" align="center"><a href="AddSession.php"><img style="margin-top:3px; margin-right:10px;"  width="82" height="17" border="0" src="<?=$button?>/b_add.png" onmouseout="this.src='<?=$button?>/b_add.png'"  onmouseover="this.src='<?=$button?>/b_addOver.png'"></a></td>
  <?
  }
  else
  {
  ?>
  <td align="center" valign="top"><img style="margin-right:10px; margin-top: 5px;" width="82" height="17" border="0" src="<?=$button?>/b_addDsb.png"></td>  
  <?
  }
  ?>
  <td align="center" valign="top"><img name="modify" id="modify" style="margin-right:10px; margin-top: 5px;"  width="82" height="17" border="0" src="<?=$button?>/b_modifyDsb.png"></td>
  <td align="center" valign="top" ><img name="delete" id="delete" style="margin-right:10px;margin-top: 5px;"  width="82" height="17" border="0" src="<?=$button?>/b_deleteDsb.png"></td>
</tr>
</table>
<?
 } 
?>
</div>

</form>





<? include("includes/Footer.php") ?>
