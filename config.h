#ifndef CONFIG_H
#define CONFIG_H

// #include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QString>
#include <functional>

struct Config {
  static std::function<const QSettings *()> get;
};
#endif // CONFIG_H
