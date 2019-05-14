/**
 * Copyright 2009 Tomáš Frýda <t.fryda@gmail.com>
 */
#include <QtGui>
#include <QtNetwork>
#include "browserapplication.h"
#include "ftpwindow.h"
#include "downloadmanager.h"

FtpWindow::FtpWindow(QWidget *parent)
    : QWidget(parent), ftp(0)
{

    ftpServerLabel = new QLabel(tr("Ftp server:"));
    ftpServerLineEdit = new QLineEdit();

    fileList = new QTreeWidget;
    fileList->setEnabled(false);
    fileList->setRootIsDecorated(false);
    fileList->setHeaderLabels(QStringList() << tr("Name") << tr("Size") << tr("Owner") << tr("Group") << tr("Time"));

    cdToParentButton = new QPushButton;
    cdToParentButton->setIcon(QPixmap(QLatin1String(":/cdtoparent.png")));
    cdToParentButton->setEnabled(false);

    buttonBox = new QDialogButtonBox;

    connect(fileList, SIGNAL(itemActivated(QTreeWidgetItem *, int)),
            this, SLOT(processItem(QTreeWidgetItem *, int)));
    connect(cdToParentButton, SIGNAL(clicked()), this, SLOT(cdToParent()));

    QHBoxLayout *topLayout = new QHBoxLayout;
    topLayout->addWidget(ftpServerLabel);
    topLayout->addWidget(ftpServerLineEdit);
    topLayout->addWidget(cdToParentButton);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(fileList);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

}
FtpWindow::FtpWindow(const FtpWindow &):
   QWidget(0)
{
}

void FtpWindow::setServer(QString str){
    ftpServerLineEdit->setText(str);
}
void FtpWindow::Connect(){
    connectOrDisconnect();
}
void FtpWindow::connectOrDisconnect()
{
     if (ftp) {
        ftp->abort();
        ftp->deleteLater();
        ftp = 0;
        fileList->setEnabled(false);
        cdToParentButton->setEnabled(false);
#ifndef QT_NO_CURSOR
        setCursor(Qt::ArrowCursor);
#endif
        return;
    }

#ifndef QT_NO_CURSOR
    setCursor(Qt::WaitCursor);
#endif

    ftp = new QFtp(this);
    connect(ftp, SIGNAL(commandFinished(int, bool)),
            this, SLOT(ftpCommandFinished(int, bool)));
    connect(ftp, SIGNAL(listInfo(const QUrlInfo &)),
            this, SLOT(addToList(const QUrlInfo &)));


    fileList->clear();
    currentPath.clear();
    isDirectory.clear();
    currentPath = ftpServerLineEdit->text();
    if (currentPath.endsWith(QLatin1Char('/')))
        currentPath.chop(1);
    QUrl url(currentPath);
    if (!url.isValid() || url.scheme().toLower() != QLatin1String("ftp")) {
        ftp->connectToHost(currentPath+QLatin1Char('/'), 21);
        ftp->login();
    } else {
        ftp->connectToHost(url.host(), url.port(21));

        if (!url.userName().isEmpty())
            ftp->login(QUrl::fromPercentEncoding(url.userName().toLatin1()), url.password());
        else
            ftp->login();
        if (!url.path().isEmpty())
            ftp->cd(url.path());
    }
    fileList->setEnabled(true);
     QString temp;
    temp = currentPath.left(currentPath.indexOf(QLatin1Char('/'),6));
    if (temp.count()<currentPath.count())
        cdToParentButton->setEnabled(true);
}

void FtpWindow::downloadFile()
{
	BrowserApplication::instance()->downloadManager()->download(currentPath+QLatin1Char('/')+fileList->currentItem()->text(0), true);

}

void FtpWindow::ftpCommandFinished(int, bool error)
{
#ifndef QT_NO_CURSOR
    setCursor(Qt::ArrowCursor);
#endif

    if (ftp->currentCommand() == QFtp::ConnectToHost) {
        if (error) {
            QMessageBox::information(this, tr("FTP"),
                                     tr("Unable to connect to the FTP server "
                                        "at %1. Please check that the host "
                                        "name is correct.")
                                     .arg(currentPath));
            connectOrDisconnect();
            return;
        }
        fileList->setFocus();
        return;
    }
    if (ftp->currentCommand() == QFtp::Login)
        ftp->list();
    if (ftp->currentCommand() == QFtp::Get) {
        if (error) {
            file->close();
            file->remove();
        } else {
            file->close();
        }
        delete file;
    } else if (ftp->currentCommand() == QFtp::List) {
        if (isDirectory.isEmpty()) {
            fileList->addTopLevelItem(new QTreeWidgetItem(QStringList() << tr("<empty>")));
            fileList->setEnabled(false);
        }
    }
}
void FtpWindow::addToList(const QUrlInfo &urlInfo)
{
    QTreeWidgetItem *item = new QTreeWidgetItem;
    item->setText(0, urlInfo.name());
    item->setText(1, QString::number(urlInfo.size()));
    item->setText(2, urlInfo.owner());
    item->setText(3, urlInfo.group());
    item->setText(4, urlInfo.lastModified().toString(QLatin1String("MMM dd yyyy")));

    QPixmap pixmap(urlInfo.isDir() ? QLatin1String(":/dir.png") : QLatin1String(":/file.png"));
    item->setIcon(0, pixmap);

    isDirectory[urlInfo.name()] = urlInfo.isDir();
    fileList->addTopLevelItem(item);
    if (!fileList->currentItem()) {
        fileList->setCurrentItem(fileList->topLevelItem(0));
        fileList->setEnabled(true);
    }
}
void FtpWindow::processItem(QTreeWidgetItem *item, int /*column*/)
{
    QString name = item->text(0);
    if (isDirectory.value(name)) {
        fileList->clear();
        isDirectory.clear();
        currentPath += QLatin1String("/") + name;
        ftpServerLineEdit->setText(currentPath+QLatin1Char('/'));
        ftp->cd(name);
        ftp->list();
        cdToParentButton->setEnabled(true);
#ifndef QT_NO_CURSOR
        setCursor(Qt::WaitCursor);
#endif
        return;
    } else
        downloadFile();
}
void FtpWindow::cdToParent()
{
#ifndef QT_NO_CURSOR
    setCursor(Qt::WaitCursor);
#endif
    fileList->clear();
    isDirectory.clear();
    QString temp;
    temp = currentPath.left(currentPath.indexOf(QLatin1Char('/'),6));
    if (temp.count()<currentPath.count())
       currentPath = currentPath.left(currentPath.lastIndexOf(QLatin1Char('/')));
    else
        cdToParentButton->setEnabled(false);

    ftp->cd(QLatin1String(".."));
    ftp->list();
    ftpServerLineEdit->setText(currentPath+QLatin1Char('/'));
}

