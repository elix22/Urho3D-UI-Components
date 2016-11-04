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

#include "LineBatcher.h"

namespace Urho3D
{
class Button;
class CheckBox;
class BorderImage;
class Text;
}

using namespace Urho3D;

class LineBatcher;

//=============================================================================
//=============================================================================
enum IOType
{
    IO_Input,
    IO_Output,
};

class ConnectorLine : public BorderImage
{
    URHO3D_OBJECT(ConnectorLine, BorderImage);

    // connection curve only requires 5 points
    enum PointSizeType{ MAX_POINTS = 5 };
public:
    static void RegisterObject(Context* context);

    ConnectorLine(Context *context);
    virtual ~ConnectorLine();

    static IntRect& GetBoxRect()    { return boxRect_; }
    static IntVector2& GetBoxSize() { return boxSize_; }

    void SetParent(UIElement *uiparent) { parentNode_ = uiparent; }
    bool Create(Texture2D *tex2d, const IntRect &rect);

    BorderImage* GetTailImage(){ return tailBorderImage_; }
    void SetEnabled(bool enabled);
    void SetPosition(int x, int y);
    void SetPosition(const IntVector2& pos);
    void SetTailPosition(const IntVector2& pos);

    virtual void OnDragBegin(const IntVector2& position, const IntVector2& screenPosition, 
                             int buttons, int qualifiers, Cursor* cursor);
    virtual void OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                            const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor);

protected:
    void InitInternal();
    void HandleParentLayoutUpdated(StringHash eventType, VariantMap& eventData);

protected:
    static IntRect          boxRect_;
    static IntVector2       boxSize_;

    WeakPtr<UIElement>      parentNode_;
    WeakPtr<LineBatcher>    lineBatcher_;
    IntVector2              parentPosOffset_;
    IntVector2              rectSize_;
    Color                   rectColor_;
    WeakPtr<BorderImage>    tailBorderImage_;

    PODVector<IntVector2>   pointList_;
    int                     numPoints_;
    IntVector2              dragBeginPos_;
};

//=============================================================================
//=============================================================================
class LineTest : public UIElement
{
    URHO3D_OBJECT(LineTest, UIElement);
public:
    static void RegisterObject(Context* context);

    LineTest(Context *context);
    virtual ~LineTest();
    bool SetPoints(const PODVector<IntVector2> &points, LineType linetype, const Color& color, float pixelSize);

protected:
    bool CreateLineBatcher(LineType linetype, const Color& color, float pixelSize);
    void HandleDragMove(StringHash eventType, VariantMap& eventData);

protected:
    WeakPtr<LineBatcher>  lineBatcher_;
    PODVector<IntVector2> pointList_;
    Vector<Button*>       buttonList_;
    LineType              linetype_;
    float                 pixelSize_;
};
