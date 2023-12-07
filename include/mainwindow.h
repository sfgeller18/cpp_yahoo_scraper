#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QLineEdit>
#include <QDateEdit>
#include <QCheckBox>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QStringList& expirationDates, QWidget *parent = nullptr);
    ~MainWindow();
    // Getters and Setters for QString variables
QString getTicker() const;
void setTicker(const QString &value);

QString getFilePath() const;
void setFilePath(const QString &value);

// Getters and Setters for QDate variables
QDate getStartDate() const;
void setStartDate(const QDate &value);

QDate getEndDate() const;
void setEndDate(const QDate &value);

QString getExpirationDate() const;
void setExpirationDate(const QString &value);

// Getters and Setters for bool variables
bool getIncludeCalls() const;
void setIncludeCalls(bool value);

bool getIncludePuts() const;
void setIncludePuts(bool value);

// Getter and Setter for the button pushed flag
bool getButtonPushed() const;
void setButtonPushed(bool value);

    bool includeOpen() const;
    void setIncludeOpen(bool value);

    bool includeClose() const;
    void setIncludeClose(bool value);

    bool includeHigh() const;
    void setIncludeHigh(bool value);

    bool includeLow() const;
    void setIncludeLow(bool value);

    bool includeVolume() const;
    void setIncludeVolume(bool value);

    bool includeAdj() const;
    void setIncludeAdj(bool value);

    std::string getInterval() const;

signals:
    void buttonPushed();

private slots:
    // Add slots for line edits
    void on_lineEdit_2_textChanged(const QString &text);
    void on_lineEdit_11_textChanged(const QString &text);
    
    void onComboBoxIntervalChanged(const int& index);
    void onComboBoxExpirationChanged(const int& index);


        // Add slots for date edits
    void on_dateEdit_startDate_dateChanged(const QDate &date);
    void on_dateEdit_endDate_dateChanged(const QDate &date);

    // Add slots for check boxes
    void on_checkBox_calls_stateChanged(int state);
    void on_checkBox_puts_stateChanged(int state);

    void on_checkBox_open_stateChanged(int state);
    void on_checkBox_close_stateChanged(int state);
    void on_checkBox_high_stateChanged(int state);
    void on_checkBox_low_stateChanged(int state);
    void on_checkBox_volume_stateChanged(int state);
    void on_checkBox_adj_stateChanged(int state);

    void on_pushButton_clicked(); 
    


private:
    Ui::MainWindow *ui;

    // Add QString variables for line edits
    QString ticker;       // Assuming lineEdit_2 is the Ticker      // Assuming lineEdit_7 is the End Date
    QString filePath;     // Assuming lineEdit_11 is the File Path

    QString interval = "1d";
    QComboBox *comboBox;

    QString expirationDate;
    QComboBox *comboBox_2;

        // Define variables for date edit
    QDate startDate = QDate::currentDate();
    QDate endDate = QDate::currentDate();

    // Define variables for check boxes
    bool includeCalls;
    bool includePuts;

    std::vector<bool> stocksInclude = {false, false, false, false, false, false};

};

#endif // MAINWINDOW_H
