#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "processdialog.h"
#include <Windows.h>
#include <tlhelp32.h>
#include <QFileDialog>
#include <QProcessEnvironment>
#include <QMessageBox>
#include <fstream>
#include <QCloseEvent>

void MainWindow::closeEvent(QCloseEvent*) {
    ExitProcess(0);// Process being dumb, too lazy to fix properly.
}

bool MainWindow::findProcessId(const std::wstring& processName) {
    ui->PIDs->clear();
    PROCESSENTRY32 processInfo;
    processInfo.dwSize = sizeof(processInfo);

    HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if(processesSnapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    Process32First(processesSnapshot, &processInfo);
    if(!processName.compare(processInfo.szExeFile)) {
        CloseHandle(processesSnapshot);
        ui->PIDs->addItem(QString::number(processInfo.th32ProcessID));
    }

    while(Process32Next(processesSnapshot, &processInfo)) {
        if(!processName.compare(processInfo.szExeFile)) {
            CloseHandle(processesSnapshot);
            ui->PIDs->addItem(QString::number(processInfo.th32ProcessID));
        }
    }

    CloseHandle(processesSnapshot);
    return ui->PIDs->count() != 0;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lastPath = QProcessEnvironment::systemEnvironment().value("USERPROFILE") + "\\Desktop";

    std::ifstream pathData("lastpath.txt", std::ios::in);
    if(pathData.is_open()) {
        std::string pathLine;
        std::getline(pathData, pathLine);
        if(pathLine.length() > 0) {
            lastPath = QString::fromStdString(pathLine);
        }
    }
    pathData.close();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_getPIDsButton_released()
{
    findProcessId(ui->exeName->text().toStdWString());
}

void MainWindow::on_browseButton_released()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open DLL", lastPath, "Dynamic Link Libraries (*.dll)");
    if(fileName.length() > 0) {
        ui->dllPath->setText(fileName);
        lastPath = fileName.left(fileName.lastIndexOf('/'));
        std::ofstream pathData("lastpath.txt", std::ios::out);
        pathData << lastPath.toStdString();
        pathData.close();
    }
}

void MainWindow::on_injectSelectedButton_released()
{
    if(ui->PIDs->count() == 0 || ui->PIDs->selectedItems().count() == 0) {
        QMessageBox::critical(this, "DLL Injector", "No Process IDs selected!");
    } else if(ui->dllPath->text().length() < 4 || ui->dllPath->text().right(4) != ".dll" || !QFile::exists(ui->dllPath->text())) {
        QMessageBox::critical(this, "DLL Injector", "Invalid DLL name!");
    } else {
        std::string dllPath = ui->dllPath->text().toStdString();

        std::vector<uint32_t> failed{};

        for(auto it : ui->PIDs->selectedItems()) {
            uint32_t PID = it->text().toUInt();

            if(void* client = OpenProcess(PROCESS_ALL_ACCESS, false, PID)) {
                if(void* LoadLibA = (void*)GetProcAddress(GetModuleHandleA((char*)"kernel32.dll"), (char*)"LoadLibraryA")) {
                    if(void* allocString = (void*)VirtualAllocEx(client, NULL, dllPath.length(), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)) {
                        if(WriteProcessMemory(client, (void*)allocString, dllPath.c_str(), dllPath.length(), NULL)) {
                            if(CreateRemoteThread(client, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibA, (void*)allocString, 0, NULL)) {
                                CloseHandle(client);
                                continue;
                            }
                        }
                    }
                }
                CloseHandle(client);
            }

            failed.push_back(PID);
        }

        if(failed.size() == 0) {
            QMessageBox::information(this, "DLL Injector", QString::number(ui->PIDs->count() - failed.size()) + "/" + QString::number(ui->PIDs->count()) + " Successfully injected!");
        } else {
            QString failedStr = "";
            for(uint32_t p : failed) {
                failedStr += "\n" + QString::number(p);
            }
            QMessageBox::warning(this, "DLL Injector", QString::number(ui->PIDs->count() - failed.size()) + "/" + QString::number(ui->PIDs->count()) + " Successfully injected!" + "\n\nFailed:" + failedStr);
        }
    }
}

void MainWindow::on_injectAllButton_released()
{
    findProcessId(ui->exeName->text().toStdWString());
    if(ui->PIDs->count() == 0) {
        QMessageBox::critical(this, "DLL Injector", "Could not find any Process IDs!");
    } else if(ui->dllPath->text().length() < 4 || ui->dllPath->text().right(4) != ".dll" || !QFile::exists(ui->dllPath->text())) {
        QMessageBox::critical(this, "DLL Injector", "Invalid DLL name!");
    } else {
        std::string dllPath = ui->dllPath->text().toStdString();

        std::vector<uint32_t> failed{};

        for(int i = 0; i < ui->PIDs->count(); i++) {
            uint32_t PID = ui->PIDs->item(i)->text().toUInt();

            if(void* client = OpenProcess(PROCESS_ALL_ACCESS, false, PID)) {
                if(void* LoadLibA = (void*)GetProcAddress(GetModuleHandleA((char*)"kernel32.dll"), (char*)"LoadLibraryA")) {
                    if(void* allocString = (void*)VirtualAllocEx(client, NULL, dllPath.length(), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)) {
                        if(WriteProcessMemory(client, (void*)allocString, dllPath.c_str(), dllPath.length(), NULL)) {
                            if(CreateRemoteThread(client, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibA, (void*)allocString, 0, NULL)) {
                                CloseHandle(client);
                                continue;
                            }
                        }
                    }
                }
                CloseHandle(client);
            }

            failed.push_back(PID);
        }

        if(failed.size() == 0) {
            QMessageBox::information(this, "DLL Injector", QString::number(ui->PIDs->count() - failed.size()) + "/" + QString::number(ui->PIDs->count()) + " Successfully injected!");
        } else {
            QString failedStr = "";
            for(uint32_t p : failed) {
                failedStr += "\n" + QString::number(p);
            }
            QMessageBox::warning(this, "DLL Injector", QString::number(ui->PIDs->count() - failed.size()) + "/" + QString::number(ui->PIDs->count()) + " Successfully injected!" + "\n\nFailed:" + failedStr);
        }
    }
}

void MainWindow::on_browseButton_2_released()
{
    ProcessDialog* pd = new ProcessDialog(this);
    pd->setWindowFlags(pd->windowFlags() | Qt::WindowType::Tool | Qt::WindowStaysOnTopHint);
    pd->show();
}

void MainWindow::setExeName(QString str) {
    ui->exeName->setText(str);
}
