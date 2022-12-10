#include "controler.h"
#include "ui_controler.h"
#include "slot.h"
#include "ui_slot.h"
#include "view.h"

std::map<std::string,std::string> Controler::valuables;

std::string Controler::getValuable(const std::string name, bool& found)
{
    if(valuables.find(name)!=valuables.end())
    {
        found=true;
        return valuables[name];
    }else
    {
        found=false;
        return "";
    }
}

void Controler::addValuable(const std::string name,const std::string content)
{
    if(valuables.find(name)!=valuables.end())
    {
       throw std::exception("Declared valuable name");
    }

    valuables[name]=content;
}

void Controler::deleteValuable(const std::string name)
{
    if(valuables.find(name)==valuables.end())
    {
       return;
    }

    valuables.erase(name);
}

Controler::Controler(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::controler)
{
    ui->setupUi(this);
}

void Controler::setView(View *_view)
{
    this->view=_view;
}

Controler::~Controler()
{
    delete ui;
}

void Controler::on_toolButton_clicked()
{
    Slot *s=new Slot;

    this->view->slotList.append(s);

    s->setFixedSize(s->size());
    this->view->setConnect(s);

    QListWidgetItem *i=new QListWidgetItem();
    i->setSizeHint(s->size());

    ui->listWidget->addItem(i);
    ui->listWidget->setItemWidget(i,s);

    s->setListWidgetItem(i);
    connect(s,SIGNAL(deleteThis(QListWidgetItem*)),this,SLOT(deleteSlot(QListWidgetItem*)));
}

void Controler::deleteSlot(QListWidgetItem* it)
{
    Slot* t=(Slot*)(sender());
    this->view->clearSlot(t);
    this->view->slotList.removeOne(t);
    delete it;
    qDebug()<<this->view->slotList.size();
    this->view->updateView();
}

