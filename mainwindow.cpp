#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initializeGroupBoxWidgets();
    setGroupBoxWidgetSizePolicy();
    connectConversionButtons();

    connect(ui->leInputValue, &QLineEdit::textChanged, this, &MainWindow::onInputValueChanged);

    // 초기 DEC 모드 설정
    activeSuffix = "Deci";
    ui->leDeci->setEnabled(true);
    labelMap["Deci"]->setStyleSheet("background-color: black; color: white;");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initializeGroupBoxWidgets()
{
    labelList = ui->groupBox->findChildren<QLabel*>();
    buttonList = ui->groupBox->findChildren<QPushButton*>();
    lineEditList = ui->groupBox->findChildren<QLineEdit*>();

    for (QLabel* label : labelList) {
        QString name = label->objectName();
        if (name.startsWith("lb")) {
            QString key = name.mid(2);
            labelMap[key] = label;
        }
    }

    for (QLineEdit* edit : lineEditList) {
        QString name = edit->objectName();
        if (name.startsWith("le") && name != "leInputValue") {
            QString key = name.mid(2);
            lineEditMap[key] = edit;
            edit->installEventFilter(this); // 클릭 이벤트 필터 등록
        }
    }
}

void MainWindow::setGroupBoxWidgetSizePolicy()
{
    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    for (QLabel* label : labelList) label->setSizePolicy(policy);
    for (QPushButton* button : buttonList) button->setSizePolicy(policy);
    for (QLineEdit* edit : lineEditList) edit->setSizePolicy(policy);
}

void MainWindow::connectConversionButtons()
{
    connect(ui->pbnBin, &QPushButton::clicked, this, &MainWindow::onConversionButtonClicked);
    connect(ui->pbnOct, &QPushButton::clicked, this, &MainWindow::onConversionButtonClicked);
    connect(ui->pbnDeci, &QPushButton::clicked, this, &MainWindow::onConversionButtonClicked);
    connect(ui->pbnHexa, &QPushButton::clicked, this, &MainWindow::onConversionButtonClicked);
}

void MainWindow::resetLabelStyles()
{
    for (QLabel* label : labelList) label->setStyleSheet("");
    for (QLineEdit* edit : lineEditMap.values()) edit->setEnabled(false);
}

void MainWindow::onConversionButtonClicked()
{
    QPushButton* senderBtn = qobject_cast<QPushButton*>(sender());
    if (!senderBtn) return;

    QString suffix = senderBtn->objectName().mid(3); // "Bin", "Oct", etc.
    activeSuffix = suffix;

    resetLabelStyles();

    if (labelMap.contains(suffix)) {
        labelMap[suffix]->setStyleSheet("background-color: black; color: white;");
    }

    if (lineEditMap.contains(suffix)) {
        QLineEdit* targetEdit = lineEditMap[suffix];
        targetEdit->setEnabled(true);
        targetEdit->setFocus();

        QString value = targetEdit->text();
        ui->leInputValue->setText(value);
        updateConversions(suffix, value);
    }
}

void MainWindow::onInputValueChanged(const QString &text)
{
    if (activeSuffix.isEmpty()) return;
    if (!isValidInput(activeSuffix, text)) return;

    updateConversions(activeSuffix, text);
}

bool MainWindow::isValidInput(const QString &base, const QString &value)
{
    QRegularExpression regex;
    if (base == "Bin") regex.setPattern("^[01]+$");
    else if (base == "Oct") regex.setPattern("^[0-7]+$");
    else if (base == "Deci") regex.setPattern("^[0-9]+$");
    else if (base == "Hexa") regex.setPattern("^[0-9A-Fa-f]+$");
    else return false;

    return regex.match(value).hasMatch();
}

void MainWindow::updateConversions(const QString &base, const QString &value)
{
    bool ok = false;
    int number = 0;

    if (base == "Bin") number = value.toInt(&ok, 2);
    else if (base == "Oct") number = value.toInt(&ok, 8);
    else if (base == "Deci") number = value.toInt(&ok, 10);
    else if (base == "Hexa") number = value.toInt(&ok, 16);

    if (!ok) return;

    lineEditMap["Bin"]->setText(QString::number(number, 2));
    lineEditMap["Oct"]->setText(QString::number(number, 8));
    lineEditMap["Deci"]->setText(QString::number(number, 10));
    lineEditMap["Hexa"]->setText(QString::number(number, 16).toUpper());
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        for (auto it = lineEditMap.begin(); it != lineEditMap.end(); ++it) {
            if (watched == it.value()) {
                QString suffix = it.key();
                QString value = it.value()->text();

                if (!isValidInput(suffix, value)) return false;

                activeSuffix = suffix;
                ui->leInputValue->setText(value);
                updateConversions(suffix, value);

                resetLabelStyles();
                if (labelMap.contains(suffix)) {
                    labelMap[suffix]->setStyleSheet("background-color: black; color: white;");
                }

                return true;
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}
