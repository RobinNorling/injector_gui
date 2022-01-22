#ifndef PROCESSDIALOG_H
#define PROCESSDIALOG_H

#include <QDialog>

namespace Ui {
class ProcessDialog;
}

class ProcessDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProcessDialog(QWidget *parent = nullptr);
    ~ProcessDialog();

private slots:
    void on_pushButton_released();

    void on_pushButton_2_released();

private:
    Ui::ProcessDialog *ui;
    void findProcesses();
};

#endif // PROCESSDIALOG_H
