// $Id$

JSROOT.define(["painter", "jquery", "jquery-ui"], (jsrp, $) => {

   if (typeof JSROOT != "object") {
      let e1 = new Error("analysiseditor.js requires JSROOT to be already loaded");
      e1.source = "analysiseditor.js";
      throw e1;
   }

   if (typeof GO4 != "object") {
      let e1 = new Error("analysiseditor.js requires GO4 to be already loaded");
      e1.source = "analysiseditor.js";
      throw e1;
   }

   // only during transition
   JSROOT.loadScript("https://root.cern/js/6.3.2/style/jquery-ui.min.css");


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
      };

   GO4.AnalysisStatusEditor = function(dom, stat) {
      JSROOT.BasePainter.call(this, dom);
      this.stat = stat;
      this.changes = [["dummy0", "init0"],["dummy1","init1"]];  // changes array stepwise, index 0 = no step, index = stepindex+1
      this.showmore= [false, false];
      this.clearChanges();
      this.clearShowstates();
   }

   GO4.AnalysisStatusEditor.prototype = Object.create(JSROOT.BasePainter.prototype);

   GO4.AnalysisStatusEditor.prototype.markChanged = function(key, step) {
      // first avoid duplicate keys:
      for (let index = 0; index < this.changes[step].length; index++) {
         if (this.changes[step][index]== key) return;
      }
      this.changes[step].push(key);
      console.log("Mark changed :%s at step %d", key, step);

      this.selectDom().select(".buttonAnaChangeLabel").style('display', null); // show warning sign
   }

   // clear changed elements' list, make warning sign invisible
   GO4.AnalysisStatusEditor.prototype.clearChanges = function() {
      let numsteps = this.changes.length;
      for (let step = 0; step < numsteps ; step++)
         this.changes[step] = [];
      this.selectDom().select(".buttonAnaChangeLabel").style('display', 'none'); // hide warning sign
   }

   GO4.AnalysisStatusEditor.prototype.clearShowstates = function() {
      this.showmore = [];
   }

   //scan changed value list and return optionstring to be send to server
   GO4.AnalysisStatusEditor.prototype.evaluateChanges = function(optionstring) {
      let editor = this;
      let numsteps = this.changes.length;
      for (let step = 0; step < numsteps ; step++) {
         let len = this.changes[step].length;
         let stepoptions = "";
         for (let index = 0; index < len ; index++) {

            let key = this.changes[step][index];
            //console.log("Evaluate change key:%s", key);

            let theElement = editor.stat.fxStepArray.arr[step];
            // here mapping of key to editor field:
            if (key == "stepenabled") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fbProcessEnabled;
            } else if (key == "sourceenabled") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fbSourceEnabled;
            } else if (key == "storeenabled") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fbStoreEnabled;
            } else if (key == "sourcesel") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fxSourceType.fiID;
            } else if (key == "sourcename") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fxSourceType.fName;
            } else if (key == "sourceport") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fxSourceType.fiPort;
            } else if (key == "sourcetmout") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fxSourceType.fiTimeout;
            } else if (key == "sourceretry") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fxSourceType.fiRetryCnt;
            } else if (key == "sourcefirst") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fxSourceType.fuStartEvent;
            } else if (key == "sourcelast") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fxSourceType.fuStopEvent;
            } else if (key == "sourceskip") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fxSourceType.fuEventInterval;
            } else if (key == "storesel") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fxStoreType.fiID;
            } else if (key == "storename") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fxStoreType.fName;
            } else if (key == "storesplit") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fxStoreType.fiSplit;
            } else if (key == "storebuf") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fxStoreType.fiBufsize;
            } else if (key == "storecomp") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fxStoreType.fiCompression;
            } else if (key == "storeasf") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fxStoreType.fiAutosavesize;
            } else if (key == "storeover") {
               stepoptions += "&" + key + "_" + step + "=" + theElement.fxStoreType.fbOverwrite;
            }
            // non step specific options are in step 0 options too:
            else if (key == "asfname") {
               stepoptions += "&" + key + "=" + editor.stat.fxAutoFileName;
            } else if (key == "asfenabled") {
               stepoptions += "&" + key + "=" + editor.stat.fbAutoSaveOn;
            } else if (key == "asftime") {
               stepoptions += "&" + key + "=" + editor.stat.fiAutoSaveInterval;
            } else if (key == "asfcomp") {
               stepoptions += "&" + key + "=" + editor.stat.fiAutoSaveCompression;
            } else if (key == "asfoverwrite") {
               stepoptions += "&" + key + "=" + editor.stat.fbAutoSaveOverwrite;
            } else if (key == "anaprefsname") {
               stepoptions += "&" + key + "=" + editor.stat.fxConfigFileName;
            } else {
               console.log("Warning: evaluateChanges found unknown key:%s", key);
            }

         }// for index

         optionstring+=stepoptions;
      } // for step
      console.log("Resulting option string:%s", optionstring);
      return optionstring;
   }

   GO4.AnalysisStatusEditor.prototype.refreshEditor = function()
   {
      let id = "#" + this.getDomId(),
          editor = this,
          stat = this.stat;

      ///////////// ANALYSIS STEPS:
      this.clearShowstates();
      let tabelement = $(id+" .steptabs");
      tabelement.tabs( "option", "disabled", [0, 1, 2, 3, 4, 5, 6, 7] );
      for(let j=0; j<8;++j){
         $(id +" .steptabs ul:first li:eq("+ j +")").hide(); // disable and hide all tabs
      }
      stat.fxStepArray.arr.forEach(function(element, index, array) {
         tabelement.tabs("enable",index);
         $(id +" .steptabs ul:first li:eq("+index+") a").text(element.fName);
         $(id +" .steptabs ul:first li:eq("+index+")").show(); // only show what is really there
         editor.showmore.push(false); // prepare showmore array for each step
         //console.log("refreshEditor for step name:"+ element.fName);
         tabelement.tabs("load",index); // all magic is in the on load event callback
      }); // for each

      /////////////////// AUTO SAVE FILE:
      dom.select(".anaASF_name").property("value", stat.fxAutoFileName);
      dom.select(".anaASF_enabled")
         .property('checked', stat.fbAutoSaveOn)
         .on("click", () =>  {
               this.markChanged("asfenabled",0);
               this.stat.fbAutoSaveOn = dom.select(".anaASF_enabled").property('checked');
             });

      dom.select(".anaASF_time")
          .property("value", stat.fiAutoSaveInterval)
          .on("change", () => {
             this.markChanged("asftime", 0);
             this.stat.fiAutoSaveInterval = dom.select(".anaASF_time").property("value");
         });
      dom.select(".anaASF_compression")
         .property("value", stat.fiAutoSaveCompression)
         .on("change", () => {
            this.markChanged("asfcomp", 0);
            this.stat.fiAutoSaveCompression = dom.select(".anaASF_compression").property("value");
         });

      dom.select(".anaASF_overwrite")
         .property('checked', stat.fbAutoSaveOverwrite)
         .on("click", () => {
                this.markChanged("asfoverwrite",0);
                this.stat.fbAutoSaveOverwrite = dom.select(".anaASF_overwrite").property('checked');
             });

      ////////////////// PREFS FILE:
      dom.select(".anaprefs_name").property("value", stat.fxConfigFileName);

      editor.clearChanges();
   }

   GO4.AnalysisStatusEditor.prototype.showStepEditor = function(pthis, theElement, theIndex)
   {
      let id = "#" + this.getDomId();
      let editor = this;
      let showmore = editor.showmore[theIndex];
      //console.log("showStepEditor for index "+theIndex+" has showmore="+showmore);
      let storetable = pthis.find(" .step_store");
      let sourcetable = pthis.find(" .step_source");
      let enablebox = pthis.find(" .step_box_step_enab");
      let sourcebox = pthis.find(" .step_box_source_enab");
      let storebox = pthis.find(" .step_box_store_enab");

      let sourcesel = pthis.find(" .step_source_select");
      let sourcemore = pthis.find(" .step_source_expand");
      let sourceform = pthis.find(" .step_source_form");
      let sourcename = pthis.find(" .step_source_name");
      let sourcenamelabel = pthis.find(" .step_source_name_label");
      let sourcetag = pthis.find(" .step_source_tagfile");
      let sourcetaglabel = pthis.find(" .step_source_tagfile_label");
      let sourceport = pthis.find(" .step_source_port");
      let sourceportlabel = pthis.find(" .step_source_port_label");
      let sourcetmout = pthis.find(" .step_source_tmout");
      let sourcetmoutlabel = pthis.find(" .step_source_tmout_label");
      let sourceretry = pthis.find(" .step_source_retry");
      let sourceretrylabel = pthis.find(" .step_source_retry_label");
      let sourceargs = pthis.find(" .step_source_args");
      let sourceargslabel = pthis.find(" .step_source_args_label");
      let sourcefirst = pthis.find(" .step_source_firstev");
      let sourcefirstlabel = pthis.find(" .step_source_firstev_label");
      let sourcelast = pthis.find(" .step_source_lastev");
      let sourcelastlabel = pthis.find(" .step_source_lastev_label");
      let sourceskip = pthis.find(" .step_source_stepev");
      let sourceskiplabel = pthis.find(" .step_source_stepev_label");

      let storesel = pthis.find(" .step_store_select");
      let storename = pthis.find(" .step_store_name");
      let storesplit = pthis.find(" .step_store_split");
      let storebuf = pthis.find(" .step_store_buf");
      let storecomp = pthis.find(" .step_store_comp");
      let storetreeasf = pthis.find(" .step_store_asf");
      let storeover = pthis.find(" .step_store_overwrite");

    // here step control checkboxes and source/store visibility:
      if (theElement.fbProcessEnabled) {
         sourcebox.prop('disabled', false);
         storebox.prop('disabled', false);
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
         sourcebox.prop('disabled', true);
         storebox.prop('disabled', true);
         sourcetable.hide();
         storetable.hide();
      }

      sourceform.show();

      //console.log("show step editor with source id:"+theElement.fxSourceType.fiID);
      switch (theElement.fxSourceType.fiID) {
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
            if (showmore) {
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
            } else {
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
            if (showmore) {
               sourceport.show();
               sourceportlabel.show();
               sourcetmout.show();
               sourcetmoutlabel.show();
               sourceargs.show();
               sourceargslabel.show();
            } else {
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
            console.log("showStepEditor WARNING: unknown event source id: " + theElement.fxSourceType.fiID);
         case GO4.EvIOType.GO4EV_MBS_FILE:
            if (showmore) {
               sourcetag.show();
               sourcetaglabel.show();
               sourcefirst.show();
               sourcefirstlabel.show();
               sourcelast.show();
               sourcelastlabel.show();
               sourceskip.show();
               sourceskiplabel.show();
            } else {
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
      switch (theElement.fxStoreType.fiID) {
         default:
            console.log("showStepEditor WARNING: unknown event store id: " + theElement.fxStoreType.fiID);
         case GO4.EvIOType.GO4EV_FILE:
            storecomp.spinner("enable");
            storetreeasf.spinner("enable");
            storeover.prop('disabled', false);
            break;
         case GO4.EvIOType.GO4EV_BACK:
            storecomp.spinner("disable");
            storetreeasf.spinner("disable");
            storeover.prop('disabled', true);
            break;
      };

      sourcesel.selectmenu("option", "width", sourcetable.width() * 0.8); // expand to table width
      sourcesel.selectmenu('refresh', true);

      storesel.selectmenu("option", "width", storetable.width() * 0.8); // expand to table width
      storesel.selectmenu('refresh', true);

      pthis.css("padding", "5px");

      $(id + " .steptabs").tabs("refresh");
   }

   GO4.AnalysisStatusEditor.prototype.fillEditor = function()
   {
      let id = "#" + this.getDomId(),
          editor = this,
          dom = this.selectDom();

      $(id + " .steptabs").tabs({
         heightStyle: "fill",
         activate: function(event, ui) {
            //updateElementsSize();
            //console.log("analysis editor: activated tab: "+ ui.newTab.text());
         },

         load: function(event, ui) {

            // note that load will also be triggered when activating tab!
            // so we need to backup all changes in local step status theElement!!!

            let theIndex = ui.tab.index();
            //console.log("On load function for "  + ui.tab.text() + " index=" + theIndex );
            let pthis = ui.panel;
            let theElement = editor.stat.fxStepArray.arr[theIndex];

            //              console.log("process enabled="+theElement.fbProcessEnabled + "for theElement: "+theElement.fName);
            //              console.log("source enabled="+theElement.fbSourceEnabled + "for theElement: "+theElement.fName);
            //              console.log("store enabled="+theElement.fbStoreEnabled + "for theElement: "+theElement.fName);

            let storetable = pthis.find(" .step_store");
            let sourcetable = pthis.find(" .step_source");
            let enablebox = pthis.find(" .step_box_step_enab");
            let sourcebox = pthis.find(" .step_box_source_enab");
            let storebox = pthis.find(" .step_box_store_enab");

            let sourcesel = pthis.find(" .step_source_select");
            let sourcemore = pthis.find(" .step_source_expand");
            let sourceform = pthis.find(" .step_source_form");
            let sourcename = pthis.find(" .step_source_name");
            let sourcetag = pthis.find(" .step_source_tagfile");
            let sourceport = pthis.find(" .step_source_port");
            let sourcetmout = pthis.find(" .step_source_tmout");
            let sourceretry = pthis.find(" .step_source_retry");
            let sourceargs = pthis.find(" .step_source_args");
            let sourcefirst = pthis.find(" .step_source_firstev");
            let sourcelast = pthis.find(" .step_source_lastev");
            let sourceskip = pthis.find(" .step_source_stepev");

            let storesel = pthis.find(" .step_store_select");
            let storename = pthis.find(" .step_store_name");
            let storesplit = pthis.find(" .step_store_split");
            let storebuf = pthis.find(" .step_store_buf");
            let storecomp = pthis.find(" .step_store_comp");
            let storetreeasf = pthis.find(" .step_store_asf");
            let storeover = pthis.find(" .step_store_overwrite");

            enablebox.prop('checked', theElement.fbProcessEnabled)
               .click(function() {
                  editor.markChanged("stepenabled", theIndex);
                  theElement.fbProcessEnabled = this.checked;
                  editor.showStepEditor(pthis, theElement, theIndex);
               }); // clickfunction

            sourcebox.prop('checked', theElement.fbSourceEnabled)
               .click(function() {
                  editor.markChanged("sourceenabled", theIndex);
                  theElement.fbSourceEnabled = this.checked;
                  editor.showStepEditor(pthis, theElement, theIndex);
               }); // clickfunction


            storebox.prop('checked', theElement.fbStoreEnabled)
               .click(function() {
                  editor.markChanged("storeenabled", theIndex);
                  theElement.fbStoreEnabled = this.checked;
                  editor.showStepEditor(pthis, theElement, theIndex);
               }); // clickfunction

            //// EVENT SOURCE: /////////////////////////////////////////////////
            sourcesel.selectmenu({
               change: function(event, ui) {
                  editor.markChanged("sourcesel", theIndex);
                  // change here eventsource status object?!
                  // in javascript we can just add dynamically any missing members!
                  // so exchange of class object is not necessary hopefully...

                  switch (Number(ui.item.value)) {
                     case 0:
                        theElement.fxSourceType.fiID = GO4.EvIOType.GO4EV_FILE;
                        break;
                     case 2:
                        theElement.fxSourceType.fiID = GO4.EvIOType.GO4EV_MBS_STREAM;
                        break;
                     case 3:
                        theElement.fxSourceType.fiID = GO4.EvIOType.GO4EV_MBS_TRANSPORT;
                        break;
                     case 4:
                        theElement.fxSourceType.fiID = GO4.EvIOType.GO4EV_MBS_EVENTSERVER;
                        break;
                     case 5:
                        theElement.fxSourceType.fiID = GO4.EvIOType.GO4EV_MBS_REVSERV;
                        break;
                     case 6:
                        theElement.fxSourceType.fiID = GO4.EvIOType.GO4EV_MBS_RANDOM;
                        break;
                     case 7:
                        theElement.fxSourceType.fiID = GO4.EvIOType.GO4EV_USER;
                        break;
                     default:
                     case 1:
                        theElement.fxSourceType.fiID = GO4.EvIOType.GO4EV_MBS_FILE;
                        break;
                  }; // switch

                  // but: we have to set back all values  from GUI to theElement and optionally create new members:
                  theElement.fxSourceType.fName = sourcename.val();
                  theElement.fxSourceType.fiPort = sourceport.val();
                  theElement.fxSourceType.fiTimeout = sourcetmout.val();
                  theElement.fxSourceType.fiRetryCnt = sourceretry.val();
                  theElement.fxSourceType.fxTagFile = sourcetag.val();
                  theElement.fxSourceType.fuStartEvent = sourcefirst.val();
                  theElement.fxSourceType.fuStopEvent = sourcelast.val();
                  theElement.fxSourceType.fuEventInterval = sourceskip.val();
                  theElement.fxSourceType.fxExpression = sourceargs.val();

                  editor.showStepEditor(pthis, theElement, theIndex);
               }
            }); // source selectmenu change


            sourcemore.prop('checked', editor.showmore[theIndex]).click(
               function() {
                  //console.log("show more clickfunction...");
                  let doshow = $(this).prop('checked');
                  if (doshow) {
                     editor.showmore[theIndex] = true;
                  } else {
                     editor.showmore[theIndex] = false;
                  }
                  editor.showStepEditor(pthis, theElement, theIndex);
               });  // clickfunction


            sourcename.val(theElement.fxSourceType.fName);
            //          .change(function(){
            //         editor.markChanged("sourcename",theIndex);
            //         theElement.fxSourceType.fName=this.value.trim();
            //         }); ;

            sourceform.submit(
               function(event) {
                  event.preventDefault(); // do not send automatic request to server!
                  let content = sourcename[0].value;
                  content = content.trim();
                  editor.markChanged("sourcename", theIndex);
                  theElement.fxSourceType.fName = content;
                  console.log("Submitting sourcename form with: " + content);
               });


            //console.log("on tab load finds source name: "+ theElement.fxSourceType.fName);
            //         sourcename.val(theElement.fxSourceType.fName)
            //            .change(function(){
            //               editor.markChanged("sourcename",theIndex);
            //               theElement.fxSourceType.fName=this.value.trim();
            //               });

            sourceport.spinner({
               min: 0,
               max: 100000,
               step: 1,
               stop: function(event, ui) {
                  editor.markChanged("sourceport", theIndex);
                  theElement.fxSourceType.fiPort = this.value;
                  //console.log("spinner stop event with thisvalue="+this.value+", ui.value="+ui.value);
               }
            });

            sourcetmout.spinner({
               min: 0,
               max: 9999,
               step: 1,
               stop: function(event, ui) {
                  editor.markChanged("sourcetmout", theIndex);
                  theElement.fxSourceType.fiTimeout = this.value;
               }
            });

            sourceretry.spinner({
               min: 0,
               max: 10000,
               step: 1,
               stop: function(event, ui) {
                  editor.markChanged("sourceretry", theIndex);
                  theElement.fxSourceType.fiRetryCnt = this.value;
               }
            });

            sourcefirst.spinner({
               min: 0,
               max: 2000000000,
               step: 1000,
               stop: function(event, ui) {
                  editor.markChanged("sourcefirst", theIndex);
                  theElement.fxSourceType.fuStartEvent = this.value;
               }
            });

            sourcelast.spinner({
               min: 0,
               max: 2000000000,
               step: 1000,
               stop: function(event, ui) {
                  editor.markChanged("sourcelast", theIndex);
                  theElement.fxSourceType.fuStopEvent = this.value;
               }
            });

            sourceskip.spinner({
               min: 0,
               max: 999999999,
               step: 1,
               stop: function(event, ui) {
                  editor.markChanged("sourceskip", theIndex);
                  theElement.fxSourceType.fuEventInterval = this.value;
               }
            });

            //// EVENT STORE: /////////////////////////////////////////////////
            storesel.selectmenu({
               change: function(event, ui) {
                  editor.markChanged("storesel", theIndex);

                  //console.log("store selector with value "+ Number(ui.item.value));
                  switch (Number(ui.item.value)) {
                     case 1:
                        theElement.fxStoreType.fiID = GO4.EvIOType.GO4EV_BACK;
                        break;
                     case 0:
                     default:
                        theElement.fxStoreType.fiID = GO4.EvIOType.GO4EV_FILE;
                        break;
                  }; // switch
                  theElement.fxStoreType.fName = storename.val();
                  theElement.fxStoreType.fiSplit = storesplit.val();
                  theElement.fxStoreType.fiBufsize = storebuf.val() * 1000.;
                  theElement.fxStoreType.fiCompression = storecomp.val();
                  theElement.fxStoreType.fiAutosavesize = storetreeasf.val();

                  editor.showStepEditor(pthis, theElement, theIndex);
               } // change function
            }); // selectmenu


            //console.log("on tab load finds store name: "+ theElement.fxStoreType.fName);
            storename.val(theElement.fxStoreType.fName)
               .change(function() {
                  editor.markChanged("storename", theIndex);
                  theElement.fxStoreType.fName = this.value.trim();
               }); // change function

            storesplit.spinner({
               min: 0,
               max: 99,
               step: 1,
               stop: function(event, ui) {
                  editor.markChanged("storesplit", theIndex);
                  theElement.fxStoreType.fiSplit = this.value;
               }
            });

            storebuf.spinner({
               min: 4,
               max: 256,
               step: 1,
               stop: function(event, ui) {
                  editor.markChanged("storebuf", theIndex);
                  theElement.fxStoreType.fiBufsize = this.value * 1000;
               }
            });

            storecomp.spinner({
               min: 0,
               max: 9,
               step: 1,
               stop: function(event, ui) {
                  editor.markChanged("storecomp", theIndex);
                  theElement.fxStoreType.fiCompression = this.value;
               }
            });
            storetreeasf.spinner({
               min: 0,
               max: 99999,
               step: 100,
               stop: function(event, ui) {
                  editor.markChanged("storeasf", theIndex);
                  theElement.fxStoreType.fiAutosavesize = this.value;
               }
            });

            storeover.click(function() {
               editor.markChanged("storeover", theIndex);
               theElement.fxStoreType.fbOverwrite = this.checked;
            });

            ////////////////// here set event source values:


            // set event source selector and special fields:
            //console.log("load tab "+theIndex+" sees source id:"+theElement.fxSourceType.fiID);
            switch (theElement.fxSourceType.fiID) {
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
                  console.log("WARNING: unknown event source id: " + theElement.fiID);
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
            switch (theElement.fxStoreType.fiID) {

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

      dom.select(".buttonGetAnalysis")
         .style('background-image', "url(" + GO4.source_dir + "icons/right.png)")
         .on("click", () => {
               if (JSROOT.hpainter) JSROOT.hpainter.display(this.getItemName());
         });

      dom.select(".buttonSetAnalysis")
         .style('background-image', "url(" + GO4.source_dir + "icons/left.png)")
         .on("click", () => {
            let options = this.evaluateChanges(""); // complete option string from all changed elements
            console.log("submit analysis " + this.getItemName() + ", options=" + options);
            GO4.ExecuteMethod(this, "UpdateFromUrl", options).then(() => {
               console.log("setting analyis configuration done.");
               this.clearChanges();
               if (JSROOT.hpainter && (typeof JSROOT.hpainter.reload == 'function')) JSROOT.hpainter.reload();
            });
         });

      dom.select(".buttonAnaChangeLabel")
         .style('background-image', "url(" + GO4.source_dir + "icons/info1.png)");

      dom.select(".buttonSetStartAnalysis")
         .style('background-image', "url(" + GO4.source_dir + "icons/start.png)")
         .on("click", () => {
            let options = this.evaluateChanges(""); // complete option string from all changed elements
            options += "&start";
            console.log("submit and start analysis " + this.getItemName() + ", options=" + options);
            GO4.ExecuteMethod(this, "UpdateFromUrl", options).then(() => {
               console.log("submit and start analyis configuration done. ");
               this.clearChanges();
               if (JSROOT.hpainter && (typeof JSROOT.hpainter.reload == 'function')) JSROOT.hpainter.reload();
            });
         });

      dom.select(".buttonCloseAnalysis")
         .style('background-image', "url(" + GO4.source_dir + "icons/close.png)")
         .on("click", () => {
            let options = "&close";
            console.log("close analysis " + editor.getItemName() + ", options=" + options);
            GO4.ExecuteMethod(this, "UpdateFromUrl", options).then(() => {
               console.log("closing down analyis done. ");
            });
         });

      dom.select(".buttonSaveAnaASF")
         .style('background-image', "url(" + GO4.source_dir + "icons/filesave.png)");


      dom.select(".anaASF_form").on("submit", event => {
         event.preventDefault(); // do not send automatic request to server!
         let content = dom.select(".anaASF_name").property("value");
         content = content.trim();
         // before we write immediately, mark name as changed in setup:
         this.markChanged("asfname", 0);
         this.stat.fxAutoFileName = content;

         jsrp.createMenu(event, this)
             .then(menu => menu.runModal("Save auto save file",`<p tabindex="0">${content}</p>`, { btns: true, height: 120, width: 400 }))
             .then(elem => (elem ? GO4.ExecuteMethod(this, "UpdateFromUrl", "&saveasf=" + content) : null))
             .then(() => console.log("Writing autosave file done. "));
      });


      $(id + " .buttonSaveAnaConf")
         .button({ text: false, icons: { primary: "ui-icon-blank MyButtonStyle" } })
         .children(":first") // select first button element, used for images
         .css('background-image', "url(" + GO4.source_dir + "icons/filesave.png)");

      $(id + " .buttonLoadAnaConf")
         .button({ text: false, icons: { primary: "ui-icon-blank MyButtonStyle" } })
         .click(function() {
            let content = $(id + " .anaprefs_name")[0].value;
            content = content.trim();
            let requestmsg = "Really load analysis preferences: " + content;
            let response = confirm(requestmsg);
            if (!response) return;
            console.log("Loading analysis Prefs from " + content);
            let options = "&loadprefs=" + content;
            GO4.ExecuteMethodOld(editor, "UpdateFromUrl", options, function(result) {
               if (result) {
                  if (JSROOT.hpainter) JSROOT.hpainter.display(editor.getItemName());
                  else console.log("dabc object not found!");
               }
               console.log("Loading preferences " + (result ? "done" : "FAILED."));
            });
         })
         .children(":first") // select first button element, used for images
         .css('background-image', "url(" + GO4.source_dir + "icons/fileopen.png)");


      $(id + " .anaprefs_form").submit(
         function(event) {
            event.preventDefault(); // do not send automatic request to server!
            let content = $(id + " .anaprefs_name")[0].value;
            content = content.trim();

            // before we write immediately, mark name as changed in setup:
            editor.markChanged("anaprefsname", 0);
            editor.stat.fxConfigFileName = content;
            let requestmsg = "Really save analysis preferences: " + content;
            let response = confirm(requestmsg);
            if (!response) return;
            console.log("Saving analysis Prefs to " + content);
            let options = "&saveprefs=" + content;
            GO4.ExecuteMethodOld(editor, "UpdateFromUrl", options, function(result) {
               console.log(result ? "Saving preferences done. " : "Saving preferences  FAILED.");
            });
         });

      this.refreshEditor();
   }

   GO4.AnalysisStatusEditor.prototype.redrawObject = function(obj /*, opt */) {
      if (obj._typename != this.stat._typename) return false;
      this.stat = JSROOT.clone(obj);
      this.refreshEditor();
      return true;
   }

   GO4.drawGo4AnalysisStatus = function(dom, stat) {
      let editor = new GO4.AnalysisStatusEditor(dom, stat),
          sel = editor.selectDom(),
          h = sel.node().clientHeight,
          w = sel.node().clientWidth;

      if ((h < 10) && (w > 10)) sel.style("height", Math.round(w * 0.7)+"px");

      return JSROOT.httpRequest(GO4.source_dir + "html/analysiseditor.htm", "text").then(code => {
         sel.html(code);

         let html = "";
         for (let i=0;i<8;i++)
            html += `<li><a href="${GO4.source_dir}html/stepeditor.htm">Step ${i}</a></li>`;

         sel.select(".steptabs").html("<ul>" + html + "</ul>");

         editor.fillEditor();
         editor.setTopPainter();
         return editor;
      });

   }

}); // factory function
