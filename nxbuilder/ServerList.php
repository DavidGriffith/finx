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
checkSmallPermission();

$error="";
$countDesk="";


if(isset($_POST['createSessionId']) && (!detectInjection($_POST['createSessionId'])) )
{

 $_POST['createServerId']=addslashes($_POST['createServerId']);
 
 $f="resolution-".$_POST['createServerId'];
 $k="link-".$_POST['createServerId'];

 $fileName=getFileName($_POST['createSessionId']);


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
 CreateSessionFile3_0($_POST['createSessionId'], $_POST[$f], $_POST[$k]);
 exit();
}


include ("Messages.php");
include ("includes/Top.php");

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
<!--SectionIcon Start-->
<table border="0" cellspacing="0" cellpadding="0" class="header3" width="100%" style="min-width:900px;">
<tr>
 <td><img src="<?=$sharedSkin?>/icon-serverList.png" width="34" height="26" border="0"></td>
 <td class="txtHeader3" nowrap align="left">Server List</td>
</tr>
</table>
<!--SectionIcon Stop-->
<?

if($_SESSION['identity']=="guest") $_SESSION['idUser']=getGuestId();

$serverList = getListServerForUser($_SESSION['idUser']);


$serverNumber = count($serverList);

if($serverNumber==0)
{
 $error=1;
 $messageType="NoServerAvailable";
}
?>



<table border="0" width="98%" cellspacing="0" cellpadding="0" style="min-width:900px;margin:0px;margin-top:15px;" class="bgHedearServer">
<tr>
 <td width="100%" height="26"  class="txtHeaderTable">&nbsp;Server</td>
</tr>
</table>

 <div id="result" style="z-index: 1000; display:none; position:absolute; top:100px; left: 350px; border:1px solid #000; background-color:white;"></div>
<form name="serverList" id="serverList" action="<?=$_SERVER['PHP_SELF']?>" method="post" ENCTYPE="multipart/form-data" style="margin: 0px; padding: 0px;">
<input type="hidden" id="createSessionId" name="createSessionId" value="">
<input type="hidden" id="createServerId" name="createServerId" value="">
<?



$countColor=1;
foreach ($serverList as $list)
{
 

 $serverListById = getServer($list);
 $server = mysql_fetch_array($serverListById);
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

 $class="row1";
 
 if($countColor%2) $class="row2";
 $countColor++;
 $checked="";
 if($countColor==2) $checked=" checked"; 
 
 if($server['displayMode']!="0")
 {
?>
<!-- start table for list 1row*4cols -->
<table width="98%" class="<?=$class?>" cellspacing="0" cellpadding="5" border="0">
 <tr>
  <td width="26" valign="top" class="border_right">
   <div style="width:26px;"><input type="radio" name="idServer" id="idServer" value="<?=$server['id']?>" <?=$checked?> ></div>
  </td>
  <td width="310" valign="top" class="border_right">

   <table  width="310" cellspacing="0" cellpadding="0" border="0">
   <tr>
     <td colspan="2" class="txtLabel">
       <b>NX Server:</b> <?=$server['serverName']?>   
     </td>
     <td colspan="2">
       <img width="17" height="17" src="<?=$shared?>/info.png"  style="cursor:pointer" onmouseover="document.getElementById('idServer').value='<?=$server['id']?>';setResultPosition(this); document.getElementById('result').style.display='block'; setDescrition('resp.php'); " onmouseout="document.getElementById('result').style.display='none';">
     </td>
   </tr>
    <tr>
     <td>
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
     <td valign="bottom" style="padding-bottom:5px;">
     <div style="display:<?=$disabledLink?>;">
      <span class="txtLabel">Link&nbsp;&nbsp;&nbsp;&nbsp;</span>
      <select name="link-<?=$server['id']?>" class="textBlack" ></option>
       <option value="ADSL" <?=$adsl?> >ADSL</option>
       <option value="MODEM" <?=$modem?> >MODEM&nbsp;&nbsp;&nbsp;&nbsp;</option>
       <option value="ISDN" <?=$isdn?> >ISDN</option>
       <option value="WAN" <?=$wan?> >WAN</option>
       <option value="LAN" <?=$lan?> >LAN</option>
      </select><br />
      </div><br />
      <span class="txtLabel">Display</span>
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
  <!--  </div> -->
  </td>

  <?
   if($server['displayMode']=="1")
   {
  ?>
  <td valign="top" align="left" class="txtHeaderTable border_right">
  <div class="bgDivServer" style="height:20px; margin:-5px; margin-bottom:0px; padding:5px; padding-bottom:0px;">Desktop</div>
    <table cellspacing="0" cellpadding="0" border="0" valign="middle">
    <tr>
    <!-- start dinamic desk -->
    <?
    $querySess=getSessionsListForUser($list, $_SESSION['idUser'],"<>");
    $n=mysql_num_rows($querySess);
   
    if($n==0)
    {
    ?>
     <td align="center"><span class="titleBlueSmall"><br><br><br>-</span></td>    
    <? 
    }
    else
    {
    
      while($session=mysql_fetch_array($querySess))
      {
       $countDesk++;
       $countDesk=0;
       $icon="empty.png";
       if($session['icon']!="") $icon = $session['icon'];

    ?>
 
     <td align="left">
      <table width="63" border="0" cellpadding="1" cellspacing="0">
      <tr>
       <td valign="top" align="center"> <img src="<?=$sessionShared?>/<?=$icon?>" style="cursor: pointer;"  width="62" height="50" title="<?=$session['sessionName']?>" onclick="createSession('serverList',<?=$session['id']?>,<?=$server['id']?>)"></td>
      </tr>
      <tr> 
       <td valign="top" align="center"> 
         <span class="titleBlueSmall" style="cursor: pointer;" onclick="createSession('serverList',<?=$session['id']?>,<?=$server['id']?>)" title="<?=$session['sessionName']?>"><?=getSessionTitle($session['sessionName'],7)?></span>
       </td>
      </tr>
      </table>
    </td>     
    <?  
       if($countDesk==4)
        {
         echo "</tr><tr>";
         $countDesk=0;
        }
      }     
    }
    ?>

    <!-- stop dinamic desk -->
    </tr>
   </table>
  </td>

  <td width="80%" valign="top" align="left"  class="txtHeaderTable">
  <div class="bgDivServer" style="height:20px; margin:-5px; margin-bottom:0px; padding:5px; padding-bottom:0px;">Application</div>
   <table cellspacing="0" cellpadding="0" border="0" valign="middle">
    <tr>
    <!-- start dinamic application -->
    
    <?
    $querySessApp=getSessionsListForUser($list, $_SESSION['idUser'],"=");
    $n=mysql_num_rows($querySessApp);
   
    if($n==0)
    {
    ?>
     <td align="center"><span class="titleBlueSmall"><br><br><br>-</span></td>    
    <? 
    }
    else
    {
    
      while($sessionApp=mysql_fetch_array($querySessApp))
      {
       $countApp++;
       $icon="empty.png";
       if($sessionApp['icon']!="") $icon = $sessionApp['icon'];
    ?>

    <td align="left">
      <table width="63" border="0" cellpadding="1" cellspacing="0">
      <tr>
       <td valign="top" align="center"> <img style="cursor: pointer;"  src="<?=$sessionShared?>/<?=$icon?>" width="62" height="50" title="<?=$sessionApp['sessionName']?>"  onclick="createSession('serverList',<?=$sessionApp['id']?>,<?=$server['id']?>)"></td>
      </tr>
      <tr> 
       <td valign="top" align="center"> 
         <span class="titleBlueSmall" style="cursor: pointer;"  onclick="createSession('serverList',<?=$sessionApp['id']?>,<?=$server['id']?>)" title="<?=$sessionApp['sessionName']?>"><?=getSessionTitle($sessionApp['sessionName'],7)?></span>
       </td>
      </tr>
      </table>
    </td> 

   
    <?  
        if($countApp==4)
        {
         echo "</tr><tr>";
         $countApp=0;
        }
      }     
    }
    ?>    
 
    <!-- stop  dinamic application -->
    </tr>
   </table>
  </td>
  <?
   } //end if(displayMode==1)
   elseif($server['displayMode']=="2")
   {
  ?>
  <td colspan="2">
    <table width="100%" height="100" cellpadding="0" cellspacing="0" border="0" style="margin: 10px; background-color: #fff;">
    <tr>
     <td valign="middle" align="center" valign="middle">
       <?=$server['replacingText']?>
     </td>
    </tr> 
    </table>
  </td>
  <?
   }//
  ?> 
 </tr>
</table>
<?
 }// end if(displayMode!='0')
} //end while($server = mysql_fetch_array($serverList))
?>
</form>

<?if($error==1)
{ 
?>
<div class="message">
 <p style="margin:0px;padding:0px;line-height:27px;" class="t_2orange"><img src="<?=$sharedSkin?>/icon_alertWhite.png" align="middle">
  <?=$Message[$messageType]?>
 </p>
</div>
<?} ?>


<!-- stop table for list 1row*4cols -->
<table cellSpacing="0" cellPadding="0" width="98%" height="27" style="min-width:900px; border: 1px solid #919B9D; background-color: #ECE9D8;margin-top:20px;">
<tr>
  <td width="100%"></td>
</tr>
</table>
</div>
<? include("includes/Footer.php") ?>
