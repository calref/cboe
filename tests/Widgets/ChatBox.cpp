/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// TGUI - Texus' Graphical User Interface
// Copyright (C) 2012-2022 Bruno Van de Velde (vdv_b@tgui.eu)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Tests.hpp"
#include <TGUI/Widgets/ChatBox.hpp>

TEST_CASE("[ChatBox]")
{
    tgui::ChatBox::Ptr chatBox = tgui::ChatBox::create();
    chatBox->getRenderer()->setFont("resources/DejaVuSans.ttf");

    SECTION("WidgetType")
    {
        REQUIRE(chatBox->getWidgetType() == "ChatBox");
    }

    SECTION("Position and Size")
    {
        chatBox->setPosition(40, 30);
        chatBox->setSize(150, 100);
        chatBox->getRenderer()->setBorders(2);

        REQUIRE(chatBox->getPosition() == tgui::Vector2f(40, 30));
        REQUIRE(chatBox->getSize() == tgui::Vector2f(150, 100));
        REQUIRE(chatBox->getFullSize() == chatBox->getSize());
        REQUIRE(chatBox->getWidgetOffset() == tgui::Vector2f(0, 0));
    }

    SECTION("Adding lines")
    {
        chatBox->setTextColor(tgui::Color::Black);
        chatBox->setTextStyle(tgui::TextStyle::Bold);

        REQUIRE(chatBox->getLineAmount() == 0);
        chatBox->addLine("Line 1");
        REQUIRE(chatBox->getLineAmount() == 1);
        chatBox->addLine("Line 2", tgui::Color::Green);
        REQUIRE(chatBox->getLineAmount() == 2);
        chatBox->addLine("Line 3", tgui::Color::Blue, tgui::TextStyle::Italic);
        REQUIRE(chatBox->getLineAmount() == 3);

        REQUIRE(chatBox->getLine(0) == "Line 1");
        REQUIRE(chatBox->getLine(1) == "Line 2");
        REQUIRE(chatBox->getLine(2) == "Line 3");

        REQUIRE(chatBox->getLineColor(0) == tgui::Color::Black);
        REQUIRE(chatBox->getLineColor(1) == tgui::Color::Green);
        REQUIRE(chatBox->getLineColor(2) == tgui::Color::Blue);

        REQUIRE(chatBox->getLineTextStyle(0) == tgui::TextStyle::Bold);
        REQUIRE(chatBox->getLineTextStyle(1) == tgui::TextStyle::Bold);
        REQUIRE(chatBox->getLineTextStyle(2) == tgui::TextStyle::Italic);
    }

    SECTION("Removing lines")
    {
        REQUIRE(!chatBox->removeLine(0));
        chatBox->addLine("Line 1");
        chatBox->addLine("Line 2");
        chatBox->addLine("Line 3");

        REQUIRE(!chatBox->removeLine(5));
        REQUIRE(chatBox->getLineAmount() == 3);

        REQUIRE(chatBox->removeLine(1));
        REQUIRE(chatBox->getLineAmount() == 2);
        REQUIRE(chatBox->getLine(0) == "Line 1");
        REQUIRE(chatBox->getLine(1) == "Line 3");

        chatBox->removeAllLines();
        REQUIRE(chatBox->getLineAmount() == 0);
    }

    SECTION("Line limit")
    {
        REQUIRE(chatBox->getLineLimit() == 0);

        SECTION("Oldest on top")
        {
            chatBox->addLine("Line 1");
            chatBox->addLine("Line 2");
            chatBox->addLine("Line 3");

            chatBox->setLineLimit(2);
            REQUIRE(chatBox->getLineLimit() == 2);

            REQUIRE(chatBox->getLineAmount() == 2);
            REQUIRE(chatBox->getLine(0) == "Line 2");
            REQUIRE(chatBox->getLine(1) == "Line 3");

            chatBox->addLine("Line 4");
            REQUIRE(chatBox->getLine(0) == "Line 3");
            REQUIRE(chatBox->getLine(1) == "Line 4");
        }

        SECTION("Oldest at the bottom")
        {
            chatBox->setNewLinesBelowOthers(false);

            chatBox->addLine("Line 1");
            chatBox->addLine("Line 2");
            chatBox->addLine("Line 3");

            chatBox->setLineLimit(2);
            REQUIRE(chatBox->getLineLimit() == 2);

            REQUIRE(chatBox->getLineAmount() == 2);
            REQUIRE(chatBox->getLine(0) == "Line 3");
            REQUIRE(chatBox->getLine(1) == "Line 2");

            chatBox->addLine("Line 4");
            REQUIRE(chatBox->getLine(0) == "Line 4");
            REQUIRE(chatBox->getLine(1) == "Line 3");
        }
    }

    SECTION("Text size")
    {
        chatBox->setTextSize(30);
        REQUIRE(chatBox->getTextSize() == 30);
    }

    SECTION("Default text color")
    {
        chatBox->setTextColor(tgui::Color::Red);
        REQUIRE(chatBox->getTextColor() == tgui::Color::Red);

        chatBox->addLine("Text");
        REQUIRE(chatBox->getLineColor(0) == tgui::Color::Red);
    }

    SECTION("Default text style")
    {
        chatBox->setTextStyle(tgui::TextStyle::StrikeThrough);
        REQUIRE(chatBox->getTextStyle() == tgui::TextStyle::StrikeThrough);

        chatBox->addLine("Text");
        REQUIRE(chatBox->getLineTextStyle(0) == tgui::TextStyle::StrikeThrough);
    }

    SECTION("Get unexisting line")
    {
        chatBox->setTextColor(tgui::Color::Yellow);

        chatBox->addLine("Text", tgui::Color::Blue);
        REQUIRE(chatBox->getLine(1) == "");
        REQUIRE(chatBox->getLineColor(1) == tgui::Color::Yellow);
    }

    SECTION("Lines start from top or bottom")
    {
        REQUIRE(!chatBox->getLinesStartFromTop());
        chatBox->setLinesStartFromTop(true);
        REQUIRE(chatBox->getLinesStartFromTop());
        chatBox->setLinesStartFromTop(false);
        REQUIRE(!chatBox->getLinesStartFromTop());
    }

    SECTION("Events / Signals")
    {
        SECTION("Widget")
        {
            testWidgetSignals(chatBox);
        }

        // TODO
    }

    testWidgetRenderer(chatBox->getRenderer());
    SECTION("Renderer")
    {
        auto renderer = chatBox->getRenderer();

        SECTION("colored")
        {
            tgui::ScrollbarRenderer scrollbarRenderer;
            scrollbarRenderer.setTrackColor(tgui::Color::Red);
            scrollbarRenderer.setThumbColor(tgui::Color::Blue);

            SECTION("set serialized property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", "rgb(10, 20, 30)"));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", "rgb(40, 50, 60)"));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", "(1, 2, 3, 4)"));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", "(5, 6, 7, 8)"));
                REQUIRE_NOTHROW(renderer->setProperty("Scrollbar", "{ TrackColor = Red; ThumbColor = Blue; }"));
                REQUIRE_NOTHROW(renderer->setProperty("ScrollbarWidth", "15"));
            }

            SECTION("set object property")
            {
                REQUIRE_NOTHROW(renderer->setProperty("BackgroundColor", tgui::Color{10, 20, 30}));
                REQUIRE_NOTHROW(renderer->setProperty("BorderColor", tgui::Color{40, 50, 60}));
                REQUIRE_NOTHROW(renderer->setProperty("Borders", tgui::Borders{1, 2, 3, 4}));
                REQUIRE_NOTHROW(renderer->setProperty("Padding", tgui::Borders{5, 6, 7, 8}));
                REQUIRE_NOTHROW(renderer->setProperty("Scrollbar", scrollbarRenderer.getData()));
                REQUIRE_NOTHROW(renderer->setProperty("ScrollbarWidth", 15));
            }

            SECTION("functions")
            {
                renderer->setBackgroundColor({10, 20, 30});
                renderer->setBorderColor({40, 50, 60});
                renderer->setBorders({1, 2, 3, 4});
                renderer->setPadding({5, 6, 7, 8});
                renderer->setScrollbar(scrollbarRenderer.getData());
                renderer->setScrollbarWidth(15);
            }

            REQUIRE(renderer->getProperty("BackgroundColor").getColor() == tgui::Color(10, 20, 30));
            REQUIRE(renderer->getProperty("BorderColor").getColor() == tgui::Color(40, 50, 60));
            REQUIRE(renderer->getProperty("Borders").getOutline() == tgui::Borders(1, 2, 3, 4));
            REQUIRE(renderer->getProperty("Padding").getOutline() == tgui::Borders(5, 6, 7, 8));
            REQUIRE(renderer->getProperty("ScrollbarWidth").getNumber() == 15);

            REQUIRE(renderer->getScrollbar()->propertyValuePairs.size() == 2);
            REQUIRE(renderer->getScrollbar()->propertyValuePairs["TrackColor"].getColor() == tgui::Color::Red);
            REQUIRE(renderer->getScrollbar()->propertyValuePairs["ThumbColor"].getColor() == tgui::Color::Blue);
        }

        SECTION("textured")
        {
            tgui::Texture textureBackground("resources/Black.png", {0, 154, 48, 48}, {16, 16, 16, 16});

            SECTION("set serialized property") {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", tgui::Serializer::serialize(textureBackground)));
            }

            SECTION("set object property") {
                REQUIRE_NOTHROW(renderer->setProperty("TextureBackground", textureBackground));
            }

            SECTION("functions") {
                renderer->setTextureBackground(textureBackground);
            }

            REQUIRE(renderer->getProperty("TextureBackground").getTexture().getData() != nullptr);

            REQUIRE(renderer->getTextureBackground().getData() == textureBackground.getData());
        }
    }

    SECTION("Saving and loading from file")
    {
        chatBox->setTextColor(tgui::Color::White);
        chatBox->setTextStyle(tgui::TextStyle::Underlined);
        chatBox->setTextSize(34);
        chatBox->setLineLimit(5);
        chatBox->setLinesStartFromTop(true);
        chatBox->setNewLinesBelowOthers(false);
        chatBox->addLine("L2");
        chatBox->addLine("L4", tgui::Color::Magenta, tgui::TextStyle::Bold | tgui::TextStyle::StrikeThrough);
        chatBox->addLine("L3");
        chatBox->addLine("L1", tgui::Color::Cyan);

        testSavingWidget("ChatBox", chatBox);

        // Make sure that the lines are still in the correct order
        REQUIRE(chatBox->getLine(0) == "L1");
        REQUIRE(chatBox->getLine(1) == "L3");
        REQUIRE(chatBox->getLine(2) == "L4");
        REQUIRE(chatBox->getLine(3) == "L2");
    }
}
