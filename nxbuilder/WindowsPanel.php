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
if(!isset($dis_WindowDomain)) $dis_WindowDomain="";

if(!isset($check_RdpCredential0)) $check_RdpCredential0="";
if(!isset($usernameWindowsValue)) $usernameWindowsValue="";
if(!isset($passwordWindowsValue)) $passwordWindowsValue="";


if(!isset($check_RdpCredential1)) $check_RdpCredential1="";

if(!isset($check_runWindowsType0)) $check_runWindowsType0="";
if(!isset($check_runWindowsType1)) $check_runWindowsType1="";
if(!isset($applicationWindowsValue)) $applicationWindowsValue="";
if(!isset($dis_applicationWindows)) $dis_applicationWindows="";

?>
<div id="border_box">
<div class="content_box_pop_up">
   <p class="textBlack">Windows Terminal Server</p>
    <div class="white_nx_box_right">
    <table width="250" align="center" cellpadding="2" cellspacing="0" border="0">
     <tr>
      <td><span class="textBlack">Server</span></td><td><input name="WindowServer" id="WindowServer" value="<?=$WindowServerValue?>" type="text"></td>
     </tr>
     <tr>
      <td><span class="textBlack">Domain</span></td><td><input name="WindowDomain" id="WindowDomain" value="<?=$WindowDomainValue?>" <?=$dis_WindowDomain?> type="text"></td>
     </tr>
    </table>
   </div>
   <p class="textBlack">Authentication</p>
    <div class="white_nx_box_right">
    <table width="250" align="center" cellpadding="2" cellspacing="0" border="0">
     <tr>
      <td width="20"><input name="RdpCredential" id="RdpCredential" type="radio" value="2"  <?=$check_RdpCredential2?> onclick="setWindowsPanel('2')"></td>
      <td><span class="textBlack">Use the NX user 's credential</span></td>
     </tr>
     <tr>
      <td width="20"><input name="RdpCredential" id="RdpCredential" type="radio" value="0" <?=$check_RdpCredential0?> onclick="setWindowsPanel('0')"></td>
      <td><span class="textBlack">Use following credential</span></td>
     </tr>
    </table>
    <table width="250" align="center" cellpadding="2" cellspacing="0" border="0">
      <tr>
      <td width="20">&nbsp;</td>
      <td><span class="textBlack">User</span></td><td><input name="usernameWindows" id="usernameWindows" type="text" <?=$dis_usernameWindows?> value="<?=$usernameWindowsValue?>"></td>
     </tr>
     <tr>
     <td width="20">&nbsp;</td>
      <td><span class="textBlack">Password</span></td><td><input name="passwordWindows" id="passwordWindows" <?=$dis_passwordWindows?> type="password" value="<?=$passwordWindowsValue?>"></td>
     </tr>
    </table>
    <table align="center" cellpadding="2" cellspacing="0" border="0" style="margin-left: 18px;" >
    <tr>
      <td width="20"><input name="RememberPasswordWindows" id="RememberPasswordWindows" value="true" type="checkbox" <?=$check_RememberPasswordWindows?> <?=$dis_RememberPasswordWindows?>></td>
      <td><span class="textBlack">Remember my password</span></td>
     </tr>
    </table>
    <table width="250"  align="center" cellpadding="2" cellspacing="0" border="0">   
    <tr>
      <td width="20"><input name="RdpCredential" id="RdpCredential" type="radio" value="1" <?=$check_RdpCredential1?> onclick="setWindowsPanel('1')"></td>
      <td><span class="textBlack">Show the Windows login screen</span></td>
     </tr>
     </table>
    </div>
  <p class="textBlack">Session Type</p>
    <div class="white_nx_box_right">
    <table width="250" align="center">
     <tr>
      <td><input name="runWindowsType" id="runWindowsType0" type="radio" <?=$check_runWindowsType0?> value="false" onclick="enableApplicationWindows('0')"></td><td><span class="textBlack">Run desktop</span></td>
     </tr>
     <tr>
      <td><input name="runWindowsType" id="runWindowsType1" type="radio" <?=$check_runWindowsType1?> value="true" onclick="enableApplicationWindows('1')"></td><td><span class="textBlack">Run application</span></td>
     </tr>
     <tr>
      <td></td><td><input name="applicationWindows" id="applicationWindows" <?=$dis_applicationWindows?> value="<?=$applicationWindowsValue?>" type="text"></td>
     </tr>
    </table>
    </div>
 <table>
  <tr>
   <td>
    <img src="<?=$button?>/b_ok.png" onclick="javascript: document.getElementById('winOption').style.display='none';  disableAction();" onmouseout="this.src='<?=$button?>/b_ok.png'" onmouseover="this.src='<?=$button?>/b_okOver.png'">
   </td>
   <td>
    <img src="<?=$button?>/b_cancel.png" onclick="javascript: document.getElementById('winOption').style.display='none';  disableAction();" onmouseout="this.src='<?=$button?>/b_cancel.png'" onmouseover="this.src='<?=$button?>/b_cancelOver.png'">
   </td>
  </tr>
 </table>


</div></div>
