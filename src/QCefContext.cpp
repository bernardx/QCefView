﻿#include <QCefContext.h>

#pragma region qt_headers
#include <QDebug>
#pragma endregion qt_headers

#include "details/QCefContextPrivate.h"

QCefContext* QCefContext::s_self;

QCefContext::QCefContext(QCoreApplication* app, const QCefConfig* config)
  : QObject(app)
  , d_ptr(new QCefContextPrivate(app))
{
  init(config);
}

QCefContext*
QCefContext::instance()
{
  return s_self;
}

QCefContext::~QCefContext()
{
  uninit();
}

void
QCefContext::addLocalFolderResource(const QString& path, const QString& url, int priority /*= 0*/)
{
  Q_D(QCefContext);

  d->pClient_->AddLocalDirectoryResourceProvider(path.toStdString(), url.toStdString());
}

void
QCefContext::addArchiveResource(const QString& path,
                                const QString& url,
                                const QString& password /*= ""*/,
                                int priority /*= 0*/)
{
  Q_D(QCefContext);

  d->pClient_->AddArchiveResourceProvider(path.toStdString(), url.toStdString(), password.toStdString(), priority);
}

bool
QCefContext::addCookie(const QString& name, const QString& value, const QString& domain, const QString& url)
{
  Q_D(QCefContext);

  return d->addGlobalCookie(name.toStdString(), value.toStdString(), domain.toStdString(), url.toStdString());
}

bool
QCefContext::init(const QCefConfig* config)
{
  Q_ASSERT_X(!s_self, "QCefContext::init()", "There can be only one QCefContext instance");
  s_self = this;

  Q_D(QCefContext);
  d->initialize(config ? config->d_func() : nullptr);

  return true;
}

void
QCefContext::uninit()
{
  Q_D(QCefContext);
  d->uninitialize();

  s_self = nullptr;
}
