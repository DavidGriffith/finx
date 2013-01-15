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

if(!isset($check_winComp1)) $check_winComp1="";
if(!isset($check_winComp2)) $check_winComp2="";
if(!isset($check_winComp0)) $check_winComp0="";

if(!isset($check_useRdpJpg)) $check_useRdpJpg="";
if(!isset($dis_useRdpJpg)) $dis_useRdpJpg="";

if(!isset($jpegQualityRdpValue)) $jpegQualityRdpValue="6";

 

?>

<div id="border_box">
 <div class="content_box_pop_up">
   <p class="textBlack">Colors</p>
    <div class="white_nx_box_right">
     <?
      $brow=$_SERVER['HTTP_USER_AGENT'];
      if(strpos($brow, "Konqueror") || strpos($brow, "Mac_PowerPC") )
      {
       if($_POST['KonqRdbQuality'] == "8") $check_KonqRdbQuality0=" checked";
       if($_POST['KonqRdbQuality'] == "15") $check_KonqRdbQuality1=" checked";
       if($_POST['KonqRdbQuality'] == "16") $check_KonqRdbQuality2=" checked";
       if($_POST['KonqRdbQuality'] == "24") $check_KonqRdbQuality3=" checked";
       
      ?>
      <input type="hidden" size="4" name="rdpQuality" id="rdpQuality" value="8">
      <table  width="195" cellpadding="0" cellspacing="0" border="0">
       <tr>
        <td colspan="4" align="center" valign="top"><img src="<?=$shared?>/empty.png" border="0" height="8"></td>
       </tr>
       <tr>
        <td width="48" align="center"><input type="radio" name="KonqRdbQuality" onclick="setValueJpg('rdpQuality',this.value);" name="KonqRdbQuality0" value="8" <?=$check_KonqRdbQuality0?> ></td>        
        <td width="48" align="center"><input type="radio" name="KonqRdbQuality" onclick="setValueJpg('rdpQuality',this.value);" name="KonqRdbQuality1" value="15" <?=$check_KonqRdbQuality1?> ></td>
        <td width="48" align="center"><input type="radio" name="KonqRdbQuality" onclick="setValueJpg('rdpQuality',this.value);" name="KonqRdbQuality2" value="16" <?=$check_KonqRdbQuality2?> ></td>
        <td width="51" align="center"><input type="radio" name="KonqRdbQuality" onclick="setValueJpg('rdpQuality',this.value);" name="KonqRdbQuality3" value="24" <?=$check_KonqRdbQuality3?> ></td>
       </tr>
       <tr>
        <td width="48"><div style="margin-left: 0px; text-align: center; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>        
        <td width="48"><div style="margin-left: 0px; text-align: center; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
        <td width="48"><div style="margin-left: 0px; text-align: center; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
        <td width="51"><div style="margin-top: 0px; text-align: center;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
       </tr>
       </table>
       <table  width="195" cellpadding="0" cellspacing="0" border="0"  class="textBlack">
       <tr>
        <td align="center" width="48">256</td>
        <td align="center" width="48">32K</td>
        <td align="center" width="48">64K</td>
        <td align="center" width="51">16M</td>
       </tr>
       </table>      
      
      <?
      }
      else
      {
      ?>
      <table width="195" cellpadding="0" cellspacing="0" border="0" style="margin-left:5px;">
      <tr>
       <td>
        <div id="sliderDivWindows"></div>
        <input type="hidden" size="4" name="rdpQuality" id="rdpQuality" value="8">
       </td>
      </tr>
      </table>
      
      <table  width="195" cellpadding="0" cellspacing="0" border="0" class="BgSlider">
       <tr>
        <td colspan="4" align="center" valign="top"><img src="<?=$shared?>/empty.png" border="0" height="8"></td>
       </tr>
       <tr>
        <td width="48"><div style="margin-left: 9px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>        
        <td width="48"><div style="margin-left: 22px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
        <td width="48"><div style="margin-left: 35px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
        <td width="51"><div style="margin-top: 0px; text-align: right; margin-right: 1px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
       </tr>
       </table>
       <table  width="200" cellpadding="0" cellspacing="0" border="0"  class="textBlack">
       <tr>
        <td width="48">256</td>
        <td align="center" width="48">32K</td>
        <td align="right" width="48">64K</td>
        <td align="right" width="48">16M</td>
       </tr>
       </table>      
       <?
       }
       ?>
    </div>
   <p class="textBlack">Images</p>
    <div class="white_nx_box_right">
    <table border="0" width="250" align="center">
     <tr>
      <td><input name="winComp" id="winComp" type="radio" value="3" onclick="slideSetting('useRdpJpg','1');" <?=$check_winComp3?> ></td>
      <td><span class="textBlack">Use both JPEG and RGB compression</span></td>
     </tr>
     <tr>
      <td><input name="winComp" id="winComp" value="1" type="radio" onclick="slideSetting('useRdpJpg','1');"  <?=$check_winComp1?> ></td>
      <td><span class="textBlack">Only use JPEG compression</span></td>
     </tr>
     <tr>
      <td><input name="winComp" id="winComp" type="radio" value="2" onclick="disableSlide('useRdpJpg'); sliderJpgRdp.setDisabled(true);"  <?=$check_winComp2?>></td>
      <td ><span class="textBlack">Only use the RGB compression</span></td>
     </tr>
     <tr>
      <td><input name="winComp" id="winComp" type="radio" value="0" onclick="disableSlide('useRdpJpg'); sliderJpgRdp.setDisabled(true);" <?=$check_winComp0?>></td>
      <td><span class="textBlack">Use plain X bitmaps</span></td>
     </tr>
     </table>

     
     <table border="0" width="200" align="center" style="margin-left: 10px;" >
     <tr>
      <td colspan="2">Quality:</td>
     </tr>
      <?
      $brow=$_SERVER['HTTP_USER_AGENT'];
      if(strpos($brow, "Konqueror") || strpos($brow, "Mac_PowerPC") )
      {
       $dis_KonqJpgRdp0=" disabled";
       $dis_KonqJpgRdp1=" disabled";
       $dis_KonqJpgRdp2=" disabled";
       $dis_KonqJpgRdp3=" disabled";
       $dis_KonqJpgRdp4=" disabled";
       $dis_KonqJpgRdp5=" disabled";
       $dis_KonqJpgRdp6=" disabled";
       $dis_KonqJpgRdp7=" disabled";
       $dis_KonqJpgRdp8=" disabled";
       $dis_KonqJpgRdp9=" disabled";
       
       if($_POST['useRdpJpg']=="-1")
       {
       //$check_useRdpJpg=" checked";
       $dis_KonqJpgRdp0=" ";
       $dis_KonqJpgRdp1=" ";
       $dis_KonqJpgRdp2=" ";
       $dis_KonqJpgRdp3=" ";
       $dis_KonqJpgRdp4=" ";
       $dis_KonqJpgRdp5=" ";
       $dis_KonqJpgRdp6=" ";
       $dis_KonqJpgRdp7=" ";
       $dis_KonqJpgRdp8=" ";
       $dis_KonqJpgRdp9=" ";
              
        if($_POST['KonqJpgRdp']=="0") $check_KonqJpgRdp0=" checked";
        if($_POST['KonqJpgRdp']=="1") $check_KonqJpgRdp1=" checked";
        if($_POST['KonqJpgRdp']=="2") $check_KonqJpgRdp2=" checked";
        if($_POST['KonqJpgRdp']=="3") $check_KonqJpgRdp3=" checked";
        if($_POST['KonqJpgRdp']=="4") $check_KonqJpgRdp4=" checked";
        if($_POST['KonqJpgRdp']=="5") $check_KonqJpgRdp5=" checked";
        if($_POST['KonqJpgRdp']=="6") $check_KonqJpgRdp6=" checked";
        if($_POST['KonqJpgRdp']=="7") $check_KonqJpgRdp7=" checked";
        if($_POST['KonqJpgRdp']=="8") $check_KonqJpgRdp8=" checked";
        if($_POST['KonqJpgRdp']=="9") $check_KonqJpgRdp9=" checked";
       } 
      ?>
     <tr>
      <td width="25">
      <input name="useRdpJpg" id="useRdpJpg" type="checkbox" value="-1" <?=$check_useRdpJpg?> <?=$dis_useRdpJpg?> onclick="slideSettingKonq('useRdpJpg','KonqJpgRdp');"> </td>
      <td><span class="textBlack">Use custom JPEG quality:</span></td>
     </tr>
      <td colspan="2">
       <input type="hidden" id="jpegQualityRdp" name="jpegQualityRdp" value="<?=$jpegQualityRdpValue?>">
       <table  width="232" cellpadding="0" cellspacing="0" border="0">
        <tr>
         <td width="24"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityRdp',this.value);" name="KonqJpgRdp" id="KonqJpgRdp0" value="0" <?=$check_KonqJpgRdp0?> <?=$dis_KonqJpgRdp0?> ></div></td>        
         <td width="24"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityRdp',this.value);" name="KonqJpgRdp" id="KonqJpgRdp1" value="1" <?=$check_KonqJpgRdp1?> <?=$dis_KonqJpgRdp1?> ></div></td>
         <td width="24"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityRdp',this.value);" name="KonqJpgRdp" id="KonqJpgRdp2" value="2" <?=$check_KonqJpgRdp2?> <?=$dis_KonqJpgRdp2?> ></div></td>
         <td width="22"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityRdp',this.value);" name="KonqJpgRdp" id="KonqJpgRdp3" value="3" <?=$check_KonqJpgRdp3?> <?=$dis_KonqJpgRdp3?> ></div></td>
         <td width="23"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityRdp',this.value);" name="KonqJpgRdp" id="KonqJpgRdp4" value="4" <?=$check_KonqJpgRdp4?> <?=$dis_KonqJpgRdp4?> ></div></td>
         <td width="23"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityRdp',this.value);" name="KonqJpgRdp" id="KonqJpgRdp5" value="5" <?=$check_KonqJpgRdp5?> <?=$dis_KonqJpgRdp5?> ></div></td>
         <td width="23"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityRdp',this.value);" name="KonqJpgRdp" id="KonqJpgRdp6" value="6" <?=$check_KonqJpgRdp6?> <?=$dis_KonqJpgRdp6?> ></div></td>
         <td width="23"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityRdp',this.value);" name="KonqJpgRdp" id="KonqJpgRdp7" value="7" <?=$check_KonqJpgRdp7?> <?=$dis_KonqJpgRdp7?> ></div></td>
         <td width="23"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityRdp',this.value);" name="KonqJpgRdp" id="KonqJpgRdp8" value="8" <?=$check_KonqJpgRdp8?> <?=$dis_KonqJpgRdp8?> ></div></td>
         <td width="23" align="center"><input type="radio" onclick="setValueJpg('jpegQualityRdp',this.value);" name="KonqJpgRdp" id="KonqJpgRdp9" value="9" <?=$check_KonqJpgRdp9?> <?=$dis_KonqJpgRdp9?> ></td>
        </tr>
        <tr>
         <td width="24"><div style="text-align:center; margin-left: 0px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>        
         <td width="24"><div style="text-align:center; margin-left: 0px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
         <td width="24"><div style="text-align:center; margin-left: 0px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
         <td width="22"><div style="text-align:center; margin-left: 0px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
         <td width="23"><div style="text-align:center; margin-left: 0px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
         <td width="23"><div style="text-align:center; margin-left: 0px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
         <td width="23"><div style="text-align:center; margin-left: 0px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
         <td width="23"><div style="text-align:center; margin-left: 0px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
         <td width="23"><div style="text-align:center; margin-left: 0px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
         <td width="23" align="center"><img src="<?=$button?>/sep.png"  height="4" width="1"></td>
        </tr>
        </table>
        <table id="tableSlide-useRdpJpg" width="232" cellpadding="0" cellspacing="0" border="0">
        <tr>
         <td class="t_grey" width="24" align="center">0</td>
         <td class="t_grey" width="24" align="center">1</td>
         <td class="t_grey" width="24" align="center">2</td>
         <td class="t_grey" width="22" align="center">3</td>
         <td class="t_grey" width="23" align="center">4</td>
         <td class="t_grey" width="23" align="center">5</td>
         <td class="t_grey" width="23" align="center">6</td>
         <td class="t_grey" width="23" align="center">7</td>
         <td class="t_grey" width="23" align="center">8</td>
         <td class="t_grey" width="23" align="center">9</td>
        </tr>
        </table>
       </td>
      <?
      }
      else
      {
      ?>

     <tr>
      <td><input name="useRdpJpg" id="useRdpJpg" type="checkbox" value="1" onclick="slideSetting('useRdpJpg','1');"  <?=$check_useRdpJpg?> <?=$dis_useRdpJpg?>></td>      
      <td>Use custom JPEG quality</td>
     </tr>

     <tr>
      <td colspan="2">
       
       <table border="0" align="center">
       <tr>  
        <td width="25">&nbsp;</td>
        <td>

         <input type="hidden" id="jpegQualityRdp" name="jpegQualityRdp" value="<?=$jpegQualityRdpValue?>">
 
         <table width="195" cellpadding="0" cellspacing="0" border="0" style="margin-left:5px;">
         <tr>
          <td>  
           <div id="sliderDivJpgRdp"></div>
          </td>
         </tr>
         </table>
 
         <table  width="195" cellpadding="0" cellspacing="0" border="0" class="BgSlider">
         <tr>
          <td colspan="10" align="center" valign="top"><img src="<?=$shared?>/empty.png" border="0" height="8"></td>
         </tr>
         <tr>
          <td width="19"><div style="margin-left: 10px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>        
          <td width="19"><div style="margin-left: 11px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
          <td width="19"><div style="margin-left: 13px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
          <td width="19"><div style="margin-left: 14px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
          <td width="19"><div style="margin-left: 16px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
          <td width="19"><div style="margin-left: 17px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
          <td width="19"><div style="margin-left: 19px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
          <td width="19"><div style="margin-left: 19px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
          <td width="19"><div style="margin-left: 19px; margin-top: 0px;"><img src="<?=$button?>/sep.png"  height="4" width="1"></div></td>
          <td width="24" align="right"><img src="<?=$button?>/sep.png"  height="4" width="1"></td>
         </tr>
         </table>
         <table id="tableSlide-useRdpJpg"  width="198" cellpadding="1" cellspacing="0" border="0">
         <tr>
          <td class="t_grey" width="22" align="center">0</td>
          <td class="t_grey" width="19" align="center">1</td>
          <td class="t_grey" width="22" align="center">2</td>
          <td class="t_grey" width="19" align="center">3</td>
          <td class="t_grey" width="21" align="center">4</td>
          <td class="t_grey" width="21" align="center">5</td>
          <td class="t_grey" width="19" align="center">6</td>
          <td class="t_grey" width="20" align="center">7</td>
          <td class="t_grey" width="19" align="center">8</td>
          <td class="t_grey" width="15" align="right">9</td>
         </tr>
         </table>        
        </td>
        <?
        }
        ?>
        </tr> 
       </table>
     
      </td>
     </tr>
    </table>
    </div>
   <p class="textBlack">Cache</p>
    <div class="white_nx_box_right">
    <table width="250" align="center">
     <tr>
      <td><input name="rdpCache" id="rdpCache"  type="checkbox" value="true" <?=$check_rdpCache?>></td>
      <td><span class="textBlack">Enable RDP image chache</span></td>
     </tr>
    </table>
    </div>

   <table>
    <tr>
     <td>
      <img src="<?=$button?>/b_ok.png" onclick="javascript: document.getElementById('winCustomSetting').style.display='none';  disableAction();" onmouseout="this.src='<?=$button?>/b_ok.png'" onmouseover="this.src='<?=$button?>/b_okOver.png'">
     </td>
     <td>
      <img src="<?=$button?>/b_cancel.png" onclick="javascript: document.getElementById('winCustomSetting').style.display='none';  disableAction();" onmouseout="this.src='<?=$button?>/b_cancel.png'" onmouseover="this.src='<?=$button?>/b_cancelOver.png'">
     </td>
    </tr>
   </table>

 </div>
</div>
