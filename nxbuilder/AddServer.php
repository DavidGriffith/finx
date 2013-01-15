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
/* NX and NoMachine are trademarks of Medialogic S.p.A.                    */
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

$checkLink="";  
$adsl="";
$modem="";
$isdn="";
$wan="";
$lan="";

$imgServerValue="";
$messageType="";
if(isset($_POST['addedServer_x']))
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
 elseif (!is_numeric($_POST['port']))
 { 
   $error= true;
   $messageType="ErrorPort";
 }
 if(isset($_POST['displayMode']))
 {
  if ($_POST['displayMode']=="2")
  { 
   if(trim($_POST['substitutiveText']==""))
   {
    $error= true;
    $messageType="EmptyReplaceTxt";   
   }
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
   
    
    if(getServerByName($_POST['serverName']))
    {
     $messageType = "ErrorServerNameExist";
    }
    elseif(getServerByHost($_POST['hostName']))
    {
     $messageType = "ErrorServerHostExist";
    }            
    elseif(InsertServer($_POST['serverName'],$_POST['hostName'],$_POST['port'],$_POST['platform'],$_POST['cpu'],$_POST['memory'],
    $_POST['disk'],$_POST['description'],$icon,$displayMode,$replacingText,$defaultLink,$displayLink))
    {
      $messageType = "AddedServer";
      header('Location: ServerListAdmin.php');
    }
    else
    {
      $messageType = "ErrorServerUnknown";
    }
  }//end if(!error)  
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

    if (!isset($_POST['suspended']))
    {
     $_POST['noDisplay']='';
     $_POST['displayText']='';
     $_POST['substitutiveText']='';
    }  

}


include("includes/Top.php");
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
<? 
include("includes/Toolbar.php");

?>
<div id="min_width">
<!--SectionIcon Start-->
<table border="0" cellspacing="0" cellpadding="0" class="header3" width="100%" style="min-width:900px;">
<tr>
 <td><img src="<?=$sharedSkin?>/icon-addServer.png" width="34" height="26" border="0"></td>
 <td class="txtHeader3" nowrap align="left">Add Server</td>
</tr>
</table>

<form name="addServer" action="<?=$_SERVER['PHP_SELF']?>" method="post" ENCTYPE="multipart/form-data">


 <table border="0" width="98%" cellspacing="0" cellpadding="0" style="min-width:900px; margin:0px;margin-top:15px;">
 <tr align="left">
  <td valign="top" class="bgBox" align="left">

    <table border="0" width="100%" cellspacing="0" cellpadding="0" class="bgHeaderTable">
    <tr align="left">
     <td height="26" width="100%" align="left" ><span class="txtHeaderTable">&nbsp;Server Details</span></td>
    </tr>
    </table>  
    
    <table width="100%" height="290" cellSpacing="0" cellPadding="5" border="0" class="borderAll" style="padding-top: 10px;" >
    <tr>  
     <td width="85" class="txtLabel"  align="right" nowrap>Server name:</td>
     <td colspan="3"><input type="text" name="serverName" size="50" class="t_2black" value="<? if(isset($_POST['serverName'])){echo $_POST['serverName'];} ?>"></td>
    </tr>
    <tr>  
     <td class="txtLabel"  align="right"> Host: </td>
     <td>
      <input align="center" type="text" name="hostName" class="t_2black" size="33" value="<? if(isset($_POST['hostName'])){echo $_POST['hostName'];}?>">
      <span class="txtLabel"> Port: </span>
      <input type="text" size="6" name="port" class="t_2black" value="<? if(isset($_POST['port'])){echo $_POST['port'];} else { echo "22"; }?>">
     </td>
    </tr>
    <tr>  
     <td class="txtLabel"  align="right" valign="top"> Description: </td>
     <td colspan="3"><textarea name="description" COLS="48" rows="6" class="t_2black"><? if(isset($_POST['description'])){echo $_POST['description'];}?></textarea></td>
    </tr>
    <tr>  
     <td class="txtLabel"  align="right" nowrap> O.S.: </td>
     <td colspan="3"><input type="text" name="platform" size="50" class="t_2black" value="<? if(isset($_POST['platform'])){echo $_POST['platform'];}?>"></td>
    </tr>
    <tr> 
     <td class="txtLabel"  align="left" colspan="4">
       <table width="100%" cellSpacing="0" cellPadding="0" border="0">
         <tr>  
          <td class="txtLabel"  width="85" align="right"> CPU:&nbsp;</td>
          <td width="80">&nbsp;&nbsp;<input type="text" name="cpu" size="7" class="t_2black" value="<? if(isset($_POST['cpu'])){echo $_POST['cpu'];}?>"></td>
          <td class="txtLabel"   width="60"> Memory: </td>
          <td width="80"><input type="text" name="memory" size="7" class="t_2black" value="<? if(isset($_POST['memory'])){echo $_POST['memory'];}?>"></td>      
          <td class="txtLabel"   width="40"> Disk: </td>
          <td><input type="text" name="disk" size="7" class="t_2black" value="<? if(isset($_POST['disk'])){echo $_POST['disk'];}?>"></td>
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
         if(isset($_POST['displayMode']))
         {
          $displayDiv="block";
         }
         
         if(!isset($_POST['displayMode']))
         {
          $_POST['displayMode']="1";
         }
         
         
        ?>
        
         <div id="suspendText" style="display:<?=$displayDiv?>;"> 
           <table width="100%" bgcolor="#ffffff"  border="0" align="left" valign="top" cellspacing="2" cellpadding="0" style="margin-top: 5px; border: 1px solid #919B9C;">
           <tr>
            <td width="130" class="txtLabel" align="left"><span id="noDisplayText" style="padding-left:4px;">No display:</span></td>
            <td align="left"><input style="padding-left:4px;" type="checkbox" name="displayMode" id="noDisplay" value="0" <? if($_POST['displayMode']=="0") echo 'checked'; ?> onClick="disableText()"></td>
           </tr>
           <tr>
            <td class="txtLabel" align="left"><span id="replaceText" style="padding-left:4px;">Replace with text:</span></td>
            <td align="left"><input style="padding-left:4px;" type="checkbox" name="displayMode" id="displayText" value="2" <? if($_POST['displayMode']=="2") echo 'checked'; ?> onClick="disableNoDisplay()"></td>
           </tr>
           <tr>
            <td align="left" style="padding-left:4px; padding-right:2px;" colspan="2">
             <textarea disabled name="substitutiveText" id="substitutiveText"  <? if($_POST['displayMode']=="0") echo 'disabled'; ?> COLS="45" rows="3" class="text"><? if (!empty($_POST['substitutiveText'])) echo $_POST['substitutiveText']; ?></textarea>
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
     <td class="txtLabel" width="100" align="right"> <span style="margin-top: 10px;">Select image: </span> </td>
     <td class="t_2black" colspan="2">
 
       <select name="iconServer" size="1" class="t_2black" style="width:165px;margin-left:10px;" onchange="showServerIcon(this.value)">
       <option value="imgEmpty.png"></option> 
       <?php
         $postIcon='';
         if(isset($_POST['uploadFile_x']) && (trim($imgServerValue)!="")  )
         {
           $postIcon=$imgServerValue;
           echo '<option value="'.$postIcon.'" selected >';
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
               echo '<option value="'.$file.'">';
               $file=explode('.',$file);
               echo strtoupper($file[0]).'</option>';
           }  
         closedir($handle);
         }
                              ?> 
     </select>    
     <input type="hidden" name="imgServer" id="imgServer" value="<?=$imgServerValue?>"> 
     </td>
    </tr>
    <tr>
     <td class="txtLabel" width="100" align="right" valign="top" > <div style="margin-top: 5px;">New image:</div>  </td>
     <td class="t_2black" valign="middle"  width="285">
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
         <input type="file" name="file1" size="30" class="file"  onmouseover="this.src ='<?=$button?>/b_browseOver.png'" onmouseout="this.src ='<?=$button?>/b_browse.png'" onClick="document.getElementById('file').value=this.value;">
         <div class="fakefile" >
          <input type="text" class="textblack" id="file" size="30" >
          <img  id="b_browse" src="<?=$button?>/b_browse.png" class="buttonBrowse">
         </div>
       </div>
     <?
     }
     ?>
     
     
     
     </td>
     <td class="txtLabel" valign="top">
     <div style="margin-top: 5px;"><input type="image"  name="uploadFile" class="t_2black" src="<?=$button?>/b_upload.png" width="82" height="17"  border="0" onmouseout="this.src='<?=$button?>/b_upload.png'" onmouseover="this.src='<?=$button?>/b_uploadOver.png'"></div>
     </td>
    </tr>
    <tr>
     <td  class="txtLabel" align="right" valign="top"><div style="margin-top: 5px;"> Preview:</div></td>
     <td  class="txtLabel" align="left" valign="top"><div style="margin-left:10px;margin-top:5px;">        
        <img name="Icon" id="serverIcon" width="122" height="93" src=
          <?php
       
          if(isset($_POST['uploadFile_x']) && (trim($imgServerValue)!="")  )
          {
           echo '"images/servers/'.$imgServerValue.'"';
          }
          elseif(isset($_POST['addedServer_x']))
          {
           echo '"images/servers/'.$_POST['iconServer'].'"';
          
          }
          else
          {
           echo '"images/servers/imgEmpty.png"';
          }
          ?> border="0"> </div>
                  
            
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
  <table align="center" cellSpacing="0" cellPadding="0" border="0" height="75" width="100%" class="bgBox borderAll" >
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
   <td width="100%" align="right">
     <input type="hidden" value="operation" name="addServer">
     <input style="margin-right: 12px;" type="image" name="addedServer" width="82" height="17" src="<?=$button?>/b_add.png" border="0"  onmouseout="this.src='<?=$button?>/b_add.png'" onmouseover="this.src='<?=$button?>/b_addOver.png'">
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
</div>
<? include("includes/Footer.php");?>
