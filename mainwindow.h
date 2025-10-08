#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QList>
#include <QMap>
#include <QEvent>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onConversionButtonClicked();
    void onInputValueChanged(const QString &text);

private:
    Ui::MainWindow *ui;

    QList<QLabel*> labelList;
    QList<QPushButton*> buttonList;
    QList<QLineEdit*> lineEditList;

    QMap<QString, QLabel*> labelMap;
    QMap<QString, QLineEdit*> lineEditMap;

    QString activeSuffix; // 현재 선택된 진수: Bin, Oct, Deci, Hexa

    void initializeGroupBoxWidgets();
    void setGroupBoxWidgetSizePolicy();
    void connectConversionButtons();
    void connectLineEditInputs(); // ✅ 추가
    void resetLabelStyles();
    void updateConversions(const QString &base, const QString &value);
    bool isValidInput(const QString &base, const QString &value);
    void PropertySetting(const QString &tabObjectName, const QString &iconPath); // ✅ 아이콘 설정 함수
};
#endif // MAINWINDOW_H
