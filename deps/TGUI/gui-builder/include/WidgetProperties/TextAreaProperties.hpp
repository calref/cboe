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


#ifndef TGUI_GUI_BUILDER_TEXT_AREA_PROPERTIES_HPP
#define TGUI_GUI_BUILDER_TEXT_AREA_PROPERTIES_HPP

#include "WidgetProperties.hpp"
#include <TGUI/Widgets/TextArea.hpp>

struct TextAreaProperties : WidgetProperties
{
    // TODO: Scrollbar renderer

    void updateProperty(tgui::Widget::Ptr widget, const tgui::String& property, const tgui::String& value) const override
    {
        auto textArea = widget->cast<tgui::TextArea>();
        if (property == "Text")
            textArea->setText(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, value).getString());
        else if (property == "DefaultText")
            textArea->setDefaultText(tgui::Deserializer::deserialize(tgui::ObjectConverter::Type::String, value).getString());
        else if (property == "TextSize")
            textArea->setTextSize(value.toUInt());
        else if (property == "MaximumCharacters")
            textArea->setMaximumCharacters(value.toUInt());
        else if (property == "ReadOnly")
            textArea->setReadOnly(parseBoolean(value, false));
        else if (property == "VerticalScrollbarPolicy")
            textArea->setVerticalScrollbarPolicy(deserializeScrollbarPolicy(value));
        else if (property == "HorizontalScrollbarPolicy")
            textArea->setHorizontalScrollbarPolicy(deserializeScrollbarPolicy(value));
        else
            WidgetProperties::updateProperty(widget, property, value);
    }

    PropertyValueMapPair initProperties(tgui::Widget::Ptr widget) const override
    {
        auto pair = WidgetProperties::initProperties(widget);
        auto textArea = widget->cast<tgui::TextArea>();
        pair.first["Text"] = {"MultilineString", tgui::Serializer::serialize(textArea->getText())};
        pair.first["DefaultText"] = {"MultilineString", tgui::Serializer::serialize(textArea->getDefaultText())};
        pair.first["TextSize"] = {"UInt", tgui::String::fromNumber(textArea->getTextSize())};
        pair.first["MaximumCharacters"] = {"UInt", tgui::String::fromNumber(textArea->getMaximumCharacters())};
        pair.first["ReadOnly"] = {"Bool", tgui::Serializer::serialize(textArea->isReadOnly())};
        pair.first["VerticalScrollbarPolicy"] = {"Enum{Automatic,Always,Never}", serializeScrollbarPolicy(textArea->getVerticalScrollbarPolicy())};
        pair.first["HorizontalScrollbarPolicy"] = {"Enum{Automatic,Always,Never}", serializeScrollbarPolicy(textArea->getHorizontalScrollbarPolicy())};

        const auto renderer = textArea->getSharedRenderer();
        pair.second["Borders"] = {"Outline", renderer->getBorders().toString()};
        pair.second["Padding"] = {"Outline", renderer->getPadding().toString()};
        pair.second["BackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getBackgroundColor())};
        pair.second["TextColor"] = {"Color", tgui::Serializer::serialize(renderer->getTextColor())};
        pair.second["SelectedTextColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextColor())};
        pair.second["SelectedTextBackgroundColor"] = {"Color", tgui::Serializer::serialize(renderer->getSelectedTextBackgroundColor())};
        pair.second["BorderColor"] = {"Color", tgui::Serializer::serialize(renderer->getBorderColor())};
        pair.second["CaretColor"] = {"Color", tgui::Serializer::serialize(renderer->getCaretColor())};
        pair.second["CaretWidth"] = {"Float", tgui::String::fromNumber(renderer->getCaretWidth())};
        pair.second["TextureBackground"] = {"Texture", tgui::Serializer::serialize(renderer->getTextureBackground())};
        pair.second["ScrollbarWidth"] = {"Float", tgui::String::fromNumber(renderer->getScrollbarWidth())};
        return pair;
    }

private:

    static tgui::EditBox::Alignment deserializeAlignment(tgui::String value)
    {
        value = value.trim().toLower();
        if (value == "right")
            return tgui::EditBox::Alignment::Right;
        else if (value == "center")
            return tgui::EditBox::Alignment::Center;
        else
            return tgui::EditBox::Alignment::Left;
    }

    static tgui::String serializeAlignment(tgui::EditBox::Alignment alignment)
    {
        if (alignment == tgui::EditBox::Alignment::Center)
            return "Center";
        else if (alignment == tgui::EditBox::Alignment::Right)
            return "Right";
        else
            return "Left";
    }
};

#endif // TGUI_GUI_BUILDER_TEXT_AREA_PROPERTIES_HPP
