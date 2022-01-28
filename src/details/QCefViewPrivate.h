﻿#pragma once
#pragma region qt_headers
#include <QImage>
#include <QString>
#include <QWindow>
#pragma endregion qt_headers

#include <CefViewBrowserClient.h>

#include "QCefContextPrivate.h"
#include "QCefSettingPrivate.h"

#include <QCefView.h>

class QCefViewPrivate : public QObject
{
  Q_OBJECT
  Q_DECLARE_PUBLIC(QCefView)
  QCefView* q_ptr;

  friend class CCefClientDelegate;

private:
  /// <summary>
  ///
  /// </summary>
  QCefContextPrivate* pContext_;

  /// <summary>
  ///
  /// </summary>
  CefRefPtr<CefBrowser> pCefBrowser_;

  /// <summary>
  ///
  /// </summary>
  QImage qCefFrameBuffer_;

protected:
  void createBrowser(QCefView* view, const QString url, const QCefSettingPrivate* setting);

  void closeBrowser();

  void destroyBrowser();

public:
  explicit QCefViewPrivate(QCefView* view, const QString& url, const QCefSettingPrivate* setting = nullptr);

  ~QCefViewPrivate();

  int browserId();

  void navigateToString(const QString& content);

  void navigateToUrl(const QString& url);

  bool browserCanGoBack();

  bool browserCanGoForward();

  void browserGoBack();

  void browserGoForward();

  bool browserIsLoading();

  void browserReload();

  void browserStopLoad();

  bool triggerEvent(const QString& name, const QVariantList& args, int frameId = CefViewBrowserClient::MAIN_FRAME);

  bool responseQCefQuery(const QCefQuery& query);

  void notifyMoveOrResizeStarted();

  bool sendEventNotifyMessage(int frameId, const QString& name, const QVariantList& args);

  void setFocus(bool focus);

  void paintCefFrameBuffer(const uchar* buf, int width, int height);

protected:
  /// <summary>
  ///
  /// </summary>
  /// <param name="watched"></param>
  /// <param name="event"></param>
  /// <returns></returns>
  virtual bool eventFilter(QObject* watched, QEvent* event) override;
};
