<!-- START: move this section on the top of webpage -->
<?
if(isset($_POST['createSessionId']))
{
 
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
 
 include ("$absPath/CreateSession.php");
 CreateSessionFile3_0($_POST['createSessionId'], $_POST[$f], $_POST[$k]);
 exit();
}
 $skinQuery=getColorTemplate();

 $skin=mysql_fetch_array($skinQuery);

 $css=$skin['name'];

 if(empty($css))
 {
  $css="defaultGuest";
 }
?>
<!--STOP: move this section on the top of webpage -->
<link rel="stylesheet" type="text/css" href="<?=$absHost?>/style/<?=$css?>.css" media="all" >
<link rel="stylesheet" type="text/css" href="<?=$absHost?>/style/general.css" media="all" >
<script src="<?=$absHost?>/js/browser.js"  language="javaScript" type="text/javascript"></script>
<script src="<?=$absHost?>/js/function.js"  language=javaScript type="text/javascript"></script>
<center>
<form name="serverList" id="serverList" action="<?=$_SERVER['PHP_SELF']?>" method="post" ENCTYPE="multipart/form-data" style="margin: 0px; padding: 0px;">


<table border="0" width="98%" cellspacing="0" cellpadding="0" style="min-width:900px;margin:0px;margin-top:15px;" class="bgHedearServerGuest">
  <tr>
   <td width="100%" height="26"  class="txtHeaderTableGuest">&nbsp;Server</td>
  </tr>
</table>


 <input type="hidden" name="serverId" id="serverId" value="">
 <input type="hidden" id="createServerId" name="createServerId" value="<?=$server['id']?>">
 <input type="hidden" id="createSessionId" name="createSessionId" value="<?=$session['id']?>">
 
 <div class="borderAllGuest" id="result" style="z-index: 1000; display:none; position:absolute; top:100px; left: 350px; background-color:white;"></div>

<?php
$error="";
$id=getGuestId();
$serverList = getListServerForUser($id);
$countColor= count($serverList);
$cont=0;
if($countColor==0)
{
 $error=1;
 $messageType="NoServerAvailable";
}

if($error==1)
{ 
  ?>
  <div class="message">
   <p style="margin:0px;padding:0px;line-height:27px;"><img src="<?=$sharedSkin?>/icon_alertWhite.png" align="middle">
    No server Available.
   </p>
  </div>
<?
} 

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

 $class="row2Guest";
 
 if($countColor%2) $class="row1Guest";
 $countColor++;
 $checked="";

   if($server['displayMode']!="0")
   { 
 ?>
<table width="98%" class="<?=$class?>" cellspacing="0" cellpadding="5" border="0">
 <tr>
  <td width="250" valign="top" class="border_rightGuest">

   <table  width="250" cellspacing="1" cellpadding="5" border="0">
   <tr>
     <td colspan="2" class="txtLabel">
       <span class="txtLabelGuest">NX Server:</span><br /><span class="txtValueGuest"><?=$server['serverName']?></span>   
     </td>
     <td colspan="2">
       <img width="17" height="17" src="<?=$absHost?>/<?=$shared?>/info.png" name="idServer" id="idServer"  style="cursor:pointer" onmouseover="document.getElementById('idServer').value='<?=$list?>'; setResultPosition(this); document.getElementById('result').style.display='block'; setDescrition('<?=$absHost?>/respGuest.php'); " onmouseout="document.getElementById('result').style.display='none';">
     </td>
   </tr>
    <tr>
     <td align="center">
       <?
        if(trim($server['icon'])=="")
        {       
       ?>
       <img width="122" height="93" src="<?=$absHost?>/<?=$serverShared?>/imgEmpty.png" style="margin:5px; margin-left:0px;">
       <?
        }
        else
        {
       ?>
       <img width="122" height="93" src="<?=$absHost?>/<?=$serverShared?>/<?=$server['icon']?>" style="margin:5px; margin-left:0px;">
       <?
        }
       ?>        
      <br />
      <div style="display:<?=$disabledLink?>;">
       <span class="txtLabel">Link&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;</span>
       <select name="link-<?=$server['id']?>" class="textBlack" ></option>
        <option value="ADSL" <?=$adsl?> >ADSL</option>
        <option value="MODEM" <?=$modem?> >MODEM&nbsp;&nbsp;&nbsp;&nbsp;</option>
        <option value="ISDN" <?=$isdn?> >ISDN</option>
        <option value="WAN" <?=$wan?> >WAN</option>
        <option value="LAN" <?=$lan?> >LAN</option>
       </select>
       <br />
      </div>
      <br />
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
     <td>&nbsp;</td>
    </tr>
   </table>
  <!--  </div> -->
  </td>
  <?
  $cont++;
   if($server['displayMode']=="1")
   {
   
  ?>
  <td valign="top" align="left">
  <table border="0" cellspacing="0" cellpadding="0" width="100%">
   <tr>
    <td class="txtLabelGuest">
    <div style="height:20px; margin:0px; margin-bottom:0px; padding:5px; padding-bottom:0px;">Desktop:</div>

    <table cellspacing="0" cellpadding="5" border="0" style="margin-top:-15px; margin-left: 50px;" valign="middle">
    <tr>
    <!-- start dinamic desk -->
    <?
    $querySess=getSessionsListForUser($list,$id,"<>");
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
      <table width="63" border="0" cellpadding="0" cellspacing="0">
      <tr>
       <td valign="top" align="center"> <img style="cursor: pointer;"  onclick="createSession('serverList',<?=$session['id']?>,<?=$server['id']?>)" src="<?=$absHost?>/<?=$sessionShared?>/<?=$icon?>" width="62" height="50"></td>
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
</tr>
 <tr>
  <td class="txtLabelGuest">
   <div style="height:20px; margin:0px; margin-bottom:0px; margin-top:10px; padding:5px; padding-bottom:0px;">Application:</div>
    <table cellspacing="0" cellpadding="5" border="0" style="margin-top:-15px; margin-left: 50px;" valign="middle">
    <tr>
    <!-- start dinamic application -->
    
    <?
    $querySessApp=getSessionsListForUser($list,$id,"=");
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
      <table width="63" border="0" cellpadding="0" cellspacing="0">
      <tr>
       <td valign="top" align="center"> <img style="cursor: pointer;" onclick="createSession('serverList',<?=$sessionApp['id']?>,<?=$server['id']?>)"  src="<?=$absHost?>/<?=$sessionShared?>/<?=$icon?>" width="62" height="50"></td>
      </tr>
      <tr> 
       <td valign="top" align="center"> 
        <span class="titleBlueSmall" style="cursor: pointer;" onclick="createSession('serverList',<?=$sessionApp['id']?>,<?=$server['id']?>)" title="<?=$sessionApp['sessionName']?>"><?=getSessionTitle($sessionApp['sessionName'],7)?></span>
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
  </td>
</tr>
</table>
<?
 } 
}
?>
<div style="height: 15px; width: 97%;">&nbsp;</div>
</form>
