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


  $Message=array();


  //
  // Update managment.
  //

  $Message['CreatedDb']               ="DB created.";
  $Message['errorCreateDb']           ="DB not created.";
  $Message['UpdateDb']                ="DB updated.";    
  $Message['ErrorUpdateDb']           ="Cannot update DB.";
  $Message['WriteDumpConfirm']        ="Dump file created.";
  $Message['ErrorWriteDump']          ="Cannot write dump  file.";
  $Message['ErrorCreateDump']         ="Cannot create dump file.";    
  $Message['EmptyAdministrator']      ="Administrator DB field is empty."; 
  $Message['EmptyAdminPasswd']        ="Administrator DB password field is empty."; 

  $Message['EmptyHost']               ="Host name DB field is empty."; 
  $Message['EmptyDbName']             ="DB name field is empty."; 
    
  $Message['EmptyHostOld']            ="Host name existing DB field is empty."; 
  $Message['EmptyDbNameOld']          ="Existing DB name field is empty."; 
  $Message['EmptyAdministratorOld']   ="Administrator existing DB field is empty."; 
  $Message['EmptyAdminPasswdOld']     ="Administrator existing DB password field is empty."; 

  $Message['TablesNotFound']          ="No database tables found"; 
  $Message['WrongDbCredit']           ="Wrong DB credentials";
  
  if(!isset($_POST['dbName_old'])) $_POST['dbName_old']="";
   
  $Message['WrongDbName']             ="DB ".$_POST['dbName_old']." not found"; 

  $Message['WrongDbName']             ="DB ".$_POST['dbName_old']." not found";

  $Message['UpdateImageSuccessifull'] ="Icon update successful";

  $Message['UpdateImageFailed']       ="Icon update failed";



  //
  // Setup managment. 
  //
  $Message['createFile']              ="cannot create file 'builder.cfg' ";

  $Message['writeFile']               ="cannot write in 'builder.cfg'";

  //
  // Wrong authentication.
  //

  $errorLogin="Invalid login or password";
  $emptyLogin="Username field is empty";
  $emptyPassword="Password field is empty";
  $emptyLogPass="Username and Password fields are empty";
  $errorGuest='Please check "Login as Guest Builder user"';
  $Message['cfgNoFound']     ="No database connection has been found.";  

  //
  // Users managment.
  //

  $Message['ErrorUserUnknown']     ="One or more fields are empty.";  
  $Message['ErrorEmptyUsername']   ="Username field is empty.";
  $Message['ErrorEmptyPassword']   ="Password field is empty.";
  $Message['ErrorConfirmPassword'] ="Passwords do not match.";
  if(!isset($_POST['userName'])) $_POST['userName']="";
  $Message['AddedUser']            ="User ".$_POST['userName']." has been added.";
  $Message['ErrorAddUser']         ="Cannot insert user ".$_POST['userName'].". A user with the same name already exists.";
  $Message['AddedUser']            ="User ".$_POST['userName']." has been added.";  
  $Message['ErrorModifyNoAdmin']   ="The administrator account cannot be modified";
  $Message['ErrorModify']          ="Cannot update user ".$_POST['userName'].". Error in database.";
  $Message['ErrorDeleteAdmin']     ="The administrator account cannot be deleted.";  
  $Message['ErrorDelete']          ="Cannot delete user . Error in database.";  
  $Message['ConfirmDelete']        ="User ".$_POST['userName']." has been deleted.";


  //
  // Server management.
  //


  $Message['EmptyHostName']         ="Host field is empty.";  
  $Message['EmptyHostPort']         ="Port field is empty.";
  $Message['EmptyServerName']       ="Server name field is empty.";
  $Message['ErrorPort']             ="Invalid port: it is not a numeric value.";
  if(!isset($_POST['serverName'])) $_POST['serverName']="";
  $Message['AddedServer']           ="Server ".$_POST['serverName']." has been added.";
  $Message['UpdatedServer']         ="Server ".$_POST['serverName']." has been updated.";  
  $Message['ErrorServerUnknown']    ="Cannot insert server ".$_POST['serverName'];
  $Message['ErrorServerHostExist']  ="Cannot insert server ".$_POST['serverName'].". A server with the same host already exists.";
  $Message['ErrorServerNameExist']  ="Cannot insert server ".$_POST['serverName'].". A server with the same name already exists.";
  if(!isset($_POST['sessionName'])) $_POST['sessionName']="";
  $Message['UpdateSession']         ="Session ".$_POST['sessionName']." has been updated.";
  $Message['ErrorSessionNameExist'] ="Cannot insert server ".$_POST['sessionName'].". A session with the same name already exists.";
  $Message['EmptyReplaceTxt']       ="Replace text field is empty.";  

  if(!isset($_POST['sName'])) $_POST['sName']="";
  $Message['DeleteServer']          ="Server ".$_POST['sName']." has been deleted";
  $Message['ErrorDeleteServer']     ="Cannot delete server ".$_POST['sName'];  
  $Message['NoServerAvailable']     ="No servers available.";  

   
  //
  // Upload management. 
  //
   
  $Message['UploadedFile']        ="The image has been uploaded on the Web server.";
  //$Message['UploadedFileError']   ="This file is not an image format or you don't have permissions to access it.";
  $Message['UploadedFileError']   ="This file is not an image format or you don't have access rights."; 
  $Message['UploadEmptyField']    ="Category field is empty or no file selected."; 

  //
  // XDM management.
  //
 

  $Message['XDMEmptyDomain']    ="Host field is empty in XDM settings.";  
  $Message['XDMEmptyPort']      ="Port field is empty in XDM settings.";
  $Message['XDMPortFormat']     ="Invalid port: it is not a numeric value in XDM settings.";
  
  $Message['XDMInsertError']    ="Cannot insert XDM options.";
  $Message['XDMUpdateError']    ="Cannot update XDM options.";
  $Message['UpdateXdmDone']     ="XDM options have been updated.";
  
  //
  // Custom management.
  //
 

  $Message['CustomEmptyCommand']  ="Command field is empty in Custom settings.";  
  $Message['CustomInsertError']   ="Cannot insert Custom options.";
  $Message['CustomUpdateError']   ="Cannot update Custom options.";
  $Message['UpdateCustomDone']    ="Custom options have been updated.";
  
  //
  // Windows management.
  //
 


  $Message['WindowsEmptyServer']   ="Server field is empty in Windows settings.";  
  $Message['WindowsEmptyDomain']   ="Domain field is empty in Windows settings.";
  $Message['WindowsEmptyUserName'] ="Username field is empty in Windows settings.";
  $Message['WindowsEmptyPassword'] ="Password field is empty in Windows settings.";    
  $Message['RDPInsertError']       ="Cannot insert Windows options.";
  $Message['RDPUpdateError']       ="Cannot update Windows options.";
  $Message['UpdateWinDone']        ="Windows options have been updated.";

  
  //
  // VNC management.
  //

  $Message['VncEmptyServer']   ="Host field is empty in VNC settings.";
  $Message['VncEmptyPort']     ="Port field is empty in VNC settings.";
  $Message['VncPortFormat']    ="Invalid port: it is not a numeric value in VNC settings.";
  $Message['VncEmptyPassword'] ="Password field is empty in VNC settings.";
  $Message['VNCInsertError']   ="Cannot insert VNC options.";
  $Message['VNCUpdateError']   ="Cannot update VNC options.";  
  $Message['UpdateVncDone']    ="VNC options have been updated.";  
  

  //
  // Display Option management.
  //

  $Message['DisplayInsertError']   ="Cannot insert Display options.";
  $Message['DisplayUpdateError']   ="Cannot update Display options."; 
  $Message['DisplayUploadDone']    ="Display options have been updated.";

  //
  // HTTP Proxy management.
  //
    
  $Message['ProxyEmptyServer']   ="Host field is empty in HTTP Proxy settings.";
  $Message['ProxyEmptyPort']     ="Port field is empty in HTTP Proxy settings.";
  $Message['ProxyPortFormat']    ="Invalid port: it is not a numeric value in HTTP Proxy settings.";
  $Message['ProxyEmptyUserName'] ="Username field is empty in HTTP Proxy settings.";
  $Message['ProxyEmptyPassword'] ="Password field is empty in HTTP Proxy settings.";
  
  $Message['ProxyInsertError']   ="Cannot insert HTTP Proxy options.";
  $Message['ProxyUpdateError']   ="Cannot update HTTP Proxy options.";   
  $Message['UpdateProxyDone']    ="HTTP Proxy options have been updated."; 
  
  //
  // Font server management.
  //
    
  $Message['FontEmptyServer']   ="Host field is empty in Font server settings.";
  $Message['FontEmptyPort']     ="Port field is empty in Font server settings.";
  $Message['FontPortFormat']    ="Invalid port: it is not a numeric value in Font server settings.";

  $Message['FontInsertError']   ="Cannot insert Font Server options.";
  $Message['FontUpdateError']   ="Cannot update Font Server options.";    
  $Message['UpdateFontDone']    ="Font options have been updated.";  


  //
  // Session management.
  //
  $Message['EmptySessionName']      ="Session name field is empty.";  
  $Message['AddedSession']          ="Session ".$_POST['sessionName']." has been added.";
  $Message['UpdateSession']         ="Session ".$_POST['sessionName']." has been updated.";
  $Message['ErrorSessionUnknown']   ="Cannot insert session ".$_POST['sessionName'];
  $Message['ErrorSessionNameExist'] ="Cannot insert session ".$_POST['sessionName'].". A session with the same name already exists on this server.";



  $Message['SessionDeleted']        ="Session has been deleted.";
  $Message['ErrorSessionDelete']    ="Cannot delete session.";
  $Message['NoSessionAvailable']    ="No sessions available.";
  $Message['NoServerSelected']      ="No server selected.";    
 
 


?>
