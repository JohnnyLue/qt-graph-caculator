#include "slot.h"
#include "ui_slot.h"
#include <QDebug>
#include <iostream>
#include <set>
Slot::Slot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::slot)
{
    ui->setupUi(this);
    this->equation=new Equation();
    this->ui->setVisible->setIcon(QIcon(":/canSee.png"));
    this->ui->toolButton->setIcon(QIcon(":/delete.png"));
}


void Slot::setcolor(QColor& color)
{
    this->color=color;
    QPalette pt;
    pt.setColor(QPalette::Background, color);
    ui->label->setAutoFillBackground(true);
    ui->label->setBackgroundRole(QPalette::Background);
    ui->label->setPalette(pt);
}

QColor Slot::getColor()
{
    return color;
}

void Slot::updateEquation()//reset the equation
{

    this->ui->label->setText("");
    try{
        equation->setequation(ui->lineEdit->text().toStdString());
    }
    catch (std::set<std::string> undefineds) {
        std::string message="Unknown valuable name:";
        for(auto i=undefineds.begin();i!=undefineds.end();i++)
        {
            message+=("'"+*i+"',");
        }
        message.pop_back();
        qDebug()<<QString::fromStdString(message)<<endl;

    }
    catch(std::exception& e)
    {
        qDebug()<<e.what();
    }

    if(!equation->isready)
        this->ui->label->setText("E");
}

void Slot::setListWidgetItem(QListWidgetItem *it)
{
    iter=it;
}


bool Slot::visible()
{
    return cansee;
}

Slot::~Slot()
{
    delete ui;
}

void Slot::on_lineEdit_textEdited(const QString &arg1)
{
    if(!cansee)return;

    this->ui->label->setText("");
    bool oriIsSetVal=equation->isSetVal;
    try{
        equation->setequation(arg1.toStdString());
    }
    catch (std::set<std::string> undefineds) {
        std::string message="Unknown valuable name:";
        for(auto i=undefineds.begin();i!=undefineds.end();i++)
        {
            message+=("'"+*i+"',");
        }
        message.pop_back();
        qDebug()<<QString::fromStdString(message)<<endl;
    }
    catch(std::exception& e)
    {
        qDebug()<<e.what();
    }

    if(oriIsSetVal||equation->isSetVal)
        emit updateView();
    else
        emit initDrawSelf(this);

    if(!equation->isready)
        this->ui->label->setText("E");

}


void Slot::on_toolButton_clicked()
{
    emit deleteThis(iter);
}


void Slot::on_setVisible_clicked()
{
    if(equation->isSetVal)return;

    if(cansee)
    {
        cansee=false;
        this->ui->setVisible->setIcon(QIcon(":/canNotSee.png"));
        this->ui->label->setText("X");
    }
    else
    {
        cansee=true;
        this->ui->setVisible->setIcon(QIcon(":/canSee.png"));
        updateEquation();
    }
    emit initDrawSelf(this);
}

