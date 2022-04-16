#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_toolButton_clicked();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QString directory;
    unsigned int checked = 0;
    void setRadiobtnCheck();
};

#endif
