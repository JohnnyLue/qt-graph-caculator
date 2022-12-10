#include "view.h"
#include "slot.h"
#include "controler.h"
#include <QColor>
#include <QPen>
#include <queue>
#include <set>

#define FMAX 1e10
#define FMIN -1e10

namespace setting{
    float maxValueBound=1e2;//100
    float range=5.5;
    float xpos=0,ypos=0;
    std::vector<QColor> colors;
    void setColors();
    void setbound();
    QPointF upLeft(-range,range),butRight(range,-range);
    QPointF oldUpLeft,oldButRight;
    float startDragx,startDragy;
    bool mouseOnDrag=false;
    float xPrecision=0.01;
    float yPrecision=0.0001;
}

void setting::setbound()
{
    oldUpLeft=upLeft;
    oldButRight=butRight;
    upLeft.setX(upLeft.x()<xpos-range-1?upLeft.x():xpos-range-1);
    upLeft.setY(upLeft.y()>ypos+range+1?upLeft.y():ypos+range+1);
    butRight.setX(butRight.x()>xpos+range+1?butRight.x():xpos+range+1);
    butRight.setY(butRight.y()<ypos-range-1?butRight.y():ypos-range-1);

}
void setting::setColors()
{
    colors.push_back(Qt::blue);//0
    colors.push_back(Qt::darkCyan);
    colors.push_back(Qt::darkBlue);//2
    colors.push_back(Qt::red);
    colors.push_back(Qt::green);//4
    colors.push_back(Qt::darkGray);
    colors.push_back(Qt::darkMagenta);//6
}

View::View(QWidget* parent):QCustomPlot(parent){
    setting::setbound();
    setting::setColors();

    xAxis->setRange(setting::xpos-setting::range,setting::xpos+setting::range);
    yAxis->setRange(setting::ypos-setting::range,setting::ypos+setting::range);

}

void View::wheelEvent(QWheelEvent *event)
{
    int d=event->delta()/120.0;//wheel change value,-1 or 1

    float nx = xAxis->pixelToCoord(event->pos().x());
    float ny = yAxis->pixelToCoord(event->pos().y());

    setting::xpos=setting::xpos*(4/5.0)+d*nx*(1/5.0);
    setting::ypos=setting::ypos*(4/5.0)+d*ny*(1/5.0);


    if(d<0)
        setting::range*=1.2;
    else
        setting::range*=0.8;

    xAxis->setRange(setting::xpos-setting::range,setting::xpos+setting::range);
    yAxis->setRange(setting::ypos-setting::range,setting::ypos+setting::range);

    setting::setbound();

    updateDraw();
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    if(setting::mouseOnDrag)
    {
        float mousex = xAxis->pixelToCoord(event->pos().x());
        float mousey = yAxis->pixelToCoord(event->pos().y());
        setting::xpos-=mousex-setting::startDragx;
        setting::ypos-=mousey-setting::startDragy;

        setting::setbound();

        xAxis->setRange(setting::xpos-setting::range,setting::xpos+setting::range);
        yAxis->setRange(setting::ypos-setting::range,setting::ypos+setting::range);

        updateDraw();
    }
}

void View::mousePressEvent(QMouseEvent *event)
{
    setting::mouseOnDrag=true;
    setting::startDragx = xAxis->pixelToCoord(event->pos().x());
    setting::startDragy = yAxis->pixelToCoord(event->pos().y());
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    setting::mouseOnDrag=false;
}

void View::paintEvent(QPaintEvent *event)
{
    QCustomPlot::paintEvent(event);
    QPainter p(this);
    QPen thinpen,boldpen,mediumpen;
    thinpen.setWidthF(0.5);
    thinpen.setColor(Qt::gray);
    mediumpen.setWidthF(1.5);
    mediumpen.setColor(Qt::gray);
    boldpen.setWidth(2);
    float interval=setting::range/10;
    float trueInterval=0.0001;
    for(;trueInterval<interval;trueInterval*=2.5);
    interval=trueInterval;
    int muti=0;

    float limit=std::max(std::max(abs(setting::ypos-setting::range),abs(setting::ypos+setting::range)),std::max(abs(setting::xpos-setting::range),abs(setting::range+setting::xpos)));
    for(;interval*muti<limit+1;muti++)
    {
        if(muti==0)
            p.setPen(boldpen);
        else if(muti%4==0)
            p.setPen(mediumpen);
        else
            p.setPen(thinpen);

        p.drawLine(xAxis->coordToPixel(interval*muti),0,xAxis->coordToPixel(interval*muti),1000);
        p.drawLine(xAxis->coordToPixel(-interval*muti),0,xAxis->coordToPixel(-interval*muti),1000);
        p.drawLine(0,yAxis->coordToPixel(interval*muti),1000,yAxis->coordToPixel(interval*muti));
        p.drawLine(0,yAxis->coordToPixel(-interval*muti),1000,yAxis->coordToPixel(-interval*muti));
        if(muti%4==0)
        {
            p.setPen(boldpen);
            p.drawText(xAxis->coordToPixel(interval*muti),yAxis->coordToPixel(0),QString::number(interval*muti));
            p.drawText(xAxis->coordToPixel(-interval*muti),yAxis->coordToPixel(0),QString::number(-interval*muti));
            p.drawText(xAxis->coordToPixel(0),yAxis->coordToPixel(interval*muti),QString::number(interval*muti));
            p.drawText(xAxis->coordToPixel(0),yAxis->coordToPixel(-interval*muti),QString::number(-interval*muti));
        }
    }

}

int View::getValidGraph()
{
    for(int i=0;i<this->graphCount();i++)
    {
        if(this->graph(i)->dataCount()<10)
        {
            return i;//unused graph
        }
    }

    this->addGraph();//if no valid graph, create new
    return this->graphCount()-1;
}

void View::updateView()//connect with slot SIGNAL "updateView()"
{
    updateValuable();

    for(int slotindex=0;slotindex<this->slotList.size();slotindex++)
    {
        Slot *slot=this->slotList[slotindex];
        if(!slot->equation->isSetVal)
        {
            slot->updateEquation();
            initDraw(slot);
        }
    }
    replot();
}

void View::initDraw(Slot *slot)
{
    clearSlot(slot);
    updateValuable();
    if(slot->equation->isSetVal||!slot->equation->isready||!slot->visible())
    {
        replot();
        return;
    }

    QPen pen=QPen(slot->getColor());
    pen.setWidthF(3.5);

    float lastcuc,curcuc;

    bool firstIn=true,outOfBound,lastOutOfBound;
    for (float tx=setting::upLeft.x(); tx<setting::butRight.x(); tx+=setting::xPrecision)
    {

        curcuc=-setting::maxValueBound;//no less then -1000
        int startPositive=(slot->equation->calculateDiff(tx,curcuc)>0)?1:-1;
        float step=setting::maxValueBound;
        for(;step>setting::yPrecision&&curcuc<setting::maxValueBound;step/=2)//no more then setting::maxValueBound
        {
            while(curcuc<setting::maxValueBound&&slot->equation->calculateDiff(tx,step+curcuc)*startPositive>0)
            {
                curcuc+=step;
            }
        }
        outOfBound=(abs(curcuc)>setting::maxValueBound-1);

        int cur=-1;
        float dis=FMAX;

        for(auto i=slot->usingGraphs.begin();i!=slot->usingGraphs.end();i++)
        {
            if(tx-(graph(*i)->data()->constEnd()-1)->key<dis)
            {
                //qDebug()<<(graph(*i)->data()->end()-1)->key<<" "<<curcuc;
                cur=*i;
                dis=tx-(graph(*i)->data()->constEnd()-1)->key;
            }
        }


        if(cur==-1)
        {
            cur=getValidGraph();
            slot->usingGraphs.push_back(cur);
            graph(cur)->setPen(pen);
        }
        else if(dis>setting::xPrecision*1.5)
        {
            cur=getValidGraph();
            slot->usingGraphs.push_back(cur);
            graph(cur)->setPen(pen);
        }

        if(!firstIn&&!lastOutOfBound&&outOfBound)//in to out
        {
            if(abs(lastcuc)>setting::maxValueBound/2)
            {
                if(lastcuc<0)
                    graph(cur)->addData(tx,FMIN);
                else
                    graph(cur)->addData(tx,FMAX);
            }
        }else if(!firstIn&&lastOutOfBound&&!outOfBound)//out to in
        {
            if(abs(curcuc)>setting::maxValueBound/2)
            {
                if(curcuc<0)
                    graph(cur)->addData(tx,FMIN);
                else
                    graph(cur)->addData(tx,FMAX);
            }
        }else if(!outOfBound)
        {
            graph(cur)->addData(tx,curcuc);
        }

        lastOutOfBound=outOfBound;
        lastcuc=curcuc;
        firstIn=false;
    }
    replot();
}

void View::updateDraw()
{

    for(int slotindex=0;slotindex<this->slotList.size();slotindex++)
    {
        Slot *slot=this->slotList[slotindex];

        if(slot->equation->isSetVal||!slot->equation->isready||!slot->visible())continue;

        QPen pen=QPen(slot->getColor());
        pen.setWidthF(3.5);

        float lastcuc,curcuc=0;

        //bound go left
        bool firstIn=true,outOfBound,lastOutOfBound;;
        for(float tx=setting::oldUpLeft.x(); tx>setting::upLeft.x(); tx-=setting::xPrecision)
        {
            curcuc=-setting::maxValueBound;//no less then -setting::maxValueBound
            int startPositive=(slot->equation->calculateDiff(tx,curcuc)>0)?1:-1;
            float step=setting::maxValueBound;

            for(;step>setting::yPrecision&&curcuc<setting::maxValueBound;step/=2)//no more then setting::maxValueBound
            {
                while(curcuc<setting::maxValueBound&&slot->equation->calculateDiff(tx,step+curcuc)*startPositive>0)
                {
                    curcuc+=step;
                }
            }
            outOfBound=(abs(curcuc)>setting::maxValueBound-1);

            int cur=-1;

            float dis=FMAX;
            for(auto i=slot->usingGraphs.begin();i!=slot->usingGraphs.end();i++)
            {
                if(abs(graph(*i)->data()->constBegin()->key-tx)<dis)
                {
                    cur=*i;
                    dis=abs(graph(*i)->data()->constBegin()->key-tx);//greedy to find closest graph
                }
            }

            if(cur==-1)
            {
                cur=getValidGraph();
                slot->usingGraphs.push_back(cur);
                graph(cur)->setPen(pen);
            }
            else if(dis>setting::xPrecision*1.5)
            {
                cur=getValidGraph();
                slot->usingGraphs.push_back(cur);
                graph(cur)->setPen(pen);

            }


            if(!firstIn&&!lastOutOfBound&&outOfBound)//in to out
            {
                if(abs(lastcuc)>setting::maxValueBound/2)
                {
                    if(lastcuc<0)
                        graph(cur)->addData(tx,FMIN);
                    else
                        graph(cur)->addData(tx,FMAX);
                }
            }else if(!firstIn&&lastOutOfBound&&!outOfBound)//out to in
            {
                if(abs(curcuc)>setting::maxValueBound/2)
                {
                    if(curcuc<0)
                        graph(cur)->addData(tx,FMIN);
                    else
                        graph(cur)->addData(tx,FMAX);
                }
            }else if(!outOfBound)
            {
                graph(cur)->addData(tx,curcuc);
            }

            lastOutOfBound=outOfBound;
            lastcuc=curcuc;

            firstIn=false;
        }

        //bound go right
        firstIn=true;
        for(float tx=setting::oldButRight.x(); tx<setting::butRight.x(); tx+=setting::xPrecision)
        {

            lastcuc=curcuc;

            curcuc=-setting::maxValueBound;//no less then -1000
            int startPositive=(slot->equation->calculateDiff(tx,curcuc)>0)?1:-1;
            float step=setting::maxValueBound;
            for(;step>setting::yPrecision&&curcuc<setting::maxValueBound;step/=2)//no more then 1000
            {
                while(curcuc<setting::maxValueBound&&slot->equation->calculateDiff(tx,step+curcuc)*startPositive>0)
                {
                    curcuc+=step;
                }
            }
            outOfBound=(abs(curcuc)>setting::maxValueBound-1);

            int cur=-1;
            float dis=FMAX;

            for(auto i=slot->usingGraphs.begin();i!=slot->usingGraphs.end();i++)
            {
                if(tx-(graph(*i)->data()->end()-1)->key<dis)
                {
                    cur=*i;
                    dis=tx-(graph(*i)->data()->end()-1)->key;
                }
            }

            if(cur==-1)
            {
                cur=getValidGraph();
                slot->usingGraphs.push_back(cur);
                graph(cur)->setPen(pen);
            }
            else if(dis>setting::xPrecision*1.5)
            {
                cur=getValidGraph();
                slot->usingGraphs.push_back(cur);
                graph(cur)->setPen(pen);
            }

            if(!firstIn&&!lastOutOfBound&&outOfBound)//in to out
            {
                if(abs(lastcuc)>setting::maxValueBound/2)
                {
                    if(lastcuc<0)
                        graph(cur)->addData(tx,FMIN);
                    else
                        graph(cur)->addData(tx,FMAX);
                }
            }else if(!firstIn&&lastOutOfBound&&!outOfBound)//out to in
            {
                if(abs(curcuc)>setting::maxValueBound/2)
                {
                    if(curcuc<0)
                        graph(cur)->addData(tx,FMIN);
                    else
                        graph(cur)->addData(tx,FMAX);
                }
            }else if(!outOfBound)
            {
                graph(cur)->addData(tx,curcuc);
            }

            lastOutOfBound=outOfBound;
            lastcuc=curcuc;

            firstIn=false;
        }
    }

    replot();
}

void View::clearSlot(Slot *s)
{

    for(auto iter=s->usingGraphs.begin();iter!=s->usingGraphs.end();iter++)
    {
        QVector<double>empty;
        graph(*iter)->setData(empty,empty);
    }
    s->usingGraphs.clear();
}

void View::setConnect(Slot *s)
{
    s->setcolor(setting::colors[curColorIndex%7]);
    curColorIndex++;
    QObject::connect(s, SIGNAL(updateView()), this, SLOT(updateView()));
    QObject::connect(s,SIGNAL(initDrawSelf(Slot*)),this,SLOT(initDraw(Slot*)));
}

void View::updateValuable()
{
    Controler::valuables.clear();//reset valuables

    std::queue <Slot*> sQueue;

    //push all "set valuable" into queue
    for(int slotindex=0;slotindex<this->slotList.size();slotindex++)
    {
        Slot *slot=this->slotList[slotindex];
        if(slot->equation->isSetVal)
        {
            sQueue.push(slot);
        }
    }

    int limit=(sQueue.size()*(sQueue.size()+1))/2,counter=0;
    while(sQueue.size()>0&&counter<limit)
    {
        counter++;
        sQueue.front()->updateEquation();

        if(sQueue.front()->equation->isready)
        {
            sQueue.pop();//update success
        }
        else
        {
            //update it later
            sQueue.push(sQueue.front());
            sQueue.pop();
        }
    }
};

