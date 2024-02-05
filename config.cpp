#include "config.h"

std::function<const QSettings *()> Config::get = []() {
  const QString __DIR__ = QFileInfo(__FILE__).absolutePath();

  const QString config_path = __DIR__ + "/config.ini";

  if (!QFile(config_path).exists()) {
    // throw exception
  }

  const QSettings *parsed_config =
      new QSettings(config_path, QSettings::IniFormat);

  // in case of empty or not loaded config

  if (parsed_config->allKeys().size() == 0) {
    // throw exception
  }

  // redefine once it is parsed
  get = [parsed_config]() { return parsed_config; };

  return parsed_config;
};
