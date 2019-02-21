#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <string>
#include<bits/stdc++.h>
#include "nfamanager.h"
#include <QDir>
#include <QFileInfo>
#include<QCoreApplication>
#include<QProcess>
#include<QString>
#include<QDebug>
#include <QPixmap>
#include <vector>
#include <QFileDialog>
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

}

MainWindow::~MainWindow()
{
    delete ui;
}

int chars(string s)
{
    int n=0;
    for(int i=0;i<s.size();i++)
    {
        if((s[i]>=48 && s[i]<=57)||(s[i]>=65 && s[i]<=90)
                ||(s[i]>=97 && s[i]<=122))
            n++;
    }
    return n;
}

void MainWindow::on_Button1_clicked()
{
    ui->label_3->clear();
    ui->textBrowser->clear();
    nfaManager NFA;
    string zzz;
    QString text = ui->textEdit->toPlainText();
    string s = text.toStdString();
    //将正则表达式转化为nfa
    s = NFA.insert_concat(s);
    s = NFA.regexp_to_postfix(s);
    NFA.postfix_to_nfa(s);
    //将nfa转化为dfa
    set<int> si;
    NFA.nfa_to_dfa(si);
    //最小化dfa
    NFA.minimize_dfa();

    QString z = QCoreApplication::applicationDirPath();
    zzz = z.toStdString();

    NFA.show_nfa(zzz);
    NFA.show_dfa(zzz);
    NFA.show_mini_dfa(zzz);
    NFA.show_code(zzz);

}

void MainWindow::on_Button2_clicked()
{
    QString z = QCoreApplication::applicationDirPath();
    z+="/graph/nfa.txt";
    QString m = QCoreApplication::applicationDirPath();
    m+= "/graph/dot.exe";
    qDebug()<<m;
    //创建进程执行命令，根据dot.txt文件画出png图并保存在其目录下
    QStringList arguments;
    arguments << z<<"-Tpng"<<"-o"<<"nfa.png";
    qDebug()<<arguments;
    QProcess *pro = new QProcess;
    int flag =pro->execute(m, arguments);
    //将生成的nfa图展示出来
    QPixmap image;
    image.load("nfa.png");
    ui->label_3->clear();
    ui->label_3->setPixmap(image);
    ui->label_3->show();

}

void MainWindow::on_Button3_clicked()
{
    QString z = QCoreApplication::applicationDirPath();
    z+="/graph/dfa.txt";
    QString m = QCoreApplication::applicationDirPath();
    m+= "/graph/dot.exe";
    QStringList arguments;
    arguments << z<<"-Tpng"<<"-o"<<"dfa.png";
    QProcess *pro = new QProcess;
    int flag =pro->execute(m, arguments);
    //将生成的dfa图展示出来
    QPixmap image;
    image.load("dfa.png");
    ui->label_3->clear();
    ui->label_3->setPixmap(image);
    ui->label_3->show();

}

void MainWindow::on_Button4_clicked()
{
    QString z= QCoreApplication::applicationDirPath();
    z+="/graph/mini_dfa.txt";
    QString m = QCoreApplication::applicationDirPath();
    m+= "/graph/dot.exe";
    QStringList arguments;
    arguments << z<<"-Tpng"<<"-o"<<"mini_dfa.png";
    QProcess *pro = new QProcess;
    //int flag =pro->execute(m, arguments);
    int flag =pro->execute(m, arguments);
    //将生成的dfa图展示出来
    QPixmap image;
    image.load("mini_dfa.png");
    ui->label_3->clear();
    ui->label_3->setPixmap(image);
    ui->label_3->show();
}

void MainWindow::on_Button4_2_clicked()
{
    ifstream in("Ccode.txt");
    string content;
    ui->textBrowser->clear();
    while (getline (in, content))
    {
        QString  content_1=QString::fromStdString(content);
        ui->textBrowser->append(content_1);
    }

}

void MainWindow::on_pushButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(
                this, tr("open image file"),
                "./", tr("Txt files(*.txt);;All files (*.*)"));
    string filepath = file.toStdString();
    cout<<filepath<<endl;
    string content;
    ifstream in(filepath);
    if(in)
    {
        ui->textEdit->clear();
        while (getline (in, content))
        {
            QString  content_1=QString::fromStdString(content);
            ui->textEdit->append(content_1);
        }
    }
    else{
        cout<<"error";
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    ofstream out("re.txt");
    QString text = ui->textEdit->toPlainText();
    string s = text.toStdString();
    out<<s;
    out.close();
}
