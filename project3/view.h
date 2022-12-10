#ifndef VIEW_H
#define VIEW_H

#include "qcustomplot.h"
#include "slot.h"

class View : public QCustomPlot
{
    Q_OBJECT

public:
    View(QWidget* parent);

    void updateDraw();
    void clearSlot(Slot*);
    void setConnect(Slot*);
    void updateValuable();
    QList <Slot*> slotList;
private:
    void mousePressEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    int getValidGraph();
    int curColorIndex=0;

public slots:
    void updateView();
    void initDraw(Slot*);
};

#endif // VIEW_H
