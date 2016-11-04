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

#include <Urho3D/Core/Context.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/UI/UIEvents.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/UI/Button.h>
#include <Urho3D/UI/CheckBox.h>
#include <Urho3D/UI/BorderImage.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/UI/Text.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture2D.h>

#include "LineComponent.h"

#include <Urho3D/DebugNew.h>
//=============================================================================
//=============================================================================
#define MIN_X_DIST        0.3f
#define FRACTION_LEN      0.2f
#define MIN_BEND_LEN     20.0f
#define MAX_BEND_LEN    100.0f

//=============================================================================
//=============================================================================
IntRect ConnectorLine::boxRect_(84,87,85,88);
IntVector2 ConnectorLine::boxSize_(16, 16);

void ConnectorLine::RegisterObject(Context* context)
{
    context->RegisterFactory<ConnectorLine>();
}

ConnectorLine::ConnectorLine(Context *context)
    : BorderImage(context)
    , numPoints_(0)
{
    SetEnabled(false);
}

ConnectorLine::~ConnectorLine()
{
}

void ConnectorLine::InitInternal()
{
    if (tailBorderImage_ == NULL)
    {
        rectSize_ = IntVector2(16, 16);
        rectColor_ = Color(0.2f, 0.8f, 0.8f);

        SetSize(rectSize_);
        SetColor(rectColor_);
        SetPriority(1);
        SetBringToFront(true);

        tailBorderImage_ = GetSubsystem<UI>()->GetRoot()->CreateChild<ConnectorLine>();
        tailBorderImage_->SetSize(rectSize_);
        tailBorderImage_->SetColor(Color(0.2f, 0.8f, 0.8f));
        tailBorderImage_->SetPriority(1);
        tailBorderImage_->SetBringToFront(true);

        lineBatcher_ = CreateChild<LineBatcher>();
        lineBatcher_->SetBringToBack(true);
    }
}

bool ConnectorLine::Create(Texture2D *tex2d, const IntRect &rect)
{
    assert(parentNode_ && "SetParent() must be called before create");

    InitInternal();

    lineBatcher_->SetLineTexture(tex2d);
    lineBatcher_->SetLineRect(rect);
    lineBatcher_->SetLineType(CURVE_LINE);
    lineBatcher_->SetLinePixelSize(2.0f);
    lineBatcher_->SetColor(Color::YELLOW * 0.9f);
    lineBatcher_->SetNumPointsPerSegment(NUM_PTS_PER_CURVE_SEGMENT);

    // update pos
    IntVector2 pos = parentNode_->GetPosition();
    IntVector2 size = parentNode_->GetSize();
    size.y_ = size.y_ - boxSize_.y_ - boxSize_.y_ / 2;
    parentPosOffset_ = pos + size;

    SetPosition(parentPosOffset_);
    tailBorderImage_->SetPosition(parentPosOffset_);

    return true;
}

void ConnectorLine::SetEnabled(bool enabled)
{
    if (parentNode_)
    {
        if (enabled)
        {
            SubscribeToEvent(parentNode_, E_LAYOUTUPDATED, URHO3D_HANDLER(ConnectorLine, HandleParentLayoutUpdated));
        }
        else
        {
            UnsubscribeFromEvent(parentNode_, E_LAYOUTUPDATED);
        }
    }
    UIElement::SetEnabled(enabled);
}

void ConnectorLine::SetPosition(int x, int y)
{
    SetPosition(IntVector2(x, y));
}

void ConnectorLine::SetPosition(const IntVector2& pos)
{
    UIElement::SetPosition(pos);
}

void ConnectorLine::SetTailPosition(const IntVector2& pos)
{
    tailBorderImage_->SetPosition(pos);
}

void ConnectorLine::OnDragBegin(const IntVector2& position, const IntVector2& screenPosition, 
                                int buttons, int qualifiers, Cursor* cursor)
{
    dragBeginPos_ = screenPosition;
}

void ConnectorLine::HandleParentLayoutUpdated(StringHash eventType, VariantMap& eventData)
{
    using namespace LayoutUpdated;

    UIElement *element = (UIElement*)eventData[P_ELEMENT].GetVoidPtr();

    IntVector2 pos = element->GetPosition();
    IntVector2 size = element->GetSize();
    size.y_ = size.y_ - boxSize_.y_ - boxSize_.y_ / 2;
    parentPosOffset_ = pos + size;

    SetPosition(parentPosOffset_);
    tailBorderImage_->SetPosition(parentPosOffset_);

    if (pointList_.Size() > 0 ) 
    {
        pointList_[0] = parentPosOffset_;
    }
}

void ConnectorLine::OnDragMove(const IntVector2& position, const IntVector2& screenPosition, 
                               const IntVector2& deltaPos, int buttons, int qualifiers, Cursor* cursor)
{
    IntVector2 halfBtnSize = boxSize_/2;

    if (pointList_.Size() == 0)
    {
        pointList_.Resize(MAX_POINTS);
        pointList_[0] = parentPosOffset_ + halfBtnSize;
    }

    // calculate pts
    // 1 - set ends: 0 & 4
    // 2 - calculate pts next to the ends: 1 & 3
    // 3 - pt 2 = avg of 1 & 3
    pointList_[4] = screenPosition;

    Vector2 p0 = Vector2((float)pointList_[0].x_, (float)pointList_[0].y_);
    Vector2 p4 = Vector2((float)pointList_[4].x_, (float)pointList_[4].y_);
    Vector2 dir(p4 - p0);
    Vector2 dirN = dir.Normalized();
    if (dirN.x_ < MIN_X_DIST) dirN.x_ = MIN_X_DIST;
    float dLen = Clamp(dir.Length()*FRACTION_LEN, MIN_BEND_LEN, MAX_BEND_LEN);

    pointList_[1] = IntVector2((int)(p0.x_ + dirN.x_*dLen), (int)(p0.y_ + dirN.y_*dLen*0.4f));
    pointList_[3] = IntVector2((int)(p4.x_ - dirN.x_*dLen), (int)(p4.y_ - dirN.y_*dLen*0.4f));
    pointList_[2] = (pointList_[1] + pointList_[3])/2;

    SetPosition( screenPosition - halfBtnSize );

    // draw call
    lineBatcher_->DrawPoints(pointList_);

    BringToFront();
    tailBorderImage_->BringToFront();
}

//=============================================================================
//=============================================================================
void LineTest::RegisterObject(Context* context)
{
    context->RegisterFactory<LineTest>();
}

LineTest::LineTest(Context *context)
    : UIElement(context)
{
}

LineTest::~LineTest()
{
}

bool LineTest::SetPoints(const PODVector<IntVector2> &points, LineType linetype, const Color& color, float pixelSize)
{
    if ( !CreateLineBatcher(linetype, color, pixelSize) )
        return false;

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Texture2D *uiTex2d = cache->GetResource<Texture2D>("Textures/UI.png");
    IntRect rect = ConnectorLine::GetBoxRect();

    for ( int i = 0; i < (int)points.Size(); ++i )
    {
        // point button
        IntVector2 pt = points[i];
        pointList_.Push(pt);

        Button *button = GetSubsystem<UI>()->GetRoot()->CreateChild<Button>();
        button->SetTexture(uiTex2d);
        button->SetImageRect(rect);
        button->SetPosition(points[i]);
        button->SetSize(ConnectorLine::GetBoxSize());
        button->SetOpacity(0.4f);
        button->SetVisible(true);
        SubscribeToEvent(button, E_DRAGMOVE, URHO3D_HANDLER(LineTest, HandleDragMove));

        buttonList_.Push(button);
    }

    lineBatcher_->DrawPoints(pointList_);

    return true;
}

bool LineTest::CreateLineBatcher(LineType linetype, const Color& color, float pixelSize)
{
    if (lineBatcher_)
        return false;

    ResourceCache* cache = GetSubsystem<ResourceCache>();
    Texture2D *uiTex2d = cache->GetResource<Texture2D>("Textures/UI.png");
    IntRect rect = ConnectorLine::GetBoxRect();

    pixelSize_ = pixelSize;

    lineBatcher_ = GetSubsystem<UI>()->GetRoot()->CreateChild<LineBatcher>();
    lineBatcher_->SetLineTexture(uiTex2d);
    lineBatcher_->SetLineRect(rect);
    lineBatcher_->SetLineType(linetype);
    lineBatcher_->SetLinePixelSize(pixelSize_);
    lineBatcher_->SetColor(color);

    if (linetype == STRAIGHT_LINE)
    {
        lineBatcher_->SetNumPointsPerSegment(0);
    }
    else
    {
        lineBatcher_->SetNumPointsPerSegment(NUM_PTS_PER_CURVE_SEGMENT);
    }

    return true;
}

void LineTest::HandleDragMove(StringHash eventType, VariantMap& eventData)
{
    using namespace DragMove;
    Button* button = (Button*)eventData[P_ELEMENT].GetVoidPtr();
    int buttons = eventData[P_BUTTONS].GetInt();
    IntVector2 d = button->GetVar("DELTA").GetIntVector2();
    int X = eventData[P_X].GetInt() + d.x_;
    int Y = eventData[P_Y].GetInt() + d.y_;
    //int BUTTONS = element->GetVar("BUTTONS").GetInt();

    for ( unsigned i = 0; i < buttonList_.Size(); ++i )
    {
        if ( button == buttonList_[i] )
        {
            pointList_[i] = IntVector2(X, Y);
            button->SetPosition(X, Y);
        }
    }
    //if (GetParent())
    //{
    //    IntVector2 newPos(X, Y);
    //    IntVector2 pPos(GetParent()->GetPosition());
    //    IntVector2 pSize(GetParent()->GetSize());
    //    if ( newPos.x_ > pPos.x_ && newPos.x_ < pPos.x_ + pSize.x_ - element->GetSize().x_ && 
    //         newPos.y_ > pPos.y_ && newPos.y_ < pPos.y_ + pSize.y_ - element->GetSize().y_ )
    //    {
    //        element->SetPosition( newPos - pPos );
    //    }
    //}
    //else
    //{
    //    element->SetPosition(IntVector2(X, Y));
    //}

    lineBatcher_->DrawPoints(pointList_);

    // line segments
    //AjustLines(element);
}




