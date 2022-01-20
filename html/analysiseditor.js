// $Id$

JSROOT.define(["painter"], jsrp => {

   if (typeof GO4 != "object") {
      let e1 = new Error("analysiseditor.js requires GO4 to be already loaded");
      e1.source = "analysiseditor.js";
      throw e1;
   }

   GO4.EvIOType = {
          GO4EV_NULL: 0,                // no event store/source
          GO4EV_FILE: 1,                // root file with own tree
          GO4EV_TREE: 2,                // branch of singleton tree
          GO4EV_MBS_FILE: 3,            // mbs listmode file (input only)
          GO4EV_MBS_STREAM: 4,          // mbs stream server (input only)
          GO4EV_MBS_TRANSPORT: 5,       // mbs transport server (input only)
          GO4EV_MBS_EVENTSERVER: 6,     // mbs event server  (input only)
          GO4EV_MBS_REVSERV: 7,         // remote event server (input only)
          GO4EV_BACK: 8,                // backstore in memory (pseudo-ringbuffer?)
          GO4EV_USER: 9,                // user defined source class
          GO4EV_MBS_RANDOM: 10,         // random generated mbs event
          GO4EV_HDF5: 11                // HDF5 file format
      };

   GO4.AnalysisStatusEditor = function(dom, stat) {
      JSROOT.BasePainter.call(this, dom);
      this.stat = stat;
      this.changes = [["dummy0", "init0"],["dummy1","init1"]];  // changes array stepwise, index 0 = no step, index = stepindex+1
      this.clearChanges();
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

   //scan changed value list and return optionstring to be send to server
   GO4.AnalysisStatusEditor.prototype.evaluateChanges = function(optionstring) {
      let editor = this;
      let numsteps = this.changes.length;
      for (let step = 0; step < numsteps ; step++) {
         let theElement = editor.stat.fxStepArray.arr[step],
             stepoptions = "",
             len = this.changes[step].length;

         for (let index = 0; index < len ; index++) {

            let key = this.changes[step][index],
                preifx = `&${key}=`,
                step_prefix = `&${key}_${step}=`;

            // here mapping of key to editor field:
            if (key == "stepenabled") {
               stepoptions += step_prefix + theElement.fbProcessEnabled;
            } else if (key == "sourceenabled") {
               stepoptions += step_prefix + theElement.fbSourceEnabled;
            } else if (key == "storeenabled") {
               stepoptions += step_prefix + theElement.fbStoreEnabled;
            } else if (key == "sourcesel") {
               stepoptions += step_prefix + theElement.fxSourceType.fiID;
            } else if (key == "sourcename") {
               stepoptions += step_prefix + theElement.fxSourceType.fName;
            } else if (key == "sourcetag") {
               stepoptions += step_prefix + theElement.fxSourceType.fxTagFile;
            } else if (key == "sourceport") {
               stepoptions += step_prefix + theElement.fxSourceType.fiPort;
            } else if (key == "sourcetmout") {
               stepoptions += step_prefix + theElement.fxSourceType.fiTimeout;
            } else if (key == "sourceretry") {
               stepoptions += step_prefix + theElement.fxSourceType.fiRetryCnt;
            } else if (key == "sourcefirst") {
               stepoptions += step_prefix + theElement.fxSourceType.fuStartEvent;
            } else if (key == "sourcelast") {
               stepoptions += step_prefix + theElement.fxSourceType.fuStopEvent;
            } else if (key == "sourceskip") {
               stepoptions += step_prefix + theElement.fxSourceType.fuEventInterval;
            } else if (key == "storesel") {
               stepoptions += step_prefix + theElement.fxStoreType.fiID;
            } else if (key == "storename") {
               stepoptions += step_prefix + theElement.fxStoreType.fName;
            } else if (key == "storesplit") {
               stepoptions += step_prefix + theElement.fxStoreType.fiSplit;
            } else if (key == "storebuf") {
               stepoptions += step_prefix + theElement.fxStoreType.fiBufsize;
            } else if (key == "storecomp") {
               stepoptions += step_prefix + theElement.fxStoreType.fiCompression;
            } else if (key == "storeasf") {
               stepoptions += step_prefix + theElement.fxStoreType.fiAutosavesize;
            } else if (key == "storeover") {
               stepoptions += step_prefix + theElement.fxStoreType.fbOverwrite;
            }
            // non step specific options are in step 0 options too:
            else if (key == "asfname") {
               stepoptions += preifx + editor.stat.fxAutoFileName;
            } else if (key == "asfenabled") {
               stepoptions += preifx + editor.stat.fbAutoSaveOn;
            } else if (key == "asftime") {
               stepoptions += preifx + editor.stat.fiAutoSaveInterval;
            } else if (key == "asfcomp") {
               stepoptions += preifx + editor.stat.fiAutoSaveCompression;
            } else if (key == "asfoverwrite") {
               stepoptions += preifx + editor.stat.fbAutoSaveOverwrite;
            } else if (key == "anaprefsname") {
               stepoptions += preifx + editor.stat.fxConfigFileName;
            } else {
               console.log(`Warning: evaluateChanges found unknown key: ${key}`);
            }

         }// for index

         optionstring += stepoptions;
      } // for step
      console.log("Resulting option string:%s", optionstring);
      return optionstring;
   }

   GO4.AnalysisStatusEditor.prototype.showStepEditor = function(tab, theElement, theIndex) {

      let sourcebox = tab.select(".step_box_source_enab"),
          storebox = tab.select(".step_box_store_enab"),
          step_source = tab.select(".step_source"),
          step_store = tab.select(".step_store"),
          sourcemore = tab.select(".step_source_expand");

    // here step control checkboxes and source/store visibility:
      if (theElement.fbProcessEnabled) {
         sourcebox.attr('disabled', null);
         storebox.attr('disabled', null);
         step_source.attr('disabled', theElement.fbSourceEnabled ? null : "true");
         step_store.style('display', theElement.fbStoreEnabled ? null : "none");
      } else {
         sourcebox.attr('disabled', "true");
         storebox.attr('disabled', "true");
         step_source.attr('disabled', "true");
         step_store.style('display', "none");
      }

      let show_tag = false, show_ports = false, show_args = false, show_more = sourcemore.property("checked");

      switch (theElement.fxSourceType.fiID) {
         case GO4.EvIOType.GO4EV_MBS_FILE:
            show_tag = show_args = show_more; break;
         case GO4.EvIOType.GO4EV_MBS_STREAM:
         case GO4.EvIOType.GO4EV_MBS_TRANSPORT:
         case GO4.EvIOType.GO4EV_MBS_EVENTSERVER:
         case GO4.EvIOType.GO4EV_MBS_REVSERV:
            show_ports = show_args = show_more; break;
         case GO4.EvIOType.GO4EV_USER:
            show_ports = show_more; break;
         case GO4.EvIOType.GO4EV_FILE:
         case GO4.EvIOType.GO4EV_MBS_RANDOM:
         default:
            // show no extra parameters
            break;
      };

      tab.select(".step_source_tagfile_args").style('display', show_tag ? null : 'none');
      tab.select(".step_source_port_args").style('display', show_ports ? null : 'none');
      tab.select(".step_source_number_args").style('display', show_args ? 'inline' : 'none');

      let enbale_store_pars = false, enbale_store_name = true;
      switch (theElement.fxStoreType.fiID) {
         case GO4.EvIOType.GO4EV_FILE:
            enbale_store_pars = true;
            break;
         case GO4.EvIOType.GO4EV_BACK:
            enbale_store_pars = true;
            enbale_store_name = false;
            break;
         case GO4.EvIOType.GO4EV_USER:
            break;
         default:
            break;
      };

      tab.select(".step_store_pars").selectAll("input").each(function() {
         d3.select(this).attr("disabled", enbale_store_pars ? null : "true");
      });

      tab.select(".step_store_name").attr("disabled", enbale_store_name ? null : "true");
   }

   GO4.AnalysisStatusEditor.prototype.fillEditor = function() {
      let dom = this.selectDom(),
          stat = this.stat;

      let head_html = "", step_html = "";
      stat.fxStepArray.arr.forEach((step, indx) => {
         head_html += `<button for="go4_analysis_step_${indx}">${step.fName}</button>`;
         step_html += `<div class="go4_analysis_step_${indx}" style="display:none">${this.stepPageHtml}</div>`;
      });

      dom.select(".ana_step_tabs_header").html(head_html);

      dom.select(".ana_step_tabs_body").html(step_html);

       // assign tabs buttons handlers
      dom.select('.ana_step_tabs_header').selectAll("button").on("click", function() {
         let btn = d3.select(this);

         dom.select('.ana_step_tabs_header').selectAll("button").each(function() {
            d3.select(this).classed("active_btn", false);
         });

         btn.classed("active_btn", true);

         dom.selectAll('.ana_step_tabs_body>div').each(function() {
            let tab = d3.select(this);
            tab.style('display', tab.classed(btn.attr("for")) ? null : "none");
         });
      });

      // activate first step
      dom.select(".ana_step_tabs_body").select(".go4_analysis_step_0").style('display', null);
      dom.select('.ana_step_tabs_header').select("button").classed("active_btn", true);

      stat.fxStepArray.arr.forEach((theElement, theIndex) => {
         let tab = dom.select(".ana_step_tabs_body").select(`.go4_analysis_step_${theIndex}`);

         let enablebox = tab.select(".step_box_step_enab"),
             sourcebox = tab.select(".step_box_source_enab"),
             storebox = tab.select(".step_box_store_enab"),
             sourcesel = tab.select(".step_source_select"),
             sourcemore = tab.select(".step_source_expand"),
             sourcename = tab.select(".step_source_name"),
             sourcetag = tab.select(".step_source_tagfile"),
             sourceport = tab.select(".step_source_port"),
             sourcetmout = tab.select(".step_source_tmout"),
             sourceretry = tab.select(".step_source_retry"),
             sourcefirst = tab.select(".step_source_firstev"),
             sourcelast = tab.select(".step_source_lastev"),
             sourceskip = tab.select(".step_source_stepev"),
             storesel = tab.select(".step_store_select"),
             storename = tab.select(".step_store_name"),
             storesplit = tab.select(".step_store_split"),
             storebuf = tab.select(".step_store_buf"),
             storecomp = tab.select(".step_store_comp"),
             storetreeasf = tab.select(".step_store_asf"),
             storeover = tab.select(" .step_store_overwrite");

         enablebox.property('checked', theElement.fbProcessEnabled)
            .on("click", () => {
               this.markChanged("stepenabled", theIndex);
               theElement.fbProcessEnabled = enablebox.property('checked');
               this.showStepEditor(tab, theElement, theIndex);
            }); // clickfunction

         sourcebox.property('checked', theElement.fbSourceEnabled)
            .on("click", () => {
               this.markChanged("sourceenabled", theIndex);
               theElement.fbSourceEnabled = sourcebox.property('checked');
               this.showStepEditor(tab, theElement, theIndex);
            }); // clickfunction

         storebox.property('checked', theElement.fbStoreEnabled)
            .on("click", () => {
               this.markChanged("storeenabled", theIndex);
               theElement.fbStoreEnabled = storebox.property('checked');
               this.showStepEditor(tab, theElement, theIndex);
            }); // clickfunction

         //// EVENT SOURCE: /////////////////////////////////////////////////
         sourcesel.property("value", theElement.fxSourceType.fiID).on("change", () => {
            this.markChanged("sourcesel", theIndex);

            theElement.fxSourceType.fiID = parseInt(sourcesel.property("value"));

            this.showStepEditor(tab, theElement, theIndex);
         }); // source selectmenu change

         sourcemore.on("click", () => {
            this.showStepEditor(tab, theElement, theIndex);
         });

         sourcename.property("value", theElement.fxSourceType.fName)
                   .on("change", () => {
                        this.markChanged("sourcename", theIndex);
                        theElement.fxSourceType.fName = sourcename.property("value").trim();
                    });

         sourcetag.property("value", theElement.fxSourceType.fxTagFile || "")
                  .on("change", () => {
                     this.markChanged("sourcetag", theIndex);
                     theElement.fxSourceType.fxTagFile = sourcetag.property("value").trim();
                  });

         sourceport.property("value", theElement.fxSourceType.fiPort || 0)
                   .on("change", () => {
                      this.markChanged("sourceport", theIndex);
                      theElement.fxSourceType.fiPort = parseInt(sourceport.property("value"));
                   });

         sourcetmout.property("value", theElement.fxSourceType.fiTimeout || 100)
                    .on("change", () => {
                        this.markChanged("sourcetmout", theIndex);
                        theElement.fxSourceType.fiTimeout = parseInt(sourcetmout.property("value"));
                    });

         sourceretry.property("value", theElement.fxSourceType.fiRetryCnt || 0)
                    .on("change", () => {
                        this.markChanged("sourceretry", theIndex);
                        theElement.fxSourceType.fiRetryCnt = parseInt(sourceretry.property("value"));
                     });

         sourcefirst.property("value", theElement.fxSourceType.fuStartEvent || 0)
                    .on("change", () => {
                        this.markChanged("sourcefirst", theIndex);
                        theElement.fxSourceType.fuStartEvent = parseInt(sourcefirst.property("value"));
                     });

         sourcelast.property("value", theElement.fxSourceType.fuStopEvent || 0)
                    .on("change", () => {
                       this.markChanged("sourcelast", theIndex);
                       theElement.fxSourceType.fuStopEvent = parseInt(sourcelast.property("value"));
                     });

         sourceskip.property("value", theElement.fxSourceType.fuEventInterval || 0)
                   .on("change", () => {
                      this.markChanged("sourceskip", theIndex);
                      theElement.fxSourceType.fuEventInterval = parseInt(sourceskip.property("value"));
                   });

         storesel.property("value", theElement.fxStoreType.fiID).on("change", () => {
            this.markChanged("storesel", theIndex);
            theElement.fxStoreType.fiID = parseInt(storesel.property("value"));
            this.showStepEditor(tab, theElement, theIndex);
         });

         storename.property("value", theElement.fxStoreType.fName)
            .on("change", () => {
               this.markChanged("storename", theIndex);
               theElement.fxStoreType.fName = storename.property("value").trim();
            });

         storesplit.property("value", theElement.fxStoreType.fiSplit || 1)
                   .on("change", () => {
                      theElement.fxStoreType.fiSplit = parseInt(storesplit.property("value"));
                      this.markChanged("storesplit", theIndex);
                   });

         storebuf.property("value", Math.round((theElement.fxStoreType.fiBufsize || 16*1024) / 1024))
                   .on("change", () => {
                      theElement.fxStoreType.fiBufsize = parseInt(storebuf.property("value")) * 1024;
                      this.markChanged("storebuf", theIndex);
                   });

         storecomp.property("value", theElement.fxStoreType.fiCompression || 0)
                   .on("change", () => {
                      theElement.fxStoreType.fiCompression = parseInt(storecomp.property("value"));
                      this.markChanged("storecomp", theIndex);
                   });

         storetreeasf.property("value", theElement.fxStoreType.fiAutosavesize || 0)
                   .on("change", () => {
                      theElement.fxStoreType.fiAutosavesize = parseInt(storetreeasf.property("value"));
                      this.markChanged("storeasf", theIndex);
                   });

         storeover.property("checked", theElement.fxStoreType.fbOverwrite || true)
                  .on("click", () => {
                     theElement.fxStoreType.fbOverwrite = storeover.property("checked");
                     this.markChanged("storeover", theIndex);
                   });

         this.showStepEditor(tab, theElement, theIndex); // handle all visibility issues here, also refresh tabs

      }); // steps loop

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
         .style('background-image', "url(" + GO4.source_dir + "icons/info1.png)")
         .style('display', 'none'); // do not show at the begin

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
            console.log("close analysis " + this.getItemName());
            GO4.ExecuteMethod(this, "UpdateFromUrl", "&close").then(() => {
               console.log("closing down analyis done. ");
            });
         });

      dom.select(".buttonSaveAnaASF")
         .style('background-image', "url(" + GO4.source_dir + "icons/filesave.png)");

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


      dom.select(".buttonSaveAnaConf")
         .style('background-image', "url(" + GO4.source_dir + "icons/filesave.png)")
         .on("click", event => {
            let content = dom.select(".anaprefs_name").property("value").trim(),
                requestmsg = "Really save analysis preferences: " + content;
            jsrp.createMenu(event, this)
                .then(menu => menu.runModal("Saving analysis preferences",`<p tabindex="0">${requestmsg}</p>`, { btns: true, height: 120, width: 400 }))
                .then(elem => {
                     if (!elem) return null;
                     this.markChanged("anaprefsname", 0);
                     this.stat.fxConfigFileName = content;
                     return GO4.ExecuteMethod(this, "UpdateFromUrl", "&saveprefs=" + content);
                 }).then(res => console.log(res !== null ? "Loading Saving preferences done. " : "Saving preferences  FAILED."));
         });

      dom.select(".buttonLoadAnaConf")
         .style('background-image', "url(" + GO4.source_dir + "icons/fileopen.png)")
         .on("click", event => {
            let content = dom.select(".anaprefs_name").property("value").trim(),
                requestmsg = "Really load analysis preferences: " + content;
            jsrp.createMenu(event, this)
                .then(menu => menu.runModal("Loading analysis preferences",`<p tabindex="0">${requestmsg}</p>`, { btns: true, height: 120, width: 400 }))
                .then(elem => (elem ? GO4.ExecuteMethod(this, "UpdateFromUrl", "&loadprefs=" + content) : null))
                .then(res => { if ((res!==null) && JSROOT.hpainter) JSROOT.hpainter.display(this.getItemName()); });
         });
   }

   GO4.AnalysisStatusEditor.prototype.redrawObject = function(obj /*, opt */) {
      if (obj._typename != this.stat._typename) return false;
      this.stat = JSROOT.clone(obj);
      this.clearChanges();
      this.fillEditor();
      return true;
   }

   GO4.drawGo4AnalysisStatus = function(domarg, stat) {
      let editor = new GO4.AnalysisStatusEditor(domarg, stat),
          dom = editor.selectDom(),
          h = dom.node().clientHeight,
          w = dom.node().clientWidth;

      if ((h < 10) && (w > 10)) dom.style("height", Math.round(w * 0.7)+"px");

      return JSROOT.httpRequest(GO4.source_dir + "html/analysiseditor.htm", "text").then(code => {
         dom.html(code);

         return JSROOT.httpRequest(GO4.source_dir + "html/stepeditor.htm", "text");

      }).then(step_code => {

         editor.stepPageHtml = step_code;

         editor.fillEditor();

         editor.setTopPainter();
         return editor;
      });
   }

});
