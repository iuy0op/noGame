/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "noWorldScene.h"
#include "noMenuScene.h"

USING_NS_CC;

Scene* noWorld::createScene(void){
    auto scene = Scene::createWithPhysics();
    auto layer = noWorld::create();
    scene->addChild(layer);
    return scene;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename){   
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool noWorld::init(void) {
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    drawWorld();

    // Загружаем спрайт-атлас
    auto texture = Director::getInstance()->getTextureCache()->addImage("sprites/test_character.png");
    if (texture == nullptr) {
        problemLoading("'test_character.png'");
    }
    else {
        Rect rect(0, 0, 10, 20); 

        hero->heroSprite = Sprite::createWithTexture(texture, rect);
        if (hero->heroSprite == nullptr)
            problemLoading("'character.png'");
        else {
            std::ifstream file(hero->heroStat);
            if (!file.is_open()) {
                return 1;
            }

            nlohmann::json j;
            file >> j;

            hero->x = j["hero"]["position"]["x"];
            hero->y = j["hero"]["position"]["y"];

            hero->heroSprite->setPosition(Vec2(hero->x, hero->y));
            auto heroPhysicsBody = PhysicsBody::createBox(hero->heroSprite->getContentSize());
            heroPhysicsBody->setDynamic(true);
            heroPhysicsBody->setGravityEnable(true);
            heroPhysicsBody->setRotationEnable(false);
            hero->heroSprite->setPhysicsBody(heroPhysicsBody);

            this->addChild(hero->heroSprite);

            file.close();
        }
    }

    return true;
}

void noWorld::drawWorld(void) {
    auto background = Sprite::create("sprites/background.png");
    background->setPosition(Vec2(1024/4, 576/4 + 100));
    this->addChild(background);

    const int blockSize = 24;
    const int rows = 5 * 2;
    const int cols = 43 * 2;

    for (int row = 1; row < rows; row += 2) {
        for (int col = 1; col < cols; col += 2) {
            cocos2d::Sprite* block;
            if (row == 9) {
                block = Sprite::create("sprites/earth_up.png");
            } else {
                block = Sprite::create("sprites/earth_down.png");
            }

            block->setPosition(Vec2(col * blockSize/4, blockSize + row * blockSize/4));
            auto physicsBody = PhysicsBody::createBox(block->getContentSize());
            physicsBody->setDynamic(false);
            block->setPhysicsBody(physicsBody);

            block->setName("block");
            blocks.push_back(block);

            this->addChild(block);
        }
    }
}

void noWorld::onEnter(void) {
    Scene::onEnter();

    auto listener = EventListenerKeyboard::create();
    listener->onKeyPressed = CC_CALLBACK_2(noWorld::onKeyPressed, this);
    listener->onKeyReleased = CC_CALLBACK_2(noWorld::onKeyReleased, this);

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    schedule(CC_SCHEDULE_SELECTOR(noWorld::update));
}

void noWorld::onKeyPressed(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    keysPressed.insert(keyCode);
}

void noWorld::onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event) {
    keysPressed.erase(keyCode);
}

bool noWorld::saveHeroPosition(void) {
    std::ifstream fileJson(hero->heroStat);
    if (!fileJson.is_open()) {
        return false;
    }

    nlohmann::json j;
    fileJson >> j;
    Vec2 pos = hero->heroSprite->getPosition();

    j["hero"]["position"]["x"] = pos.x;
    j["hero"]["position"]["y"] = pos.y;

    std::ofstream file(hero->heroStat); 
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
    }

    file.close();
    return true;
}

void noWorld::update(float delta) {
    static int countSprite = 1;
    Vec2 position(0, 0);
    if (keysPressed.count(EventKeyboard::KeyCode::KEY_A)) {
        Rect rect(0, countSprite * 32,  10, 20);
        hero->heroSprite->setTextureRect(rect);
        hero->heroSprite->setFlippedX(false);
        if (countSprite == 6)
            countSprite = 0;
        position.x -= 70; 
        countSprite++;
    }
    if (keysPressed.count(EventKeyboard::KeyCode::KEY_D)) {
        Rect rect(0, countSprite * 32, 10, 20);
        hero->heroSprite->setTextureRect(rect);
        hero->heroSprite->setFlippedX(true);
        if (countSprite == 6)
            countSprite = 0;
        position.x += 70;
        countSprite++;
    }
    if (keysPressed.count(EventKeyboard::KeyCode::KEY_SPACE)) {
        position.y += 50;
    }
    if (keysPressed.count(EventKeyboard::KeyCode::KEY_ESCAPE)) {
        if (saveHeroPosition() != true) {
            // Эта плоха
        }
        auto scene = noMenu::createScene();
        Director::getInstance()->replaceScene(scene);
    }

    if (position.x != 0 || position.y != 0) {
        hero->heroSprite->setPosition(hero->heroSprite->getPosition() + position * delta);
    }
}
