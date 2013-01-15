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

//
// Check the site from which to download the NX Client in relation to the O.S.
//










function siteDownload()
{
  
  if(this.document.getElementById('NoMachineLink'))
  {
    if(navigator.userAgent.toLowerCase().indexOf("windows") != -1)
      this.document.getElementById('NoMachineLink').href = 'http://www.nomachine.com/download-client-windows.php';
    else if((navigator.userAgent.toLowerCase().indexOf("linux") != -1) || (navigator.userAgent.indexOf("Unix") != -1))
      this.document.getElementById('NoMachineLink').href = 'http://www.nomachine.com/download-client-linux.php';
    else if(navigator.userAgent.toLowerCase().indexOf("mac") != -1)
      this.document.getElementById('NoMachineLink').href = 'http://www.nomachine.com/download-client-macosx.php';
    else if ((navigator.userAgent.toLowerCase().indexOf("solaris") != -1) || (navigator.userAgent.toLowerCase().indexOf("sunos") != -1))
      this.document.getElementById('NoMachineLink').href = 'http://www.nomachine.com/download-client-solaris.php';
    }  
}


function disableLogin()
{
  if(this.document.Login.userName.disabled==false)
  {
    this.document.Login.userName.disabled=true;
    this.document.Login.password.disabled=true; 
    this.document.getElementById('nameText').style.color='#A1A5A9';
    this.document.getElementById('pwdText').style.color='#A1A5A9';     
  }
  else
  {
    this.document.Login.userName.disabled=false;
    this.document.Login.password.disabled=false;      
    this.document.getElementById('nameText').style.color='#514E53';
    this.document.getElementById('pwdText').style.color='#514E53';     
  }
}




function showInfo(divId)
{
  if (divId.style.display=='block') 
    divId.style.display='none';
  else
    divId.style.display='block';
}


function showSessions(server)
{
  selectOptions();
  val=server.split("-");
  server=val[0];
  if (val[1]!='')
    icon=val[1];
  else 
    icon='imgEmpty.gif';  
  this.document.getElementById('serverIcon').src="images/shared/builder/imgEmpty.gif";
  l=this.document.guest.length;
  for(i=0;i<l;i++)
  {
    if(this.document.guest.elements[i].name.indexOf("sessionServer-") != -1)
      if (this.document.getElementById(this.document.guest.elements[i].name).style.display == 'block')
        this.document.getElementById(this.document.guest.elements[i].name).style.display='none';
  }
  this.document.getElementById('sessionServer-'+server).style.display='block';
  this.document.getElementById('serverIcon').src="images/shared/servers/"+icon;
}



function showIcon(idSession)
{
  this.document.guest.icon.disabled=false;
  this.document.getElementById('uploadFileIcon').disabled=false;
  this.document.getElementById('assignFileIcon').disabled=false;
  this.document.getElementById('categoryText').disabled=false;
  this.document.guest.file1.disabled=false;
  
  sessionName=this.document.getElementById('nameSession'+idSession).value.split('-');
  app=sessionName[1].split('/'); 
  this.document.getElementById('sessionTitle').innerHTML="&nbsp;&nbsp;"+sessionName[0]+"-"+app[(app.length)-1];
  
  icon=this.document.guest.icon;
  image=this.document.getElementById('hiddenIcon'+idSession).value;
  if(image == '')
    image='empty.gif';
  l=icon.length;
  for(i=0;i<l;i++)
  {
    if (icon.options[i].value==image)
    {
      icon.options[i].selected=true;
    } 
  }
  this.document.getElementById('iconSessionEmpty').style.display='none';
  this.document.getElementById('iconSession'+idSession).style.display='block';
  this.document.getElementById('b_browse').src="images/buttons/b_browse.gif";
  //
  // display select fields 
  //
  l=this.document.guest.length;
  for(i=0;i<l;i++)
  {
    if(this.document.guest.elements[i].name.indexOf("sessionServer-") != -1)
      if (this.document.getElementById(this.document.guest.elements[i].name).style.display == 'block')
        this.document.getElementById(this.document.guest.elements[i].name).style.display='none';
  }
  divName='sessionServer-'+sessionName[0];
  this.document.getElementById(divName).style.display='block';
  l=this.document.guest.sessionServer.length;
  for(i=0;i<l;i++)
  {
    if (this.document.guest.sessionServer.options[i].value.indexOf(sessionName[0])!=-1)
    {
      this.document.guest.sessionServer.options[i].selected=true;
    } 
  }
  
}


function showServerIcon(icon)
{
  this.document.getElementById('serverIcon').src='images/servers/'+icon;
  this.document.getElementById('imgServer').value=icon;
}


function setDeskTop(chooser) 
{

 var session = new Object()
 session["unix"] = [
                     {value:"kde", text:"KDE"},
                     {value:"gnome", text:"GNOME"},
                     {value:"cde", text:"CDE"},
                     {value:"xdm", text:"XDM"},
                     {value:"custom", text:"Custom"}
                   ];
                   
 session["windows"] = [{value:"rdp", text:"RDP"}];
 session["vnc"]     = [{value:"vnc", text:"RFB"}];
 session["shadow"]  = [{value:"nx", text:"NX"}];

 var newElem;
 var where = (navigator.appName == "Microsoft Internet Explorer") ? -1 : null;
 var sessionChooser = chooser.form.elements["deskType"];
 
 while (sessionChooser.options.length) 
 {
     sessionChooser.remove(0);
 }
 var choice = chooser.options[chooser.selectedIndex].value;
 var SettingButton = document.getElementById('DeskSettingButton');
 
 var sessionElem = session[choice];

 for (var i = 0; i < sessionElem.length; i++) 
 {
   newElem = document.createElement("option");
   newElem.text = sessionElem[i].text;
   newElem.value = sessionElem[i].value;
   sessionChooser.add(newElem, where);
   
   if(choice!="unix")
   {
    sessionChooser.disabled="disabled";
    
    if(choice=="shadow")
    {
     SettingButton.disabled="disabled";
    }
    else
    {
     SettingButton.disabled="";
    }
    
   }
   else
   {
     sessionChooser.disabled="";
     
     if((choice=="shadow")||(choice=="unix"))
     {
      SettingButton.disabled="disabled";
     }
     else
     {
      SettingButton.disabled="";
     }     
   }
 }
}

function enableSetting(choose)
{
  var buttonSetting= document.getElementById('DeskSettingButton');
  
  if( (choose=='xdm') || (choose=='custom') )
  {
    buttonSetting.disabled="";
  }
  else 
  {
    buttonSetting.disabled="disabled";
  } 
} 


function disableAction()
{
if (self.innerHeight) // all except Explorer
{
	h = self.innerHeight;
   // alert('first h'+h);
}
else if (document.documentElement && document.documentElement.clientHeight) // Explorer 6 Strict Mode
{
	h = document.documentElement.clientHeight;
      disableTab();
   // alert('second h'+h);
   
}
else if (document.body) // other Explorers
{
	h = document.body.clientHeight;
      disableTab();
   // alert('tirdh h'+h);
   
}



 var ghost= document.getElementById('ghost');
 ghost.style.height=h;
 if(ghost.style.display=="none")
 {
  ghost.style.display="block"
 }
 else
 {
  ghost.style.display="none"
 }

}  
  
function disableTab()
{
   var SelectList = document.getElementsByTagName("select");
   var contrary = document.getElementById('deskType');
  //alert("length: "+SelectList.length+" status "+status);
   for(i=0; i < SelectList.length; i++)
  {
   if(SelectList.item(i).disabled == "")
   {
    
    SelectList.item(i).disabled = "disabled";
    //alert('Vedi:'+SelectList.item(i).disabled);
   } 
   else 
   {
     SelectList.item(i).disabled = ""; 
   }
 
} 
 if(contrary.disabled == true )
    {
     contrary.disabled="";
    }
    else
    {
    contrary.disabled="disabled";
    }
  
}    
 



function checkBox(param) 
{
  var listCheck = document.getElementsByTagName("input");
  var selectUserType = document.getElementById('userType');
  
 // alert('id: '+param.id+ ' value: '+selectUserType.value);
  
  //(param.id!="logUserGuest")&&
  if(selectUserType.value!="guest")
  {
    
   for(i=0; i < listCheck.length; i++)
   {
    if(listCheck[i].type == "checkbox") 
    {
     if(param.checked == true)
     {
      listCheck[i].disabled = true;
      param.disabled = false;
     }
     else if(param.checked == false)
     {
      listCheck[i].disabled = false;
     } 
    }
   } 
 }  
}

function setPreview()
{
  var showDiv   = document.getElementById("preview");
        
  showDiv.style.display="block";

  var diffW=document.getElementById('preview').clientWidth;
  var diffH=document.getElementById('preview').clientHeight;
  
  var pLeft = ((document.body.clientWidth-diffW)/2)+"px";
  var pTop  = ((document.body.clientHeight-diffH)/2)+"px";
  
  document.getElementById('preview').style.left=pLeft;
  document.getElementById('preview').style.top="150px";
  
}



function setPanelOption()
{
  var optionValueDesk = document.getElementById('deskType');
  var optionValueSession = document.getElementById('sessionType');


  switch (optionValueDesk.value) 
  {
   case "rdp":
      var showDiv   = document.getElementById("winOption");
      var titleDiv  = document.getElementById('winTitle');
      
      showDiv.style.display="block";
      
      var diffW     = showDiv.clientWidth;
      var diffH     = showDiv.clientHeight;

      showDiv.style.left=((document.body.clientWidth-diffW)/2)+"px";
      showDiv.style.top=((document.body.clientHeight-diffH)/2)+"px";

      titleDiv.innerHTML="RDP - Setting";
   break;

   case "vnc":
      var showDiv   = document.getElementById("vncOption");
      var titleDiv = document.getElementById('vncTitle');
      
      showDiv.style.display="block";
      
      var diffW     = showDiv.clientWidth;
      var diffH     = showDiv.clientHeight;

      showDiv.style.left=((document.body.clientWidth-diffW)/2)+"px";
      showDiv.style.top=((document.body.clientHeight-diffH)/2)+"px";
            
      titleDiv.innerHTML="VNC - Setting";   
   break;

   case "xdm":
      var showDiv   = document.getElementById("xdmOption");
      var titleDiv = document.getElementById('xdmTitle');
      
      showDiv.style.display="block";
      
      var diffW     = showDiv.clientWidth;
      var diffH     = showDiv.clientHeight;

      showDiv.style.left=((document.body.clientWidth-diffW)/2)+"px";
      showDiv.style.top=((document.body.clientHeight-diffH)/2)+"px";      
      
      titleDiv.innerHTML="XDM - Setting"; 
   break;
   
   case "custom":
      var showDiv   = document.getElementById("customOption");
      var titleDiv = document.getElementById('customTitle');
      
      showDiv.style.display="block";
      
      var diffW     = showDiv.clientWidth;
      var diffH     = showDiv.clientHeight;

      showDiv.style.left=((document.body.clientWidth-diffW)/2)+"px";
      showDiv.style.top=((document.body.clientHeight-diffH)/2)+"px";      
      
      titleDiv.innerHTML="Custom - Setting"; 
   break; 
   
   default:
   break;
  }
  
}   


function toUpper(param) {
    var pattern = /(\w)(\w*)/; // a letter, and then one, none or more letters 

    var a = param.split(/\s+/g); // split the sentence into an array of words

    for (i = 0 ; i < a.length ; i ++ ) {
        var parts = a[i].match(pattern); // just a temp variable to store the fragments in.

        var firstLetter = parts[1].toUpperCase();
        var restOfWord = parts[2].toLowerCase();

        a[i] = firstLetter + restOfWord; // re-assign it back to the array and move on
    }
    
   return a.join(' '); // join it back together
}
function setPanelOptionOther(choose)
{
var titleSpan="";

 
  switch (choose) 
  {
   case "proxy":        
      var showDiv   = document.getElementById("proxyOption");
      var titleDiv = document.getElementById('proxyTitle');
      
      showDiv.style.display="block";
      
      var diffW     = showDiv.clientWidth;
      var diffH     = showDiv.clientHeight;

      showDiv.style.left=((document.body.clientWidth-diffW)/2)+"px";
      showDiv.style.top=((document.body.clientHeight-diffH)/2)+"px";
      
      titleDiv.innerHTML="HTTP Proxy - Setting";
   break;

   case "displayCustom":        

      typeDesk=document.getElementById('sessionType');
       
      titleSpan=toUpper(typeDesk.value);  
      
      if(typeDesk.value=="vnc")
      {
       var showDiv   = document.getElementById("vncCustomSetting");
       var titleDiv =  document.getElementById('displayVncCustomTitle'); 
          
      }
      else if(typeDesk.value=="windows")
      {
       var showDiv   = document.getElementById("winCustomSetting");
       var titleDiv =  document.getElementById('displayWinCustomTitle');      
      }
      else 
      {
       var showDiv   = document.getElementById("unixCustomSetting");
       var titleDiv =  document.getElementById('displayUnixCustomTitle');
       titleSpan="Unix ";
      }
      
      titleDiv.innerHTML=titleSpan+" Display Setting";      
      showDiv.style.display="block";
      
      var diffW     = showDiv.clientWidth;
      var diffH     = showDiv.clientHeight;

      showDiv.style.left=((document.body.clientWidth-diffW)/2)+"px";
      showDiv.style.top=((document.body.clientHeight-diffH)/2)+"px";
      
   break;

  }
  
}   



function sessionList(strURL, optValue, idUser) 
{
    var xmlHttpReq = false;
    var self = this;
    // Xhr per Mozilla/Safari/Ie7
    if (window.XMLHttpRequest) {
        self.xmlHttpReq = new XMLHttpRequest();
    }
    // for all versions of IE
    else if (window.ActiveXObject) {
        self.xmlHttpReq = new ActiveXObject("Microsoft.XMLHTTP");
    }
    self.xmlHttpReq.open('POST', strURL, true);
    self.xmlHttpReq.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    self.xmlHttpReq.onreadystatechange = function() {
        if (self.xmlHttpReq.readyState == 4) {
            updatepageSession(self.xmlHttpReq.responseText, idUser);
        }
    }
    self.xmlHttpReq.send(getquerystringSession(optValue, idUser));
}

function getquerystringSession(optValue,idUser) {

    qstr = 'idServer=' + escape(optValue)+ '&idUser='+ escape(idUser);
    return qstr;
}

function updatepageSession(str,idUser){
    document.getElementById("respose-"+idUser).innerHTML = str;
}




function setDescrition(strURL) 
{
    var xmlHttpReq = false;
    var self = this;
    // Xhr per Mozilla/Safari/Ie7
    if (window.XMLHttpRequest) {
        self.xmlHttpReq = new XMLHttpRequest();
    }
    // for all versions of IE
    else if (window.ActiveXObject) {
        self.xmlHttpReq = new ActiveXObject("Microsoft.XMLHTTP");
    }
    self.xmlHttpReq.open('POST', strURL, true);
    self.xmlHttpReq.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    self.xmlHttpReq.onreadystatechange = function() {
        if (self.xmlHttpReq.readyState == 4) {
            updatepage(self.xmlHttpReq.responseText);
        }
    }
    self.xmlHttpReq.send(getquerystring());
}

function getquerystring() {
  
    var word = document.getElementById('idServer').value;
    qstr = 'idServer=' + escape(word);
    return qstr;
}

function updatepage(str){
    document.getElementById("result").innerHTML = str;
}

function enableSetHttpProxy(elem){
    
    var buttonElem=document.getElementById("setHttpProxy");
    
    if(elem.checked==true)
       buttonElem.disabled=false;
    else
       buttonElem.disabled=true;
}

function enableCommand(status)
{
 var filedCommand=document.getElementById('customCommand');
 
 if(status=="enable")
 {
  filedCommand.disabled=false;
 }
 else
 {
  filedCommand.disabled=true;
 }
 

}

function enableTaint()
{
 
 var disableX=document.getElementById('disableX');
 var taint=document.getElementById('taint');
 
  
 if(disableX.checked==true)
 {
   taint.disabled=false;
 }
 else
 {
  taint.disabled=true;
 }
 

}


function xdmPanel(val)
{
 var XdmPortList=document.getElementById('XdmPortList');
 var XdmDomainList=document.getElementById('XdmDomainList');

 var XdmPortQuery=document.getElementById('XdmPortQuery');
 var XdmDomainQuery=document.getElementById('XdmDomainQuery');

 var XdmPortBroadCast=document.getElementById('XdmPortBroadCast');
    
 
 if(val=="server decide")
 {
  XdmPortList.disabled=true;
  XdmDomainList.disabled=true;
  XdmPortQuery.disabled=true;
  XdmDomainQuery.disabled=true;
  XdmPortBroadCast.disabled=true;
 }
 else if(val=="query")
 {
  XdmPortList.disabled=true;
  XdmDomainList.disabled=true;
  XdmPortQuery.disabled=false;
  XdmDomainQuery.disabled=false;
  XdmPortBroadCast.disabled=true;
 
 }
 else if(val=="broadcast")
 {
  XdmPortList.disabled=true;
  XdmDomainList.disabled=true;
  XdmPortQuery.disabled=true;
  XdmDomainQuery.disabled=true;
  XdmPortBroadCast.disabled=false; 
 }
 else if(val=="list")
 {
  XdmPortList.disabled=false;
  XdmDomainList.disabled=false;
  XdmPortQuery.disabled=true;
  XdmDomainQuery.disabled=true;
  XdmPortBroadCast.disabled=true; 
 }
}




function enableCommandOption(status)
{
 
 var disableX=document.getElementById('disableX');
 var taint=document.getElementById('taint');
 
  
 if(status=="enable")
 {
  disableX.disabled=false;
  
  if(disableX.checked==true)
  {
   taint.disabled=false;
  }
  
  
 }
 else
 {
  disableX.disabled=true;
  taint.disabled=true;
 }
 

}


function rbpOption(status)
{
 
 var wincompMaps=document.getElementById('winCompMaps');
 
  
 if(status=="enable")
 {
   wincompMaps.disabled=false;
 }
 else
 {
   wincompMaps.disabled=true; 
 }
 

}

/*function findPosition( oLink ) {
  if( oLink.offsetParent ) {
    for( var posX = 0, posY = 0; oLink.offsetParent; oLink = oLink.offsetParent ) {
      posX += oLink.offsetLeft;
      posY += oLink.offsetTop;
    }
    return [ posX, posY ];
  } else {
    return [ oLink.x, oLink.y ];
  }
}*/

function setResultPosition(oLink)
{

  var showResult=document.getElementById('result');

  if( oLink.offsetParent ) 
  {
    for( var posX = 0, posY = 0; oLink.offsetParent; oLink = oLink.offsetParent ) 
    {
      posX += oLink.offsetLeft;
      posY += oLink.offsetTop;
    }
  } 
  else 
  {
    posX=oLink.x;
    posY=oLink.y;

  }
  
 
//alert("x "+posX);
//alert("y "+posY);

showResult.style.top = posY+20;
showResult.style.left = posX+20;
}
function setResultPosition2(pos, cont)
{
 var showResult=document.getElementById('result');

 var posDivTop;
 var posDivLeft;
 var delta;
 
 delta=cont*200;
 posDivTop=pos['top']-85;
 posDivLeft=pos['left']-235;
 

 //alert('TOPDIV: '+posDivTop)
 //alert('LEFTDIV: '+posDivLeft)
 
 

 showResult.style.top = pos['top']-posDivTop+20+delta;

 showResult.style.left = pos['left']-posDivLeft+20;

 //alert('TOP DIV2: '+showResult.style.top)
 //alert('LEFT DIV2: '+showResult.style.left)


/*  var showResult=document.getElementById('result');

  if( oLink.offset ) 
  {
    for( var posX = 0, posY = 0; oLink.offset; oLink = oLink.offset ) 
    {
      posX += oLink.offsetLeft;
      posY += oLink.offsetTop;
    }
  } 
  else 
  {
    posX=oLink.x;
    posY=oLink.y;

  }
  
	var curleft = curtop = 0;
	if (obj.offsetParent) {
		curleft = obj.offsetLeft
		curtop = obj.offsetTop
		while (obj = obj.offsetParent) {
			curleft += obj.offsetLeft
			curtop += obj.offsetTop
		}
	}

  
 
 showResult.style.top = curtop+19;
 showResult.style.left = curleft+10;*/
}


function setFontServer()
{
 
 var useFontServer=document.getElementById('useFontServer');
 var fontServerHost=document.getElementById('fontServerHost');
 var fontServerPort=document.getElementById('fontServerPort');
 
 if(useFontServer.checked==true)
 {
  fontServerHost.disabled=false;
  fontServerPort.disabled=false;
 }
 else
 {
  fontServerHost.disabled=true;
  fontServerPort.disabled=true; 
 }
 

}
function setWindowsPanel(param)
{
 var WindowDomain=document.getElementById('WindowDomain'); 
 var usernameWindows=document.getElementById('usernameWindows');
 var passwordWindows=document.getElementById('passwordWindows');
 var RememberPasswordWindows=document.getElementById('RememberPasswordWindows');
  
 if (param=='2')
 {
  WindowDomain.disabled=false;
  usernameWindows.disabled=true;
  passwordWindows.disabled=true;
  RememberPasswordWindows.disabled=true;
 }
 else if(param=='0')
 {
  WindowDomain.disabled=false;
  usernameWindows.disabled=false;
  passwordWindows.disabled=false;
  RememberPasswordWindows.disabled=false;
 }
 else if(param=='1')
 {
  WindowDomain.disabled=true;
  usernameWindows.disabled=true;
  passwordWindows.disabled=true;
  RememberPasswordWindows.disabled=true;
 }

}

function enableDisplaySetting()
{
 
 var CustomDisplaySettings=document.getElementById('CustomDisplaySettings');
 var displaySetting=document.getElementById('displaySetting');
 
 if(CustomDisplaySettings.checked==true)
 {
  displaySetting.disabled=false;
 }
 else
 {
  displaySetting.disabled=true;
 }
}

function enableSlide()
{
 
  var enJpeg=document.getElementById('enableJpegQuality');
   
  var table = document.getElementById("tableSlide"); 
  var cells = table.getElementsByTagName("td"); 
 
 
 
  if(enJpeg.checked==true)
  {
   for (var i = 0; i < cells.length; i++) 
   { 
        cells[i].style.color="#000000"; 
   } 
  }
  else
  {
   for (var i = 0; i < cells.length; i++) 
   { 
    cells[i].style.color="#999999"; 
   } 
  }
}

function setPix()
{
 var shm=document.getElementById('shm');
 var pix=document.getElementById('sharedPix'); 
  
  if(shm.checked==true)
  {
   pix.disabled=true;
  } 
  else
  {
   pix.disabled=false;
  }
}
function setSlide(param)
{
 var idTable='tableSlide-'+param; 
 var table = document.getElementById(idTable); 
 var cells = table.getElementsByTagName("td");    
 
   for (var i = 0; i < cells.length; i++) 
   { 
     cells[i].style.color="#000000"; 
   }   
}

function unSetSlide(param)
{
 var idTable='tableSlide-'+param; 
 var table = document.getElementById(idTable); 
 var cells = table.getElementsByTagName("td");    
 
   for (var i = 0; i < cells.length; i++) 
   { 
     cells[i].style.color="#999999"; 
   }   
}

function disableRadio(param)
{
   for (var i = 0; i < 10; i++) 
   { 
    document.getElementById(param+i).disabled = true;     
   }
}

function enableRadio(param)
{
   for (var i = 0; i < 10; i++) 
   { 
    document.getElementById(param+i).disabled = false;     
   }
 
}

function slideSettingKonq(check,param)
{
 var checkId=document.getElementById(check);
 var paramId=document.getElementById(param);
 checkId.disabled=false;
 
 if(checkId.checked==true)
 {
  setSlide(check);
  enableRadio(param);
 }
 else
 {
  unSetSlide(check);
  disableRadio(param);
 } 
}

function slideSetting(param,nSlide)
{
 var check=document.getElementById(param);
 check.disabled=false; 
  
  if(check.checked==true)
  {
    setSlide(param);
    if(nSlide == '1')
    {
    sliderJpgRdp.setDisabled(false);
    }
    else if(nSlide == '0')
    {
     sliderUnix.setDisabled(false);
    }
    else if(nSlide == '2')
    {
     sliderJpgVnc.setDisabled(false);
    }
  }
  else
  {
    unSetSlide(param);
    if(nSlide == '1')
    {
     sliderJpgRdp.setDisabled(true);
    }
    else if(nSlide == '0')
    {
     sliderUnix.setDisabled(true);
    }
    else if(nSlide == '2')
    {
     sliderJpgVnc.setDisabled(true);
    }
  }
}

function disableSlide(param)
{
  var enJpeg=document.getElementById(param);
  var idTable='tableSlide-'+param; 
  
  var table = document.getElementById(idTable); 
  var cells = table.getElementsByTagName("td");  
  var jpegQuality=document.getElementById('jpegQuality'); 
 
  jpegQuality.value="6";
  enJpeg.disabled=true;
  unSetSlide(param);

}

function setValueJpg(qual, param)
{
  var quality = document.getElementById(qual);
  quality.value = param;
}

function sendSessionForm(param)
{
 var form = document.getElementById('SessionForm');
 var fieldHidden=document.getElementById('operation').value=param;
 form.submit();
}

function sendFormTo(id, url)
{
 var form = document.getElementById(id);
 form.action=url;
 form.submit();
}


function createSession(idForm, idSession, idServer)
{
 var form = document.getElementById(idForm);
 var hiddenSession=document.getElementById('createSessionId');
 var hiddenServer=document.getElementById('createServerId');
 
 hiddenSession.value=idSession;
 hiddenServer.value=idServer;
 
 form.submit();
}

function showSessionIcon(icon)
{
  var id=this.document.getElementById('serverIcon').src='images/sessions/'+icon;
}

function getSetupForm(strURL, type) 
{
    var xmlHttpReq = false;
    var self = this;
    // Xhr per Mozilla/Safari/Ie7
    if (window.XMLHttpRequest) {
        self.xmlHttpReq = new XMLHttpRequest();
    }
    // for all versions of IE
    else if (window.ActiveXObject) {
        self.xmlHttpReq = new ActiveXObject("Microsoft.XMLHTTP");
    }
    self.xmlHttpReq.open('POST', strURL, true);
    self.xmlHttpReq.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded');
    self.xmlHttpReq.onreadystatechange = function() {
        if (self.xmlHttpReq.readyState == 4) {
            updatepageSetup(self.xmlHttpReq.responseText);
        }
    }
    self.xmlHttpReq.send(getquerystringSetup(type));
}

function getquerystringSetup(type) {
  
    qstr = 'type=' + escape(type);
    return qstr;
}

function updatepageSetup(str){
document.getElementById("contentSetup").innerHTML = str;
}


//
// These functions visualizes or hides some fields in AddServer and
// ModifyServer. 
//
  
/*function displaySuspendText()
{
  if(this.document.getElementById('suspendText').style.display=='block')
    this.document.getElementById('suspendText').style.display = 'none';
  else
    this.document.getElementById('suspendText').style.display='block';
}
*/
function displaySuspendText(path,ids,id)
{
 var idBox=document.getElementById(id);
 var idSpan=document.getElementById(ids);
 //var idImage=document.getElementById(idm);
 var display = idBox.style.display;
 //var imgSrc = idImage.src;
 //var newImg="";
 var Up='<img style="cursor: pointer" src="'+path+'/b_orderOff.png" onClick="displaySuspendText(\''+path+'\',\'changeArrow\',\'suspendText\');" onmouseover="this.src=\''+path+'/b_orderOff_over.png\'" onmouseout="this.src=\''+path+'/b_orderOff.png\'" >';
 var Down='<img style="cursor: pointer" src="'+path+'/b_orderOnDown.png" onClick="displaySuspendText(\''+path+'\',\'changeArrow\',\'suspendText\');" onmouseover="this.src=\''+path+'/b_orderOnDown_over.png\'" onmouseout="this.src=\''+path+'/b_orderOnDown.png\'" >';
 
  idBox.style.display = (display == "block") ? "none" : "block";
  //newImg = (display == "block") ? path+"/b_orderOff.png" : path+"/b_orderOnDown.png"; 
  //idImage.src=newImg;
  idSpan.innerHTML = (display == "block") ? Up : Down ;
}
function displayCreateCategory()
{
 if(this.document.getElementById('tableCreateCategory').style.display=='block')
    this.document.getElementById('tableCreateCategory').style.display = 'none';
  else
    this.document.getElementById('tableCreateCategory').style.display='block';
}


function disableText()
{
  var displayText=document.getElementById('displayText');
  var noDisplay=document.getElementById('noDisplay');
  var substitutiveText=document.getElementById('substitutiveText');
  var replaceText=document.getElementById('replaceText');
  var noDisplayText=document.getElementById('noDisplayText');

  displayText.checked=false;
  substitutiveText.disabled=true;
  replaceText.style.color='#A1A5A9';
  noDisplayText.style.color='#514E53';
  
  if(displayText.checked==true)
  {
   substitutiveText.disabled=false;
   
  }
  else if(displayText.checked==false)
  {
    substitutiveText.disabled=true;
    replaceText.style.color='#000000';
  } 
}


function disableNoDisplay()
{
  this.document.addServer.noDisplay.checked=false;
  this.document.getElementById('noDisplayText').style.color='#A1A5A9';
  this.document.getElementById('replaceText').style.color='#514E53';
  this.document.addServer.substitutiveText.disabled=false;
}

function setLoginFiels(elem)
{ 
 var userName=document.getElementById('userName');
 var passWord=document.getElementById('password');
 if(elem.checked==true)
 {
  userName.disabled=true;
  passWord.disabled=true;  
 }
 else
 {
  userName.disabled=false;
  passWord.disabled=false;  
 
 }

}

function enableApplicationWindows(param)
{
 var runAppWin = document.getElementById("applicationWindows");
 if (param == '0') 
 {
  runAppWin.disabled = true;
 }
 else if (param == '1') 
 {
  runAppWin.disabled = false;
 }
}


