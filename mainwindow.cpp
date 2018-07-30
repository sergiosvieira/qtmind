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
#include <cmath>


const char* MainWindow::kName = "name";
const char* MainWindow::kItems = "items";
const char* MainWindow::kLastReinforcement = "last_reinforcement_end";
const char* MainWindow::kNextReinforcement = "next_reinforcement_start";
const char* MainWindow::kRetention = "retention";
const char* MainWindow::kTotalReinforcement = "total_reinforcements";
const char* MainWindow::kType = "type";
const char* MainWindow::kPage = "page";
const char* MainWindow::kDateFormat = "yyyy-MM-dd hh:mm:ss";
const std::vector<MainWindow::Parameters> kParameters;

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
    QStringList list = this->getColumnNames();
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
        QModelIndex index = ui->treeView->rootIndex();
        QModelIndex childIndex = model->index(index.row() + 1, 0, index.parent());
        QJsonObject object;
        object[kName] = text;
        this->insertRow(object, childIndex);
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
    QString fileName = QFileDialog::getSaveFileName(this,
            tr("Salvar"), "",
            tr("JSON (*.json);;All Files (*)"));
    if (fileName.isEmpty())
    {
        return;
    }
    else
    {
        QJsonDocument* document = this->loadFromModel(*ui->treeView->model());
        QFile jsonFile(fileName);
        jsonFile.open(QFile::WriteOnly);
        jsonFile.write(document->toJson());
        jsonFile.close();
    }
}

QJsonDocument *MainWindow::loadFromModel(const QAbstractItemModel &model)
{
    QJsonDocument* document = new QJsonDocument();
    QModelIndex rootIndex = ui->treeView->rootIndex();
    QJsonObject rootObject = this->forEach(rootIndex, model);
    document->setObject(rootObject);
    return document;
}

QJsonObject MainWindow::forEach(const QModelIndex &parentIndex,
                                const QAbstractItemModel &model)
{
    QJsonObject object;
    QJsonArray array;
    auto rows = model.rowCount(parentIndex);
    auto cols = model.columnCount(parentIndex);
    QStringList& list = this->getJsonKeys();
    if (model.hasChildren(parentIndex))
    {
        for (int r = 0; r < rows; ++r)
        {
            for (int c = 0; c < cols; ++c)
            {
                QModelIndex index = model.index(r, c, parentIndex);
                object.insert(list[c], model.data(index).toString());
            }
            QModelIndex childIndex = model.index(r, 0, parentIndex);
            QJsonObject child = this->forEach(childIndex, model);
            array.append(child);
        }
    }
    object.insert(kName, model.data(parentIndex).toString());
    object.insert(kItems, array);
    return object;
}

QMap<QString, QString> MainWindow::extractValuesFromModel(const QAbstractItemModel &model, int row, QModelIndex& parent)
{
    QMap<QString, QString> result;
    QStringList list = this->getJsonKeys();
    for (int col = 0; col < model.columnCount(parent); ++col)
    {
        QModelIndex index = model.index(row, col, parent);
        result[list[col]] = model.data(index).toString();
    }
    return result;
}

void MainWindow::iterate(const QModelIndex & index, const QAbstractItemModel * model,
             const std::function<void(const QModelIndex&, int)> & fun,
             int depth)
{
    if (index.isValid())
        fun(index, depth);
    if (!model->hasChildren(index)) return;
    auto rows = model->rowCount(index);
    auto cols = model->columnCount(index);
    for (int i = 0; i < rows; ++i)
        for (int j = 0; j < cols; ++j)
            iterate(model->index(i, j, index), model, fun, depth+1);
}


QStringList MainWindow::getColumnNames()
{
    static QStringList list = {
        tr("Nome"),
        tr("Tipo"),
        tr("Página"),
        tr("Data da última revisão"),
        tr("Data da próxima revisão"),
        tr("% de Retenção"),
        tr("Número de Revisões"),
    };
    return list;
}

QStringList& MainWindow::getJsonKeys()
{
    static QStringList list = {
        kName,
        kType,
        kPage,
        kLastReinforcement,
        kNextReinforcement,
        kRetention,
        kTotalReinforcement,
        kItems
    };
    return list;
}

MainWindow::Parameters MainWindow::getParameters(int index)
{
    static Parameters p[] =
    {
        {100.0, 0.0, 19.576151889712175},
        {90.0, 10.0, 73.98910387129295},
        {100.0, 10.0, 142.36832371544847},
        {100.0, 25.0, 487.3931436555927}

    };
   return p[index];
}

void MainWindow::setValue(const QModelIndex &index,
                          int col,
                          QVariant value)
{
    QModelIndex setIndex = model->index(index.row(), col, index.parent());
    model->setData(setIndex, value);
}

void MainWindow::on_treeView_doubleClicked(const QModelIndex &index)
{
    QString type = this->getValue(index, 1);
    if (type == "PDF")
    {
        QString filepath = this->getValue(index, 0);
        QString page = this->getValue(index, 2);
        QString url = QString("%1#page=%2").arg(filepath).arg(page);
        if (!QDesktopServices::openUrl(QUrl(url, QUrl::StrictMode)))
        {
            QMessageBox msg;
            msg.setText(tr("Falha ao tentar abrir o PDF!"));
                msg.exec();
        };
    }
    else if (type == "LINK")
    {
        QString filepath = this->getValue(index, 0);
        QString page = this->getValue(index, 2);
        QString url = QString("%1#page=%2").arg(filepath).arg(page);
        QDesktopServices::openUrl(QUrl(url, QUrl::TolerantMode));
    }
    else if (type == "TOPIC")
    {
        QString lastDateStr = this->getValue(index, 3);
        if (lastDateStr.isEmpty())
        {
            /* Data da última revisão */
            QDateTime currentDate = QDateTime::currentDateTime();
            QString currentDateStr = currentDate.toString(kDateFormat);
            this->setValue(index, 3, QVariant(currentDateStr));
            /* Data da próxima revisão */
            QDateTime nextDate = currentDate.addDays(3);
            QString nextDateStr = nextDate.toString(kDateFormat);
            this->setValue(index, 4, QVariant(nextDateStr));
            /* % de Retenção */
            this->setValue(index, 5, QVariant(100));
            /* % Total de Revisões */
            this->setValue(index, 6, QVariant(1));
        }
        else
        {
            QDateTime lastDate = QDateTime::fromString(lastDateStr, kDateFormat);
            QDateTime nextDate = QDateTime::fromString(this->getValue(index, 4), kDateFormat);
            QDateTime currentDate = QDateTime::currentDateTime();
            if (currentDate < nextDate)
            {
                QMessageBox msg;
                msg.setText(tr("Ainda não é hora de revisar."));
                msg.exec();
            }
            else
            {
                /* Data da última revisão */
                QString currentDateStr = currentDate.toString(kDateFormat);
                this->setValue(index, 3, QVariant(currentDateStr));
                /* Data da próxima revisão */
                int total = this->getValue(index, 6).toInt();
                int days = (total == 2) ? (7) : ((total == 3) ? (25) : (25));
                QDateTime nextDate = currentDate.addDays(days);
                QString nextDateStr = nextDate.toString(kDateFormat);
                this->setValue(index, 4, QVariant(nextDateStr));
                /* % de Retenção */
                Parameters p = this->getParameters(total % 3);
                int nDays = currentDate.daysTo(lastDate);
                double r = this->f(nDays, p);
                this->setValue(index, 5, QVariant(r));
                /* % Total de Revisões */
                this->setValue(index, 6, QVariant(++total));
            }
        }
    }
}

void MainWindow::on_actionNovo_Link_triggered()
{
    static int id = 1;
    bool ok = false;
    QString title = tr("Nome Link:");
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
        object[kType] = "LINK";
        this->insertChild(object, index);
        ui->treeView->selectionModel()->setCurrentIndex(model->index(0, 0, index),
                                                QItemSelectionModel::ClearAndSelect);
    }
}

double MainWindow::f(double x, const Parameters& p)
{
    return p.c1 * exp((-x-p.c2)/p.c3);
}
