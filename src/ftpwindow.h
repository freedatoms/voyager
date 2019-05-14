/**
 * Copyright 2009 Tomáš Frýda <t.fryda@gmail.com>
 */
#ifndef FTPWINDOW_H
#define FTPWINDOW_H

#include <QMetaType>
#include <QDialog>
#include <QHash>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QFile;
class QFtp;
class QLabel;
class QLineEdit;
class QTreeWidget;
class QTreeWidgetItem;
class QPushButton;
class QUrlInfo;
QT_END_NAMESPACE

class FtpWindow : public QWidget
{
    Q_OBJECT

public:
    FtpWindow(QWidget *parent = 0);
    FtpWindow(const FtpWindow &copy);
public slots:
    void setServer(QString str);
    void Connect();

private slots:
    void connectOrDisconnect();
    void downloadFile();
    void ftpCommandFinished(int commandId, bool error);
    void addToList(const QUrlInfo &urlInfo);
    void processItem(QTreeWidgetItem *item, int column);
    void cdToParent();


private:
    QLineEdit *ftpServerLineEdit;
    QLabel *ftpServerLabel;
    QTreeWidget *fileList;
    QPushButton *cdToParentButton;
    QDialogButtonBox *buttonBox;

    QHash<QString, bool> isDirectory;
    QString currentPath;
    QFtp *ftp;
    QFile *file;
};
Q_DECLARE_METATYPE(FtpWindow)
#endif
