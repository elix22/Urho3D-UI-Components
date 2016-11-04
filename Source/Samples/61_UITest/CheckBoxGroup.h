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
#include <Urho3D/UI/Text.h>

namespace Urho3D
{
class Text;
}
using namespace Urho3D;
//=============================================================================
//=============================================================================
/// group checkbox toggled.
URHO3D_EVENT(E_CHECKGROUPTOGGLED, CheckGroupToggled)
{
    URHO3D_PARAM(P_ELEMENT, Element);              // UIElement pointer
    URHO3D_PARAM(P_INDEX, Index);                  // int
}

//=============================================================================
//=============================================================================
class CheckText : public Text
{
    URHO3D_OBJECT(CheckText, Text);
public:
    static void RegisterObject(Context* context);

    CheckText(Context *context) : Text(context){}
    virtual ~CheckText(){}

    virtual void OnClickBegin(const IntVector2& position, const IntVector2& screenPosition, 
                              int button, int buttons, int qualifiers, Cursor* cursor);

};


struct CheckboxDesc
{
    WeakPtr<CheckBox>  checkBox_;
    WeakPtr<CheckText> desc_;
    WeakPtr<UIElement> element_;
};


class CheckBoxGroup : public BorderImage
{
    URHO3D_OBJECT(CheckBoxGroup, BorderImage);
public:
    static void RegisterObject(Context* context);

    CheckBoxGroup(Context *context);
    virtual ~CheckBoxGroup();

    CheckboxDesc* CreateCheckboxDesc(const String& desc);
    CheckboxDesc* GetCheckboxDesc(unsigned idx);

    UIElement* GetHeaderElement() { return headerElement_; }
    Text* GetTitleTextElement();

    void SetEnabled(bool enabled);
    void SetSize(int width, int height);
    void SetSize(const IntVector2& size);

protected:
    void InitInternal();
    void HandleCheckbox(StringHash eventType, VariantMap& eventData);
    void HandlePressed(StringHash eventType, VariantMap& eventData);
    void SendGroupToggleEvent(int idx);

protected:
    WeakPtr<UIElement>   headerElement_;
    WeakPtr<Text>        titleText_;

    Vector<CheckboxDesc> childList_;
    IntVector2           internalSize_; 
};


