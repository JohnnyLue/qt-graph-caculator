#ifndef CONTROLER_H
#define CONTROLER_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <map>
#include "slot.h"
#include "ui_slot.h"
#include "view.h"
//#include <QVBoxLayout>


namespace Ui {
class controler;
}

class Controler : public QWidget
{
    Q_OBJECT

public:
    explicit Controler(QWidget *parent = nullptr);
    void setView(View *_view);
    static std::map<std::string,std::string> valuables;
    static std::string getValuable(const std::string,bool&);
    static void addValuable(const std::string,const std::string);
    static void deleteValuable(const std::string);
    ~Controler();

public slots:
    void deleteSlot(QListWidgetItem*);

private slots:
    void on_toolButton_clicked();


private:
    Ui::controler *ui;
    View *view;

};

#endif // CONTROLER_H
