#include "mainwindow.h"
#include "stdlib.h"
#include "QFileDialog"
#include "QMessageBox"
#include "compresser.h"
#include "ui_mainwindow.h"
#include "zip.h"

void MainWindow::setRadiobtnCheck()
{
    if (this->checked == 0)
    {
        ui->radioButton->setChecked(true);
        ui->radioButton_2->setChecked(false);
    }
    else if (this->checked == 1)
    {
        ui->radioButton->setChecked(false);
        ui->radioButton_2->setChecked(true);
    }
}

MainWindow::MainWindow(QWidget *parent) : QWidget(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setRadiobtnCheck();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if (directory.length() == 0)
    {
        QMessageBox::warning(this, "Warning", "No file is selected");
        return;
    }
    QByteArray cdata = directory.toLocal8Bit();
    string filebyte = string(cdata);
    if (checked == 0)
    {
        hfmCompress(filebyte);
        QMessageBox::information(this, "Info", "OK");
    }
    else if (checked == 1)
    {
        string ok = zipCompress(filebyte);
        QMessageBox::information(this, "Finish", QString::fromStdString(ok));
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    if (directory.length() == 0)
    {
        QMessageBox::warning(this, "Warning", "No file is selected");
        return;
    }
    QByteArray cdata = directory.toLocal8Bit();
    string filebyte = string(cdata);
    if (checked == 0)
    {
        decompress(filebyte);
        QMessageBox::information(this, "Info", "OK");
    }
    else if (checked == 1)
    {
        string ok = zipUncompress(filebyte);
        QMessageBox::information(this, "Finish", QString::fromStdString(ok));
    }
}

void MainWindow::on_toolButton_clicked()
{
    directory = QFileDialog::getOpenFileName();
    ui->lineEdit->setText(directory);
}

void MainWindow::on_radioButton_clicked()
{
    checked = 0;
}

void MainWindow::on_radioButton_2_clicked()
{
    checked = 1;
}
