function openDMVASchedule(){var l=1e-9;function h(v,t){var s=t>=12;if(s){t-=12}var u=Math.floor(t+l);$("#selectHr"+v).get(0).selectedIndex=(u===0)?12:u;$("#selectMin"+v).get(0).selectedIndex=Math.floor(((t-u)*60)/5+l)+1;$("#selectAMPM"+v).get(0).selectedIndex=s?1:0}function o(s){logDebug("schedule clicked: ",s);r.currSelectedSchedule=s;$("#selectRuleType").get(0).selectedIndex=s.ruletype;updateSelection("selectRuleSetting",r.loadSettingNames[s.ruletype],s.loadsetting);$("#selectStartDay").get(0).selectedIndex=s.startday;$("#selectEndDay").get(0).selectedIndex=s.startday;h("start",s.startHour);h("end",s.endHour);$("#inputRuleLabel").val(s.rulelabel);m()}function m(){updateFromCombo(r,"currRuleType","selectRuleType");updateFromCombo(r,"currRuleSetting","selectRuleSetting");updateFromCombo(r,"currStartDay","selectStartDay");i("start");i("end");updateFromInput(r,"currRuleLabel","inputRuleLabel")}function b(){$("#dmvaScheduleContainer").append(map(f,function(t,u){return mkElemClass("div","scheduleBlock next-line scheduleType"+u.ruletype,u.rulelabel).css({top:24+28*u.startHour,left:1+105*(u.startDay+1),width:105*u.numDays-1,height:7*4*u.numHours}).click(function(){o(u)})}))}function i(w){var t=parseDecimal($("#selectHr"+w).val());var v=parseDecimal($("#selectMin"+w).val());var u=$("#selectAMPM"+w).val()==="PM";if(!u&&t===12){t=0}var s=NaN;if($.isNumeric(t)&&$.isNumeric(v)){s=t*60+v;if(u){if(t!==12){s+=12*60}}}logDebug("time"+w,s);r["currTime"+w]=s}function j(t){var s=["-- "].concat(map(range(12),function(u,v){return v*5}));return[mkSelectionElem("selectHr"+t,["-- "].concat(range(1,13)),{selectClass:"input-miniSchedule",change:curry(i,t)}),mkSelectionElem("selectMin"+t,s,{selectClass:"input-miniSchedule",change:curry(i,t)}),mkSelectionElem("selectAMPM"+t,["AM","PM"],{selectClass:"input-miniSchedule",change:curry(i,t)})]}var r={currRuleType:0};var f=[];function q(s,u,t){logDebug("Computing num days for ",u,t);if(u<=6){s.startDay=u;s.numDays=1+t-u}else{if(u===7){s.startDay=0;s.numDays=7}else{if(u===8){s.startDay=0;s.numDays=5}else{if(u===9){s.startDay=5;s.numDays=2}}}}}function k(s){if(s===0){f=[]}$.ajax({url:IPNC.serverURL+"vb.htm?dmvaschedulestart="+(s+1)+"&paratest=dmvascheduleruletype&paratest=dmvascheduleloadsetting&paratest=dmvaschedulestartday&paratest=dmvaschedulestarttime&paratest=dmvascheduleendday&paratest=dmvascheduleendtime&paratest=dmvaschedulerulelabel",success:function(v){var t=parseVBResponse(v);if(!t.dmvaschedulerulelabel.length||t.dmvaschedulerulelabel.length===0){logDebug(f);b();m()}else{var u={ruletype:parseDecimal(t.dmvascheduleruletype),loadsetting:parseDecimal(t.dmvascheduleloadsetting),startday:parseDecimal(t.dmvaschedulestartday),starttime:parseDecimal(t.dmvaschedulestarttime),endday:parseDecimal(t.dmvascheduleendday),endtime:parseDecimal(t.dmvascheduleendtime),rulelabel:t.dmvaschedulerulelabel};q(u,u.startday,u.endday);u.startHour=u.starttime/60;u.endHour=u.endtime/60;u.numHours=(u.endtime-u.starttime)/60;f.push(u);k(s+1)}}})}function e(){var s={};q(s,r.currStartDay,r.currStartDay);logDebug("Curr days",s);var t=false;logDebug("Rule label",r.currRuleLabel);$.each(f,function(z,E){var D=E.rulelabel==r.currRuleLabel;if(!t&&!D){logDebug("Checking with",E);var v=s.startDay+s.numDays-1;var w=(E.startDay<=v&&E.startDay>=s.startDay);var B=(E.endday<=v&&E.endday>=s.startDay);var C=w||B;logDebug("days overlap",C);var A=(E.starttime<=r.currTimeend&&E.starttime>=r.currTimestart);var x=(E.endtime<=r.currTimeend&&E.endtime>=r.currTimestart);var u=(E.starttime<=r.currTimestart&&E.endtime>=r.currTimeend);var y=A||x||u;logDebug("times overlap",y);t=C&&y}});return t}function n(s){return $(s).val()}function g(){var u=r.loadSettingNames[r.currRuleType][r.currRuleSetting];if(u==="NA"){return{err:true,msg:"Please select rule settings"}}var w=n("#selectHrstart");if(isNaN(w)){return{err:true,msg:"Please select start hour"}}var s=n("#selectMinstart");if(isNaN(s)){return{err:true,msg:"Please select start minute"}}var v=n("#selectHrend");if(isNaN(v)){return{err:true,msg:"Please select end hour"}}var t=n("#selectMinend");if(isNaN(t)){return{err:true,msg:"Please select end minute"}}if(r.currRuleLabel.length===0){return{err:true,msg:"Please enter rule label."}}if(r.currRuleSetting===-1){return{err:true,msg:"Save Failed. Rule setting is not specified."}}if(r.currTimeend<=r.currTimestart){return{err:true,msg:"Save Failed.<br/> Start and End time are incorrect (End time is less than/equal to start time).<br/> Please re-enter start and endtime."}}if(f.length===10){return{err:true,msg:"Save Failed.<br>Schedule list full.<br>Please delete existing rule(s)"}}if(e()){return{err:true,msg:"Save Failed.<br/> Conflict with existing rule. Please re-enter the rule."}}else{return{err:false}}}function d(){var s=g();if(s.err){showMessageModal("Message",s.msg)}else{var t={dmvascheduleloadsetting:r.currRuleSetting,dmvaschedulerulelabel:r.currRuleLabel,dmvascheduleruletype:r.currRuleType,dmvaschedulestartday:r.currStartDay,dmvaschedulestarttime:r.currTimestart,dmvascheduleendday:r.currStartDay,dmvascheduleendtime:r.currTimeend};$.ajax({url:IPNC.serverURL+"vb.htm",data:t,success:function(u){showMessageModal("Message","Schedule added successfully",c)}})}}function a(){if(r.currSelectedSchedule){var s=r.currSelectedSchedule.rulelabel;$.ajax({url:IPNC.serverURL+"vb.htm?dmvascheduledelrule="+s,success:function(t){c()}})}else{showOverlaidMessageModal("Message","Select a rule first")}}function c(){r.currSelectedSchedule=null;var s=mkElemClass("div","well form spanSchedule3",[mkSelectionInput("Rule Type","selectRuleType",r.ruleTypeNames,{selectClass:"input-large",change:function(){updateFromCombo(r,"currRuleType","selectRuleType");var w=r.loadSettingNames[r.currRuleType];updateSelection("selectRuleSetting",w);r.currRuleSetting=w.length>0?0:-1}}),mkSelectionInput("Rule Setting","selectRuleSetting",r.loadSettingNames[0],{selectClass:"input-medium",change:curry(updateFromCombo,r,"currRuleSetting","selectRuleSetting")}),mkBreak(),mkBreak(),mkSelectionInput("Start Time","selectStartDay",r.dayNames,{selectClass:"input-medium",change:function(){updateFromCombo(r,"currStartDay","selectStartDay");updateSelection("selectEndDay",r.dayNames,r.currStartDay)}}),j("start"),mkBreak(),mkBreak(),mkSelectionInput("End Time","selectEndDay",r.dayNames,{selectClass:"input-medium",disabled:true}),j("end"),mkBreak(),mkBreak(),mkInput({label:"Rule Label",id:"inputRuleLabel","class":"input-large",maxlength:20,change:curry(updateFromInput,r,"currRuleLabel","inputRuleLabel")}),mkButton("Add Rule",null,d),mkBreak(),mkButton("Delete Rule",null,a)]);var u=map(r.dayNames.slice(0,7),function(x,w){return{label:w,options:{"class":"emptyTd"}}});var t=mkElemClass("div","well form spanSchedule9",[mkElem("label",{},"Schedule Analyics for IPCAM "),mkElem("div",{id:"dmvaScheduleContainer"}),mkTable([{label:mkElem("div",{},"Time"),options:{"class":"timeTd"}}].concat(u),map(range(24),function(x,w){return map(range(8),function(B,A){var z=(w%12===0)?12:(w%12);var y=(B===0)?(z+" "+(w<12?"AM":"PM")):null;return{label:mkElem("div",{},y),options:{"class":(B===0)?"timeTd":"emptyTd"}}})}),{tableClass:"dmvaScheduleTable headerDark headerCentered"})]);var v=mkElemClass("div","form-actions",[mkButton("Cancel").click(openDMVASchedule),"  ",mkButton("Return to Event Monitor").click(openSmartAnalytics)]);$("#mainbar").html(mkElemClass("div",null,[mkElem("h2",{},"DMVA Analytics Schedule"),mkElemClass("div","row-fluid",[s,t]),v]));validateAlphaNumericField("#inputRuleLabel",curry(filterSizeField,"#inputRuleLabel",20,false));r.currRuleSetting=-1;r.currRuleLabel="";r.currRuleType=0;r.currStartDay=0;r.currTimestart=NaN;r.currTimeend=NaN;k(0)}function p(s){return $.extend({},s,{ruleTypeNames:s.dmvascheduleruletypenames.split(/;/g),dayNames:s.dmvascheduledaynames.split(/;/g),loadSettingNames:map(s.dmvascheduleloadsettingnames.split(/@/g),function(t,u){return u.split(/;/g)})})}$.ajax({url:IPNC.serverURL+"ini.htm",success:function(s){r=p(parseINI(s));c()}})};