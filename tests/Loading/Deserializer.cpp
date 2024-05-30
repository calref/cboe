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
#include <TGUI/Loading/Deserializer.hpp>

#if TGUI_HAS_BACKEND_SFML
    #include <SFML/System/Err.hpp>
#endif

using Type = tgui::ObjectConverter::Type;

TEST_CASE("[Deserializer]")
{
    SECTION("deserialize bool")
    {
        REQUIRE(tgui::Deserializer::deserialize(Type::Bool, "True").getBool());
        REQUIRE(!tgui::Deserializer::deserialize(Type::Bool, "FALSE").getBool());
        REQUIRE(tgui::Deserializer::deserialize(Type::Bool, "YeS").getBool());
        REQUIRE(!tgui::Deserializer::deserialize(Type::Bool, "nO").getBool());
        REQUIRE(tgui::Deserializer::deserialize(Type::Bool, "1").getBool());
        REQUIRE(!tgui::Deserializer::deserialize(Type::Bool, "0").getBool());

        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Bool, "InvalidString"), tgui::Exception);
    }

    SECTION("deserialize font")
    {
        REQUIRE(tgui::Deserializer::deserialize(Type::Font, "resources/DejaVuSans.ttf").getFont() != nullptr);
        REQUIRE(tgui::Deserializer::deserialize(Type::Font, "\"resources/DejaVuSans.ttf\"").getFont() != nullptr);
        REQUIRE(tgui::Deserializer::deserialize(Type::Font, "nullptr").getFont() == nullptr);
        REQUIRE(tgui::Deserializer::deserialize(Type::Font, "null").getFont() == nullptr);

#if TGUI_HAS_BACKEND_SFML
        std::streambuf *oldbuf = sf::err().rdbuf(0); // Prevent SFML from printing a warning
#endif

        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Font, "NonExistentFile"), tgui::Exception);

#if TGUI_HAS_BACKEND_SFML
        sf::err().rdbuf(oldbuf);
#endif
    }

    SECTION("deserialize color")
    {
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, ",,").getColor() == tgui::Color(0, 0, 0));
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "10, 20, 30").getColor() == tgui::Color(10, 20, 30));
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "40, 50, 60, 70").getColor() == tgui::Color(40, 50, 60, 70));
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "(80, 90, 100)").getColor() == tgui::Color(80, 90, 100));
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "(110, 120, 130, 140)").getColor() == tgui::Color(110, 120, 130, 140));
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "rgb(100, 0, 50)").getColor() == tgui::Color(100, 0, 50));
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "rgba(100, 0, 50, 200)").getColor() == tgui::Color(100, 0, 50, 200));
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "#123").getColor() == tgui::Color(17, 34, 51));
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "#4567").getColor() == tgui::Color(68, 85, 102, 119));
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "#89ABCD").getColor() == tgui::Color(137, 171, 205));
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "#FEDCBA98").getColor() == tgui::Color(254, 220, 186, 152));

        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "BLACK").getColor() == tgui::Color::Black);
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "White").getColor() == tgui::Color::White);
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "red").getColor() == tgui::Color::Red);
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "green").getColor() == tgui::Color::Green);
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "blue").getColor() == tgui::Color::Blue);
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "magenta").getColor() == tgui::Color::Magenta);
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "yellow").getColor() == tgui::Color::Yellow);
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "cyan").getColor() == tgui::Color::Cyan);

        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "red").getColor().isSet());
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "#89ABCD").getColor().isSet());
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "rgb(100, 0, 50)").getColor().isSet());
        REQUIRE(!tgui::Deserializer::deserialize(Type::Color, "").getColor().isSet());
        REQUIRE(!tgui::Deserializer::deserialize(Type::Color, "None").getColor().isSet());

        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Color, "rgb(0,1)"), tgui::Exception);
        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Color, ",,,,"), tgui::Exception);
        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Color, "#Hi"), tgui::Exception);
        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Color, "#12345"), tgui::Exception);
        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Color, "#123456789"), tgui::Exception);
        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Color, "RandomString"), tgui::Exception);
    }

    SECTION("deserialize string")
    {
        REQUIRE(tgui::Deserializer::deserialize(Type::String, "\"\"").getString() == "");
        REQUIRE(tgui::Deserializer::deserialize(Type::String, "\"Just a string.\"").getString() == "Just a string.");
        REQUIRE(tgui::Deserializer::deserialize(Type::String, "\"\\\\\"").getString() == "\\");
        REQUIRE(tgui::Deserializer::deserialize(Type::String, "\"\\n\"").getString() == "\n");
        REQUIRE(tgui::Deserializer::deserialize(Type::String, "\"\\t\"").getString() == "\t");
        REQUIRE(tgui::Deserializer::deserialize(Type::String, "\"\\v\"").getString() == "\v");
        REQUIRE(tgui::Deserializer::deserialize(Type::String, "\"\\\"\"").getString() == "\"");
        REQUIRE(tgui::Deserializer::deserialize(Type::String, "\"a\\t\\\"str\\\"?\\nYES!\"").getString() == "a\t\"str\"?\nYES!");

        REQUIRE(tgui::Deserializer::deserialize(Type::String, "").getString() == "");
        REQUIRE(tgui::Deserializer::deserialize(Type::String, "\\").getString() == "\\");
        REQUIRE(tgui::Deserializer::deserialize(Type::String, "\\n").getString() == "\\n");
        REQUIRE(tgui::Deserializer::deserialize(Type::String, "str").getString() == "str");
        REQUIRE(tgui::Deserializer::deserialize(Type::String, "\"\\m\"").getString() == "m");
        REQUIRE(tgui::Deserializer::deserialize(Type::String, "\"\n\"").getString() == "\n");
    }

    SECTION("deserialize number")
    {
        REQUIRE(tgui::Deserializer::deserialize(Type::Number, "0").getNumber() == 0);
        REQUIRE(tgui::Deserializer::deserialize(Type::Number, "1").getNumber() == 1);
        REQUIRE(tgui::Deserializer::deserialize(Type::Number, "2.25").getNumber() == 2.25f);
    }

    SECTION("deserialize outline")
    {
        REQUIRE(tgui::Deserializer::deserialize(Type::Outline, "1, 2, 3, 4").getOutline() == tgui::Outline(1, 2, 3, 4));
        REQUIRE(tgui::Deserializer::deserialize(Type::Outline, "(5, 6, 7, 8)").getOutline() == tgui::Outline(5, 6, 7, 8));
        REQUIRE(tgui::Deserializer::deserialize(Type::Outline, "9, 10").getOutline() == tgui::Outline(9, 10, 9, 10));
        REQUIRE(tgui::Deserializer::deserialize(Type::Outline, "(11, 12)").getOutline() == tgui::Outline(11, 12, 11, 12));
        REQUIRE(tgui::Deserializer::deserialize(Type::Outline, "13").getOutline() == tgui::Outline(13, 13, 13, 13));
        REQUIRE(tgui::Deserializer::deserialize(Type::Outline, "(14)").getOutline() == tgui::Outline(14, 14, 14, 14));

        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Outline, "(1,2,3)"), tgui::Exception);
        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Outline, ",,,,"), tgui::Exception);
    }

    SECTION("deserialize texture")
    {
        tgui::Texture texture;

        texture = tgui::Deserializer::deserialize(Type::Texture, "resources/image.png").getTexture();
        REQUIRE(texture.getData() != nullptr);
        REQUIRE(texture.getPartRect() == tgui::UIntRect(0, 0, 50, 50));
        REQUIRE(texture.getMiddleRect() == tgui::UIntRect(0, 0, 50, 50));
        REQUIRE(texture.isSmooth());

        texture = tgui::Deserializer::deserialize(Type::Texture, "\"resources/image.png\" NoSmooth").getTexture();
        REQUIRE(texture.getData() != nullptr);
        REQUIRE(texture.getPartRect() == tgui::UIntRect(0, 0, 50, 50));
        REQUIRE(texture.getMiddleRect() == tgui::UIntRect(0, 0, 50, 50));
        REQUIRE(!texture.isSmooth());

        texture = tgui::Deserializer::deserialize(Type::Texture, "\"resources/image.png\" Part(0, 0, 25, 25)").getTexture();
        REQUIRE(texture.getData() != nullptr);
        REQUIRE(texture.getPartRect() == tgui::UIntRect(0, 0, 25, 25));
        REQUIRE(texture.getMiddleRect() == tgui::UIntRect(0, 0, 25, 25));
        REQUIRE(texture.isSmooth());

        texture = tgui::Deserializer::deserialize(Type::Texture, "\"resources/image.png\" Middle(10, 10, 30, 30) NoSmooth").getTexture();
        REQUIRE(texture.getData() != nullptr);
        REQUIRE(texture.getPartRect() == tgui::UIntRect(0, 0, 50, 50));
        REQUIRE(texture.getMiddleRect() == tgui::UIntRect(10, 10, 30, 30));
        REQUIRE(!texture.isSmooth());

        texture = tgui::Deserializer::deserialize(Type::Texture, "\"resources/image.png\" Part(0, 0, 40, 40) Middle(10, 10, 20, 20)").getTexture();
        REQUIRE(texture.getData() != nullptr);
        REQUIRE(texture.getPartRect() == tgui::UIntRect(0, 0, 40, 40));
        REQUIRE(texture.getMiddleRect() == tgui::UIntRect(10, 10, 20, 20));
        REQUIRE(texture.isSmooth());

        texture = tgui::Deserializer::deserialize(Type::Texture, "\"resources/image.png\" Middle(10, 10, 20, 10) Part(20, 10, 40, 30) NoSmooth").getTexture();
        REQUIRE(texture.getData() != nullptr);
        REQUIRE(texture.getPartRect() == tgui::UIntRect(20, 10, 40, 30));
        REQUIRE(texture.getMiddleRect() == tgui::UIntRect(10, 10, 20, 10));
        REQUIRE(!texture.isSmooth());

        texture = tgui::Deserializer::deserialize(Type::Texture, "\"resources/image.png\" Part(0, 0, 40, 40) Middle(5)").getTexture();
        REQUIRE(texture.getMiddleRect() == tgui::UIntRect(5, 5, 30, 30));

        texture = tgui::Deserializer::deserialize(Type::Texture, "\"resources/image.png\" Middle(7, 4)").getTexture();
        REQUIRE(texture.getMiddleRect() == tgui::UIntRect(7, 4, 36, 42));

        texture = tgui::Deserializer::deserialize(Type::Texture, "none").getTexture();
        REQUIRE(texture.getData() == nullptr);

        texture = tgui::Deserializer::deserialize(Type::Texture, "").getTexture();
        REQUIRE(texture.getData() == nullptr);

        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Texture, "\"resources/image.png"), tgui::Exception);
        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Texture, "\"resources/image.png\" (0,0,0,0)"), tgui::Exception);
        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Texture, "\"resources/image.png\" xyz(0,0,0,0)"), tgui::Exception);
        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Texture, "\"resources/image.png\" Part"), tgui::Exception);
        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Texture, "\"resources/image.png\" Part(0,1)"), tgui::Exception);
        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Texture, "\"resources/image.png\" Middle(0,1,2)"), tgui::Exception);
        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Texture, "\"resources/image.png\" Middle(10, 10, 20, 20"), tgui::Exception);

#if TGUI_HAS_BACKEND_SFML
        std::streambuf *oldbuf = sf::err().rdbuf(0); // Prevent SFML from printing a warning
#endif

        REQUIRE_THROWS_AS(tgui::Deserializer::deserialize(Type::Texture, "NonExistentFile"), tgui::Exception);

#if TGUI_HAS_BACKEND_SFML
        sf::err().rdbuf(oldbuf);
#endif
    }

    SECTION("deserialize text style")
    {
        REQUIRE(tgui::Deserializer::deserialize(Type::TextStyle, "Regular").getTextStyle() == tgui::TextStyle::Regular);
        REQUIRE(tgui::Deserializer::deserialize(Type::TextStyle, "Bold | Italic").getTextStyle() == (tgui::TextStyle::Bold | tgui::TextStyle::Italic));

        REQUIRE(tgui::Deserializer::deserialize(Type::TextStyle, "| Underlined").getTextStyle() == tgui::TextStyle::Underlined);
        REQUIRE(tgui::Deserializer::deserialize(Type::TextStyle, "StrikeThrough |").getTextStyle() == tgui::TextStyle::StrikeThrough);

        REQUIRE(tgui::Deserializer::deserialize(Type::TextStyle, "Invalid").getTextStyle() == tgui::TextStyle::Regular);
        REQUIRE(tgui::Deserializer::deserialize(Type::TextStyle, "Bold + Italic").getTextStyle() == tgui::TextStyle::Regular);
    }

    SECTION("deserialize renderer")
    {
        tgui::String data = "{\n"
                           "  Nested = {\n"
                           "    Num = 5;\n"
                           "  };\n"
                           "  SomeColor = Red;\n"
                           "  TextStyleProperty = StrikeThrough;\n"
                           "}";

        std::shared_ptr<tgui::RendererData> rendererData = tgui::Deserializer::deserialize(Type::RendererData, data).getRenderer();
        REQUIRE(rendererData->propertyValuePairs.size() == 3);
        REQUIRE(rendererData->propertyValuePairs["SomeColor"].getString() == "Red");
        REQUIRE(rendererData->propertyValuePairs["TextStyleProperty"].getString() == "StrikeThrough");
        REQUIRE(rendererData->propertyValuePairs["Nested"].getString() == "{\nNum = 5;\n}");
    }

    SECTION("custom deserialize function")
    {
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "rgb(10, 20, 30)").getColor() == tgui::Color(10, 20, 30));
        auto oldFunc = tgui::Deserializer::getFunction(tgui::ObjectConverter::Type::Color);
        
        tgui::Deserializer::setFunction(Type::Color, [](const tgui::String&){ return tgui::ObjectConverter{tgui::Color::Green}; });
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "rgb(10, 20, 30)").getColor() == tgui::Color::Green);
        REQUIRE(tgui::Deserializer::deserialize(Type::Outline, "(50, 60, 70, 80)").getOutline() == tgui::Outline(50, 60, 70, 80));
        
        tgui::Deserializer::setFunction(tgui::ObjectConverter::Type::Color, oldFunc);
        REQUIRE(tgui::Deserializer::deserialize(Type::Color, "rgb(10, 20, 30)").getColor() == tgui::Color(10, 20, 30));
    }
}
