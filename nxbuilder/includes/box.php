<?
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

if(!isset($imgSessionValue)) $imgSessionValue="";

?>

<!-- start general -->
 <div id="panelLogin0" valign="middle" class="bgBoxInternal">
  <div class="content_nx_box_right">
   <span class="txtLabel">Desktop</span>
    <div class="general_nx_box_right">
     <table class="table_nx_box_right" align="center" cellpadding="0" cellspacing="0" border="0">
      <tr class="table_nx_box_right">
       <td>
        <select name="sessionType" id="sessionType" class="textBlack"  onchange="javascript: setDeskTop(this);">
          <option value="unix" <?=$selUnix?> >Unix &nbsp;&nbsp;&nbsp;</option>
          <option value="windows" <?=$selWin?>>Windows</option>
          <option value="vnc" <?=$selVnc?>>VNC</option>
          <option value="shadow" <?=$selShadow?>>Shadow</option>
        </select>
       </td>
       <td>
        <select name="deskType" id="deskType" <?=$dis_deskType?> onchange="javascript:enableSetting(this.value);" class="textBlack" >
        <?=$optionsDesk?>
       
       <!--   <option value="kde">KDE</option>
          <option value="gnome">GNOME</option>
          <option value="cde">CDE</option>
          <option value="xdm">XDM</option>
          <option value="custom">Custom</option> -->
        </select>
        
       </td>
       <td>
       <input <?=$dis_DeskSettingButton?> id="DeskSettingButton" type="button" name="Setting" value="Settings..." onclick="javascript: setPanelOption(); disableAction();">
       </td>
      </tr>
      <tr class="table_nx_box_right">
       <td>
         <table cellpadding="0" cellspacing="0" border="0">
         <tr>
          <td valign="top">
             <IMG width="62" height="50" id="serverIcon" src=<?
               if(isset($_POST['uploadFile_x'])&&($upResult==true)) { echo '"images/sessions/'.$nameExt.'" '; }
               elseif(empty($iconSet)|| ($upResult==false && $upResult!=""  ) )
                  echo '"images/sessions/empty.png" ';
               else
                  echo '"images/sessions/'.$iconSet.'" ';
             ?> border="0"  >
          </td>
          <td style="text-align:right;" class="textBlack">Category</td>
          </tr>
          </table>
       </td>
       <td align="right">
            <select name="icon" size="1" class="textblack" style="width:120px;" onchange="showSessionIcon(this.value)" >
            <option value="empty.png"></option>
            <?php
              $postIcon='';
              if(isset($_POST['uploadFile_x'])  && (trim($_POST['nameCategory'])!=""))
              {
               
               $postIcon = $nameExt;

                echo '<option value="'.$postIcon.'" selected >';
                  $file=explode('.',$postIcon);
                echo strtoupper($file[0]).'</option>'; 
              }
              elseif (isset($_POST['icon']))
              {
                $postIcon=$_POST['icon'];
                echo '<option value="'.$postIcon.'" selected>';
                  $file=explode('.',$postIcon);
                echo strtoupper($file[0]).'</option>';
              }
              else
              {
               if(isset($sessionDB['icon']))
               {
               $postIcon=$sessionDB['icon'];

                 echo '<option value="'.$postIcon.'" selected>';
                  $file=explode('.',$postIcon);
                echo strtoupper($file[0]).'</option>';
               } 
              }

              if ($handle = opendir('images/sessions/')) 
              {
                while (false !== ($file = readdir($handle))) 
                {
                 if ($file != "." && $file != ".." && $file!= "Thumbs.db" && $file !=$postIcon && $file !="empty.gif" && $file !="empty.png" )
                  echo '<option value="'.$file.'" >';
                  $file=explode('.',$file);
                  echo strtoupper($file[0]).'</option>';
                }  
                closedir($handle);
              }
            ?> 
          </select> 
       </td>
       <td>
       <input type="button" id="create" name="create" value="Create..." onclick="displayCreateCategory()">
       </td>
      </tr>
      <tr>
       <td colspan="3">
      <table  cellpadding="0" cellspacing="0" border="0" id="tableCreateCategory" style="display:none;">
        <tr>
          <td class="txtLabel" align="right" valign="center" width="110">New Category </td>
          <td class="txtLabel" valign="middle" >
           <input type="text" name="nameCategory" id="nameCategory" style="margin-left:10px; width:120px;" value="">
           <input type="hidden" name="imgSession" id="imgSession" value="<?=$imgSessionValue?>">
          </td>    
        </tr>
        <tr>
             <td class="txtLabel"  align="right" valign="top" style="padding-top:5px;" width="110" >New image </td>
             <td class="txtLabel" valign="middle">
               <div class="fileinputs">
                 <input type="file" name="file1"  class="file"   onClick="document.getElementById('file').value=this.value;">
                 <div class="fakefile" >
                  <input type="text" class="textblack" id="file" style="margin-left:0px; width:120px;">
                  <img  id="b_browse" src="<?=$button?>/b_browse.png" class="buttonBrowse" onmouseout="this.src='<?=$button?>/b_browse.png'" onmouseover="this.src='<?=$button?>/b_browseOver.png'">
                 </div>
               </div>
             </td>
            </tr>
            <tr>
             <td colspan="2">
              <div style="margin-top: 5px;"><input type="image" style="cursor:pointer; margin-left:120px;" name="uploadFile"  src="<?=$button?>/b_upload.png" width="82" height="17"  border="0" onmouseout="this.src='<?=$button?>/b_upload.png'" onmouseover="this.src='<?=$button?>/b_uploadOver.png'"></div>
             </td>
            </tr>           
        </table>
       </td>
      </tr>
     </table>
    </div>
    <div class="txtLabel" style="margin-top: 10px;">Display</div>
    <div class="white_nx_box_right">
    <table width="402">
  <!--   <tr height="15">
      <td width="20">
       <input type="radio" name="display" value="">
      </td>
      <td>
       <span class="textBlack">Use default image encoding<br /></span>
      </td>
      <td></td>
     </tr> -->
     <tr height="15">
      <td>
       <input <?=$check_CustomDisplaySettings?> type="checkbox" id="CustomDisplaySettings" value="1" name="CustomDisplaySettings" onclick="enableDisplaySetting();">
      </td>
      <td>
       <span class="textBlack">Use custom settings<br /></span>
      </td>
      <td align="right">
      <input type="button" id="displaySetting" name="displaySetting" <?=$dis_displaySetting?> value="Settings..." onclick="javascript: setPanelOptionOther('displayCustom'); disableAction();">
      </td>
     </tr>
    </table>
    </div>
  </div>
 </div>
<!-- stop general -->

<!-- start advanced -->
<div id="panelLogin1" valign="middle" class="bgBoxInternal">
  <div class="content_nx_box_right">
    <span class="txtLabel">Network</span>
     <div class="white_nx_box_right">
     <table width="422">
     <tr height="20">
      <td width="20">
       <input type="checkbox" value="false" name="EnableSSL" id="EnableSSL" <?=$check_EnableSSL?>>
      </td>
      <td>
       <span class="txtLabel">Disable encryption of all traffic</span>
      </td>
      <td></td>
     </tr>
     <tr height="20">
      <td width="20">
       <input type="checkbox" name="NoZlibStreamCompression" id="NoZlibStreamCompression"  value="true" <?=$check_NoZlibStreamCompression?>>
      </td>
      <td>
       <span class="txtLabel">Disable ZLIB stream compression</span>
      </td>
      <td></td>
     </tr>
     <tr height="15">
      <td>
       <input type="checkbox"  value="true"  onchange="enableSetHttpProxy(this)" <?=$check_HttpProxy?> name="HttpProxy" id="HttpProxy">
      </td>
      <td>
       <span class="txtLabel">Connect through HTTP proxy</span>
      </td>
      <td align="right">
       <input name="setHttpProxy" onclick="setPanelOptionOther('proxy'); disableAction();" id="setHttpProxy" type="button" <?=$dis_setHttpProxy?>  value="Setting" style="width:100px;">
      </td>
     </tr>
    </table>
     </div>
  </div>
  


  <div class="content_nx_box_right" style="margin-top: 10px;">
    <span class="txtLabel">System</span>
     <div class="white_nx_box_right">
     <table width="422">
     <tr height="20">
      <td width="20">
       <input type="checkbox" value="true" name="grabKeyboard" id="grabKeyboard" <?=$check_grabKeyboard?>>
      </td>
      <td>
       <span class="txtLabel">Grab the keyboard when the client has focus</span>
      </td>
      <td></td>
     </tr>

     <tr height="20">
      <td width="20">
       <input type="checkbox" value="true" name="directDraw" id="directDraw" <?=$check_directDraw?>>
      </td>
      <td>
       <span class="txtLabel">Disable DirectDraw for screen rendering</span>
      </td>
      <td></td>
     </tr>
     
     <tr height="15">
      <td width="20">
       <input type="checkbox" name="lazyEncoder" value="true" id="lazyEncoder" <?=$check_lazyEncoder?>>
      </td>
      <td>
       <span class="txtLabel">Disable deferred screen updates</span>
      </td>
      <td></td>
     </tr>

    </table>
   </div>
   <div style="height:10px">&nbsp;</div>
  </div>
  
  
</div>
<!-- stop advanced -->
<!-- start service -->
<div id="panelLogin2" class="bgBoxInternal">
  <div class="content_nx_box_right">
   <span class="txtLabel">Multimedia</span>
     <div class="white_nx_box_right">
      <table>
       <tr>
        <td>
        <input type="checkbox" name="EnableMultimedia" id="EnableMultimedia" value="true" <?=$check_EnableMultimedia?>>
        </td>
        <td>
         <span class="txtLabel">Enable multimedia support</span>
        </td>
       </tr>
      </table>
     </div>
  </div>
</div>

<!-- stop  service -->
<!-- start  environment -->
<div id="panelLogin3" class="bgBoxInternal">
  <div class="content_nx_box_right">
   <span class="txtLabel">Font server</span>
     <div class="white_nx_box_right">
     <table width="322">
       <tr>
        <td width="30">
        <input type="checkbox" name="useFontServer" value="true" id="useFontServer" <?=$check_useFontServer?> onclick="setFontServer();">
        </td>
        <td>
         <span class="txtLabel">Use font server</span>
        </td><td></td>
       </tr>
       <tr>
        <td>
         <span class="txtLabel">Host</span>
        </td>
        <td>
         <input type="text" name="fontServerHost" id="fontServerHost" value="<?=$fontServerHostValue?>" <?=$dis_fontServerHost?>>
        </td>
        <td align="right">
         <span class="txtLabel">Port:</span> <input type="text" size="4" value="<?=$fontServerPortValue?>"  name="fontServerPort" id="fontServerPort" <?=$dis_fontServerPort?> >
        </td>
       </tr>
      </table>
     </div>
     
  </div>
</div>
<!-- stop  environment -->

