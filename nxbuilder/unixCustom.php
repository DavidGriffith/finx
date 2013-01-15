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

//init following variables to prevent notice output by php interpreter 

if(!isset($check_compressionType1)) $check_compressionType1="";
if(!isset($check_compressionType2)) $check_compressionType2="";
if(!isset($check_compressionType0)) $check_compressionType0="";
if(!isset($check_enableJpegQuality)) $check_enableJpegQuality="";
if(!isset($dis_enableJpegQuality)) $dis_enableJpegQuality="";
if(!isset($check_renderExt)) $check_renderExt="";
if(!isset($check_backingStore)) $check_backingStore="";
if(!isset($check_composite)) $check_composite="";
if(!isset($check_shm)) $check_shm="";
if(!isset($check_sharedPix)) $check_sharedPix="";
if(!isset($dis_sharedPix)) $dis_sharedPix="";











?>
<div id="border_box">
<div class="content_box_pop_up">
   <p class="textBlack">Images</p>
    <div class="white_nx_box_right">
   <table border="0" width="250" align="center">
    <tr>
      <td><input name="compressionType" type="radio" value="3" <?=$check_compressionType3?> onclick="slideSetting('enableJpegQuality','0');"></td>
      <td colspan="2"><span class="textBlack">Use both JPEG and RGB compression</span></td>
     </tr>
     <tr>
      <td><input name="compressionType" type="radio" value="1"  <?=$check_compressionType1?> onclick="slideSetting('enableJpegQuality','0');"></td>
      <td colspan="2"><span class="textBlack">Only use JPEG compression</span></td>
     </tr>
 
     <tr>
      <td><input name="compressionType"  value="2" type="radio" <?=$check_compressionType2?> onclick="disableSlide('enableJpegQuality'); sliderUnix.setDisabled(true);"></td>
      <td colspan="2"><span class="textBlack">Only use RGB compression</span></td>
     </tr>
     <tr>
      <td><input name="compressionType"  value="0" type="radio" <?=$check_compressionType0?> onclick="disableSlide('enableJpegQuality'); sliderUnix.setDisabled(true);"></td>
      <td colspan="2"><span class="textBlack">Use plain X bitmaps</span></td>
     </tr>
    <tr>
     <td colspan="3">Quality:</td>
    </tr>
      <?
      $brow=$_SERVER['HTTP_USER_AGENT'];
      if(strpos($brow, "Konqueror") || strpos($brow, "Mac_PowerPC") )
      {
       $dis_KonqJpgUnix0=" disabled";
       $dis_KonqJpgUnix1=" disabled";
       $dis_KonqJpgUnix2=" disabled";
       $dis_KonqJpgUnix3=" disabled";
       $dis_KonqJpgUnix4=" disabled";
       $dis_KonqJpgUnix5=" disabled";
       $dis_KonqJpgUnix6=" disabled";
       $dis_KonqJpgUnix7=" disabled";
       $dis_KonqJpgUnix8=" disabled";
       $dis_KonqJpgUnix9=" disabled";
       if($enableJpegQuality=="-1")
       {
       $dis_KonqJpgUnix0=" ";
       $dis_KonqJpgUnix1=" ";
       $dis_KonqJpgUnix2=" ";
       $dis_KonqJpgUnix3=" ";
       $dis_KonqJpgUnix4=" ";
       $dis_KonqJpgUnix5=" ";
       $dis_KonqJpgUnix6=" ";
       $dis_KonqJpgUnix7=" ";
       $dis_KonqJpgUnix8=" ";
       $dis_KonqJpgUnix9=" ";
              
       if($_POST['KonqJpgUnix']=="0") $check_KonqJpgUnix0=" checked";
       if($_POST['KonqJpgUnix']=="1") $check_KonqJpgUnix1=" checked";
       if($_POST['KonqJpgUnix']=="2") $check_KonqJpgUnix2=" checked";
       if($_POST['KonqJpgUnix']=="3") $check_KonqJpgUnix3=" checked";
       if($_POST['KonqJpgUnix']=="4") $check_KonqJpgUnix4=" checked";
       if($_POST['KonqJpgUnix']=="5") $check_KonqJpgUnix5=" checked";
       if($_POST['KonqJpgUnix']=="6") $check_KonqJpgUnix6=" checked";
       if($_POST['KonqJpgUnix']=="7") $check_KonqJpgUnix7=" checked";
       if($_POST['KonqJpgUnix']=="8") $check_KonqJpgUnix8=" checked";
       if($_POST['KonqJpgUnix']=="9") $check_KonqJpgUnix9=" checked";
       }
      ?>
     <tr>
      <td width="25">
      <input name="enableJpegQuality" id="enableJpegQuality" type="checkbox" value="-1" <?=$check_enableJpegQuality?> <?=$dis_enableJpegQuality?> onclick="slideSettingKonq('enableJpegQuality','KonqJpgUnix');"> </td>
      <td><span class="textBlack">Use custom JPEG quality:</span></td>
     </tr>
      <td colspan="2">
       <input type="hidden" id="jpegQuality" name="jpegQuality" value="<? if(isset($jpegQualityValue)) echo $jpegQualityValue; ?>">
       <table  width="232" cellpadding="0" cellspacing="0" border="0">
        <tr>
         <td width="24"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQuality',this.value);" name="KonqJpgUnix" id="KonqJpgUnix0" value="0" <?=$check_KonqJpgUnix0?> <?=$dis_KonqJpgUnix0?> ></div></td>        
         <td width="24"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQuality',this.value);" name="KonqJpgUnix" id="KonqJpgUnix1" value="1" <?=$check_KonqJpgUnix1?> <?=$dis_KonqJpgUnix1?> ></div></td>
         <td width="24"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQuality',this.value);" name="KonqJpgUnix" id="KonqJpgUnix2" value="2" <?=$check_KonqJpgUnix2?> <?=$dis_KonqJpgUnix2?> ></div></td>
         <td width="22"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQuality',this.value);" name="KonqJpgUnix" id="KonqJpgUnix3" value="3" <?=$check_KonqJpgUnix3?> <?=$dis_KonqJpgUnix3?> ></div></td>
         <td width="23"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQuality',this.value);" name="KonqJpgUnix" id="KonqJpgUnix4" value="4" <?=$check_KonqJpgUnix4?> <?=$dis_KonqJpgUnix4?> ></div></td>
         <td width="23"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQuality',this.value);" name="KonqJpgUnix" id="KonqJpgUnix5" value="5" <?=$check_KonqJpgUnix5?> <?=$dis_KonqJpgUnix5?> ></div></td>
         <td width="23"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQuality',this.value);" name="KonqJpgUnix" id="KonqJpgUnix6" value="6" <?=$check_KonqJpgUnix6?> <?=$dis_KonqJpgUnix6?> ></div></td>
         <td width="23"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQuality',this.value);" name="KonqJpgUnix" id="KonqJpgUnix7" value="7" <?=$check_KonqJpgUnix7?> <?=$dis_KonqJpgUnix7?> ></div></td>
         <td width="23"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQuality',this.value);" name="KonqJpgUnix" id="KonqJpgUnix8" value="8" <?=$check_KonqJpgUnix8?> <?=$dis_KonqJpgUnix8?> ></div></td>
         <td width="23" align="center"><input type="radio" onclick="setValueJpg('jpegQuality',this.value);" name="KonqJpgUnix" id="KonqJpgUnix9" value="9" <?=$check_KonqJpgUnix9?> <?=$dis_KonqJpgUnix9?> ></td>
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
        <table id="tableSlide-enableJpegQuality" width="232" cellpadding="0" cellspacing="0" border="0">
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
      <!-- start of slide -->     
     <tr>
      <td width="25">
      <input name="enableJpegQuality" id="enableJpegQuality" type="checkbox" value="-1" <?=$check_enableJpegQuality?> <?=$dis_enableJpegQuality?> onclick="slideSetting('enableJpegQuality','0');"> </td>
      <td><span class="textBlack">Use custom JPEG quality:</span></td>
     </tr>
     <td width="25">&nbsp;</td>
      <td align="left"> 
        <input type="hidden" id="jpegQuality" name="jpegQuality" value="<?if(isset($jpegQualityValue)) echo $jpegQualityValue;?>">
      <table width="195" cellpadding="0" cellspacing="0" border="0" style="margin-left:5px;">
        <tr>
         <td>  
          <div id=sliderDivUnix></div>
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
        <table id="tableSlide-enableJpegQuality"  width="198" cellpadding="1" cellspacing="0" border="0">
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
      <!-- end of slide -->
      <?
      }
      ?>
     </tr>
     </table>
    </div>
   <p class="textBlack">Performance</p>
    <div class="white_nx_box_right">
    <table width="250" align="center">
     <tr>
      <td><input name="renderExt" id="renderExt" value="false" type="checkbox" <?=$check_renderExt?> ></td><td><span class="textBlack">Disable the render extension</span></td>
     </tr>
     <tr>
      <td><input name="backingStore" id="backingStore" value="true" type="checkbox" <?=$check_backingStore?> ></td><td><span class="textBlack">Disable the backing-store</span></td>
     </tr>
     <tr>
      <td><input name="composite" id="composite" type="checkbox" value="true" <?=$check_composite?> ></td><td><span class="textBlack">Disable the composite extension</span></td>
     </tr>
     <tr>
      <td><input name="shm" id="shm" type="checkbox" value="true" onclick="setPix();" <?=$check_shm?> ></td><td><span class="textBlack">Disable the shared memory extension</span></td>
     </tr>
     <tr>
      <td><input name="sharedPix" id="sharedPix" value="true" type="checkbox" <?=$check_sharedPix?> <?=$dis_sharedPix?> ></td><td><span class="textBlack">Disable emolation of shared pixmap</span></td>
     </tr>
    </table>
    </div>
    <table>
  <tr>
   <td>
    <img src="<?=$button?>/b_ok.png" onclick="javascript: document.getElementById('unixCustomSetting').style.display='none';  disableAction();" onmouseout="this.src='<?=$button?>/b_ok.png'" onmouseover="this.src='<?=$button?>/b_okOver.png'">
   </td>
   <td>
    <img src="<?=$button?>/b_cancel.png" onclick="javascript: document.getElementById('unixCustomSetting').style.display='none';  disableAction();" onmouseout="this.src='<?=$button?>/b_cancel.png'" onmouseover="this.src='<?=$button?>/b_cancelOver.png'">
   </td>
  </tr>
</table>

</div></div>
