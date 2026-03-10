#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

class $modify(SliderMuteLayer, PauseLayer) {
public:
	struct Fields {
		CCMenuItemToggler* m_musicToggle = nullptr;
		CCMenuItemToggler* m_sfxToggler = nullptr;

		CCSprite* m_musicMuteOff = nullptr;
		CCSprite* m_musicMuteOn = nullptr;

		CCSprite* m_sfxMuteOff = nullptr;
		CCSprite* m_sfxMuteOn = nullptr;
	};

	void customSetup() {
		PauseLayer::customSetup();
		if (!Mod::get()->getSettingValue<bool>("enabled")) return; //if the setting "enabled" is not on then it will not run code

		auto gm = GameManager::sharedState();
		
		// sprites
		bool useDiff = Mod::get()->getSettingValue<bool>("use-diff-textures");
		const char* offSpriteName = useDiff ? "GJ_fxOffBtn_001.png" : "GJ_musicOffBtn_001.png";
		const char* onSpriteName = useDiff ? "GJ_fxOnBtn_001.png" : "GJ_musicOnBtn_001.png";

		auto musicOffSprite = CCSprite::createWithSpriteFrameName(offSpriteName);
		auto musicOnSprite = CCSprite::createWithSpriteFrameName(onSpriteName);
		musicOffSprite->setScale(0.75f);
		musicOnSprite->setScale(0.75f);

		auto sfxOffSprite = CCSprite::createWithSpriteFrameName(offSpriteName);
		auto sfxOnSprite = CCSprite::createWithSpriteFrameName(onSpriteName);
		sfxOffSprite->setScale(0.75f);
		sfxOnSprite->setScale(0.75f);

		auto musicSlider = this->getChildByID("music-slider");
		auto sfxSlider = this->getChildByID("sfx-slider");

		auto muteTogglesMenu = CCMenu::create();
		muteTogglesMenu->setID("mute-toggle-menu"_spr);

		// toggles
		if (musicSlider) {
			auto musicMuteToggle = CCMenuItemToggler::create(
				musicOnSprite,
				musicOffSprite,
				this,
				menu_selector(SliderMuteLayer::onMusicToggle)
			);
			// position them relitive to the slider
			musicMuteToggle->setPosition({ musicSlider->getPositionX() - 110.f, musicSlider->getPositionY() });
			musicMuteToggle->setID("mute-music-toggle"_spr);
			musicMuteToggle->toggle(gm->m_bgVolume == 0.0f);


			m_fields->m_musicToggle = musicMuteToggle;
			muteTogglesMenu->addChild(musicMuteToggle);
		}

		if (sfxSlider) {
			auto sfxMuteToggle = CCMenuItemToggler::create(
				sfxOnSprite,
				sfxOffSprite,
				this,
				menu_selector(SliderMuteLayer::onSfxToggle)
			);
			// position them relitive to the slider
			sfxMuteToggle->setPosition({ sfxSlider->getPositionX() + 110.f, sfxSlider->getPositionY() });
			sfxMuteToggle->setID("mute-sfx-toggle"_spr);
			sfxMuteToggle->toggle(gm->m_sfxVolume == 0.0f);

			m_fields->m_sfxToggler = sfxMuteToggle;
			muteTogglesMenu->addChild(sfxMuteToggle);
		}

		// add and position the menu
		muteTogglesMenu->setPosition({0, 0});
		this->addChild(muteTogglesMenu);
	}

	void musicSliderChanged(CCObject* sender) {
		PauseLayer::musicSliderChanged(sender);
		if (!Mod::get()->getSettingValue<bool>("enabled")) return;

		auto fmod = FMODAudioEngine::sharedEngine();
		bool isMuted = (fmod->getBackgroundMusicVolume() <= 0.01f);

		// this is for updating the sprites based on what the value of the slider is
		if (m_fields->m_musicToggle) {
			if (m_fields->m_musicToggle->isToggled() != isMuted) {
				m_fields->m_musicToggle->toggle(isMuted);
			}
		}
	}
	
	void sfxSliderChanged(CCObject* sender) {
		PauseLayer::sfxSliderChanged(sender);
		if (!Mod::get()->getSettingValue<bool>("enabled")) return;

		auto fmod = FMODAudioEngine::sharedEngine();
		bool isMuted = (fmod->getEffectsVolume() <= 0.01f);

		// this is for upading the sprites based on what the value of the slider is
		if (m_fields->m_sfxToggler) {
			if (m_fields->m_sfxToggler->isToggled() != isMuted) {
				m_fields->m_sfxToggler->toggle(isMuted);
			}
		}
	}

	void onMusicToggle(CCObject* sender) {
		if (!Mod::get()->getSettingValue<bool>("enabled")) return;

		auto fmod = FMODAudioEngine::sharedEngine();
		auto gm = GameManager::sharedState();

		float currentVol = fmod->getBackgroundMusicVolume();
		bool isMuted = (currentVol <= 0.01f);
		float targetVolume = 0.0f;

		if (!isMuted) {
			Mod::get()->setSavedValue("saved-music-volume", currentVol);
			targetVolume = 0.0f;
		}
		else {
			targetVolume = Mod::get()->getSavedValue<float>("saved-music-volume", 1.0f);
			if (targetVolume == 0.0f) targetVolume = 0.5f; // prevents a bug
		}

		gm->m_bgVolume = targetVolume;
		fmod->setBackgroundMusicVolume(targetVolume);

		if (auto musicSliderNode = this->getChildByID("music-slider")) {
			if (auto slider = typeinfo_cast<Slider*>(musicSliderNode)) {
				slider->setValue(targetVolume);
				slider->updateBar();
			}
		}
	}

	void onSfxToggle(CCObject* sender) {
		if (!Mod::get()->getSettingValue<bool>("enabled")) return;

		auto fmod = FMODAudioEngine::sharedEngine();
		auto gm = GameManager::sharedState();

		float currentVol = fmod->getEffectsVolume();
		bool isMuted = (currentVol <= 0.01f);
		auto targetVolume = 0.0f;

		if (!isMuted) {
			Mod::get()->setSavedValue("saved-sfx-volume", currentVol);
			targetVolume = 0.0f;
		}
		else {
			targetVolume = Mod::get()->getSavedValue<float>("saved-sfx-volume");
			if (targetVolume == 0.0f) targetVolume = 0.5f; // prevents a bug
		}

		gm->m_sfxVolume = targetVolume;
		fmod->setEffectsVolume(targetVolume);

		if (auto sfxSliderNode = this->getChildByID("sfx-slider")) {
			if (auto slider = typeinfo_cast<Slider*>(sfxSliderNode)) {
				slider->setValue(targetVolume);
				slider->updateBar();
			}
		}
	}
};