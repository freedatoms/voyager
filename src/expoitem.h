#ifndef EXPOITEM_H
#define EXPOITEM_H

#include "browserapplication.h"
#include "browsermainwindow.h"
#include "tabwidget.h"

#include <QtGui/QWidget>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>


class ExpoItem : public QWidget
{
    Q_OBJECT

public:
    ExpoItem(QPixmap shot,QString pagetitle,QSize size,int i,QWidget *parent = 0);
    ~ExpoItem();
    QVBoxLayout *boxLayout;
    QLabel *pixmap;
    QLabel *title;
    QPushButton *main;
    QPushButton *exit;
    int tab;
private slots:
    void open();

private:
	QSize imgsize;
};

#endif // EXPOITEM_H
