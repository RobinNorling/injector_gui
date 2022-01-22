#include "mainwindow.h"
#include "processdialog.h"
#include "ui_processdialog.h"
#include <Windows.h>
#include <tlhelp32.h>
#include <QMessageBox>

void ProcessDialog::findProcesses() {
    ui->PIDs->clear();
    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(processInfo);

    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if(processesSnapshot == INVALID_HANDLE_VALUE) {
        return;
    }

    Process32First(processesSnapshot, &processInfo);
    do {
        QString exeName = QString::fromWCharArray(processInfo.szExeFile, 260).simplified();
        int whereEnd = exeName.indexOf(".exe", Qt::CaseSensitivity::CaseInsensitive);
        if(whereEnd == -1) {
            continue;
        }

        exeName = exeName.left(whereEnd + 4);


        if(ui->PIDs->findItems(exeName, Qt::MatchFlag::MatchExactly).count() == 0)
            ui->PIDs->addItem(exeName);
    } while(Process32Next(processesSnapshot, &processInfo));

    CloseHandle(processesSnapshot);
    ui->PIDs->sortItems();
}

ProcessDialog::ProcessDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProcessDialog)
{
    ui->setupUi(this);
    findProcesses();
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
}

ProcessDialog::~ProcessDialog()
{
    delete ui;
}

void ProcessDialog::on_pushButton_released()
{
    if(ui->PIDs->selectedItems().count() == 0) {
        QMessageBox::critical(this, "DLL Injector", "Nothing selected!");
    } else {
        ((MainWindow*)parent())->setExeName(ui->PIDs->selectedItems().at(0)->text());
        close();
    }
}

void ProcessDialog::on_pushButton_2_released()
{
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    findProcesses();
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
}
