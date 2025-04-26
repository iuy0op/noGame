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

#include "noMenuScene.h"
#include "noWorldScene.h"

USING_NS_CC;
 
Scene* noMenu::createScene(){
    return noMenu::create();
}
 
// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename){   
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}
 
// on "init" you need to initialize your instance
bool noMenu::init(){
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }
 
    drawMenu();

    return true;
}
 
void noMenu::drawMenu() {
    auto background = Sprite::create("Sprites/background_menu.png");
    background->setPosition(Vec2(1024/4, 576/4));
    this->addChild(background);

    start = Sprite::create("Sprites/start_button.png");
    start->setPosition(Vec2(1024/4 - 20, 576/4 + 10));
    this->addChild(start);
}
 
void noMenu::onEnter() {
    Scene::onEnter();
 
    auto mouseListener = EventListenerMouse::create();
    mouseListener->onMouseMove = CC_CALLBACK_1(noMenu::onMouseMove, this);
    mouseListener->onMouseDown = CC_CALLBACK_1(noMenu::onMouseDown, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);
 
    schedule(CC_SCHEDULE_SELECTOR(noMenu::update));
}

void noMenu::onMouseMove(cocos2d::Event* event) {
    EventMouse* mouseEvent = dynamic_cast<EventMouse*>(event);
    if (mouseEvent) {
        Vec2 mousePos = mouseEvent->getLocation();

        if (start->getBoundingBox().containsPoint(mousePos)) {
            CCLOG("Навелся на кнопку Start");
            start->setColor(Color3B::YELLOW);
        }
        else {
            start->setColor(Color3B::WHITE);
        }
    }
}

void noMenu::onMouseDown(cocos2d::Event* event) {
    EventMouse* mouseEvent = dynamic_cast<EventMouse*>(event);
    if (mouseEvent && mouseEvent->getMouseButton() == EventMouse::MouseButton::BUTTON_LEFT) {
        Vec2 mousePos = mouseEvent->getLocation();

        if (start->getBoundingBox().containsPoint(mousePos)) {
            auto scene = noWorld::createScene();
            Director::getInstance()->replaceScene(scene);
        }
    }
}
 