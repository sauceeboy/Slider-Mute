#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// hooking into PlayLayer so auto mute works when you enter the level and not when you pause

class $modify(SliderMutePlayLayer, PlayLayer) {
public:
    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
        

        if (Mod::get()->getSettingValue<bool>("auto-mute-music")) {
            this->turnOffSound( true, false );
        }
        if (Mod::get()->getSettingValue<bool>("auto-mute-sfx")) {
            this->turnOffSound( false, true );
        }

        return true;
    }

    void turnOffSound(bool music, bool sfx) {
        auto fmod = FMODAudioEngine::sharedEngine();
        auto gm = GameManager::sharedState();

        if (music) {
            fmod->setBackgroundMusicVolume(0.0f);
            gm->m_bgVolume = 0.0f;
        }
        if (sfx) {
            fmod->setEffectsVolume(0.0f);
            gm->m_sfxVolume = 0.0f;
        }
    }
};