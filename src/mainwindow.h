#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setExeName(QString str);

private slots:
    void on_getPIDsButton_released();

    void on_browseButton_released();

    void on_injectSelectedButton_released();

    void on_injectAllButton_released();

    void on_browseButton_2_released();

    void closeEvent(QCloseEvent*);
private:
    Ui::MainWindow *ui;
    bool findProcessId(const std::wstring& processName);
    QString lastPath;
};
#endif // MAINWINDOW_H
