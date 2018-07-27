#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QInputDialog>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>
#include <QDesktopServices>

const char* MainWindow::kName = "name";
const char* MainWindow::kItems = "items";
const char* MainWindow::kLastReinforcement = "last_reinforcement_end";
const char* MainWindow::kNextReinforcement = "next_reinforcement_start";
const char* MainWindow::kRetention = "retention";
const char* MainWindow::kTotalReinforcement = "total_reinforcements";
const char* MainWindow::kType = "type";
const char* MainWindow::kPage = "page";


void MainWindow::insertRow(QJsonObject& object, const QModelIndex &mIndex)
{
    QStringList values;
    QJsonArray array = this->extractValues(object, values);
    if (!model->insertRow(mIndex.row() + 1, mIndex.parent())) return;
    for (int column = 0; column < values.size(); ++column)
    {
        QModelIndex child = model->index(mIndex.row() + 1, column, mIndex.parent());
        model->setData(child, QVariant(values[column]), Qt::EditRole);
        for (int index = 0; index < array.size(); ++index)
        {
            QJsonObject childObject = array[index].toObject();
            this->insertChild(childObject, child);
        }
    }
}

void MainWindow::insertChild(QJsonObject& object, const QModelIndex &index)
{
    if (model->columnCount(index) == 0)
    {
        model->insertColumns(0, 7, index);
    }
    if (!model->insertRow(0, index)) return;
    QStringList values;
    QJsonArray array = this->extractValues(object, values);
    for (int column = 0; column < values.size(); ++column)
    {
        QModelIndex child = model->index(0, column, index);
        model->setData(child, QVariant(values[column]), Qt::EditRole);
        for (int index = 0; index < array.size(); ++index)
        {
            QJsonObject childObject = array[index].toObject();
            this->insertChild(childObject, child);
        }
    }
}

QJsonArray MainWindow::extractValues(QJsonObject &contest, QStringList &list)
{
    QString name = contest[kName].toString();
    QString lastStr = contest[kLastReinforcement].toString();
    QString nextStr = contest[kNextReinforcement].toString();
    QString retention = QString("%1").arg(contest[kRetention].toInt());
    QString reinforcements = QString("%1").arg(contest[kTotalReinforcement].toInt());
    QString type = contest[kType].toString();
    QString page = contest[kPage].toString();
    QStringList auxList = {
        name,
        type,
        page,
        lastStr,
        nextStr,
        retention,
        reinforcements
    };
    list.swap(auxList);
    QJsonArray result = contest[kItems].toArray();
    return result;
}

void MainWindow::createModel()
{
    model = new QStandardItemModel(0, 7, this);
    QStringList list = {
        tr("Nome"),
        tr("Tipo"),
        tr("Página"),
        tr("Data da última revisão"),
        tr("Data da próxima revisão"),
        tr("% de Retenção"),
        tr("Número de Revisões"),
    };
    model->setHorizontalHeaderLabels(list);
    ui->treeView->setModel(model);
}

QString MainWindow::getValue(const QModelIndex &index, int col)
{
    QModelIndex childIndex = model->index(index.row(), col, index.parent());
    return model->data(childIndex).toString();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->createModel();
    for (int i = 0; i < 7; ++i)
    {
        ui->treeView->header()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionAdicionar_Novo_triggered()
{
    static int id = 1;
    bool ok = false;
    QString title = tr("Nome do Concurso:");
    QString text = QInputDialog::getText(
        this,
        tr("Reforço Mental"),
        title,
        QLineEdit::Normal,
        QString("Concurso %1").arg(id++),
        &ok
    );
    if (ok && !text.isEmpty())
    {
        QModelIndex index = ui->treeView->selectionModel()->currentIndex();
        QJsonObject object;
        object[kName] = text;
        this->insertRow(object, index);
    }
}

void MainWindow::on_actionNovo_Topico_triggered()
{
    static int id = 1;
    bool ok = false;
    QString title = tr("Nome do Tópico:");
    QString text = QInputDialog::getText(
        this,
        tr("Reforço Mental"),
        title,
        QLineEdit::Normal,
        QString("Tópico %1").arg(id++),
        &ok
    );
    if (ok && !text.isEmpty())
    {
        QModelIndex index = ui->treeView->selectionModel()->currentIndex();
        QJsonObject object;
        object[kName] = text;
        this->insertChild(object, index);
        ui->treeView->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                                QItemSelectionModel::ClearAndSelect);
    }

}

void MainWindow::on_actionRemover_Item_triggered()
{
    QModelIndex index = ui->treeView->selectionModel()->currentIndex();
    if (model->removeRow(index.row(), index.parent()))
        return;
}

void MainWindow::on_actionCarregar_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Carregar Arquivo"), "",
            tr("JSON (*.json);;All Files (*)"));
    if (fileName.isEmpty())
    {
           return;
    }
    else
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
           QMessageBox::information(this, tr("Unable to open file"),
               file.errorString());
           return;
        }
        QString val = file.readAll();
        file.close();
        QJsonDocument document = QJsonDocument::fromJson(val.toUtf8());
        QJsonObject rootObject = document.object();
        QJsonValue mainItems = rootObject.value(QString(kItems));
        QJsonArray contestsArray = mainItems.toArray();
        createModel();
        for (int index = 0; index < contestsArray.size(); ++index)
        {
            QJsonObject contest = contestsArray[index].toObject();
            this->insertRow(contest, ui->treeView->selectionModel()->currentIndex());
        }
        ui->treeView->expandAll();
    }

}

void MainWindow::on_actionSalvar_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
            tr("Salvar"), "",
            tr("JSON (*.json);;All Files (*)"));
    if (fileName.isEmpty())
    {
           return;
    }
    else
    {

    }
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    QString type = this->getValue(index, 1);
    if (type == "PDF")
    {
        QString filepath = this->getValue(index, 0);
        QString page = this->getValue(index, 2);
        QString url = QString("file://%1#page=%2").arg(filepath).arg(page);
        QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
    }
    else if (type == "LINK")
    {
        QString filepath = this->getValue(index, 0);
        QString page = this->getValue(index, 2);
        QString url = QString("%1#page=%2").arg(filepath).arg(page);
        QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
    }
}
