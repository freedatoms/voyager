/*
 * Copyright 2009 Tomáš Frýda <t.fryda@gmail.com>
 * Copyright 2008 Benjamin C. Meyer <ben@meyerhome.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

/****************************************************************************
**
** Copyright (C) 2007-2008 Trolltech ASA. All rights reserved.
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License versions 2.0 or 3.0 as published by the Free Software
** Foundation and appearing in the files LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file.  Alternatively you may (at
** your option) use any later version of the GNU General Public
** License if such license has been publicly approved by Trolltech ASA
** (or its successors, if any) and the KDE Free Qt Foundation. In
** addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.2, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** Please review the following information to ensure GNU General
** Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/. If
** you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech, as the sole
** copyright holder for Qt Designer, grants users of the Qt/Eclipse
** Integration plug-in the right for the Qt/Eclipse Integration to
** link to functionality provided by Qt Designer and its related
** libraries.
**
** This file is provided "AS IS" with NO WARRANTY OF ANY KIND,
** INCLUDING THE WARRANTIES OF DESIGN, MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE. Trolltech reserves all rights not expressly
** granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "toolbarsearch.h"
#include "autosaver.h"
#include "browserapplication.h"
#include <qcompleter.h>
#include <qcoreapplication.h>
#include <qmenu.h>
#include <qsettings.h>
#include <qstringlistmodel.h>
#include <qurl.h>
#include <qmenubar.h>
#include <iostream>
#include <qwebsettings.h>

ToolbarSearch::ToolbarSearch(QWidget *parent)
    : SearchLineEdit(parent)
    , m_autosaver(new AutoSaver(this))
    , m_maxSavedSearches(10)
    , m_SEid(0)
    , m_stringListModel(new QStringListModel(this))
    , m_urlsSE(new QStringListModel(this))
    , m_namesSE(new QStringListModel(this))
{
    load();
    QMenu *m = menu();
    connect(m, SIGNAL(aboutToShow()), this, SLOT(aboutToShowMenu()));
    connect(m, SIGNAL(triggered(QAction*)), this, SLOT(triggeredMenuAction(QAction*)));

    QCompleter *completer = new QCompleter(m_stringListModel, this);
    completer->setCompletionMode(QCompleter::InlineCompletion);
    setCompleter(completer);

    connect(this, SIGNAL(returnPressed()), SLOT(searchNow()));


    if (m_SEid<m_urlsSE->stringList().count())
        setInactiveText(QString(m_urlsSE->stringList().at(m_SEid)).split(QLatin1String("\\")).at(0));


}

ToolbarSearch::~ToolbarSearch()
{
    m_autosaver->saveIfNeccessary();
}

void ToolbarSearch::save()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("toolbarsearch"));
    settings.setValue(QLatin1String("recentSearches"), m_stringListModel->stringList());
    settings.setValue(QLatin1String("maximumSaved"), m_maxSavedSearches);
    settings.setValue(QLatin1String("SEid"), m_SEid);
    settings.setValue(QLatin1String("urlSE"), m_urlsSE->stringList());
    settings.endGroup();
}

void ToolbarSearch::load()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("toolbarsearch"));
    QStringList list = settings.value(QLatin1String("recentSearches")).toStringList();
    m_maxSavedSearches = settings.value(QLatin1String("maximumSaved"), m_maxSavedSearches).toInt();
    m_stringListModel->setStringList(list);
    m_SEid = settings.value(QLatin1String("SEid"),m_SEid).toInt();
    m_urlsSE->setStringList(settings.value(QLatin1String("urlSE")).toStringList());
    settings.endGroup();
}

void ToolbarSearch::searchNow()
{
    QString searchText = text();
    QStringList newList = m_stringListModel->stringList();
    if (newList.contains(searchText))
        newList.removeAt(newList.indexOf(searchText));
    newList.prepend(searchText);
    if (newList.size() >= m_maxSavedSearches)
        newList.removeLast();

    QWebSettings *globalSettings = QWebSettings::globalSettings();
    if (!globalSettings->testAttribute(QWebSettings::PrivateBrowsingEnabled)) {
        m_stringListModel->setStringList(newList);
        m_autosaver->changeOccurred();
    }

    QString url;
    if (m_SEid<m_urlsSE->stringList().count())
        url=QString(m_urlsSE->stringList().at(m_SEid)).split(QLatin1String("\\")).at(1);
    else
        url=QLatin1String("http://www.google.com/search?q=%SEARCHQUERY%&ie=UTF-8&oe=UTF-8&client=%CLIENT%");
    url.replace(QLatin1String("%SEARCHQUERY%"),searchText,Qt::CaseSensitive);
    url.replace(QLatin1String("%CLIENT%"),QCoreApplication::applicationName(),Qt::CaseSensitive);

    emit search(QUrl(url));
}

void ToolbarSearch::aboutToShowMenu()
{
    selectAll();
    QMenu *m = menu();
    m->clear();
    QStringList list = m_stringListModel->stringList();
   QSettings settings;
    settings.beginGroup(QLatin1String("toolbarsearch"));
    m_urlsSE->setStringList(settings.value(QLatin1String("urlSE")).toStringList());
    settings.endGroup();
    QStringList url = m_urlsSE->stringList();

   if (url.isEmpty()){
        url.append(QLatin1String("Cuil\\http://www.cuil.com/search?q=%SEARCHQUERY%&ie=UTF-8&oe=UTF-8&client=%CLIENT%"));
        url.append(QLatin1String("Google\\http://www.google.com/search?q=%SEARCHQUERY%&ie=UTF-8&oe=UTF-8&client=%CLIENT%"));
        m_urlsSE->setStringList(url);
   }

    if (list.isEmpty()) {
        m->addAction(tr("No Recent Searches"));
        m->addSeparator();
    for (int t = 0; t < url.count(); ++t) {
        QAction *item;
        item = new QAction(BrowserApplication::instance()->icon(QUrl(url.at(t).split(QLatin1String("\\")).at(1))),QStringList(url.at(t).split(QLatin1String("\\"))).at(0), this);
        item->setCheckable(true);
        if (t==m_SEid)
            item->setChecked(true);
        else
            item->setChecked(false);
        item->setData(t);
        m->addAction(item);
    }
        return;
    }


    QAction *recent = m->addAction(tr("Recent Searches"));
    recent->setEnabled(false);
    for (int i = 0; i < list.count(); ++i) {
        QString text = list.at(i);
        m->addAction(text)->setData(text);
    }
    m->addSeparator();
    m->addAction(tr("Clear Recent Searches"), this, SLOT(clear()));
    m->addSeparator();
    for (int t = 0; t < url.count(); ++t) {
        QAction *item;
        item = new QAction(BrowserApplication::instance()->icon(QUrl(url.at(t).split(QLatin1String("\\")).at(1))),QStringList(url.at(t).split(QLatin1String("\\"))).at(0), this);
        item->setCheckable(true);
        if (t==m_SEid)
            item->setChecked(true);
        else
            item->setChecked(false);
        item->setData(t);
        m->addAction(item);
    }
}

void ToolbarSearch::triggeredMenuAction(QAction *action)
{
    QVariant v = action->data();
    if (action->isCheckable()){
        m_SEid=v.toInt();
        setInactiveText(QString(m_urlsSE->stringList().at(m_SEid)).split(QLatin1String("\\")).at(0));
        return;
    }
    if (v.canConvert<QString>()) {
        QString text = v.toString();
        setText(text);
        searchNow();
    }
}

void ToolbarSearch::clear()
{
    m_stringListModel->setStringList(QStringList());
    m_autosaver->changeOccurred();
    QLineEdit::clear();
    clearFocus();
}

