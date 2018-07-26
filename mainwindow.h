#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStandardItemModel;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionAdicionar_Novo_triggered();

private:
    Ui::MainWindow *ui;
    QStandardItemModel* model = nullptr;
};

#endif // MAINWINDOW_H
