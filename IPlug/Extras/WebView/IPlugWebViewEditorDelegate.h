 /*
 ==============================================================================
 
 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.
 
 See LICENSE.txt for  more info.
 
 ==============================================================================
*/

#pragma once

#include "IPlugEditorDelegate.h"
#include "IPlugWebView.h"

#include <functional>


BEGIN_IPLUG_NAMESPACE

/** This EditorDelegate allows using WKWebKitView for an iPlug user interface on macOS/iOS... */
class WebViewEditorDelegate : public IEditorDelegate
                            , public IWebView
{
public:
  WebViewEditorDelegate(int nParams);
  virtual ~WebViewEditorDelegate();
  
  //IEditorDelegate
  void* OpenWindow(void* pParent) override;
  void CloseWindow() override;
  
  void SendControlValueFromDelegate(int ctrlTag, double normalizedValue) override;
  void SendControlMsgFromDelegate(int ctrlTag, int msgTag, int dataSize, const void* pData) override;
  void SendParameterValueFromDelegate(int paramIdx, double value, bool normalized) override;
  void SendArbitraryMsgFromDelegate(int msgTag, int dataSize, const void* pData) override;
  void Resize(int width, int height);

protected:
  std::function<void()> mEditorInitFunc = nullptr;
};

END_IPLUG_NAMESPACE
