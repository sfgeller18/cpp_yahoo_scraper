#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(const QStringList& expirationDates, QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit_2->setDate(QDate::currentDate());

    comboBox = new QComboBox(this);
    comboBox_2 = new QComboBox(this);
    
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(comboBox);
    layout->addWidget(comboBox_2);

    comboBox->addItems({"1m", "2m", "5m", "15m", "30m", "60m", "90m", "1h", "1d", "5d", "1wk", "1mo", "3mo"});
    ui->comboBox_2->addItems(expirationDates);

    comboBox->setGeometry(180, 230, 111, 22);
    comboBox_2->setGeometry(540, 150, 111, 22);


    // Connect line edits to slots
    connect(ui->lineEdit_2, &QLineEdit::textChanged, this, &MainWindow::on_lineEdit_2_textChanged);
    connect(ui->lineEdit_11, &QLineEdit::textChanged, this, &MainWindow::on_lineEdit_11_textChanged);
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onComboBoxIntervalChanged);
    connect(comboBox_2, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onComboBoxExpirationChanged);

    // Connect date edits to slots
    connect(ui->dateEdit, &QDateEdit::dateChanged, this, &MainWindow::on_dateEdit_startDate_dateChanged);
    connect(ui->dateEdit_2, &QDateEdit::dateChanged, this, &MainWindow::on_dateEdit_endDate_dateChanged);

    connect(ui->checkBox_3, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_open_stateChanged);
    connect(ui->checkBox_4, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_high_stateChanged);
    connect(ui->checkBox_5, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_low_stateChanged);
    connect(ui->checkBox_6, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_close_stateChanged);
    connect(ui->checkBox_7, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_adj_stateChanged);
    connect(ui->checkBox_8, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_volume_stateChanged);


    // Connect check boxes to slots
    connect(ui->checkBox, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_calls_stateChanged);
    connect(ui->checkBox_2, &QCheckBox::stateChanged, this, &MainWindow::on_checkBox_puts_stateChanged);
}

MainWindow::~MainWindow() {
    delete ui;
}

// Implement slots to update variables
void MainWindow::on_lineEdit_2_textChanged(const QString &text) {
    ticker = text;
}
void MainWindow::onComboBoxIntervalChanged(const int& index)
{
        interval = comboBox->itemText(index);
}

void MainWindow::onComboBoxExpirationChanged(const int& index)
{
        expirationDate = comboBox->itemText(index);
        qDebug() << "Selected Interval: " << expirationDate;
}

void MainWindow::on_lineEdit_11_textChanged(const QString &text) {
    filePath = text;
}

// Implement new slots
void MainWindow::on_dateEdit_startDate_dateChanged(const QDate &date) {
    endDate = date;
}

void MainWindow::on_dateEdit_endDate_dateChanged(const QDate &date) {
    startDate = date;
}

void MainWindow::on_checkBox_calls_stateChanged(int state) {
    includeCalls = state == Qt::Checked;
}

void MainWindow::on_checkBox_puts_stateChanged(int state) {
    includePuts = state == Qt::Checked;
}

void MainWindow::on_pushButton_clicked() {
    emit buttonPushed();
}

QString MainWindow::getTicker() const {
    return ticker;
}

void MainWindow::setTicker(const QString &value) {
    ticker = value;
}

QString MainWindow::getFilePath() const {
    return filePath;
}

void MainWindow::setFilePath(const QString &value) {
    filePath = value;
}

// Getters and Setters for QDate variables
QDate MainWindow::getStartDate() const {
    return startDate;
}

void MainWindow::setStartDate(const QDate &value) {
    startDate = value;
}

QDate MainWindow::getEndDate() const {
    return endDate;
}

void MainWindow::setEndDate(const QDate &value) {
    endDate = value;
}

QString MainWindow::getExpirationDate() const {
    return expirationDate;
}

void MainWindow::setExpirationDate(const QString &value) {
    expirationDate = value;
}

// Getters and Setters for bool variables
bool MainWindow::getIncludeCalls() const {
    return includeCalls;
}

void MainWindow::setIncludeCalls(bool value) {
    includeCalls = value;
}

bool MainWindow::getIncludePuts() const {
    return includePuts;
}

void MainWindow::setIncludePuts(bool value) {
    includePuts = value;
}

bool MainWindow::includeOpen() const
{
    return stocksInclude[0];
}

void MainWindow::setIncludeOpen(bool value)
{
    stocksInclude[0] = value;
}

bool MainWindow::includeClose() const
{
    return stocksInclude[1];
}

void MainWindow::setIncludeClose(bool value)
{
    stocksInclude[1] = value;
}

bool MainWindow::includeHigh() const
{
    return stocksInclude[2];
}

void MainWindow::setIncludeHigh(bool value)
{
    stocksInclude[2] = value;
}

bool MainWindow::includeLow() const
{
    return stocksInclude[3];
}

void MainWindow::setIncludeLow(bool value)
{
    stocksInclude[3] = value;
}

bool MainWindow::includeVolume() const
{
    return stocksInclude[4];
}

void MainWindow::setIncludeVolume(bool value)
{
    stocksInclude[4] = value;
}

bool MainWindow::includeAdj() const
{
    return stocksInclude[5];
}

void MainWindow::setIncludeAdj(bool value)
{
    stocksInclude[5] = value;
}

void MainWindow::on_checkBox_open_stateChanged(int state)
{
    setIncludeOpen(state == Qt::Checked);
}

void MainWindow::on_checkBox_close_stateChanged(int state)
{
    setIncludeClose(state == Qt::Checked);
}

void MainWindow::on_checkBox_high_stateChanged(int state)
{
    setIncludeHigh(state == Qt::Checked);
}

void MainWindow::on_checkBox_low_stateChanged(int state)
{
    setIncludeLow(state == Qt::Checked);
}

void MainWindow::on_checkBox_volume_stateChanged(int state)
{
    setIncludeVolume(state == Qt::Checked);
}

void MainWindow::on_checkBox_adj_stateChanged(int state)
{
    setIncludeAdj(state == Qt::Checked);
}

std::string MainWindow::getInterval() const {
    return interval.toStdString();
}
