﻿#include <QApplication>

#include <QCefContext.h>

#include "MainWindow.h"

int
main(int argc, char* argv[])
{
  QApplication a(argc, argv);

  // build QCefConfig
  QCefConfig config(argc, argv);
  config.setUserAgent("QCefViewTest");
  config.setLogLevel(QCefConfig::LOGSEVERITY_DEFAULT);
  config.setBridgeObjectName("CallBridge");
  config.setRemoteDebuggingPort(9000);

  // initialize QCefContext instance with config
  QCefContext cefContext(&a, &config);

  MainWindow w;
  w.show();

  return a.exec();
}
