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
ini_set("error_reporting", E_ERROR);
function authentication()
{
  if(isset($_SESSION['userName']) && isset($_SESSION['password']) && (isset($_SESSION['identity']) && $_SESSION['identity']=="administrator" ))
  {}
  else 
  {
    header('Location:nxbuilder.php');
    exit;
  }  
}

function checkPermission()
{
  if(!isset($_SESSION['userName'])){session_start();}
  
  if(isset($_SESSION['userName']) && isset($_SESSION['password']) && (isset($_SESSION['identity']) && $_SESSION['identity']=="administrator" ))
  {}
  elseif((basename($_SERVER['PHP_SELF'])=="ServerList.php") && isset($_SESSION['userName']) && isset($_SESSION['password']) )
  {} 
  else
  {
    header('Location:nxbuilder.php');
    exit;
  } 
}

function checkSmallPermission()
{
  if(!isset($_SESSION['userName'])){session_start();}
  
  if((basename($_SERVER['PHP_SELF'])=="ServerList.php") && isset($_SESSION['userName']) && isset($_SESSION['password'])  && $_SESSION['identity']!="administrator")
  {} 
  elseif((basename($_SERVER['PHP_SELF'])=="ServerList.php") && isset($_SESSION['userName']) && isset($_SESSION['password'])  && $_SESSION['identity']=="administrator")
  {
   header('Location: ServerListAdmin.php');
   exit;
  } 
  else
  {
   header('Location: nxbuilder.php');
   exit;  
  }
}

function checkAjaxPermission()
{
  if(!isset($_SESSION['userName'])){session_start();}
  
  if( isset($_SESSION['userName']) && isset($_SESSION['password'])  &&  isset($_SESSION['identity']))
  {} 
  else
  {
  // header('Location: nxbuilder.php');
   exit;  
  }
}


//
// if has been detectd a possible sql injection return true 
//
function detectInjection($value)
{
 $result=false;
 if(   (substr_count($value, "'")) 
    OR (substr_count(strtolower($value), "select")) 
    OR (substr_count(strtolower($value), "union"))
    OR (substr_count(strtolower($value), "concat"))
    OR (substr_count(strtolower($value), "insert"))
    OR (substr_count(strtolower($value), "delete"))
    OR (substr_count(strtolower($value), "join"))
    OR (substr_count(strtolower($value), "like"))
    OR (substr_count(strtolower($value), "update"))
    OR (substr_count(strtolower($value), ";"))
    OR (substr_count(strtolower($value), "%"))
    OR (substr_count(strtolower($value), "#"))
    OR (substr_count(strtolower($value), "/"))
    OR (substr_count(strtolower($value), "\\"))
    OR (substr_count(strtolower($value), "*"))
    OR (substr_count(strtolower($value), "<"))            
    OR (substr_count(strtolower($value), ">"))
    OR (substr_count(strtolower($value), "OR"))
   )
 {  
  $result=true;
 } 
return $result;
}

function logger($type, $message)
{
 $string=date("M d H:i:s")." ".$_SERVER['HTTP_HOST']." NXBUILDER SETUP 3.5: ".$type.": ".$message."\r\n";
	$fp=@fopen("tmp/setup.log","a+");
 @fwrite($fp,$string);
 @fclose($fp); 
} 

function checkForUpdate()
{
 include "builder.cfg";
 
 $fields = @mysql_list_fields($dbName, "servers", $dbconn);
 
 $cols = @mysql_num_fields($fields);
 $check=false;
 
 for ($i = 0; $i < $cols; $i++) 
 {
  if(@mysql_field_name($fields, $i)=="displayLink")
  {
    $check=true;
  }
 }

 if(!$check)
 {
  @mysql_query("ALTER TABLE `servers` ADD `displayLink` INT( 1 ) DEFAULT '0' NOT NULL  ");
 }


 $fields = @mysql_list_fields($dbName, "sessions", $dbconn);
 
 $cols = @mysql_num_fields($fields);
 
 $check=false;
 
 for ($i = 0; $i < $cols; $i++) 
 {
  if(@mysql_field_name($fields, $i)=="directDraw")
  {
    $check=true;
  }
 }

 if(!$check)
 {
  @mysql_query("ALTER TABLE `sessions` ADD `directDraw` VARCHAR( 5 ) DEFAULT 'false' NOT NULL AFTER `grapkeyboard` ");
 }
 
 
 $fields = @mysql_list_fields($dbName, "rdpServers", $dbconn);
 
 $cols = @mysql_num_fields($fields);
 
 $check=false;
 
 for ($i = 0; $i < $cols; $i++) 
 {
  if(@mysql_field_name($fields, $i)=="windowsType")
  {
    $check=true;
  }
 }

 if(!$check)
 {
  @mysql_query("ALTER TABLE `rdpServers` ADD `windowsType` VARCHAR( 10 ) DEFAULT 'false' NOT NULL AFTER `remember` ");
  @mysql_query("ALTER TABLE `rdpServers` ADD `applicationWindows` VARCHAR( 255 ) DEFAULT '' NOT NULL AFTER `windowsType` ");
 } 
 
}

function InsertServer($serverName,$hostName,$port,$OS,$CPU,$memory,$disk,$description,$icon,$displayMode,$replacingText,$defaultLink,$displayLink)
{
  include "builder.cfg";
  $query="SELECT * from servers WHERE hostName= '$hostName' OR serverName='$serverName'";
  $result = @mysql_query($query,$dbconn);
  $num = @mysql_num_rows ($result);
  if ($num>0)
  {
    @mysql_close($dbconn);
    $res=-1;
  }
  else
  {
    $passwordVNC= encodePassword($passwordVNC);
    $queryServer = "INSERT INTO servers ( `id` , `serverName` , `hostName` , `port` , `CPU` , `memory` , `disk` , `OS` , `description` , `icon` , `displayMode` , `replacingText`, `defaultLink` , `displayLink` ) VALUES
    ('','$serverName','$hostName','$port', '$CPU', '$memory', '$disk','$OS','$description','$icon','$displayMode','$replacingText', '$defaultLink', '$displayLink')";
    if (@mysql_query($queryServer, $dbconn))
    {
      @mysql_close($dbconn);
      $res=1;
    }
    else
    {
      @mysql_close($dbconn);
      $res=0;
    }
  }
return $res;  
}



function UpdateServer($servID, $serverName,$hostName,$port,$OS,$CPU,$memory,$disk, $description,$icon,$displayMode,$replacingText,$defaultLink,$displayLink)
{
  include "builder.cfg";
  
  $res=1;
  
  $queryHost="SELECT * from servers WHERE serverName= '$serverName' AND id<>'$servID' "; 
  
 
  $resultHost = @mysql_query($queryHost);
  $numHost = @mysql_num_rows($resultHost);

  
  if($numHost>0)
  {
    $res=-2; //server name exist
  }

  $queryServ="SELECT * from servers WHERE hostName='$hostName'  AND id<>'$servID'"; 
  
 
  $resultServ = @mysql_query($queryServ);
  $numServ=@mysql_num_rows($resultServ);
  
   
  if (($numServ>0)&&($res==1))
  {
    @mysql_close($dbconn);
    $res=-1; //host name exist
  }   

  
  if($res==1)
  {
    $queryServer = "UPDATE servers SET `serverName`='$serverName' , `hostName`='$hostName' , `port`='$port' , `CPU`='$CPU' , `memory`='$memory' , `disk`='$disk' , `OS`='$OS' , `description`='$description' , `icon`='$icon' , `displayMode`='$displayMode' , `replacingText`='$replacingText' ,`defaultLink`='$defaultLink',`displayLink`='$displayLink' WHERE  id='$servID' "; 
   
    if (@mysql_query($queryServer))
    {
      @mysql_close($dbconn);
    }
    else
    {
      @mysql_close($dbconn);
      $res=0;
    }
  }
return $res;
}

function deleteAllOptions($idSession)
{
  include ("builder.cfg");
 
  $deleteCross=@mysql_query("DELETE FROM xdmOptions      WHERE  idSession='".$idSession."' ");
  $deleteCross=@mysql_query("DELETE FROM vncServers      WHERE  idSession='".$idSession."' ");
  $deleteCross=@mysql_query("DELETE FROM rdpServers      WHERE  idSession='".$idSession."' ");
  $deleteCross=@mysql_query("DELETE FROM httpProxy       WHERE  idSession='".$idSession."' ");
  $deleteCross=@mysql_query("DELETE FROM customOptions   WHERE  idSession='".$idSession."' ");
  $deleteCross=@mysql_query("DELETE FROM displaySettings WHERE  idSession='".$idSession."' ");
  $deleteCross=@mysql_query("DELETE FROM fontServers     WHERE  idSession='".$idSession."' ");   
  
    
}

function deleteServer($idserver)
{
 include ("builder.cfg");
 
 $result=true;
 
 if(@mysql_query("DELETE FROM servers WHERE id='$idserver' limit 1 "))
 {
 
   $querySession="SELECT id FROM sessions WHERE idServer='$idserver' ";
   $execSession=@mysql_query($querySession);
   
     
   while($session=@mysql_fetch_array($execSession))
   {
    $deleteCross=@mysql_query("DELETE FROM crossUsersSessions  WHERE  sessionId='".$session['id']."' "); 
    $deleteSess =@mysql_query("DELETE FROM sessions  WHERE  id='".$session['id']."' ");
    deleteAllOptions($session['id']);
    if((!$deleteCross) || (!$deleteSess))
    {
     $result=false;
    }
   }
 }
 else
 {
  $result=false; 
 }

return $result;

}





function InsertSession($sessionName,$deskType,$sessionType,$displayCustom, $disTcp, $disZlib, $ssl, $grap,$directDraw ,$lazyEncoder ,$enableProxy, $enableMultimedia, $useFontServer ,$icon, $serverId)
{
  include "builder.cfg";

	 $result=1; //session insert successifull                                       
  $queryName="SELECT * from sessions WHERE idServer='$serverId' AND sessionName='$sessionName' ";
  $executeQuery=@mysql_query($queryName);
  $numSession=@mysql_num_rows($executeQuery);

  if($numSession>=1)
  {
   $result=-1; //session exist
  }
  else
  { 
	                                         
   $querySession = "INSERT INTO sessions ( `id` , `sessionName` , `desktopType` , `sessionType`, `displayCustom`, `disableTcp`, `disableZlib` , `ssl`, `grapkeyboard`,`directDraw`, `lazyEncoder`, `enableHttpProxy` , `enableMultimedia`, `useFontServer`, `icon` , `idServer`) VALUES
   ('','$sessionName','$deskType','$sessionType','$displayCustom', '$disTcp' , '$disZlib', '$ssl', '$grap','$directDraw','$lazyEncoder', '$enableProxy' ,'$enableMultimedia', '$useFontServer', '$icon', '$serverId')";
   
   $result=0;
     
   if (@mysql_query($querySession, $dbconn))
   {
   
     $result=@mysql_insert_id();
     
     @mysql_close($dbconn);
   }
  }
return $result;

}

function UpdateSession($idSession, $sessionName,$deskType,$sessionType,$displayCustom, $disTcp, $disZlib, $ssl, $grap, $directDraw, $lazyEncoder ,$enableProxy, $enableMultimedia, $useFontServer ,$icon, $serverId)
{
  include "builder.cfg";
	 $res=1; //session insert successifull                                       
  $queryName="SELECT * from sessions WHERE id<>'$idSession' AND idServer='$serverId' AND sessionName='$sessionName' ";
  $executeQuery=@mysql_query($queryName);
  $numSession=@mysql_num_rows($executeQuery);
  
  if($numSession>=1)
  {
   $res=-1; //session exist
  }
  else
  { 
   $querySession = "UPDATE sessions SET `sessionName`='$sessionName' , `desktopType`='$deskType' , `sessionType`='$sessionType', `displayCustom`='$displayCustom', `disableTcp`='$disTcp', `disableZlib`='$disZlib' , `ssl`='$ssl', `grapkeyboard`='$grap', `directDraw`='$directDraw' , `lazyEncoder`='$lazyEncoder', `enableHttpProxy`='$enableProxy' , `enableMultimedia`='$enableMultimedia', `useFontServer`='$useFontServer', `icon`='$icon' , `idServer`='$serverId' WHERE id='$idSession'";
   $up=@mysql_query($querySession);
   $num=mysql_affected_rows();
    
   if(!$up)
   { 
     $res=-2; //session update error
     @mysql_close($dbconn);
   }
  }

return $res;
}

function InsertXdmOption($idSess, $xdmMode, $XdmPortList, $XdmDomainList, $XdmPortQuery, $XdmDomainQuery, $XdmPortBroadCast)
{
  include "builder.cfg";
  $queryXdm = "INSERT INTO xdmOptions  ( `id` , `idSession` , `mode` , `listHost`, `listPort`, `broadcastPort`, `queryHost`, `queryPort`) VALUES
  ('','$idSess','$xdmMode','$XdmDomainList','$XdmPortList', '$XdmPortBroadCast' , '$XdmDomainQuery', '$XdmPortQuery')";
 
  $result=false;
    
  if(@mysql_query($queryXdm))
  {
     
    $result=true; 
    @mysql_close($dbconn);
  }

return $result;  
  	                                        
}

function UpdateXdmOption($idSess, $xdmMode, $XdmPortList, $XdmDomainList, $XdmPortQuery, $XdmDomainQuery, $XdmPortBroadCast)
{
  include "builder.cfg";
  $queryXdm = "UPDATE xdmOptions SET `mode`='$xdmMode' , `listHost`='$XdmDomainList', `listPort`='$XdmPortList', `broadcastPort`='$XdmPortBroadCast', `queryHost`='$XdmDomainQuery', `queryPort`='$XdmPortQuery' WHERE idSession='$idSess'";
  $execute=@mysql_query($queryXdm);
  $update=@mysql_affected_rows(); 
  $result=false;
    
  $query="Select * FROM `xdmOptions` WHERE idSession='$idSess' ";
  $execQuery=@mysql_query($query);
  $exist=mysql_num_rows($execQuery);
    
  if($exist>0)
  {
    $result=true; 
    @mysql_close($dbconn);
  }
 
  if(!$execute)
  {
   $result=false;
  } 
  
return $result;  
  	                                        
}

function InsertCustomOption($idSess, $runApplication,$customCommand,$customOptions,$disableX, $taint)
{
  include "builder.cfg";
  
  $queryCust = "INSERT INTO customOptions  ( `id` , `idSession` , `typeDesk` , `custCommand`, `virtualDesktop`, `xAgentEncoding`, `useTaint`) VALUES
  ('','$idSess','$runApplication','$customCommand','$customOptions', '$disableX' , '$taint')";
  

  
  $result=false;
    
  if (@mysql_query($queryCust, $dbconn))
  {
     
    $result=true; 
    @mysql_close($dbconn);
  }

return $result;  
                                     
}

function UpdateCustomOption($idSess, $runApplication,$customCommand,$customOptions,$disableX, $taint)
{
  include "builder.cfg";
  
  $queryCust = "UPDATE customOptions SET `typeDesk`='$runApplication' , `custCommand`='$customCommand', `virtualDesktop`='$customOptions', `xAgentEncoding`='$disableX', `useTaint`='$taint' WHERE idSession='$idSess'";
  $execute=@mysql_query($queryCust);
  $update=@mysql_affected_rows();
  
  $query="Select * FROM `customOptions` WHERE idSession='$idSess' ";
  $execQuery=@mysql_query($query);
  $exist=mysql_num_rows($execQuery);
    
  if($exist>0)
  {
    $result=true; 
    @mysql_close($dbconn);
  }
 
  if(!$execute)
  {
   $result=false;
  } 

return $result;  
                                     
}
function InsertRdpOption($idSess, $windowServer, $windowDomain, $rdpCredential, $usernameWindows, $passwordWindows, $remember, $windType, $applWind)
{
  include "builder.cfg";
   
  $passwordWindows=encodePassword($passwordWindows); 
	                                        
  $queryRdp = "INSERT INTO rdpServers ( `id` , `idSession` , `server` , `domain`, `authentication`, `username`, `password` , `remember`, `windowsType` , `applicationWindows`) VALUES
  ('','$idSess','$windowServer','$windowDomain','$rdpCredential', '$usernameWindows' , '$passwordWindows', '$remember', '$windType', '$applWind')";
  
 
  $result=false;
    
  if (@mysql_query($queryRdp, $dbconn))
  {
     
    $result=true; 
    @mysql_close($dbconn);
  }

return $result;
}


function UpdateRdpOption($idSess, $windowServer, $windowDomain, $rdpCredential, $usernameWindows, $passowordWindows, $remember, $windType, $applWind)
{
  include "builder.cfg";

  $queryCheck="SELECT `password` from `rdpServers` WHERE `idSession`='$idSess' ";
  $executeQueryCheck=@mysql_query($queryCheck);
  $checkPwd=mysql_fetch_array($executeQueryCheck);
  
  if($checkPwd['password']!=$passowordWindows)
  {
   $passowordWindows=encodePassword($passowordWindows);
  }
  	                                        
  $queryRdp = "UPDATE `rdpServers` SET `server`='$windowServer',`domain`='$windowDomain',`authentication`='$rdpCredential',`username`='$usernameWindows',`password`='$passowordWindows',`remember`='$remember', `windowsType`='$windType', `applicationWindows`='$applWind' WHERE `idSession`='$idSess' ";
  $execute=@mysql_query($queryRdp);
  $update=@mysql_affected_rows();
  $result=false;

    
  $query="Select * FROM `rdpServers` WHERE idSession='$idSess' ";
  $execQuery=@mysql_query($query);
  $exist=mysql_num_rows($execQuery);
    
  if($exist>0)
  {
    $result=true; 
    @mysql_close($dbconn);
  }
 
  if(!$execute)
  {
   $result=false;
  } 

return $result;

}



function InsertVncOption($idSess, $vncHost, $vncPort, $vncPassword, $vncRememberPassword)
{
  include "builder.cfg";
  
  $vncPassword=encodePassword($vncPassword);
  $queryVnc = "INSERT INTO vncServers ( `id` , `idSession` , `host` , `port`, `password` , `remember`) VALUES
  ('','$idSess','$vncHost','$vncPort','$vncPassword', '$vncRememberPassword' )"; 

  $result=false;
    
  if (@mysql_query($queryVnc))
  {
     
    $result=true; 
    @mysql_close($dbconn);
  }

return $result;	                                        
}


function UpdateVncOption($idSess, $vncHost, $vncPort, $vncPassword, $vncRememberPassword)
{
  include "builder.cfg";
   
  $queryCheck="SELECT `password` from vncServers WHERE idSession='$idSess' ";
  $executeQueryCheck=@mysql_query($queryCheck);
  $checkPwd=mysql_fetch_array($executeQueryCheck);
  
  if($checkPwd['password']!=$vncPassword)
  {
   $vncPassword=encodePassword($vncPassword);
  }
   
  $queryVnc = "UPDATE vncServers SET `host`='$vncHost' , `port`='$vncPort', `password`='$vncPassword' , `remember`='$vncRememberPassword' WHERE idSession='$idSess' ";
  $execute=@mysql_query($queryVnc);
  $update=@mysql_affected_rows();  
  $result=false;
    
  $query="Select * FROM `vncServers` WHERE idSession='$idSess' ";
  $execQuery=@mysql_query($query);
  $exist=mysql_num_rows($execQuery);
    
  if($exist>0)
  {
    $result=true; 
    @mysql_close($dbconn);
  }
 
  if(!$execute)
  {
   $result=false;
  } 

return $result;	                                        
}


function InsertDisplayOption($idSess, $renderExt, $backingStore, $composite,  $compressionType, $vncComp,  $jpegQuality,  $winComp, $shm, $sharedPix, $rdpQuality, $rdpCache, $jpegQualityRdp, $jpegQualityVnc)
{
  include "builder.cfg";
 
                                                     	   
  $queryDisplay="INSERT INTO `displaySettings` ( `id` , `idSession` , `renederExt` , `backingStore` , `composite` , `compType` , `encodType` , `jpegQuality` , `winComp` , `shm` , `sharedPix` , `winColorDepth` , `rdpCache`,  `rdpJpgQuality`, `vncJpg` )
                 VALUES ('', '$idSess', '$renderExt', '$backingStore', '$composite', '$compressionType', '$vncComp', '$jpegQuality', '$winComp', '$shm', '$sharedPix', '$rdpQuality', '$rdpCache', '$jpegQualityRdp', '$jpegQualityVnc')"; 
   
  
  $result=false;
    
  if (@mysql_query($queryDisplay, $dbconn))
  {
     
    $result=true; 
    @mysql_close($dbconn);
  }

return $result;	                                        
}

function UpdateDisplayOption($idSess, $renderExt, $backingStore, $composite,  $compressionType, $encodType,  $jpegQuality,  $winComp, $shm, $sharedPix, $rdpQuality, $rdpCache, $jpegQualityRdp, $jpegQualityVnc)
{
  include "builder.cfg";
   
  $queryDisplay="UPDATE `displaySettings` SET `renederExt`='$renderExt' , `backingStore`='$backingStore' , `composite`='$composite' , `compType`='$compressionType' , `encodType`='$encodType' , `jpegQuality`='$jpegQuality' , `winComp`='$winComp' , `shm`='$shm' , `sharedPix`='$sharedPix' , `winColorDepth`='$rdpQuality' , `rdpCache`='$rdpCache',   `rdpJpgQuality`='$jpegQualityRdp', `vncJpg`='$jpegQualityVnc' WHERE idSession='$idSess'";
  $execute=@mysql_query($queryDisplay);
  $update=@mysql_affected_rows();
  $result=false;

  $query="Select * FROM `displaySettings` WHERE idSession='$idSess' ";
  $execQuery=@mysql_query($query);
  $exist=mysql_num_rows($execQuery);
    
  if($exist>0)
  {
    $result=true; 
    @mysql_close($dbconn);
  }
 
  if(!$execute)
  {
   $result=false;
  } 
  
return $result;	                                        
}



function insertHttpProxy($idSess, $proxyHost, $proxyPort, $userNameProxy, $passwordNameProxy, $rememberProxyAuth)
{
  include "builder.cfg";
  $pass=encodePassword($passwordNameProxy);
  $result=false;
  
  $queryProxy = "INSERT INTO httpProxy ( `id` , `idSession` , `host` , `port` , `username` , `password` , `remember`) VALUES
    ('','$idSess','$proxyHost','$proxyPort','$userNameProxy', '$pass', '$rememberProxyAuth')";
    if (@mysql_query($queryProxy, $dbconn))
    {
      $result=true;  
    }  
  	                                        
return $result;
}

function UpdateHttpProxy($idSess, $proxyHost, $proxyPort, $userNameProxy, $passwordNameProxy, $rememberProxyAuth)
{
  include "builder.cfg";
  
  $queryCheck="SELECT `password` from httpProxy WHERE idSession='$idSess' ";
  $executeQueryCheck=@mysql_query($queryCheck);
  $checkPwd=mysql_fetch_array($executeQueryCheck);
  
  if($checkPwd['password']!=$passwordNameProxy)
  {
   $passwordNameProxy=encodePassword($passwordNameProxy);
  }

  $result=false;
  
  $queryProxy = "UPDATE httpProxy SET `host`='$proxyHost' , `port`='$proxyPort' , `username`='$userNameProxy' , `password`='$passwordNameProxy' , `remember`='$rememberProxyAuth' WHERE idSession='$idSess'";
  $execute=@mysql_query($queryProxy);
  $update=@mysql_affected_rows();
  
  $query="Select * FROM `httpProxy` WHERE idSession='$idSess' ";
  $execQuery=@mysql_query($query);
  $exist=mysql_num_rows($execQuery);
    
  if($exist>0)
  {
    $result=true; 
    @mysql_close($dbconn);
  }
 
  if(!$execute)
  {
   $result=false;
  }   
  	                                        
return $result;
}


function insertFontServer($idSess, $fontServerHost, $fontServerPort)
{
  include "builder.cfg";
  
  $result=false;
  
  $queryFont = "INSERT INTO fontServers ( `id` , `idSession` , `host` , `port`) VALUES
    ('','$idSess','$fontServerHost','$fontServerPort')";
    if (@mysql_query($queryFont, $dbconn))
    {
      $result=true;  
    }  
  	                                        
return $result;
}

function UpdateFontServer($idSess, $fontServerHost, $fontServerPort)
{
  include "builder.cfg";
  
  $result=false;
  
  $queryFont = "UPDATE fontServers SET `host`='$fontServerHost' , `port`='$fontServerPort' WHERE idSession='$idSess'";
  $execute=@mysql_query($queryFont);
  $update=@mysql_affected_rows();
  
  $query="Select * FROM `fontServers` WHERE idSession='$idSess' ";
  $execQuery=@mysql_query($query);
  $exist=mysql_num_rows($execQuery);
    
  if($exist>0)
  {
    $result=true; 
    @mysql_close($dbconn);
  }
 
  if(!$execute)
  {
   $result=false;
  }  
 	                                        
return $result;
}




function InsertUserSession($userName,$password,$listSession)
{
  include "builder.cfg";
  
  $query="SELECT id from users WHERE userName ='$userName' AND password='$password' ";
  
  $result=@mysql_query($query,$dbconn);
  $row = @mysql_fetch_array ($result);
 
  foreach($listSession as $sessionId)
  {
   $insert = "INSERT INTO crossUsersSessions(sessionId,userId) VALUES (".$sessionId.",".$row['id'].")";
   $execute=@mysql_query($insert, $dbconn);
  }
   
  @mysql_close($dbconn);
 
 return true;    
}

function ModifyUserSession($userId,$listSession)
{
  include "builder.cfg";
  
  
  $delete="DELETE FROM crossUsersSessions WHERE userId='$userId' ";
  $del=@mysql_query($delete);
  
  foreach($listSession as $sessionId)
  {
   $insert = "INSERT INTO crossUsersSessions(sessionId,userId) VALUES (".$sessionId.",".$userId.")";
   $execute=@mysql_query($insert, $dbconn);
  }
   
  @mysql_close($dbconn);
 
 return true;    
}


function undoInsertSession($idSess)
{
  include "builder.cfg";
  
  $query[0]="DELETE FROM sessions WHERE id='$idSess' ";
  $query[1]="DELETE FROM customOptions WHERE idSession='$idSess' ";
  $query[2]="DELETE FROM displaySettings WHERE idSession='$idSess' ";
  $query[3]="DELETE FROM fontServers WHERE idSession='$idSess' ";
  $query[4]="DELETE FROM httpProxy WHERE idSession='$idSess' ";
  $query[5]="DELETE FROM rdpServers WHERE idSession='$idSess' ";
  $query[6]="DELETE FROM vncServers WHERE idSession='$idSess' ";
  $query[7]="DELETE FROM httpProxy WHERE idSession='$idSess' ";
  $query[8]="DELETE FROM xdmOptions WHERE idSession='$idSess' ";
  
  for($i=0; $i<count($query); $i++)
  {
   @mysql_query($query[$i]);
  }
      
}


function InsertUserList($userName,$password,$userType,$credentialsNX)
{
  include "builder.cfg";

  $query="SELECT * from users WHERE userName ='$userName' ";
  
  $result=@mysql_query($query,$dbconn);
  $row = @mysql_fetch_array ($result);

  if($row)
  {
    @mysql_close($dbconn);
    $result=false;
  }
  else
  {
    
    
    $query = "INSERT INTO users (id ,userName, password, identity, credentialsNX) VALUES
       ('','$userName', '$password', '$userType', '$credentialsNX' )";
   
    if(@mysql_query($query))
    {
      @mysql_close($dbconn);
      $result=true;
    }
  }
return $result;
}


function UpdateUserList($idUser, $userName,$password,$userType,$credentialsNX)
{
  include "builder.cfg";

 
  // $result
  // -1: exist only administrator  
  //  0: update fail
  //  1: update successiful
   
 
  $checkUserQuery="SELECT id,identity FROM users WHERE id = '$idUser'";
  $res = @mysql_query($checkUserQuery,$dbconn);
  $row = @mysql_fetch_array($res);
  $result=0;
 
  if(($row['identity']=="administrator")&&($userType!="administrator"))
  {
   $queryAdmin="SELECT * FROM users WHERE identity = 'administrator'";
   $adm = @mysql_query($queryAdmin,$dbconn);
   $num = @mysql_num_rows($adm);
      
   if($num==1)
   {
    $result=-1;
   }
  }


  
  
  if($result!=-1)
  { 
   $updatePass="";
   if($password!="****default****")
   {
    $updatePass=" , password='".md5($password)."' ";
   }
   
   $query = "UPDATE users SET userName='$userName', identity='$userType' , credentialsNX='$credentialsNX' $updatePass WHERE id='$idUser' LIMIT 1 ";
   
   if(@mysql_query($query))
   {
     @mysql_close($dbconn);
     $result=1;
   }
  }
  
return $result;
}


function userListById($id)
{
  include "builder.cfg";

  $query="SELECT * from users WHERE id='$id' ";
  
  $queryUser=@mysql_query($query,$dbconn);
 
  if(!$queryUser)
  {
   $queryUser=-1;
  }
 return $queryUser; 
}  


function userListAll()
{
  include "builder.cfg";

  $query="SELECT * from users ORDER BY identity ";
  
  $queryUser=@mysql_query($query,$dbconn);
 
  if(!$queryUser)
  {
   $queryUser=-1;
  }
 return $queryUser; 
}  

function sessionListUser($idUser, $idServer)
{
  include "builder.cfg"; 

  $query="SELECT crossUsersSessions.* , sessions.*  FROM crossUsersSessions, sessions WHERE crossUsersSessions.userId = '$idUser' AND crossUsersSessions.sessionId = sessions.id AND sessions.idServer='$idServer'  ";
    
  
 
  $queryUser=@mysql_query($query,$dbconn);
 
  if(!$queryUser)
  {
   $queryUser=-1;
  }
 return $queryUser; 
} 





function deleteUser($userId)
{
  //delete function return: -1 if there is only 1 administrator
  //                         0 if query has failed   
  //                         1 if query ok


  include("builder.cfg");
  
  $queryUser="SELECT id,identity FROM users WHERE id = '$userId'";
  $res = @mysql_query($queryUser,$dbconn);
  $row = @mysql_fetch_array($res);
  
  $result=1;
  
  if($row['identity']=="administrator")
  {
   $queryAdmin="SELECT * FROM users WHERE identity = 'administrator'";
   $adm = @mysql_query($queryAdmin,$dbconn);
   $num = @mysql_num_rows($adm);
      
   if($num==1)
   {
    $result=-1;
   }
  } 
  
  if($result!=-1)
  {
    $queryDel="DELETE FROM crossUsersSessions WHERE userId = '".$row['id']."'";
        
    @mysql_query($queryDel,$dbconn);
   
    $query= "DELETE from users WHERE id = '".$row['id']."'" ;
    if(@mysql_query($query, $dbconn)) $result=1;
    else $result=0;
  }

return $result;
}



function encodePassword($p)
{
  $sPass = ":";
  $sTmp = "";
  if (empty($p))
  {
    return "";
  }
  for ($i = 0; $i < strlen($p); $i++)
  {
    $c = substr($p,$i,1);
    $a=ord($c);
    $sTmp=(integer)($a+$i+1).":";
    $sPass .=$sTmp;
    $sTmp = "";
  }
  return $sPass;
}

//
// Guest User management.
//

function getServer($idServer="")
{
 include ("builder.cfg");

 $where="";
 if($idServer!="")
 {
   $where=" WHERE id='".$idServer."' ";
 }


 $queryStringHost="Select * from servers ".$where;
 
 $queryHost = @mysql_query($queryStringHost);
 
 if(!$queryHost)
 {
  $queryHost=-1;
 }
 return $queryHost; 
}  

function getServerByName($serverName)
{
 include ("builder.cfg");
 
 $res=false;
 
 $queryString="Select * from servers where serverName='$serverName' ";
  
 $query = @mysql_query($queryString);
 $num=@mysql_num_rows($query);
 
 if($num>0)
 {
  $res=true;
 }
 return $res; 
}  

function getServerByHost($hostName)
{
 include ("builder.cfg");

 $res=false;
 
 $queryString="Select * from servers where hostName='$hostName' ";

 $query = @mysql_query($queryString);
 $num=@mysql_num_rows($query);
 
 
 if($num>0)
 {
  $res=true;
 }
 return $res; 
}  

function getListServerForUser($idUser)
{
 include ("builder.cfg");

 $serverList=array();
   
 $queryString="SELECT * FROM `crossUsersSessions`  WHERE  userId='$idUser' ";

 $query = @mysql_query($queryString);
 
 while($crossList=@mysql_fetch_array($query))
 {
  $querySession="SELECT * FROM `sessions`  WHERE  id='".$crossList['sessionId']."' ";
  $execSessionQuery=@mysql_query($querySession);
  $session=@mysql_fetch_array($execSessionQuery);
  if(!in_array($session['idServer'],$serverList )) 
  {
   $serverList[]=$session['idServer'];
  } 
 }
 
 
 return $serverList; 
}


function getGuestId()
{
 include ("builder.cfg");
 
 $queryGuest="SELECT id FROM users where  identity='guest' ";
 $execQueryGuest=@mysql_query($queryGuest);
 
 $guest=@mysql_fetch_array($execQueryGuest);
 
 return $guest['id'];
}

function getGuestCredentials()
{
 include ("builder.cfg");
 
 $queryGuest="SELECT  credentialsNX FROM users where  identity='guest' ";
 $execQueryGuest=@mysql_query($queryGuest);
 
 $guest=@mysql_fetch_array($execQueryGuest);
 
 return $guest['credentialsNX'];
}

function getSessionListForGuest()
{
 include ("builder.cfg");
 
 $idGuest=getGuestId();
 
 $queryStringServer="SELECT servers.*  FROM servers, crossUsersSessions, users, sessions WHERE users.identity =  'guest' AND crossUsersSessions.userId = users.id AND sessions.idServer = servers.id AND sessions.id = crossUsersSessions.sessionId ";
 
 
 
 $queryServer = @mysql_query($queryStringServer); 

 if(!$queryServer)
 {
  $queryServer=-1;
 }

 return $queryServer;
}


function getSessionsForGuestDesk($idServer)
{
 include ("builder.cfg");
 
 $idGuest=getGuestId();
                      
 $queryStringSession="SELECT sessions. *  FROM sessions, crossUsersSessions WHERE crossUsersSessions.userId =  '$idGuest' AND sessions.idServer =  '$idServer' AND sessions.sessionType <> 'custom' AND sessions.id = crossUsersSessions.sessionId ";
 $querySession = @mysql_query($queryStringSession);
 
 if(!$querySession)
 {
  $querySession=-1;
 }
 return $querySession; 
}  

function getSessionByID($idSess)
{
 
 include ("builder.cfg");
 
 $query="SELECT * FROM sessions WHERE id='$idSess'";
 $queryDb=@mysql_query($query);
 
 $arraySession=@mysql_fetch_array($queryDb);
 

 if(!$arraySession)
 {
  $arraySession = -1;
 }
 return $arraySession; 
}  

function getHttpProxyByID($idSess)
{
 
 include ("builder.cfg");
 
 $query="SELECT * FROM httpProxy WHERE idSession='$idSess'";

 $queryDb=@mysql_query($query);
 
 $arrayHttpProxy=@mysql_fetch_array($queryDb);
 
 if(!$arrayHttpProxy)
 {
  $arrayHttpProxy = -1;
 }
 return $arrayHttpProxy; 
}

function getXdmOptionByID($idSess)
{

 include ("builder.cfg");
 
 $query="SELECT * FROM xdmOptions WHERE idSession='$idSess'";
 $queryDb=@mysql_query($query);
 
 $arrayXdmOptions=@mysql_fetch_array($queryDb);
 
 if(!$arrayXdmOptions)
 {
  $arrayXdmOptions = -1;
 }
 return $arrayXdmOptions; 
}

function getWinOptionByID($idSess)
{

 include ("builder.cfg");
 
 $query="SELECT * FROM rdpServers WHERE idSession='$idSess'";
 $queryDb=@mysql_query($query);
 
 $arrayWinOption=@mysql_fetch_array($queryDb);
 
 if(!$arrayWinOption)
 {
  $arrayWinOption = -1;
 }
 return $arrayWinOption; 
}

function getDisplayByID($idSess)
{

 include ("builder.cfg");
 
 $query="SELECT * FROM displaySettings WHERE idSession='$idSess'";
 $queryDb=@mysql_query($query);
 
 $arrayDisplay=@mysql_fetch_array($queryDb);
 
 if(!$arrayDisplay)
 {
  $arrayDisplay = -1;
 }
 return $arrayDisplay; 
}

function getVncOptionByID($idSess)
{

 include ("builder.cfg");
 
 $query="SELECT * FROM vncServers WHERE idSession='$idSess'";
 $queryDb=@mysql_query($query);
 
 $arrayVncOption=@mysql_fetch_array($queryDb);
 
 if(!$arrayVncOption)
 {
  $arrayVncOption = -1;
 }
 return $arrayVncOption; 
}

function getCustomOptionByID($idSess)
{

 include ("builder.cfg");
 
 $query="SELECT * FROM customOptions WHERE idSession='$idSess'";
 $queryDb=@mysql_query($query);
 
 $arrayCustomOption=@mysql_fetch_array($queryDb);
 
 if(!$arrayCustomOption)
 {
  $arrayCustomOption = -1;
 }
 return $arrayCustomOption; 
}



function getFontOptionByID($idSess)
{
 include ("builder.cfg");
 
 $query="SELECT * FROM fontServers WHERE idSession='$idSess'";
 $queryDb=@mysql_query($query);
 
 $arrayFontOption=@mysql_fetch_array($queryDb);
 
 if(!$arrayFontOption)
 {
  $arrayFontOption = -1;
 }
 return $arrayFontOption; 
}



function getSessionsForGuestApp($idServer)
{
 include ("builder.cfg");
 
 $idGuest=getGuestId();
 
 $queryStringSession="SELECT sessions. *  FROM sessions, crossUsersSessions WHERE crossUsersSessions.userId =  '$idGuest' AND sessions.idServer =  '$idServer' AND sessions.sessionType = 'custom' AND sessions.id = crossUsersSessions.sessionId ";
 $querySession = @mysql_query($queryStringSession);
 
 if(!$querySession)
 {
  $querySession=-1;
 }
 return $querySession; 
} 

//
//this function get a list of availables sessions from a server
//
function getSessionsDeskFromServer($idServer)
{
 include ("builder.cfg");
 $queryStringSession="SELECT * FROM sessions WHERE idServer='$idServer' AND  sessionType<>'custom' ";
 $querySession = @mysql_query($queryStringSession);
 
 if(!$querySession)
 {
  $querySession=-1;
 }
 return $querySession; 
}  



function getSessionsListForUser($idServer, $idUser, $type)
{
 include ("builder.cfg");
 $queryStringSession="SELECT * FROM sessions, crossUsersSessions WHERE sessions.idServer='$idServer' AND  sessions.sessionType$type'custom' AND crossUsersSessions.userId='$idUser'  AND crossUsersSessions.sessionId =sessions.id ";
 
 $querySession = @mysql_query($queryStringSession);
 
 if(!$querySession)
 {
  $querySession=-1;
 }
 return $querySession; 
}  


function getSessionsAppFromServer($idServer)
{
 include ("builder.cfg");
 $queryStringSession="Select * from sessions where idServer='$idServer' AND  sessionType='custom' ";
 $querySession = @mysql_query($queryStringSession);
 
 if(!$querySession)
 {
  $querySession=-1;
 }
 return $querySession; 
}  


function getSessionsListAll()
{
 include ("builder.cfg");
 $queryStringSession="SELECT * FROM  sessions ";          
 $querySession = @mysql_query($queryStringSession);
 
 if(!$querySession)
 {
  $querySession=-1;
 }
 return $querySession; 
} 


function getSessionsList($idServer)
{
 include ("builder.cfg");
 $queryStringSession="SELECT * FROM  sessions  WHERE idServer = '$idServer' ";       
 $querySession = @mysql_query($queryStringSession);
 
 if(!$querySession)
 {
  $querySession=-1;
 }
 return $querySession; 
} 


function getSessionsByUser($idUser, $idServer)
{
 include ("builder.cfg");
 $queryStringSession="SELECT sessions.id, sessions.sessionName FROM sessions, crossUsersSessions WHERE 
                      crossUsersSessions.userId='$idUser' AND sessions.id=crossUsersSessions.sessionId AND 
                      sessions.idServer='$idServer'";
 
 
         
 $querySession = @mysql_query($queryStringSession);
 
 if(!$querySession)
 {
  $querySession=-1;
 }
 return $querySession; 
} 

function getSessionChoose($idUser, $idServer)
{
 
 $session="";
 $listSession=getSessionsByUser($idUser, $idServer);
 
 while($list=@mysql_fetch_array($listSession))
 {
  $session.=$list['id'].",";
 }
 
return $session; 

}

function getSessionsNotAssigned($idServer, $resultList)
{
 include ("builder.cfg");
 
 $where="";
 
 while($list=@mysql_fetch_array($resultList))
 {
  $where.=" AND id<> ".$list['id']." ";
 }
 
 
 
 $queryStringSession="SELECT sessions.id, sessions.sessionName FROM sessions WHERE idServer='$idServer' $where "; 
                                                 
 $querySession = @mysql_query($queryStringSession);
 
 if(!$querySession)
 {
  $querySession=-1;
 }
 return $querySession; 
} 

function getSessionsListFromServer()
{
 include ("builder.cfg");
 $queryStringSession="SELECT  sess.id AS id,  sess.sessionName, sess.desktopType, sess.sessionType, sess.displayCustom, sess.ssl, sess.icon ,	sess.idServer, serv.serverName, serv.hostName  FROM  sessions AS sess , servers AS serv WHERE sess.idServer = serv.id ";         

 $querySession = @mysql_query($queryStringSession);


 
 if(!$querySession)
 {
  $querySession=-1;
 }
 return $querySession; 
} 

function getFileName($idSession)
{
  include ("builder.cfg");
 
  $queryData=@mysql_query("SELECT  sessions.sessionName, sessions.sessionType, servers.hostName FROM sessions, servers  WHERE sessions.idServer=servers.id  AND sessions.id='$idSession' ");
 
  
  if($session=@mysql_fetch_array($queryData))
  {  
   $sessionName = str_replace(' ','', $session['sessionName']);
   $serName     = str_replace(' ','', $session['hostName']);
  
 
   $fileName=strtoupper($sessionName)."@".$serName.".nxs";
  } 
  else
  {
   $fileName="default.nxs"; 
  }

 return $fileName;
}


function deleteSession($idSession)
{
 include ("builder.cfg");
 
 if(@mysql_query("DELETE FROM sessions WHERE id='$idSession' limit 1 "))
 {
  if(@mysql_query("DELETE FROM crossUsersSessions  WHERE  sessionId='$idSession' "))
  {
   deleteAllOptions($idSession);  
   $result=true;
  } 
 }
 else
 {
  $result=false; 
 }

return $result;

}




function GetwinColorDepth($idSess)
{
 include "builder.cfg";
 $query="SELECT * FROM displaySettings WHERE idSession='$idSess'";
 $queryDb=@mysql_query($query);
 $dbArray=@mysql_fetch_array($queryDb);
 return $dbArray['winColorDepth'];
}

function GetJpegQuality($idSess)
{
 include "builder.cfg";
 $query="SELECT * FROM displaySettings WHERE idSession='$idSess'";
 $queryDb=@mysql_query($query);
 $dbArray=@mysql_fetch_array($queryDb);
 return $dbArray;
}

function getSessionTitle($title, $limit)
{
 if(strlen($title)>$limit)
 {
  $title=substr($title,0,$limit)."...";
 
 }
 return $title;
}



function createDB($dbHost,$dbAdminUsername,$dbAdminPasswd,$dbName,$mode)
{
 $verify="";
 $dbconnection = @mysql_connect($dbHost, $dbAdminUsername, $dbAdminPasswd);

 $version=explode(".",mysql_get_server_info());
 $storage="TYPE=MyISAM";
 if($version[0]>=5){
  $storage="ENGINE=MyISAM";
 }

 if(!$dbconnection)
 {
  logger("ERROR", "Connection to DB failed (in createDB function).");
 }
 
 if(!@mysql_select_db($dbName))
 {  
   $sql = 'CREATE DATABASE IF NOT EXISTS `'.$dbName.'` ';
   $res=@mysql_query($sql, $dbconnection);
   
   $verify=@mysql_select_db($dbName);
   
   if($verify)
   {
     
    //
    // Table structure for table `crossUsersSessions`
    //
         
     $sql=' CREATE TABLE IF NOT EXISTS `crossUsersSessions` ( '
     . '  `sessionId` int(10) NOT NULL default \'0\', '
     . '  `userId` int(10) NOT NULL default \'0\', '
     . '   KEY `sessionId` (`sessionId`), '
     . '  KEY `userId` (`userId`) '
     . ')  '.$storage;
      
     $res=@mysql_query($sql, $dbconnection);    

     $verify=@mysql_affected_rows($dbconnection);
   
     if(!$res)
     {
      logger("ERROR", "Create Table 'crossUsersSessions' failed (in createDB function).");
     }
     
    if($res)
    {
     //
     // Table structure for table `customOptions`
     //
   
      $sql=" CREATE TABLE IF NOT EXISTS `customOptions` (
       `id` int(11) NOT NULL auto_increment,
       `idSession` int(11) NOT NULL default '0',
       `typeDesk` varchar(255) NOT NULL default '',
       `custCommand` varchar(255) NOT NULL default '',
       `virtualDesktop` varchar(255) NOT NULL default '',
       `xAgentEncoding` varchar(255) NOT NULL default '',
       `useTaint` varchar(255) NOT NULL default '',
       PRIMARY KEY  (`id`)
     ) $storage
     ";
     $res=@mysql_query($sql, $dbconnection); 

     $verify=@mysql_affected_rows($dbconnection);
  
     if(!$res)
     {
      logger("ERROR", "Create Table 'customOptions' failed (in createDB function).");
     }
    }

    if($res)
    {
       //
       // Table structure for table `skinSetting`
       //
    
    

      $sql='CREATE TABLE IF NOT EXISTS `skinSetting` (
        `id` int(11) NOT NULL auto_increment,
        `name` varchar(255) NOT NULL default \'\',
        `typeSkin` varchar(50) NOT NULL default \'\',
        `bgtoolbar` varchar(7) NOT NULL default \'\',
        `bgtoolbarImage` varchar(255) NOT NULL default \'\',
        `bgBodyImage` varchar(255) NOT NULL default \'\',
        `bgHeader1` varchar(7) NOT NULL default \'\',
        `txtHeader1` varchar(7) NOT NULL default \'\',
        `bgHeader2` varchar(7) NOT NULL default \'\',
        `txtHeader2` varchar(7) NOT NULL default \'\',
        `txtHeader3` varchar(7) NOT NULL default \'\',
        `bgHeaderBox` varchar(7) NOT NULL default \'\',
        `txtHeaderBox` varchar(7) NOT NULL default \'\',
        `bgBox` varchar(7) NOT NULL default \'\',
        `bgBoxInternal` varchar(7) NOT NULL default \'\',
        `bgMiniBar` varchar(7) NOT NULL default \'\',
        `txtLabel` varchar(7) NOT NULL default \'\',
        `labelStyle` varchar(7) NOT NULL default \'\',
        `tabOver` varchar(7) NOT NULL default \'\',
        `tabBorder` varchar(7) NOT NULL default \'\',
        `border` varchar(7) NOT NULL default \'\',
        `enable` char(2) NOT NULL default \'0\',
        PRIMARY KEY  (`id`)
      ) '.$storage;


     $res=@mysql_query($sql, $dbconnection);    

     $verify=@mysql_affected_rows($dbconnection);
  
     if(!$res)
     {
      logger("ERROR", "Create Table 'skinSetting' failed (in createDB function).");
     }
    
   
      //
      // Dumping data for table `skinSetting`
      //
    
       
        $sql="INSERT INTO `skinSetting` VALUES (1, 'defaultGuest', 'default', '#EFEFE7', 'bg_wheel.gif', 'bg_body.gif', '#EFEFE7', '#595858', '#CEC6BD', '#ffffff', '#615F54', '#ece9d8', '#716F64', '#F8F6E9', '#ffffff', '#ece9d8', '#000000', 'normal', '#ffffff', 'orange', '#919B9D', '1')";        
        $res=@mysql_query($sql, $dbconnection);  
        $sql="INSERT INTO `skinSetting` VALUES (2, 'custom', 'custom', '#EFEFE7', 'bg_wheel.gif', 'bg_body.gif', '#EFEFE7', '#595858', '#CEC6BD', '#ffffff', '#615F54', '#ece9d8', '#716F64', '#F8F6E9', '#ffffff', '#ece9d8', '#000000', 'normal', '#ffffff', 'orange', '#919B9D', '0')";
        $res=@mysql_query($sql, $dbconnection);

        $verify=@mysql_affected_rows($dbconnection);
     
        if(!$res)
        {
         logger("ERROR", "Dumping data for table `skinSetting` failed (in createDB function).");
        }

          
    }    
    
    
    
    if($res)
    {    
       //
       // Table structure for table `displaySettings`
       //
       
       $sql=" 
        CREATE TABLE IF NOT EXISTS `displaySettings` (
          `id` int(11) NOT NULL auto_increment,
          `idSession` int(11) NOT NULL default '0',
          `renederExt` varchar(10) NOT NULL default 'false',
          `backingStore` varchar(10) NOT NULL default 'false',
          `composite` varchar(10) NOT NULL default 'false',
          `compType` varchar(5) NOT NULL default '3',
          `encodType` varchar(5) NOT NULL default '0',
          `jpegQuality` varchar(5) NOT NULL default '6',
          `winComp` varchar(5) NOT NULL default '',
          `shm` varchar(10) NOT NULL default 'false',
          `sharedPix` varchar(10) NOT NULL default 'false',
          `winColorDepth` char(3) NOT NULL default '',
          `rdpCache` varchar(10) NOT NULL default 'true',
          `rdpJpgQuality` varchar(5) NOT NULL default '6',
          `vncJpg` varchar(5) NOT NULL default '6',
          PRIMARY KEY  (`id`)
        ) $storage;
       ";
    
      $res=@mysql_query($sql, $dbconnection);

      $verify=@mysql_affected_rows($dbconnection);
     
      if(!$res)
      {
       logger("ERROR", "Create Table 'displaySettings' failed (in createDB function).");
      }

    }     

    if($res)
    {   
       //
       // Table structure for table `fontServers`
       //
       
       $sql=" 
       CREATE TABLE IF NOT EXISTS `fontServers` (
         `id` int(11) NOT NULL auto_increment,
         `idSession` int(11) NOT NULL default '0',
         `host` varchar(255) NOT NULL default '',
         `port` varchar(20) NOT NULL default '',
         PRIMARY KEY  (`id`)
       ) $storage;
       ";
      $res=@mysql_query($sql, $dbconnection);

      $verify=@mysql_affected_rows($dbconnection);
     
      if(!$res)
      {
       logger("ERROR", "Create Table 'fontServers' failed (in createDB function).");
      }

    }
   
    if($res)
    {         
       //
       // Table structure for table `httpProxy`
       //
       
       $sql=" 
       CREATE TABLE IF NOT EXISTS `httpProxy` (
         `id` int(11) NOT NULL auto_increment,
         `idSession` int(11) NOT NULL default '0',
         `host` varchar(255) NOT NULL default '',
         `port` varchar(8) NOT NULL default '8080',
         `username` varchar(50) NOT NULL default '',
         `password` varchar(255) NOT NULL default '',
         `remember` varchar(10) NOT NULL default 'false',
         PRIMARY KEY  (`id`)
       ) $storage;
       ";  
      $res=@mysql_query($sql, $dbconnection);

      $verify=@mysql_affected_rows($dbconnection);
     
      if(!$res)
      {
       logger("ERROR", "Create Table 'httpProxy' failed (in createDB function).");
      }
       
    }
   
    if($res)
    {      
       //
       // Table structure for table `rdpServers`
       //
       
       $sql=" 
       CREATE TABLE IF NOT EXISTS `rdpServers` (
         `id` int(11) NOT NULL auto_increment,
         `idSession` int(11) NOT NULL default '0',
         `server` varchar(255) NOT NULL default '',
         `domain` varchar(255) NOT NULL default '',
         `authentication` varchar(255) NOT NULL default '',
         `username` varchar(255) NOT NULL default '',
         `password` varchar(255) NOT NULL default '',
         `remember` varchar(255) NOT NULL default '',
         `windowsType` varchar(10) NOT NULL default 'false',
         `applicationWindows` varchar(255) NOT NULL default '',
         PRIMARY KEY  (`id`)
       ) $storage;
       ";
       $res=@mysql_query($sql, $dbconnection);

       $verify=@mysql_affected_rows($dbconnection);
     
       if(!$res)
       {
        logger("ERROR", "Create Table 'rdpServers' failed (in createDB function).");
       }
    
    }
   
    if($res)
    {      
 
       //
       // Table structure for table `servers`
       //
       
       $sql=" 
       CREATE TABLE IF NOT EXISTS `servers` (
         `id` int(10) NOT NULL auto_increment,
         `serverName` varchar(200) NOT NULL default '',
         `hostName` varchar(200) NOT NULL default '',
         `port` varchar(50) NOT NULL default '',
         `CPU` varchar(100) NOT NULL default '',
         `memory` varchar(100) NOT NULL default '',
         `disk` varchar(100) NOT NULL default '',
         `OS` varchar(100) NOT NULL default '',
         `description` varchar(200) NOT NULL default '',
         `icon` varchar(100) NOT NULL default '',
         `displayMode` varchar(16) NOT NULL default '',
         `replacingText` varchar(200) NOT NULL default '',
         `defaultLink` varchar(6) NOT NULL default '',
         `displayLink` int(1) NOT NULL default '0',
         PRIMARY KEY  (`id`)
       ) $storage;
       ";
       $res=@mysql_query($sql, $dbconnection);

       $verify=@mysql_affected_rows($dbconnection);
     
       if(!$res)
       {
        logger("ERROR", "Create Table 'servers' failed (in createDB function).");
       }
       
    }
   
    if($res)
    {         
       //
       // Table structure for table `sessions`
       //
       
       $sql=" 
       CREATE TABLE IF NOT EXISTS `sessions` (
         `id` int(10) NOT NULL auto_increment,
         `sessionName` varchar(255) NOT NULL default '',
         `desktopType` varchar(20) NOT NULL default '',
         `sessionType` varchar(20) NOT NULL default '',
         `displayCustom` char(1) NOT NULL default '0',
         `disableTcp` varchar(10) NOT NULL default 'false',
         `disableZlib` varchar(10) NOT NULL default 'false',
         `ssl` varchar(8) NOT NULL default 'false',
         `grapkeyboard` varchar(5) NOT NULL default 'false',
         `directDraw` varchar(5) NOT NULL default 'false',
         `lazyEncoder` varchar(10) NOT NULL default 'false',
         `enableHttpProxy` varchar(10) NOT NULL default 'false',
         `enableMultimedia` varchar(10) NOT NULL default 'false',
         `useFontServer` varchar(10) NOT NULL default 'false',
         `icon` varchar(100) NOT NULL default '',
         `idServer` int(11) NOT NULL default '0',
         PRIMARY KEY  (`id`)
       ) $storage;
       ";
       
       $res=@mysql_query($sql, $dbconnection);

       $verify=@mysql_affected_rows($dbconnection);
     
       if(!$res)
       {
        logger("ERROR", "Create Table 'sessions' failed (in createDB function).");
       }

    }
   
    if($res)
    {  
       
       //
       // Table structure for table `users`
       //
       
       $sql=" 
       CREATE TABLE IF NOT EXISTS `users` (
         `id` int(10) NOT NULL auto_increment,
         `userName` varchar(255) NOT NULL default '',
         `password` varchar(50) NOT NULL default '',
         `identity` varchar(20) NOT NULL default '',
         `credentialsNX` varchar(16) NOT NULL default '',
         PRIMARY KEY  (`id`)
       ) $storage;
       ";
       
       $res=@mysql_query($sql, $dbconnection);

       $verify=@mysql_affected_rows($dbconnection);
     
       if(!$res)
       {
        logger("ERROR", "Create Table 'users' failed (in createDB function).");
       }
       
       //
       // Dumping data for table `users`
       //
       
       $numAdmin="";
       
       $checkAdmin=@mysql_query("SELECT * from users WHERE identity='administrator' ");
       $numAdmnin=@mysql_num_rows($checkAdmin);
       
       $res=true;
       
       if($numAdmin==0)
       {
       
       $sql="INSERT INTO `users` VALUES ('', 'nxbuilder', '17bc51fca08dbd7f4d2c972b09b8d1c0', 'administrator', '1');";
       $res=@mysql_query($sql, $dbconnection);

       $verify=@mysql_affected_rows($dbconnection);
       
       }
       
       if(!$res)
       {
        logger("ERROR", "Dumping data for table 'users' failed (in createDB function).");
       }
       
    }
   
    if($res)
    {         
       
      
       //
       // Table structure for table `vncServers`
       //
       
       $sql=" 
       CREATE TABLE IF NOT EXISTS `vncServers` (
         `id` int(11) NOT NULL auto_increment,
         `idSession` int(11) NOT NULL default '0',
         `host` varchar(255) NOT NULL default '',
         `port` varchar(20) NOT NULL default '',
         `password` varchar(255) NOT NULL default '',
         `remember` varchar(10) NOT NULL default '',
         PRIMARY KEY  (`id`)
       ) $storage;
       ";
       
       $res=@mysql_query($sql, $dbconnection);

       $verify=@mysql_affected_rows($dbconnection);
     
       if(!$res)
       {
        logger("ERROR", "Dumping data for table 'vncServers' failed (in createDB function).");
       }       
       
    }
   
    if($res)
    {         
       //
       // Table structure for table `xdmOptions`
       //
       
       $sql=" 
       CREATE TABLE IF NOT EXISTS `xdmOptions` (
         `id` int(11) NOT NULL auto_increment,
         `idSession` int(11) NOT NULL default '0',
         `mode` varchar(255) NOT NULL default '',
         `listHost` varchar(255) NOT NULL default 'localhost',
         `listPort` varchar(255) NOT NULL default '177',
         `broadcastPort` varchar(255) NOT NULL default '177',
         `queryHost` varchar(255) NOT NULL default 'localhost',
         `queryPort` varchar(255) NOT NULL default '177',
         PRIMARY KEY  (`id`)
       ) $storage;
       ";
      $res=@mysql_query($sql, $dbconnection);

       $verify=@mysql_affected_rows($dbconnection);
     
       if(!$res)
       {
        logger("ERROR", "Dumping data for table 'xdmOptions' failed (in createDB function).");
       }
    }
  }
  else
  {
   $res=false;
   logger("ERROR", "Database '$dbName' failed (in createDB function).");
  }
 }
 else
 {
  logger("ERROR", "Database '$dbName' already exists (in createDB function).");
 }

 return $res; 
}


function get_table_def($table, $crlf,$dbname)
{

  $schema_create = "DROP TABLE IF EXISTS $table;$crlf";
  $db = $table;
  
  $schema_create .= "CREATE TABLE $table ($crlf";
  
  $result = @mysql_query("SHOW FIELDS FROM " .$dbname.".". $table) ; //or die()
  
  while($row = @mysql_fetch_array($result))
  {
   $schema_create .= " `$row[Field]` $row[Type]";
  
   if(isset($row["Default"]) && (!empty($row["Default"]) || $row["Default"] == "0"))
   {
    $schema_create .= " DEFAULT '$row[Default]'";
   } 
  
   if($row["Null"] != "YES")
   {
    $schema_create .= " NOT NULL";
   } 
   
   if($row["Extra"] != "")
   {
    $schema_create .= " $row[Extra]";
   }  
   
   $schema_create .= ",$crlf";
  
  }
  
  $schema_create = ereg_replace(",".$crlf."$", "", $schema_create);
  $result = @mysql_query("SHOW KEYS FROM " .$dbname."." .$table) ; //or die()
  
  while($row = @mysql_fetch_array($result))
  {
   $kname=$row['Key_name'];
   $comment=(isset($row['Comment'])) ? $row['Comment'] : '';
   $sub_part=(isset($row['Sub_part'])) ? $row['Sub_part'] : '';
  
   if(($kname != "PRIMARY") && ($row['Non_unique'] == 0)) $kname="UNIQUE|$kname";
  
   if($comment=="FULLTEXT") $kname="FULLTEXT|$kname";
   if(!isset($index[$kname])) $index[$kname] = array();
  
   if ($sub_part>1) $index[$kname][] = $row['Column_name'] . "(" . $sub_part . ")";
   else $index[$kname][] = $row['Column_name'];
  }
  
  while(list($x, $columns) = @each($index))
  {
   $schema_create .= ",$crlf";
   
   if($x == "PRIMARY") $schema_create .= " PRIMARY KEY (";
   elseif (substr($x,0,6) == "UNIQUE") $schema_create .= " UNIQUE " .substr($x,7)." (";
   elseif (substr($x,0,8) == "FULLTEXT") $schema_create .= " FULLTEXT ".substr($x,9)." (";
   else $schema_create .= " KEY $x (";
  
   $schema_create .= implode($columns,", ") . ")";
  }
  
  $schema_create .= "$crlf)";
  
  if(get_magic_quotes_gpc()) 
  {
   return (stripslashes($schema_create));
  } 
  else 
  {
   return ($schema_create);
  }
}


function get_table_content($db, $table, $limit_from = 0, $limit_to = 0,$handler)
{

 if ($limit_from > 0) 
 {
  $limit_from--;
 } 
 else 
 {
  $limit_from = 0;
 }
 
 if ($limit_to > 0 && $limit_from >= 0) 
 {
  $add_query = " LIMIT $limit_from, $limit_to";
 } 
 else 
 {
  $add_query = '';
 }

 get_table_content_fast($db, $table, $add_query,$handler);
}

function get_table_content_fast($db, $table, $add_query = '',$handler)
{
  $result = @mysql_query('SELECT * FROM ' . $db . '.' . $table . $add_query); //or die()
  if ($result != false) 
  {
  
    @set_time_limit(1200); // 20 Minutes
    
    // Checks whether the field is an integer or not
    for ($j = 0; $j < @mysql_num_fields($result); $j++) 
    {
      $field_set[$j] = @mysql_field_name($result, $j);
      $type = @mysql_field_type($result, $j);
      if ($type == 'tinyint' || $type == 'smallint' || $type == 'mediumint' || $type == 'int' || $type == 'bigint' ||$type == 'timestamp') 
      {
       $field_num[$j] = true;
      } 
      else 
      {
       $field_num[$j] = false;
      }
    } 
    
    // Get the scheme
    if (isset($GLOBALS['showcolumns'])) 
    {
     $fields = implode(', ', $field_set);
     $schema_insert = "INSERT INTO $table ($fields) VALUES (";
    } 
    else 
    {
     $schema_insert = "INSERT INTO $table VALUES (";
    }
    
    $field_count = @mysql_num_fields($result);
    
    $search = array("\x0a","\x0d","\x1a"); //\x08\\x09, not required
    $replace = array("\\n","\\r","\Z");
    
    
    while ($row = @mysql_fetch_row($result)) 
    {
     for ($j = 0; $j < $field_count; $j++) 
     {
      if (!isset($row[$j])) 
      {
       $values[] = 'NULL';
      } 
      else if (!empty($row[$j])) 
      {
       // a number
       if ($field_num[$j]) 
       {
         $values[] = $row[$j];
       }
       else  // a string 
       {
         $values[] = "'" . str_replace($search, $replace, addslashes($row[$j])) . "'";
       }
      } 
      else 
      {
        $values[] = "''";
      } 
     }     
     
     $insert_line = $schema_insert . implode(',', $values) . ')';
     unset($values);
    
     $handler($insert_line);
    } // end while
  } // end if ($result != false)

return true;
}


function my_handler($sql_insert)
{
 global $crlf, $asfile;
 global $tmp_buffer;

 if(empty($asfile)) $tmp_buffer.= htmlspecialchars("$sql_insert;$crlf");
 else $tmp_buffer.= "$sql_insert;$crlf";
}



function faqe_db_error()
{
 return @mysql_error();
}



function faqe_db_insert_id($result)
{
 return @mysql_insert_id($result);
}


function getOldServer($dbHostOld, $dbAdminUsernameOld, $dbAdminPasswdOld, $dbNameOld, $idS)
{

 $dbconnectionOld = @mysql_connect($dbHostOld, $dbAdminUsernameOld, $dbAdminPasswdOld);

 $server=false;

 if(@mysql_select_db($dbNameOld, $dbconnectionOld))
 {
  $query=@mysql_query("SELECT  * FROM `servers` WHERE id='$idS' ",$dbconnectionOld);  
  $server=@mysql_fetch_array($query);
  @mysql_close();
 }
 

return $server;
}



function updateDB($dbHost,$dbAdminUsername,$dbAdminPasswd,$dbName, $dbHostOld,$dbAdminUsernameOld,$dbAdminPasswdOld,$dbNameOld)
{

 $version=explode(".",mysql_get_server_info());
 $storage="TYPE=MyISAM";
 if($version[0]>=5){
  $storage="ENGINE=MyISAM";
 }
 
 $res=true;
 
 //if()
// { 
   
   $create=createDB($dbHost, $dbAdminUsername,$dbAdminPasswd,$dbName, "0");
   
   $dbconnection = @mysql_pconnect($dbHost, $dbAdminUsername, $dbAdminPasswd);

   if(!$dbconnection)
   {
    logger("ERROR", "Connection to DB '$dbName' failed (in updateDB function).");
   }

   //start users table update

   $dbconnectionOld = @mysql_connect($dbHostOld, $dbAdminUsernameOld, $dbAdminPasswdOld);
   
   if(!$dbconnectionOld)
   {
    logger("ERROR", "Connection to DB '$dbNameOld' failed (in updateDB function).");
    $res=false;
   }
  

  
   if(@mysql_select_db($dbNameOld, $dbconnectionOld))
   {
     $queryUserOld="SELECT * from `users` ";
     $execOld=@mysql_query($queryUserOld,$dbconnectionOld );
     @mysql_close();
     
     $dbconnection = @mysql_connect($dbHost, $dbAdminUsername, $dbAdminPasswd);
     $d=@mysql_select_db($dbName, $dbconnection);
   
   
    $sqlDrop='DELETE FROM `users` ';
    $drop=@mysql_query($sqlDrop, $dbconnection);     
                 
     while($userOld=@mysql_fetch_array($execOld))
     {
      if($userOld['credentialsNX']=='1')
      {
       $userOld['credentialsNX']='2';
      }

      if($userOld['identity']=='guest')
      {
       $userOld['credentialsNX']='1';
      }

      $insert="INSERT INTO `users` SET  `id`='".$userOld['id']."', `userName`='".$userOld['userName']."', `password`='".$userOld['password']."', `identity`='".$userOld['identity']."', `credentialsNX`='".$userOld['credentialsNX']."' ";
      
      $res=@mysql_query($insert, $dbconnection);
     }


      $verify=@mysql_affected_rows($dbconnection);
      
      
   /*   if($verify>0)
      {
       $res=true;
      }
      else
      {
       $res=false;
       logger("ERROR", "ver: $verify - Update data for table `users` failed (in updateDB function).");
      } */


    if($res)
    {
     

     //start servers table update
     $dbconnectionOld = @mysql_connect($dbHostOld, $dbAdminUsernameOld, $dbAdminPasswdOld);
     
      
    
     if(@mysql_select_db($dbNameOld, $dbconnectionOld))
     {
       $queryServerOld="SELECT * from `servers` ";
       $execOld=@mysql_query($queryServerOld,$dbconnectionOld );
       @mysql_close();
     }
  
  
     $dbconnection = @mysql_connect($dbHost, $dbAdminUsername, $dbAdminPasswd);
     $d=@mysql_select_db($dbName, $dbconnection);



       //
       // Table structure for table `servers`
       //

       //$sql=' DROP TABLE IF EXISTS `servers` ';
       //$res=@mysql_query($sql, $dbconnection);
       
       $sql=" 
       CREATE TABLE IF NOT EXISTS `serversTmp` (
         `id` int(10) NOT NULL auto_increment,
         `serverName` varchar(200) NOT NULL default '',
         `hostName` varchar(200) NOT NULL default '',
         `port` varchar(50) NOT NULL default '',
         `CPU` varchar(100) NOT NULL default '',
         `memory` varchar(100) NOT NULL default '',
         `disk` varchar(100) NOT NULL default '',
         `OS` varchar(100) NOT NULL default '',
         `description` varchar(200) NOT NULL default '',
         `icon` varchar(100) NOT NULL default '',
         `displayMode` varchar(16) NOT NULL default '',
         `replacingText` varchar(200) NOT NULL default '',
         `defaultLink` varchar(6) NOT NULL default '',
         `displayLink` int(1) NOT NULL default '0',         
         PRIMARY KEY  (`id`)
       ) $storage
       ";
       $res=@mysql_query($sql, $dbconnection);

       $verify=@mysql_affected_rows($dbconnection);
     
       if(!$res)
       {
        logger("ERROR", "Create Table 'serversTmp' failed (in UpdateDB function).");
       }

 //   $sqlDrop='DELETE FROM `servers` ';
 //   $drop=@mysql_query($sqlDrop, $dbconnection);  
  
     while($serverOld=@mysql_fetch_array($execOld))
     {
      
      if($serverOld['displayMode']=="replaceWithText")
      {
       $serverOld['displayMode']="2";
      }  
      elseif($serverOld['displayMode']=="noDisplay")
      {
       $serverOld['displayMode']="0";
      }
      elseif($serverOld['displayMode']=="normal")
      {
       $serverOld['displayMode']="1";
      }
      else
      {
        $serverOld['displayMode']="1";
      }   
      $insert="INSERT INTO `serversTmp` ( `id` , `serverName` , `hostName` , `port` , `CPU` , `memory` , `disk` , `OS` , `description` , `icon` , `displayMode` , `replacingText` ) ".
               " VALUES ('".$serverOld['id']."', '".$serverOld['serverName']."', '".$serverOld['hostName']."', ". 
               " '".$serverOld['port']."', '".$serverOld['CPU']."', '".$serverOld['memory']."', ".
               " '".$serverOld['disk']."', '".$serverOld['OS']."', '".$serverOld['description']."', ". 
               " '".$serverOld['icon']."', '".$serverOld['displayMode']."', '".$serverOld['replacingText']."') ";
      
      @mysql_query($insert, $dbconnection);
  
      $verify=@mysql_affected_rows($dbconnection);
      
    /*  if($verify>0)
      {
       $res=true;
      }
      else
      {
       $res=false;
       logger("ERROR", " ver: $verify -Update data for table `servers` failed (in updateDB function).");
      }  */
      
     }
     
     
     
     
     
     
    }

    if($res)
    {
     
     
     
     //start sessions table update
     
     $dbconnectionOld = @mysql_connect($dbHostOld, $dbAdminUsernameOld, $dbAdminPasswdOld);
     
      
    
     if(@mysql_select_db($dbNameOld, $dbconnectionOld))
     {
       $querySessionsOld="SELECT * from `sessions` ";
       $execOld=@mysql_query($querySessionsOld,$dbconnectionOld );
       @mysql_close();
     }
     
     
     $dbconnection = @mysql_pconnect($dbHost, $dbAdminUsername, $dbAdminPasswd);  
     $d=@mysql_select_db($dbName, $dbconnection);



       //
       // Table structure for table `sessions`
       //

       $sqlDel=' DROP TABLE IF EXISTS `sessions` ';
       $res=@mysql_query($sqlDel, $dbconnection);
       
       $sql=" 
       CREATE TABLE IF NOT EXISTS `sessions` (
         `id` int(10) NOT NULL auto_increment,
         `sessionName` varchar(255) NOT NULL default '',
         `desktopType` varchar(20) NOT NULL default '',
         `sessionType` varchar(20) NOT NULL default '',
         `displayCustom` char(1) NOT NULL default '0',
         `disableTcp` varchar(10) NOT NULL default 'false',
         `disableZlib` varchar(10) NOT NULL default 'false',
         `ssl` varchar(8) NOT NULL default 'false',
         `grapkeyboard` varchar(5) NOT NULL default 'false',
         `directDraw` varchar(5) NOT NULL default 'false',
         `lazyEncoder` varchar(10) NOT NULL default 'false',
         `enableHttpProxy` varchar(10) NOT NULL default 'false',
         `enableMultimedia` varchar(10) NOT NULL default 'false',
         `useFontServer` varchar(10) NOT NULL default 'false',
         `icon` varchar(100) NOT NULL default '',
         `idServer` int(11) NOT NULL default '0',
         PRIMARY KEY  (`id`)
       ) $storage
       ";
       
       $res=@mysql_query($sql, $dbconnection);

       $verify=@mysql_affected_rows($dbconnection);
     
       //if(!$verify)
       //{
       // logger("ERROR", "Create Table 'sessions' failed (in UpdateDB function).");
       //}

 
     while($sessionOld=@mysql_fetch_array($execOld))
     {
  
  
     $serverOld=getOldServer($dbHostOld, $dbAdminUsernameOld, $dbAdminPasswdOld, $dbNameOld,$sessionOld['serverId']);
     
      
     
     $sessionType="";
     
     $sessName=$sessionOld['desktop']."-".$serverOld['serverName'];
     $notGood = array(",",";","_","?",":","'"," ");
     $nowGood = array("-","-","-","-","-","-","-");
     $sessName=str_replace($notGood,$nowGood,$sessName);
     
     if($sessionOld['application']!="")
     {
      $sessName=$sessionOld['applicationName']."-".$serverOld['serverName'];
      $notGood = array(",",";","_","?",":","'"," ");
      $nowGood = array("-","-","-","-","-","-","-");
      $sessName=str_replace($notGood,$nowGood,$sessName);
      
      $deskType="unix";
      $sessionType="custom";
      
      $cusCom=$sessionOld['application'];
           
      $queryCust = "INSERT INTO customOptions  ( `id` , `idSession` , `typeDesk` , `custCommand`, `virtualDesktop`, `xAgentEncoding`, `useTaint`) VALUES
      ('','".$sessionOld['id']."','application','$cusCom','false', 'false' , 'false')";
      
      @mysql_query($queryCust, $dbconnection) or die(@mysql_error());
   
      
     }
     
     if($sessionOld['desktop']=="rdp")
     {
      $deskType="windows";
      $sessionType="windows";
  
      $queryRdp = "INSERT INTO rdpServers ( `id` , `idSession` , `server` , `domain`, `authentication`, `username`, `password` , `remember`) VALUES
      ('','".$sessionOld['id']."','".$serverOld['serverRDP']."','','1', '' , '', 'false')";
  
      
  
       @mysql_query($queryRdp, $dbconnection);
      
      $verify=@mysql_affected_rows($dbconnection);

      if($verify>0)
      {
       $res=true;
      }
      else
      {
       $res=false;
       logger("ERROR", "Update data for table `rdpServers` failed (in updateDB function).");
      } 
  
     }
     elseif($sessionOld['desktop']=="rfb")
     {
      $deskType="vnc";
      $sessionType="vnc";
       $queryVnc = "INSERT INTO vncServers ( `id` , `idSession` , `host` , `port`, `password` , `remember`) VALUES
       ('','".$sessionOld['id']."','".$serverOld['serverVNC']."','".$serverOld['displayVNC']."','".$serverOld['passwordVNC']."', 'true' )"; 
   
        @mysql_query($queryVnc, $dbconnection); 

      $verify=@mysql_affected_rows($dbconnection);
      
      if($verify>0)
      {
       $res=true;
      }
      else
      {
       $res=false;
       logger("ERROR", "Update data for table `vncServers` failed (in updateDB function).");
      } 
       
     }
     else
     {
      $deskType="unix";
      $sessionType=$sessionOld['desktop'];
     }
     
  
      
     $insert = "INSERT INTO sessions ( `id` , `sessionName` , `desktopType` , `sessionType`, `displayCustom`, `disableTcp`, `disableZlib` , `ssl`, `grapkeyboard`, `lazyEncoder`, `enableHttpProxy` , `enableMultimedia`, `useFontServer`, `icon` , `idServer`) VALUES
     ('".$sessionOld['id']."','".$sessName."','".$deskType."','".$sessionType."','0', 'false' , 'false', '".$serverOld['ssl']."','false' , 'false', 'false' ,'false', 'false', '".$sessionOld['icon']."', '".$sessionOld['serverId']."')";    
      
      @mysql_query($insert, $dbconnection);

      $verify=@mysql_affected_rows($dbconnection);
      
      if($verify>0)
      {
       $res=true;
      }
      else
      {
       $res=false;
       logger("ERROR", "Update data for table `sessions` failed (in updateDB function).");
      }  

     
     } // end while($sessionOld=@mysql_fetch_array($execOld))
     
    }

    if($res)
    {
     
   
     //start crossusersessions table update
     $dbconnectionOld = @mysql_connect($dbHostOld, $dbAdminUsernameOld, $dbAdminPasswdOld);
     
      
    
     if(@mysql_select_db($dbNameOld, $dbconnectionOld))
     {
       $queryCrossUserOld="SELECT * from `crossUsersSessions` ";
       $execOld=@mysql_query($queryCrossUserOld,$dbconnectionOld );
       @mysql_close();
     }
  
  
     $dbconnection = @mysql_connect($dbHost, $dbAdminUsername, $dbAdminPasswd);
     $d=@mysql_select_db($dbName, $dbconnection);

    $sqlDrop='DELETE FROM `crossUsersSessions` ';
    $drop=@mysql_query($sqlDrop, $dbconnection); 
  
     while($crossUserrOld=@mysql_fetch_array($execOld))
     {
      //$deleteIfExist=@mysql_query("DROP TABLE IF EXISTS `sessions`");
      $insert="INSERT INTO `crossUsersSessions` ( `sessionId` , `userId` ) VALUES ('".$crossUserrOld['sessionId']."', '".$crossUserrOld['userId']."') ";
      @mysql_query($insert, $dbconnection);
     }   
   
     $verify=@mysql_affected_rows($dbconnection);
      
      if($verify>0)
      {
       $res=true;
      }
      else
      {
       $res=false;
        logger("ERROR", "Update data for table `crossUsersSessions` failed (in updateDB function).");
      } 
  
    }

   $sql=' DROP TABLE IF EXISTS `servers` ';
   $res=@mysql_query($sql, $dbconnection);
 
   $rename=@mysql_query("RENAME TABLE `serversTmp` TO `servers` ");
  
  }//end if(@mysql_select_db($dbNameOld, $dbconnectionOld))    
  else
  {
   $res=false;
   logger("ERROR", "Connection to DB '$dbNameOld' failed (in updateDB function).");
  }
 //}//end if(createBD(......)  
 //else
 //{
 // $res=false;
 // logger("ERROR", "Create DB '$dbName' failed (in updateDB function).");
 //}  
   
return $res; 
}

function getTemplate()
{
 include "builder.cfg";
 $query="SELECT * FROM skinSetting  WHERE enable='1' ";
 
 $querySkin=@mysql_query($query);
 $skin=@mysql_fetch_array($querySkin);
 
return $skin;
}

function updateTemplateOld($id)
{
 include "builder.cfg";
 $query="Update skinSetting SET enable='0' ";
 
 $querySkin=@mysql_query($query);
 
 $query="Update skinSetting SET enable='1' where id='$id' ";
 
 $querySkin=@mysql_query($query);

 
 
return $skin;
}


function formatDesc($description)
{
 $result=nl2br(wordwrap($description,50,"\n",1));

 return $result;
}


function truncSlash($path)
{
 
 $l=strlen($path);

 while((substr($path, -1)=="\\") || (substr($path, -1)=="/"))
 {
  $l--;
  $path=substr($path,0, $l);
 }
 
 return $path;
}


function updateIcon($hostDB_old,$nameDB_old,$userDB_old,$passDB_old,$path, $path2)
{
  include "conf.php";
  
  
  $path=truncSlash($path);
  $path2=truncSlash($path2);
     
  $res=true; 
  
  $dbconn = @mysql_connect($hostDB_old, $userDB_old, $passDB_old);

  if(@mysql_select_db($nameDB_old))
  {
   $query_serverOld="SELECT id, icon from servers ";
   $query=@mysql_query($query_serverOld);
   $server=@mysql_fetch_array($query);
  
   
   while($server)
   {
    $f1=$path."/".$server['icon'];
    $f2=$_SERVER['DOCUMENT_ROOT'].dirname($_SERVER['PHP_SELF'])."/".$serverShared."/".$server['icon'];
    if(@copy($f1,$f2))
    {
      //TO DO manage message
    }
    else
    {
     //TO DO manage error

    } 
    $server=@mysql_fetch_array($query);
   }
   
   $query_sessionOld="SELECT id, icon from sessions ";
   $query=@mysql_query($query_sessionOld);
   $session=@mysql_fetch_array($query);
  
   
   while($session)
   {
    $f1=$path2."/".$session['icon'];
    $f2=$_SERVER['DOCUMENT_ROOT'].dirname($_SERVER['PHP_SELF'])."/".$sessionShared."/".$session['icon'];
    if(@copy($f1,$f2))
    {
    }
    else
    {
    } 
    $session=@mysql_fetch_array($query);
   }
   
  }
  else
  {
   $res=false;
  } 
return $res;
}

function updateTemplate($c5,$c7,$c8,$c9, $c10, $c11)
{
 include "builder.cfg";
 $query="Update skinSetting SET enable='0'";
 
 $querySkin=@mysql_query($query);

 if ($handle = opendir('style')) 
 {
   while (false !== ($file = readdir($handle))) 
   { 
    if(substr($file, 0, 7)=="custom_")
    {
      unlink("style/$file"); 
    }
   }
  closedir($handle); 
 }

 
 $name="custom_".date("YmdHis");
 
 $query="UPDATE `skinSetting` SET
        `name` = '".$name."',  
        `bgHeaderBox` = '".$c5."',
        `txtHeaderBox` = '".$c7."',
        `bgBox` = '".$c8."',
        `bgBoxInternal` = '#ffffff',
        `bgMiniBar` = '".$c5."',
        `txtLabel` = '".$c10."' ,
        `labelStyle` = '".$c9."',
        `border` = '".$c11."',
        `enable`     ='1' 
        WHERE `typeSkin` = 'custom' "; 
 
 $querySkin=@mysql_query($query);

 
return $skin;
}

function getColorTemplate()
{

 include "builder.cfg";
 $queryString="SELECT * from skinSetting where enable='1' ";
 $query=@mysql_query($queryString);
 
 return $query;

}

function getColorDefaultTemplate()
{

 include "builder.cfg";
 $queryString="SELECT * from skinSetting where name='defaultGuest' AND  typeSkin='default' ";
 $query=@mysql_query($queryString);
 
 return $query;

}


function resetSkinGuest()
{

 include "builder.cfg";
 
 $query="Update skinSetting SET enable='0'";
 
 $querySkin=@mysql_query($query);

 $query="Update skinSetting SET enable='1' where  typeSkin='default' ";
 
 $querySkin=@mysql_query($query);
}



function resetSkin()
{

 include "builder.cfg";
 
 $query="Update skinSetting SET enable='0'";
 
 $querySkin=@mysql_query($query);

 $query="Update skinSetting SET enable='1' where  typeSkin='default' ";
 
 $querySkin=@mysql_query($query);
}


function rewriteCss()
{
 include "builder.cfg";
  $querySettings=getColorTemplate();
  
  $settings=@mysql_fetch_array($querySettings);
  
  $bold=$settings['labelStyle'];
   
  $c1=$settings['bgtoolbar']; 
  $c2=$settings['txtHeader1'];
  $c3=$settings['bgHeader2'];
  $c4=$settings['txtHeader2'];
  $c5=$settings['bgMiniBar'];
  $c6a=$settings['tabOver'];
  $c6b=$settings['tabBorder'];
  $c7=$settings['txtHeaderBox'];
  $c8=$settings['bgBox'];
  $c10=$settings['txtLabel'];
  $c11=$settings['border'];
   

 
$content="
.bgHeaderTableGuest
{
 background-color:".$settings['bgHeaderBox'].";
 border: 1px solid $c11;
 border-bottom: 0px;
}


.row1Guest
{
 background: $c8;
 border:1px solid $c11; 
 border-top:0px;
}
.row2Guest
{
 background: #FFFFFF; 
 border:1px solid $c11; 
 border-top:0px solid black;
}



.border_topGuest
{
 border-top: 1px solid $c11;
}
.border_rightGuest
{
 border-right:1px solid $c11;
}

.border_bottomGuest
{
 border-bottom: 1px solid $c11;
}

.border_leftGuest
{
 border-left: 1px solid $c11;
}

.borderAllGuest
{
 border: 1px solid $c11;
}
.bgDivServerGuest{
background:".$settings['bgHeaderBox'].";
border-bottom:1px solid $c11;
}



.bgGuest
{
 background-color: ".$settings['bgHeaderBox'].";
}

.txtHeaderTableGuest
{
 text-decoration : none;
 color: ".$settings['txtHeaderBox'].";
 font-family: verdana,arial,helvetica,sans-serif;
 font-size: 11px;
 font-weight: bold;
}

.bgMiniBarGuest
{
 border: 1px solid $c11; 
 background-color: ".$settings['bgHeaderBox'].";
 margin-top:20px;
 min-width:900px;
}
.bgHedearServerGuest
{
 border: 1px solid $c11; 
 background-color: ".$settings['bgHeaderBox'].";
 margin-top:20px;
 min-width:900px;
}

.messageGuest
{
 border:1px solid $c11; 
 width:85%;
 padding:1px; 
 margin:0 auto; 
 background-color:#fff;
}


.bgBoxGuest
{
 background-color: $c8;
}



.bgBoxInternalGuest
{
 background-color: $c8;
 border: 1px solid $c11; 
}


.txtLabelGuest
{
 text-decoration : none;
 color: ".$settings['txtLabel'].";
 font-family: verdana,arial,helvetica,sans-serif;
 font-size: 11px;
 font-weight: $bold; 
}

.txtValueGuest
{
 text-decoration : none;
 color: ".$settings['txtLabel'].";
 font-family: verdana,arial,helvetica,sans-serif;
 font-size: 11px;
 font-weight: normal; 
}



#border_boxGuest
{
 border-top:1px solid $c11; 
 width:99%;
 padding-bottom:15px;
 background: $c8;
}
#border_box_previewGuest
{
 border:1px solid $c11; 
 width:838px;
 padding-bottom:15px;
 background: /*$c8*/#ffffff;
}
"; 


  $css=$settings['name'];

  if (!$handle = fopen("style/$css.css", "w+"))
  {
   //echo "error: cannot create the file: style/$css.css";
  }

  if (!fwrite($handle, $content))
  {
   //echo "error: cannot write in the file: style/$css.css";
  }
  fclose($handle); 
}

?>
