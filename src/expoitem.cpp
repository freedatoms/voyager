#include "expoitem.h"

ExpoItem::ExpoItem(QPixmap shot,QString pagetitle,QSize size,int i,QWidget *parent)
    : QWidget(parent)
    , tab(i)
{
    this->setMinimumSize(size);
    this->setMaximumSize(size);
    main=new QPushButton(this);
    main->setMinimumSize(size);
    main->setMaximumSize(size);
    main->setStyleSheet(QLatin1String("QPushButton{background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #222, stop: 0.9 #121212, stop: 1 #444);border-style: solid;border-radius: 4px;border-color: #888;}QPushButton:hover{background: #333;}"));
    QPixmap pix=shot.scaled(QSize(size.width()-18,size.height()-45),Qt::KeepAspectRatio);
    imgsize=pix.size();
    boxLayout = new QVBoxLayout(main);
    pixmap = new QLabel(main);
    pixmap->setMinimumSize(imgsize);
    pixmap->setMaximumSize(imgsize);
    boxLayout->addWidget(pixmap,Qt::AlignHCenter);
    boxLayout->setAlignment(pixmap,Qt::AlignHCenter);
    title = new QLabel(main);
    title->setMaximumHeight(25);
    boxLayout->addWidget(title,Qt::AlignCenter);
	pixmap->setPixmap(pix);
    title->setText(pagetitle);
    title->setStyleSheet(QLatin1String("QWidget{background: #bbb;border-style: solid;border-width: 2px;border-radius: 10px;border-color: #888;padding: 3px;}"));
    title->setAlignment(Qt::AlignLeft);

    connect(main,SIGNAL(clicked()),this,SLOT(open()));
}

ExpoItem::~ExpoItem()
{
}

void ExpoItem::open(){
    BrowserApplication::instance()->mainWindow()->tabWidget()->setTab(tab);
    BrowserApplication::instance()->mainWindow()->tabWidget()->expoc->click();
}
