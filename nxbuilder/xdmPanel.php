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
?>


<div id="border_box">
<div class="content_box_pop_up">
   <p class="textBlack">Login</p>
    <div class="white_nx_box_right">
    <table width="245" align="center">
     <tr>
      <td><input name="xdmMode" id="xdmMode" value="server decide" <?=$check_xdmMode1?> type="radio" onclick="xdmPanel(this.value)"></td><td><span class="textBlack">Let NX server decide</span></td>
     </tr>
     <tr>
      <td></td><td><span class="textBlack">Use host and port mandated by the NX server administrator.</span></td>
     </tr>
     <tr>
      <td><input  name="xdmMode" id="xdmMode" value="query" <?=$check_xdmMode2?> type="radio" onclick="xdmPanel(this.value)"></td><td><span class="textBlack">Query an X desktop manager</span></td>
     </tr>
     <tr>
      <td></td>
      <td>
       <table>
        <tr><td><span class="textBlack">Host</span></td><td><input name="XdmDomainQuery" id="XdmDomainQuery" value="<?=$XdmDomainQueryValue?>" type="text" <?=$dis_XdmDomainQuery?> </td></tr> 
        <tr><td><span class="textBlack">Port</span></td><td><input name="XdmPortQuery" id="XdmPortQuery" value="<?=$XdmPortQueryValue?>" type="text" size="4" class="textBlack" <?=$dis_XdmPortQuery?>></tr></td>
       </table>
      </td>
     </tr>  
     <tr>
      <td></td><td><span class="textBlack">You can specify an XDM enabled host</span></td>
     </tr>
     <tr>
      <td><input  name="xdmMode" id="xdmMode" value="broadcast" <?=$check_xdmMode3?> type="radio" onclick="xdmPanel(this.value)"></td>
      <td>
        <span class="textBlack">Broadcast an XDM request</span><br />
      </td>
     </tr>
     <tr>
      <td></td>
      <td><span class="textBlack"> Port</span> <input name="XdmPortBroadCast" id="XdmPortBroadCast" value="<?=$XdmPortBroadCastValue?>" type="text" size="4" <?=$dis_XdmPortBroadCast?>></td>
     </tr>
     <tr>
      <td></td>
      <td><span class="textBlack">Run a session of the first available server</span></td>
     </tr>
     <tr>
      <td><input  name="xdmMode" id="xdmMode"  value="list" type="radio" <?=$check_xdmMode4?> onclick="xdmPanel(this.value)"></td><td><span class="textBlack">Get a list of available X desktop managers</span></td>
     </tr>
     <tr>
      <td></td>
      <td>
       <table>
        <tr><td><span class="textBlack">Host</span></td><td><input name="XdmDomainList" id="XdmDomainList" value="<?=$XdmDomainListValue?>" type="text" <?=$dis_XdmDomainList?>></td></tr> 
        <tr><td><span class="textBlack">Port</span></td><td><input name="XdmPortList" id="XdmPortList"  value="<?=$XdmPortListValue?>" type="text" size="4" class="textBlack" <?=$dis_XdmPortList?>></tr></td>
       </table>
      </td>
     </tr>
     <tr>
      <td></td>
      <td><span class="textBlack">Run an indirect query to specified host</span></td>
     </tr>
    </table>
   </div>

 <table>
  <tr>
   <td>
    <img src="<?=$button?>/b_ok.png" onclick="javascript: document.getElementById('xdmOption').style.display='none';  disableAction();" onmouseout="this.src='<?=$button?>/b_ok.png'" onmouseover="this.src='<?=$button?>/b_okOver.png'">
   </td>
   <td>
    <img src="<?=$button?>/b_cancel.png" value="Cancel" onclick="javascript: document.getElementById('xdmOption').style.display='none';  disableAction();" onmouseout="this.src='<?=$button?>/b_cancel.png'" onmouseover="this.src='<?=$button?>/b_cancelOver.png'">
   </td>
  </tr>
 </table>
     
</div></div>
