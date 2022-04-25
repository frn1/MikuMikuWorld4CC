#include "ApplicationConfiguration.h"
#include "StringOperations.h"
#include <filesystem>
#include <fstream>

using namespace nlohmann;

namespace MikuMikuWorld
{
	ApplicationConfiguration::ApplicationConfiguration() :
		version{ "1.0.0" }, windowPos{ 150, 100 }, windowSize{ 1280, 720 }, maximized{ false }, vsync{ true },
		accentColor{ 1 }, userColor{ 0.2f, 0.2f, 0.2f, 1.0f }
	{

	}

	bool ApplicationConfiguration::keyExists(const json& js, const char* key)
	{
		return (js.find(key) != js.end());
	}

	int ApplicationConfiguration::tryGetInt(const json& js, const char* key, int def)
	{
		return keyExists(js, key) ? (int)js[key] : def;
	}

	float ApplicationConfiguration::tryGetFloat(const json& js, const char* key, float def)
	{
		return keyExists(js, key) ? (float)js[key] : def;
	}

	bool ApplicationConfiguration::tryGetBool(const json& js, const char* key, bool def)
	{
		return keyExists(js, key) ? (bool)js[key] : def;
	}

	Vector2 ApplicationConfiguration::tryGetVector2(const json& js, const char* key)
	{
		Vector2 v;
		if (keyExists(js, key))
		{
			v.x = tryGetFloat(js[key], "x", 0.0f);
			v.y = tryGetFloat(js[key], "y", 0.0f);
		}

		return v;
	}

	Color ApplicationConfiguration::tryGetColor(const json& js, const char* key)
	{
		Color c;
		if (keyExists(js, key))
		{
			c.r = tryGetFloat(js[key], "r", 0.2f);
			c.g = tryGetFloat(js[key], "g", 0.2f);
			c.b = tryGetFloat(js[key], "b", 0.2f);
			c.a = tryGetFloat(js[key], "a", 1.0f);
		}

		return c;
	}

	std::string ApplicationConfiguration::tryGetString(const json& js, const char* key)
	{
		std::string s;
		if (keyExists(js, key))
			s = (std::string)js[key];

		return s;
	}

	void ApplicationConfiguration::read(const std::string& filename)
	{
		std::wstring wFilename = mbToWideStr(filename);
		if (!std::filesystem::exists(wFilename))
			return;

		std::ifstream configFile(wFilename);
		json config;
		configFile >> config;
		configFile.close();

		version = tryGetString(config["version"], "1.0.0");
		if (keyExists(config, "window"))
		{
			const json& window = config["window"];
			maximized = tryGetBool(window, "maximized", false);
			vsync = tryGetBool(window, "vsync", true);

			windowPos = tryGetVector2(window, "position");
			if (windowPos.x <= 0) windowPos.x = 50;
			if (windowPos.y <= 0) windowPos.y = 80;

			windowSize = tryGetVector2(window, "size");
			if (windowSize.x <= 0 || windowSize.y <= 0)
			{
				windowSize.x = 1280;
				windowSize.y = 720;
			}
		}

		if (keyExists(config, "timeline"))
		{
			timelineWidth = tryGetInt(config["timeline"], "lane_width", 35);
			notesHeight = tryGetInt(config["timeline"], "notes_height", 50);
		}

		if (keyExists(config, "theme"))
		{
			accentColor = tryGetInt(config["theme"], "accent_color", 1);
			userColor = tryGetColor(config["theme"], "user_color");
		}
	}

	void ApplicationConfiguration::write(const std::string& filename)
	{
		json config;
		config["version"] = version;
		config["window"]["position"] = { {"x", windowPos.x}, {"y", windowPos.y} };
		config["window"]["size"] = { {"x", windowSize.x}, {"y", windowSize.y} };
		config["window"]["maximized"] = maximized;
		config["window"]["vsync"] = vsync;

		config["timeline"] = {
			{"lane_width", timelineWidth},
			{"notes_height", notesHeight}
		};

		config["theme"] = {
			{"accent_color", accentColor},
			{"user_color",
				{ 
					{"r", userColor.r},
					{"g", userColor.g},
					{"b", userColor.b},
					{"a", userColor.a}
				}
			}
		};

		std::wstring wFilename = mbToWideStr(filename);
		std::ofstream configFile(wFilename);
		configFile << std::setw(4) << config;
		configFile.close();
	}
}