 /*
 ==============================================================================
 
 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.
 
 See LICENSE.txt for  more info.
 
 ==============================================================================
*/

#pragma once

#include "IPlugPlatform.h"
#include "wdlstring.h"

#if defined OS_MAC
  #define PLATFORM_VIEW NSView
  #define PLATFORM_RECT NSRect
  #define MAKERECT NSMakeRect
#elif defined OS_IOS
  #define PLATFORM_VIEW UIView
  #define PLATFORM_RECT CGRect
  #define MAKERECT CGRectMake
#endif

BEGIN_IPLUG_NAMESPACE

/** This EditorDelegate allows using WKWebKitView for an iPlug user interface on macOS/iOS... */
class IWebView
{
public:
  IWebView();
  virtual ~IWebView();
  
  void* OpenWebView(int x, int y, int w, int h);
  void CloseWebView();
  
  void LoadHTML(const WDL_String& html);
  void LoadURL(const char* url);
  void LoadFile(const char* fileName);
  void EvaluateJavaScript(const char* scriptStr);
  void EnableScroll(bool enable);
  
  virtual void OnWebContentLoaded() {};
  
private:
  
#if defined OS_MAC || defined OS_IOS
  void* mWKWebView = nullptr;
  void* mWebConfig = nullptr;
  void* mScriptHandler = nullptr;
#endif
};

END_IPLUG_NAMESPACE
