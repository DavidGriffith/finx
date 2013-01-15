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
if(!isset($check_encodType1)) $check_encodType1="";
if(!isset($check_encodType2)) $check_encodType2="";
if(!isset($check_encodType0)) $check_encodType0="";

if(!isset($check_useVncJpg)) $check_useVncJpg="";
if(!isset($dis_useVncJpg)) $dis_useVncJpg="";

if(!isset($jpegQualityRdpValue)) $jpegQualityRdpValue="6";

if(!isset($jpegQualityVncValue)) $jpegQualityVncValue="6";

 
?>


<div id="border_box">
<div class="content_box_pop_up">
   <p class="textBlack">Encoding</p>
    <div class="white_nx_box_right">
    <table border="0" width="250" align="center">
     <tr>
      <td><input name="encodType" id="encodType" value="3" type="radio" <?=$check_encodType3?> onclick="slideSetting('useVncJpg','2');"></td>
      <td><span class="textBlack">Use both JPEG and RGB compression</span></td>
     </tr>
     <tr>
      <td><input name="encodType" id="encodType" value="1" type="radio" <?=$check_encodType1?> onclick="slideSetting('useVncJpg','2');"></td>
      <td><span class="textBlack">Only use JPEG compression</span></td>
     </tr>
     <tr>
      <td><input name="encodType" id="encodType" value="2" type="radio" <?=$check_encodType2?> onclick="disableSlide('useVncJpg'); sliderJpgVnc.setDisabled(true);"></td>
      <td><span class="textBlack">Only use the RGB compression</span></td>
     </tr>
     <tr>
      <td><input name="encodType" id="encodType" value="0" type="radio" <?=$check_encodType0?> onclick="disableSlide('useVncJpg'); sliderJpgVnc.setDisabled(true);"></td>
      <td><span class="textBlack">Use plain X bitmaps</span></td>
     </tr>     
    </table>


   <table border="0" width="200" align="center" style="margin-left: 5px;" >
   <tr>
    <td colspan="2">Quality:</td>
   </tr>
   <?
      $brow=$_SERVER['HTTP_USER_AGENT'];
      if(strpos($brow, "Konqueror") || strpos($brow, "Mac_PowerPC") )
      {
       $dis_KonqJpgVnc0=" disabled";
       $dis_KonqJpgVnc1=" disabled";
       $dis_KonqJpgVnc2=" disabled";
       $dis_KonqJpgVnc3=" disabled";
       $dis_KonqJpgVnc4=" disabled";
       $dis_KonqJpgVnc5=" disabled";
       $dis_KonqJpgVnc6=" disabled";
       $dis_KonqJpgVnc7=" disabled";
       $dis_KonqJpgVnc8=" disabled";
       $dis_KonqJpgVnc9=" disabled";
       
       if($_POST['useVncJpg']=="-1")
       {
       $dis_KonqJpgVnc0=" ";
       $dis_KonqJpgVnc1=" ";
       $dis_KonqJpgVnc2=" ";
       $dis_KonqJpgVnc3=" ";
       $dis_KonqJpgVnc4=" ";
       $dis_KonqJpgVnc5=" ";
       $dis_KonqJpgVnc6=" ";
       $dis_KonqJpgVnc7=" ";
       $dis_KonqJpgVnc8=" ";
       $dis_KonqJpgVnc9=" ";
              
        if($_POST['KonqJpgVnc']=="0") $check_KonqJpgVnc0=" checked";
        if($_POST['KonqJpgVnc']=="1") $check_KonqJpgVnc1=" checked";
        if($_POST['KonqJpgVnc']=="2") $check_KonqJpgVnc2=" checked";
        if($_POST['KonqJpgVnc']=="3") $check_KonqJpgVnc3=" checked";
        if($_POST['KonqJpgVnc']=="4") $check_KonqJpgVnc4=" checked";
        if($_POST['KonqJpgVnc']=="5") $check_KonqJpgVnc5=" checked";
        if($_POST['KonqJpgVnc']=="6") $check_KonqJpgVnc6=" checked";
        if($_POST['KonqJpgVnc']=="7") $check_KonqJpgVnc7=" checked";
        if($_POST['KonqJpgVnc']=="8") $check_KonqJpgVnc8=" checked";
        if($_POST['KonqJpgVnc']=="9") $check_KonqJpgVnc9=" checked";
       }      
      
      
      ?>
     <tr>
      <td width="25">
      <input name="useVncJpg" id="useVncJpg" type="checkbox" value="-1" <?=$check_useVncJpg?> <?=$dis_useVncJpg?> onclick="slideSettingKonq('useVncJpg','KonqJpgVnc');"> </td>
      <td><span class="textBlack">Use custom JPEG quality:</span></td>
     </tr>
      <td colspan="2">
       <input type="hidden" id="jpegQualityVnc" name="jpegQualityVnc" value="<?=$jpegQualityVncValue?>">
       <table  width="232" cellpadding="0" cellspacing="0" border="0">
        <tr>
         <td width="24"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityVnc',this.value);" name="KonqJpgVnc" id="KonqJpgVnc0" value="0" <?=$check_KonqJpgVnc0?> <?=$dis_KonqJpgVnc0?> ></div></td>        
         <td width="24"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityVnc',this.value);" name="KonqJpgVnc" id="KonqJpgVnc1" value="1" <?=$check_KonqJpgVnc1?> <?=$dis_KonqJpgVnc1?> ></div></td>
         <td width="24"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityVnc',this.value);" name="KonqJpgVnc" id="KonqJpgVnc2" value="2" <?=$check_KonqJpgVnc2?> <?=$dis_KonqJpgVnc2?> ></div></td>
         <td width="22"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityVnc',this.value);" name="KonqJpgVnc" id="KonqJpgVnc3" value="3" <?=$check_KonqJpgVnc3?> <?=$dis_KonqJpgVnc3?> ></div></td>
         <td width="23"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityVnc',this.value);" name="KonqJpgVnc" id="KonqJpgVnc4" value="4" <?=$check_KonqJpgVnc4?> <?=$dis_KonqJpgVnc4?> ></div></td>
         <td width="23"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityVnc',this.value);" name="KonqJpgVnc" id="KonqJpgVnc5" value="5" <?=$check_KonqJpgVnc5?> <?=$dis_KonqJpgVnc5?> ></div></td>
         <td width="23"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityVnc',this.value);" name="KonqJpgVnc" id="KonqJpgVnc6" value="6" <?=$check_KonqJpgVnc6?> <?=$dis_KonqJpgVnc6?> ></div></td>
         <td width="23"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityVnc',this.value);" name="KonqJpgVnc" id="KonqJpgVnc7" value="7" <?=$check_KonqJpgVnc7?> <?=$dis_KonqJpgVnc7?> ></div></td>
         <td width="23"><div style="margin-left: 0px; margin-top: 0px;"><input type="radio" onclick="setValueJpg('jpegQualityVnc',this.value);" name="KonqJpgVnc" id="KonqJpgVnc8" value="8" <?=$check_KonqJpgVnc8?> <?=$dis_KonqJpgVnc8?> ></div></td>
         <td width="23" align="center"><input type="radio" onclick="setValueJpg('jpegQualityVnc',this.value);" name="KonqJpgVnc" id="KonqJpgVnc9" value="9" <?=$check_KonqJpgVnc9?> <?=$dis_KonqJpgVnc9?>></td>
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
        <table id="tableSlide-useVncJpg" width="232" cellpadding="0" cellspacing="0" border="0">
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
    <td><input name="useVncJpg" id="useVncJpg" type="checkbox" value="1"  <?=$check_useVncJpg?> <?=$dis_useVncJpg?> onclick="slideSetting('useVncJpg','2');"></td>      
    <td>Use custom JPEG quality</td>
   </tr>

   <tr>
    <td colspan="2">
     
     <table border="0" align="center">
     <tr>  
      <td width="25">&nbsp;</td>
      <td>

       <input type="hidden" id="jpegQualityVnc" name="jpegQualityVnc" value="<?=$jpegQualityVncValue?>">

       <table width="195" cellpadding="0" cellspacing="0" border="0" style="margin-left:5px;">
       <tr>
        <td>  
         <div id="sliderDivJpgVnc"></div>
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
       <table id="tableSlide-useVncJpg"  width="198" cellpadding="1" cellspacing="0" border="0">
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
   <table>
  <tr>
   <td>
    <img src="<?=$button?>/b_ok.png" onclick="javascript: document.getElementById('vncCustomSetting').style.display='none';  disableAction();" onmouseout="this.src='<?=$button?>/b_ok.png'" onmouseover="this.src='<?=$button?>/b_okOver.png'">
   </td>
   <td>
    <img src="<?=$button?>/b_cancel.png" onclick="javascript: document.getElementById('vncCustomSetting').style.display='none';  disableAction();" onmouseout="this.src='<?=$button?>/b_cancel.png'" onmouseover="this.src='<?=$button?>/b_cancelOver.png'">
   </td>
  </tr>
</table>

</div>
</div>

