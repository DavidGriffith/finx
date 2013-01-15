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


if(!isset($VncHostValue)) $VncHostValue="";

if(!isset($VncPortValue)) $VncPortValue="";
if(!isset($vncPasswordValue)) $vncPasswordValue="";
if(!isset($check_vncRememberPassword)) $check_vncRememberPassword="";



?>


<div id="border_box">
<div class="content_box_pop_up">
   <p class="textBlack">Application</p>
    <div class="white_nx_box_right">
     <table width="245" align="center">
      <tr>
       <td><span class="textBlack">Server 
        Please specify the name and display number of the remote VNC computer 
        connected to the NX server</span></td>
      </tr>
     </table>
     <table width="245" align="center">
      <tr>
       <td><span class="textBlack">Host</span></td><td width="90"><input name="VncHost"  id="VncHost" width="85" type="text" value="<?=$VncHostValue?>"></td>
       <td width="5">:</td>
       <td><input name="VncPort" id="VncPort" type="text" size="4" value="<?=$VncPortValue?>" ></td>
      </tr>
     </table>
    </div>
    <p class="textBlack" >Authentication</p>
    <div class="white_nx_box_right">
     <table width="245" align="center">
      <tr>
       <td><span class="textBlack">Please specify a password. NX will use it to login to the 
        remote VNC server.</span></td>
      </tr>
     </table>
     <table width="245" align="center" border="0">
      <tr>
       <td width="60">
        <span class="textBlack">Password</span>
       </td>
       <td  colspan="2">
        <input name="vncPassword" type="password" value="<?=$vncPasswordValue?>" >
       </td>
      </tr>
      <tr>
       <td width="60">&nbsp;</td>
       <td>
        <input name="vncRememberPassword" id="vncRememberPassword" type="checkbox" value="true" <?=$check_vncRememberPassword?> >
       </td>
       <td>
        <span class="textBlack">Remember my password</span>
       </td>
      </tr>      
     </table>
    </div>
    
    
 <table>
  <tr>
   <td>
    <img src="<?=$button?>/b_ok.png" onclick="javascript: document.getElementById('vncOption').style.display='none';  disableAction();" onmouseout="this.src='<?=$button?>/b_ok.png'" onmouseover="this.src='<?=$button?>/b_okOver.png'">
   </td>
   <td>
    <img src="<?=$button?>/b_cancel.png" onclick="javascript: document.getElementById('vncOption').style.display='none';  disableAction();" onmouseout="this.src='<?=$button?>/b_cancel.png'" onmouseover="this.src='<?=$button?>/b_cancelOver.png'">
   </td>
  </tr>
 </table>
</div></div>
