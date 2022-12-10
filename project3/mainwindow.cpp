#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "equation.h"
#include <cmath>
#include <iostream>
#include <set>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
     ui->setupUi(this);
     ui->view->xAxis->setVisible(false);
     ui->view->yAxis->setVisible(false);

     ui->controler->setView(ui->view);
}

MainWindow::~MainWindow()
{
    delete ui;
}
