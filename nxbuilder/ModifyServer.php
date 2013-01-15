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
include ("conf.php");
include ("Messages.php");
checkPermission();
 
//
// set default value for form fields 
// 

if($_POST['idServer'])
{
 
}
else
{
 header("Location: ServerListAdmin.php");
}

if(!isset($_POST['modifyServer_x']) && !isset($_POST['uploadFile_x']) )
{   
 
 $serverQuery= getServer($_REQUEST['idServer']);
 
 $server=mysql_fetch_array($serverQuery);
 
 $serverNameValue=$server['serverName']; 
 $hostNameValue=$server['hostName'];
 $portValue=$server['port'];
 
 $descriptionValue=$server['description'];
 $platformValue=$server['OS'];
 $cpuValue=$server['CPU'];
 $memoryValue=$server['memory'];
 $diskValue=$server['disk'];
 $suspendedValue=$server['displayMode']; 
 $substitutiveTextValue=$server['replacingText'];
 $iconValue=$server['icon'];
 $adsl="";
 $modem="";
 $isdn="";
 $wan="";
 $lan="";
 $checkLink="";
 if($server['defaultLink'] == "ADSL") $adsl="selected";
    elseif($server['defaultLink'] == "MODEM") $modem="selected";
    elseif($server['defaultLink'] == "ISDN") $isdn="selected";
    elseif($server['defaultLink'] == "WAN") $wan="selected";
    elseif($server['defaultLink'] == "LAN") $lan="selected";
    if($server['displayLink']=="1") $checkLink="checked";
    else $checkLink="";
}
else
{
 $serverNameValue=$_POST['serverName']; 
 $hostNameValue=$_POST['hostName'];
 $portValue=$_POST['port'];
 $checkLink="";
 $adsl="";
 $modem="";
 $isdn="";
 $wan="";
 $lan=""; 
 $descriptionValue=$_POST['description'];
 $platformValue=$_POST['platform'];
 $cpuValue=$_POST['cpu'];
 $memoryValue=$_POST['memory'];
 $diskValue=$_POST['disk'];
 if(isset($_POST['displayMode'])) $suspendedValue=$_POST['displayMode'];
 else $suspendedValue="";
 if(isset($_POST['replacingText'])) $substitutiveTextValue=$_POST['replacingText'];
 else $substitutiveTextValue="";
 $iconValue=$_POST['iconServer'];

}


$messageType="";   

$imgServerValue="";
if(isset($_POST['modifyServer_x']))
{ 

 $error= false;

 //
 // check error 
 //
  
 if (trim($_POST["serverName"]) == "")
 {
   $error= true;
   $messageType="EmptyServerName";
 }
 elseif (trim($_POST['hostName']) == "")
 { 
   
   $error= true;
   $messageType="EmptyHostName";
 } 
 elseif(trim($_POST["port"]) == "" )
 { 
   $error= true;
   $messageType="EmptyHostPort";
 } 
 elseif (!is_numeric($_POST["port"]))
 { 
   $error= true;
   $messageType="ErrorPort";
 }
 elseif ($_POST['displayMode']=="2")
 { 
   if(trim($_POST['substitutiveText']==""))
   {
    $error= true;
    $messageType="EmptyReplaceTxt";   
   }
 }
 
 if(!$error)
 {
    
    $icon = $_POST['iconServer'];
      
    //
    // displayMode:
    //             
    // 0 - server not dislay
    // 1 - server display
    // 2 - server replace with text
                   
       
    $displayMode='1';
    $replacingText='';   
    $defaultLink="ADSL";
    $displayLink="0";
    
    if(isset($_POST['displayMode']))
    {

     $displayMode=$_POST['displayMode'];   
     
     if(isset($_POST['substitutiveText']))
     {     
       $replacingText=$_POST['substitutiveText'];
     }      
   
    }
    if(isset($_POST['defaultLink']))
    {
     $defaultLink=$_POST['defaultLink'];
    }
    if(isset($_POST['displayLink']))
    {
    
       $displayLink=$_POST['displayLink'];
    }
    
    $resultUpdate=UpdateServer($_REQUEST['idServer'], $_POST['serverName'],$_POST['hostName'],$_POST['port'],$_POST['platform'],$_POST['cpu'],$_POST['memory'],
    $_POST['disk'], $_POST['description'],$icon,$displayMode,$replacingText,$defaultLink,$displayLink);
     
    if($resultUpdate==1) 
    {
      $messageType = "UpdatedServer";
      @header('Location: ServerListAdmin.php');
    }
    elseif($resultUpdate==-2)
    {
      $messageType = "ErrorServerNameExist";    
    }
    elseif($resultUpdate==-1)
    {
      $messageType = "ErrorServerHostExist";
    }
    elseif($resultUpdate==0)
    {
      $messageType = "ErrorServerUnknown";
    }
    
    
  }
}
elseif(isset($_POST['uploadFile_x']))
{

    $directory = 'images/servers/';
    $path1 = $_FILES['file1']['tmp_name'];
    $name = $_FILES['file1']['name'];
    
    if (!empty($path1))
    {
      if(($result=preg_match('/\\.(gif|png|jpg|jpeg|bmp|img)$/i', $_FILES['file1']['name'])) && (move_uploaded_file($path1, $directory.$name)))
      {
       $messageType = "UploadedFile";
       $imgServerValue=$name; 
      }  
      else
      {
        $messageType = "UploadedFileError";
      }  
    }
}


include("includes/Top.php");
?>




<center>

<? 

include("includes/Toolbar.php");
?>

<!--SectionIcon Start-->

<table border="0" cellspacing="0" cellpadding="0" class="header3" width="100%" style="min-width:900px;">
<tr>
 <td><img src="<?=$sharedSkin?>/icon-addServer.png" width="34" height="26" border="0"></td>
 <td class="txtHeader3" nowrap align="left">Modify Server</td>
</tr>
</table>

<form name="addServer" action="<?=$_SERVER['PHP_SELF']?>" method="post" ENCTYPE="multipart/form-data">

 <input type="hidden" name="idServer" id="idServer" value="<?=$_REQUEST['idServer']?>" >
 <table border="0" width="98%" cellspacing="0" cellpadding="0" style="margin:0px;margin-top:15px;">
 <tr align="left">
  <td valign="top" class="bgBox">

     <table border="0" width="100%" cellspacing="0" cellpadding="0" class="bgHeaderTable">
      <tr align="left">
       <td height="26" width="100%" align="left" ><span class="txtHeaderTable">&nbsp;Server Details</span></td>
      </tr>
    </table>   
    
    <table width="100%" cellSpacing="0" cellPadding="5" border="0" class="borderAll" style="padding-top: 10px;">
    <tr>  
     <td class="txtLabel"  align="right" nowrap>Server name:</td>
     <td colspan="3"><input type="text" name="serverName" size="50" class="t_2black" value="<?=$serverNameValue?>"></td>
    </tr>
    <tr>  
     <td class="txtLabel"  align="right">Host:</td>
     <td>
      <input align="center" type="text" name="hostName" class="t_2black" size="33" value="<?=$hostNameValue?>">
      <span class="txtLabel">Port:</span>
      <input type="text" size="6" name="port" class="t_2black" value="<?=$portValue?>">
     </td>
    </tr>
    <tr>  
     <td class="txtLabel"  align="right" valign="top">Description:</td>
     <td colspan="3"><textarea name="description" COLS="48" rows="6" class="t_2black"><?=$descriptionValue?></textarea></td>
    </tr>
    <tr>  
     <td class="txtLabel"  align="right" nowrap>O.S.:</td>
     <td colspan="3"><input type="text" name="platform" size="50" class="t_2black" value="<?=$platformValue?>"></td>
    </tr>
    <tr> 
     <td class="t_2black"  align="left" colspan="4">
       <table width="100%" cellSpacing="0" cellPadding="0" border="0">
       <tr>  
        <td class="txtLabel"  width="100" align="right"> CPU:&nbsp;&nbsp;&nbsp;</td>
        <td width="80"><input type="text" name="cpu" size="7" class="t_2black" value="<?=$cpuValue?>" ></td>
        <td class="txtLabel"   width="60"> Memory: </td>
        <td width="80"><input type="text" name="memory" size="7" class="t_2black" value="<?=$memoryValue?>"></td>      
        <td class="txtLabel"   width="40"> Disk: </td>
        <td ><input type="text" name="disk" size="7" class="t_2black" value="<?=$diskValue?>"></td>
       </tr>
       </table>
   
     </td>
    </tr>
    <tr>
     <td align="right" colspan="4">

       <table width="100%" cellSpacing="0" cellPadding="0" border="0" bgcolor="#ffffff" style="border: 1px solid #919B9C;"> 
       <tr>  
        <td width="22" align="center" valign="middle" height="20"><img width="17" height="17"  src="<?=$shared?>/alert.png"  ></td>
        <td class="titleGrey"  align="left" > Disable display of this server via Web</td>
        <td><span id="changeArrow"> <img style="cursor: pointer" src="<?=$button?>/b_orderOff.png" onClick="displaySuspendText('<?=$button?>','changeArrow','suspendText');"  onmouseover="this.src='<?=$button?>/b_orderOff_over.png'" onmouseout="this.src='<?=$button?>/b_orderOff.png'" > </span></td>
       </tr>  
       </table>
      
       <table width="100%" cellSpacing="0" cellPadding="0" border="0">      
       <tr>
        <td>
        <?
          $displayDiv="none";
         if(isset($suspendedValue) && $suspendedValue!="1")
         {
          $displayDiv="block";
         }
        ?>        
         <div id="suspendText" style="display:<?=$displayDiv?>;"> 
           <table width="100%" bgcolor="#ffffff"  border="0" align="right" valign="top" cellspacing="2" cellpadding="0" style="margin-top: 5px; border: 1px solid #919B9C;">
           <tr>
            <td class="txtLabel" align="right"><span id="noDisplayText">No display:</span></td>
            <td align="left"><input type="checkbox" name="displayMode" id="noDisplay" value="0" <? if($suspendedValue=="0") echo 'checked'; ?> onClick="disableText()"></td>
            <td class="txtLabel" align="right"><span id="replaceText">Replace with text:</span></td>
            <td align="left"><input type="checkbox" name="displayMode" id="displayText" value="2" <? if($suspendedValue=="2") echo 'checked'; ?> onClick="disableNoDisplay()"></td>
           </tr>
           <tr>
            <td align="right" style="padding-left:4px; padding-right:2px;" colspan="4">
             <textarea name="substitutiveText" id="substitutiveText"  <? if($suspendedValue=="0") echo 'disabled'; ?> COLS="40" rows="3" class="text"><? if(!empty($substitutiveTextValue)) echo $substitutiveTextValue; ?></textarea>
            </td>
           </tr>
           </table>
         </div>
        </td>
       </tr>
       </table>  

       
     </td>
    </tr>
    </table>
 </td>
 <td width="25">&nbsp;</td>
 <!-- START box dx -->
 <td align="center" width="484" valign="top" >
 
    <table border="0" width="100%" cellspacing="0" cellpadding="0" class="bgHeaderTable">
     <tr align="left">
      <td width="100%" height="26" align="left" ><span class="txtHeaderTable">&nbsp;Server Image</span></td>
     </tr>
    </table> 
 
   <table border="0" cellpadding="0" cellspacing="0" width="100%" class="bgBox"> 
   <tr>
    <td style="padding: 5px;" align="center" valign="top" width="100%" class="borderAll">
<!-- start server image -->

     
    <table align="center" cellSpacing="0" cellPadding="4" border="0" width="100%" >
    <tr>
     <td class="txtLabel" width="100" align="right"><span style="margin-top: 10px;">Select image: </span></td>
     <td class="txtLabel" colspan="2">
 
       <select name="iconServer" size="1" class="t_2black" style="margin-left:10px;width:165px;" onchange="showServerIcon(this.value)">
       <option value="imgEmpty.png"></option> 
       <?php
         $postIcon='';
         if(isset($_POST['uploadFile_x']))
         {
           $postIcon=$imgServerValue;
           echo '<option value="'.$postIcon.'" selected>';
             $file=explode('.',$postIcon);
           echo strtoupper($file[0]).'</option>';         
         }
         elseif(isset($_POST['iconServer']))
         {
           $postIcon=$_POST['iconServer'];
           echo '<option value="'.$postIcon.'" selected>';
             $file=explode('.',$postIcon);
           echo strtoupper($file[0]).'</option>';
        
         }
         
         if ($handle = opendir('images/servers/')) 
         {
          
           while (false !== ($file = readdir($handle))) 
           {
           
            if ($file != "." && $file != ".." && $file!= "Thumbs.db" && $file !=$postIcon && $file !="imgEmpty.png")
            {
              $selected="";
              
              if($iconValue=="$file") $selected=" selected";
              
              echo '<option value="'.$file.'" '.$selected.' >';
              $file=explode('.',$file);
              echo strtoupper($file[0]).'</option>';
            }    
           }  
         closedir($handle);
         }
                              
     if(!isset($imgServer))
     {
      $imgServer="";
     }
     ?> 
     </select>    
     <input type="hidden" name="imgServer" id="imgServer" value="<?=$imgServer?>"> 
     </td>
    </tr>
    <tr>
     <td class="txtLabel" width="100" align="right" valign="top" ><div style="margin:0px;margin-top: 5px;">New image:</div> </td>
     <td class="txtLabel" valign="middle" width="285">
      <?
      $brow=$_SERVER['HTTP_USER_AGENT'];
      if(strpos($brow, "Konqueror") || strpos($brow, "Mac_PowerPC") )
      {
      ?>
       <input type="file" name="file1" size="30" class="file"  onmouseover="this.src ='<?=$button?>/b_browseOver.png'" onmouseout="this.src ='<?=$button?>/b_browse.png'">
     <?
      }
      else
      {
     ?>   
       <div class="fileinputs">
         <input type="file" name="file1" class="file"  onmouseover="this.src ='<?=$button?>/b_browseOver.png'" onmouseout="this.src ='<?=$button?>/b_browse.png'" onClick="document.getElementById('file').value=this.value;">
         <div class="fakefile">
          <input type="text" class="textblack" id="file" style="width:150px;">
          <img  id="b_browse" src="<?=$button?>/b_browse.png" class="buttonBrowse">
         </div>
       </div>
    <?
      }
    ?>   
    
     </td>
     <td class="txtLabel" valign="top" width="">
     <div style="margin-top: 5px;"><input type="image"  name="uploadFile" class="t_2black" src="<?=$button?>/b_upload.png" width="82" height="17"  border="0" onmouseout="this.src='<?=$button?>/b_upload.png'" onmouseover="this.src='<?=$button?>/b_uploadOver.png'"></div>
     </td>
    </tr>
    <tr>
     <td  class="t_2black" align="center">&nbsp;</td>
     <td  class="txtLabel" align="left">Preview<br>
       
        <img name="Icon" id="serverIcon" width="105" height="80" src=
          <?php
       
          if(isset($_POST['uploadFile_x']))
          {
           echo '"images/servers/'.$imgServerValue.'"';
 
          }
          elseif(isset($_POST['modifyServer_x']))
          {
           echo '"images/servers/'.$_POST['iconServer'].'"';
          
          }
          else
          {
           echo '"images/servers/'.$iconValue.'"';
          }
          ?> border="0"> 
                  
            
     </td>
     <td  class="t_2black" align="center">&nbsp;</td>
    </tr>
    </table>
   



<!-- stop server image -->   
   </td>
  </tr>
  </table> 

  <?
   if(isset($_POST['defaultLink']) && $error = true)
   {
    if($_POST['defaultLink'] == "ADSL") $adsl="selected";
    elseif($_POST['defaultLink'] == "MODEM") $modem="selected";
    elseif($_POST['defaultLink'] == "ISDN") $isdn="selected";
    elseif($_POST['defaultLink'] == "WAN") $wan="selected";
    elseif($_POST['defaultLink'] == "LAN") $lan="selected";
   }
   if(isset($_POST['displayLink']) && $error = true ) $checkLink="checked";
   
  ?>
  
  
  
  <table border="0" width="100%" cellspacing="0" cellpadding="0" class="bgHeaderTable"  style="margin-top:5px;">
   <tr align="left">
    <td width="100%" height="26" align="left" ><span class="txtHeaderTable">&nbsp;Default Link</span></td>
   </tr>
   </table> 
   <table align="center" cellSpacing="0" cellPadding="0" border="0" height="62" width="100%" class="bgBox borderAll" >
   <tr class="txtLabel" >
     <td width="100" align="right" valign="middle" >
     <span class="textBlack">Hide:</span>
     </td>
     <td align="left" valign="middle">
     <input name="displayLink" id="displayLink" type="checkbox" style="margin-left:16px;" <?=$checkLink?> value="1">
    </td>
    </tr>
    <tr>
    <td align="right" width="100" valign="top">
     <span class="textBlack">Type:</span>
    </td>
    <td align="left" valign="top">
     <select name="defaultLink" class="textBlack" style="width:165px;margin-left:16px;"></option>
       <option value="ADSL" <?=$adsl?> >ADSL</option>
       <option value="MODEM" <?=$modem?> >MODEM&nbsp;&nbsp;&nbsp;&nbsp;</option>
       <option value="ISDN" <?=$isdn?> >ISDN</option>
       <option value="WAN" <?=$wan?> >WAN</option>
       <option value="LAN" <?=$lan?> >LAN</option>
      </select>
    </td>
   </tr>
  </table>
  
 </td>
</tr>
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
   <td width="100" align="center">
     <input type="hidden" value="operation" name="addServer">
     <input style="margin-right: 12px;" type="image" name="modifyServer" width="82" height="17" src="<?=$button?>/b_modify.png" border="0"  onmouseout="this.src='<?=$button?>/b_modify.png'" onmouseover="this.src='<?=$button?>/b_modifyOver.png'">
  </td>
 </tr>
 </table>
</form>
</center>
<?php



if (!empty($_POST['noDisplay']))
{
  echo'
    <script type="text/javascript">
      displaySuspendText();
      disableText();
    </script>';
}
else if (!empty($_POST['displayText']))
{
  echo'
    <script type="text/javascript">
      displaySuspendText();
      disableNoDisplay();
    </script>';
}
?>
<? include("includes/Footer.php");?>
