//
// Copyright (c) 2008-2016 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
#pragma once
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/Core/Variant.h>
#include "LineComponent.h"

namespace Urho3D
{
class Text;
}

using namespace Urho3D;
//=============================================================================
//=============================================================================
typedef void (UIElement::*VarChangedCallback)(Variant &var);

class SliderVariable : public BorderImage
{
    URHO3D_OBJECT(SliderVariable, BorderImage);
public:

    static void RegisterObject(Context* context);

    SliderVariable(Context *context);
    virtual ~SliderVariable();

    bool CreateBar(const IntVector2 &size);

    virtual void OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                            const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor);

    void SetRange(Variant &vmin, Variant &vmax);
    void SetCurrentValue(Variant &val);
    void SetSensitivity(float sensitivity) { sensitivity_ = sensitivity; }

    void SetVarChangedCallback(UIElement *process, VarChangedCallback callback)
    {
        processCaller = process;
        pfnVarChangedCallback = callback;
    }

protected:
    void ValueUpdate(float delta);

protected:
    WeakPtr<Text>       variableText_;
    UIElement           *processCaller;
    VarChangedCallback  pfnVarChangedCallback;

    Variant             varMin_;
    Variant             varMax_;
    Variant             varCurrentValue_;
    float               sensitivity_;
};

class SlideBar : public BorderImage
{
    URHO3D_OBJECT(SlideBar, BorderImage);
public:
    static void RegisterObject(Context* context);

    SlideBar(Context *context);
    virtual ~SlideBar();

    virtual void OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                            const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor);

    BorderImage* CreateBar(const IntVector2 &size);

    UIElement* GetHeaderElement()   { return headerElement_;}
    Text *GetHeaderText()           { return headerText_;    }

    void SetSize(int width, int height);
    void SetSize(const IntVector2& size);
    void SetSliderColor(const Color& color);
    void SetEnabled(bool enable);

    void SetRange(Variant &vmin, Variant &vmax);
    void SetCurrentValue(Variant &val);
    void SetSensitivity(float sensitivity) { sensitivity_ = sensitivity; }

    void SetVarChangedCallback(UIElement *process, VarChangedCallback callback)
    {
        processCaller = process;
        pfnVarChangedCallback = callback;
    }

protected:
    void InitInternal(const IntVector2 &size);
    void ValueUpdate(float val);
    void CreateOutputConnector();

protected:
    UIElement           *processCaller;
    VarChangedCallback  pfnVarChangedCallback;

    WeakPtr<UIElement>     headerElement_;
    WeakPtr<BorderImage>   sliderElement_;
    WeakPtr<ConnectorLine> outputConnector_;

    WeakPtr<Text>          headerText_;
    WeakPtr<Text>          variableText_;
    Color                  internalColor_;
    IntVector2             storedSize_;
                          
    Variant                varMin_;
    Variant                varMax_;
    Variant                varCurrentValue_;
    float                  sensitivity_;
};

