/*
  StartSceneCtrl.cpp
  Author : Cyrielle
  Contributor : BAKFR
  File under GNU GPL v3.0 license
*/
#include "StartSceneCtrl.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/System/String.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <memory>
#include <string>

#include "src/opmon/screens/overworld/OverworldCtrl.hpp"
#include "src/opmon/screens/animation/AnimationCtrl.hpp"
#include "src/opmon/core/Player.hpp"
#include "src/opmon/core/UiData.hpp"
#include "src/opmon/screens/animation/Animations.hpp"
#include "StartScene.hpp"
#include "StartSceneData.hpp"
#include "src/opmon/screens/base/AGameScreen.hpp"
#include "src/opmon/view/ui/Dialog.hpp"

//Defines created to make the code easier to understand.
#define LOAD_OVERWORLD 1
#define LOAD_ANIMATION_OPEN 2
#define LOAD_ANIMATION_CLOSE 3

namespace OpMon {

    StartSceneCtrl::StartSceneCtrl(UiData *data)
        : data(data)
        , view(this->data) {}

    GameStatus StartSceneCtrl::checkEvent(sf::Event const &event) {
        auto &startscene = view;

        switch(event.type) {
        case sf::Event::KeyPressed:
            if(event.key.code == data.getUiDataPtr()->getKeyTalk() && startscene.getDialog() != nullptr) {
                startscene.getDialog()->pass();
            }
            //Part 1 is the part where the player enters his/her name
            if(event.key.code == sf::Keyboard::Return && startscene.getPart() == 1) {
                sf::String &pName = startscene.getpName();
                if(pName.isEmpty()) {
                    pName = "Player"; //We will find a default name some day
                }
                data.getPlayer().setName(pName);
                startscene.delLoop1();
                animNext = true;
                return GameStatus::CONTINUE;
            }
            //P is used to skip the introduction, but it must be disabled when entering the name
            if(event.key.code == sf::Keyboard::P && startscene.getPart() != 1) {
                loadNext = LOAD_OVERWORLD;
                return GameStatus::NEXT;
            }
            break;
        case sf::Event::TextEntered:
            if(startscene.getPart() == 1) {
                sf::String &pName = startscene.getpName();
                if(event.text.unicode == 8) { //Backspace
                    if(!pName.isEmpty()) {
                        pName = sf::String::fromUtf32(pName.begin(), pName.end() - 1);
                    }
                } else if(event.text.unicode > 32) {
                    if(pName.toUtf32().size() < 14) { //14 = Max name length
                        pName += event.text.unicode;
                    }
                }
            }
        default:
            break;
        }

        //If it's the end of the introduction, go to the overworld
        if(view.getPart() > 2) {
            loadNext = LOAD_OVERWORLD;
            return GameStatus::NEXT;
        }
        return GameStatus::CONTINUE;
    }

    void StartSceneCtrl::loadNextScreen() {
        switch(loadNext) {
        case LOAD_OVERWORLD:
            _next_gs = std::make_unique<OverworldCtrl>(data.getPlayer(), data.getUiDataPtr());
            break;
        case LOAD_ANIMATION_OPEN:
            _next_gs = std::make_unique<AnimationCtrl>(std::make_unique<Animations::WinAnim>(screenTexture, true));
            break;
        case LOAD_ANIMATION_CLOSE:
            _next_gs = std::make_unique<AnimationCtrl>(std::make_unique<Animations::WinAnim>(screenTexture, false));
            break;
        default:
            throw Utils::UnexpectedValueException(std::to_string(loadNext), "a view to load in StartSceneCtrl::loadNextScreen()");
        }
    }

    GameStatus StartSceneCtrl::update(sf::RenderTexture &frame) {
        //If the player have finished to enter his/her name, animNext must have been set to true. This part will launch the animation.
        if(animNext) {
            animNext = false;
            frame.draw(view);
            loadNext = LOAD_ANIMATION_CLOSE;
            screenTexture = frame.getTexture();
            return GameStatus::NEXT_NLS;
        }
        GameStatus toReturn = view.update();
        if(toReturn == GameStatus::CONTINUE) {
            frame.draw(view);
        }

        if(toReturn == GameStatus::NEXT_NLS || toReturn == GameStatus::NEXT) {
            switch(view.getPart()) {
            case 1:
                loadNext = LOAD_ANIMATION_OPEN;
                screenTexture = frame.getTexture();
                toReturn = GameStatus::NEXT_NLS;
                break;
            case 3:
                loadNext = LOAD_OVERWORLD;
                break;
            default:
                throw Utils::UnexpectedValueException(std::to_string(view.getPart()), "1 or 3 in StartScene::draw()");
            }
        }

        return toReturn;
    }

    void StartSceneCtrl::suspend() {
        view.pause();
    }

    void StartSceneCtrl::resume() {
        view.play();
    }

} // namespace OpMon
