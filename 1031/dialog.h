#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QPrinter>

class QTextEdit;
class QPushButton;

class Dialog : public QDialog
{
    Q_OBJECT
public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog() override = default;

private slots:
    void onColor();        // 顏色對話盒（背景色）
    void onError();        // 錯誤訊息盒
    void onFile();         // 檔案對話盒（讀檔）
    void onFont();         // 字體對話盒
    void onInput();        // 輸入對話盒
    void onPageSetup();    // 頁面設定
    void onProgress();     // 進度對話盒
    void onPrint();        // 列印
    void onTextColor();    // ★ 更改文字顏色（新增）

private:
    void buildUi();
    void connectSignals();

    QTextEdit*  editor = nullptr;   // 右側編輯器
    QPrinter    printer{QPrinter::HighResolution};

    // 左側按鈕
    QPushButton *btnColor=nullptr, *btnError=nullptr, *btnFile=nullptr, *btnFont=nullptr;
    QPushButton *btnInput=nullptr, *btnPageSetup=nullptr, *btnProgress=nullptr, *btnPrint=nullptr;
    QPushButton *btnTextColor=nullptr;
};

#endif // DIALOG_H
