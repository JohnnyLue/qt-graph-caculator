#ifndef SLOT_H
#define SLOT_H

#include <QWidget>
#include <QPen>
#include "equation.h"
#include <QListWidgetItem>

namespace Ui {
class slot;
}

class Slot : public QWidget
{
    Q_OBJECT

public:
    explicit Slot(QWidget *parent = nullptr);
    void setcolor(QColor&);
    QColor getColor();
    void updateEquation();
    void setListWidgetItem(QListWidgetItem*);
    bool visible();
    Equation *equation;
    std::vector<int> usingGraphs;
    ~Slot();
signals:
    void updateView();
    void initDrawSelf(Slot*);
    void deleteThis(QListWidgetItem*);


private slots:
    void on_lineEdit_textEdited(const QString &arg1);

    void on_toolButton_clicked();

    void on_setVisible_clicked();

private:
    Ui::slot *ui;
    bool cansee=true;
    QListWidgetItem* iter;

    QColor color;


};

#endif // SLOT_H
