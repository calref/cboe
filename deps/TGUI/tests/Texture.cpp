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
#include <TGUI/Exception.hpp>
#include <TGUI/Texture.hpp>
#include <TGUI/TextureManager.hpp>

#if TGUI_HAS_BACKEND_SFML
    #include <SFML/System/Err.hpp>
#endif

TEST_CASE("[Texture]")
{
    SECTION("Loading")
    {
        SECTION("Failure")
        {
            tgui::Texture texture;

            SECTION("No image")
            {
                REQUIRE_NOTHROW(tgui::Texture());
            }
            SECTION("Image not found")
            {
#if TGUI_HAS_BACKEND_SFML
                std::streambuf *oldbuf = sf::err().rdbuf(0); // Prevent SFML from printing a warning
#endif
                REQUIRE_THROWS_AS(tgui::Texture("NonExistent.png"), tgui::Exception);
                REQUIRE_THROWS_AS(texture.load("NonExistent.png"), tgui::Exception);

#if TGUI_HAS_BACKEND_SFML
                sf::err().rdbuf(oldbuf);
#endif
            }

            REQUIRE(texture.getId() == "");
            REQUIRE(texture.getData() == nullptr);
            REQUIRE(texture.getImageSize() == tgui::Vector2u(0, 0));
            REQUIRE(texture.getMiddleRect() == tgui::UIntRect());
        }

        SECTION("Success")
        {
            SECTION("constructor")
            {
                REQUIRE_NOTHROW(tgui::Texture("resources/image.png"));

                tgui::Texture texture{"resources/image.png"};
                REQUIRE(texture.getId() == "resources/image.png");
                REQUIRE(texture.getData() != nullptr);
                REQUIRE(texture.getData()->backendTexture != nullptr);
                REQUIRE(texture.getPartRect() == tgui::UIntRect(0, 0, 50, 50));
                REQUIRE(texture.getImageSize() == tgui::Vector2u(50, 50));
                REQUIRE(texture.getMiddleRect() == tgui::UIntRect(0, 0, 50, 50));
                REQUIRE(texture.isSmooth() == true);
            }

            SECTION("load")
            {
                tgui::Texture texture;
                texture.load("resources/image.png", {10, 5, 40, 30}, {6, 5, 28, 20});
                REQUIRE(texture.getId() == "resources/image.png");
                REQUIRE(texture.getData() != nullptr);
                REQUIRE(texture.getData()->backendTexture != nullptr);
                REQUIRE(texture.getPartRect() == tgui::UIntRect(10, 5, 40, 30));
                REQUIRE(texture.getImageSize() == tgui::Vector2u(40, 30));
                REQUIRE(texture.getMiddleRect() == tgui::UIntRect(6, 5, 28, 20));
                REQUIRE(texture.isSmooth() == true);
            }
        }
    }

    SECTION("Copy and destruct")
    {
        unsigned int copyCount = 0;
        unsigned int destructCount = 0;
        {
            tgui::Texture texture{"resources/image.png", {}, {10, 0, 30, 50}, false};
            texture.setCopyCallback([&](std::shared_ptr<tgui::TextureData> data)
                {
                    copyCount++;
                    tgui::TextureManager::copyTexture(data);
                }
            );
            texture.setDestructCallback([&](std::shared_ptr<tgui::TextureData> data)
                {
                    destructCount++;
                    tgui::TextureManager::removeTexture(data);
                }
            );

            REQUIRE(texture.getId() == "resources/image.png");
            REQUIRE(texture.getData() != nullptr);
            REQUIRE(texture.getData()->backendTexture != nullptr);
            REQUIRE(texture.getPartRect() == tgui::UIntRect(0, 0, 50, 50));
            REQUIRE(texture.getImageSize() == tgui::Vector2u(50, 50));
            REQUIRE(texture.getMiddleRect() == tgui::UIntRect(10, 0, 30, 50));
            REQUIRE(texture.isSmooth() == false);

            REQUIRE(copyCount == 0);
            REQUIRE(destructCount == 0);
            {
                tgui::Texture textureCopy{texture};
                REQUIRE(textureCopy.getId() == "resources/image.png");
                REQUIRE(textureCopy.getData() != nullptr);
                REQUIRE(textureCopy.getData()->backendTexture != nullptr);
                REQUIRE(textureCopy.getPartRect() == tgui::UIntRect(0, 0, 50, 50));
                REQUIRE(textureCopy.getImageSize() == tgui::Vector2u(50, 50));
                REQUIRE(textureCopy.getMiddleRect() == tgui::UIntRect(10, 0, 30, 50));
                REQUIRE(textureCopy.isSmooth() == false);

                REQUIRE(copyCount == 1);
                REQUIRE(destructCount == 0);
            }
            REQUIRE(copyCount == 1);
            REQUIRE(destructCount == 1);
            {
                tgui::Texture textureCopy;
                REQUIRE(textureCopy.getData() == nullptr);

                textureCopy = texture;
                REQUIRE(textureCopy.getId() == "resources/image.png");
                REQUIRE(textureCopy.getData() != nullptr);
                REQUIRE(textureCopy.getData()->backendTexture != nullptr);
                REQUIRE(textureCopy.getPartRect() == tgui::UIntRect(0, 0, 50, 50));
                REQUIRE(textureCopy.getImageSize() == tgui::Vector2u(50, 50));
                REQUIRE(textureCopy.getMiddleRect() == tgui::UIntRect(10, 0, 30, 50));
                REQUIRE(textureCopy.isSmooth() == false);

                REQUIRE(copyCount == 2);
                REQUIRE(destructCount == 1);
            }
            REQUIRE(copyCount == 2);
            REQUIRE(destructCount == 2);
        }
        REQUIRE(copyCount == 2);
        REQUIRE(destructCount == 3);
    }

    SECTION("MiddleRect")
    {
        tgui::Texture texture;
        REQUIRE(texture.getMiddleRect() == tgui::UIntRect());

        texture.load("resources/image.png");
        REQUIRE(texture.getMiddleRect() == tgui::UIntRect(0, 0, 50, 50));
    }

    SECTION("Smooth")
    {
        // Textures are smoothed by default
        REQUIRE(tgui::Texture::getDefaultSmooth());
        REQUIRE(tgui::Texture("resources/image.png").isSmooth());

        tgui::Texture::setDefaultSmooth(false);
        REQUIRE(!tgui::Texture::getDefaultSmooth());
        REQUIRE(!tgui::Texture("resources/image.png").isSmooth());

        tgui::Texture::setDefaultSmooth(true);
        REQUIRE(tgui::Texture::getDefaultSmooth());
        REQUIRE(tgui::Texture("resources/image.png").isSmooth());
    }

    SECTION("Color")
    {
        tgui::Texture texture{"resources/image.png"};
        REQUIRE(texture.getColor() == tgui::Color::White);

        texture.setColor("red");
        REQUIRE(texture.getColor() == tgui::Color::Red);
    }

#if TGUI_HAS_BACKEND_SFML
    SECTION("Shader")
    {
        tgui::Texture texture{"resources/image.png"};
        REQUIRE(!texture.getShader());

        sf::Shader shader;
        texture.setShader(&shader);
        REQUIRE(texture.getShader() == &shader);

        texture.setShader(nullptr);
        REQUIRE(!texture.getShader());
    }
#endif

    SECTION("BackendTextureLoader")
    {
        unsigned int count = 0;
        auto oldBackendTextureLoader = tgui::Texture::getBackendTextureLoader();

        auto func = [&](tgui::BackendTextureBase&, const tgui::String& filename) {
            REQUIRE(filename == "resources/image.png");
            count++;
            return true;
        };

        tgui::Texture::setBackendTextureLoader(func);
        REQUIRE_NOTHROW(tgui::Texture{"resources/image.png"});
        REQUIRE(count == 1);

        tgui::Texture::setBackendTextureLoader(oldBackendTextureLoader);
    }

    SECTION("TextureLoader")
    {
        unsigned int count = 0;
        auto oldTextureLoader = tgui::Texture::getTextureLoader();

        auto func = [&](tgui::Texture& texture, const tgui::String& filename, bool) {
            REQUIRE(filename == "resources/image.png");
            auto data = std::make_shared<tgui::TextureData>();
            data->backendTexture = std::make_shared<tgui::BackendTextureBase>();
            texture.getBackendTextureLoader()(*data->backendTexture, filename);
            count++;
            return data;
        };

        tgui::Texture::setTextureLoader(func);
        REQUIRE_NOTHROW(tgui::Texture{"resources/image.png"});
        REQUIRE(count == 1);

        tgui::Texture::setTextureLoader(oldTextureLoader);
    }
}
