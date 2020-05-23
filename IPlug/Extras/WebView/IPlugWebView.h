 /*
 ==============================================================================
 
 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.
 
 See LICENSE.txt for  more info.
 
 ==============================================================================
*/

#pragma once

#include "IPlugPlatform.h"
#include "wdlstring.h"
#include <functional>

#if defined OS_MAC
  #define PLATFORM_VIEW NSView
  #define PLATFORM_RECT NSRect
  #define MAKERECT NSMakeRect
#elif defined OS_IOS
  #define PLATFORM_VIEW UIView
  #define PLATFORM_RECT CGRect
  #define MAKERECT CGRectMake
#elif defined OS_WIN
  #include <wrl.h>
  #include <wil/com.h>
  #include "WebView2.h"
#endif

BEGIN_IPLUG_NAMESPACE

using completionHandlerFunc = std::function<void(const WDL_String& result)>;

/** TODO */
class IWebView
{
public:
  IWebView();
  virtual ~IWebView();
  
  void* OpenWebView(void* pParent, float x, float y, float w, float h, float scale = 1.);
  void CloseWebView();
  
  void LoadHTML(const WDL_String& html);
  void LoadURL(const char* url);
  void LoadFile(const char* fileName);
  void EvaluateJavaScript(const char* scriptStr, completionHandlerFunc func);
  void EnableScroll(bool enable);
  void SetWebViewBounds(float x, float y, float w, float h, float scale = 1.);

  virtual void OnWebViewReady() {};
  virtual void OnWebContentLoaded() {};
  
private:
  
#if defined OS_MAC || defined OS_IOS
  void* mWKWebView = nullptr;
  void* mWebConfig = nullptr;
  void* mScriptHandler = nullptr;
#elif defined OS_WIN
  wil::com_ptr<ICoreWebView2Controller> mWebViewCtrlr;
  wil::com_ptr<ICoreWebView2> mWebViewWnd;
#endif
};

END_IPLUG_NAMESPACE
