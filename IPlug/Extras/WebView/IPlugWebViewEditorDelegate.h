 /*
 ==============================================================================
 
 This file is part of the iPlug 2 library. Copyright (C) the iPlug 2 developers.
 
 See LICENSE.txt for  more info.
 
 ==============================================================================
*/

#pragma once

#include "IPlugEditorDelegate.h"
#include "IPlugWebView.h"
#include "wdl_base64.h"
#include "json11.hpp"
#include <functional>

BEGIN_IPLUG_NAMESPACE

/** This Editor Delegate allows using WKWebKitView for an iPlug user interface on macOS/iOS and IWebView2 on Windows */
class WebViewEditorDelegate : public IEditorDelegate
                            , public IWebView
{
public:
  WebViewEditorDelegate(int nParams);
  virtual ~WebViewEditorDelegate();
  
  //IEditorDelegate
  void* OpenWindow(void* pParent) override;
  
  void CloseWindow() override
  {
    CloseWebView();
  }

  void SendControlValueFromDelegate(int ctrlTag, double normalizedValue) override
  {
    WDL_String str;
    str.SetFormatted(50, "SCVFD(%i, %f)", ctrlTag, normalizedValue);
    EvaluateJavaScript(str.Get());
  }

  void SendControlMsgFromDelegate(int ctrlTag, int msgTag, int dataSize, const void* pData) override
  {
    WDL_String str;
    WDL_TypedBuf<char> base64;
    int sizeOfBase64 = 4 * std::ceil(((double) dataSize/3.));
    base64.Resize(sizeOfBase64);
    wdl_base64encode(reinterpret_cast<const unsigned char*>(pData), base64.GetFast(), dataSize);
    str.SetFormatted(50, "SCMFD(%i, %i, %i, %s)", ctrlTag, msgTag, dataSize, base64.GetFast());
    EvaluateJavaScript(str.Get());
  }

  void SendParameterValueFromDelegate(int paramIdx, double value, bool normalized) override
  {
    WDL_String str;
    str.SetFormatted(50, "SPVFD(%i, %f)", paramIdx, value);
    EvaluateJavaScript(str.Get());
  }

  void SendArbitraryMsgFromDelegate(int msgTag, int dataSize, const void* pData) override
  {
    WDL_String str;
    WDL_TypedBuf<char> base64;
    int sizeOfBase64 = 4 * std::ceil(((double) dataSize/3.));
    base64.Resize(sizeOfBase64);
    wdl_base64encode(reinterpret_cast<const unsigned char*>(pData), base64.GetFast(), dataSize);
    str.SetFormatted(50, "SAMFD(%i, %i, %s)", msgTag, dataSize, base64.GetFast());
    EvaluateJavaScript(str.Get());
  }

  void OnMessageFromWebView(const char* jsonStr) override
  {
    std::string err;

    json11::Json json = json11::Json::parse(jsonStr, err);
    
    if(err.length())
    {
      DBGMSG("%s\n", err.c_str());
      return;
    }
    else
    {
      if(json["msg"] == "SPVFUI")
      {
        SendParameterValueFromUI(json["paramIdx"].int_value(), json["value"].number_value());
      }
      else if (json["msg"] == "BPCFUI")
      {
        BeginInformHostOfParamChangeFromUI(json["paramIdx"].int_value());
      }
      else if (json["msg"] == "EPCFUI")
      {
        EndInformHostOfParamChangeFromUI(json["paramIdx"].int_value());
      }
      else if (json["msg"] == "SAMFUI")
      {
        SendArbitraryMsgFromUI(json["msgTag"].int_value(), json["ctrlTag"].int_value(), json["dataSize"].int_value(), /*dataSize > 0 ? json["data"] :*/ nullptr);
      }
    }
  }

  void Resize(int width, int height)
  {
    SetWebViewBounds(0, 0, width, height);
    EditorResizeFromUI(width, height, true);
  }
  
protected:
  std::function<void()> mEditorInitFunc = nullptr;
};

END_IPLUG_NAMESPACE
