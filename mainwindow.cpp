#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    model = new QStandardItemModel(this);
    /*
    QStandardItem* group = new QStandardItem("Grupo1");
    QStandardItem* item1 = new QStandardItem("Item1");
    QStandardItem* item2 = new QStandardItem("Item2");
    group->appendRow(item1);
    group->appendRow(item2);
    model->appendRow(group);
    */
    ui->columnView->setModel(model);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAdicionar_Novo_triggered()
{
    bool ok = false;
    QString title = tr("Nome do Concurso:");
    if (ui->columnView->selectedIndexes().size() > 0)
    {
        title = tr("Nome do tópico:");
    }
    QString text = QInputDialog::getText(
        this,
        tr("Reforço Mental"),
        title,
        QLineEdit::Normal,
        "Concurso",
        &ok
    );
      if (ok && !text.isEmpty())
      {

          QStandardItem* item = new QStandardItem(text);
          if (ui->columnView->se
          {
              int index = ui->columnView->selectedIndexes()[0];
              QStandardItem* root = model->children()[index];
              root->appendRow(item);
              model->appendRow(root);
          }
          else model->appendRow(item);
      }

}
