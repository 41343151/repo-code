#include "dialog.h"

#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QFontDialog>
#include <QInputDialog>
#include <QColorDialog>
#include <QProgressDialog>
#include <QTimer>
#include <QPageSetupDialog>
#include <QPrintDialog>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QFile>
#include <QTextStream>
#include <QPalette>

Dialog::Dialog(QWidget *parent) : QDialog(parent)
{
    setWindowTitle(QString::fromUtf8("內建對話盒展示（程式版）"));
    buildUi();
    connectSignals();
}

void Dialog::buildUi()
{
    auto *root = new QHBoxLayout(this);

    // 左側按鈕區（Grid）
    auto *panel = new QWidget(this);
    auto *grid  = new QGridLayout(panel);

    btnColor      = new QPushButton(u8"顏色對話盒", this);
    btnError      = new QPushButton(u8"錯誤訊息盒", this);
    btnFile       = new QPushButton(u8"檔案對話盒", this);
    btnFont       = new QPushButton(u8"字體對話盒", this);
    btnInput      = new QPushButton(u8"輸入對話盒", this);
    btnPageSetup  = new QPushButton(u8"頁面設定對話盒", this);
    btnProgress   = new QPushButton(u8"進度對話盒", this);
    btnPrint      = new QPushButton(u8"列印對話盒", this);
    btnTextColor  = new QPushButton(u8"更改文字顏色", this); // ★ 新增

    // 排成 3×3（最後一格放新按鈕）
    grid->addWidget(btnColor,     0,0);
    grid->addWidget(btnError,     0,1);
    grid->addWidget(btnFile,      0,2);
    grid->addWidget(btnFont,      1,0);
    grid->addWidget(btnInput,     1,1);
    grid->addWidget(btnPageSetup, 1,2);
    grid->addWidget(btnProgress,  2,0);
    grid->addWidget(btnPrint,     2,1);
    grid->addWidget(btnTextColor, 2,2);

    // 右側文字編輯器
    editor = new QTextEdit(this);
    editor->setPlaceholderText(u8"在這裡輸入或開啟文字，測試字體 / 顏色 / 列印…");

    root->addWidget(panel, 0);
    root->addWidget(editor, 1);
    setLayout(root);
}

void Dialog::connectSignals()
{
    connect(btnColor,     &QPushButton::clicked, this, &Dialog::onColor);
    connect(btnError,     &QPushButton::clicked, this, &Dialog::onError);
    connect(btnFile,      &QPushButton::clicked, this, &Dialog::onFile);
    connect(btnFont,      &QPushButton::clicked, this, &Dialog::onFont);
    connect(btnInput,     &QPushButton::clicked, this, &Dialog::onInput);
    connect(btnPageSetup, &QPushButton::clicked, this, &Dialog::onPageSetup);
    connect(btnProgress,  &QPushButton::clicked, this, &Dialog::onProgress);
    connect(btnPrint,     &QPushButton::clicked, this, &Dialog::onPrint);
    connect(btnTextColor, &QPushButton::clicked, this, &Dialog::onTextColor);
}

/* 1) 顏色對話盒（改背景色） */
void Dialog::onColor()
{
    const QColor c = QColorDialog::getColor(Qt::white, this, tr("設定背景顏色"));
    if (!c.isValid()) return;
    QPalette pal = editor->palette();
    pal.setColor(QPalette::Base, c);
    editor->setPalette(pal);
}

/* 2) 錯誤訊息盒 */
void Dialog::onError()
{
    QMessageBox::critical(this, tr("錯誤"), tr("這是一個示範用的錯誤訊息。"));
}

/* 3) 檔案對話盒（讀檔） */
void Dialog::onFile()
{
    const QString path = QFileDialog::getOpenFileName(
        this, tr("開啟文字檔"), QString(),
        tr("文字檔 (*.txt *.log *.md *.cpp *.h);;所有檔案 (*.*)"));
    if (path.isEmpty()) return;

    QFile f(path);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, tr("開啟失敗"), tr("無法讀取：\n%1").arg(path));
        return;
    }
    QTextStream ts(&f);
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    ts.setEncoding(QStringConverter::Utf8);
#else
    ts.setCodec("UTF-8");
#endif
    editor->setPlainText(ts.readAll());
}

/* 4) 字體對話盒 */
void Dialog::onFont()
{
    bool ok=false;
    QFont font = QFontDialog::getFont(&ok, editor->font(), this, tr("選擇字體"));
    if (ok) editor->setFont(font);
}

/* 5) 輸入對話盒 */
void Dialog::onInput()
{
    bool ok=false;
    const QString text = QInputDialog::getText(this, tr("輸入對話盒"),
                                               tr("請輸入要插入的文字："),
                                               QLineEdit::Normal, QString(), &ok);
    if (ok && !text.isEmpty()) editor->insertPlainText(text + "\n");
}

/* 6) 頁面設定 */
void Dialog::onPageSetup()
{
    QPageSetupDialog dlg(&printer, this);
    if (dlg.exec() == QDialog::Accepted)
        QMessageBox::information(this, tr("頁面設定"), tr("頁面設定已更新。"));
}

/* 7) 進度對話盒（模擬） */
void Dialog::onProgress()
{
    QProgressDialog progress(tr("正在處理…"), tr("取消"), 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);

    int value = 0;
    QTimer timer; timer.setInterval(30);
    connect(&timer, &QTimer::timeout, this, [&](){
        value += 2;
        progress.setValue(value);
        if (progress.wasCanceled() || value >= 100) timer.stop();
    });
    timer.start();
    progress.exec();
}

/* 8) 列印 */
void Dialog::onPrint()
{
    QPrintDialog dlg(&printer, this);
    dlg.setWindowTitle(tr("列印"));
    if (dlg.exec() == QDialog::Accepted)
        editor->print(&printer);
}

/* 9) ★ 更改文字顏色（選取或之後打字） */
void Dialog::onTextColor()
{
    QColor initial = editor->currentCharFormat().foreground().color();
    if (!initial.isValid()) initial = Qt::black;

    const QColor chosen = QColorDialog::getColor(initial, this, tr("選擇文字顏色"));
    if (!chosen.isValid()) return;

    QTextCursor cur = editor->textCursor();
    QTextCharFormat fmt; fmt.setForeground(chosen);

    if (cur.hasSelection()) {
        cur.mergeCharFormat(fmt);
    } else {
        auto f = editor->currentCharFormat();
        f.merge(fmt);
        editor->setCurrentCharFormat(f);
    }
}
