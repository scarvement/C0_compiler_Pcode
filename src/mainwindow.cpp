#include "include/mainwindow.h"
#include "ui_mainwindow.h"
#include "QFileDialog"
#include <QtDebug>
#include "QStandardItem"
#include "QMessageBox"
#include <fstream>
#include<iostream>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    tk = NULL;
    OPGanalysis = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionFile_triggered()
{
    QStringList fileNameList;
    QFileDialog* fd = new QFileDialog(this);//创建对话框
    fd->resize(240, 320);    //设置显示的大小
    fd->setNameFilter(tr("Allfile(*.*)")); //设置文件过滤器
    fd->setViewMode(QFileDialog::List);  //设置浏览模式，有 列表（list） 模式和 详细信息（detail）两种方式
    if(fd->exec() == QDialog::Accepted) {  //如果成功的执行
        fileNameList = fd->selectedFiles();      //返回文件列表的名称
        SourcefilePath = fileNameList[0];            //取第一个文件名
        qDebug() << SourcefilePath;
        srcfile.setFileName(SourcefilePath);
        if(!srcfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qDebug()<<"Can't open the file!"<<endl;
        }
        QByteArray alltextbyte = srcfile.readAll();
        allcode = QString(alltextbyte);
        ui->textEdit->setPlainText(allcode);
        srcfile.close();
    }
    else
        fd->close();
}

void MainWindow::on_pushButton_clicked()
{
    freopen("Logger.txt", "w", stdout);
    QStandardItemModel* model = new QStandardItemModel();
    QStringList labels = QObject::trUtf8("单词,类别,值").simplified().split(",");
    model->setHorizontalHeaderLabels(labels);

    ui->tableView->setModel(model);
    ui->tableView->show();

    std::string file_path = "temp_saved_code.out";
    std::string src_str = ui->textEdit->toPlainText().toStdString();

    std::ofstream fout(file_path);
    fout << src_str;
    fout.close();
    if(tk != NULL) {
        delete tk;
    }
    tk = new Token(file_path);
    tk->analyse();
    QStandardItem* item;
    int cnt = 0;
    for(const st_token &v : tk->result) {
        item = new QStandardItem(QString("%1").arg(v.str_value.c_str()));
        model->setItem(cnt, 0, item);
        item = new QStandardItem(QString("%1").arg(v.type.c_str()));
        model->setItem(cnt, 1, item);
        if(v.str_bin.length() > 0) {
            item = new QStandardItem(QString("%1").arg((v.str_bin + "(二进制)").c_str()));
        } else if(v.type != "Token error!"){
            item = new QStandardItem(QString("%1").arg(v.str_value.c_str()));
        } else {
            item = new QStandardItem(QString("%1").arg(v.type.c_str()));
        }
        model->setItem(cnt, 2, item);
        cnt++;
    }

    // tk->test();
    // puts("test!");
}

void MainWindow::on_pushButton_2_clicked()
{
    // std::ofstream fout("OPG.txt");
    freopen("OPG.txt", "w", stdout);

    if(OPGanalysis != NULL) {
        delete OPGanalysis;
    }
    OPGanalysis = new OPG();

#define fail(x) {std::cout << x << std::endl;}

    string OPGgrammar = ui->textEdit->toPlainText().toStdString();
    if(!OPGanalysis->setGrammar(OPGgrammar)) {
        fail(string("Grammar error."));
        return;
    }
    string display = OPGgrammar + "\n\n", blank = "";
    for(string &str : OPGanalysis->grammar) {
        display += str + "\n";
    }
    if(!OPGanalysis->op_init()) {
        fail(string("Grammar error on generating operator table!"));
        return;
    }
    const auto &Vt = OPGanalysis->Vt;
    const auto &Vn = OPGanalysis->Vn;
    display += "\nFIRSTVT: \n";
    for(size_t i = 0; i < OPGanalysis->fVT.size(); ++i) {
        display += blank + Vn[i] + ":   ";
        for(auto &c : OPGanalysis->fVT[i])
            display += blank + c + "   ";
        display += "\n";
    }
    display += "\nLASTVT: \n";
    for(size_t i = 0; i < OPGanalysis->lVT.size(); ++i) {
        display += blank + Vn[i] + ":   ";
        for(auto &c : OPGanalysis->lVT[i])
            display += blank + c + "   ";
        display += "\n";
    }
    display += "\n算符优先矩阵:\n     ";
    for(auto c : Vt) {
        display += blank + " " + c + " ";
    }
    display += " #  \n";
    for(size_t i = 0; i <= Vt.size(); ++i) {
        display += blank + "\n  " + (i == Vt.size() ? '#' : Vt[i]) + "  ";
        for(size_t j = 0; j <= Vt.size(); ++j) {
            char c = OPGanalysis->op[i][j];
            display += blank + " " + (c ? c : '/') + " ";
        }
        display += "\n";
    }
    display += "\n";
    ui->textEdit->setPlainText(QString::fromStdString(display));

/*

*/
}

void MainWindow::on_pushButton_3_clicked() {
    if(OPGanalysis == NULL || !OPGanalysis->GrammarExist()) {
        fail("Grammar doesn't exist.");
        return;
    }
    string sentence = ui->textEdit->toPlainText().toStdString();
    if(!OPGanalysis->analyse(sentence)) {
        fail("Analysis failed.");
    }
    QStandardItemModel* model = new QStandardItemModel();
    QStringList labels = QObject::trUtf8("步骤,分析栈,关系标志,当前输入符,剩余输入串").simplified().split(",");
    model->setHorizontalHeaderLabels(labels);

    ui->tableView->setModel(model);
    ui->tableView->show();
    QStandardItem* item;

    int cnt = 0;
    for(const st_OPG_result &v : OPGanalysis->result) {
        for(int i = 0 ; i < 5; ++i) {
            item = new QStandardItem(QString("%1").arg(v.a[i].c_str()));
            model->setItem(cnt, i, item);
        }
        cnt++;
    }
}
