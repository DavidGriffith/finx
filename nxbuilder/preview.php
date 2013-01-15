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

include "builder.cfg";
require_once("HandleDB.php");
checkPermission(); 
?>
<center>

<style type="text/css">
 @import url("style/<?=$css?>.css");
</style>
<table border="0" width="97%" cellspacing="0" cellpadding="0" class="bgGuest borderAllGuest" style="margin:0px;margin-top:15px;">
<tr>
 <td width="100%" height="26" class="txtHeaderTableGuest">&nbsp;Server</td>
</tr>
</table>
<input type="hidden" name="serverId" id="serverId" value="">



<?

$k=0;
$id=getGuestId();

$serverList = getListServerForUser($id);
$countDesk = "";

$countColor= count($serverList);

if($countColor==0)
{
 $error=1;
 $messageType="NoServerAvailable";
}

//$server = getServer($serverList['idServer']);
if(isset($error))
{
 if($error==1)
  { 
  ?>
  <div class="message">
   <p style="margin:0px;padding:0px;line-height:27px;"><img src="<?=$sharedSkin?>/icon_alertWhite.png" align="middle">
    <?=$Message[$messageType]?>
   </p>
  </div>
<?
 } 
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

<table width="97%" class="<?=$class?>" cellspacing="0" cellpadding="0" border="0" >
 <tr>
  <td width="250" valign="top" class="border_rightGuest">
   <div class="borderAllGuest" id="result" style="z-index: 1000; top: 100px; left: 100px; width: 300px; display:none; position:absolute; background-color:white;" align="left"></div>
   <table  width="250" cellspacing="1" cellpadding="5" border="0">
   <tr>
     <td colspan="2" class="txtLabel">
       <span class="txtLabelGuest">NX Server:</span><br /><span class="txtValueGuest"><?=$server['serverName']?></span>   
     </td>
     <td colspan="2">
       <img width="17" height="17" src="<?=$absHost?>/<?=$shared?>/info.png" name="idServer" id="idServer"  style="cursor:pointer" onmouseover="javascript: document.getElementById('idServer').value='<?=$list?>';setResultPosition2(Position.get(this),<?=$k?>); document.getElementById('result').style.display='block'; setDescrition('respGuest.php'); " onmouseout="document.getElementById('result').style.display='none';">
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
      </select><br />
      </div><br />
      <span class="txtLabel">Display</span>
      <select name="resolution-<?=$server['id']?>" class="textBlack" style="width:85px;">
       <option value="800x600">800x600</option>
       <option value="1024x768">1024x768</option>
       <option value="availablearea">Available area</option>
       <option value="fullscreen">Fullscreen</option>
       <option value="available_area_mm">Available area, Multi-head</option>
       <option value="fullscreen_mm">Fullscreen, Multi-head </option>       

      </select>
     </td>
     <td>&nbsp;</td>
    </tr>
   </table>
  <!--  </div> -->
  </td>

  <?
   $k++;
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
     <td align="left" valign="top">
      <table width="63" style="margin:" border="0" cellpadding="1" cellspacing="0">
      <tr>
       <td valign="top" align="center"> <img src="<?=$absHost?>/<?=$sessionShared?>/<?=$icon?>" width="62" height="50"></td>
      </tr>
      <tr>
       <td valign="top" align="center">
         <span class="titleBlueSmall" style="cursor: pointer;" title="<?=$session['sessionName']?>"><?=getSessionTitle($session['sessionName'],7)?></span>
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
   <table cellspacing="0" cellpadding="1" border="0" style="margin-top:-15px; margin-left: 50px;" valign="middle">
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
       <td valign="top" align="center"> <img src="<?=$absHost?>/<?=$sessionShared?>/<?=$icon?>" width="62" height="50"></td>
      </tr>
      <tr>
       <td valign="top" align="center">
         <span class="titleBlueSmall" style="cursor: pointer;" title="<?=$sessionApp['sessionName']?>"><?=getSessionTitle($sessionApp['sessionName'],7)?></span>
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
 }// end if(displayMode!='0')
} //end while($server = mysql_fetch_array($serverList))
?>
<div style="height: 15px; width: 97%;">&nbsp;</div>


