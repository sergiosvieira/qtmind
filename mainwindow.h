#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractItemModel>
#include <vector>

class QStandardItemModel;
class QStandardItem;
class QJsonArray;
class QJsonObject;
class QPdfDocument;
class QJsonDocument;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
public:
    typedef struct Parameters
    {
        double c1 = 0.0, c2 = 0.0, c3 = 0.0;
        Parameters(double c1, double c2, double c3): c1(c1), c2(c2), c3(c3) {}
    } Parameters;
private:
    Q_OBJECT
    static const char* kName;
    static const char* kItems;
    static const char* kLastReinforcement;
    static const char* kNextReinforcement;
    static const char* kRetention;
    static const char* kTotalReinforcement;
    static const char* kType;
    static const char* kPage;
    static const char* kDateFormat;
    void insertRow(QJsonObject& object, const QModelIndex& modelIndex);
    void insertChild(QJsonObject &object, const QModelIndex& modelIndex);
    QJsonArray extractValues(QJsonObject& object, QStringList& list);
    void createModel();
    QString getValue(const QModelIndex &index, int col);
     QMap<QString, QString> extractValuesFromModel(const QAbstractItemModel &model, int row, QModelIndex& parent);
    QJsonDocument *loadFromModel(const QAbstractItemModel &model);
    QJsonObject forEach(const QModelIndex &parentIndex,
                         const QAbstractItemModel &model);
    QStringList getColumnNames();
    QStringList &getJsonKeys();
    Parameters getParameters(int index);
//    void forEach(QAbstractItemModel *model, QModelIndex parent = QModelIndex());
    void iterate(const QModelIndex & index, const QAbstractItemModel * model,
                 const std::function<void(const QModelIndex&, int)> & fun,
                 int depth = 0);
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void setValue(const QModelIndex &index, int col, QVariant value);
private slots:
    void on_actionAdicionar_Novo_triggered();
    void on_actionNovo_Topico_triggered();
    void on_actionRemover_Item_triggered();
    void on_actionCarregar_triggered();
    void on_actionSalvar_triggered();
    void on_treeView_doubleClicked(const QModelIndex &index);
    void on_actionNovo_Link_triggered();
    /*
     * f(x) = c1 . e^(-x - c2/ c3)
     */
    double f(double x, const Parameters& p);
    void on_actionZerar_T_pico_triggered();

private:
    Ui::MainWindow *ui;
    QStandardItemModel* model = nullptr;
    QStandardItem* selectedItem = nullptr;
    QPdfDocument *m_document = nullptr;
};

#endif // MAINWINDOW_H
