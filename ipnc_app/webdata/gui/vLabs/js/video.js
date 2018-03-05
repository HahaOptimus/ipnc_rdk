function initVideoSettings(a){return $.extend({},a,{streamTypes:a.videocodecname.split(/;/g),codecTypeMaster:a.videocodeccomboname.split(/@/g),resolutionMaster:a.videocodecresname.split(/@/g),frameRateNameMaster:$.map(range(3),function(b){return[a["frameratenameall"+(b+1)].split(/@/g)]}),logoPositions:a.logopositionname.split(/;/g),textPositions:a.textpositionname.split(/;/g),dateTimePositions:a.datetimepositionname.split(/;/g),rateControlNames:a.ratecontrolname.split(/;/g),meConfigs:a.meconfigname.split(/;/g),localDisplayNames:a.localdisplayname.split(/;/g),mirrorControlNames:a.mirctrlname.split(/;/g),aviFormatNames:a.aviformatname.split(/;/g),aviDurationNames:a.avidurationname.split(/;/g),getCodecTypes:function(){return this.codecTypeMaster[this.videocodec].split(/;/g)},getAbsoluteCodecIndex:function(){var c=0,b=0;for(b=0;b<this.videocodec;b+=1){c+=this.codecTypeMaster[b].split(/;/g).length}return c+this.videocodeccombo},getResolutions:function(){return this.resolutionMaster[this.getAbsoluteCodecIndex()].split(/;/g)},getFrameRates:function(c){var b=this.frameRateNameMaster[c];var e=b[this.getAbsoluteCodecIndex()].split(/;/g),d=e[this.videocodecres].split(/,/g);return d},getCodec:function(c){var b=this.getCodecTypes()[this.videocodeccombo].split(/\+/g)[c];return b==="H264"?"H.264":b},hasBitRate:function(b){return this.getCodec(b).indexOf("JPEG")<0},getOrigUrlName:function(b){return this["streamname"+b].split(/@/g)},getStreamName:function(b){var d=Math.min(this.videocodec,b);var c=this.getOrigUrlName(d+1);return c[0]},getStreamResolutionStr:function(b){var c=this.getStreamName(b);var d=c.match(/\([0-9]+x[0-9]+\)$/);return d?d[0]:""},getStreamUrl:function(b){var d=Math.min(this.videocodec,b);var c=this.getOrigUrlName(d+1);return c[1]+"="+this.getCodec(d)}})}var origVideoSettings={},videoSettings={};function openVideo(){var c=getPluginWidthAndHeight(videoSettings,getMainBarHalfAvailableSpace(),1);function e(k){var i=k+1;function l(p){return origVideoSettings[p+i]}function m(q,u,t,s){var r=t+i;var p=u+i;return mkInlineCheckButtonElem(q,p,{checked:parseDecimal(origVideoSettings[r])===1,change:function(w){var v=w?1:0;videoSettings[r]=v;if(s){s(w)}}})}var j="framerate"+i;var o="select"+j;var n=videoSettings.hasBitRate(k);return[mkSelectionInput("Framerate",o,videoSettings.getFrameRates(k),{selectClass:"input-mini",help:"fps",selectedIndex:l("framerate"),change:curry(updateFromCombo,videoSettings,j,o)}),n?mkInput({label:"Bit Rate",help:" Kbps","class":"input-mini",maxlength:5,value:videoSettings["bitrate"+i],id:"inputBitRate"+i}):mkInput({label:"Quality Factor","class":"input-micro",maxlength:2,value:videoSettings.livequality,id:"inputQualityFactor"}),n?mkSelectionInput("Rate Control","selectRateControl"+i,origVideoSettings.rateControlNames,{selectClass:"input-small",selectedIndex:l("ratecontrol"),change:curry(updateFromCombo,videoSettings,"ratecontrol"+i,"selectRateControl"+i)}):null,mkElemClass("div","form-inline",mkFieldSet("Overlay setting",[m("Date","checkDate","datestampenable"),space(),m("Time","checkTime","timestampenable"),newLine(),m("Logo","checkLogo","logoenable",curry(enableInput,"#selectLogoPosition"+i)),mkInlineSelectionInput("Logo Position ","selectLogoPosition"+i,videoSettings.logoPositions,{disabled:videoSettings["logoenable"+i]===0,selectClass:"input-medium",selectedIndex:origVideoSettings["logoposition"+i],change:curry(updateFromCombo,videoSettings,"logoposition"+i,"selectLogoPosition"+i)}),newLine(),m("Text","checkText","textenable",curry(enableInput,"#stream"+k+" .text-setting")),space(),mkInputElem({disabled:videoSettings["textenable"+i]===0,id:"inputOverlayText"+i,value:origVideoSettings["overlaytext"+i],maxlength:20,"class":"input-small text-setting",change:curry(updateFromInput,videoSettings,"overlaytext"+i,"inputOverlayText"+i)}),mkInlineSelectionInput("Text Position ","selectTextPosition"+i,videoSettings.textPositions,{disabled:videoSettings["textenable"+i]===0,selectClass:"input-medium text-setting",selectedIndex:origVideoSettings["textposition"+i],change:curry(updateFromCombo,videoSettings,"textposition"+i,"selectTextPosition"+i)}),newLine(),m("Detailed Info","checkDetailedInfo","detailinfo")]))]}function g(){updateFromCombo(videoSettings,"videocodec","selectStreamType");videoSettings.videocodeccombo=0;h()}function a(){updateFromCombo(videoSettings,"videocodeccombo","selectCodec");h()}function f(){updateFromCombo(videoSettings,"videocodecres","selectResolution");h()}function h(){updateSelection("selectStreamType",videoSettings.streamTypes,videoSettings.videocodec);updateSelection("selectCodec",videoSettings.getCodecTypes(),videoSettings.videocodeccombo);updateSelection("selectResolution",videoSettings.getResolutions(),videoSettings.videocodecres);$("#tabContainer").html(mkTabbable("streamTabs",$.map(range(videoSettings.videocodec+1),function(j){return{label:"Stream "+(j+1),target:"stream"+j,content:e(j),click:function(){c=getPluginWidthAndHeight(videoSettings,getMainBarHalfAvailableSpace(),j+1);addVideoDiv(videoSettings,"#mainVideo",c.width,c.height,"video");videoManager.play(origVideoSettings.getStreamUrl(j))}}})));$.each(range(videoSettings.videocodec+1),function(k){var j=k+1;if(videoSettings.hasBitRate(k)){validateNumericField("#inputBitRate"+j,5,curry(updateFromInput,videoSettings,"bitrate"+j,"inputBitRate"+j))}else{validateNumericField("#inputQualityFactor",2,curry(updateFromInputHandleEmpty,videoSettings,"livequality","inputQualityFactor"))}validateAlphaNumericField("#inputOverlayText"+j)});$("#streamTabs a:first").tab("show")}function d(){logDebug("Updating title");videoSettings.title=$("#inputCamera").val()}function b(){return"Video settings saved to "+videoSettings.title}$.ajax({url:IPNC.serverURL+"ini.htm",success:function(l){origVideoSettings=initVideoSettings(parseINI(l));videoSettings=$.extend({},origVideoSettings);var i=mkElem("div",{id:"leftPanel","class":"span6"},[mkPrependedInput("Camera",{id:"inputCamera",value:videoSettings.title,maxlength:11},"icon-camera",d),mkSelectionInput("StreamType","selectStreamType",videoSettings.streamTypes,{selectedIndex:videoSettings.videocodec,change:g}),mkSelectionInput("Codec","selectCodec",videoSettings.getCodecTypes(),{selectedIndex:videoSettings.videocodeccombo,change:a}),mkSelectionInput("Resolution","selectResolution",videoSettings.getResolutions(),{selectedIndex:videoSettings.videocodecres,change:f}),mkElem("div",{id:"tabContainer"})]);var j=mkElem("div",{id:"rightPanel","class":"span5"},[centerWrap(mkElem("div",{id:"mainVideo"})),mkElemClass("div","form-horizontal row-fluid",mkElemClass("fieldset","",[mkRadioButtons(mkElemClass("label","control-label disabled","Encrypt Video"),"selectEncryption",[{label:"On",disabled:true},{label:"Off",disabled:true}],{}),mkSelectionInput("Local Display Video","selectLocalDisp",videoSettings.localDisplayNames,{selectedIndex:videoSettings.localdisplay,change:curry(updateFromCombo,videoSettings,"localdisplay","selectLocalDisp")}),mkSelectionInput("Mirror","selectMirror",videoSettings.mirrorControlNames,{selectedIndex:videoSettings.mirctrl,change:curry(updateFromCombo,videoSettings,"mirctrl","selectMirror")}),mkElem("h3",{},"Video File"),mkSelectionInput("Stream","selectFileStream",videoSettings.aviFormatNames,{selectedIndex:videoSettings.aviformat,change:curry(updateFromCombo,videoSettings,"aviformat","selectFileStream")}),mkSelectionInput("Video Size","selectFileVideoSize",videoSettings.aviDurationNames,{selectedIndex:videoSettings.aviduration,change:curry(updateFromCombo,videoSettings,"aviduration","selectFileVideoSize")})]))]);var k=mkElemClass("div","form-horizontal row-fluid",[i,j]);var m=mkElemClass("div","form-actions",[mkButton("Ok","btn-primary",mkVerboseSaver(saveVideoSettings,b,openVideo))," ",mkButton("Cancel",null,openVideo)," ",mkButton("Advanced","btn-success",curry(closeVideo,openVideoAdvanced))]);$("#mainbar").html(mkElem("div",{},[mkElem("h2",{},"Video"),mkUpdateModal(),k,mkSaveStatusDiv(),m]));addVideoDiv(videoSettings,"#mainVideo",c.width,c.height,"video");h();videoManager.play(origVideoSettings.getStreamUrl(0));adjustSidebarHeight();showPendingStatus()}})}function saveVideoSettings(d){if(videoSettings.livequality<2||videoSettings.livequality>97){showOverlaidMessageModal("Message","Quality factor should range from 2 to 97");return}var b=["framerate","datestampenable","timestampenable","logoenable","logoposition","textenable","textposition","overlaytext","detailinfo"];function a(f,g){var e=g.slice(0);if(videoSettings.hasBitRate(f-1)){e.push("bitrate"+f);e.push("ratecontrol"+f)}else{e.push("livequality")}$.each(b,function(j,h){e.push(h+f)});return e}var c=curry(checkReload,function(f,e){if(f){videoManager.stopAndHide();$("#updateModal").modal({backdrop:"static",keyboard:false});new modalProgress(e,function(){$("#updateModal").modal("hide");setTimeout(d,100)})}else{setTimeout(d,100)}});sendSettings(videoSettings,["videocodec","videocodeccombo","videocodecres","mirctrl","aviformat","aviduration"],function(){sendSettings(videoSettings,a(1,["title","encryptvideo","localdisplay"]),function(){if(videoSettings.videocodec>=1){sendSettings(videoSettings,a(2,["livequality"]),function(){if(videoSettings.videocodec>=2){sendSettings(videoSettings,a(3,[]),c)}else{c()}})}else{c()}})})}function closeVideo(a){if(isDifferent(origVideoSettings,videoSettings)){videoManager.hideAll();showConfirmationModal("Message","Do you want to save the changes?",curry(saveVideoSettings,a),a,true)}else{closeAdvancedVideo(a)}};