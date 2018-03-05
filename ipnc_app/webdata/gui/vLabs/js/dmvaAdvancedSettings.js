var dmvaAdvSettings={};var origDmvaAdvSettings={};function saveDmvaAdvSettings(a){sendSettings(dmvaAdvSettings,["dmvaoverlaypropertiesfonttype","dmvaoverlaypropertiesfontsize","dmvaoverlaypropertiesfontcolor","dmvaoverlaypropertiesboxcolor","dmvaoverlaypropertiescentriodcolor","dmvaoverlaypropertiesactivezonecolor","dmvaoverlaypropertiesinactivezonecolor","dmvaoverlaypropertiesvelocitycolor","dmvaenvironmentprofileload","dmvaenvironmentprofilestore","dmvaenvironmentprofileclear","dmvaenvironmentprofilestate","dmvaobjectmetadata"],curry(delayedExec,a))}function openDmvaAdvancedSettings(){var g=["red","yellow","lime","blue","pink","cyan","black","white","darkcoral","teal","dim","maroon","darkscarlet","peru","forestgreen","green","violet","indigo","darkblue","darkviolet","darkolive","fractel"];var f=g.slice(0,8);function e(n){var m="Complete Environment profile to help calibrate analytics for camera's present scene and operating conditions";return mkElemClass("div","",createTab("tabSettingsInfo","Settings Info","TargetsettingsInfoWindow",m))}function d(n,m){return mkElemClass("div","imgSize",mkElem("img",{src:"img/"+n+".png","class":g[m]+" imgSize"}))}function b(){return mkTable(["True","Environmental Description"],[[mkCheckButtonElem("","true1Id",{disabled:true}),"Ths size of moving objects varies substantiallywithin the camera's field of view"],[mkCheckButtonElem("","true2Id",{disabled:true})," Glare from sunlight amd other bright light sourcemay appear in the camera's field of view"],[mkCheckButtonElem("","true3Id",{disabled:true}),"Camera's automatic gain control is ON"],[mkCheckButtonElem("","true4Id",{disabled:true}),"Camera is prome to substantial vibration"],[mkCheckButtonElem("","true5Id",{disabled:true}),"The ground plane is visible in the camera's field of view"]],{tableClass:"actionControlTable headerLight"})}var k=mkElem("div",{id:"rightPanel","class":"span5"},[mkElemClass("div","",createTab("tabEnvironment","Environment Profile","tabStreamTargetEnvironmentPrfl",b())),mkBreak(),e("settingInfo")]);function c(){var m=mkElemId("div","metadataTabContent",[mkCheckButtonElem("Bounding Box","boundingBoxId",{checked:isBitSet(dmvaAdvSettings.dmvaobjectmetadata,0),change:function(n){dmvaAdvSettings.dmvaobjectmetadata=changeBitOf(dmvaAdvSettings.dmvaobjectmetadata,0,n)}}),mkBreak(),mkCheckButtonElem("Centroid","centroidId",{checked:isBitSet(dmvaAdvSettings.dmvaobjectmetadata,4),change:function(n){dmvaAdvSettings.dmvaobjectmetadata=changeBitOf(dmvaAdvSettings.dmvaobjectmetadata,4,n)}}),mkBreak(),mkCheckButtonElem("Centroid Velocity","centroidVelocityId",{disabled:true}),mkBreak(),mkCheckButtonElem("Object ID","objectId",{disabled:true}),mkBreak(),mkBreak(),mkBreak(),mkBreak(),mkBreak(),mkBreak()]);return mkElem("div",{id:"metadataDispSettings","class":"span4"},createTab("tabMetadataDispSettings","DMVA Advanced Settings","tabStreamTargetdmvaSettings",m))}function j(q,n){function p(s,r){return{options:{"class":r,id:q+r}}}var m=n.length;var o=m/2;return mkTable([],[map(n.slice(0,o),p),map(n.slice(o,m),p)],{tableClass:"actionControlTable",id:q})}function a(r,n,o,m,q,p){$("#"+o).html(j(r,g));$("#"+o).show();logDebug("in change image function1");$.each(g,function(t,s){var u=r+s;$("#"+u).click(function(){$("#"+o).hide();dmvaAdvSettings[m]=t+1;$("#"+n).html(d(q,t));videoManager.setProperty(p,'"'+getRGB(t+1)+'"',"dmvaAdv")})})}function l(r,n,o,m,q,p){$("#"+o).html(j(r,f));$("#"+o).show();$.each(f,function(t,s){var u=r+s;$("#"+u).click(function(){$("#"+o).hide();videoManager.play(videoSettings.getStreamUrl(0));dmvaAdvSettings[m]=t+1;$("#"+n).html(d(q,t));var w=getRGB(dmvaAdvSettings.dmvaoverlaypropertiesactivezonecolor);var x=getRGB(dmvaAdvSettings.dmvaoverlaypropertiesinactivezonecolor);var v=w+","+x;logDebug("avtInact colors...",v);videoManager.setProperty("zoneColor",v,"dmvaAdv")})})}function i(){var m=mkElem("div",{id:"fontsId"},[mkInlineSelectionInput("Font  ","fontId",dmvaAdvSettings.fontTypes,{selectClass:"input-medium",selectedIndex:dmvaAdvSettings.dmvaoverlaypropertiesfonttype,change:curry(updateFromCombo,dmvaAdvSettings,"dmvaoverlaypropertiesfonttype","fontId")}),mkInlineSelectionInput("Font Size  ","fontSizeId",dmvaAdvSettings.fontSizes,{selectClass:"input-mini",selectedIndex:dmvaAdvSettings.dmvaoverlaypropertiesfontsize,change:curry(updateFromCombo,dmvaAdvSettings,"dmvaoverlaypropertiesfontsize","fontSizeId")}),mkElem("div",{"class":"form-inline"},[mkElem("label",{id:"fcolor"},"Font Color"),mkElem("label",{id:"fontColor"},d("fontcolor",dmvaAdvSettings.dmvaoverlaypropertiesfontcolor-1)),mkElem("label",{id:"fontColorTbl"})]),mkElem("div",{"class":"form-inline"},[mkElem("label",{id:"actcolor"},"Active Zone Color"),mkElem("label",{id:"activeZoneColor"},d("boxcolor",dmvaAdvSettings.dmvaoverlaypropertiesactivezonecolor-1)),mkElem("label",{id:"activeZoneColorTbl"})]),mkElem("div",{"class":"form-inline"},[mkElem("label",{id:"bcolor"},"Inactive Zone Color"),mkElem("label",{id:"inActiveZoneColor"},d("boxcolor",dmvaAdvSettings.dmvaoverlaypropertiesinactivezonecolor-1)),mkElem("label",{id:"inActiveZoneColorTbl"})]),mkElem("div",{"class":"form-inline"},[mkElem("label",{id:"bcolor"},"Box Color"),mkElem("label",{id:"boxColor"},d("boxcolor",dmvaAdvSettings.dmvaoverlaypropertiesboxcolor-1)),mkElem("label",{id:"boxColorTbl"})]),mkElem("div",{"class":"form-inline"},[mkElem("label",{id:"ccolor"},"Centriod Color"),mkElem("label",{id:"centriodColor"},d("centriodcolor",dmvaAdvSettings.dmvaoverlaypropertiescentriodcolor-1)),mkElem("label",{id:"centriodColorTbl"})]),mkElem("div",{"class":"form-inline"},[mkElem("label",{id:"vcolor"},"Velocity Color"),mkElem("label",{id:"velocityColor"},d("velocitycolor",dmvaAdvSettings.dmvaoverlaypropertiesvelocitycolor-1)),mkElem("label",{id:"velocityColorTbl"})])]);return mkElem("div",{id:"overlayId","class":"span7"},createTab("tabOverLaySettings","Overlay Properties","tabStreamTargetOverlayProperties",m))}function h(m){return $.extend({},m,{fontTypes:m.dmvaoverlaypropertiesfonttypelist.split(/;/g),fontSizes:m.dmvaoverlaypropertiesfontsizelist.split(/;/g)})}$.ajax({url:IPNC.serverURL+"ini.htm",success:function(p){var q=parseINI(p);videoSettings=initVideoSettings(q);origDmvaAdvSettings=h(q);dmvaAdvSettings=$.extend({},origDmvaAdvSettings);var n=mkElem("div",{id:"leftPanel","class":"span7 row-fluid"},[centerWrap(mkElem("div",{id:"videoDmvaAdv"})),c(),i()]);var o=mkElemClass("div","form-horizontal form-inline row-fluid",[n,k]);var m=mkElemClass("div","form-actions",[mkButton("Save"," ",mkVerboseSaver(saveDmvaAdvSettings,"DMVA Advanced Settings saved to "+dmvaAdvSettings.title,openDmvaAdvancedSettings))," ",mkButton("Return to Event Monitor").click(openSmartAnalytics)]);$("#smartRightPanel").hide();$("#smartLeftPanel").hide();$("#mainbar").html(mkElem("div",{},[mkElem("h2",{},"DMVA Advanced Settings"),mkBreak(),o,mkBreak(),mkSaveStatusDiv(),m]));$("#mainbar .nav-tabs a").tab("show");$("#fontColor").click(function(){$("#activeZoneColorTbl").hide();$("#velocityColorTbl").hide();$("#inActiveZoneColorTbl").hide();$("#boxColorTbl").hide();$("#centriodColorTbl").hide();dmvaAdvSettings.clicked=0;a("fc","fontColor","fontColorTbl","dmvaoverlaypropertiesfontcolor","fontcolor","fontColor")});$("#activeZoneColor").click(function(){$("#velocityColorTbl").hide();$("#inActiveZoneColorTbl").hide();$("#fontColorTbl").hide();$("#boxColorTbl").hide();$("#centriodColorTbl").hide();l("azc","activeZoneColor","activeZoneColorTbl","dmvaoverlaypropertiesactivezonecolor","boxcolor","zoneColor")});$("#inActiveZoneColor").click(function(){$("#activeZoneColorTbl").hide();$("#velocityColorTbl").hide();$("#fontColorTbl").hide();$("#boxColorTbl").hide();$("#centriodColorTbl").hide();l("inazc","inActiveZoneColor","inActiveZoneColorTbl","dmvaoverlaypropertiesinactivezonecolor","boxcolor","zoneColor")});$("#boxColor").click(function(){$("#activeZoneColorTbl").hide();$("#velocityColorTbl").hide();$("#inActiveZoneColorTbl").hide();$("#fontColorTbl").hide();$("#centriodColorTbl").hide();a("bc","boxColor","boxColorTbl","dmvaoverlaypropertiesboxcolor","boxcolor","rectColor")});$("#centriodColor").click(function(){$("#activeZoneColorTbl").hide();$("#velocityColorTbl").hide();$("#inActiveZoneColorTbl").hide();$("#fontColorTbl").hide();$("#boxColorTbl").hide();a("cc","centriodColor","centriodColorTbl","dmvaoverlaypropertiescentriodcolor","centriodcolor","centroidColor")});$("#velocityColor").click(function(){$("#activeZoneColorTbl").hide();$("#inActiveZoneColorTbl").hide();$("#fontColorTbl").hide();$("#boxColorTbl").hide();$("#centriodColorTbl").hide();a("vc","velocityColor","velocityColorTbl","dmvaoverlaypropertiesvelocitycolor","velocitycolor","velocityColor")});showPendingStatus();addVideoDiv(dmvaAdvSettings,"#videoDmvaAdv",320,240,"dmvaAdv");setOverlayProperties(dmvaAdvSettings,"dmvaAdv");playFirstStream()}})};