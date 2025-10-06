#include <QApplication>
#include "widget.h"

/*编译环境Qt5.12.2 MinGw32bit*/
/*00 00 00 00 00 00 01 03 04 01 0F 03 14*/
/*换算温度：010F，16进制转10进制*/
/*换算湿度：0314，16进制转10进制*/
/*9 10,11 12*/

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication a(argc, argv);

    /*加载资源字体*/
    QFontDatabase::addApplicationFont(":/fonts/resources/fonts/digifaw.ttf");

    /*因调用setAttribute(Qt::WA_DeleteOnClose),此处必须new在堆上,否则将会触发两次析构*/

    Widget *w = new Widget;
    w->show();
    return a.exec();
}
