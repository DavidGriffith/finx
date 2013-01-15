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
if (!isset($_SESSION['userName'])) {session_start();}
define('numValidCharList',85);
define('dummyString',"{{{{");

$validCharList = array
(
  '!',  '#',  '$',  '%',  '&',  '(', ')',  '*',  '+',  '-',
  '.',  '0',   '1',  '2',   '3',  '4',  '5',  '6', '7', '8',
  '9', ':',  ';',  '<',  '>',  '?',  '@',  'A',  'B', 'C',
  'D',  'E',  'F',  'G',  'H',  'I',  'J',  'K',  'L', 'M',
  'N', 'O',  'P',  'Q',  'R',  'S',  'T', 'U', 'V', 'W',
  'X',  'Y',  'Z',  '[', ']',  '_',  'a',  'b',  'c',  'd',
  'e',  'f',  'g',  'h',  'i',  'j',  'k',  'l',  'm',  'n',
  'o',  'p',  'q',  'r',  's',  't',  'u',  'v',  'w',  'x',
  'y',  'z',  '{',  '|',  '}'
);

function findCharInList($c)
{

  global $validCharList;

  $i = -1;

  for ($j = 0; $j < numValidCharList; $j++)
  {
    if ($validCharList[$j] == "$c")
    {
      $i = $j;
      return $i;
    }
  }


  return $i;
}

function getRandomValidCharFromList()
{
  global $validCharList;
  $k = (integer) date("s");
  return $validCharList[$k];
}

function scrambleString($s)
{
  global $validCharList;
  $sRet = "";
  if ((!isset($s))||(empty($s)))
  {
    return $s;
  }
  $str = encodePassword($s);
  if (strlen($str) < 32)
  {
    $sRet .= dummyString;
  }

  for ( $iR = (strlen($str) - 1); $iR >= 0; $iR--)
  {
    //
    // Reverse string.
    //
    $sRet .= substr($str,$iR,1);
  }

  if (strlen($sRet) < 32)
  {
    $sRet .= dummyString;
  }

  $app=getRandomValidCharFromList();
  $k=ord($app);
  $l=$k + strlen($sRet) -2;
  $sRet= $app.$sRet;


  for ($i1 = 1; $i1 < strlen($sRet); $i1++)
  {

    $app2=substr($sRet,$i1,1);
    $j = findCharInList($app2);

    if ($j == -1)
    {
      return $sRet;
    }
    $i = ($j + $l * ($i1 + 1)) % numValidCharList;
    $car=$validCharList[$i];

    $sRet=substr_replace($sRet,$car,$i1,1);

  }

  $c = (ord(getRandomValidCharFromList())) + 2;
  $c2=chr($c);

  $sRet=$sRet.$c2;


  return htmlspecialchars($sRet,ENT_QUOTES);
}

function CreateSessionFile3_0($id, $setDisplayResolution, $setLink)
{
//
// Function write parameters in to session file.
//

  include "builder.cfg";
 
  $querySession=mysql_query("SELECT sessions.*, servers.hostName, servers.port as serverPort, servers.defaultLink from sessions, servers where servers.id=sessions.idServer AND sessions.id= '".$id."'");
  $session=@mysql_fetch_array($querySession);
 

  if(!$session)
  {
   echo("d js");
   return -1;
  }
  else
  {
   if ($setLink == "") $setLink = $session['defaultLink'];
 
 
   $isGuest='false';
   $multimonitor="false";
     
  //
  // resolution
  //
  
  
  if( $setDisplayResolution == "800x600") 
  {
   $screen = "800x600";
   $h = "600";
   $w = "800";
  }
  elseif( $setDisplayResolution == "1024x768") 
  {
   $screen = "1024x768";
   $h = "768";
   $w = "1024";
  }
  elseif( $setDisplayResolution == "fullscreen")
  {
   $screen = "fullscreen";
   $h = "600";
   $w = "800"; 
  }
  elseif( $setDisplayResolution == "availablearea")
  {
   $screen = "available";
   $h = "600";
   $w = "800"; 
  }
  elseif( $setDisplayResolution == "fullscreen_mm")
  {
   $screen = "fullscreen";
   $h = "600";
   $w = "800"; 
   $multimonitor = "true" ; 
  }
  elseif( $setDisplayResolution == "available_area_mm")
  {
   $screen = "available";
   $h = "600";
   $w = "800"; 
   $multimonitor = "true" ;
  }
  else {
   $screen = "800x600";
   $h = "600";
   $w = "800";
  }
  

   $proxyHost = "";
   $proxyPort = "8080";
   $proxyUser = "";
   $proxyPsw="";
  
  if($session['enableHttpProxy'] == "true")
  {
   $proxy=@mysql_query("SELECT * FROM httpProxy WHERE idSession = '".$id."'");
   $proxySess=@mysql_fetch_array($proxy);
   $proxyHost = $proxySess['host'];
   $proxyPort = $proxySess['port'];
   $proxyUser = $proxySess['username'];
   $proxyPsw = $proxySess['password'];
   $remember = "false";
   if($proxySess['remember'] == "true") 
   {
    $remember="true";
   }
   
  }
  
   $usefont = "false";
   $fontHost = "";
   $fontPort = "7100";
   
  
  if($session['useFontServer'] == "true")
  {
   $font=@mysql_query("SELECT * FROM fontServers WHERE idSession = '".$id."'");
   $fontSess=@mysql_fetch_array($font);
   $usefont = "true";
   $fontHost = $fontSess['host'];
   $fontPort = $fontSess['port'];
  }
   
   $mode       = "server decide";
   $queryHost  = "localhost";
   $queryPort  = "177";
   $listHost   = "localhost";
   $listPort   = "177";
   $broadPort  = "177";
   
  
  if($session['sessionType'] == "custom")
  {
   $option=@mysql_query("SELECT * FROM customOptions WHERE idSession = '".$id."'");
   $optionSess=@mysql_fetch_array($option);
   if( $optionSess['typeDesk'] == "application")
   {
    $desk = "application";
    $command = $optionSess['custCommand'];
   }
   elseif( $optionSess['typeDesk'] == "console")
   {
    $desk = "console";
    $command = "";
   }
   elseif( $optionSess['typeDesk'] == "default")
   {
    $desk = "default";
    $command = "";
   }
  
  }
  elseif($session['sessionType'] == "xdm")
  {
   $option=@mysql_query("SELECT * FROM xdmOptions WHERE idSession = '".$id."'");
   $optionSess=@mysql_fetch_array($option);
   
   if( $optionSess['mode'] == "query")
   {
    $mode = "query";
    $queryHost = $optionSess['queryHost'];
    $queryPort = $optionSess['queryPort'];
   }
   elseif( $optionSess['mode'] == "server decide")
   {
    $mode = "server decide";
   }
   elseif( $optionSess['mode'] == "list")
   {
    $mode = "list";
    $listHost = $optionSess['listHost'];
    $listPort = $optionSess['listPort'];
   }
   elseif( $optionSess['mode'] == "broadcast")
   {
    $mode = "broadcast";
    $broadPort = $optionSess['broadcastPort'];
   }
  }
  
   $display     = @mysql_query("SELECT * FROM displaySettings WHERE idSession = '".$id."'");
   $displaySess = @mysql_fetch_array($display);
  
  if($displaySess['shm']== "") $shm = "false";
  else $shm =  $displaySess['shm'];
  
  if($displaySess['sharedPix']== "") $esp = "false";
  else $esp =  $displaySess['sharedPix'];
  
  if($displaySess['winColorDepth']== "") $colorDepth = "8";
  else $colorDepth =  $displaySess['winColorDepth'];
  
   $rdp     = @mysql_query("SELECT * FROM rdpServers WHERE idSession = '".$id."'");
   $rdpSess = @mysql_fetch_array($rdp);
   
   
  if($rdpSess['authentication'] == "") $auth = "2";
  else $auth =  $rdpSess['authentication'];
  
  if($rdpSess['authentication'] == "2")
  {
   $rdpSess['username'] = $rdpUser  = $_SESSION['userName'];
   $rdpSess['password'] = $rdpPsw   = $_SESSION['password'];
  }
  
  if($rdpSess['domain'] == "") $domain ="";
  else $domain =$rdpSess['domain'];
  
  if($rdpSess['password'] == "" ) $rdpPsw ="EMPTY_PASSWORD";
  else $rdpPsw = $rdpSess['password'];
  
  if($rdpSess['remember'] == "" ) $rdpRem ="true";
  else $rdpRem = $rdpSess['remember'];
  
  if($rdpSess['username'] == "" ) $rdpUser="";
  else $rdpUser = $rdpSess['username'];
  
   $vnc     = @mysql_query("SELECT * FROM vncServers WHERE idSession = '".$id."'");
   $vncSess = @mysql_fetch_array($vnc);
  
  $directDraw="false";
  if($session['directDraw']!="")
  {
   $directDraw=$session['directDraw'];
  }
  
  if(!isset($optionSess['useTaint'])) $optionSess['useTaint']="";
  if(!isset($optionSess['virtualDesktop'])) $optionSess['virtualDesktop']="";
  if(!isset($optionSess['xAgentEncoding'])) $optionSess['xAgentEncoding']="";
  if(!isset($remember)) $remember = "";
  if(!isset($command))  $command  = "";
  if(!isset($desk))     $desk     = "";
  
  
echo'<!DOCTYPE NXClientSettings>
<NXClientSettings application="nxclient" version="1.3" >
<group name="Advanced" >
<option key="Cache size" value="8" />
<option key="Cache size on disk" value="32" />
<option key="Current keyboard" value="true" />
<option key="Custom keyboard layout" value="" />
<option key="Disable DirectDraw" value="'.$directDraw.'" />
<option key="Disable ZLIB stream compression" value="'.$session['disableZlib'].'" />
<option key="Disable deferred updates" value="'.$session['lazyEncoder'].'" />
<option key="Enable HTTP proxy" value="'.$session['enableHttpProxy'].'" />
<option key="Enable SSL encryption" value="'.$session['ssl'].'" />
<option key="Enable response time optimisations" value="false" />
<option key="Grab keyboard" value="'.$session['grapkeyboard'].'" />
<option key="HTTP proxy host" value="'.$proxyHost.'" />
<option key="HTTP proxy port" value="'.$proxyPort.'" />
<option key="HTTP proxy username" value="'.$proxyUser.'" />
<option key="HTTP proxy password" value="'.$proxyPsw.'" />
<option key="Remember HTTP proxy password" value="'.$remember.'" />
<option key="Restore cache" value="true" />
<option key="StreamCompression" value="" />
</group>
<group name="Environment" >
<option key="Font server host" value="'.$fontHost.'" />
<option key="Font server port" value="'.$fontPort.'" />
<option key="Use font server" value="'.$usefont.'" />
</group>
<group name="General" >
<option key="Automatic reconnect" value="true" />
<option key="Command line" value="'.$command.'" /> 
<option key="Custom Unix Desktop" value="'.$desk.'" />
<option key="Desktop" value="'.$session['sessionType'].'" />
<option key="Disable SHM" value="'.$shm.'" />
<option key="Disable emulate shared pixmaps" value="'.$esp.'" />
<option key="Link speed" value="'.$setLink.'" />
<option key="Remember password" value="true" />
<option key="Resolution" value="'.$screen.'" />
<option key="Resolution height" value="'.$h.'" />
<option key="Resolution width" value="'.$w.'" />
<option key="Spread over monitors" value="'.$multimonitor.'" />
<option key="Server host" value="'.$session['hostName'].'" />
<option key="Server port" value="'.$session['serverPort'].'" />
<option key="Session" value="'.$session['desktopType'].'" />
<option key="Use default image encoding" value="'.$session['displayCustom'].'" />
<option key="Use render" value="'.$displaySess['renederExt'].'" />
<option key="Use taint" value="'.$optionSess['useTaint'].'" />
<option key="Virtual desktop" value="'.$optionSess['virtualDesktop'].'" />
<option key="XAgent encoding" value="'.$optionSess['xAgentEncoding'].'" />
<option key="displaySaveOnExit" value="true" />
<option key="xdm broadcast port" value="'.$broadPort.'" />
<option key="xdm list host" value="'.$listHost.'" />
<option key="xdm list port" value="'.$listPort.'" />
<option key="xdm mode" value="'.$mode.'" />
<option key="xdm query host" value="'.$queryHost.'" />
<option key="xdm query port" value="'.$queryPort.'" />
</group>';

if($displaySess['idSession'] !== "")
{
echo'
<group name="Images" >
<option key="Disable JPEG Compression" value="0" />
<option key="Disable all image optimisations" value="false" />
<option key="Disable backingstore" value="'.$displaySess['backingStore'].'" />
<option key="Disable composite" value="'.$displaySess['composite'].'" />
<option key="Image Compression Type" value="'.$displaySess['compType'].'" />
<option key="Image Encoding Type" value="0" />
<option key="Image JPEG Encoding" value="false" />
<option key="JPEG Quality" value="'.$displaySess['jpegQuality'].'" />
<option key="RDP Image Encoding" value="'.$displaySess['winComp'].'" />
<option key="RDP JPEG Quality" value="'.$displaySess['rdpJpgQuality'].'" />
<option key="RDP optimization for low-bandwidth link" value="false" />
<option key="Reduce colors to" value="" />
<option key="Use PNG Compression" value="true" />
<option key="VNC JPEG Quality" value="'.$displaySess['vncJpg'].'" />
<option key="VNC images compression" value="'.$displaySess['compType'].'" />
</group>
';
}
else
{
echo'
<group name="Images" >
<option key="Disable JPEG Compression" value="0" />
<option key="Disable all image optimisations" value="false" />
<option key="Disable backingstore" value="false" />
<option key="Disable composite" value="false" />
<option key="Disable image streaming" value="false" />
<option key="Image Compression Type" value="1" />
<option key="Image Encoding Type" value="0" />
<option key="Image JPEG Encoding" value="false" />
<option key="JPEG Quality" value="6" />
<option key="RDP optimization for low-bandwidth link" value="false" />
<option key="Reduce colors to" value="" />
<option key="Use PNG Compression" value="true" />
<option key="VNC images compression" value="0" />
<option key="Windows Image Compression" value="1" />
</group>
';
}
echo'

<group name="Login" >';
$credentialAccess="";
if(isset($_SESSION['credentialsNX']))
{
 $credentialAccess=$_SESSION['credentialsNX'];
}
else
{
 $credentialAccess=getGuestCredentials();
}

if($credentialAccess == "2") 
{
 $userGuest = "false";
 $guestPsw  = scrambleString($_SESSION['password']);
 $guestUser  = $_SESSION['userName'];
}
elseif(($credentialAccess == "1"))
{
 $userGuest = "true";
 $guestPsw  = "EMPTY_PASSWORD";
 $guestUser  = "";
}
elseif($credentialAccess == "0")
{
 $userGuest = "false";
 $guestPsw  = "";
 $guestUser  = "";
}

echo'
<option key="Auth" value="'.$guestPsw.'" />
<option key="Guest Mode" value="'.$userGuest.'" />
<option key="Guest password" value="" />
<option key="Guest username" value="" />
<option key="Login Method" value="nx" />
<option key="Public Key" value="-----BEGIN DSA PRIVATE KEY-----
MIIBuwIBAAKBgQCXv9AzQXjxvXWC1qu3CdEqskX9YomTfyG865gb4D02ZwWuRU/9
C3I9/bEWLdaWgJYXIcFJsMCIkmWjjeSZyTmeoypI1iLifTHUxn3b7WNWi8AzKcVF
aBsBGiljsop9NiD1mEpA0G+nHHrhvTXz7pUvYrsrXcdMyM6rxqn77nbbnwIVALCi
xFdHZADw5KAVZI7r6QatEkqLAoGBAI4L1TQGFkq5xQ/nIIciW8setAAIyrcWdK/z
5/ZPeELdq70KDJxoLf81NL/8uIc4PoNyTRJjtT3R4f8Az1TsZWeh2+ReCEJxDWgG
fbk2YhRqoQTtXPFsI4qvzBWct42WonWqyyb1bPBHk+JmXFscJu5yFQ+JUVNsENpY
+Gkz3HqTAoGANlgcCuA4wrC+3Cic9CFkqiwO/Rn1vk8dvGuEQqFJ6f6LVfPfRTfa
QU7TGVLk2CzY4dasrwxJ1f6FsT8DHTNGnxELPKRuLstGrFY/PR7KeafeFZDf+fJ3
mbX5nxrld3wi5titTnX+8s4IKv29HJguPvOK/SI7cjzA+SqNfD7qEo8CFDIm1xRf
8xAPsSKs6yZ6j1FNklfu
-----END DSA PRIVATE KEY-----
" />
<option key="User" value="'.$guestUser.'" />
</group>
<group name="Services" >
<option key="Audio" value="false" />
<option key="IPPPort" value="631" />
<option key="IPPPrinting" value="false" />
<option key="Shares" value="false" />
</group>';

if($vncSess['host'] == "")
{
echo'
<group name="VNC Session" >
<option key="Display" value="0" />
<option key="Remember" value="false" />
<option key="Server" value="" />
</group>';
}
else
{
$vncSessPass=$vncSess['password'];
echo'
<group name="VNC Session" >
<option key="Display" value=":'.$vncSess['port'].'" />
<option key="Password" value="'.$vncSessPass.'" />
<option key="Remember" value="'.$vncSess['remember'].'" />
<option key="Server" value="'.$vncSess['host'].'" />
</group>';
}
echo'
<group name="Windows Session" >
<option key="Application" value="'.stripslashes($rdpSess['applicationWindows']).'" />
<option key="Authentication" value="'.$auth.'" />
<option key="Color Depth" value="'.$colorDepth.'" />
<option key="Domain" value="'.$domain.'" />
<option key="Image Cache" value="true" />
<option key="Password" value="'.$rdpPsw.'" />
<option key="Remember" value="'.$rdpRem.'" />
<option key="Run application" value="'.$rdpSess['windowsType'].'" /> 
<option key="Server" value="'.$rdpSess['server'].'" />
<option key="User" value="'.$rdpUser.'" />
</group>
<group name="share chosen" >
<option key="Share number" value="0" />
</group>
</NXClientSettings>';

  return 1;
  }
}
?>
