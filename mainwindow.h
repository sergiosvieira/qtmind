#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStandardItemModel;
class QStandardItem;
class QJsonArray;
class QJsonObject;
class QPdfDocument;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    static const char* kName;
    static const char* kItems;
    static const char* kLastReinforcement;
    static const char* kNextReinforcement;
    static const char* kRetention;
    static const char* kTotalReinforcement;
    static const char* kType;
    static const char* kPage;
    void insertRow(QJsonObject& object, const QModelIndex& modelIndex);
    void insertChild(QJsonObject &object, const QModelIndex& modelIndex);
    QJsonArray extractValues(QJsonObject& object, QStringList& list);
    void createModel();
    QString getValue(const QModelIndex &index, int col);
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionAdicionar_Novo_triggered();
    void on_actionNovo_Topico_triggered();

    void on_actionRemover_Item_triggered();

    void on_actionCarregar_triggered();

    void on_actionSalvar_triggered();

    void on_treeView_doubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;
    QStandardItemModel* model = nullptr;
    QStandardItem* selectedItem = nullptr;
    QPdfDocument *m_document;
};

#endif // MAINWINDOW_H
