(function(){

   if (typeof JSROOT != "object") {
      var e1 = new Error("analysiseditor.js requires JSROOT to be already loaded");
      e1.source = "analysiseditor.js";
      throw e1;
   }

   if (typeof GO4 != "object") {
      var e1 = new Error("analysiseditor.js requires GO4 to be already loaded");
      e1.source = "analysiseditor.js";
      throw e1;
   }


   GO4.EvIOType = {
          GO4EV_NULL: 0,                // no event store/source
          GO4EV_FILE: 1,                // root file with own tree
          GO4EV_TREE: 2,                // branch of singleton tree
          GO4EV_MBS_FILE: 3,             // mbs listmode file (input only)
          GO4EV_MBS_STREAM: 4,           // mbs stream server (input only)
          GO4EV_MBS_TRANSPORT: 5,        // mbs transport server (input only)
          GO4EV_MBS_EVENTSERVER: 6,      // mbs event server  (input only)
          GO4EV_MBS_REVSERV: 7,           // remote event server (input only)
          GO4EV_BACK: 8,            // backstore in memory (pseudo-ringbuffer?)
          GO4EV_USER: 9,             // user defined source class
          GO4EV_MBS_RANDOM: 10            // random generated mbs event

      }

   GO4.AnalysisStatusEditor = function(stat) {
      JSROOT.TBasePainter.call(this, stat);
      this.stat = stat;
      this.step;
      this.changes = [["dummy0", "init0"],["dummy1","init1"]];  // changes array stepwise, index 0 = no step, index = stepindex+1
      this.showmore= [false, false];
      this.ClearChanges();
      this.ClearShowstates();
   }


   GO4.AnalysisStatusEditor.prototype = Object.create(JSROOT.TBasePainter.prototype);

   GO4.AnalysisStatusEditor.prototype.DabcCommand = function(cmd, option, callback) {
      var pre="";
      if (this.GetItemName()!="") { // note: check against !=null does not work here!
           pre = this.GetItemName() + "/"; // suppress / if item name is empty
            //console.log("Found non null itemname= -"+this.GetItemName()+"-");
      }
      pre +="exe.json\?method=";
      var fullcom = pre + cmd + option;

      JSROOT.NewHttpRequest(fullcom, 'text', function(res) {
         console.log("DabcCommand completed.");
         callback(res!=null);
      }).send(null);
   }

  GO4.AnalysisStatusEditor.prototype.MarkChanged = function(key, step) {
        // first avoid duplicate keys:
        var index;

        for   (index = 0; index < this.changes[step].length; index++) {
            if (this.changes[step][index]== key) return;
         }
        this.changes[step].push(key);
        console.log("Mark changed :%s at step %d", key, step);
        var id = "#"+this.divid;

        $(id+" .buttonAnaChangeLabel").show();// show warning sign
     }

     // clear changed elements' list, make warning sign invisible
GO4.AnalysisStatusEditor.prototype.ClearChanges = function() {
   var index, step;
   var numsteps=this.changes.length;
   for   (step = 0; step < numsteps ; step++) {
      var len=this.changes[step].length;
      for   (index = 0; index < len ; index++) {
         var removed=this.changes[step].pop();
         //console.log("Clear changes removed :%s at step %d",removed, step);
      }
   }
   var id = "#"+this.divid;
   $(id+" .buttonAnaChangeLabel").hide(); // hide warning sign

     }

GO4.AnalysisStatusEditor.prototype.ClearShowstates = function() {
   var index;
   var len=this.showmore.length;
   for   (index = 0; index < len ; index++) {
      var removed=this.showmore.pop();
       console.log("ClearShowstates removed :%s", removed);
   }
}

//scan changed value list and return optionstring to be send to server
GO4.AnalysisStatusEditor.prototype.EvaluateChanges = function(optionstring) {
   var id = "#"+this.divid;
   var editor=this;
   var index;
   var numsteps=this.changes.length;
   for   (step = 0; step < numsteps ; step++) {
      var len=this.changes[step].length;
      var stepoptions="";
      for   (index = 0; index < len ; index++) {

      var key=this.changes[step][index];
      //console.log("Evaluate change key:%s", key);

      var theElement=editor.stat.fxStepArray.arr[step];
      // here mapping of key to editor field:
      if(key=="stepenabled")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fbProcessEnabled;
      }
      else if(key=="sourceenabled")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fbSourceEnabled;
      }
      else if(key=="storeenabled")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fbStoreEnabled;
      }
      else if(key=="sourcesel")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fxSourceType.fiID;
      }
      else if(key=="sourcename")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fxSourceType.fName;
      }
      else if(key=="sourceport")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fxSourceType.fiPort;
      }
      else if(key=="sourcetmout")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fxSourceType.fiTimeout;
      }
      else if(key=="sourceretry")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fxSourceType.fiRetryCnt;
      }
      else if(key=="sourcefirst")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fxSourceType.fuStartEvent;
      }
      else if(key=="sourcelast")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fxSourceType.fuStopEvent;
      }
      else if(key=="sourceskip")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fxSourceType.fuEventInterval;
      }
      else if(key=="storesel")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fxStoreType.fiID;
      }
      else if(key=="storename")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fxStoreType.fName;
      }
      else if(key=="storesplit")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fxStoreType.fiSplit;
      }
      else if(key=="storebuf")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fxStoreType.fiBufsize;
      }
      else if(key=="storecomp")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fxStoreType.fiCompression;
      }
      else if(key=="storeasf")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fxStoreType.fiAutosavesize;
      }
      else if(key=="storeover")
      {
         stepoptions+="&"+key+"_"+step+"="+theElement.fxStoreType.fbOverwrite;
      }
      // non step specific options are in step 0 options too:

      else if(key=="asfname")
      {
         stepoptions+="&"+key+"="+editor.stat.fxAutoFileName;
      }

      else if(key=="asfenabled")
      {
         stepoptions+="&"+key+"="+editor.stat.fbAutoSaveOn;
      }

      else if(key=="asftime")
      {
         stepoptions+="&"+key+"="+editor.stat.fiAutoSaveInterval;
      }
      else if(key=="asfcomp")
      {
         stepoptions+="&"+key+"="+editor.stat.fiAutoSaveCompression;
      }
      else if(key=="asfoverwrite")
      {
         stepoptions+="&"+key+"="+editor.stat.fbAutoSaveOverwrite;
      }
      else if(key=="anaprefsname")
      {
         stepoptions+="&"+key+"="+editor.stat.fxConfigFileName;
      }

      else{
         console.log("Warning: EvaluateChanges found unknown key:%s", key);
      }



   }// for index


      optionstring+=stepoptions;
   } // for step
   console.log("Resulting option string:%s", optionstring);
   return optionstring;
}








   GO4.AnalysisStatusEditor.prototype.refreshEditor = function()
   {
      var id = "#"+this.divid;
      var editor=this;
      var stat=this.stat;
      var names = "";

      ///////////// ANALYSIS STEPS:
      this.ClearShowstates();
      var tabelement=$(id+" .steptabs");
      tabelement.tabs( "option", "disabled", [0, 1, 2, 3, 4, 5, 6, 7] );
      for(var j=0; j<8;++j){
         $(id +" .steptabs ul:first li:eq("+ j +")").hide(); // disable and hide all tabs
      }
      stat.fxStepArray.arr.forEach(function(element, index, array) {
         tabelement.tabs("enable",index);
         $(id +" .steptabs ul:first li:eq("+index+") a").text(element.fName);
         $(id +" .steptabs ul:first li:eq("+index+")").show(); // only show what is really there
         editor.showmore.push(false); // prepare showmore array for each step
         //console.log("refreshEditor for step name:"+ element.fName);
         tabelement.tabs("load",index); // all magic is in the on load event callback
         //console.log("refreshEditor after issuing load of index:"+ index);


      }); // for each

      /////////////////// AUTO SAVE FILE:
      $(id + " .anaASF_name").val(stat.fxAutoFileName);
      $(id+" .anaASF_enabled")
                .prop('checked', stat.fbAutoSaveOn)
                .click(function() {
                   editor.MarkChanged("asfenabled",0);
                    editor.stat.fbAutoSaveOn=this.checked;
                    });


      $(id +" .anaASF_time").val(stat.fiAutoSaveInterval);
      $(id +" .anaASF_compression").val(stat.fiAutoSaveCompression);

      $(id+" .anaASF_overwrite")
             .prop('checked', stat.fbAutoSaveOverwrite)
             .click(function() {
                editor.MarkChanged("asfoverwrite",0);
               editor.stat.fbAutoSaveOverwrite= this.checked;
             });

      ////////////////// PREFS FILE:
      $(id + " .anaprefs_name").val(stat.fxConfigFileName);


      //console.log("analysis editor: refreshEditor");
      editor.ClearChanges();

   }


   GO4.AnalysisStatusEditor.prototype.showStepEditor = function(pthis, theElement, theIndex)

   {
      var id = "#"+this.divid;
      var editor=this;
      var showmore=editor.showmore[theIndex];
      //console.log("showStepEditor for index "+theIndex+" has showmore="+showmore);
       var storetable=pthis.find(" .step_store");
       var sourcetable=pthis.find(" .step_source");
       var enablebox=pthis.find(" .step_box_step_enab");
       var sourcebox=pthis.find(" .step_box_source_enab");
       var storebox=pthis.find(" .step_box_store_enab");

       var sourcesel=pthis.find(" .step_source_select");
       var sourcemore=pthis.find(" .step_source_expand");
       var sourceform=pthis.find(" .step_source_form");
       var sourcename=pthis.find(" .step_source_name");
       var sourcenamelabel=pthis.find(" .step_source_name_label");
       var sourcetag=pthis.find(" .step_source_tagfile");
       var sourcetaglabel=pthis.find(" .step_source_tagfile_label");
       var sourceport=pthis.find(" .step_source_port");
       var sourceportlabel=pthis.find(" .step_source_port_label");
       var sourcetmout=pthis.find(" .step_source_tmout");
       var sourcetmoutlabel=pthis.find(" .step_source_tmout_label");
       var sourceretry=pthis.find(" .step_source_retry");
       var sourceretrylabel=pthis.find(" .step_source_retry_label");
       var sourceargs=pthis.find(" .step_source_args");
       var sourceargslabel=pthis.find(" .step_source_args_label");
       var sourcefirst=pthis.find(" .step_source_firstev");
       var sourcefirstlabel=pthis.find(" .step_source_firstev_label");
       var sourcelast=pthis.find(" .step_source_lastev");
       var sourcelastlabel=pthis.find(" .step_source_lastev_label");
       var sourceskip=pthis.find(" .step_source_stepev");
       var sourceskiplabel=pthis.find(" .step_source_stepev_label");

       var storesel=pthis.find(" .step_store_select");
       var storename=pthis.find(" .step_store_name");
       var storesplit=pthis.find(" .step_store_split");
       var storebuf=pthis.find(" .step_store_buf");
       var storecomp=pthis.find(" .step_store_comp");
       var storetreeasf=pthis.find(" .step_store_asf");
       var storeover=pthis.find(" .step_store_overwrite");

    // here step control checkboxes and source/store visibility:
      if (theElement.fbProcessEnabled) {
            sourcebox.prop('disabled',false);
            storebox.prop('disabled',false);
            if (theElement.fbSourceEnabled) {
               sourcetable.show();
            } else {
               sourcetable.hide();
            }
            if (theElement.fbStoreEnabled) {
               storetable.show();
            } else {
               storetable.hide();
            }
         } else {
            sourcebox.prop('disabled',true);
            storebox.prop('disabled',true);
            sourcetable.hide();
            storetable.hide();
         }




       sourceform.show();

       //console.log("show step editor with source id:"+theElement.fxSourceType.fiID);
       switch(theElement.fxSourceType.fiID)
               {
               case GO4.EvIOType.GO4EV_FILE:
               case GO4.EvIOType.GO4EV_MBS_RANDOM:
                  sourceport.hide();
                  sourceportlabel.hide();
                  sourcetmout.hide();
                  sourcetmoutlabel.hide();
                  sourceretry.hide();
                  sourceretrylabel.hide();
                  sourcetag.hide();
                  sourcetaglabel.hide();
                  sourcefirst.hide();
                  sourcefirstlabel.hide();
                  sourcelast.hide();
                  sourcelastlabel.hide()
                  sourceskip.hide();
                  sourceskiplabel.hide();
                  sourceargs.hide();
                  sourceargslabel.hide();
               break;
               case GO4.EvIOType.GO4EV_MBS_STREAM:
               case GO4.EvIOType.GO4EV_MBS_TRANSPORT:
               case GO4.EvIOType.GO4EV_MBS_EVENTSERVER:
               case GO4.EvIOType.GO4EV_MBS_REVSERV:
                  if(showmore)
                     {
                        sourceport.show();
                        sourceportlabel.show();
                         sourcetmout.show();
                         sourcetmoutlabel.show();
                         sourceretry.show();
                         sourceretrylabel.show();
                         sourcefirst.show();
                         sourcefirstlabel.show();
                         sourcelast.show();
                         sourcelastlabel.show();
                         sourceskip.show();
                         sourceskiplabel.show();
                     }
                  else
                     {
                         sourceport.hide();
                         sourceportlabel.hide();
                         sourcetmout.hide();
                         sourcetmoutlabel.hide();
                         sourceretry.hide();
                         sourceretrylabel.hide();
                        sourcefirst.hide();
                        sourcefirstlabel.hide();
                        sourcelast.hide();
                        sourcelastlabel.hide();
                        sourceskip.hide();
                        sourceskiplabel.hide();
                     }
                  sourcetag.hide();
                  sourcetaglabel.hide();
                  sourceargs.hide();
                  sourceargslabel.hide();


               break;
               case GO4.EvIOType.GO4EV_USER:
                  if(showmore)
                  {
                     sourceport.show();
                     sourceportlabel.show();
                     sourcetmout.show();
                     sourcetmoutlabel.show();
                     sourceargs.show();
                     sourceargslabel.show();
                  }
                  else
                  {
                     sourceport.hide();
                     sourceportlabel.hide();
                     sourcetmout.hide();
                     sourcetmoutlabel.hide();
                     sourceargs.hide();
                     sourceargslabel.hide();
                  }
                    sourceretry.hide();
                 sourceretrylabel.hide();
                    sourcetag.hide();
                    sourcetaglabel.hide();
                    sourcefirst.hide();
                    sourcefirstlabel.hide();
                    sourcelast.hide();
                    sourcelastlabel.hide();
                    sourceskip.hide();
                    sourceskiplabel.hide();
               break;
               default:
                console.log("showStepEditor WARNING: unknown event source id: "+theElement.fxSourceType.fiID);
                case GO4.EvIOType.GO4EV_MBS_FILE:
                 if(showmore)
                {
                    sourcetag.show();
                    sourcetaglabel.show();
                    sourcefirst.show();
                    sourcefirstlabel.show();
                    sourcelast.show();
                    sourcelastlabel.show();
                    sourceskip.show();
                    sourceskiplabel.show();
                }
                 else
                  {
                    sourcetag.hide();
                    sourcetaglabel.hide();
                    sourcefirst.hide();
                    sourcefirstlabel.hide();
                    sourcelast.hide();
                    sourcelastlabel.hide();
                    sourceskip.hide();
                    sourceskiplabel.hide();
                }
                sourceport.hide();
                sourceportlabel.hide();
                sourcetmout.hide();
                sourcetmoutlabel.hide();
                sourceretry.hide();
                sourceretrylabel.hide();
                sourceargs.hide();
                sourceargslabel.hide();
                break;
               };


      storesplit.show();
   storebuf.show();
   storecomp.show();
   storetreeasf.show();
   storeover.show();
   //console.log("show step editor with store id:"+theElement.fxStoreType.fiID);
   switch(theElement.fxStoreType.fiID)
      {
      default:
            console.log("showStepEditor WARNING: unknown event store id: "+theElement.fxStoreType.fiID);
      case GO4.EvIOType.GO4EV_FILE:
         storecomp.spinner("enable");
         storetreeasf.spinner("enable");
         storeover.prop('disabled',false);
          break;
      case GO4.EvIOType.GO4EV_BACK:
         storecomp.spinner("disable");
         storetreeasf.spinner("disable");
         storeover.prop('disabled',true);
          break;

      };


      sourcesel.selectmenu("option", "width", sourcetable.width()*0.8); // expand to table width
      sourcesel.selectmenu('refresh', true);

      storesel.selectmenu("option", "width", storetable.width()*0.8); // expand to table width
      storesel.selectmenu('refresh', true);


      pthis.css("padding","5px");

      $(id+" .steptabs").tabs("refresh");

      //console.log("analysis editor: showStepEditor leaving.");
   }

   GO4.AnalysisStatusEditor.prototype.fillEditor = function()
   {
      var id = "#"+this.divid;
      var editor=this;

      $(id +" .steptabs").tabs({
           heightStyle: "fill",
           activate : function(event, ui) {
              //updateElementsSize();
              //console.log("analysis editor: activated tab: "+ ui.newTab.text());
           },

           load: function(event, ui) {

              // note that load will also be triggered when activating tab!
              // so we need to backup all changes in local step status theElement!!!

              var theIndex = ui.tab.index();
              //console.log("On load function for "  + ui.tab.text() + " index=" + theIndex );
              var pthis=ui.panel;
              var theElement=editor.stat.fxStepArray.arr[theIndex];

//              console.log("process enabled="+theElement.fbProcessEnabled + "for theElement: "+theElement.fName);
//              console.log("source enabled="+theElement.fbSourceEnabled + "for theElement: "+theElement.fName);
//              console.log("store enabled="+theElement.fbStoreEnabled + "for theElement: "+theElement.fName);

              var storetable=pthis.find(" .step_store");
              var sourcetable=pthis.find(" .step_source");
              var enablebox=pthis.find(" .step_box_step_enab");
              var sourcebox=pthis.find(" .step_box_source_enab");
              var storebox=pthis.find(" .step_box_store_enab");

              var sourcesel=pthis.find(" .step_source_select");
              var sourcemore=pthis.find(" .step_source_expand");
              var sourceform=pthis.find(" .step_source_form");
              var sourcename=pthis.find(" .step_source_name");
              var sourcetag=pthis.find(" .step_source_tagfile");
              var sourceport=pthis.find(" .step_source_port");
              var sourcetmout=pthis.find(" .step_source_tmout");
              var sourceretry=pthis.find(" .step_source_retry");
              var sourceargs=pthis.find(" .step_source_args");
              var sourcefirst=pthis.find(" .step_source_firstev");
              var sourcelast=pthis.find(" .step_source_lastev");
              var sourceskip=pthis.find(" .step_source_stepev");

              var storesel=pthis.find(" .step_store_select");
              var storename=pthis.find(" .step_store_name");
              var storesplit=pthis.find(" .step_store_split");
              var storebuf=pthis.find(" .step_store_buf");
              var storecomp=pthis.find(" .step_store_comp");
              var storetreeasf=pthis.find(" .step_store_asf");
              var storeover=pthis.find(" .step_store_overwrite");


              enablebox.prop('checked', theElement.fbProcessEnabled)
                .click(function()
                      {
                         editor.MarkChanged("stepenabled", theIndex);
                         theElement.fbProcessEnabled=this.checked;
                         editor.showStepEditor(pthis, theElement, theIndex);

                }); // clickfunction

                sourcebox.prop('checked', theElement.fbSourceEnabled)
                .click(function()
                      {
                         editor.MarkChanged("sourceenabled", theIndex);
                         theElement.fbSourceEnabled=this.checked;
                         editor.showStepEditor(pthis, theElement, theIndex);
                }); // clickfunction


                  storebox.prop('checked', theElement.fbStoreEnabled)
                  .click(function()
                        {
                           editor.MarkChanged("storeenabled", theIndex);
                           theElement.fbStoreEnabled=this.checked;
                           editor.showStepEditor(pthis, theElement, theIndex);
                 }); // clickfunction

            //// EVENT SOURCE: /////////////////////////////////////////////////
               sourcesel.selectmenu({
            change : function(event, ui) {
               editor.MarkChanged("sourcesel",theIndex);
               // change here eventsource status object?!
               // in javascript we can just add dynamically any missing members!
               // so exchange of class object is not necessary hopefully...

               switch(Number(ui.item.value))
               {

                  case 0:
                        theElement.fxSourceType.fiID=GO4.EvIOType.GO4EV_FILE;
                     break;
                     case 2:
                           theElement.fxSourceType.fiID=GO4.EvIOType.GO4EV_MBS_STREAM;
                     break;
                     case 3:
                           theElement.fxSourceType.fiID=GO4.EvIOType.GO4EV_MBS_TRANSPORT;
                       break;
                     case 4:
                        theElement.fxSourceType.fiID=GO4.EvIOType.GO4EV_MBS_EVENTSERVER;
                       break;
                     case 5:
                        theElement.fxSourceType.fiID=GO4.EvIOType.GO4EV_MBS_REVSERV;
                       break;
                     case 6:
                        theElement.fxSourceType.fiID=GO4.EvIOType.GO4EV_MBS_RANDOM;
                        break;
                     case 7:
                        theElement.fxSourceType.fiID=GO4.EvIOType.GO4EV_USER;
                        break;
                     default:
                     case 1:
                           theElement.fxSourceType.fiID=GO4.EvIOType.GO4EV_MBS_FILE;
                           break;

               }; // switch

               // but: we have to set back all values  from GUI to theElement and optionally create new members:
               theElement.fxSourceType.fName=sourcename.val();
               theElement.fxSourceType.fiPort=sourceport.val();
               theElement.fxSourceType.fiTimeout=sourcetmout.val();
               theElement.fxSourceType.fiRetryCnt=sourceretry.val();
               theElement.fxSourceType.fxTagFile=sourcetag.val();
               theElement.fxSourceType.fuStartEvent=sourcefirst.val();
               theElement.fxSourceType.fuStopEvent=sourcelast.val();
               theElement.fxSourceType.fuEventInterval=sourceskip.val();
               theElement.fxSourceType.fxExpression=sourceargs.val();

               editor.showStepEditor(pthis, theElement, theIndex);
            }
         }); // source selectmenu change


            sourcemore.prop('checked', editor.showmore[theIndex]).click(
                  function(){
                     //console.log("show more clickfunction...");
                     var doshow=$(this).prop('checked');
                     if (doshow) {
                     editor.showmore[theIndex]=true;
               } else {
                  editor.showmore[theIndex]=false;
               }
                  editor.showStepEditor(pthis, theElement, theIndex);
                  });  // clickfunction


          sourcename.val(theElement.fxSourceType.fName);
//          .change(function(){
//         editor.MarkChanged("sourcename",theIndex);
//         theElement.fxSourceType.fName=this.value.trim();
//         }); ;

          sourceform.submit(
               function(event) {
                  event.preventDefault(); // do not send automatic request to server!
                  var content= sourcename[0].value;
                  content=content.trim();
                  editor.MarkChanged("sourcename",theIndex);
                  theElement.fxSourceType.fName=content;
                  console.log("Submitting sourcename form with: "+content);
               });


         //console.log("on tab load finds source name: "+ theElement.fxSourceType.fName);
//         sourcename.val(theElement.fxSourceType.fName)
//            .change(function(){
//               editor.MarkChanged("sourcename",theIndex);
//               theElement.fxSourceType.fName=this.value.trim();
//               });





            sourceport.spinner({
               min: 0,
               max: 100000,
               step: 1,
              stop: function( event, ui ) {
                 editor.MarkChanged("sourceport",theIndex);
                 theElement.fxSourceType.fiPort=this.value;
                 //console.log("spinner stop event with thisvalue="+this.value+", ui.value="+ui.value);
              }
           });

         sourcetmout.spinner({
              min: 0,
              max: 9999,
              step: 1,
             stop: function( event, ui ) {
                editor.MarkChanged("sourcetmout",theIndex);
                theElement.fxSourceType.fiTimeout=this.value;
             }


            });

         sourceretry.spinner({
           min: 0,
           max: 10000,
           step: 1,
          stop: function( event, ui ) {
             editor.MarkChanged("sourceretry",theIndex);
             theElement.fxSourceType.fiRetryCnt=this.value;
             }
       });


        sourcefirst.spinner({
           min: 0,
           max: 2000000000,
           step: 1000,
          stop: function( event, ui ) {
             editor.MarkChanged("sourcefirst",theIndex);
             theElement.fxSourceType.fuStartEvent=this.value;
          }
       });

        sourcelast.spinner({
           min: 0,
           max: 2000000000,
           step: 1000,
          stop: function( event, ui ) {
             editor.MarkChanged("sourcelast",theIndex);
             theElement.fxSourceType.fuStopEvent=this.value;
          }
       });

        sourceskip.spinner({
           min: 0,
           max: 999999999,
           step: 1,
          stop: function( event, ui ) {
             editor.MarkChanged("sourceskip",theIndex);
             theElement.fxSourceType.fuEventInterval=this.value;
          }
       });

         //// EVENT STORE: /////////////////////////////////////////////////
            storesel.selectmenu({
                 change : function(event, ui) {
                    editor.MarkChanged("storesel",theIndex);

                 //console.log("store selector with value "+ Number(ui.item.value));
                 switch(Number(ui.item.value))
               {

                    default:
                    case 0:
                        theElement.fxStoreType.fiID=GO4.EvIOType.GO4EV_FILE;
                        break;
                        case 1:
                           theElement.fxStoreType.fiID=GO4.EvIOType.GO4EV_BACK;
                           break;
               }; // switch
               theElement.fxStoreType.fName=storename.val();
               theElement.fxStoreType.fiSplit=storesplit.val();
               theElement.fxStoreType.fiBufsize=storebuf.val()* 1000.;
               theElement.fxStoreType.fiCompression=storecomp.val();
               theElement.fxStoreType.fiAutosavesize=storetreeasf.val();



               editor.showStepEditor(pthis, theElement, theIndex);

                 } // change function
              }); // selectmenu


         //console.log("on tab load finds store name: "+ theElement.fxStoreType.fName);
         storename.val(theElement.fxStoreType.fName)
          .change(function(){
             editor.MarkChanged("storename",theIndex);
             theElement.fxStoreType.fName=this.value.trim();
          }); // change function


         storesplit.spinner({
           min: 0,
           max: 99,
           step: 1,
          stop: function( event, ui ) {
             editor.MarkChanged("storesplit",theIndex);
             theElement.fxStoreType.fiSplit=this.value;
             }
       });

         storebuf.spinner({
           min: 4,
           max: 256,
           step: 1,
          stop: function( event, ui ) {
             editor.MarkChanged("storebuf",theIndex);
             theElement.fxStoreType.fiBufsize=this.value * 1000;
             }
       });

         storecomp.spinner({
           min: 0,
           max: 9,
           step: 1,
          stop: function( event, ui ) {
             editor.MarkChanged("storecomp",theIndex);
             theElement.fxStoreType.fiCompression=this.value;
             }
       });
         storetreeasf.spinner({
             min: 0,
             max: 99999,
             step: 100,
            stop: function( event, ui ) {
               editor.MarkChanged("storeasf",theIndex);
               theElement.fxStoreType.fiAutosavesize=this.value;
               }
         });

       storeover.click(function() {
         editor.MarkChanged("storeover",theIndex);
        theElement.fxStoreType.fbOverwrite=this.checked;
          });


         ////////////////// here set event source values:



            // set event source selector and special fields:
         //console.log("load tab "+theIndex+" sees source id:"+theElement.fxSourceType.fiID);
            switch(theElement.fxSourceType.fiID)
            {
            case GO4.EvIOType.GO4EV_FILE:
            sourcesel.val(0);
            break;
            case GO4.EvIOType.GO4EV_MBS_STREAM:
            sourcesel.val(2);
            sourceport.val(theElement.fxSourceType.fiPort);
            sourcetmout.val(theElement.fxSourceType.fiTimeout);
            sourceretry.val(theElement.fxSourceType.fiRetryCnt);
            sourcetag.text(theElement.fxSourceType.fxTagFile);
             sourcefirst.val(theElement.fxSourceType.fuStartEvent);
             sourcelast.val(theElement.fxSourceType.fuStopEvent);
             sourceskip.val(theElement.fxSourceType.fuEventInterval);
            break;
            case GO4.EvIOType.GO4EV_MBS_TRANSPORT:
             sourcesel.val(3);
             sourceport.val(theElement.fxSourceType.fiPort);
            sourcetmout.val(theElement.fxSourceType.fiTimeout);
            sourceretry.val(theElement.fxSourceType.fiRetryCnt);
            sourcetag.text(theElement.fxSourceType.fxTagFile);
             sourcefirst.val(theElement.fxSourceType.fuStartEvent);
             sourcelast.val(theElement.fxSourceType.fuStopEvent);
             sourceskip.val(theElement.fxSourceType.fuEventInterval);
             break;
            case GO4.EvIOType.GO4EV_MBS_EVENTSERVER:
             sourcesel.val(4);
             sourceport.val(theElement.fxSourceType.fiPort);
            sourcetmout.val(theElement.fxSourceType.fiTimeout);
            sourceretry.val(theElement.fxSourceType.fiRetryCnt);
            sourcetag.text(theElement.fxSourceType.fxTagFile);
             sourcefirst.val(theElement.fxSourceType.fuStartEvent);
             sourcelast.val(theElement.fxSourceType.fuStopEvent);
             sourceskip.val(theElement.fxSourceType.fuEventInterval);
             break;
            case GO4.EvIOType.GO4EV_MBS_REVSERV:
             sourcesel.val(5);
             sourceport.val(theElement.fxSourceType.fiPort);
            sourcetmout.val(theElement.fxSourceType.fiTimeout);
            sourceretry.val(theElement.fxSourceType.fiRetryCnt);
            sourcetag.text(theElement.fxSourceType.fxTagFile);
             sourcefirst.val(theElement.fxSourceType.fuStartEvent);
             sourcelast.val(theElement.fxSourceType.fuStopEvent);
             sourceskip.val(theElement.fxSourceType.fuEventInterval);
             break;
            case GO4.EvIOType.GO4EV_MBS_RANDOM:
            sourcesel.val(6);
            break;
            case GO4.EvIOType.GO4EV_USER:
            sourcesel.val(7);
            sourceport.val(theElement.fxSourceType.fiPort);
            sourcetmout.val(theElement.fxSourceType.fiTimeout);
            sourceargs.text(theElement.fxSourceType.fxExpression);
            break;
            default:
             console.log("WARNING: unknown event source id: "+theElement.fiID);
             case GO4.EvIOType.GO4EV_MBS_FILE:
             sourcesel.val(1);
             sourcetag.text(theElement.fxSourceType.fxTagFile);
             sourcefirst.val(theElement.fxSourceType.fuStartEvent);
             sourcelast.val(theElement.fxSourceType.fuStopEvent);
              sourceskip.val(theElement.fxSourceType.fuEventInterval);
             break;
            };

            sourcesel.selectmenu('refresh', true);


         // event store properties:
         storesplit.val(theElement.fxStoreType.fiSplit);
         storebuf.val(theElement.fxStoreType.fiBufsize / 1000);
         storecomp.val(theElement.fxStoreType.fiCompression);
         storetreeasf.val(theElement.fxStoreType.fiAutosavesize);

       // set event store selector and special fields:
      //console.log("load tab "+theIndex+" sees store id:"+theElement.fxStoreType.fiID);
          switch(theElement.fxStoreType.fiID)
          {

             case GO4.EvIOType.GO4EV_BACK:
                storesel.val(1);
             break;
             case GO4.EvIOType.GO4EV_FILE:
                storeover.prop('checked', theElement.fxStoreType.fbOverwrite);
             default:
                storesel.val(0);
          break;
          };

         storesel.selectmenu('refresh', true);

         editor.showStepEditor(pthis, theElement, theIndex); // handle all visibility issues here, also refresh tabs



     }// load

        }); // tabs init


//////////////////////// END ANALYSIS STEP TABS


      $(id+" .buttonGetAnalysis")
         .button({text: false, icons: { primary: "ui-icon-blank MyButtonStyle"}})
         .click(function() {
              if (JSROOT.hpainter) JSROOT.hpainter.display(editor.GetItemName());
                             else  console.log("dabc object not found!");
         })
        .children(":first") // select first button element, used for images
        .css('background-image', "url(" + GO4.source_dir + "icons/right.png)");



       $(id+" .buttonSetAnalysis")
         .button({text: true, icons: { primary: "ui-icon-blank MyButtonStyle"}})
         .click(function() {
             var options=""; // do not need to use name here
               options=editor.EvaluateChanges(options); // complete option string from all changed elements
               console.log("submit analysis "+ editor.GetItemName()+ ", options="+options);
               editor.DabcCommand("UpdateFromUrl",options,function(result) {
                 console.log(result ? "setting analyis configuration done. " : "set analysis FAILED.");
                  if(result) {
                     editor.ClearChanges();
                     if ((JSROOT.hpainter != null) && ('reload' in JSROOT.hpainter)) JSROOT.hpainter.reload();
                  }
               });
         })
        .children(":first") // select first button element, used for images
        .css('background-image', "url(" + GO4.source_dir + "icons/left.png)");


       $(id+" .buttonAnaChangeLabel")
         .button({text: false, icons: { primary: "ui-icon-blank MyButtonStyle"}})
         .children(":first") // select first button element, used for images
         .css('background-image', "url(" + GO4.source_dir + "icons/info1.png)");


       $(id+" .buttonSetStartAnalysis")
         .button({text: true, icons: { primary: "ui-icon-blank MyButtonStyle"}})
         .click(function() {
             var options=""; // do not need to use name here
               options=editor.EvaluateChanges(options); // complete option string from all changed elements
               options +="&start";
               console.log("submit and start analysis "+ editor.GetItemName()+ ", options="+options);
               editor.DabcCommand("UpdateFromUrl",options,function(result) {
                  console.log(result ? "submit and start analyis configuration done. " : "set analysis FAILED.");
                 if(result) {
                    editor.ClearChanges();
                  if ((JSROOT.hpainter != null) && ('reload' in JSROOT.hpainter)) JSROOT.hpainter.reload();
                 }
                 // todo: start analysis only after submission was successful?
                 // for the moment, try to handle everythingin UpdateFromUrl
           });
         })
         .children(":first") // select first button element, used for images
         .css('background-image', "url(" + GO4.source_dir + "icons/start.png)");


       $(id+" .buttonCloseAnalysis")
         .button({text: true, icons: { primary: "ui-icon-closethick MyButtonStyle"}}).click(function() {
             var options="&close";
        //    options=editor.EvaluateChanges(options); // complete option string from all changed elements
            console.log("close analysis "+ editor.GetItemName()+ ", options="+options);
            editor.DabcCommand("UpdateFromUrl",options,function(
                 result) {
                     console.log(result ? "closing down analyis done. "
                   : "set analysis FAILED.");
                     //if(result) editor.ClearChanges();

           });
         });



       $(id+" .buttonSaveAnaASF")
        .button({text: false, icons: { primary: "ui-icon-blank MyButtonStyle"}})
        .children(":first") // select first button element, used for images
        .css('background-image', "url(" + GO4.source_dir + "icons/filesave.png)");


       $(id + " .anaASF_form").submit(
            function(event) {
               //event.preventDefault(); // do not send automatic request to server!
               var content= $(id + " .anaASF_name")[0].value;
               content=content.trim();
               // before we write immediately, mark name as changed in setup:
               editor.MarkChanged("asfname",0);
               editor.stat.fxAutoFileName=content;
               var requestmsg = "Really Write autosave file : "+ content;
               var response = confirm(requestmsg);
               if (!response){
                  event.preventDefault(); // do not send automatic request to server!
                  return;
                  }

               console.log("Writing autosave file: "+content);
               var options="&saveasf="+content;
               editor.DabcCommand("UpdateFromUrl",options,function(
                          result) {
                              console.log(result ? "Writing autosave file done. "
                            : "Writing autosave file FAILED.");
                    });
               event.preventDefault(); // do not send automatic request to server!
            });



       $(id +" .anaASF_time").spinner({
           min: 0,
           max: 100000,
           step: 10,
          stop: function( event, ui ) {

             editor.MarkChanged("asftime",0);
             editor.stat.fiAutoSaveInterval=this.value;
             //console.log("asftime stop.")
             }
       });

       $(id +" .anaASF_compression").spinner({
           min: 0,
           max: 9,
           step: 1,
          stop: function( event, ui ) {
             editor.MarkChanged("asfcomp",0);
             editor.stat.fiAutoSaveCompression=this.value;
             //console.log("asfcomp stop.")
             }
       });


       $(id+" .buttonSaveAnaConf")
        .button({text: false, icons: { primary: "ui-icon-blank MyButtonStyle"}})
        .children(":first") // select first button element, used for images
        .css('background-image', "url(" + GO4.source_dir + "icons/filesave.png)");

       $(id+" .buttonLoadAnaConf")
         .button({text: false, icons: { primary: "ui-icon-blank MyButtonStyle"}})
         .click(function() {
             var content= $(id + " .anaprefs_name")[0].value;
             content=content.trim();
             var requestmsg = "Really load analysis preferences: "+ content;
            var response = confirm(requestmsg);
            if (!response) return;
             console.log("Loading analysis Prefs from "+content);
             var options="&loadprefs="+content;
              editor.DabcCommand("UpdateFromUrl",options,function(result) {
              if(result){
                  if (JSROOT.hpainter) JSROOT.hpainter.display(editor.GetItemName());
                  else  console.log("dabc object not found!");
              }
              console.log("Loading preferences " + (result ? "done" : "FAILED."));
            });
         })
        .children(":first") // select first button element, used for images
        .css('background-image', "url(" + GO4.source_dir + "icons/fileopen.png)");


         $(id + " .anaprefs_form").submit(
            function(event) {
               event.preventDefault(); // do not send automatic request to server!
               var content= $(id + " .anaprefs_name")[0].value;
               content=content.trim();

               // before we write immediately, mark name as changed in setup:
               editor.MarkChanged("anaprefsname",0);
               editor.stat.fxConfigFileName=content;
               var requestmsg = "Really save analysis preferences: "+ content;
               var response = confirm(requestmsg);
               if (!response)   return;
               console.log("Saving analysis Prefs to "+content);

                 var options="&saveprefs="+content;
                 editor.DabcCommand("UpdateFromUrl",options,function(result) {
                     console.log(result ? "Saving preferences done. " : "Saving preferences  FAILED.");
                 });
            });

      this.refreshEditor();
   }

   GO4.AnalysisStatusEditor.prototype.drawEditor = function(divid) {
      var pthis = this;

      $("#"+divid).empty();
      $("#"+divid).load(GO4.source_dir + "html/analysiseditor.htm", "", function() {
         var html = "<ul>";
         for (var i=0;i<8;i++)
            html+='<li><a href="'+ GO4.source_dir + 'html/stepeditor.htm">Step ' + i + '</a></li>';
         html+="</ul>";
         $("#"+divid+" .steptabs").html(html);
         pthis.SetDivId(divid);
         pthis.fillEditor();
         pthis.DrawingReady()
      });
      return pthis;
      //console.log("analysis editor: drawEditor");
   }

   GO4.AnalysisStatusEditor.prototype.RedrawPad = function(resize) {
      //console.log("analysis editor: RedrawPad");
      this.refreshEditor();
   }

   GO4.AnalysisStatusEditor.prototype.UpdateObject = function(obj) {
      if (obj._typename != this.stat._typename) return false;
      this.stat = JSROOT.clone(obj);
      return true;
   }

   GO4.drawGo4AnalysisStatus = function(divid, stat, option, painter) {
      //console.log("Draw analysis status");
      var h = $("#"+divid).height(), w = $("#"+divid).width();
      if ((h<10) && (w>10)) $("#"+divid).height(w*0.7);
      var status = new GO4.AnalysisStatusEditor(stat);
      if (painter) status = JSROOT.extend(painter, status);
      return status.drawEditor(divid);
   }

})(); // function
