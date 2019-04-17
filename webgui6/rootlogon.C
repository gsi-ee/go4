// macro configures usage of full-functional TWebCanvas

{
   gPluginMgr->AddHandler("TGuiFactory", "webfull", "TWebGuiFactoryFull", "WebGuiFull", "TWebGuiFactoryFull()");
   gEnv->SetValue("Gui.Factory", "webfull");
}
